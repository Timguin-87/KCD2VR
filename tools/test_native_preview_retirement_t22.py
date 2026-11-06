"""Actual cleanup-only adapter with the actual paid/transient fixture leaves.

Reads WHGame as data, runs MSVC ASAN host code. No game build/install/native run.
"""
from pathlib import Path
import argparse,hashlib,json,subprocess,sys,re,struct
from source_text import read_source
from native_research112 import PE


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--dll',type=Path,required=True)
    ap.add_argument('--build-dir',type=Path,required=True)
    ap.add_argument('--out',type=Path,required=True)
    a=ap.parse_args();root=Path(__file__).resolve().parents[1];out=a.build_dir.resolve()
    if out==root or root in out.parents:raise ValueError('external output required')
    out.mkdir(parents=True,exist_ok=True)
    baseline=out/'PAID_BASELINE.json'
    result=subprocess.run([sys.executable,str(root/'tools/test_native_paid_ammo_release_t20.py'),
        '--dll',str(a.dll),'--build-dir',str(out),'--out',str(baseline)],capture_output=True,text=True)
    if result.returncode:print(result.stdout+result.stderr);return result.returncode
    fixture=root/'tests/subsystems/throw_drop/native_paid_ammo_release_t20_test.cpp'
    cases=root/'tests/subsystems/throw_drop/native_preview_retirement_t22_cases.inc'
    header=root/'src/subsystems/throw_drop/native_preview_retirement_t22.h'
    runtime=root/'src/subsystems/throw_drop/runtime/native_preview_retirement_t22.inl'
    native=PE(a.dll)
    model_pins=[]
    for match in re.finditer(r'\{(0x[\dA-Fa-f]+),(\d+),\{([^}]+)\}\}',read_source(runtime)):
        rva=int(match[1],16);size=int(match[2]);value=bytes(int(v.strip(),0) for v in match[3].split(','))
        assert len(value)==size
        model_pins.append(dict(rva=rva,size=size,expected=value.hex(),actual=native.read(rva,size).hex()))
    assert len(model_pins)==8 and all(p['actual']==p['expected'] for p in model_pins)
    model_getter=struct.unpack('<Q',native.read(0x3AA4B98+0x610,8))[0]-native.base
    assert model_getter==0x1A81DD0
    (out/'model_pins_t22.inc').write_text('\n'.join('{0x%X,%d,{%s}},'%(p['rva'],p['size'],','.join('0x'+p['expected'][n:n+2] for n in range(0,len(p['expected']),2))) for p in model_pins)+'\n')
    prefix=read_source(fixture).split('static int Main(){',1)[0]
    prefix=prefix.replace('namespace fixture {','namespace fixture {\nnamespace st2=kcd2vr::supply_t2;\n'
        'static kcd2vr::preview_retirement_t22::Receipt* retireProbe=nullptr;\n'
        'static bool retireReenter=false,retireDenyOld=false,retireDenyFlight=false;\n'
        'static unsigned retireDeniedReads=0;\nstatic void ReenterRetireT22();\n'
        'static void ModelEffectT22(void*,int);\nstatic void NativeModelOnlyT22(void*,int,bool);\n'
        'static void* NativeFindAttachmentT22(void*,const char*);\n'
        'static bool ModelReadDeniedT22(const void*,std::size_t);\n'
        'template<class F>static F Native13(std::uintptr_t);\n'
        'namespace np13 {constexpr std::uintptr_t kFindAttachment=0x1085F74;}\n',1)
    needle_copy='static bool SehCopy(const void* p,void* out,std::size_t n){'
    assert prefix.count(needle_copy)==1
    prefix=prefix.replace(needle_copy,needle_copy+'\n'
        'if(ModelReadDeniedT22(p,n)){++retireDeniedReads;return false;}\n'
        'const auto within=[&](const Memory& m){auto a=reinterpret_cast<std::uintptr_t>(p),b=reinterpret_cast<std::uintptr_t>(&m);return a>=b&&a+n<=b+sizeof(m);};\n'
        'if((retireDenyOld&&(within(held)||within(heldEntity)||within(payload)||within(definition)))||'
        '(retireDenyFlight&&(within(flight)||within(flightEntity)||within(payload)))){++retireDeniedReads;return false;}\n')
    enum='enum class AmmoLookupT18 {Unknown,Absent,Present};'
    assert prefix.count(enum)==1
    prefix=prefix.replace(enum,enum+'\nstatic AmmoLookupT18 ModelActorResolveT22(unsigned,void*&);')
    resolve='static AmmoLookupT18 AmmoResolveT18(unsigned id,void*& e){'
    assert prefix.count(resolve)==1
    prefix=prefix.replace(resolve,resolve+'\nif(id==7)return ModelActorResolveT22(id,e);')
    # One native test leaf changes contract: the NEW adapter has its own spent
    # receipt and may intentionally retire the entity with player=NULL. No
    # production adapter body is changed in the fixture composition.
    needle='assert(!first&&player==reinterpret_cast<void*>(preview.owner.player)&&*id==9&&side==preview.side&&receipt.cleanupEntered);'
    assert prefix.count(needle)==1
    prefix=prefix.replace(needle,'assert(!first&&(!player||player==reinterpret_cast<void*>(preview.owner.player))&&*id==9&&side==preview.side);lastCleanupPlayer=player;')
    effect='if(!cleanupNoop){heldPresent=false;*id=0;}'
    assert prefix.count(effect)==1
    prefix=prefix.replace(effect,'if(!cleanupNoop){ModelEffectT22(player,side);heldPresent=false;*id=0;}')
    needle_mutate='if(cleanupMutation)MutateScope(cleanupMutation);'
    assert prefix.count(needle_mutate)==1
    prefix=prefix.replace(needle_mutate,'if(retireReenter)ReenterRetireT22();\n'
        'if(cleanupMutation==8){++preview.serial;++retireProbe->identity.serial;}else if(cleanupMutation)MutateScope(cleanupMutation);')
    # Execute actual source getters/typed manager-backlink qualification. Only
    # native lookup leaves and Henry's current cached-character observer differ.
    carrier=read_source(root/'src/subsystems/throw_drop/runtime/native_ammo_carrier_t18.inl')
    actual_model=carrier[carrier.index('static void* AmmoModelT18('):carrier.index('static bool AmmoSlotOwnedT18(')]
    actual_attachment=carrier[carrier.index('static bool AmmoAttachmentT18('):carrier.index('static bool AmmoSameOwnerT18(')]
    old_model=prefix[prefix.index('static void* AmmoModelT18('):prefix.index('static bool AmmoOwnPayloadT20(')]
    prefix=prefix.replace(old_model,actual_model+actual_attachment)
    grasp=read_source(root/'src/native_grasp59c13.inl')
    actual_hand=grasp[grasp.index('static bool HandKey13('):grasp.index('static bool Empty13(')]
    native_template='template<class F>static F Native13(std::uintptr_t rva){'
    assert prefix.count(native_template)==1
    prefix=prefix.replace(native_template,actual_hand+'\n#define KCD2VR_T22_MODEL_FIXTURE_LEAVES\n#include "'+cases.as_posix()+'"\n#undef KCD2VR_T22_MODEL_FIXTURE_LEAVES\n'+native_template)
    prefix=prefix.replace('case 0x40B730:return reinterpret_cast<F>(&NativeCleanup);',
        'case 0x23D0DCC:return reinterpret_cast<F>(&NativeModelOnlyT22);case 0x1085F74:return reinterpret_cast<F>(&NativeFindAttachmentT22);case 0x40B730:return reinterpret_cast<F>(&NativeCleanup);')
    cpp=out/'retirement.cpp'
    cpp.write_text('#include "subsystems/throw_drop/native_preview_retirement_t22.h"\nstatic void* lastCleanupPlayer=nullptr;\n'+prefix+
        '\n#include "subsystems/throw_drop/runtime/native_preview_retirement_t22.inl"\n'+
        '#include "'+cases.as_posix()+'"\n')
    vcvars=Path('C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat')
    cmd=out/'run_retirement.cmd'
    cmd.write_text('@echo off\ncall "'+str(vcvars)+'" >nul\nif errorlevel 1 exit /b %errorlevel%\ncd /d "'+str(out)+'"\n'
        'cl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG /fsanitize=address /I "'+str(root/'src')+'" /I "'+str(out)+'" retirement.cpp /Fe:retirement.exe\n'
        'if errorlevel 1 exit /b %errorlevel%\nretirement.exe\nexit /b %errorlevel%\n')
    run=subprocess.run(['cmd.exe','/d','/c',str(cmd)],capture_output=True,text=True)
    log=run.stdout+run.stderr;(out/'RETIREMENT.log').write_text(log)
    rows=[line for line in log.splitlines() if line.startswith('{"checks":')]
    # Causal regression: restore ONLY the former NULL-player native cleanup
    # behavior, leaving the actual required-model assertions unchanged.
    original_runtime=read_source(runtime)
    native_call='PreviewRetireCallT22(receipt,player,original.side);'
    assert original_runtime.count(native_call)==1
    mutant=out/'legacy_null_retirement.inl'
    mutant.write_text(original_runtime.replace(native_call,'PreviewRetireCallT22(receipt,nullptr,original.side);'))
    red_cpp=out/'retirement_red.cpp'
    red_cpp.write_text(cpp.read_text().replace('"subsystems/throw_drop/runtime/native_preview_retirement_t22.inl"','"'+mutant.as_posix()+'"'))
    red_cmd=out/'run_retirement_red.cmd'
    red_cmd.write_text(cmd.read_text().replace('retirement.cpp','retirement_red.cpp').replace('retirement.exe','retirement_red.exe'))
    red=subprocess.run(['cmd.exe','/d','/c',str(red_cmd)],capture_output=True,text=True)
    red_log=red.stdout+red.stderr;(out/'CAUSAL_RED.log').write_text(red_log)
    red_expected=red.returncode!=0 and 'MODEL_RETIRE_FAILURE entityACK=1 modelACK=0 count=1 id=9 cleanup=1 playerNULL=1' in red_log
    report={'passed':run.returncode==0 and bool(rows),'exit_code':run.returncode,
        'fixture':json.loads(rows[-1]) if rows else None,'native_execution':False,
        'model_native_pins':model_pins,'model_native_getter':hex(model_getter),
        'causal_legacy_null':{'expected_failure':red_expected,'exit_code':red.returncode,'only_change':native_call+' -> NULL player','log':str(out/'CAUSAL_RED.log')},
        'paid_baseline':json.loads(baseline.read_text()),'native_context_rebased':False,
        'inputs':{str(f.relative_to(root)):hashlib.sha256(f.read_bytes()).hexdigest() for f in
                  (fixture,cases,header,runtime,root/'src/native_grasp59c13.inl',
                   root/'src/subsystems/throw_drop/runtime/native_ammo_carrier_t18.inl',Path(__file__).resolve())},
        'limits':['Native/VM/entity/item/retirement leaves explicitly substituted.',
                  'Required captured model ACK is separate from entity absence. Temporary source/record/launcher obligations remain independent.',
                  'Caller must preserve original receipt storage across independent service invocations.']}
    report['passed']=report['passed'] and red_expected
    a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2)+'\n')
    print(log);return 0 if report['passed'] else 1


if __name__=='__main__':raise SystemExit(main())
