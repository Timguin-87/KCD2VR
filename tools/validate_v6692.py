#!/usr/bin/env python3
"""V66.92 source/host verifier. Does NOT prove native Windows injection/headset behavior."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse,hashlib,json,re,shutil,struct,subprocess,sys,tempfile
R=Path(__file__).resolve().parents[1]
def run(cmd,log=None,cwd=None):
    p=subprocess.run([str(x) for x in cmd],cwd=cwd or R,text=True,capture_output=True)
    text=p.stdout+p.stderr
    if log: Path(log).write_text(text)
    if p.returncode: raise RuntimeError(f"failed {cmd}\n{text[-4000:]}")
    return text
def pe_rva_bytes(path,rva,n):
    b=Path(path).read_bytes();pe=struct.unpack_from('<I',b,0x3c)[0]
    sections=struct.unpack_from('<H',b,pe+6)[0];opt=struct.unpack_from('<H',b,pe+20)[0]
    for i in range(sections):
        j=pe+24+opt+i*40
        vsize,va,rawsize,raw=struct.unpack_from('<IIII',b,j+8)
        if va<=rva<va+max(vsize,rawsize) and rva+n<=va+max(vsize,rawsize):
            off=raw+(rva-va);return b[off:off+n]
    raise ValueError(hex(rva))
def mutation_pose_adapter(name,find,repl,out):
    with tempfile.TemporaryDirectory() as td:
        t=Path(td);(t/'src/vrik').mkdir(parents=True);(t/'tests').mkdir()
        for f in ('pose_math.h','coherent_pose.inl'):shutil.copy2(R/'src/vrik'/f,t/'src/vrik'/f)
        for f in ('pose_adapter_test.cpp','generated_adapter_types.h'):shutil.copy2(R/'tests'/f,t/'tests'/f)
        p=t/'src/vrik/coherent_pose.inl';s=read_source(p,root=t);assert find in s;p.write_text(s.replace(find,repl,1))
        c=run(['g++','-std=c++17',t/'tests/pose_adapter_test.cpp','-o',t/'test'])
        x=subprocess.run([str(t/'test')],capture_output=True,text=True)
        (out/f'mutant_{name}.log').write_text(x.stdout+x.stderr)
        if x.returncode==0:raise AssertionError(name+' survived')
def mutation_pose_math(name,find,repl,out):
    with tempfile.TemporaryDirectory() as td:
        t=Path(td);(t/'src/vrik').mkdir(parents=True);(t/'tests').mkdir()
        shutil.copy2(R/'src/vrik/pose_math.h',t/'src/vrik/pose_math.h');shutil.copy2(R/'tests/pose_math_test.cpp',t/'tests/pose_math_test.cpp')
        p=t/'src/vrik/pose_math.h';s=read_source(p,root=t);assert find in s;p.write_text(s.replace(find,repl,1))
        run(['g++','-std=c++17',t/'tests/pose_math_test.cpp','-o',t/'test'])
        x=subprocess.run([str(t/'test')],capture_output=True,text=True);(out/f'mutant_{name}.log').write_text(x.stdout+x.stderr)
        if x.returncode==0:raise AssertionError(name+' survived')
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--whgame',type=Path,required=True);ap.add_argument('--build-dir',type=Path,default=R/'build-contracts-v6692');a=ap.parse_args()
    out=R/'delivery';out.mkdir(exist_ok=True)
    report={'build':'V66.92','native_windows_compile':'NOT_RUN','headset':'NOT_RUN','native_damage_injection':'NONE'}
    marker='KCD2VR-V66.92-VRIK-WORLD-COLLISION'
    assert marker in read_source(R/'src/mod.cpp') and marker in (R/'build_release.bat').read_text()
    preserved=json.loads((R/'continuity/V66.92_PRESERVED_SOURCE_SHA256.json').read_text())
    for rel,h in preserved.items(): assert hashlib.sha256((R/rel).read_bytes()).hexdigest()==h,rel
    report['preserved_v6691_source_files']=len(preserved)
    abi=json.loads((R/'continuity/native_v6692/WORLD_COLLISION_ABI.json').read_text())
    b=a.whgame.read_bytes();assert hashlib.sha256(b).hexdigest()==abi['whgame_sha256']
    pe=struct.unpack_from('<I',b,0x3c)[0];assert f"0x{struct.unpack_from('<I',b,pe+8)[0]:08X}"==abi['pe_timestamp']
    rva=int(abi['ray_world_intersection_rva'],16);prefix=bytes.fromhex(abi['ray_entry_prefix_hex'])
    assert pe_rva_bytes(a.whgame,rva,len(prefix))==prefix
    report['retail_world_collision_abi']='SHA/TIMESTAMP/RAY_PREFIX_PASS'
    src=read_source(R/'src/frik_pose_probe.cpp')
    for lit in ('0x492B800ull','0x4846E4ull','0x8002040Fu','0x101'): assert lit in src
    # contracts
    if a.build_dir.exists():shutil.rmtree(a.build_dir)
    run(['cmake','-S',R/'tests','-B',a.build_dir],out/'v6692_contracts_configure.log')
    run(['cmake','--build',a.build_dir,'--parallel','4'],out/'v6692_contracts_build.log')
    ctext=run(['ctest','--test-dir',a.build_dir,'--output-on-failure','-V'],out/'v6692_contracts_ctest.log')
    pairs=[(int(x),int(y)) for x,y in re.findall(r'=([0-9]+)/([0-9]+)',ctext)]
    assert len(pairs)==6 and all(x==y for x,y in pairs)
    report['cpp_test_groups']=6;report['cpp_assertions']=sum(x for x,y in pairs)
    # parser regressions
    py={}
    for f in ('test_diagnostic_report.py','test_combo_report.py'):
        t=run([sys.executable,R/'tests'/f,'-v'],out/(f+'.v6692.log'));m=re.search(r'Ran ([0-9]+) tests',t);assert m;py[f]=int(m.group(1))
    report['python_tests']=py
    # full native-facing host objects
    with tempfile.TemporaryDirectory() as td:
        t=Path(td);objs=[]
        for n in ('frik_pose_probe','animation_filter','combat_diag_probe','diagnostic_trace'):
            o=t/(n+'.o');run(['g++','-std=c++17','-O0','-Wformat=2','-Werror=format','-I'+str(R/'tests/syntax_shims'),'-I'+str(R/'src'),'-c',R/'src'/(n+'.cpp'),'-o',o],out/(n+'_v6692_host.log'));objs.append(o)
        (t/'main.cpp').write_text('int main(){return 0;}\n');run(['g++',*objs,t/'main.cpp','-o',t/'mock'],out/'v6692_host_link.log')
    report['host_shim']='4 full TUs compile/link; full mod.cpp NOT compiled; NOT MSVC'
    # sanitizer on portable pose math
    with tempfile.TemporaryDirectory() as td:
        exe=Path(td)/'asan';run(['g++','-std=c++17','-O1','-fsanitize=address,undefined','-fno-omit-frame-pointer',R/'tests/pose_math_test.cpp','-o',exe],out/'v6692_pose_sanitizer_compile.log');run([exe],out/'v6692_pose_sanitizer_run.log')
    report['pose_math_sanitizers']='ASAN_UBSAN_PASS'
    mutation_pose_adapter('collision_bypass','goals[side]=s.allowedGoal[side];','goals[side]=requestedGoals[side];',out)
    mutation_pose_math('collision_clamp_bypass','blocked=true; const double d=std::max(0.0,*hit-skin);','blocked=false; const double d=n;',out)
    mutation_pose_math('axis_weight_loss','v.z*weight.z','v.z*weight.x',out)
    report['mutations_rejected']=3
    (out/'VERIFICATION_V66.92.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))
if __name__=='__main__':
    main()
