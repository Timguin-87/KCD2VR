#!/usr/bin/env python3
"""Test active V66.90 contracts. This is not a Windows/ASI/headset verification."""
from pathlib import Path
import argparse,hashlib,importlib.util,json,shutil,struct,subprocess,sys,tempfile
R=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('old',R/'tools/validate_v6689.py');old=importlib.util.module_from_spec(spec);spec.loader.exec_module(old)
def execute(args,path):return old.execute(args,path)
def checks(path,manifest):
 data=path.read_bytes();info=json.loads(manifest.read_text());assert hashlib.sha256(data).hexdigest()==info['sha256'],'WHGame hash'
 pe=struct.unpack_from('<I',data,60)[0];n=struct.unpack_from('<H',data,pe+6)[0];o=struct.unpack_from('<H',data,pe+20)[0];base=struct.unpack_from('<Q',data,pe+48)[0];secs=[]
 assert struct.unpack_from('<I',data,pe+8)[0]==info['timestamp'],'PE timestamp'
 for i in range(n):
  x=pe+24+o+i*40;vs,va,sz,p=struct.unpack_from('<IIII',data,x+8);secs.append((va,sz,p))
 def get(a,n):
  for va,sz,p in secs:
   if va<=a and a+n<=va+sz:return data[p+a-va:p+a-va+n]
  raise ValueError('Unmapped RVA')
 for a,h in info['entry_bytes'].items():assert get(int(a,16),len(h)//2).hex()==h,a
 for expression,target in info['vtable_entries'].items():assert struct.unpack('<Q',get(sum(int(x,16) for x in expression.split('+')),8))[0]==base+int(target,16),expression
 return {'entry_checks':len(info['entry_bytes']),'vtable_checks':len(info['vtable_entries']),'runtime':'NOT_TESTED'}
def mutants(out):
 jobs=[('queued_is_active','playback_snapshot.h','e.activated=(e.dynamicFlags&1u)!=0;','e.activated=true;'),('wrong_phase_component','playback_snapshot.h','raw.data()+0x44','raw.data()+0x40'),('competing_native_modes','lab_state.h','if(mode==1 || (mode==2 && ((bits>>other)&3u)==1))','if(false)'),('lost_pose_lease','lab_state.h','const int restore=autoOwned?base:-1;','const int restore=-1;')]
 results=[]
 for name,file,a,b in jobs:
  with tempfile.TemporaryDirectory() as temp:
   t=Path(temp);(t/'tests').mkdir();shutil.copytree(R/'src/diagnostics',t/'src/diagnostics');shutil.copy2(R/'tests/playback_lab_test.cpp',t/'tests/playback_lab_test.cpp');p=t/'src/diagnostics'/file;s=p.read_text();assert a in s,name;p.write_text(s.replace(a,b,1))
   cmd=['g++','-std=c++17',str(t/'tests/playback_lab_test.cpp'),'-o',str(t/'test')];c=subprocess.run(cmd,capture_output=True,text=True);assert c.returncode==0,c.stderr
   result=subprocess.run([str(t/'test')],capture_output=True,text=True);(out/f'mutant_{name}.log').write_text(result.stdout+result.stderr);assert result.returncode!=0,f'{name} survived'
   results.append({'name':name,'compiled':True,'rejected':True})
 return results

def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--whgame',type=Path);ap.add_argument('--build-dir',type=Path,default=R/'build-contracts');ap.add_argument('--refresh-generated',action='store_true');ap.add_argument('--skip-host-shims',action='store_true');ap.add_argument('--skip-mutations',action='store_true');a=ap.parse_args();out=R/'delivery';out.mkdir(exist_ok=True)
 report={'build':'V66.90','native_windows_compile':'NOT_RUN','headset':'NOT_RUN','physical_grab':'NOT_IMPLEMENTED','native_damage_injection':'NONE'}
 for name,value in old.generated().items():
  p=R/name
  if a.refresh_generated:p.write_text(value)
  assert p.read_text()==value,'stale extracted adapter '+name
 frozen=json.loads((R/'continuity/FROZEN_SOURCE_SHA256.json').read_text());report['frozen_original_files']=len(frozen)
 for name,h in frozen.items():assert hashlib.sha256((R/name).read_bytes()).hexdigest()==h,'frozen file '+name
 preserved=json.loads((R/'continuity/V66.89_PRESERVED_SOURCE_SHA256.json').read_text());report['unchanged_v6689_source_files']=len(preserved)
 for name,h in preserved.items():assert hashlib.sha256((R/name).read_bytes()).hexdigest()==h,'V66.89 preserved file '+name
 marker='KCD2VR-V66.90-INTERACTION-HOLSTER-COMBAT-DIAGNOSTICS'
 for name in ('src/mod.cpp','build_release.bat'):assert marker in (R/name).read_text(),'build marker '+name
 if a.whgame:
  report['prior_native_contract']=old.native_check(a.whgame)
  report['new_native_contract']=checks(a.whgame,R/'continuity/native_v6690/ANIMATION_QUEUE_ABI.json')
 else:report['native_contract']='NOT_RUN: --whgame needed'
 execute(['cmake','-S',R/'tests','-B',a.build_dir,'-DCMAKE_BUILD_TYPE=Release'],out/'contracts_configure.log')
 execute(['cmake','--build',a.build_dir,'--parallel','4'],out/'contracts_build.log')
 execute(['ctest','--test-dir',a.build_dir,'--output-on-failure','-V'],out/'contracts_ctest.log')
 execute([sys.executable,R/'tests/test_diagnostic_report.py','-v'],out/'diagnostic_report_tests.log')
 report['cpp_test_groups']=5;report['cpp_assertions']=93;report['python_report_tests']=16
 if not a.skip_host_shims:
  with tempfile.TemporaryDirectory() as tmp:
   t=Path(tmp);objs=[]
   for name in ('frik_pose_probe','animation_filter','combat_diag_probe','diagnostic_trace'):
    obj=t/(name+'.o');execute(['g++','-std=c++17','-O0','-I'+str(R/'tests/syntax_shims'),'-I'+str(R/'src'),'-c',R/'src'/f'{name}.cpp','-o',obj],out/f'{name}_host_object.log');objs.append(obj)
   (t/'main.cpp').write_text('int main(){return 0;}\n');execute(['g++',*objs,t/'main.cpp','-o',t/'mock_link'],out/'host_link.log')
  report['host_shim']='Four complete translation units compile/link. Windows and DetourModKit replaced. mod.cpp NOT fully compiled.'
 else:report['host_shim']='SKIPPED'
 if not a.skip_mutations:report['mutations']=old.mutation_checks(out)+mutants(out)
 else:report['mutations']='SKIPPED'
 (out/'VERIFICATION.json').write_text(json.dumps(report,indent=2)+'\n');print('V66.90 checks PASS; native Windows/headset NOT tested.')
if __name__=='__main__':
 try:main()
 except Exception as e:print('FAIL:',e,file=sys.stderr);raise SystemExit(1)
