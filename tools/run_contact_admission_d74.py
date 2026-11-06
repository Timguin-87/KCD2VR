#!/usr/bin/env python3
"""Compile current contact/shoulder admission source with synthetic native leaves.
Generates ONLY into --out, preserving hardlinked frozen source/evidence files.
"""
import argparse,hashlib,json,os,runpy,subprocess,sys
from pathlib import Path

def main():
 ap=argparse.ArgumentParser(description=__doc__)
 ap.add_argument('--out',type=Path,required=True)
 ap.add_argument('--only',default='contact_admission_d74,contact_locality_d64,contact_corner_d56,contact_continuity_d71,contact_reseed_d53,contact_fairness_d67')
 ap.add_argument('--asan',action='store_true')
 ap.add_argument('--negative',action='store_true',help='restore old renewal/layout bugs in generated fixture only; expect failing assertions')
 a=ap.parse_args();r=Path(__file__).resolve().parents[1];o=a.out.resolve();o.mkdir(parents=True,exist_ok=True)
 if o==r/'tests':raise ValueError('Use a separate output folder, never tests/')
 wt=Path.write_text;rp=os.replace
 def redirect(p):return o/p.name if p.parent==r/'tests' and p.name.startswith('generated_') else p
 def write(p,text,*args,**kwargs):return wt(redirect(p),text,*args,**kwargs)
 def replace(x,y):return rp(redirect(Path(x)),redirect(Path(y)))
 Path.write_text=write;os.replace=replace
 try:
  for script in ['extract_adapter_types.py','extract_world_adapter_test.py']:runpy.run_path(str(r/'tools'/script))
 finally:Path.write_text=wt;os.replace=rp
 reach=(r/'src/subsystems/vrik/pose_reach_input.inl').read_text();start=reach.index('    auto accommodateShoulderR38=');end=reach.index('    V3 modelIntentGoalsH30',start)
 (o/'generated_shoulder_caller_d74.inc').write_text(reach[start:end])
 physical=(r/'src/vrik/physical_weapon.h').read_text();line=next(x for x in physical.splitlines() if x.startswith('inline double reach_max('));(o/'generated_reach_max_d74.inc').write_text(line+'\n')
 fixture=(r/'tests/world_adapter_test.cpp').read_text().replace('"../src/','"'+(r/'src').as_posix()+'/');(o/'world_adapter_test.cpp').write_text(fixture)
 if a.negative:
  p=o/'generated_world_adapter.inc';s=p.read_text();needle='if(!resolvedD74)return incomplete();';assert s.count(needle)==1;s=s.replace(needle,'(void)resolvedD74; // negative control only')
  needle='if(c.have&&previousCount!=count&&(previousCount<6||count<6))std::copy_n(offsets,count,c.offset);';assert s.count(needle)==1;s=s.replace(needle,'if(false)std::copy_n(offsets,count,c.offset);');p.write_text(s)
 records=[];cases=a.only.split(',');cxx=os.environ.get('CXX','cl' if os.name=='nt' else 'g++');msvc=Path(cxx).name.lower() in ['cl','cl.exe']
 for name in cases:
  s=(r/'tests'/(name+'_test.cpp')).read_text().replace('"../src/','"'+(r/'src').as_posix()+'/');src=o/(name+'_test.cpp');src.write_text(s);exe=o/(name+('.exe' if os.name=='nt' else ''))
  includes=[r/'src',o,r/'tests/syntax_shims',r/'tests/openxr_sdk']
  if msvc:
   cmd=[cxx,'/nologo','/std:c++20','/EHsc','/Od','/UNDEBUG','/Gy','/Gw','/bigobj',*['/I'+str(p) for p in includes],str(src),'/Fe:'+str(exe),'/Fo:'+str(o)+os.sep,'/Fd:'+str(o/(name+'.pdb'))]
   if a.asan:cmd+=['/fsanitize=address','/Zi']
   cmd+=['/link','/OPT:REF','/STACK:8388608']
  else:
   cmd=[cxx,'-std=c++23','-O0','-g','-pthread','-fms-extensions','-D__fastcall=','-ffunction-sections','-fdata-sections',*['-I'+str(p) for p in includes],str(src),'-Wl,--gc-sections','-o',str(exe)]
   if a.asan:cmd+=['-fsanitize=address,undefined','-fno-omit-frame-pointer']
  c=subprocess.run(cmd,capture_output=True,text=True);(o/(name+'.compile.log')).write_text(c.stdout+c.stderr);rec={'case':name,'command':cmd,'compile':c.returncode}
  if c.returncode==0:
   x=subprocess.run([str(exe)],capture_output=True,text=True);(o/(name+'.run.log')).write_text(x.stdout+x.stderr);rec.update(run=x.returncode,summary=x.stdout.splitlines()[-1:],failures=[line for line in x.stdout.splitlines() if line.startswith('FAIL')])
  print(name,rec.get('summary'),rec.get('failures'),flush=True);records.append(rec)
 inputs=['src/frik_pose_probe.cpp','src/subsystems/vrik/pose_reach_input.inl','src/vrik/coherent_pose.inl','src/vrik/contact_admission_d74.h','src/vrik/hand_contact_types.inl','src/vrik/contact_release_d45.h','tests/contact_admission_d74_test.cpp','tools/run_contact_admission_d74.py']
 report={'cases':records,'negative':a.negative,'asan':a.asan,'source_sha256':{p:hashlib.sha256((r/p).read_bytes()).hexdigest() for p in inputs},'limits':'Actual contact/shoulder code with simulated rays and native/hierarchy-write leaves; no engine/headset claim.'}
 (o/'RESULTS.json').write_text(json.dumps(report,indent=2))
 return 0 if all(x.get('run')==(1 if a.negative else 0) for x in records) else 1
if __name__=='__main__':raise SystemExit(main())
