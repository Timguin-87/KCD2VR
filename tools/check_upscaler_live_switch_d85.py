#!/usr/bin/env python3
from pathlib import Path
import argparse,subprocess,json,hashlib
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args();root=Path(__file__).resolve().parents[1];out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
source=root/'src/upscaler_live_switch_d85.h';text=source.read_text()
mutations={
 'contexts_claim_success':text.replace('phase_.store(int(Phase::StartingStereo),std::memory_order_release);','Finish(Phase::Done,Why::None,log);'),
 'old_epoch_accepted':text.replace('visible.epoch!=epochBaseline_&&',''),
 'wrong_technology_accepted':text.replace('visible.technology==current_.tech','true'),
 'stale_publication_accepted':text.replace('visible.epoch==visible.currentEpoch&&',''),
 'hold_deadlocks_autostart':text.replace('hold_.store(false,std::memory_order_release);return;','hold_.store(true,std::memory_order_release);return;'),
}
results=[]
for name,body in [('positive',text),*mutations.items()]:
 assert name=='positive' or body!=text
 d=out/name;d.mkdir(exist_ok=True);(d/source.name).write_text(body)
 c=subprocess.run(['g++','-std=c++20','-O2','-Wall','-Wextra','-Werror','-I'+str(d),str(root/'tests/upscaler_live_switch_d85_test.cpp'),'-o',str(d/'fixture')],text=True,capture_output=True);(d/'compile.log').write_text(c.stdout+c.stderr)
 assert c.returncode==0,str(d/'compile.log')
 r=subprocess.run([str(d/'fixture')],text=True,capture_output=True,timeout=10);(d/'run.log').write_text(r.stdout+r.stderr)
 assert (r.returncode==0 if name=='positive' else r.returncode==1),str(d/'run.log')
 results.append({'name':name,'passed':True,'exit_code':r.returncode,'output':r.stdout.strip()})
report={'production_header':str(source.relative_to(root)),'sha256':hashlib.sha256(source.read_bytes()).hexdigest(),'runs':results,'windows_build':False,'headset_test':False}
(out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
