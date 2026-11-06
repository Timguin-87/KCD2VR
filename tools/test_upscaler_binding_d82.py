#!/usr/bin/env python3
"""Independent aggregate TU and literal FSR binding/install regression."""
from pathlib import Path
import argparse,hashlib,json,re,subprocess
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);p.add_argument('--cxx',default='g++');p.add_argument('--baseline-aggregate',type=Path);a=p.parse_args();root=Path(__file__).resolve().parents[1]
a.out.mkdir(parents=True,exist_ok=True)
production=['src/native_upscaler_stereo_d82.cpp','src/native_fsr_stereo_d82.cpp','src/fsr_stereo_runtime_d82.h']
production_start={rel:hashlib.sha256((root/rel).read_bytes()).hexdigest() for rel in production}
aggregate=(root/'src/native_upscaler_stereo_d82.cpp').read_text();fsr=(root/'src/native_fsr_stereo_d82.cpp').read_text()
def function(text,name):
 # Match a definition, not a same-named runtime method call inside a wrapper.
 match=re.search(r'(?m)^(?:ffxReturnCode_t|bool|void)\s+'+re.escape(name)+r'\(',text)
 if not match:raise ValueError('Missing production definition: '+name)
 start=match.start();at=text.index('{',match.end());depth=1;end=at+1
 while depth:depth+=(text[end]=='{')-(text[end]=='}');end+=1
 return text[start:end]
def command(cmd,log,expected=0,contains=None):
 x=subprocess.run(cmd,capture_output=True,text=True);log.write_text(x.stdout+x.stderr)
 if x.returncode!=expected or (contains and contains not in x.stdout+x.stderr):print(x.stdout+x.stderr);raise SystemExit(f'Unexpected regression result: {log.name} rc={x.returncode}')
 return {'command':cmd,'exit':x.returncode,'stdout':x.stdout.strip()}
def build_aggregate(label,text):
 out=a.out/label;out.mkdir(exist_ok=True);source=out/'aggregate_actual.cpp';source.write_text(text);exe=out/'aggregate'
 cmd=[a.cxx,'-std=c++20','-O1','-Wall','-Wextra','-I'+str(root/'src'),str(source),str(root/'tests/upscaler_aggregate_d82_test.cpp'),'-o',str(exe),'-pthread']
 command(cmd,out/'build.log');return out,exe
results={};out,exe=build_aggregate('aggregate',aggregate);results['aggregate']=command([str(exe)],out/'run.log');print(results['aggregate']['stdout'])
if a.baseline_aggregate:
 baseline=a.baseline_aggregate.read_text();out,exe=build_aggregate('aggregate_pre_epoch_pin_negative',baseline)
 results['aggregate_pre_epoch_pin_negative']=command([str(exe)],out/'run.log',1,'MIDPAIR_TEMPORAL_TO_NATIVE_REFUSED')
 results['baseline_aggregate_sha256']=hashlib.sha256(a.baseline_aggregate.read_bytes()).hexdigest()
# Negative control: the stale cached technology cannot override current CVar.
old='return read?read():technology.load(std::memory_order_acquire);';assert old in aggregate
out,exe=build_aggregate('aggregate_cached_technology_negative',aggregate.replace(old,'return technology.load(std::memory_order_acquire);',1))
results['aggregate_cached_technology_negative']=command([str(exe)],out/'run.log',1,'FAIL')
wrappers='\n'.join(function(fsr,n) for n in ['Create','Destroy','Configure','Dispatch'])
install='\n'.join(function(fsr,n) for n in ['Install','Installed','SetEnabled','ReadyForStereo','PairReady','Stop','LogStatus'])
def build_binding(label,body):
 out=a.out/label;out.mkdir(exist_ok=True);(out/'fsr_wrappers_extracted.inc').write_text(wrappers);(out/'fsr_install_extracted.inc').write_text(body);exe=out/'fsr_binding'
 cmd=[a.cxx,'-std=c++20','-O1','-Wall','-Wextra','-I'+str(root/'src'),'-I'+str(out),str(root/'tests/fsr_binding_d82_host.cpp'),'-o',str(exe),'-pthread']
 command(cmd,out/'build.log');return out,exe
out,exe=build_binding('fsr_binding',install);rows=[]
for case in ['success','late_module','wrong_pin','construction','enable0','enable1','enable2','enable3']:
 row=command([str(exe),case],out/(case+'.log'));rows.append(row);print(row['stdout'])
results['fsr_binding']=rows
# Reproduce the concrete unsafe cleanup that used to discard live trampolines.
old='for(auto& old:hooks)if(old)(void)old->disable();';assert old in install
bad=install.replace(old,old+'\n        for(auto& h:hooks)h.reset(); runtime.SetApi({}); originals={};',1)
out,exe=build_binding('fsr_partial_cleanup_negative',bad);results['fsr_partial_cleanup_negative']=command([str(exe),'enable1'],out/'run.log',1,'FAIL')
# Enabling create first must fail before any native context could be duplicated.
old='for(auto& h:hooks)if(!h->enable())';assert old in install
bad=install.replace(old,'for(auto h=hooks.rbegin();h!=hooks.rend();++h)if(!(*h)->enable())',1)
out,exe=build_binding('fsr_create_first_negative',bad);results['fsr_create_first_negative']=command([str(exe),'success'],out/'run.log',1,'FAIL')
production_end={rel:hashlib.sha256((root/rel).read_bytes()).hexdigest() for rel in production}
if production_start!=production_end:raise SystemExit('Production changed during run; rerun against stable source before reporting success')
results['source_sha256']={**production_start,**{rel:hashlib.sha256((root/rel).read_bytes()).hexdigest() for rel in ['tests/upscaler_aggregate_d82_test.cpp','tests/fsr_binding_d82_host.cpp','tools/test_upscaler_binding_d82.py']}}
results['limits']='Actual aggregate TU and literal Install/wrapper bodies; controlled backend, pin, module and hook leaves. FFX Runtime actual. Windows ABI/GPU/headset NOT_RUN.'
(a.out/'results.json').write_text(json.dumps(results,indent=2)+'\n');print('PASS: aggregate, eight FSR binding scenarios, three production mutation negative controls plus optional actual pre-fix negative')
