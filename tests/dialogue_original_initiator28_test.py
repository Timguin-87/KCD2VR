"""Actual retail-XML preservation and scoped request fixture; not native BT execution."""
from pathlib import Path
import sys
import unittest
import zipfile
import xml.etree.ElementTree as ET
try:
    from lupa import LuaRuntime
except ImportError:
    LuaRuntime = None
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import build_dialogue_original_initiator28 as proposal

class OriginalInitiator(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        with zipfile.ZipFile('C:/Steam/steamapps/common/KingdomComeDeliverance2/Data/Scripts.pak') as pak:
            cls.retail = pak.read('AI/speech/dialoginitiator_inner.xml')
            cls.polylog = ET.fromstring(pak.read('AI/speech/polyloginitiator.xml'))
            cls.init = ET.fromstring(pak.read('AI/speech/polyloginitiator_init.xml'))
            cls.range = ET.fromstring(pak.read('AI/quests/lukostreleckaSoutez/shootingRange.xml'))
        cls.result = proposal.build(cls.retail)
        cls.original = ET.fromstring(cls.retail)
        cls.patched = ET.fromstring(cls.result)

    def test_exact_input_only(self):
        with self.assertRaises(ValueError): proposal.build(self.retail + b' ')

    def test_ignored_preset_bypasses_existing_fix(self):
        branch = next(n for n in self.polylog.iter('IfElseCondition') if n.get('condition') == '$preset == $enum:dialogPreset.ignored')
        self.assertEqual(branch.find('Then/Expression').get('expressions'), '$dialogParameters = $customParameters')
        self.assertIsNone(branch.find('Then/Function_speech_getParametersFromPreset'))
        self.assertIsNotNone(branch.find('Else/Function_speech_getParametersFromPreset'))

    def test_native_self_is_initiator(self):
        self.assertTrue(any(n.get('expressions', '').startswith('$thisToMetaroles.soul = $this.id') for n in self.init.iter('Expression')))
        conditional = next(n for n in self.init.iter('IfElseCondition') if n.get('condition') == '$soulToMetaroles.soul == $this.id')
        self.assertEqual(conditional.find('Then/PushToArray').get('direction'), 'Front')
        self.assertTrue(any(n.get('condition') == '$this.id == $__player' for n in self.range.iter('IfCondition')))

    def test_native_request_arguments_preserved(self):
        original = self.original.find('BehaviorTree/Root/Behavior/FuseBox/Child/Sequence/RequestDialog')
        request = self.patched.find('BehaviorTree/Root/Behavior/FuseBox/Child/Sequence/RequestDialog')
        attrs = dict(request.attrib)
        attrs['UseTwins'] = '$dialogParameters.useTwinsAsInitiator'
        attrs['AllowPositioning'] = '$dialogParameters.movePlayer'
        self.assertEqual(attrs, original.attrib)
        self.assertEqual(len(list(self.patched.iter('RequestDialog'))), len(list(self.original.iter('RequestDialog'))))

    def test_guard_and_original_values(self):
        sequence = self.patched.find('BehaviorTree/Root/Behavior/FuseBox/Child/Sequence')
        self.assertEqual([n.tag for n in sequence], ['Expression','Expression','ExecuteLua','IfCondition','RequestDialog','DoDialog'])
        self.assertEqual(sequence[1].get('expressions'), proposal.COPY)
        self.assertEqual(sequence[2].get('code'), proposal.LUA)
        self.assertEqual(sequence[3].get('condition'), proposal.CONDITION)
        self.assertEqual(sequence[3][0].get('expressions'), proposal.OVERRIDE)
        self.assertEqual(sequence[3].get('failOnCondition'), 'false')

    @unittest.skipIf(LuaRuntime is None, 'Lua fixture dependency unavailable')
    def test_execute_generated_lua_and_guard(self):
        sequence = self.patched.find('BehaviorTree/Root/Behavior/FuseBox/Child/Sequence')
        # Native BT expression uses $variable and boolean &, Lua uses data.variable
        # and `and`. Execute the actual generated code, not a mirrored predicate.
        def expression(text): return text.replace('$','data.').replace('&',' and ')
        for no_camera in (0,1,2):
            for force_pov in (0,1,2):
                for local_initiator in (False,True):
                    for twins in (False,True):
                        lua = LuaRuntime()
                        lua.execute('data={dialogParameters={},this={},disableRotations=false}; System={}')
                        data = lua.globals().data
                        data.dialogParameters.useTwinsAsInitiator = twins
                        data.dialogParameters.movePlayer = True
                        data.this.id = 123 if local_initiator else 456
                        data['__player'] = 123
                        def get_cvar(name): return no_camera if name=='wh_dlg_NoCameras' else force_pov
                        lua.globals().System.GetCVar = get_cvar
                        lua.execute(expression(sequence[1].get('expressions')))
                        lua.execute(sequence[2].get('code'))
                        accepted = lua.eval(expression(sequence[3].get('condition')))
                        if accepted: lua.execute(expression(sequence[3][0].get('expressions')))
                        wanted = no_camera==1 and force_pov==1 and local_initiator
                        self.assertEqual(accepted,wanted)
                        self.assertEqual(data.kcd2vrUseTwins28,False if wanted else twins)
                        self.assertEqual(data.kcd2vrMovePlayer28,not wanted)
                        self.assertEqual(data.disableRotations,wanted)

    def test_remove_additions_recovers_entire_retail_tree(self):
        tree = ET.fromstring(self.result)
        bt = tree.find('BehaviorTree')
        for path in ('Variables','EditorData/Variables'):
            parent = bt.find(path)
            for child in list(parent):
                if child.get('name') in proposal.VARIABLES: parent.remove(child)
        for path in ('Root/Behavior/FuseBox/Child/Sequence','EditorData/Root/Behavior/FuseBox/Child/Sequence'):
            parent = bt.find(path)
            at = list(parent).index(parent.find('RequestDialog'))
            for child in list(parent)[at-3:at]: parent.remove(child)
        request = bt.find('Root/Behavior/FuseBox/Child/Sequence/RequestDialog')
        request.set('UseTwins','$dialogParameters.useTwinsAsInitiator')
        request.set('AllowPositioning','$dialogParameters.movePlayer')
        self.assertEqual(proposal.normalized(tree),proposal.normalized(self.original))

if __name__ == '__main__': unittest.main()
