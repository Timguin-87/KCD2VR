#!/usr/bin/env python3
"""Run production letter Lua and native feed with explicit native leaves.

Linux Lua5.4/C++ fixtures check lifecycle; neither is retail Lua5.1, Windows ABI,
native animation evaluation or headset rendering. The Lua scenario also runs
against D43's old loaded presenter to demonstrate its incompatible behaviour.
"""
from pathlib import Path
import argparse, ctypes, ctypes.util, json, subprocess, tempfile

p=argparse.ArgumentParser()
p.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1])
p.add_argument('--output',type=Path)
p.add_argument('--lua-only',action='store_true')
a=p.parse_args();root=a.source.resolve();out=a.output or Path(tempfile.mkdtemp(prefix='letter_d44_'))
out.mkdir(parents=True,exist_ok=True)
lua=ctypes.CDLL(ctypes.util.find_library('lua5.4'))
lua.luaL_newstate.restype=ctypes.c_void_p
lua.luaL_openlibs.argtypes=[ctypes.c_void_p]
lua.luaL_loadbufferx.argtypes=[ctypes.c_void_p,ctypes.c_char_p,ctypes.c_size_t,ctypes.c_char_p,ctypes.c_char_p]
lua.lua_pcallk.argtypes=[ctypes.c_void_p,ctypes.c_int,ctypes.c_int,ctypes.c_int,ctypes.c_longlong,ctypes.c_void_p]
lua.lua_tolstring.argtypes=[ctypes.c_void_p,ctypes.c_int,ctypes.c_void_p];lua.lua_tolstring.restype=ctypes.c_char_p
lua.lua_close.argtypes=[ctypes.c_void_p]
state=lua.luaL_newstate();lua.luaL_openlibs(state)
setup=r'''
t=10;header=false;starts=0;refuse=false;badReadback=false;checks=0
function check(v,msg) checks=checks+1;assert(v,msg)end
g_localActor={id=42};KCD2VRMap={world=42,awaitStart=false,entities={}}
System={GetCurrTime=function()return t end,GetEntity=function(id)return id==42 and g_localActor end,LogAlways=function()end}
function makePaper()
 local e={time=0,speed=0,hidden=true}
 function e:GetAnimationLength(slot,clip)return header and 1 or 0 end
 function e:StopAnimation()end
 function e:ResetAnimation()self.fifo=false end
 function e:StartAnimation(slot,clip,layer,blend,speed,loop,hold)
  check(clip=='open' and speed==0 and not loop,'only a frozen open clip may be prepared')
  check(self.hidden,'a paper must be hidden before preparation')
  starts=starts+1;self.time=0;self.speed=speed;self.fifo=not refuse;return not refuse
 end
 function e:SetAnimationSpeed(slot,layer,speed)check(speed==0,'no visible animation playback');self.speed=speed end
 function e:SetAnimationTime(slot,layer,v)check(self.fifo and v==1,'prepare only complete open pose');self.time=v end
 function e:GetAnimationTime()return badReadback and 0 or self.time end
 function e:ForceCharacterUpdate(slot,on)self.forced=on end
 function e:Activate(on)self.active=on==1 end
 function e:Hide(on)self.hidden=on==1 end
 return e
end
left=makePaper();other=makePaper()
KCD2VRMap.entities.a={entity=left};KCD2VRMap.entities.b={entity=other}
function KCD2VRMap.SyncPresentation()
 for _,s in pairs(KCD2VRMap.entities)do KCD2VRMap.AdvancePaper31(s,s.entity,not s.entity.hidden)end
end
function intent(a,b)return kcd2vr_map_m31_intent('I31|'..a..'|'..b)end
function tick(dt)t=t+dt;KCD2VRMap.SyncPresentation()end
'''
scenario=r'''
check(intent(1,1)=='I31|1|PREPARING','first request remains hidden while clip streams')
check(starts==2 and left.hidden and other.hidden,'two owned helpers requested exactly once')
tick(.1);check(starts==2,'stream wait never restarts accepted clip')
header=true;tick(.1);check(KCD2VRMap.entities.a.paper31.phase=='PREPARING','setting time does not acknowledge same update')
check(intent(1,1)=='I31|1|PREPARING','same time is not a completed native update')
tick(.02);check(intent(1,1)=='I31|1|STATIC_READY','both actual time readbacks ready')
check(left.hidden and other.hidden,'Lua never reveals paper before native placement')
left.hidden=false
for i=1,20 do tick(.016)end
check(starts==2 and left.time==1 and left.speed==0,'visible letter never unfolds or restarts')
check(kcd2vr_map_fold_m39('F39|1|0.2')=='F39|RETIRED_D44','retired fold cannot deform current letter')
check(intent(2,0)=='I31|2|CLOSED' and left.hidden and other.hidden,'close is immediate for both helpers')
check(starts==2 and not left.active,'no close animation starts')
check(intent(3,1)=='I31|3|STATIC_READY' and starts==2,'same frozen open asset reopens without playback')
check(intent(2,0)=='I31|REFUSED','old close serial cannot close a new letter')
left.hidden=true;tick(.1);left.hidden=false;tick(.1)
check(starts==2 and left.time==1,'temporary native pose hide never restarts animation')
KCD2VRMap.WorldEnded()
check(left.hidden and not left.forced and not left.active,'world retirement hides and stops owned helper updates')
check(intent(4,1)=='I31|NO_CURRENT_PAPER','ended world cannot reopen')
KCD2VRMap.awaitStart=false;badReadback=true
check(intent(4,1)=='I31|4|PREPARING','new preparation needs new readback')
tick(.1);check(intent(4,1)=='I31|4|PREPARING','elapsed time cannot synthesize open pose')
tick(3);check(intent(4,1)=='I31|4|FAULT' and left.hidden,'failed pose bounded and hidden')
badReadback=false
check(intent(5,1)=='I31|5|PREPARING','new explicit intent retries a fault')
tick(.02);check(intent(5,1)=='I31|5|STATIC_READY','new observed preparation succeeds')
check(intent(6,0)=='I31|6|CLOSED','final close')
'''
def run(code,name):
    data=code.encode()
    rc=lua.luaL_loadbufferx(state,data,len(data),name.encode(),None)
    if not rc:rc=lua.lua_pcallk(state,0,0,0,0,None)
    if rc:raise RuntimeError(lua.lua_tolstring(state,-1,None).decode())
