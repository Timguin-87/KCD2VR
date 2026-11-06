"""Read-only native selection pins. No process, installer, hook or game write."""
from pathlib import Path
import argparse,hashlib,json,re,struct,subprocess,zipfile,io,os
from smith_native_probe46 import PE,EXPECTED

ROOT=Path(__file__).resolve().parents[1]
def address(text):
    parts=text.lower().replace('ull','').strip().split('+')
    if not all(re.fullmatch(r'(?:0x[0-9a-f]+|[0-9]+)',p.strip()) for p in parts):raise ValueError(text)
    return sum(int(p.strip(),0) for p in parts)
def verify(dll,source):
    pe=PE(dll);raw=source.read_bytes();text=raw.decode('utf-8-sig')
    code=text.split('kNativeSelectionPins68[]{',1)[1].split('};',1)[0]
    pins=[]
    for r,h in re.findall(r'\{(0x[0-9A-Fa-f]+)ull,"([0-9a-f]+)"\}',code):
        r=int(r,16);expected=bytes.fromhex(h);actual=pe.read(r,len(expected))
        pins.append(dict(rva=hex(r),length=len(expected),sha256=hashlib.sha256(actual).hexdigest(),pass_=actual==expected))
    optional=[]
    for array in ['kNativeNomineeLosPins68','kNativeNomineePacketPins68']:
        if array+'[]{' not in text:continue
        extra=text.split(array+'[]{',1)[1].split('};',1)[0]
        for r,h in re.findall(r'\{(0x[0-9A-Fa-f]+)ull,"([0-9a-f]+)"\}',extra):
            r=int(r,16);wanted=bytes.fromhex(h);actual=pe.read(r,len(wanted))
            optional.append(dict(rva=hex(r),length=len(wanted),sha256=hashlib.sha256(actual).hexdigest(),pass_=actual==wanted))
    pointer=text.split('kNativeSelectionPointers68[][2]{',1)[1].split('};',1)[0]
    ptrs=[]
    for a,b in re.findall(r'\{([^{}]+),([^{}]+)\}',pointer):
        r=address(a);wanted=address(b);actual=struct.unpack('<Q',pe.read(r,8))[0]-pe.base
        ptrs.append(dict(rva=hex(r),target=hex(actual),expected=hex(wanted),pass_=actual==wanted))
    entries=[dict(entry=hex(r),unwind=list(map(hex,pe.function(r))),pass_=pe.function(r)[0]==r) for r in (0x63F9AC,0x63F7A0,0xDE88A8)]
    calls=[]
    for at,wanted in ((0x7F5722,0x63F9AC),(0x640329,0xDE88A8))+(((0x7F5B0A,0x530324),) if optional else ()):
        b=pe.read(at,5);actual=at+5+struct.unpack('<i',b[1:])[0]
        calls.append(dict(call=hex(at),target=hex(actual),pass_=b[0]==0xE8 and actual==wanted))
    passed=len(pins)==12 and len(ptrs)==5 and (not optional or len(optional)==7) and all(p['pass_'] for p in pins+optional+ptrs+entries+calls)
    return dict(pass_=passed,retail_sha256=EXPECTED,timestamp=hex(pe.stamp),source_sha256=hashlib.sha256(raw).hexdigest(),
                code_pins=pins,optional_descriptor_pins=optional,pointer_pins=ptrs,unwind_entries=entries,direct_calls=calls,native_execution=False,game_writes=False)

