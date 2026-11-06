"""Extract current placement code, execute it, then reject the old overwrite bug."""
from pathlib import Path
import argparse, hashlib, json, subprocess
from check_crossbow_d82 import function
from host_compiler import command, executable_path

root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--out',required=True,type=Path);p.add_argument('--compiler',default='cl');a=p.parse_args()
out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
leaf=root/'src/frik_pose_probe.cpp';owner=root/'src/vrik/tool_attachment59c10.inl'
place=function(leaf.read_text(),'static bool PlaceBothConsumers6689(')
s=owner.read_text();start=s.index('    bool child=false;\n    if(!PlaceBothConsumers6689(')
end=s.index('    if(nativeAlchemy18&&alchemy18.type==3&&side==0)',start)
branch=s[start:end]
(out/'pickup_place_actual.inc').write_text(place)
(out/'pickup_placement_result_actual.inc').write_text(branch)
source=root/'tests/pickup_attachment_r91r3_test.cpp'
results={}
for name,opt,mutation in [('debug','/Od',False),('optimized','/O2',False),('old_native_fallthrough','/O2',True)]:
    if mutation:
        assert branch.count('return nativeGrasp;')==1
        (out/'pickup_placement_result_actual.inc').write_text(branch.replace('return nativeGrasp;','return false;'))
    exe=executable_path(out/name)
    cmd=command(source,exe,[out],a.compiler)
    if '/O2' in cmd:cmd[cmd.index('/O2')]=opt
    b=subprocess.run(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
    (out/(name+'_build.log')).write_text(b.stdout)
    if b.returncode:raise RuntimeError(b.stdout)
    r=subprocess.run([str(exe)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
    (out/(name+'_run.log')).write_text(r.stdout)
    results[name]={'exit':r.returncode,'output':r.stdout,'expected_failure':mutation}
    if (r.returncode==0)==mutation:raise RuntimeError(f'{name}: unexpected result\n{r.stdout}')
(out/'pickup_placement_result_actual.inc').write_text(branch)
paths=[leaf,owner,source,Path(__file__)]
(out/'RESULT.json').write_text(json.dumps({'passed':True,'results':results,'sha256':{str(x.relative_to(root)):hashlib.sha256(x.read_bytes()).hexdigest() for x in paths},'limitations':'Current production leaf and callback decision with explicit native doubles. No DLL build, injection or headset execution.'},indent=2))
print(json.dumps(results,indent=2))
