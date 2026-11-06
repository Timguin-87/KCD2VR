#!/usr/bin/env python3
"""Current source behavioral/static checks. Native API replacements are not KCD2 execution."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse, hashlib, json, os, re, shutil, struct, subprocess, sys, tempfile
from validate_v6692r1 import native_contract as world_contract
R=Path(__file__).resolve().parents[1]

def run(cmd, log, cwd=None):
    result=subprocess.run([str(x) for x in cmd],cwd=cwd or R,capture_output=True,text=True)
    text=result.stdout+result.stderr;Path(log).write_text(text,encoding='utf8')
    if result.returncode:raise RuntimeError(f"{cmd} failed ({result.returncode}); {log}\n{text[-3000:]}")
    return text

def native_contract(dll):
    world=world_contract(dll);b=dll.read_bytes();pe=struct.unpack_from('<I',b,0x3c)[0]
    n=struct.unpack_from('<H',b,pe+6)[0];opt=struct.unpack_from('<H',b,pe+20)[0];base=struct.unpack_from('<Q',b,pe+24+24)[0]
    def data(rva,size):
        for i in range(n):
            o=pe+24+opt+40*i;vs,va,rs,raw=struct.unpack_from('<IIII',b,o+8)
            if va<=rva and rva+size<=va+rs:return b[raw+rva-va:raw+rva-va+size]
        raise ValueError(hex(rva))
    slots=[]
    for vt,slot,fn in [(0x406BB38,0x30,0xAA805C),(0x406BB38,0x38,0x37A0554),(0x406BB38,0xB8,0x37A2150),
                       (0x4077048,0x70,0x4B4F98),(0x4072C78,0x168,0x509810),(0x40602F8,0x190,0x423C30)]:
        assert struct.unpack('<Q',data(vt+slot,8))[0]-base==fn
        slots.append({'vtable':hex(vt),'slot':hex(slot),'function':hex(fn)})
    assert data(0x82D014,19)==bytes.fromhex('48 8b 91 88 00 00 00 33 c0 48 85 d2 75 01 c3 8b 42 30 c3')
    # Native puppet entity lookup loads the entity-system singleton then dispatches +0x70.
    load=0x82B7B8+10;instruction=data(load,7);assert instruction[:3]==bytes.fromhex('48 8b 3d')
    assert load+7+struct.unpack_from('<i',instruction,3)[0]==0x492D8A0
    assert bytes.fromhex('48 8b 58 70') in data(0x82B7B8,55)
    # The name->ID path computes the exact case-folded native alias key.
    block=data(0xAA805C,70);i=block.index(bytes.fromhex('e8 86 af bf ff'))
    assert 0xAA805C+i+5+struct.unpack_from('<i',block,i+1)[0]==0x6A3014
    assert data(0x6A3014,10)==bytes.fromhex('48 89 5c 24 08 89 54 24 10 55')
    sampler=data(0x37A2150,270)
    for expected in ['48 8b 5c 24 60','41 8b f1','0f 28 f2','48 69 f8 a8 00 00 00','f7 47 18 00 08 00 00','ff 50 28']:
        assert bytes.fromhex(expected) in sampler
    code={p.relative_to(R).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in (R/'src').rglob('*') if p.is_file()}
    return {'world':world,'item_id_getter':{'rva':'0x82D014','bytes':data(0x82D014,19).hex(' '),'linked_record':'+0x88','entity_id':'+0x30'},
            'entity_system_global':'0x492D8A0','verified_slots':slots,'alias_hash_call':'AA805C -> 6A3014',
            'sampler_register_contract':'set RCX; id EDX; phase XMM2; joint CRC R9D; QuatT output stack fifth parameter',
            'current_source_sha256':code,'native_runtime':'NOT_EXECUTED'}

def mutation(out, name, file, old, new, test):
    with tempfile.TemporaryDirectory() as td:
        t=Path(td)
        shutil.copytree(R/'tests',t/'tests',ignore=shutil.ignore_patterns('__pycache__'))
        shutil.copytree(R/'src/vrik',t/'src/vrik')
        shutil.copytree(R/'src/diagnostics',t/'src/diagnostics')
        p=t/file;s=p.read_text();assert old in s,(name,old);p.write_text(s.replace(old,new,1))
        run(['g++','-std=c++17','-O0','-pthread',t/'tests'/test,'-o',t/'test'],out/(name+'_compile.log'))
        result=subprocess.run([str(t/'test')],cwd=t,capture_output=True,text=True)
        (out/(name+'_run.log')).write_text(result.stdout+result.stderr)
        assert result.returncode==1 and 'FAIL' in result.stdout,f'{name}: not a clean behavioral rejection ({result.returncode})'

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--whgame',type=Path,required=True);ap.add_argument('--build-dir',type=Path,default=R/'build-contracts-v6694');a=ap.parse_args()
    out=R/'delivery/v6694';out.mkdir(parents=True,exist_ok=True)
    nc=native_contract(a.whgame);(out/'NATIVE_CONTRACTS.json').write_text(json.dumps(nc,indent=2)+'\n')
    frik=read_source(R/'src/frik_pose_probe.cpp');mod=read_source(R/'src/mod.cpp');body=read_source(R/'src/vrik/coherent_pose.inl')
    assert 'KCD2VR-V66.94-CONTROLLER-CORE' in mod and 'constexpr int f4_immersive_ui_rung=0;' in mod
    assert 'FindCEntityForItemObjectV6679' not in frik
    writer=frik[frik.index('static bool ControllerWeaponTransform6694'):frik.index('static void WeaponControlTickImpl')]
    for forbidden in ['g_f12_visible_owner_proven','proofUntil','fingersReady','ReadPoseReferenceState','FindCEntityForItemObject']:assert forbidden not in writer
    assert 'mode==3' in writer and 'ReadWeaponTracking6694(raw)' in writer
    assert 'ServiceFingerReferenceIo6694();' in frik and 'WeaponControlTick(); // bounded' in frik
    assert 'const bool full=rung==3;' in body and 'const bool collisionMode=false;' in body
    assert 'ReadPoseReferenceState(' not in body and 'AcquireCurveReferences6694' in body
    assert 'validationTick>=poseSnapshot.tick&&validationTick-poseSnapshot.tick<=100' in frik
    run(['cmake','-S',R/'tests','-B',a.build_dir],out/'configure.log')
    run(['cmake','--build',a.build_dir,'--parallel','4'],out/'build.log')
    text=run(['ctest','--test-dir',a.build_dir,'-V'],out/'ctest.log')
    counts={k:[int(v),int(n)] for k,v,n in re.findall(r'\b(active_math|active_adapter|controller_adapter|state_queue|playback_and_lab|combo_snapshot|world_adapter)=(\d+)/(\d+)',text)}
    assert len(counts)==7 and all(a==b for a,b in counts.values())
    py={}
    for test in ['test_diagnostic_report.py','test_combo_report.py','test_controller_report.py']:
        s=run([sys.executable,R/'tests'/test,'-v'],out/(test+'.log'));m=re.search(r'Ran (\d+) tests',s);assert m;py[test]=int(m.group(1))
    with tempfile.TemporaryDirectory() as td:
        t=Path(td);objs=[]
        for name in ['frik_pose_probe','animation_filter','combat_diag_probe','diagnostic_trace']:
            obj=t/(name+'.o');run(['g++','-std=c++17','-O0','-Wformat=2','-Werror=format','-I'+str(R/'tests/syntax_shims'),'-I'+str(R/'src'),'-c',R/'src'/(name+'.cpp'),'-o',obj],out/(name+'_host.log'));objs.append(obj)
        (t/'main.cpp').write_text('int main(){return 0;}\n');run(['g++',*objs,t/'main.cpp','-o',t/'hostlink'],out/'host_link.log')
        for name in ['pose_math','pose_adapter','controller_adapter','world_adapter']:
            run(['g++','-std=c++17','-O1','-pthread','-fsanitize=address,undefined','-fno-omit-frame-pointer','-no-pie',R/'tests'/(name+'_test.cpp'),'-o',t/name],out/(name+'_sanitizer_compile.log'))
            run([t/name],out/(name+'_sanitizer_run.log'),t)
    mutation(out,'bad_item_link','tests/generated_controller_adapter.inc','+0x88,link','+0x80,link','controller_adapter_test.cpp')
    mutation(out,'body_required_for_weapon_only','tests/generated_controller_adapter.inc','if(!ReadWeaponTracking6694(raw))return false;','if(!ReadWeaponTracking6694(raw)||g_cal_state.load()!=2)return false;','controller_adapter_test.cpp')
    mutation(out,'old_extension_reserve','src/vrik/pose_math.h','double reserve=-1.0','double reserve=0.008','pose_math_test.cpp')
    mutation(out,'finger_inputs_swapped','src/vrik/pose_math.h','finger==Finger::Index?trigger:grip','finger==Finger::Index?grip:trigger','pose_adapter_test.cpp')
    report={'revision':'V66.94','marker':'KCD2VR-V66.94-CONTROLLER-CORE','ctest_groups':7,'assertions':counts,
      'total_cpp_assertions':sum(x[0] for x in counts.values()),'python_tests':py,'total_python_tests':sum(py.values()),
      'native_contract':'pinned retail hash/timestamp; native linked-ID getter; entity-system load/vslot; sampler/alias slots and native hashing call; retained world-callsite checks',
      'sanitizers':'ASan+UBSan: portable math and current pose/controller/world adapters with native replacements',
      'clean_behavioral_mutants_rejected':['bad_item_link','body_required_for_weapon_only','old_extension_reserve','finger_inputs_swapped'],
      'host_compiled_linked':'4 complete native-facing TUs using explicit Windows/DetourModKit replacements',
      'full_windows_mod_cpp':'NOT_COMPILED','msvc':'NOT_RUN','windows_seh':'NOT_RUN','game_injection':'NOT_RUN','headset':'NOT_RUN','fps':'NOT_MEASURED',
      'reference_data':'Native curve data path implemented; resident assets, additive/base composition and visible mesh still require runtime acceptance; no fabricated final endpoint arrays bundled',
      'weapon':'Actual raw-controller path implemented; identity/lifetime/readback mocks are not visible sword proof',
      'physics':'No environment query requested by normal F10 ladder; experimental backend retained outside active ladder'}
    (out/'VERIFICATION.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':main()
