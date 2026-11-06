"""Compile actual old/new flight Tick and unchanged T22 scope wrapper, ASAN.
All native identity/status/mutation leaves are explicit substitutes. No ASI,
game DLL load/call, final build, PAK, install or game execution.
"""
import argparse,hashlib,json,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def function(text,name):
    start=text.index('static void '+name+'(');i=text.index('{',start)+1;depth=1
    while depth:
        if text[i]=='{':depth+=1
        elif text[i]=='}':depth-=1
        i+=1
    return text[start:i]+'\n'
def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--build-dir',type=Path,required=True);ap.add_argument('--out',type=Path,required=True);a=ap.parse_args()
    build=a.build_dir.resolve();assert ROOT not in build.parents and build!=ROOT;build.mkdir(parents=True,exist_ok=True)
    fixture=ROOT/'tests/subsystems/throw_drop/flight_policy_t22_test.cpp'
    regression=ROOT/'tests/throw_projectile_t17_test.cpp'
    frozen=ROOT/'tests/subsystems/throw_drop/fixtures/flight_policy_t22'
    policy=ROOT/'src/subsystems/throw_drop/projectile_flight_t17.h';carrier=ROOT/'src/subsystems/throw_drop/runtime/native_ammo_carrier_t18.inl'
    service=ROOT/'src/subsystems/throw_drop/runtime/native_ammo_service_t22.inl'
    paths=[fixture,regression,policy,carrier,service,frozen/'frozen_projectile_flight_t17.h',frozen/'frozen_AmmoTickFlightsT18.inl']
    captured={p:p.read_bytes() for p in paths}
    text=lambda p:captured[p].decode().replace('\r\n','\n')
    for name,expected in [('frozen_projectile_flight_t17.h','b10a210c4a6c7b64dcbd3e87dde2a9161085db33c0ae5486c42148ecb43b507e'),
        ('frozen_AmmoTickFlightsT18.inl','21e5127b8d217d79ab4e4f66ac7c023974e5759e433882298191fabe2ce9931b')]:
        assert hashlib.sha256(captured[frozen/name]).hexdigest()==expected,'frozen RED input changed'
    old=text(frozen/'frozen_projectile_flight_t17.h');assert old.count('namespace kcd2vr::throw_projectile_t17')==1
    (build/'old_policy_generated.hpp').write_text(old.replace('namespace kcd2vr::throw_projectile_t17','namespace old_policy'))
    (build/'old_tick.inc').write_bytes(captured[frozen/'frozen_AmmoTickFlightsT18.inl'])
    (build/'new_tick.inc').write_text(function(text(carrier),'AmmoTickFlightsT18'))
    (build/'service_tick.inc').write_text(function(text(service),'AmmoServiceTickFlightsT22'))
    vc='C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
    command=build/'run.cmd';command.write_text('@echo off\ncall "'+vc+'" >nul\nif errorlevel 1 exit /b %errorlevel%\ncd /d "'+str(build)+'"\ncl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG /fsanitize=address /I "'+str(ROOT/'src')+'" /I "'+str(build)+'" "'+str(fixture)+'" /Fe:test.exe\nif errorlevel 1 exit /b %errorlevel%\ntest.exe\nif errorlevel 1 exit /b %errorlevel%\ncl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG /fsanitize=address /I "'+str(ROOT/'src')+'" "'+str(regression)+'" /Fe:regression.exe\nif errorlevel 1 exit /b %errorlevel%\nregression.exe\nexit /b %errorlevel%\n')
    r=subprocess.run(['cmd.exe','/d','/c',str(command)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(build/'RUN.log').write_text(r.stdout);print(r.stdout)
    lines=[line for line in r.stdout.splitlines() if line.startswith('{"checks":')]
    report={'native_execution':False,'ASI_built':False,'PAK_built':False,'installed':False,'exit_code':r.returncode,
        'input_sha256':{str(p.relative_to(ROOT)):hashlib.sha256(captured[p]).hexdigest() for p in paths},
        'compiled_extracted_sha256':{n:hashlib.sha256((build/n).read_bytes()).hexdigest() for n in ['old_policy_generated.hpp','old_tick.inc','new_tick.inc','service_tick.inc']},
        'fixture':json.loads(lines[-1]) if lines else None,'existing_regression_passed':r.returncode==0 and bool(lines),'all_passed':r.returncode==0 and bool(lines),
        'actual_production':['full old frozen policy/Tick','full current policy/Tick','unchanged T22 manager/world/load wrapper'],
        'explicit_leaves':['native identity lookup','particle GetStatus/velocity adapter','native slot writer','VM/current scope and manager/world getters'],
        'limits':['Native leaves are substitutes. No native impact/damage/sticking or appearance acceptance.','No new cosmetic baseline/tumble enabled.']}
    a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2)+'\n');return 0 if report['all_passed'] else 1
if __name__=='__main__':raise SystemExit(main())
