"""Current expanded companion contract; original C45 count26 tests still run."""
from pathlib import Path
import hashlib,json,subprocess,sys,tempfile,unittest,zipfile
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import patch_companion_profile45 as tool
PAK=ROOT/'companion/Mods/KCD2VR/Data/KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak'
PROFILE=ROOT/'companion/source/Libs/Config/defaultProfile.xml'

class CompanionProfileT2(unittest.TestCase):
 def test_expanded_pak_profile_and_protected_parent_payloads(self):
  receipt=json.loads((ROOT/'review/T3/THROWING_PAK_PRESERVATION.json').read_text())
  expected={x['member'] for x in receipt['preserved_members']+receipt['added_members']}|set(receipt['replaced_members'])
  self.assertEqual(len(receipt['preserved_members']),66)
  with zipfile.ZipFile(PAK)as z:
   self.assertIsNone(z.testzip());self.assertEqual(len(z.namelist()),69)
   self.assertEqual(set(z.namelist()),expected)
   self.assertEqual(z.read(tool.PROFILE),PROFILE.read_bytes())
   for item in receipt['preserved_members']:
    self.assertEqual(hashlib.sha256(z.read(item['member'])).hexdigest(),item['sha256'],item['member'])

 def test_actual_profile_tool_preserves_all_expanded_payloads_deterministically(self):
  with tempfile.TemporaryDirectory()as d:
   with zipfile.ZipFile(PAK)as z:members={n:z.read(n)for n in z.namelist()}
   stale=Path(d)/'stale.pak'
   with zipfile.ZipFile(stale,'w')as z:
    for n,data in members.items():z.writestr(n,b'<profile/>'if n==tool.PROFILE else data)
   hashes=[]
   for _ in range(3):
    subprocess.run([sys.executable,str(ROOT/'tools/patch_companion_profile45.py'),'--pak',str(stale)],check=True,capture_output=True)
    with zipfile.ZipFile(stale)as z:
     self.assertEqual(z.namelist(),list(members));self.assertIsNone(z.testzip())
     for n,data in members.items():self.assertEqual(z.read(n),data if n!=tool.PROFILE else PROFILE.read_bytes(),n)
    hashes.append(hashlib.sha256(stale.read_bytes()).hexdigest())
   self.assertEqual(len(set(hashes)),1)
   report=json.loads(stale.with_suffix('.C45.json').read_text());self.assertEqual(report['members'],69)
