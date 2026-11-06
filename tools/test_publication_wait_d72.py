"""Extract and execute the complete current XrManager publication wait."""
from pathlib import Path
import argparse, hashlib, json, subprocess
ap=argparse.ArgumentParser();ap.add_argument('output',type=Path);ap.add_argument('--compiler',default='g++');ap.add_argument('--extract-only',action='store_true');a=ap.parse_args()
root=Path(__file__).resolve().parents[1];a.output.mkdir(parents=True,exist_ok=True)
text=(root/'src/xr_manager.cpp').read_text()
start=text.index('bool XrManager::WaitForNextRenderFrameToken(')
end=text.index('\n}\n',start)+2
body=text[start:end]
assert 'publication_wait_d67::floor' in body and 'wait_for' in body and 'publication_wait_d67::ready' in body
(a.output/'publication_wait_d72_production.inc').write_text(body+'\n')
if a.extract_only:
 print('Extracted actual D73 wait '+hashlib.sha256(body.encode()).hexdigest());raise SystemExit(0)
cmd=[a.compiler,'-std=c++23','-O2','-Wall','-Wextra','-Werror','-pthread','-I',str(root/'src'),'-I',str(a.output),str(root/'tests/publication_wait_d72_test.cpp'),'-o',str(a.output/'publication_wait_d72')]
built=subprocess.run(cmd,text=True,capture_output=True)
report={'build_exit':built.returncode,'build_stdout':built.stdout,'build_stderr':built.stderr,'extracted_sha256':hashlib.sha256(body.encode()).hexdigest(),'retail_native_execution':False,'headset':'NOT_RUN','inputs':{p:hashlib.sha256((root/p).read_bytes()).hexdigest() for p in ['src/xr_manager.cpp','src/xr_manager.h','src/core/publication_wait_d67.h','tests/publication_wait_d72_test.cpp']}}
if built.returncode==0:
 r=subprocess.run([str(a.output/'publication_wait_d72')],text=True,capture_output=True,timeout=10);report.update(exit=r.returncode,stdout=r.stdout,stderr=r.stderr)
(a.output/'report.json').write_text(json.dumps(report,indent=2)+'\n')
print(report.get('stdout',''),report.get('stderr',''),report['build_stderr']);raise SystemExit(0 if report.get('exit')==0 else 1)
