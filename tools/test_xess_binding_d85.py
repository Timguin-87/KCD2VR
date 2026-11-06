#!/usr/bin/env python3
"""D85 UPSCALERS: literal XeSS binding/install regression.

Extracts the production Install and wrapper bodies from
src/native_xess_stereo_d85.cpp and runs them with controlled module, pin,
import-binding and hook leaves. Negative controls mutate one production rule.
"""
from pathlib import Path
import argparse,hashlib,json,re,subprocess
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);p.add_argument('--cxx',default='g++');a=p.parse_args()
root=Path(__file__).resolve().parents[1];a.out.mkdir(parents=True,exist_ok=True)
production=['src/native_xess_stereo_d85.cpp','src/xess_stereo_runtime_d85.h']
start={rel:hashlib.sha256((root/rel).read_bytes()).hexdigest() for rel in production}
text=(root/'src/native_xess_stereo_d85.cpp').read_text()
def function(name):
 m=re.search(r'(?m)^(?:rt::Result|bool|void)\s+'+re.escape(name)+r'\(',text)
 if not m:raise SystemExit('Missing production definition: '+name)
 at=text.index('{',m.end());depth=1;end=at+1
 while depth:depth+=(text[end]=='{')-(text[end]=='}');end+=1
 return text[m.start():end]
exports=re.search(r'(?m)^constexpr const char\* kExports\[5\][^\n]+',text).group(0)
wrappers='\n'.join(function(n) for n in ['Create','Init','Velocity','Destroy','Execute'])
install='\n'.join(function(n) for n in ['Install','Installed','SetEnabled','ReadyForStereo','PairReady','Stop','LogStatus'])
def command(cmd,log,expected=0):
 x=subprocess.run(cmd,capture_output=True,text=True);log.write_text(x.stdout+x.stderr)
 if x.returncode!=expected or (expected and 'FAIL' not in x.stdout+x.stderr):print(x.stdout+x.stderr);raise SystemExit(f'Unexpected result: {log} rc={x.returncode}')
 return {'command':cmd,'exit':x.returncode,'output':(x.stdout+x.stderr).strip()}
def build(label,body,flags=('-O1',)):
 out=a.out/label;out.mkdir(exist_ok=True)
 (out/'xess_exports_extracted.inc').write_text(exports+'\n');(out/'xess_wrappers_extracted.inc').write_text(wrappers);(out/'xess_install_extracted.inc').write_text(body)
 exe=out/'xess_binding'
 cmd=[a.cxx,'-std=c++20',*flags,'-Wall','-Wextra','-I'+str(root/'src'),'-I'+str(out),str(root/'tests/xess_binding_d85_host.cpp'),'-o',str(exe),'-pthread']
 command(cmd,out/'build.log');return out,exe
results={}
cases=['success','late_module','wrong_pin','construction','enable0','enable1','enable2','enable3','enable4']
for label,flags in (('debug',('-O0','-g')),('optimized',('-O2',))):
 out,exe=build(label,install,flags);rows=[]
 for case in cases:
  row=command([str(exe),case],out/(case+'.log'));rows.append(row);print(label,row['output'])
 results[label]=rows
old='for(auto& old:hooks)if(old)(void)old->disable();';assert install.count(old)==1
out,exe=build('negative_partial_cleanup',install.replace(old,old+'\n        for(auto& h:hooks)h.reset(); runtime.SetApi({}); originals={};',1))
results['negative_partial_cleanup']=command([str(exe),'enable2'],out/'run.log',1)
old='for(auto& h:hooks)if(!h->enable())';assert install.count(old)==1
out,exe=build('negative_create_first',install.replace(old,'for(auto h=hooks.rbegin();h!=hooks.rend();++h)if(!(*h)->enable())',1))
results['negative_create_first']=command([str(exe),'success'],out/'run.log',1)
old='    if(!ExactGame()){';assert install.count(old)==1
out,exe=build('negative_no_pin',install.replace(old,'    if(false&&!ExactGame()){',1))
results['negative_no_pin']=command([str(exe),'wrong_pin'],out/'run.log',1)
end={rel:hashlib.sha256((root/rel).read_bytes()).hexdigest() for rel in production}
if start!=end:raise SystemExit('Production changed during run')
results['source_sha256']={**start,**{rel:hashlib.sha256((root/rel).read_bytes()).hexdigest() for rel in ['tests/xess_binding_d85_host.cpp','tools/test_xess_binding_d85.py']}}
results['limits']='Literal Install/wrapper bodies; module, pin, import-binding and hook leaves controlled; runtime actual. Windows ABI/GPU/headset NOT_RUN.'
(a.out/'results.json').write_text(json.dumps(results,indent=2)+'\n')
print('PASS: nine XeSS binding scenarios x2 builds, three production mutation negative controls')
