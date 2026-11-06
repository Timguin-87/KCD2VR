#!/usr/bin/env python3
"""Reproduce host-only checks. Requires CMake, Python3 and g++ on a POSIX host.
Does not compile the Windows mod.cpp translation unit or validate engine execution.
Use build_release.bat on Windows for the actual ASI build.
"""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse, hashlib, json, re, struct, subprocess, sys, tempfile
from validate_v6694 import native_contract as inherited_native_contract
R=Path(__file__).resolve().parents[1]

def run(cmd,log,cwd=None):
    p=subprocess.run([str(x) for x in cmd],cwd=cwd or R,capture_output=True,text=True)
    text=p.stdout+p.stderr;log.write_text(text,encoding='utf-8')
    if p.returncode:raise RuntimeError(f'Command failed ({p.returncode}), see {log}\n{text[-2000:]}')
    return text

def source_hashes():
    return {p.relative_to(R).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted((R/'src').rglob('*')) if p.is_file()}

def native_contract(dll):
    result=inherited_native_contract(dll)
    b=dll.read_bytes();pe=struct.unpack_from('<I',b,60)[0];n=struct.unpack_from('<H',b,pe+6)[0];opt=struct.unpack_from('<H',b,pe+20)[0];base=struct.unpack_from('<Q',b,pe+48)[0]
    sec=[struct.unpack_from('<IIII',b,pe+24+opt+40*i+8) for i in range(n)]
    def data(rva,size):
        for vs,va,rs,raw in sec:
            if va<=rva and rva+size<=va+rs:return b[raw+rva-va:raw+rva-va+size]
        raise ValueError(hex(rva))
    def u(rva):return struct.unpack('<I',data(rva,4))[0]
    def name(rva):return data(rva,220).split(b'\0')[0].decode('ascii')
    def rtti(vt):
        col=struct.unpack('<Q',data(vt-8,8))[0]-base;ch=u(col+16);arr=u(ch+12)
        out={'vtable':hex(vt),'name':name(u(col+12)+16),'complete_object_offset':u(col+4),'bases':[]}
        for i in range(min(u(ch+8),50)):
            bc=u(arr+i*4);out['bases'].append({'name':name(u(bc)+16),'member_displacement':struct.unpack('<i',data(bc+8,4))[0]})
        return out
    result['new_exact_types']=[rtti(v) for v in [0x3A80610,0x3E3FAC8,0x3EC9958,0x3AA4B98]]
    x=result['new_exact_types'][0]
    assert x['name']=='.?AVC_CombatPlayer@combatmodule@wh@@'
    assert any(y['name']=='.?AVC_CombatActor@combatmodule@wh@@' and y['member_displacement']==0 for y in x['bases'])
    result['lifecycle_static_windows']={hex(rva):data(rva,size).hex(' ') for rva,size in [(0x9D34E8,64),(0x8D0758,64),(0x8D06F0,104)]}
    # Pinned bytes plus retained disassembly are evidence for the small native hand-key chain.
    assert bytes.fromhex('48 8b 41 38') in data(0x9D34E8,64)
    assert bytes.fromhex('90 09 00 00') in data(0x9D34E8,64)
    result['normal_field_evidence']='Hit point +0x24 is present in native query code; normal +0x30 is a matching x64 Cry ray_hit layout candidate, checked at runtime for unit length and point/distance consistency. Independent retail normal-writer proof is not claimed.'
    return result

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--whgame',type=Path)
    ap.add_argument('--build-dir',type=Path,default=R/'build-contracts-v6697')
    ap.add_argument('--out',type=Path,default=R/'delivery/v6697')
    ap.add_argument('--sanitizers',action='store_true')
    a=ap.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
    mod=read_source(R/'src/mod.cpp');assert 'KCD2VR-V66.97-EMBODIMENT-FOUNDATION' in mod
    assert 'constexpr int f4_immersive_ui_rung=0;' in mod
    start_hashes=source_hashes()
    if a.whgame:(out/'NATIVE_CONTRACTS.json').write_text(json.dumps(native_contract(a.whgame),indent=2)+'\n')
    run(['cmake','-S',R/'tests','-B',a.build_dir],out/'configure.log')
    run(['cmake','--build',a.build_dir,'--parallel','4'],out/'build.log')
    text=run(['ctest','--test-dir',a.build_dir,'-V'],out/'ctest.log')
    counts={k:[int(v),int(n)] for k,v,n in re.findall(r'\b(active_math|active_adapter|controller_adapter|state_queue|playback_and_lab|combo_snapshot|world_adapter|snapshot_mailbox|elbow_continuity|trigger_ownership)=(\d+)/(\d+)',text)}
    assert len(counts)==10 and all(x==y for x,y in counts.values())
    py={}
    for test in ['test_diagnostic_report.py','test_combo_report.py','test_controller_report.py','test_continuity_report.py','test_finger_reference_report.py']:
        t=run([sys.executable,R/'tests'/test,'-v'],out/(test+'.log'))
        m=re.search(r'Ran (\d+) tests',t);assert m and 'OK' in t;py[test]=int(m.group(1))
    with tempfile.TemporaryDirectory() as td:
        t=Path(td);objs=[]
        for name in ['frik_pose_probe','animation_filter','combat_diag_probe','diagnostic_trace']:
            obj=t/(name+'.o');run(['g++','-std=c++17','-O0','-Wformat=2','-Werror=format','-I'+str(R/'tests/syntax_shims'),'-I'+str(R/'src'),'-c',R/'src'/(name+'.cpp'),'-o',obj],out/(name+'_host.log'));objs.append(obj)
        (t/'main.cpp').write_text('int main(){return 0;}\n');run(['g++',*objs,t/'main.cpp','-o',t/'hostlink'],out/'host_link.log')
        sanitized=[]
        if a.sanitizers:
            for name in ['pose_math','pose_adapter','controller_adapter','world_adapter','snapshot_mailbox','elbow_continuity','trigger_ownership','combo_snapshot']:
                run(['g++','-std=c++17','-O1','-pthread','-fsanitize=address,undefined','-fno-omit-frame-pointer','-no-pie',R/'tests'/(name+'_test.cpp'),'-o',t/name],out/(name+'_sanitizer_compile.log'))
                run([t/name],out/(name+'_sanitizer_run.log'),t);sanitized.append(name)
    assert source_hashes()==start_hashes,'Source changed during verification'
    report={'revision':'V66.97','ctest_groups':10,'cpp_assertions':counts,'total_cpp_assertions':sum(v[0] for v in counts.values()),'python_tests':py,'total_python_tests':sum(py.values()),
      'host_compile_link':'Four complete native-facing translation units with explicit Windows/DetourModKit/native replacements; not a Windows ABI validation',
      'sanitizers':sanitized,'full_mod_cpp':'NOT_COMPILED','msvc':'NOT_RUN','windows_seh':'NOT_RUN','headset':'NOT_RUN','game_injection':'NOT_RUN','performance':'NOT_MEASURED',
      'fingers':'New whole-authored-loop medoid selection and generated-reference audit. Neutral alias remains relaxed_idle_both_player. Mesh-safe maximum/natural relaxed appearance NOT_PROVEN.',
      'sheath':'Exact live hand-key gate implemented. Full live draw/sheath event sequence NOT_TESTED; no complete DRAWING/SHEATHING phase enum.',
      'contact':'Five anatomical feature rays, not convex rigid-body hands. One query pass per fresh XR sample; up to18 rays and checked1000us budget retained. More query passes per second are possible than the old16ms limiter. Initial overlap, concave gaps, native scheduling and mesh culling remain unproven.',
      'physical_combat_bow_grab_support_hand':'NOT_IMPLEMENTED', 'source_sha256':start_hashes}
    (out/'VERIFICATION.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k!='source_sha256'},indent=2))
if __name__=='__main__':main()
