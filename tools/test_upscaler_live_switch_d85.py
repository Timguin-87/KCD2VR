#!/usr/bin/env python3
"""D85 UPSCALERS: restart-free upscaler switch regression.

Extracts the literal worker block and autostart hold operand from src/mod.cpp,
compiles them with the production switch owner and the real D82 FSR / D85 XeSS
per-eye runtimes (tests/upscaler_live_switch_d85_test.cpp). Negative controls
mutate one production rule each and must make the fixture fail.
"""
from pathlib import Path
import argparse,hashlib,json,os,re,shutil,subprocess
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);p.add_argument('--cxx',default='g++');a=p.parse_args()
root=Path(__file__).resolve().parents[1];a.out.mkdir(parents=True,exist_ok=True)
mod=(root/'src/mod.cpp').read_text(encoding='utf-8-sig')
start=mod.index('        // D85 UPSCALERS: restart-free upscaler switch.')
end_marker='[&](const char* line) noexcept {diagnostic_trace::Write(line);});'
end=mod.index(end_marker,start)+len(end_marker)
block=mod[start:end]+'\n'
holds=re.findall(r'(?m)^\s*!upscaler_live_switch_d85::Instance\(\)\.HoldsStereo\(\) &&[^\n]*$',mod)
if len(holds)!=1:raise SystemExit('expected exactly one autostart hold operand')
hold=holds[0]+'\n'
header=(root/'src/upscaler_live_switch_d85.h').read_text()
def run(name,flags,mutation=None):
 d=a.out/name;d.mkdir(parents=True,exist_ok=True);inc=d/'inc';inc.mkdir(exist_ok=True)
 b,h,hdr=block,hold,header
 if mutation:
  where,old,new=mutation;text={'block':b,'hold':h,'header':hdr}[where]
  if text.count(old)!=1:raise SystemExit(f'{name}: mutation anchor must match once')
  text=text.replace(old,new)
  if where=='block':b=text
  elif where=='hold':h=text
  else:hdr=text
 (inc/'live_switch_block_extracted.inc').write_text(b);(inc/'live_switch_hold_extracted.inc').write_text(h)
 (inc/'upscaler_live_switch_d85.h').write_text(hdr)
 exe=d/'fixture'
 cmd=[a.cxx,'-std=c++20','-pthread','-Wall','-Wextra','-Wno-unused-parameter',*flags,'-I'+str(inc),'-I'+str(root/'src'),str(root/'tests/upscaler_live_switch_d85_test.cpp'),'-o',str(exe)]
 c=subprocess.run(cmd,capture_output=True,text=True);(d/'compile.log').write_text(c.stdout+c.stderr)
 r={'name':name,'negative_control':bool(mutation),'command':cmd,'compile_returncode':c.returncode,'passed':False}
 if c.returncode:return r
 env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0')
 x=subprocess.run([str(exe)],capture_output=True,text=True,timeout=60,env=env);(d/'run.log').write_text(x.stdout+x.stderr)
 r.update(run_returncode=x.returncode,output=(x.stdout+x.stderr).strip(),passed=(x.returncode==1 and 'FAIL' in x.stderr) if mutation else x.returncode==0)
 return r
jobs=[run('debug',['-O0','-g']),run('optimized',['-O2']),run('asan_ubsan',['-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer'])]
mutations={
 'write_while_stereo_busy':('header','            if(busy()){','            if(false&&busy()){'),
 'busy_leaf_ignores_run':('block','return f9_postfx_step==4||native_stereo_outer_lab::SingleRunBusy();','return false;'),
 'no_stop_request':('block','native_stereo_outer_lab::StopSingleRun();','(void)0;'),
 'accept_before_settle':('header','if(nowMs>=settle_&&ready(current_.tech))','if(ready(current_.tech))'),
 'autostart_not_held':('hold','!upscaler_live_switch_d85::Instance().HoldsStereo() &&','true &&'),
 'mode_written_before_quality':('block','for(const auto& v:values){','std::reverse(values.begin(),values.end());for(const auto& v:values){'),
 'no_readback':('block','||SafeCVarGetInt(cvar,-99)!=v.second',''),
 'no_pair_wait':('header','if(!stereoWanted||current_.tech==0){Finish(Phase::Done,Why::None,log);return;}','{Finish(Phase::Done,Why::None,log);return;}'),
}
for name,m in mutations.items():jobs.append(run('negative_'+name,['-O1'],m))
paths=['src/upscaler_live_switch_d85.h','src/mod.cpp','tests/upscaler_live_switch_d85_test.cpp','tools/test_upscaler_live_switch_d85.py','src/xess_stereo_runtime_d85.h','src/fsr_stereo_runtime_d82.h']
report={'passed':all(j['passed'] for j in jobs),'jobs':jobs,'source_sha256':{q:hashlib.sha256((root/q).read_bytes()).hexdigest() for q in paths},
 'limits':['Literal mod.cpp worker block and hold operand; real switch owner and FSR/XeSS runtimes.','Game CVars, stereo run, plugin create/size paths and vendor SDKs are fixture leaves modelled on the cited WHGame RVAs.','No Windows DLL, game, GPU or headset execution.']}
(a.out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n')
for j in jobs:print(j['name'],'PASS' if j['passed'] else 'FAIL',j.get('output','compile failed')[:200])
raise SystemExit(0 if report['passed'] else 1)