def exercise(source_root,out,dmk,openxr,vcpkg,parent=None):
    """Own-process actual-DMK host only; no game is opened or modified."""
    out=out.resolve();source_root=source_root.resolve();dmk=dmk.resolve()
    if out==source_root or source_root in out.parents:raise ValueError('test output must be outside source')
    if out.exists() and any(out.iterdir()):raise ValueError('use a new empty output directory')
    out.mkdir(parents=True,exist_ok=True)
    fixture=source_root/'tests/subsystems/alchemy/native_selection68_test.cpp'
    def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
    roots=[('current',source_root,1)]
    if parent:
        retained=out/'parent_source';retained.mkdir()
        archive_stream=subprocess.check_output(['git','archive','--format=zip',parent,'src','tests'],cwd=source_root)
        with zipfile.ZipFile(io.BytesIO(archive_stream)) as z:
            for item in z.infolist():
                if item.is_dir():continue
                target=(retained/item.filename).resolve()
                if retained not in target.parents:raise ValueError(item.filename)
                target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(z.read(item))
        # Only the new existing-test fixture overlays the literal parent src.
        (retained/fixture.relative_to(source_root)).write_bytes(fixture.read_bytes())
        roots.append(('parent',retained,0))
        (out/'PARENT.json').write_text(json.dumps(dict(commit=parent,archive_sha256=hashlib.sha256(archive_stream).hexdigest(),test_overlay_sha256=sha(fixture)),indent=2)+'\n')
    asm=out/'nominee_harness.asm';asm.write_text(HARNESS68,encoding='ascii')
    def snapshot(root):return {str(p.relative_to(root)).replace('\\','/'):sha(p) for p in sorted((root/'src').rglob('*')) if p.is_file()}
    env_script=out/'environment.cmd'
    env_script.write_text('@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\nset\n',encoding='ascii')
    env_text=subprocess.check_output(['cmd','/d','/c',str(env_script)],text=True)
    run_env=os.environ.copy()
    for line in env_text.splitlines():
        if '=' in line and not line.startswith('='):
            key,value=line.split('=',1);run_env[key]=value
    before=snapshot(source_root);results=[]
    for name,root,present in roots:
        for config,flags in [('Debug','/Od /Zi'),('ASAN','/Od /Zi /fsanitize=address'),('O2_ASAN','/O2 /Ob1 /Zi /fsanitize=address')]:
            build=out/(name+'_'+config);build.mkdir()
            include=[root/'src',dmk/'include',dmk/'include/DirectXMath',openxr]
            if vcpkg:include.append(vcpkg)
            inc=' '.join('/I"'+p.resolve().as_posix()+'"' for p in include)
            libs=' '.join('"'+(dmk/'lib'/p).as_posix()+'"' for p in ['libDetourModKit.lib','safetyhook.lib','Zydis.lib','Zycore.lib'])
            modes=['']+(['success','pin_fail','foreign_fail','retail_fail','disable_fail'] if present else [])
            cmd='@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\ncd /d "'+build.as_posix()+'"\n'
            cmd+='ml64 /nologo /c /Fo"harness.obj" "'+asm.as_posix()+'" >asm.log 2>&1\nif errorlevel 1 exit /b %errorlevel%\n'
            cmd+='cl /nologo /std:c++latest /EHsc /MD /UNDEBUG /bigobj /Gy /Gw '+flags+' /D KCD2VR_NOMINEE_LOS_JOINED /D KCD2VR_TEST_LOS_PRESENT='+str(present)+' /D KCD2VR_TEST_PACKET_PREPIN='+str(present)+' /D WIN32_LEAN_AND_MEAN /D NOMINMAX /D _DISABLE_VECTOR_ANNOTATION /D _DISABLE_STRING_ANNOTATION '+inc+' "'+(root/fixture.relative_to(source_root)).as_posix()+'" /Fe:joined.exe /sourceDependencies dependencies.json harness.obj /link /OPT:REF /INCREMENTAL:NO '+libs+' kernel32.lib user32.lib >compile.log 2>&1\nexit /b %errorlevel%\n'
            (build/'run.cmd').write_text(cmd,encoding='ascii')
            compile_run=subprocess.run(['cmd','/d','/c',str(build/'run.cmd')],capture_output=True,text=True)
            executions=[]
            if compile_run.returncode==0:
                for mode in modes:
                    run=subprocess.run([str(build/'joined.exe')]+([mode] if mode else []),cwd=build,capture_output=True,text=True,env=run_env)
                    log=build/((mode or 'joined')+'.log');log.write_text(run.stdout+run.stderr)
                    executions.append(dict(mode=mode or 'joined',exit=run.returncode,log=log.name,log_sha256=sha(log),text=run.stdout+run.stderr))
            deps={}
            if (build/'dependencies.json').exists():
                d=json.loads((build/'dependencies.json').read_text(encoding='utf-8-sig'))['Data']
                for path in [d['Source']]+d['Includes']:
                    q=Path(path)
                    if q.is_file():deps[q.as_posix()]=sha(q)
            (build/'INPUTS.json').write_text(json.dumps(deps,indent=2)+'\n')
            expected=0 if present else 2
            good=compile_run.returncode==0 and bool(executions) and all(r['exit']==expected for r in executions)
            result=dict(source=name,config=config,compile_exit=compile_run.returncode,expected_runtime_exit=expected,pass_=good,executions=executions,input_count=len(deps),inputs_sha256=sha(build/'INPUTS.json'),compile_log_sha256=sha(build/'compile.log'))
            results.append(result)
            print(name,config,'PASS' if good else 'FAIL',executions[0]['text'].strip() if executions else (build/'compile.log').read_text()[-3000:],flush=True)
    libs={p.name:sha(p) for p in (dmk/'lib').glob('*.lib')}
    report=dict(pass_=all(r['pass_'] for r in results),results=results,source_stable=before==snapshot(source_root),source_sha256=before,fixture_sha256=sha(fixture),tool_sha256=sha(Path(__file__)),dmk_prefix=str(dmk),dmk_libraries=libs,harness_sha256=sha(asm),native_execution=False,game_writes=False,limits=['Own-process actual hook/whole production interaction/input composition; native geometry/result is a controlled leaf/model, not headset acceptance.','Current native fingerprint/bytes separately verified from actual retail file; host retail activation is controlled. Release DMK libraries are not ASAN-instrumented; host disables STL container annotations to match those binaries while retaining heap/stack ASAN.','Exact parent src receives only the same updated test fixture, no runtime callback shim.'])
    (out/'RESULTS.json').write_text(json.dumps(report,indent=2)+'\n')
    return report

