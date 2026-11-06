#!/usr/bin/env python3
"""Recovery checks. Host models and static bytes are not native runtime acceptance."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse,hashlib,json,re,struct,subprocess,sys,tempfile,shutil
R=Path(__file__).resolve().parents[1]
def run(cmd,log,cwd=None):
 p=subprocess.run([str(x) for x in cmd],cwd=cwd or R,capture_output=True,text=True)
 Path(log).write_text(p.stdout+p.stderr)
 if p.returncode:raise RuntimeError(f'{cmd}: failed; see {log}\n'+(p.stdout+p.stderr)[-3000:])
 return p.stdout+p.stderr
def native_contract(dll):
 b=dll.read_bytes();h=hashlib.sha256(b).hexdigest()
 assert h=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7','wrong retail DLL'
 pe=struct.unpack_from('<I',b,0x3c)[0];assert struct.unpack_from('<I',b,pe+8)[0]==0x6A350E20
 n,opt=struct.unpack_from('<H',b,pe+6)[0],struct.unpack_from('<H',b,pe+20)[0]
 def data(rva,size):
  for i in range(n):
   o=pe+24+opt+40*i;vs,va,rs,raw=struct.unpack_from('<IIII',b,o+8)
   if va<=rva and rva+size<=va+rs:return b[raw+rva-va:raw+rva-va+size]
  raise ValueError(hex(rva))
 loads=[]
 for site in [0x4838C6,0xD54AE9,0x2F364F4]:
  op=data(site,7);assert op[:3]==b'\x48\x8b\x0d';target=site+7+struct.unpack_from('<i',op,3)[0]
  assert target==0x492D830
  loads.append({'instruction_rva':hex(site),'bytes':op.hex(' '),'world_pointer_slot_rva':hex(target)})
 assert data(0x4846E4,20)==bytes.fromhex('48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 70 20 55')
 assert struct.unpack('<Q',data(0x407FC80+0x118,8))[0]==0x18050A3E8
 header=read_source(R/'src/vrik/world_query_contract.h')
 actual=int(re.search(r'kWorldSlotRva\s*=\s*(0x[0-9A-Fa-f]+)',header).group(1),16)
 assert all(int(v['world_pointer_slot_rva'],16)==actual for v in loads),'source world slot disagrees with native instructions'
 return {'dll_sha256':h,'native_loads':loads,'world_vtable_rva':'0x407FC80','method_slot':'0x118','method_rva':'0x50A3E8','old_wrong_slot':'0x492B830'}
def mutant(out,name,which,old,new):
 with tempfile.TemporaryDirectory() as td:
  t=Path(td);(t/'tests').mkdir();(t/'src/vrik').mkdir(parents=True)
  for f in ['world_adapter_test.cpp','generated_world_adapter.inc','generated_adapter_types.h']:shutil.copy2(R/'tests'/f,t/'tests'/f)
  shutil.copy2(R/'src/vrik/world_query_contract.h',t/'src/vrik/world_query_contract.h')
  p=t/which;s=p.read_text();assert old in s;p.write_text(s.replace(old,new,1))
  run(['g++','-std=c++17','-O0',t/'tests/world_adapter_test.cpp','-o',t/'test'],out/(name+'_compile.log'))
  p=subprocess.run([str(t/'test')],capture_output=True,text=True);(out/(name+'_run.log')).write_text(p.stdout+p.stderr)
  assert p.returncode!=0,name+' escaped the tests'
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--whgame',required=True,type=Path);ap.add_argument('--build-dir',type=Path,default=R/'build-contracts-r1');a=ap.parse_args()
 out=R/'delivery/v6692r1';out.mkdir(parents=True,exist_ok=True)
 contract=native_contract(a.whgame);(out/'native_world_contract.json').write_text(json.dumps(contract,indent=2)+'\n')
 src=read_source(R/'src/mod.cpp');assert 'KCD2VR-V66.92R1-RUNTIME-CONTAINMENT' in src
 assert 'constexpr int f4_immersive_ui_rung=0;' in src and 'f4_immersive_ui_rung=(f4_' not in src
 block=src[src.index('// V66.92R1: F4 retirement'):src.index('f4_down_prev=f4_down;',src.index('// V66.92R1: F4 retirement'))]
 for forbidden in ['SendInput(', 'SetPhysicalMapMode(', 'RequestPhysicalMapReadingAnimation(', 'ApplyWristHudLayout(']:assert forbidden not in block
 assert 'vrik_auto_enable_after_cal=2;' in src and 'vrik_auto_enable_after_cal=6;' not in src
 assert 'vrik_menu_mode_requested_.store(enabled ? 2 : 0' in src
 frik=read_source(R/'src/frik_pose_probe.cpp')
 body=frik[frik.index('void __fastcall SkinningTransformCoreDetour'):]
 assert body.index('ReadTrackingSnapshot(poseSnapshot)')<body.index('const auto validationTick=')
 assert 'validationTick>=poseSnapshot.tick&&validationTick-poseSnapshot.tick<=100' in body
 assert 'nowTick>=poseSnapshot.tick&&nowTick-poseSnapshot.tick<=100' not in body
 # A sample published during callback preparation remains fresh, while stale/future samples do not.
 fresh=lambda sample,now: bool(sample and now>=sample and now-sample<=100)
 assert not fresh(102,100) and fresh(102,105) and not fresh(102,203) and not fresh(106,105)
 run(['cmake','-S',R/'tests','-B',a.build_dir],out/'configure.log')
 run(['cmake','--build',a.build_dir,'--parallel','4'],out/'build.log')
 text=run(['ctest','--test-dir',a.build_dir,'-V'],out/'ctest.log')
 counts=re.findall(r'\b(?:pose_math|pose_adapter|weapon_adapter|state_queue|playback_lab|combo_snapshot|world_adapter)=([0-9]+)/([0-9]+)',text)
 # Older test groups use a few different labels; store the complete ctest output,
 # do not use an invented assertion count as runtime proof.
 py={}
 for f in ['test_diagnostic_report.py','test_combo_report.py']:
  s=run([sys.executable,R/'tests'/f,'-v'],out/(f+'.log'));m=re.search(r'Ran (\d+) tests',s);assert m;py[f]=int(m.group(1))
 with tempfile.TemporaryDirectory() as td:
  t=Path(td);objs=[]
  for name in ['frik_pose_probe','animation_filter','combat_diag_probe','diagnostic_trace']:
   o=t/(name+'.o');run(['g++','-std=c++17','-O0','-Wformat=2','-Werror=format','-I'+str(R/'tests/syntax_shims'),'-I'+str(R/'src'),'-c',R/'src'/(name+'.cpp'),'-o',o],out/(name+'_host.log'));objs.append(o)
  (t/'main.cpp').write_text('int main(){return 0;}\n');run(['g++',*objs,t/'main.cpp','-o',t/'hostlink'],out/'host_link.log')
  run(['g++','-std=c++17','-O1','-fsanitize=address,undefined','-fno-omit-frame-pointer',R/'tests/world_adapter_test.cpp','-o',t/'sanitized'],out/'world_sanitizer_compile.log')
  run([t/'sanitized'],out/'world_sanitizer_run.log')
 mutant(out,'bad_world_slot','src/vrik/world_query_contract.h','kWorldSlotRva = 0x492D830ull','kWorldSlotRva = 0x492B830ull')
 mutant(out,'no_fault_circuit','src/vrik/world_query_contract.h','blocked_.compare_exchange_strong(expected,1,std::memory_order_acq_rel);','(void)expected;')
 mutant(out,'fault_as_clear','tests/generated_world_adapter.inc','__except(EXCEPTION_EXECUTE_HANDLER){return -1;}','__except(EXCEPTION_EXECUTE_HANDLER){return 0;}')
 report={'revision':'V66.92R1','native_contract':'3 independently decoded RCX world loads + helper prefix + physical-world virtual slot + pinned DLL hash/timestamp',
 'cpp_ctest_groups':7,'new_actual_extracted_world_adapter_checks':14,'python_tests':py,'compiled_fault_mutants_rejected':3,
 'host_shim_compile_link':'4 complete native-facing translation units; full mod.cpp NOT compiled',
 'sanitizer':'ASan+UBSan extracted world adapter with sparse-memory/native-call replacements',
 'native_windows_msvc':'NOT_RUN','windows_seh_runtime':'NOT_RUN','headset':'NOT_RUN','performance':'NOT_MEASURED',
 'wrist_orientation':'UNRESOLVED','finger_endpoint_arrays_from_user_run':'NOT_CAPTURED','visible_weapon':'NO_SUCCESS_IN_USER_RUN'}
 (out/'VERIFICATION.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':main()
