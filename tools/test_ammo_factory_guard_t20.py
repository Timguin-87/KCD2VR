"""Actual eligibility observer + existing full factory callback composition.

Native calls are substituted; the three real NULL branches are byte-verified.
No game build, installation, native execution or ordinary-weapon UX claim.
"""
from pathlib import Path
import hashlib
import json
import subprocess
from native_research112 import PE
from source_text import read_source

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT.parent/'factory_guard_t20_host'


def main():
    OUT.mkdir(exist_ok=True)
    fixture = read_source(ROOT/'tests/native_ammo_eligibility_t20_test.cpp')
    prefix = fixture[:fixture.index('static void run(){')]
    native = read_source(ROOT/'src/subsystems/throw_drop/runtime/native_ammo_launch_t17.inl')
    callback = native[native.index('using CreateAmmoFnT17='):native.index('static bool InstallAmmoLaunchT17()')]
    body = r'''
static unsigned factoryCalls=0;static bool lastFired=false,lastTransfer=false;
static void *argManager=nullptr,*argLauncher=nullptr,*argAmmo=nullptr,*argPlayer=nullptr;
static bool reenterFactory=false,returnNullFactory=false;
static void* nestedFactory=nullptr;
struct ReturnObservation {unsigned calls=0;void* result=nullptr;};
static bool reenterObserver=false;
static void ObserveReturn(void* state,void* result) noexcept {
 auto* o=static_cast<ReturnObservation*>(state);++o->calls;o->result=result;
 CHECK(g_ammo_capture_t17&&g_ammo_capture_t17->calls==1&&g_ammo_capture_t17->projectile==result);
 if(reenterObserver){reenterObserver=false;
  CHECK(!CreateAmmoDetourT17(argManager,argLauncher,argAmmo,argPlayer,true,false));}
}
static void* __fastcall OriginalFactory(void* m,void* l,void* a,void* p,bool f,bool t){
 ++factoryCalls;argManager=m;argLauncher=l;argAmmo=a;argPlayer=p;lastFired=f;lastTransfer=t;
 if(reenterFactory){reenterFactory=false;nestedFactory=CreateAmmoDetourT17(m,l,a,p,f,t);}
 return returnNullFactory?nullptr:reinterpret_cast<void*>(0x777);
}
static void testFactory(){
 reset(); constexpr std::uintptr_t a=0x110000;
 put(a,g_base.load()+0x3A2DA88);put(a+0x48,rowAddress);
 void* ammo=reinterpret_cast<void*>(a);void* m=reinterpret_cast<void*>(1);
 void* l=reinterpret_cast<void*>(2);void* p=reinterpret_cast<void*>(3);
 g_create_ammo_original_t17=&OriginalFactory;
 for(auto family:{ae20::Family::Dagger,ae20::Family::Axe}){
  row(family);CHECK(!AmmoMaskLeaseT20());
  for(auto pc:{0x2895A5A,0x289F821,0x2A8A083})for(bool f:{false,true})for(bool t:{false,true}){
   factoryCalls=0;g_caller=WhBase()+pc;g_ammo_capture_t17=nullptr;
   CHECK(!CreateAmmoDetourT17(m,l,ammo,p,f,t));CHECK(factoryCalls==0);
  }
  // Unknown native consumers preserve complete factory call, even for our role.
  factoryCalls=0;g_caller=WhBase()+0x12345;
  CHECK(CreateAmmoDetourT17(m,l,ammo,p,true,false)==reinterpret_cast<void*>(0x777));
  CHECK(factoryCalls==1&&lastFired&&!lastTransfer&&argManager==m&&argLauncher==l&&argAmmo==ammo&&argPlayer==p);
  // Only the correlated firing invocation may require a finite transfer.
  factoryCalls=0;g_caller=WhBase()+0x289F821;AmmoLaunchCaptureT17 capture{m,p,l,ammo};g_ammo_capture_t17=&capture;
  CHECK(CreateAmmoDetourT17(m,l,ammo,p,true,false)==reinterpret_cast<void*>(0x777));
  CHECK(factoryCalls==1&&lastTransfer&&capture.calls==1&&capture.projectile==reinterpret_cast<void*>(0x777));
  for(int mismatch=0;mismatch<6;++mismatch){
   capture={m,p,l,ammo};factoryCalls=0;g_caller=WhBase()+0x289F821;bool fired=true;
   if(mismatch==0)capture.manager=nullptr;if(mismatch==1)capture.player=nullptr;
   if(mismatch==2)capture.launcher=nullptr;if(mismatch==3)capture.ammo=nullptr;
   if(mismatch==4)fired=false;if(mismatch==5)g_caller=WhBase()+0x2895A5A;
   CHECK(!CreateAmmoDetourT17(m,l,ammo,p,fired,false));CHECK(factoryCalls==0&&capture.calls==0);
  }
  g_ammo_capture_t17=nullptr;
 }
 // Standard player or NPC ammo keeps exact flags and is never captured as ours.
 row(ae20::Family::None);
 for(auto pc:{0x2895A5A,0x289F821,0x2A8A083,0x12345})for(bool f:{false,true})for(bool t:{false,true}){
  ReturnObservation observation{};AmmoLaunchCaptureT17 capture{m,p,l,ammo};
  capture.returnedState=&observation;capture.returnedObserver=&ObserveReturn;g_ammo_capture_t17=&capture;
  factoryCalls=0;g_caller=WhBase()+pc;
  CHECK(CreateAmmoDetourT17(m,l,ammo,p,f,t)==reinterpret_cast<void*>(0x777));
  CHECK(factoryCalls==1&&lastFired==f&&lastTransfer==t&&capture.calls==0&&observation.calls==0);
 }
 // Unreadable/unknown payload identities cannot become an invented own role.
 row();denied=a+0x48;g_ammo_capture_t17=nullptr;factoryCalls=0;g_caller=WhBase()+0x289F821;
 CHECK(CreateAmmoDetourT17(m,l,ammo,p,true,false)==reinterpret_cast<void*>(0x777));CHECK(factoryCalls==1&&!lastTransfer);
 // Reserve admission BEFORE the original. These cases failed with the earlier
 // post-original counter; neither recursion, a sequential call nor a NULL
 // native return may transfer another real unit from this same release.
 denied=0;g_caller=WhBase()+0x289F821;
 for(auto family:{ae20::Family::Dagger,ae20::Family::Axe})for(int mode=0;mode<4;++mode){
  row(family);factoryCalls=0;AmmoLaunchCaptureT17 capture{m,p,l,ammo};g_ammo_capture_t17=&capture;
  ReturnObservation observation{};capture.returnedState=&observation;capture.returnedObserver=&ObserveReturn;
  reenterObserver=mode==3;
  reenterFactory=mode==0;returnNullFactory=mode==2;nestedFactory=reinterpret_cast<void*>(0x999);
  void* first=CreateAmmoDetourT17(m,l,ammo,p,true,false);
  CHECK(factoryCalls==1&&capture.calls==1&&capture.projectile==first);
  CHECK(first==(returnNullFactory?nullptr:reinterpret_cast<void*>(0x777)));
  CHECK(observation.calls==1&&observation.result==first);
  if(mode==0)CHECK(!nestedFactory);
  CHECK(!CreateAmmoDetourT17(m,l,ammo,p,true,false));CHECK(factoryCalls==1&&capture.calls==1&&capture.projectile==first&&observation.calls==1);
 }
 g_ammo_capture_t17=nullptr;returnNullFactory=false;
}
} // namespace fixture
int main(){fixture::testFactory();std::printf("PASS %u actual eligibility/factory composition checks\n",fixture::checks);}
'''
    text = '#include <optional>\nnamespace DetourModKit::hook {struct Hook{};}\n'+prefix
    text += '\n#define KCD2VR_GRASP_CALLER() (g_caller)\n'+callback+'\n#undef KCD2VR_GRASP_CALLER\n'+body
    cpp = OUT/'fixture.cpp'
    cpp.write_text(text)
    pe = PE(Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll'))
    assert hashlib.sha256(pe.data).hexdigest() == 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
    pins = [(0x2895A55, 'e8329a00004885c07435'),
            (0x289F81C, 'e86bfcffff488bf84885c00f84b3010000'),
            (0x2A8A07E, 'e80954e1ff488bf84885c07422')]
    for rva, expected in pins:
        assert pe.read(rva, len(bytes.fromhex(expected))).hex() == expected, hex(rva)
    vcvars = Path('C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat')
    cmd = OUT/'run.cmd'
    cmd.write_text('@echo off\ncall "'+str(vcvars)+'" >nul\nif errorlevel 1 exit /b %errorlevel%\ncd /d "'+str(OUT)+'"\n'
        'cl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG /fsanitize=address /I "'+str(ROOT/'src')+'" fixture.cpp /Fe:test.exe\n'
        'if errorlevel 1 exit /b %errorlevel%\ntest.exe\nexit /b %errorlevel%\n')
    result = subprocess.run(['cmd.exe','/d','/c',str(cmd)],text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    (OUT/'RUN.log').write_text(result.stdout)
    report = dict(passed=result.returncode == 0, exit_code=result.returncode, native_execution=False,
        native_null_branches=[hex(rva) for rva, _ in pins], log=result.stdout,
        input_sha256={str(path.relative_to(ROOT)):hashlib.sha256(path.read_bytes()).hexdigest() for path in
            (ROOT/'src/subsystems/throw_drop/runtime/native_ammo_launch_t17.inl',
             ROOT/'src/subsystems/throw_drop/runtime/native_ammo_eligibility_t20.inl',
             ROOT/'tests/native_ammo_eligibility_t20_test.cpp')})
    receipt = ROOT/'review/T20/FACTORY_GUARD.json'
    receipt.parent.mkdir(parents=True,exist_ok=True)
    receipt.write_text(json.dumps(report,indent=2)+'\n')
    print(result.stdout)
    return result.returncode


if __name__ == '__main__':
    raise SystemExit(main())
