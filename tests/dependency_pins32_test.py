"""Execute real digest/refusal PowerShell leaves, without installer/network calls."""
from pathlib import Path
import hashlib,json,shutil,subprocess,tempfile,unittest
ROOT=Path(__file__).resolve().parents[1]
PS=shutil.which('powershell')
@unittest.skipUnless(PS,'Windows PowerShell unavailable')
class DependencyPins32(unittest.TestCase):
 def run_ps(self,script,*args):
  return subprocess.run([PS,'-NoProfile','-NonInteractive','-ExecutionPolicy','Bypass','-File',str(ROOT/'tools'/script),*map(str,args)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
 def test_actual_digest_leaf_accepts_and_refuses(self):
  with tempfile.TemporaryDirectory(prefix='dependency space32 ') as d:
   p=Path(d)/'test package.zip';p.write_bytes(b'controlled archive bytes')
   for algorithm in ('SHA256','SHA512'):
    expected=hashlib.new(algorithm.lower(),p.read_bytes()).hexdigest()
    result=self.run_ps('verify_dependency32.ps1','-Archive',p,'-Algorithm',algorithm,'-Expected',expected)
    self.assertEqual(result.returncode,0,result.stdout)
    bad='0'*len(expected)
    result=self.run_ps('verify_dependency32.ps1','-Archive',p,'-Algorithm',algorithm,'-Expected',bad)
    self.assertNotEqual(result.returncode,0);self.assertIn('fingerprint mismatch',result.stdout)
    self.assertEqual(p.read_bytes(),b'controlled archive bytes')
 def test_unpinned_tag_refuses_before_output_or_network(self):
  with tempfile.TemporaryDirectory() as d:
   out=Path(d)/'not created'
   result=self.run_ps('fetch_dmk32.ps1','-OutputRoot',out,'-Tag','latest')
   self.assertNotEqual(result.returncode,0);self.assertIn('Unpinned',result.stdout);self.assertFalse(out.exists())
 def test_existing_output_not_overwritten(self):
  with tempfile.TemporaryDirectory() as d:
   out=Path(d);p=out/'sentinel';p.write_text('keep')
   result=self.run_ps('fetch_dmk32.ps1','-OutputRoot',out)
   self.assertNotEqual(result.returncode,0);self.assertIn('refusing to overwrite',result.stdout);self.assertEqual(p.read_text(),'keep')
 def test_cmake_fallback_matches_lock(self):
  lock=json.loads((ROOT/'data/build_dependencies32.json').read_text());cm=(ROOT/'CMakeLists.txt').read_text()
  self.assertIn('URL '+lock['OpenXR_SDK_Source']['url'],cm)
  self.assertIn('URL_HASH SHA512='+lock['OpenXR_SDK_Source']['sha512'],cm)
  self.assertNotIn('refs/heads/main.zip',cm)
if __name__=='__main__':unittest.main()
