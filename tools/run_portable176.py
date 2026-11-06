#!/usr/bin/env python3
"""Run the repository's portable C++ targets when CMake/Clang are unavailable.

Uses the source lists in tests/CMakeLists.txt, the original production extraction
scripts and GCC. It is not an MSVC build, CTest, or a game/native ABI emulator.
Full output and every command/return code are retained separately per target.
"""
import argparse, concurrent.futures, json, os, re, shlex, subprocess, sys
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]

def targets():
    text=(ROOT/'tests/CMakeLists.txt').read_text()
    text=re.sub(r'(?m)#.*$', '', text)
    commands=[(m[1].lower(),m[2]) for m in re.finditer(r'(\w+)\((.*?)\)',text,re.S)]
    result={}
    def walk(start,end,variables):
        i=start
        while i<end:
            command,body=commands[i]
            for key,value in variables.items():body=body.replace('${'+key+'}',value)
            args=shlex.split(body)
            if command in ('if','foreach'):
                depth=1;j=i+1;other=None
                while j<end:
                    if commands[j][0]==command:depth+=1
                    if commands[j][0]==('endif' if command=='if' else 'endforeach'):
                        depth-=1
                        if not depth:break
                    if command=='if' and commands[j][0]=='else' and depth==1:other=j
                    j+=1
                if command=='foreach':
                    if len(args)>1 and args[1]!='IN':
                        for value in args[1:]:walk(i+1,j,{**variables,args[0]:value})
                else:
                    enabled=(args==['NOT','MSVC'])
                    if enabled:walk(i+1,other if other is not None else j,variables)
                    elif other is not None:walk(other+1,j,variables)
                i=j+1;continue
            if command=='add_executable' and len(args)>1:
                paths=[str((ROOT/'tests'/p).resolve()) for p in args[1:] if p.endswith(('.cpp','.cc'))]
                if paths:result[args[0]]=paths
            i+=1
    walk(0,len(commands),{'CMAKE_CURRENT_SOURCE_DIR':str(ROOT/'tests')})
    for name in ('fist','calibration','physical','native_contract','native104','lifecycle','bridges168','attack_timing178r22c2'): # C10R19: + attack timing bridge (portable since BC1)
        result[name+'_bridge']=[str(ROOT/f'tests/{name}_bridge_test.cpp'),str(ROOT/'tests/bridge_external_stubs112.cpp')]
    for name in ('bow_native_contract175','diagnostic_modes175'):
        p=ROOT/f'tests/{name}_bridge_test.cpp'
        if p.exists():result[name+'_bridge']=[str(p),str(ROOT/'tests/bridge_external_stubs112.cpp')]
    for name in ('animation_ownership112_bridge','contact_hook115_test'):
        p=ROOT/f'tests/{name}.cpp'
        if p.exists():result[name]=[str(p)]
    return result

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--out',required=True);parser.add_argument('--sanitizers',action='store_true')
    parser.add_argument('--only',default='');parser.add_argument('--jobs',type=int,default=2);args=parser.parse_args()
    out=Path(args.out).resolve();out.mkdir(parents=True,exist_ok=True)
    for script in ('extract_adapter_types.py','extract_world_adapter_test.py','extract_controller_test.py','extract_physical_frame_test.py','extract_regression_test.py','extract_user_controls175.py','extract_fist_transport175.py','extract_r3_route176.py','extract_state5912.py','extract_prop5916c1.py','extract_effort5916c1.py','extract_repair5916c3.py','extract_door_request38.py','extract_reload5916c3.py','extract_impulse5916c3.py','extract_tracking59c6.py','extract_contact_queue59c6.py','extract_render_pacing59c6.py','extract_hud59c7.py','extract_dialogue_camera19c2.py','extract_activity_eye_camera43.py','extract_tracked_frame51.py'):
        done=subprocess.run([sys.executable,str(ROOT/'tools'/script)],cwd=ROOT,capture_output=True,text=True)
        (out/(script+'.log')).write_text(done.stdout+done.stderr)
        if done.returncode:raise SystemExit(f'Production extraction failed: {script}')
    # Match CMake's build-directory generated includes. A clean extraction must
    # not depend on a previous CMake run or a stale source-side fixture.
    generated=out/'generated';generated.mkdir(exist_ok=True)
    for script,filename in (('extract_phys_bodies19c1.py','phys_bodies_adapter19c1.inc'),
                            ('extract_dialogue_selector19c2.py','dialogue_selector_adapter19c2.inc'),
                            ('extract_dialogue_native_bridge28.py','dialogue_native_bridge28_extracted.inc')):
        done=subprocess.run([sys.executable,str(ROOT/'tools'/script),str(generated/filename)],cwd=ROOT,capture_output=True,text=True)
        (out/(script+'.log')).write_text(done.stdout+done.stderr)
        if done.returncode:raise SystemExit(f'Production extraction failed: {script}')
    selected=targets()
    if args.only:
        wanted=set(args.only.split(','));missing=wanted-selected.keys()
        if missing:raise SystemExit('Unknown targets: '+','.join(sorted(missing)))
        selected={name:sources for name,sources in selected.items() if name in wanted}
    cxx=os.environ.get('CXX','g++')
    compiler=subprocess.run([cxx,'--version'],capture_output=True,text=True,check=True).stdout.splitlines()[0]
    def run(item):
        name,sources=item
        flags=['-std=c++17','-O0','-g0','-fms-extensions','-ffunction-sections','-fdata-sections','-Wno-subobject-linkage','-D_ReturnAddress=HostReturnAddress','-D__fastcall=']
        # These full-TU fixtures deliberately replace __builtin_return_address.
        # Their non-MSVC intrinsic is defined in the TU/shim itself; mapping it to
        # an undeclared MSVC HostReturnAddress breaks compilation, not gameplay.
        if name in ('quiet_hit_feedback168_test','parry_outcome176_test',
                    'contact_gate176_test','f4_contact_install176_test',
                    'combo_timing176_test','contact_hook115_test'):
            flags.remove('-D_ReturnAddress=HostReturnAddress')
        # Match the explicit standard of this target in tests/CMakeLists.txt.
        if name == 'dialogue_native_bridge28_test':
            flags[0]='-std=c++20'
        if name in ('input_repair175_test','hud_writer175_bridge_test','activity_native175_bridge_test','fist_transport175_test'):
            flags+=['-I'+str(ROOT/'tests/ui175/mocks')]
        flags+=['-I'+str(generated),'-I'+str(ROOT/'tests/syntax_shims'),'-I'+str(ROOT/'tests/openxr_sdk'),'-I'+str(ROOT/'src')]
        if name in ('fist_bridge','calibration_bridge'):flags+=['-DBRIDGE_HAS_MODE_STUBS']
        if name=='fist_bridge':flags+=['-DBRIDGE_HAS_TARGET_STUB']
        if args.sanitizers:flags+=['-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-pie','-no-pie']
        command=[cxx,*flags,*sources,'-pthread','-Wl,--gc-sections','-o',str(out/name)]
        compiled=subprocess.run(command,cwd=ROOT,capture_output=True,text=True)
        (out/(name+'_compile.log')).write_text(compiled.stdout+compiled.stderr)
        result={'name':name,'command':command,'compile_exit':compiled.returncode,'run_exit':None}
        if compiled.returncode==0:
            env=dict(os.environ)
            # LSan process enumeration is unavailable in this managed container.
            # Address/undefined checks are active; leak detection is not claimed.
            env.update(ASAN_OPTIONS='detect_leaks=0',UBSAN_OPTIONS='print_stacktrace=1:halt_on_error=1')
            completed=subprocess.run([str(out/name)],cwd=ROOT,capture_output=True,text=True,env=env)
            (out/(name+'.log')).write_text(completed.stdout+completed.stderr)
            result['run_exit']=completed.returncode
        result['passed']=result['compile_exit']==0 and result['run_exit']==0
        print(('PASS ' if result['passed'] else 'FAIL ')+name,flush=True)
        return result
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:results=list(pool.map(run,selected.items()))
    report={'compiler':compiler,'sanitizers':args.sanitizers,'leak_detection':False,'native_execution':False,'results':results,
            'passed':sum(x['passed'] for x in results),'total':len(results)}
    (out/'results.json').write_text(json.dumps(report,indent=2)+'\n')
    print(f"{report['passed']}/{report['total']} passed; native game/Windows/headset not executed.")
    return 0 if all(x['passed'] for x in results) else 1

if __name__=='__main__':raise SystemExit(main())
