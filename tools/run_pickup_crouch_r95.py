"""Literal game-thread native crouch publisher plus pinned native query host fixture."""
from pathlib import Path
import argparse,hashlib,json,subprocess
from host_compiler import command,executable_path
R=Path(__file__).resolve().parents[1]
ap=argparse.ArgumentParser();ap.add_argument('--out',type=Path,required=True);a=ap.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
s=(R/'tests/native_crouch_r93_test.cpp').read_text();(out/'pickup_crouch_memory_r95_actual.inc').write_text(s[:s.index('static f::Input input()')])
p=R/'src/subsystems/vrik/native_crouch_r93.inl';s=p.read_text();s=s[s.index('static void PublishNativeCrouchR93()'):];(out/'pickup_crouch_publisher_r95_actual.inc').write_text(s)
rows=[]
for mode in ['debug','optimized','old_policy']:
 d=out/mode;d.mkdir(exist_ok=True)
 if mode=='old_policy':(d/'pickup_crouch_publisher_r95_actual.inc').write_text(s.replace('presentationCurrentR95()&&!g_ctx_menu.load()','g_gameplay_allowed.load()&&!g_ctx_scripted.load()&&!g_ctx_menu.load()'))
 exe=executable_path(d/'test');cmd=command(R/'tests/pickup_native_crouch_r95_test.cpp',exe,[d,out,R/'src'],'cl')
 if mode=='debug':cmd=[('/Od'if x=='/O2'else x)for x in cmd]
 b=subprocess.run(cmd,cwd=d,text=True,capture_output=True);(d/'build.log').write_text(b.stdout+b.stderr);row={'test':mode,'compile_exit':b.returncode,'passed':False}
 if not b.returncode:
  q=subprocess.run([str(exe)],text=True,capture_output=True);(d/'run.log').write_text(q.stdout+q.stderr);row.update(exit=q.returncode,output=q.stdout,passed=(q.returncode!=0 and 'FAIL'in q.stdout)if mode=='old_policy'else q.returncode==0)
 rows.append(row);print(mode+': '+str(row['passed']),flush=True)
(out/'RESULT.json').write_text(json.dumps({'passed':all(x['passed']for x in rows),'tests':rows,'source_sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'limits':['Actual publication/query and shared frame; native engine memory/call and adopted reader controlled; adopted reader separately tested. No DLL or headset execution.']},indent=2));raise SystemExit(0 if all(x['passed']for x in rows)else 1)
