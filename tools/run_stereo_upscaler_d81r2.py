"""Run actual D81R2 startup boundaries with host filesystem/Win32 leaf adapters."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
from run_startup_d56r1 import function
ROOT=Path(__file__).resolve().parents[1]
def main():
 p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);p.add_argument('--baseline-startup',type=Path);a=p.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
 source=ROOT/'src/subsystems/base/startup_setup_s15.inl';text=source.read_text()
 chunks=[]
 for start,end in [('struct Field {','// Panel-native'),('struct State {','static constexpr int contentHeight')]:
  chunks.append(text[text.index(start):text.index(end,text.index(start))])
 signatures=['static bool Parse(','static int ProductionMode(','static bool ReadSource(','static bool LaunchCompatible(','static bool SaveSource(']
 provenance=[]
 for signature in signatures:
  body,line=function(text,signature);chunks.append(body);provenance.append(dict(signature=signature,line=line,sha256=hashlib.sha256(body.encode()).hexdigest()))
 (out/'startup_d81r2_actual.inc').write_text('\n'.join(chunks))
 results=[]
 for name,flags in [('debug',['-O0','-g']),('release',['-O2']),('asan',['-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer'])]:
  exe=out/name;cmd=['g++','-std=c++20','-UNDEBUG',*flags,'-I'+str(ROOT/'src'),'-I'+str(out),str(ROOT/'tests/stereo_upscaler_d81r2_test.cpp'),'-o',str(exe)]
  build=subprocess.run(cmd,text=True,capture_output=True);(out/(name+'.compile.log')).write_text(build.stdout+build.stderr)
  record=dict(name=name,command=cmd,compile_exit=build.returncode)
  if build.returncode==0:
   env=os.environ.copy();env['ASAN_OPTIONS']='detect_leaks=0'
   run=subprocess.run([str(exe),str(out/(name+'_files'))],text=True,capture_output=True,env=env);record.update(exit=run.returncode,output=run.stdout+run.stderr);(out/(name+'.run.log')).write_text(run.stdout+run.stderr)
  record['passed']=record.get('exit')==0;results.append(record)
 negative=None
 if a.baseline_startup:
  # Causal control: replace only SaveSource with the exact pre-repair function.
  baseline=a.baseline_startup.resolve();old,line=function(baseline.read_text(),'static bool SaveSource(')
  (out/'startup_d81r2_actual.inc').write_text('\n'.join(chunks[:-1]+[old]))
  exe=out/'baseline_writer';cmd=['g++','-std=c++20','-UNDEBUG','-O0','-I'+str(ROOT/'src'),'-I'+str(out),str(ROOT/'tests/stereo_upscaler_d81r2_test.cpp'),'-o',str(exe)]
  build=subprocess.run(cmd,text=True,capture_output=True);(out/'baseline_writer.compile.log').write_text(build.stdout+build.stderr)
  negative=dict(source=str(baseline),source_sha256=hashlib.sha256(baseline.read_bytes()).hexdigest(),line=line,command=cmd,compile_exit=build.returncode)
  if build.returncode==0:
   run=subprocess.run([str(exe),str(out/'baseline_files')],text=True,capture_output=True);negative.update(exit=run.returncode,output=run.stdout+run.stderr)
   (out/'baseline_writer.run.log').write_text(run.stdout+run.stderr)
  negative['passed']=negative.get('exit')!=0 and 'FAIL check=102' in negative.get('output','')
  (out/'startup_d81r2_actual.inc').write_text('\n'.join(chunks))
 report=dict(source=str(source),source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),extracted=provenance,results=results,passed=all(x['passed'] for x in results) and (negative is None or negative['passed']),baseline_writer_negative=negative,asan_leak_detection=False,asan_leak_reason='LeakSanitizer cannot enumerate /proc task in this sandbox; address and UB instrumentation enabled',windows_build=False,headset_test=False)
 (out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2));return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
