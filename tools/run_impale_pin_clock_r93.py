"""Replay actual pin consumer/weapon/coordinator with independent sample and consume clocks."""
from pathlib import Path
import argparse, hashlib, json, os, re, subprocess
from host_compiler import command, executable_path
ROOT=Path(__file__).resolve().parents[1]

def extract(source):
    text=(source/'src/subsystems/base/pose_physical_executor.inl').read_text(encoding='utf-8')
    start=text.index('        {ImpalePin178r15 pin{};')
    return text[start:text.index('        if(pi.impaleEnabled178r15){const auto rq26=',start)]

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--out',type=Path,required=True);ap.add_argument('--baseline',type=Path);ap.add_argument('--compiler',default=None);ap.add_argument('--only',default='');args=ap.parse_args()
    out=args.out.resolve();assert out!=ROOT and ROOT not in out.parents;out.mkdir(parents=True,exist_ok=True)
    rows=[]
    only=set(args.only.split(',')) if args.only else set()
    def run(name,fixture,pin=None,expected_failure=False,sanitize=False):
        if only and name not in only:return
        folder=out/name;folder.mkdir(exist_ok=True)
        if pin is not None:
            (folder/'actual_impale_pin_r93.inc').write_text(pin,encoding='utf-8')
            producer=(ROOT/'src/subsystems/base/pose_physical_executor.inl').read_text(encoding='utf-8')
            a=producer.index('        pi.impaleHang32=');b=producer.index('        if(pi.impaleEnabled178r15)',a)
            (folder/'actual_impale_options_r93.inc').write_text(producer[a:b],encoding='utf-8')
        source=ROOT/'tests'/fixture
        if fixture in ('impale_native_handoff59_test.cpp','impale_successor_h18_coordinator_test.cpp'):
            source=folder/'fixture.cpp'
            source.write_text('#include "impale_native_eligibility_r93_fixture.h"\n#include "'+(ROOT/'tests'/fixture).as_posix()+'"\n')
        exe=executable_path(folder/'test');cmd=command(source,exe,[ROOT/'src',ROOT/'tests',folder],args.compiler)
        msvc=Path(cmd[0]).name.lower() in ('cl','cl.exe')
        if msvc:
            cmd=[('/Od' if option=='/O2' else option) for option in cmd]
            cmd+=['/link','/STACK:8388608']
        if sanitize:
            if msvc:cmd.insert(1,'/fsanitize=address')
            else:cmd[1:1]=['-fsanitize=address,undefined','-fno-omit-frame-pointer']
        built=subprocess.run(cmd,capture_output=True,text=True);(folder/'build.log').write_text(built.stdout+built.stderr)
        row=dict(test=name,compile_rc=built.returncode,command=cmd,expected_failure=expected_failure,passed=False)
        if not built.returncode:
            result=subprocess.run([str(exe)],capture_output=True,text=True);(folder/'run.log').write_text(result.stdout+result.stderr)
            row.update(run_rc=result.returncode,output=result.stdout+result.stderr)
            row['passed']=(result.returncode==1 and 'FAIL fresh pin newer than tracking sample' in result.stdout and re.search(r'R93 impale pin clock: \d+ checks, [1-9]\d* failures',result.stdout) is not None and not result.stderr.strip()) if expected_failure else result.returncode==0
        rows.append(row);print(name+': '+('PASS' if row['passed'] else 'FAIL'),flush=True)
    pin=extract(ROOT)
    # Literal combat-OFF branch and normal N2/coordinator call sites. Unrelated
    # combat work is omitted, not reimplemented or enabled by this fixture.
    host=(ROOT/'src/frik_pose_probe.cpp').read_text();start=host.index('static void NativeRhythmTick178()');gate=host.index('    if(!PhysicalCombat177Enabled())',start);end=host.index('    ReadScope101R2 reads;',gate)
    normal=host[end:host.index('void RequestGoreVisualRestore59()',end)]
    calls=[line for line in normal.splitlines() if 'CorpseImpaleProducerR93();' in line or 'ImpalePartTick59c7g();' in line or 'lab58n::Tick(' in line]
    assert len(calls)==3
    outer='static void actualOuterR93(){\n'+host[gate:end]+'\n'.join(calls)+'\n}\n'
    for name in ('outer','outer_asan'):
        folder=out/name;folder.mkdir(exist_ok=True);(folder/'actual_impale_outer_r93.inc').write_text(outer)
    run('outer','impale_outer_r93_test.cpp',pin)
    run('outer_asan','impale_outer_r93_test.cpp',pin,sanitize=True)
    run('current','impale_pin_clock_r93_test.cpp',pin)
    run('current_asan','impale_pin_clock_r93_test.cpp',pin,sanitize=True)
    if args.baseline:run('baseline_clock_negative','impale_pin_clock_r93_test.cpp',extract(args.baseline.resolve()),True)
    for name in ['impale_hang44','impale40_weapon','impale_native_handoff59','impale_successor_h18_coordinator','impale_constraint_h21','impale_corpse_r15','impale_pending59','impale47']:
        run(name,name+'_test.cpp')
    paths=['src/frik_pose_probe.cpp','src/subsystems/gore/corpse_impale_producer_r93.inl','tests/impale_outer_r93_test.cpp','src/subsystems/base/pose_physical_executor.inl','src/vrik/physical_weapon.h','src/subsystems/gore/native_impale_part.inl','tests/impale_pin_clock_r93_test.cpp']
    report=dict(passed=all(r['passed'] for r in rows),results=rows,sha256={p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},limits=['Native memory/body/status and PartVelocity are fixtures; no game/headset claim.','Current-part velocity servo is tested at F9 0/1/2/3; true rigid sword-to-corpse constraint remains separately gated and unavailable.','Baseline negative uses verbatim baseline pin consumer against current physical/coordinator fixture.'])
    (out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n');return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
