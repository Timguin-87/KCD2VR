"""Compile the actual pure authority getter with explicit atomic/tracking leaves."""
import argparse, hashlib, json, subprocess
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]


def function(text,name):
    start=text.index('ThrowAuthorityT22 '+name+'(')
    begin=text.index('{',start);depth=1;end=begin+1
    while depth:
        depth+=(text[end]=='{')-(text[end]=='}');end+=1
    return text[start:end]


FIXTURE=r'''
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cassert>
#include <initializer_list>
std::atomic<std::uint64_t> g_pose_epoch6689{1},g_weapon_request_epoch6694{2};
namespace tracking59c6 {std::atomic<std::uint64_t> epoch{3};}
std::atomic<void*> g_committed_character6689{reinterpret_cast<void*>(0x10000)};
std::atomic<int> g_weapon_control_mode{4};
std::atomic<bool> g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_reading{false},g_ctx_scripted{false},g_pose_restore_fault6689{false},g_phys_fault6698{false};
namespace animation_filter {std::atomic<bool> nativeBodyOwned112{false};}
namespace diagnostics::lab {bool reference=false;bool NativeReference() noexcept{return reference;}}
bool head=true,hands[2]{true,true};int mutate=0;
bool TrackingHeadUsable111(std::uint64_t) noexcept{return head;}
bool TrackingHandUsable111(int side,std::uint64_t) noexcept{
 switch(mutate){case 1:++g_pose_epoch6689;break;case 2:++g_weapon_request_epoch6694;break;
 case 3:++tracking59c6::epoch;break;case 4:g_committed_character6689=reinterpret_cast<void*>(0x20000);break;
 case 5:g_gameplay_allowed=false;break;case 6:animation_filter::nativeBodyOwned112=true;break;
 case 7:g_phys_fault6698=true;break;case 8:g_ctx_cutscene=true;break;}
 return hands[side];
}
#include "actual_enum.inc"
#include "actual_getter.inc"
unsigned checks=0;
void ck(bool x){assert(x);++checks;}
void reset(){g_pose_epoch6689=1;g_weapon_request_epoch6694=2;tracking59c6::epoch=3;
 g_committed_character6689=reinterpret_cast<void*>(0x10000);g_weapon_control_mode=4;
 g_gameplay_allowed=true;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_reading=g_ctx_scripted=false;
 g_pose_restore_fault6689=g_phys_fault6698=animation_filter::nativeBodyOwned112=false;
 diagnostics::lab::reference=false;head=hands[0]=hands[1]=true;mutate=0;}
ThrowAuthorityT22 call(int side=0){return ThrowNativeAuthorityT22(side,0x10000,1,2,3,100);}
int main(){
 reset();ck(call()==ThrowAuthorityT22::Allowed);ck(call(1)==ThrowAuthorityT22::Allowed);
 ck(call(-1)==ThrowAuthorityT22::Unknown);ck(call(2)==ThrowAuthorityT22::Unknown);
 ck(ThrowNativeAuthorityT22(0,0,1,2,3,100)==ThrowAuthorityT22::Unknown);
 ck(ThrowNativeAuthorityT22(0,0x10000,0,2,3,100)==ThrowAuthorityT22::Unknown);
 ck(ThrowNativeAuthorityT22(0,0x10000,1,0,3,100)==ThrowAuthorityT22::Unknown);
 ck(ThrowNativeAuthorityT22(0,0x10000,1,2,0,100)==ThrowAuthorityT22::Unknown);
 ck(ThrowNativeAuthorityT22(0,0x10000,1,2,3,0)==ThrowAuthorityT22::Unknown);
 for(auto p:{&g_ctx_menu,&g_ctx_dialogue,&g_ctx_cutscene,&g_ctx_reading,&g_ctx_scripted,&animation_filter::nativeBodyOwned112}){
   reset();*p=true;ck(call()==ThrowAuthorityT22::Denied);ck(call(1)==ThrowAuthorityT22::Denied);}
 reset();g_gameplay_allowed=false;ck(call()==ThrowAuthorityT22::Denied);
 reset();g_weapon_control_mode=3;ck(call()==ThrowAuthorityT22::Denied);
 reset();diagnostics::lab::reference=true;ck(call()==ThrowAuthorityT22::Denied);
 reset();g_pose_restore_fault6689=true;ck(call()==ThrowAuthorityT22::Unknown);
 reset();g_phys_fault6698=true;ck(call()==ThrowAuthorityT22::Unknown);
 reset();head=false;ck(call()==ThrowAuthorityT22::Unknown);
 for(int side:{0,1}){reset();hands[side]=false;ck(call(side)==ThrowAuthorityT22::Unknown);ck(call(1-side)==ThrowAuthorityT22::Allowed);}
 for(int which:{1,2,3,4}){reset();mutate=which;ck(call()==ThrowAuthorityT22::Unknown);}
 for(int which:{5,6,8}){reset();mutate=which;ck(call()==ThrowAuthorityT22::Denied);}
 reset();mutate=7;ck(call()==ThrowAuthorityT22::Unknown);
 reset();++g_pose_epoch6689;ck(call()==ThrowAuthorityT22::Unknown);
 reset();++g_weapon_request_epoch6694;ck(call()==ThrowAuthorityT22::Unknown);
 reset();++tracking59c6::epoch;ck(call()==ThrowAuthorityT22::Unknown);
 reset();g_committed_character6689=reinterpret_cast<void*>(0x20000);ck(call()==ThrowAuthorityT22::Unknown);
 // Changing only an unrelated pose publication is not a current-authority
 // change. This getter has no pose/publication/timestamp-renewal dependency.
 reset();for(int publication:{2,4,6,8}){(void)publication;ck(call()==ThrowAuthorityT22::Allowed);}
 std::printf("{\"checks\":%u,\"passed\":true,\"actual_getter\":true,\"native_execution\":false}\n",checks);
}
'''


