"""C45: the shipped companion pak carries the source defaultProfile (Quick talk prompt = X), and the patch tool is deterministic."""
from pathlib import Path
import hashlib,json,subprocess,sys,tempfile,unittest,zipfile
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import patch_companion_profile45 as tool
PAK=ROOT/'companion/Mods/KCD2VR/Data/KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak'
PROFILE=ROOT/'companion/source/Libs/Config/defaultProfile.xml'
class CompanionProfile45(unittest.TestCase):
 def test_source_profile_bindings(self):
  tool.validate(PROFILE.read_bytes())
  text=PROFILE.read_text(encoding='utf-8')
  self.assertEqual(text.count('name="chat_init_with_focus"'),1)
  self.assertIn('<action name="chat_init_with_focus" onPress="1" onRelease="1" keyboard="_keybinds_ref_" xboxpad="xi_x" pspad="pad_l1" />',text)
 def test_shipped_pak_in_sync(self):
  with zipfile.ZipFile(PAK) as z:
   self.assertIsNone(z.testzip());self.assertEqual(len(z.namelist()),26)
   self.assertEqual(z.read(tool.PROFILE),PROFILE.read_bytes())
 def test_tool_preserves_other_members_and_is_deterministic(self):
  with tempfile.TemporaryDirectory() as d:
   # a stale profile member is replaced; every other member stays byte-identical, twice the same output
   with zipfile.ZipFile(PAK) as z:members={n:z.read(n) for n in z.namelist()}
   stale=Path(d)/'stale.pak'
   with zipfile.ZipFile(stale,'w') as z:
    for n,data in members.items():z.writestr(n,b'<profile/>' if n==tool.PROFILE else data)
   for _ in range(2):
    subprocess.run([sys.executable,str(ROOT/'tools/patch_companion_profile45.py'),'--pak',str(stale)],check=True,capture_output=True)
    with zipfile.ZipFile(stale) as z:
     self.assertEqual(z.namelist(),list(members))
     for n,data in members.items():self.assertEqual(z.read(n),data if n!=tool.PROFILE else PROFILE.read_bytes(),n)
   first=hashlib.sha256(stale.read_bytes()).hexdigest()
   subprocess.run([sys.executable,str(ROOT/'tools/patch_companion_profile45.py'),'--pak',str(stale)],check=True,capture_output=True)
   self.assertEqual(first,hashlib.sha256(stale.read_bytes()).hexdigest())
   report=json.loads(stale.with_suffix('.C45.json').read_text());self.assertEqual(report['members'],26)
 def test_rejects_lb_prompt(self):
  bad=PROFILE.read_text(encoding='utf-8').replace('xboxpad="xi_x" pspad="pad_l1" />','xboxpad="xi_shoulderl" pspad="pad_l1" />')
  with self.assertRaises(ValueError):tool.validate(bad.encode())
if __name__=='__main__':unittest.main()
