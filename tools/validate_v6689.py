#!/usr/bin/env python3
"""Reproducible V66.89 checks. Uses actual adapter source; no claim of MSVC/headset validation."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse, hashlib, json, re, shutil, struct, subprocess, sys, tempfile
ROOT=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def function(text,name):
    at=text.index(name+'(');start=text.rfind('\nstatic ',0,at)+1;op=text.index('{',at);level=1;k=op+1
    while level:level+=(text[k]=='{')-(text[k]=='}');k+=1
    return text[start:k]
def generated():
    s=read_source(ROOT/'src/frik_pose_probe.cpp')
    start=s.index('struct V3 {');op=s.index('{',s.index('static QuatT28 AbsoluteFromRelative'));level=1;k=op+1
    while level:level+=(s[k]=='{')-(s[k]=='}');k+=1
    rig=s[s.index('struct RigMap {'):s.index('\n};',s.index('struct RigMap {'))+3]
    types='// Generated from current src/frik_pose_probe.cpp by tools/validate_v6689.py; not hand-maintained.\n'+rig+'\n'+s[start:k]+'\n'
    a=s.index('struct AttachmentState6689');b=s.index('static bool ReadAttachmentQs6689',a)
    weapon='// Extracted from active frik_pose_probe.cpp; regenerated/checked by validator.\n'+s[a:b]+'\n'.join(function(s,n) for n in ['PlaceBothConsumers6689','PlacementReadback6689','FollowTransform6689','EntityAttachmentUpdateDetourV6688'])
    return {'tests/generated_adapter_types.h':types,'tests/generated_weapon_adapter.h':weapon}
def execute(cmd,log):
    print('RUN',' '.join(map(str,cmd)),flush=True)
    r=subprocess.run(list(map(str,cmd)),stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
    log.write_text(r.stdout);print(r.stdout,end='')
    if r.returncode:raise RuntimeError(f'Command failed ({r.returncode}): {cmd[0]}')
    return r.stdout

def native_check(path):
    info=json.loads((ROOT/'continuity/NATIVE_ABI.json').read_text());data=path.read_bytes()
    if hashlib.sha256(data).hexdigest()!=info['sha256']:raise RuntimeError('WHGame SHA256 mismatch')
    pe=struct.unpack_from('<I',data,0x3c)[0];num=struct.unpack_from('<H',data,pe+6)[0];size=struct.unpack_from('<H',data,pe+20)[0];base=struct.unpack_from('<Q',data,pe+24+24)[0];sections=[]
    if struct.unpack_from('<I',data,pe+8)[0]!=info['timestamp']:raise RuntimeError('PE timestamp mismatch')
    for i in range(num):
        o=pe+24+size+i*40;vs,va,rs,ptr=struct.unpack_from('<IIII',data,o+8);sections.append((va,max(vs,rs),ptr))
    def read(rva,n):
        for va,sz,ptr in sections:
            if va<=rva<va+sz:return data[ptr+rva-va:ptr+rva-va+n]
        raise RuntimeError(f'Unmapped RVA {rva:x}')
    for rva,expected in info['entry_bytes'].items():
        if read(int(rva,16),len(expected)//2).hex()!=expected:raise RuntimeError('Entry mismatch '+rva)
    for expression,target in info['vtable_entries'].items():
        rva=sum(int(x,16) for x in expression.split('+'))
        if struct.unpack('<Q',read(rva,8))[0]!=base+int(target,16):raise RuntimeError('Slot mismatch '+expression)
    return {'sha256':'PASS','timestamp':'PASS','function_entries':len(info['entry_bytes']),'vtable_entries':len(info['vtable_entries']),'runtime':'NOT_TESTED'}

def mutation_checks(out):
    # Mutants are generated in a temporary copy of tests + active headers, never in the release tree.
    jobs=[('mutable_arm_length','src/vrik/coherent_pose.inl','s.upper[side],s.fore[side],PV(pole)','double(Len(Sub(FromT(s.original[f]),FromT(s.work[u])))),s.fore[side],PV(pole)','pose_adapter'),
          ('live_attack_elbow','src/vrik/coherent_pose.inl','s.fore[side],PV(pole),PV(s.lastBend[side])','s.fore[side],PV(Sub(FromT(s.original[f]),FromT(s.work[u]))),PV(s.lastBend[side])','pose_adapter'),
          ('grip_closes_index','src/vrik/pose_math.h','finger==Finger::Index?trigger:grip','grip','pose_math'),
          ('changing_finger_length','src/vrik/pose_math.h','LocalPose{*q,offset}','LocalPose{*q,offset*(1-0.25*curl)}','pose_math')]
    results=[]
    for name,file,old,new,test in jobs:
        with tempfile.TemporaryDirectory(prefix='kcd2vr_mutation_') as td:
            t=Path(td);shutil.copytree(ROOT/'tests',t/'tests');shutil.copytree(ROOT/'src/vrik',t/'src/vrik');shutil.copytree(ROOT/'src/core',t/'src/core')
            p=t/file;s=p.read_text()
            if old not in s:raise RuntimeError('Mutation anchor missing: '+name)
            p.write_text(s.replace(old,new,1));binary=t/'test'
            r=subprocess.run(['g++','-std=c++17','-O1',str(t/'tests'/f'{test}_test.cpp'),'-o',str(binary)],capture_output=True,text=True)
            if r.returncode:raise RuntimeError('Mutation must compile, not merely cause syntax failure: '+name+'\n'+r.stderr)
            run=subprocess.run([str(binary)],capture_output=True,text=True);(out/f'mutant_{name}.log').write_text(run.stdout+run.stderr)
            killed=run.returncode!=0;results.append({'mutant':name,'compiled':True,'tests_rejected':killed});print('MUTANT',name,'REJECTED' if killed else 'SURVIVED',flush=True)
            if not killed:raise RuntimeError('Regression test allowed bad algorithm: '+name)
    return results

def main():
    p=argparse.ArgumentParser();p.add_argument('--whgame',type=Path);p.add_argument('--build-dir',type=Path,default=ROOT/'build-contracts');p.add_argument('--refresh-generated',action='store_true');p.add_argument('--skip-host-shims',action='store_true');p.add_argument('--skip-mutations',action='store_true');args=p.parse_args()
    out=ROOT/'delivery';out.mkdir(exist_ok=True);report={'build':'V66.89','native_windows_compilation':'NOT_RUN','headset':'NOT_RUN'}
    for name,expected in generated().items():
        path=ROOT/name
        if args.refresh_generated:path.write_text(expected)
        elif path.read_text()!=expected:raise RuntimeError('Stale extracted test adapter: '+name+'; regenerate intentionally, then rerun tests')
    frozen=json.loads((ROOT/'continuity/FROZEN_SOURCE_SHA256.json').read_text())
    for name,digest in frozen.items():
        if sha(ROOT/name)!=digest:raise RuntimeError('Out-of-scope source change: '+name)
    report['frozen_source_files']=len(frozen)
    fr=read_source(ROOT/'src/frik_pose_probe.cpp');core=read_source(ROOT/'src/vrik/coherent_pose.inl');anim=read_source(ROOT/'src/animation_filter.cpp')
    for bad in ['SolveTwoBonePositions(', 'ApplyUpperBodyFoundationV6583(', 'ApplyGripCurlV6546(', 'g_f12_visible_owner_proven']:
        if bad in fr:raise RuntimeError('Retired production implementation returned: '+bad)
    if 'NativeInteractionHandOffActive() noexcept { return false;' not in anim:raise RuntimeError('Native interaction timer ownership returned')
    if 'if(mode>=2 && !CalibrationReady())' not in fr:raise RuntimeError('Rung1 must not require calibration')
    if 'vrik_auto_enable_after_cal=6' not in read_source(ROOT/'src/mod.cpp'):raise RuntimeError('Calibration auto-activation lost')
    if 'if(pre' in core and 'PublishF12HandDeltaV6557' in core:raise RuntimeError('Premature hand publication')
    marker='KCD2VR-V66.89-COHERENT-BODY-WEAPON'
    if marker not in (ROOT/'build_release.bat').read_text() or marker not in read_source(ROOT/'src/mod.cpp'):raise RuntimeError('Build-script/source version marker disagreement')
    if args.whgame:report['retail_static']=native_check(args.whgame)
    else:report['retail_static']='NOT_RUN (supply --whgame)'
    execute(['cmake','-S',ROOT/'tests','-B',args.build_dir,'-DCMAKE_BUILD_TYPE=Release'],out/'contracts_configure.log')
    execute(['cmake','--build',args.build_dir,'--config','Release','--parallel','4'],out/'contracts_build.log')
    execute(['ctest','--test-dir',args.build_dir,'-C','Release','--output-on-failure','-V'],out/'contracts_ctest.log');report['contract_groups']=4
    if not args.skip_host_shims:
        cc=shutil.which('g++')
        if not cc:raise RuntimeError('g++ missing; use --skip-host-shims and a real Windows compiler separately')
        objects=[]
        with tempfile.TemporaryDirectory(prefix='kcd2vr_host_compile_') as td:
            t=Path(td)
            for name in ['frik_pose_probe','animation_filter','combat_diag_probe','diagnostic_trace']:
                obj=t/(name+'.o');execute([cc,'-std=c++17','-O0','-I'+str(ROOT/'tests/syntax_shims'),'-I'+str(ROOT/'src'),'-c',ROOT/'src'/f'{name}.cpp','-o',obj],out/f'{name}_host_object.log');objects.append(obj)
            (t/'main.cpp').write_text('int main(){return 0;}\n');execute([cc,*objects,t/'main.cpp','-o',t/'mock_link'],out/'host_link.log')
        report['host_shim_compile_and_link']='4 complete changed translation units; mock Windows/DetourModKit, NOT native MSVC; mod.cpp not fully compiled'
    else:report['host_shim_compile_and_link']='SKIPPED'
    report['mutations']=mutation_checks(out) if not args.skip_mutations else 'SKIPPED'
    (out/'VERIFICATION.json').write_text(json.dumps(report,indent=2)+'\n');print('V66.89 checks passed. Native Windows/headset validation remains NOT RUN.')
if __name__=='__main__':
    try:main()
    except Exception as e:print('FAIL:',e,file=sys.stderr);sys.exit(1)
