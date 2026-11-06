import unittest, importlib.util, xml.etree.ElementTree as ET, tempfile, zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('activity_builder',ROOT/'tools/build_activity_dialogue47c1.py')
builder=importlib.util.module_from_spec(spec);spec.loader.exec_module(builder)

class ActivityDialogue(unittest.TestCase):
 def test_stock_tree_preserved_before_conditional_override(self):
  # The fixture models the retail preset switch and a separate untouched tree.
  data=b'<BehaviorTrees><BehaviorTree name="speech_getParametersFromPreset"><Variables/><Root><Behavior><Switch variable="$preset"><Case value="fader"><Expression expressions="$dialogParameters.enableEnding = false"/></Case></Switch></Behavior></Root></BehaviorTree><BehaviorTree name="unrelated"><Root><Behavior><Success/></Behavior></Root></BehaviorTree></BehaviorTrees>'
  old=ET.fromstring(data);new=ET.fromstring(builder.patch_dialogue(data))
  seq=new.find('BehaviorTree/Root/Behavior/Sequence')
  self.assertEqual(ET.tostring(seq[0]),ET.tostring(old.find('BehaviorTree/Root/Behavior')[0]))
  self.assertEqual(ET.tostring(new[1]),ET.tostring(old[1]))
  self.assertEqual(seq[1].get('code'),"data.kcd2vrNoCameras = (System.GetCVar('wh_dlg_NoCameras') == 1)")
  self.assertEqual(seq[2].get('condition'),'$kcd2vrNoCameras and $preset==$enum:dialogPreset.fader')
  changes=seq[2][0].get('expressions').splitlines()
  self.assertEqual(changes,['$dialogParameters.useTwinsAsInitiator = false','$dialogParameters.movePlayer = false','$dialogParameters.rotatePlayer = false'])
 def test_pack_preserves_existing_activity_assets(self):
  key='AI/speech/getparametersfrompreset.xml'
  data=b'<BehaviorTrees><BehaviorTree name="speech_getParametersFromPreset"><Variables/><Root><Behavior><Success/></Behavior></Root></BehaviorTree></BehaviorTrees>'
  with tempfile.TemporaryDirectory() as d:
   r=Path(d);base=r/'base.pak';scripts=r/'scripts.pak';out=r/'out.pak'
   assets={'Animations/activity.adb':b'original camera and body identities','Animations/dialogue.adb':b'unchanged'}
   with zipfile.ZipFile(base,'w') as z:
    for name,value in assets.items():z.writestr(name,value)
   with zipfile.ZipFile(scripts,'w') as z:z.writestr(key,data)
   builder.build(base,scripts,out)
   with zipfile.ZipFile(out) as z:
    self.assertEqual(set(z.namelist()),set(assets)|{key})
    for name,value in assets.items():self.assertEqual(z.read(name),value)
    ET.fromstring(z.read(key))
if __name__=='__main__':unittest.main()
