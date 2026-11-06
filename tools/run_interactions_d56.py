#!/usr/bin/env python3
"""Run D56 interaction production-path regressions; native leaves are mocked."""
import argparse, hashlib, json, os, subprocess, sys
from pathlib import Path

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--out',type=Path,required=True)
    ap.add_argument('--only',default='')
    args=ap.parse_args(); root=Path(__file__).resolve().parents[1];out=args.out.resolve();out.mkdir(parents=True,exist_ok=True)
    generated=out/'generated';generated.mkdir(exist_ok=True)
    fixture_sources=[root/'tests'/name for name in ('generated_adapter_types.h','generated_world_adapter.inc','generated_free_hand_caller.inc')]
    crlf_sources=[p for p in fixture_sources if p.exists() and b'\r\n' in p.read_bytes()]
    for script,extra in [('extract_adapter_types.py',[]),('extract_world_adapter_test.py',[]),
                         ('extract_tracked_camera_pickup_d55.py',['--out',str(generated)]),
                         ('extract_acquisition_foundation_d56.py',['--out',str(generated)])]:
        r=subprocess.run([sys.executable,str(root/'tools'/script),*extra],cwd=root,capture_output=True,text=True)
        (out/(script+'.log')).write_text(r.stdout+r.stderr)
        if r.returncode:return r.returncode
    for p in crlf_sources:p.write_bytes(p.read_bytes().replace(b'\r\n',b'\n').replace(b'\n',b'\r\n'))
    cases=['tracked_frame51','pose_foundation_d56','tracked_camera_pickup_d55','contact_corner_d56',
           'contact_reseed_d53','contact_occupancy_d56','hand_pair_contact_d39','animation_ownership112_bridge','acquisition_foundation_d56']
    if args.only:
        selected=args.only.split(',');assert set(selected)<=set(cases);cases=selected
    cxx=os.environ.get('CXX','cl' if os.name=='nt' else 'g++');msvc=Path(cxx).name.lower() in ('cl','cl.exe')
    records=[]
    for name in cases:
        filename=name+'.cpp' if name.endswith('_bridge') else name+'_test.cpp'
        sources=[root/'tests'/filename]
        exe=out/(name+('.exe' if msvc else ''));dep=out/(name+'.d')
        includes=[root/'src',generated,root/'tests/syntax_shims',root/'tests/openxr_sdk']
        if msvc:
            cmd=[cxx,'/nologo','/std:c++20','/EHsc','/Od','/UNDEBUG','/Gy','/Gw','/bigobj',
                 *['/I'+str(p) for p in includes],*map(str,sources),'/Fe:'+str(exe),'/Fo:'+str(out)+os.sep,
                 '/link','/OPT:REF','/STACK:8388608']
        else:
            cmd=[cxx,'-std=c++23','-O0','-g0','-pthread','-fms-extensions','-ffunction-sections','-fdata-sections',
                 '-Wno-subobject-linkage','-D_ReturnAddress=HostReturnAddress','-D__fastcall=',
                 '-MMD','-MF',str(dep),*['-I'+str(p) for p in includes],*map(str,sources),'-Wl,--gc-sections','-o',str(exe)]
        build=subprocess.run(cmd,cwd=root,capture_output=True,text=True);(out/(name+'.build.log')).write_text(build.stdout+build.stderr)
        row={'test':name,'command':cmd,'compile_exit':build.returncode,'native':'MOCKED','headset':'NOT_RUN'}
        if build.returncode==0:
            run=subprocess.run([str(exe)],cwd=root,capture_output=True,text=True,timeout=90)
            row.update(run_exit=run.returncode);(out/(name+'.run.log')).write_text(run.stdout+run.stderr)
            print(name,'PASS' if run.returncode==0 else 'FAIL',run.stdout[-550:],flush=True)
        else:print(name,'COMPILE_FAIL',build.stderr[-2200:],flush=True)
        row['passed']=row.get('run_exit')==0;records.append(row)
    changed=subprocess.run(['git','diff','--name-only'],cwd=root,capture_output=True,text=True).stdout.splitlines()
    hashes={p:hashlib.sha256((root/p).read_bytes()).hexdigest() for p in changed if (root/p).is_file()}
    report={'passed':all(r['passed'] for r in records),'results':records,'source_sha256':hashes,
            'limits':'Host executable fixtures exercise current production logic. Synthetic native memory/rays/OS leaves; no Windows DLL link, game execution, rendered mesh, or headset result is implied.'}
    (out/'RESULTS.json').write_text(json.dumps(report,indent=2)+'\n')
    return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
