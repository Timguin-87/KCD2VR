"""Run the actual crossbow publisher and check its body-publication ordering."""
from pathlib import Path
import argparse, json, hashlib, subprocess
from check_crossbow_d82 import function
from host_compiler import command, executable_path
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);p.add_argument('--compiler');a=p.parse_args()
out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
ranged=root/'src/subsystems/ranged_reload/crossbow_rifle_request_h21.inl'
pose=root/'src/vrik/coherent_pose.inl'
actual=function(ranged.read_text(),'static void RangedPublishXR34(')
(out/'crossbow_publisher_actual.inc').write_text(actual)
s=pose.read_text(); pub=s.index('g_prepareReloadPoseXR34(tx.reloadPassengerXR34')
assert pub < s.index('FramePublicationScope51 publication51;',pub) < s.index('g_committed_frame6695.publish(packet)',pub)
assert s.count('g_prepareReloadPoseXR34(tx.reloadPassengerXR34')==1
assert 'g_publishReloadPoseXR34' not in s
aux=s[s.rfind('if(tx.reloadWorkingHandXR34)',0,pub):s.index('FramePublicationScope51 publication51;',pub)]
assert 'rejectPublish169r2' not in aux and 'return;' not in aux
exe=executable_path(out/'crossbow_publication')
cmd=command(root/'tests/crossbow_publication_r91r2_test.cpp',exe,[out],a.compiler)
b=subprocess.run(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(out/'build.log').write_text(b.stdout)
if b.returncode:raise RuntimeError(b.stdout)
r=subprocess.run([str(exe)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(out/'run.log').write_text(r.stdout);print(r.stdout)
paths=[ranged,pose,root/'tests/crossbow_publication_r91r2_test.cpp',Path(__file__)]
(out/'RESULT.json').write_text(json.dumps({'passed':r.returncode==0,'order_check':True,'result':r.stdout,'source_sha256':{str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},'limitations':'Production publisher executes with explicit native identity and mailbox leaves. No headset or game scheduler execution.'},indent=2))
raise SystemExit(r.returncode)
