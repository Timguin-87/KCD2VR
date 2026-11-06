"""Compile the actual T22 service inl with explicit existing native leaf substitutes.
No DLL load, native game execution, integration, ASI or PAK build.
"""
import argparse,hashlib,json,re,struct,subprocess
from pathlib import Path
from native_research112 import PE

def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--dll',required=True,type=Path);ap.add_argument('--build-dir',required=True,type=Path);ap.add_argument('--out',required=True,type=Path);a=ap.parse_args()
 root=Path(__file__).resolve().parents[1];build=a.build_dir.resolve();assert root not in build.parents and build!=root;build.mkdir(parents=True,exist_ok=True)
 runtime=root/'src/subsystems/throw_drop/runtime/native_ammo_service_t22.inl';fixture=root/'tests/subsystems/throw_drop/native_ammo_service_t22_test.cpp'
 text=runtime.read_text();base=PE(a.dll);checks=[]
 def ck(n,actual,expected):checks.append(dict(name=n,actual=actual,expected=expected,passed=actual==expected))
 ck('exact retail SHA256',hashlib.sha256(base.data).hexdigest(),'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7');ck('timestamp',base.stamp,0x6A350E20)
 pins=[]
 for name,rva in [('hideD72',0x87963C),('slot',0x7F1E3C),('filename',0x1A81E20),('helper',0x51EF4C),('selected',0x2AAD1AD),('worldDisableT26',0xABDFF8),('proxyEnableT26',0xABE028),('getProxyT26',0x5D70C4)]:
  match=re.search(r'char '+name+r'\[\]\{([^}]+)\}',text);value=bytes(int(x.strip(),0) for x in match[1].split(','));pins.append((rva,value));ck(name,base.read(rva,len(value)).hex(),value.hex())
 # Exact native attach/reference-count layout proof, not a guessed model field.
 for name,rva,value in [('nativeModelIdWrite',0x10852F2,bytes.fromhex('44897cb72c')),('nativeModelCountOne',0x10852FA,bytes.fromhex('c744b72801000000')),('nativeModelIdClear',0x10854C8,bytes.fromhex('c744b32c00000000'))]:
  ck(name,base.read(rva,len(value)).hex(),value.hex())
 for vft,off,fn in [(0x4072C78,0x1F8,0x87963C),(0x4072C78,0x338,0x7F1E3C),(0x3A2F888,0x148,0x1A81E20),(0x3A2F888,0x1F0,0x51EF4C)]:ck(f'vft{vft:X}+{off:X}',struct.unpack('<Q',base.read(vft+off,8))[0]-base.base,fn)
 dependencies=[runtime,fixture,Path(__file__),root/'src/subsystems/throw_drop/native_ammo_service_t22.h']
 for rel in ['src/core/mutual_admission_t25.h','src/subsystems/throw_drop/pending_reach_t23.h','src/subsystems/throw_drop/hand_claim_t24.h','src/subsystems/throw_drop/native_supply_publication.inl','src/subsystems/throw_drop/ammo_carry_t18.h','src/subsystems/throw_drop/inventory_lifecycle_t8.h','src/subsystems/throw_drop/native_transient_ammo_t20.h','src/subsystems/throw_drop/native_paid_ammo_release_t20.h','src/subsystems/throw_drop/native_private_launcher_t21.h','src/subsystems/throw_drop/native_preview_retirement_t22.h','src/subsystems/throw_drop/runtime/native_transient_ammo_t20.inl','src/subsystems/throw_drop/runtime/native_paid_ammo_release_t20.inl','src/subsystems/throw_drop/runtime/stock_equipment_d65.inl','src/subsystems/throw_drop/runtime/native_private_launcher_t21.inl','src/subsystems/throw_drop/runtime/native_preview_retirement_t22.inl','src/subsystems/throw_drop/runtime/native_ammo_eligibility_t20.inl','src/subsystems/throw_drop/runtime/ammo_eligibility_t20.h','src/subsystems/throw_drop/runtime/native_ammo_launch_t17.inl','src/subsystems/throw_drop/runtime/native_ammo_carrier_t18.inl','src/subsystems/throw_drop/runtime/native_escrow_return_t19.inl','src/subsystems/throw_drop/projectile_flight_t17.h','src/native_script_t2.h','src/frik_pose_probe.h','src/core/native_grasp13.h','src/core/snapshot_mailbox.h','src/vrik/native_supply_t2.h','src/vrik/weapon_release_t1.h']:
  dependencies.append(root/rel)
 report=dict(enabled=False,native_execution=False,static_checks=checks,input_sha256={str(p.resolve().relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in dependencies})
 if not all(c['passed'] for c in checks):a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2));print(json.dumps(checks,indent=2));return 1
 (build/'pins_t22.inc').write_text('\n'.join('{0x%X,{%s}},'%(r,','.join('0x%02x'%x for x in v)) for r,v in pins))
 vc='C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
 cmd=build/'run.cmd';cmd.write_text('@echo off\ncall "'+vc+'" >nul\nif errorlevel 1 exit /b %errorlevel%\ncd /d "'+str(build)+'"\ncl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG /fsanitize=address /I "'+str(root/'src')+'" /I "'+str(build)+'" "'+str(fixture)+'" /Fe:test.exe\nif errorlevel 1 exit /b %errorlevel%\ntest.exe\nexit /b %errorlevel%\n')
 r=subprocess.run(['cmd.exe','/d','/c',str(cmd)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(build/'RUN.log').write_text(r.stdout);print(r.stdout)
 report['fixture_exit_code']=r.returncode;lines=[s for s in r.stdout.splitlines() if s.startswith('{"checks":')];report['fixture']=json.loads(lines[-1]) if lines else None
 # Causal negative control: remove ONLY the new foreign-actor release guard.
 # The same actual service/T24 fixture must catch the original safety failure,
 # rather than merely passing a policy model that mirrors its implementation.
 marker='if(!AmmoServiceForeignActorT22(ctx,h)){'
 assert text.count(marker)==1
 mutant=build/'unguarded_world_service_t22.inl';mutant.write_text(text.replace(marker,'if(false){'),encoding='utf-8')
 original=fixture.read_text();include='#include "subsystems/throw_drop/runtime/native_ammo_service_t22.inl"';assert original.count(include)==1
 probe=build/'unguarded_world_service_t22_test.cpp';probe.write_text(original.replace(include,'#include "'+mutant.as_posix()+'"'),encoding='utf-8')
 failed_line=next(n for n,s in enumerate(original.splitlines(),1) if 'Check(g_ammo_current22[0]==original&&' in s)
 negative=build/'causal.cmd';negative.write_text('@echo off\ncall "'+vc+'" >nul\nif errorlevel 1 exit /b %errorlevel%\ncd /d "'+str(build)+'"\ncl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG /fsanitize=address /I "'+str(root/'src')+'" /I "'+str(build)+'" "'+str(probe)+'" /Fe:causal.exe\nif errorlevel 1 exit /b %errorlevel%\ncausal.exe\nexit /b %errorlevel%\n')
 nr=subprocess.run(['cmd.exe','/d','/c',str(negative)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(build/'CAUSAL_RED.log').write_text(nr.stdout)
 causal=nr.returncode!=0 and ('FAILED line='+str(failed_line)+' ') in nr.stdout
 report['causal_world_guard_removal']={'expected':'RED at original same-actor world-change retention assertion','exit_code':nr.returncode,'expected_line':failed_line,'passed':causal,'log':str(build/'CAUSAL_RED.log'),'mutated_only':marker+' -> if(false){','actual_T24_classifier':True}
 report['all_passed']=r.returncode==0 and bool(lines) and causal;a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2)+'\n');print('World guard removal causal RED:',causal);return 0 if report['all_passed'] else 1
if __name__=='__main__':raise SystemExit(main())