result={'lua_runtime':'5.4 host leaves','retail_Lua51_Windows_headset':'NOT_RUN'}
try:
    run(setup,'fixture')
    module=root/'mods/KCD2VRMap/map_letter_d44.lua'
    if not module.exists():module=root/'mods/KCD2VRMap/map_presentation_m31.lua'
    run(module.read_text(),'production_letter')
    run(scenario,'actual_lifecycle')
    result['lua']='PASS'
except Exception as e:
    result['lua']='FAIL';result['lua_error']=str(e)
finally:lua.lua_close(state)
if not a.lua_only:
    shim=out/'host_shims';shim.mkdir(exist_ok=True)
    (shim/'windows.h').write_text('#pragma once\n#include <cstdint>\n#define __fastcall\n#define __try try\n#define __except(x) catch(...)\n#define EXCEPTION_EXECUTE_HANDLER 1\ninline void* GetModuleHandleW(const wchar_t*){return reinterpret_cast<void*>(0x180000000ull);}\n')
    binary=out/'static_letter_d44'
    build=subprocess.run(['c++','-std=c++17','-O2','-pthread','-I',str(shim),'-I',str(root/'src'),str(root/'tests/static_letter_d44_test.cpp'),'-o',str(binary)],capture_output=True,text=True)
    (out/'native_build.log').write_text(build.stdout+build.stderr)
    result['native_build']='PASS' if build.returncode==0 else 'FAIL'
    if not build.returncode:
        test=subprocess.run([str(binary)],capture_output=True,text=True)
        (out/'native_run.log').write_text(test.stdout+test.stderr)
        result['native']='PASS' if test.returncode==0 else 'FAIL';result['native_output']=test.stdout+test.stderr

    presenter=(root/'src/subsystems/vrmap/runtime/held_map_m1.cpp').read_text()
    begin=presenter.index('void NativeFrameTick() noexcept {')
    body=presenter[begin:presenter.index('\nvoid Shutdown()',begin)]
    code=r"""
#include "subsystems/vrmap/runtime/held_map_m1.h"
#include "subsystems/vrmap/runtime/held_map_feed_t2.h"
#include "held_map_pose_m1.h"
#include <atomic>
#include <cstring>
#include <cstdio>
static std::uint64_t nowD44=1000;
static std::uint64_t GetTickCount64(){return nowD44;}
static bool trackedD44=true,preparedD44=false,feedD44=false,missingD44=false,placementD44=true;
static unsigned setsD44=0,unhidesD44=0;static bool hideFailsD44=false;
namespace kcd2vr {
namespace diagnostic_trace {void Write(const char*){}}
namespace frik_pose_probe {
bool ReadHeldMapLeftPoseM1(float (&out)[8],unsigned* refusal,std::uintptr_t* actor,std::uint64_t* epoch,std::uint64_t* tick,void*) {
 if(!trackedD44){*refusal=6;return false;}
 const float v[]{0,0,0,1,1,2,3,1};std::memcpy(out,v,sizeof(v));*actor=0x123400;*epoch=7;*tick=nowD44;return true;
}}
namespace held_map_feed_t2 {
void Tick(bool v,std::uint64_t)noexcept{feedD44=v;}
void RequestReadiness(std::uint32_t,std::uint64_t,std::uint64_t,std::uint64_t)noexcept{}
bool ReportHelperMissing(std::uint32_t,std::uint64_t,std::uint64_t)noexcept{return true;}
bool LetterReadyD44(std::uint32_t actor,std::uint64_t epoch,std::uint64_t serial,std::uint64_t)noexcept{
 return preparedD44&&actor==42&&epoch==7&&serial==held_map_m1::activation.load();
}
bool Read(Snapshot&,std::uint64_t)noexcept{return false;}
}
namespace held_map_m1 {
static std::atomic<bool> ready{true},shown{false};
static std::atomic_flag busy=ATOMIC_FLAG_INIT;
static std::atomic<std::uint64_t> placed{0},showN{0},hideN{0},missingN{0},staleN{0};
static std::atomic<unsigned> poseRefusal19c3{0},lastHideReasonM4{0};
static constexpr std::uintptr_t base=0x180000000ull;
static const char* regionNames[]{"TROSKY","KUTTENBERG"};
struct PaperD44 {bool hidden=true;} paperD44;
static bool Copy(std::uintptr_t p,void* out,std::size_t n){
 if(p==0x123400&&n==8){auto v=base+0x4072C78;std::memcpy(out,&v,n);return true;}
 if(p==0x12340C&&n==4){std::uint32_t id=42;std::memcpy(out,&id,n);return true;}return false;
}
static void* Find(PrintedRegion,unsigned* reason=nullptr){if(reason)*reason=missingD44?3:0;return missingD44?nullptr:&paperD44;}
static bool Hidden(void* p,bool& h){h=static_cast<PaperD44*>(p)->hidden;return true;}
static bool Hide(void* p,bool h){if(h&&hideFailsD44)return false;static_cast<PaperD44*>(p)->hidden=h;if(!h)++unhidesD44;return true;}
static bool Place(void*,const float*,unsigned& reason){if(!placementD44){reason=3;return false;}++setsD44;return true;}
ACTUAL_PRESENTER_D44
}}
static unsigned checks=0,failures=0;
static void ck(bool b,const char* s){++checks;if(!b){++failures;std::printf("FAIL %s\n",s);}}
int main(){using namespace kcd2vr::held_map_m1;
 auto frame=[](std::uint64_t t){nowD44=t;kcd2vr::held_map_m1::PublishAllowed(true,t);kcd2vr::held_map_m1::NativeFrameTick();};
 Select(true);frame(1000);
 ck(feedD44,"hidden paper still schedules its own preparation feed (no readiness deadlock)");
 ck(paperD44.hidden&&setsD44==0&&unhidesD44==0,"unprepared folded character never placed or shown");
 preparedD44=true;frame(1016);
 ck(!paperD44.hidden&&setsD44==1&&unhidesD44==1,"receipt precedes first placement and show");
 preparedD44=false;frame(1032);
 ck(paperD44.hidden&&setsD44==1,"lost current preparation receipt hides immediately");
 preparedD44=true;frame(1048);trackedD44=false;frame(1064);
 ck(paperD44.hidden&&!feedD44,"tracking loss hides and cannot keep requesting preparation");
 trackedD44=true;frame(1080);SetRequested31(false,1081);nowD44=1081;NativeFrameTick();
 ck(paperD44.hidden&&!shown.load()&&!feedD44,"B hides at native frame even without script ACK");
 Select(true);missingD44=true;frame(1100);
 ck(paperD44.hidden&&feedD44,"missing helper retains fresh bootstrap request without revealing");
 missingD44=false;Select(true);placementD44=false;frame(1300);
 ck(paperD44.hidden&&!shown.load(),"failed native character placement cannot reveal helper");
 placementD44=true;Select(true);frame(1500);
 ck(!paperD44.hidden,"new valid placement recovers after explicit intent");
 PublishAllowed(false,1501);nowD44=1501;NativeFrameTick();
 ck(paperD44.hidden&&!feedD44,"cutscene/menu/native-body gate retires visible letter");
 Select(true);preparedD44=true;frame(1600);preparedD44=false;hideFailsD44=true;frame(1616);
 ck(!paperD44.hidden&&shown.load(),"failed readiness-loss hide preserves outstanding visibility ownership");
 SetRequested31(false,1617);nowD44=1617;NativeFrameTick();
 ck(!paperD44.hidden&&shown.load(),"B cannot erase a still-failed native hide obligation");
 hideFailsD44=false;nowD44=1618;NativeFrameTick();
 ck(paperD44.hidden&&!shown.load(),"closed intent retries outstanding hide until native success");
 Select(true);preparedD44=true;frame(1700);trackedD44=false;hideFailsD44=true;frame(1716);
 ck(!paperD44.hidden&&shown.load(),"tracking-loss hide failure also retains visibility obligation");
 SetRequested31(false,1717);hideFailsD44=false;nowD44=1717;NativeFrameTick();
 ck(paperD44.hidden&&!shown.load(),"tracking-loss obligation retires only after successful hide");
 std::printf("D44 actual NativeFrameTick: %u checks, %u failures; native leaves only, headset NOT_RUN\n",checks,failures);
 return failures?1:0;
}
""".replace('ACTUAL_PRESENTER_D44',body)
    fixture=out/'actual_letter_presenter_d44.cpp';fixture.write_text(code)
    binary=out/'actual_letter_presenter_d44'
    build=subprocess.run(['c++','-std=c++17','-O2','-pthread','-I',str(root/'src'),str(fixture),'-o',str(binary)],capture_output=True,text=True)
    (out/'presenter_build.log').write_text(build.stdout+build.stderr)
    result['presenter_build']='PASS' if build.returncode==0 else 'FAIL'
    if not build.returncode:
        run=subprocess.run([str(binary)],capture_output=True,text=True)
        (out/'presenter_run.log').write_text(run.stdout+run.stderr)
        result['presenter']='PASS' if run.returncode==0 else 'FAIL';result['presenter_output']=run.stdout+run.stderr
(out/'results.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result,indent=2))
raise SystemExit(0 if result['lua']=='PASS' and (a.lua_only or (result.get('native')=='PASS' and result.get('presenter')=='PASS')) else 1)
