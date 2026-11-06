"""Compare inherited historical D54 fixture outcomes at D77 donor and current Lua.
This reports donor parity, never turns historical D54 assertions into D77 passes.
"""
from pathlib import Path
import argparse,importlib.util,unittest,json,hashlib
ROOT=Path(__file__).resolve().parents[1]
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--baseline',type=Path,required=True);ap.add_argument('--out',type=Path,required=True);a=ap.parse_args()
 spec=importlib.util.spec_from_file_location('d54parity',ROOT/'tests/test_gore_presentation_d54.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
 original=m.GorePresentationD54.runtime
 def deterministic(self):
  lua=original(self);lua.execute('math.randomseed(1)');return lua
 m.GorePresentationD54.runtime=deterministic
 class Result(unittest.TestResult):
  def __init__(self):super().__init__();self.rows={}
  def addSuccess(self,test):super().addSuccess(test);self.rows[str(test)]='PASS'
  def addFailure(self,test,err):super().addFailure(test,err);self.rows[str(test)]='FAIL:'+err[0].__name__+':'+str(err[1])
  def addError(self,test,err):super().addError(test,err);self.rows[str(test)]='ERROR:'+err[0].__name__+':'+str(err[1])
  def addSubTest(self,test,subtest,err):
   super().addSubTest(test,subtest,err)
   if err:self.rows[str(subtest)]='SUBTEST:'+err[0].__name__+':'+str(err[1])
 current=ROOT/'mods/KCD2VRGore/r93r3/zzkcd2vr_gore.lua';reports=[]
 for path in [a.baseline,current]:
  m.SOURCE=path;result=Result();unittest.defaultTestLoader.loadTestsFromModule(m).run(result)
  reports.append({'source':str(path),'sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'outcomes':result.rows})
 passed=reports[0]['outcomes']==reports[1]['outcomes']
 record={'identical_outcomes':passed,'comparison':reports,'note':'Historical D54 fixture predates D77 wrapper/cap behavior. Inherited failures are retained as failures, not claimed current passes.'}
 a.out.write_text(json.dumps(record,indent=2)+'\n');print('D77 historical fixture parity:',passed);return 0 if passed else 1
if __name__=='__main__':raise SystemExit(main())