HARNESS68 = r'''
option casemap:none
EXTERN RayDispatchTarget68:QWORD,QueryReturn68:QWORD
PUBLIC NomineeHarness68,NomineeSeam68,NomineeReturn68
.code
NomineeHarness68 PROC
 push rbx
 push rbp
 push rsi
 push rdi
 push r12
 push r14
 push r15
 sub rsp,80h
 mov rbx,rcx
 mov r15,[rbx+32]
 mov rdi,[rbx+24]
 mov r12,[rbx+8]
 mov r14d,[rbx+48]
 mov eax,[rbx+40]
 mov [rsp+58h],eax
 mov rax,[rbx+56]
 mov [rsp+28h],rax
 mov al,[rbx+52]
 mov [rsp+30h],al
 mov al,[rbx+64]
 mov [rsp+38h],al
 mov rcx,[rbx]
 mov rdx,r12
 mov r8,[rbx+16]
 mov r9d,[rbx+44]
NomineeSeam68 LABEL BYTE
 db 44h,89h,74h,24h,20h
 call RayDispatch68
NomineeReturn68 LABEL BYTE
 add rsp,80h
 pop r15
 pop r14
 pop r12
 pop rdi
 pop rsi
 pop rbp
 pop rbx
 ret
NomineeHarness68 ENDP
RayDispatch68 PROC
 mov rax,[rsp]
 mov QueryReturn68,rax
 jmp qword ptr [RayDispatchTarget68]
RayDispatch68 ENDP
END
'''

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--dll',type=Path,required=True);ap.add_argument('--source',type=Path,default=ROOT/'src/interaction_native_selection68.inl');ap.add_argument('--out',type=Path)
    ap.add_argument('--exercise',type=Path);ap.add_argument('--dmk-prefix',type=Path);ap.add_argument('--openxr-include',type=Path);ap.add_argument('--vcpkg-include',type=Path);ap.add_argument('--parent')
    a=ap.parse_args();report=verify(a.dll,a.source);formatted=json.dumps(report,indent=2)+'\n'
    if a.out:a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(formatted,encoding='utf-8')
    print('NATIVE_SELECTION68_RETAIL_PINS', 'PASS' if report['pass_'] else 'FAIL',len(report['code_pins'])+len(report['optional_descriptor_pins'])+len(report['pointer_pins'])+len(report['unwind_entries'])+len(report['direct_calls']))
    if not report['pass_']:raise SystemExit(1)
    if a.exercise:
        if not a.dmk_prefix or not a.openxr_include:ap.error('--exercise requires --dmk-prefix and --openxr-include')
        if not exercise(a.source.resolve().parents[1],a.exercise,a.dmk_prefix,a.openxr_include,a.vcpkg_include,a.parent)['pass_']:raise SystemExit(2)
if __name__=='__main__':main()
