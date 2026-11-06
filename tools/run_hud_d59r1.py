#!/usr/bin/env python3
"""Focused D59R1/P1 notification, quiet phase timing and production-hook checks.

Uses mock native/OS leaves; does not build or execute a Windows mod DLL.
"""
import argparse, hashlib, json, os, subprocess
from pathlib import Path

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--out',required=True,type=Path)
    ap.add_argument('--sanitizers',action='store_true')
    args=ap.parse_args();root=Path(__file__).resolve().parents[1]
    out=args.out.resolve();out.mkdir(parents=True,exist_ok=True)
    cxx=os.environ.get('CXX','g++');msvc=Path(cxx).name.lower() in ('cl','cl.exe')
    cases=['hud_notifications_d59r1','hud_writer175_bridge','hud_policy59c7','hud_work_p1','hud_phase_cost_p1']
    rows=[]
    for name in cases:
        exe=out/(name+('.exe' if msvc else ''))
        inc=[root/'tests/ui175/mocks',root/'tests/syntax_shims',root/'tests/openxr_sdk',root/'src']
        if msvc:
            cmd=[cxx,'/nologo','/std:c++20','/EHsc','/Od','/UNDEBUG','/Gy','/Gw','/bigobj',
                 *['/I'+str(p) for p in inc],str(root/'tests'/(name+'_test.cpp')),
                 '/Fe:'+str(exe),'/Fo:'+str(out)+os.sep,'/link','/OPT:REF']
        else:
            cmd=[cxx,'-std=c++23','-O0','-g0','-fms-extensions','-ffunction-sections','-fdata-sections',
                 '-Wno-subobject-linkage','-D_ReturnAddress=HostReturnAddress','-D__fastcall=',
                 *['-I'+str(p) for p in inc]]
            if args.sanitizers:cmd+=['-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-pie','-no-pie']
            cmd += [str(root/'tests'/(name+'_test.cpp')),'-pthread','-Wl,--gc-sections','-o',str(exe)]
        build=subprocess.run(cmd,cwd=root,capture_output=True,text=True)
        (out/(name+'.compile.log')).write_text(build.stdout+build.stderr)
        row={'name':name,'command':cmd,'compile_exit':build.returncode,'run_exit':None}
        if build.returncode==0:
            env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0',UBSAN_OPTIONS='print_stacktrace=1:halt_on_error=1')
            run=subprocess.run([str(exe)],cwd=root,capture_output=True,text=True,env=env,timeout=60)
            row['run_exit']=run.returncode;(out/(name+'.run.log')).write_text(run.stdout+run.stderr)
            print(run.stdout.strip(),flush=True)
        row['passed']=row['compile_exit']==0 and row['run_exit']==0;rows.append(row)
        print(name,'PASS' if row['passed'] else 'FAIL',flush=True)
    paths=['src/ui_prompt_scale120r3.h','src/ui_hud_work59c7.h',
           'src/subsystems/hud_ui_controls/runtime/flashui_probe.cpp',
           *['tests/'+n+'_test.cpp' for n in cases]]
    receipt={'passed':all(r['passed'] for r in rows),'results':rows,
        'source_sha256':{p:hashlib.sha256((root/p).read_bytes()).hexdigest() for p in paths},
        'sanitizers':bool(args.sanitizers and not msvc),'leak_detection':False,
        'limits':'Production notification logic and full FlashUI TU detours; native/OS leaves mocked. No game, headset, Windows SEH-unwind or DLL link validation.'}
    (out/'RESULTS.json').write_text(json.dumps(receipt,indent=2)+'\n')
    return 0 if receipt['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
