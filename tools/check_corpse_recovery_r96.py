"""Execute actual corpse/load/physics adapters with explicit native leaves."""
from pathlib import Path
import argparse, hashlib, json, subprocess, sys
from host_compiler import command, executable_path

ROOT=Path(__file__).resolve().parents[1]
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--out',required=True,type=Path)
    parser.add_argument('--compiler');args=parser.parse_args();out=args.out.resolve()
    if out==ROOT or ROOT in out.parents:parser.error('--out must be outside the source tree')
    out.mkdir(parents=True,exist_ok=True)
    subprocess.run([sys.executable,str(ROOT/'tools/extract_corpse_drag_d40.py'),str(out)],check=True)
    source=(ROOT/'src/subsystems/gore/native_gore.inl').read_text()
    actual=source[source.index('using ActionFn32='):source.index('// DS-F91 (Claude CL-IMPALE60):')]
    (out/'corpse_velocity_r96.inc').write_text(actual)
    subprocess.run([sys.executable,str(ROOT/'tools/extract_corpse_contact_h7.py'),str(out/'corpse_contact_native_h7.inc')],check=True)
    part=(ROOT/'src/subsystems/gore/native_part_anchor.inl').read_text()
    a=part.index('static bool PartStatus47(');b=part.index('// A NEW post-death contact',a)
    selected=part[a:b]
    a=part.index('struct AnchorPart47');b=part.index('// H18 successor recontact:',a)
    selected+=part[a:b]+part[part.index('// R95: saved-corpse Grip'):]
    (out/'saved_part_native_r95.inc').write_text(selected)
    rows=[]
    for name in ('corpse_fault_recovery_r96','corpse_contact_native_h7','saved_corpse_part_r95',
                 'saved_corpse_native_d72','corpse_drag_native_d40',
                 'native_physics_phase_t4','corpse_grip_cg1','corpse_body_owner_d40','part_anchor59c7g','lab58'):
        exe=executable_path(out/name);cmd=command(ROOT/'tests'/f'{name}_test.cpp',exe,[ROOT/'src',ROOT/'tests',out],args.compiler)
        build=subprocess.run(cmd,text=True,capture_output=True);(out/f'{name}.build.txt').write_text(build.stdout+build.stderr)
        row={'name':name,'command':cmd,'build_rc':build.returncode,'passed':False}
        if not build.returncode:
            run=subprocess.run([str(exe)],text=True,capture_output=True)
            (out/f'{name}.run.txt').write_text(run.stdout+run.stderr)
            row.update(run_rc=run.returncode,passed=run.returncode==0,output=run.stdout+run.stderr)
        rows.append(row);print(name,'PASS' if row['passed'] else 'FAIL',flush=True)
    report={'scope':'Actual production adapters; native memory/API leaves doubled. No game/headset or Windows DLL execution. Linux exceptions model, but do not execute, Windows SEH.',
            'velocity_adapter_sha256':hashlib.sha256(actual.encode()).hexdigest(),'results':rows}
    (out/'results.json').write_text(json.dumps(report,indent=2)+'\n')
    return 0 if all(row['passed'] for row in rows) else 1
if __name__=='__main__':raise SystemExit(main())
