"""R151 -> R157: Follow is back on the retail row (LB / L1 only) in the companion profile and BasicAIActions.lua is the pre-R151
file again (the X Quick talk prompt is listed next to Follow like retail, no kcd2vr_follow_lb_press alias). Follow on X is mod-side
only (ride_follow_r147.h: X tap -> one LB tap while the Follow row is listed). Every other profile action is unchanged
(the R153/R156 hold-to-look rows are tests/test_lb_prompt_x_r153.py)."""
from pathlib import Path
import re,subprocess,unittest,xml.etree.ElementTree as E
ROOT=Path(__file__).resolve().parents[1]
PROFILE=ROOT/'companion/source/Libs/Config/defaultProfile.xml'
LUA=ROOT/'companion/source/Scripts/Entities/AI/Shared/BasicAIActions.lua'
BASE='f252355'   # R150 = pre-R151 companion (profile and Lua)
def inputs(a,dev):
    return ([a.get(dev)] if a.get(dev) else [])+[i.get('input') for i in a.findall('./%s/inputdata'%dev)]
def actions(root):
    return {(m.get('name'),a.get('name')):a for m in root.iter('actionmap') for a in m.findall('action')}
def base(path):
    r=subprocess.run(['git','-C',str(ROOT),'show',BASE+':'+path],capture_output=True)
    return None if r.returncode else r.stdout
class FollowX151(unittest.TestCase):
    def setUp(self):
        self.root=E.fromstring(PROFILE.read_bytes());self.acts=actions(self.root)
    def test_follow_binding_is_retail(self):
        f=self.acts[('focus_follow_init_interactor','focus_follow_init_interactor_press')]
        self.assertEqual(inputs(f,'xboxpad'),['xi_shoulderl'])   # R157: LB only again (the mod sends LB for the X tap)
        self.assertEqual(inputs(f,'pspad'),['pad_l1'])
        self.assertEqual(f.get('onPress'),'1');self.assertEqual(f.get('keyboard'),'_keybinds_ref_')
        self.assertNotIn(('focus_follow_init_interactor','kcd2vr_follow_lb_press'),self.acts)   # R151 alias gone
        self.assertEqual([a.get('name') for a in self.root.iter('actionmap') if a.get('name')=='focus_follow_init_interactor' for a in a.findall('action')],
                         ['focus_follow_init_interactor_press'])
    def test_mod_side_follow_still_possible(self):
        # ride_follow_r147.h turns an X tap into LB while the follow row is listed: LB must stay bound to Follow, and
        # LB must not carry Quick Speak (otherwise the LB tap would talk instead of follow)
        f=self.acts[('focus_follow_init_interactor','focus_follow_init_interactor_press')]
        self.assertIn('xi_shoulderl',inputs(f,'xboxpad'))
        for k in [('chat_init','chat_init_press'),('chat_init_interactor','chat_init_interactor_press'),('chat_init_interactor','chat_init_with_focus')]:
            self.assertNotIn('xi_shoulderl',inputs(self.acts[k],'xboxpad'),k)
        # the hint-row matcher keys on this exact action name (native_input_look_lab.cpp ReadFollowHint147)
        lab=(ROOT/'src/subsystems/hud_ui_controls/runtime/native_input_look_lab.cpp').read_text(encoding='utf-8',errors='replace')
        self.assertIn('static const char kName[]="focus_follow_init_interactor_press";',lab)
    def test_other_actions_unchanged(self):
        old_bytes=base('companion/source/Libs/Config/defaultProfile.xml')
        if old_bytes is None:self.skipTest('base commit not in this checkout')
        old=actions(E.fromstring(old_bytes));new=self.acts
        changed={# R153/R156: hold to look / watch on X first (tests/test_lb_prompt_x_r153.py)
                 ('infinite_hold_interaction_init','infinite_hold_interaction_init_press'),
                 ('infinite_hold_interaction_inprogress','infinite_hold_interaction_inprogress_release')}
        self.assertEqual(set(new),set(old))   # R157: no added or removed action (kcd2vr_follow_lb_press is gone)
        for k,a in old.items():
            if k not in changed:self.assertEqual(E.tostring(new[k]),E.tostring(a),k)
        # the proven ones stay: quick talk X, kick LB, reading pages, menu tabs
        self.assertEqual(inputs(new[('chat_init_interactor','chat_init_with_focus')],'xboxpad'),['xi_x'])
        self.assertEqual(inputs(new[('combat','attack_special')],'xboxpad'),['xi_shoulderl'])
        self.assertEqual(inputs(new[('reading_base','read_prev')],'xboxpad'),['xi_shoulderl'])
        self.assertEqual(inputs(new[('apse_tabs','tab_next')],'xboxpad'),['xi_shoulderr'])
    def test_lua_is_pre_r151(self):
        s=LUA.read_text()
        self.assertNotIn('kcd2vr_follow_lb_press',s)
        self.assertNotIn('if not canFollow then',s)
        # retail shape: Follow row, then the X Quick talk row (chat_init_with_focus) whenever CanChat, plus the hidden pointing alias
        self.assertRegex(s,r'if canFollow then\n\t\tlocal followHint = "@ui_hud_follow_start"\n\t\tAddInteractorAction\(output, false, Action\(\):hint\(followHint\):action\("focus_follow_init_interactor_press"\)')
        self.assertRegex(s,r'if showInitHint then\n\t\tchatHint = "@ui_hud_chat_init"\n(\t\t--[^\n]*\n)*\t\tAddInteractorAction\(output, false, Action\(\):hint\(chatHint\):action\("chat_init_with_focus"\)')
        self.assertEqual(s.count('action("kcd2vr_chat_init_gesture")'),1)   # pointing quick talk always offered
        body=s[s.index('function BasicAIActions:GetChatActions'):s.index('function BasicAIActions:OnTalk')]
        self.assertEqual(len(re.findall(r'\b(if|function)\b',body)),len(re.findall(r'\bend\b',body)))  # blocks closed
        old=base('companion/source/Scripts/Entities/AI/Shared/BasicAIActions.lua')
        if old is None:self.skipTest('base commit not in this checkout')
        self.assertEqual(LUA.read_bytes(),old)   # byte-identical to the R150 file
if __name__=='__main__':unittest.main()
