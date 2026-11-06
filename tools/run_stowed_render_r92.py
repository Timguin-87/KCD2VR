"""Actual stowed render leaf with explicit native paper/install/memory leaves."""
from pathlib import Path
import argparse, hashlib, json, subprocess
from host_compiler import command, executable_path
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--out',required=True,type=Path);p.add_argument('--baseline',action='store_true');a=p.parse_args()
out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
path='src/vrik/stowed_render_native_d63.inl'
text=subprocess.check_output(['git','show','4ba390e8d51ed1c4234c903404813a12b25a5e98:'+path],cwd=root,text=True) if a.baseline else (root/path).read_text()
paper='#include "../subsystems/vrmap/native_paper_render_m43.inl"'
assert text.count(paper)==1
# Paper is a separate binding consumer, outside these stowed-slot cases. The
# actual target detour/helper bodies remain unmodified in this extracted TU.
leaf='static bool NativePaperRenderM43(void*,void*,const StowedRenderHeaderD63&,std::uintptr_t,void*,void*,StowedRenderFnD63) noexcept{return false;}'
extracted=out/'stowed_render_actual.inl';extracted.write_text(text.replace(paper,leaf))
results=[]
for opt in (['/Od'] if a.baseline else ['/Od','/O2']):
 exe=executable_path(out/('stowed_render_r92'+opt[1:]));cmd=command(root/'tests/stowed_render_native_d63_test.cpp',exe,[root/'src'],'cl')
 cmd[cmd.index('/O2')]=opt;cmd.insert(1,'/DKCD2VR_STOWED_RENDER_R92_EXTRACT="'+str(extracted).replace('\\','/')+'"')
 b=subprocess.run(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);exe.with_suffix('.build.log').write_text(b.stdout)
 if b.returncode:raise RuntimeError(b.stdout)
 r=subprocess.run([str(exe)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);exe.with_suffix('.run.log').write_text(r.stdout);print(r.stdout,flush=True)
 ok=r.returncode!=0 and 'visible native cached bow cannot bypass' in r.stdout if a.baseline else r.returncode==0
 results.append({'optimization':opt,'exit':r.returncode,'output':r.stdout,'expected':ok})
 if not ok:raise RuntimeError(r.stdout)
paths=[path,'src/subsystems/ranged_reload/stowed_hide_hw.h','tests/stowed_render_native_d63_test.cpp','tools/run_stowed_render_r92.py']
(out/'RESULT.json').write_text(json.dumps({'passed':True,'baseline_expected_failure':a.baseline,'results':results,'tested_production_sha256':hashlib.sha256(text.encode()).hexdigest(),'source_sha256':{x:hashlib.sha256((root/x).read_bytes()).hexdigest() for x in paths},'limits':'Actual render bridge. Paper/install/native memory leaves controlled. No rendered frame or DLL.'},indent=2))