if __name__=='__main__':
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--out',type=Path,required=True);args=ap.parse_args()
    out=args.out.resolve();assert out!=ROOT and ROOT not in out.parents;out.mkdir(parents=True,exist_ok=True)
    cpp=ROOT/'src/frik_pose_probe.cpp';header=ROOT/'src/frik_pose_probe.h'
    source=function(cpp.read_text(),'ThrowNativeAuthorityT22')
    enum=next(s for s in header.read_text().splitlines() if s.startswith('enum class ThrowAuthorityT22'))
    assert all(token not in source for token in ('ReadCommittedPacket','g_frame_publication','g_committed_pose_valid','PhysPublish','Native13<'))
    (out/'actual_getter.inc').write_text(source+'\n');(out/'actual_enum.inc').write_text(enum+'\n');(out/'fixture.cpp').write_text(FIXTURE)
    vc='C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
    cmd=out/'run.cmd';cmd.write_text('@echo off\ncall "'+vc+'" >nul\nif errorlevel 1 exit /b %errorlevel%\ncd /d "'+str(out)+'"\ncl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG /fsanitize=address fixture.cpp /Fe:test.exe\nif errorlevel 1 exit /b %errorlevel%\ntest.exe\nexit /b %errorlevel%\n')
    result=subprocess.run(['cmd.exe','/d','/c',str(cmd)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
    (out/'RUN.log').write_text(result.stdout);print(result.stdout)
    lines=[s for s in result.stdout.splitlines() if s.startswith('{"checks":')]
    report={'exit_code':result.returncode,'fixture':json.loads(lines[-1]) if lines else None,'all_passed':result.returncode==0 and bool(lines),
            'actual_getter_sha256':hashlib.sha256(source.encode()).hexdigest(),'source_sha256':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in (cpp,header)},
            'native_execution':False,'DLL_built':False,'limits':'Actual getter body/enum; existing atomic and tracking leaves substituted. No game/native execution.'}
    (out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n')
    raise SystemExit(0 if report['all_passed'] else 1)
