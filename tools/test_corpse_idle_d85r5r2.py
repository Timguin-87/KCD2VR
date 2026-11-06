#!/usr/bin/env python3
"""Execute production corpse Lua and native scheduling/proximity helpers.

Native leaves are counted substitutes. Lua is system 5.4, not retail 5.1; no
game/headset performance or successful native body movement is claimed.
The optional baseline runs the SAME scenario against the previous source.
"""
import argparse
import ctypes
import ctypes.util
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
LUA = r'''
local counts={scans=0,bones=0,bounds=0,writes=0,lookups=0,death=0,positions=0,clock=0};local entities,list={},{};local clockTime=1
g_localActor={id=1};player=g_localActor
System={GetEntity=function(id)return entities[id]end,
 GetEntitiesInSphere=function(p,r)assert(r==1.6);counts.scans=counts.scans+1;return list end}
local function setup(n,x,box)
 counts={scans=0,bones=0,bounds=0,writes=0,lookups=0,death=0,positions=0,clock=0};entities={};list={};clockTime=1
 System={GetEntity=function(id)counts.lookups=counts.lookups+1;return entities[id]end,
  GetCurrTime=function()counts.clock=counts.clock+1;return clockTime end,
  GetEntitiesInSphere=function(p,r)
   assert(r==1.6 or r==2.4);counts.scans=counts.scans+1;local found={}
   for _,e in pairs(list)do local o=e.origin or e.p
    if (p.x-o.x)^2+(p.y-o.y)^2+(p.z-o.z)^2<=r*r then found[#found+1]=e end
   end
   return found
  end}
 for i=1,n do
  local e={id=100+i,actor={IsDead=function()counts.death=counts.death+1;return true end},p={x=x,y=0,z=0}}
  function e:GetBonePos(name)counts.bones=counts.bones+1;if name=='RightHand'then return self.p end end
  function e:GetWorldPos()counts.positions=counts.positions+1;return self.origin or self.p end
  function e:SetVelocity(v)counts.writes=counts.writes+1 end
  if box~='missing'then function e:GetWorldBBox()
   counts.bounds=counts.bounds+1
   if box=='error'then error('no render proxy')end
   if box=='nan'then return {x=0/0,y=0,z=0},{x=1,y=1,z=1}end
   if box=='zero'then return {x=0,y=0,z=0},{x=0,y=0,z=0}end
   return {x=x-.2,y=-.2,z=-.2},{x=x+.2,y=.2,z=.2}
  end end
  list[i]=e;entities[e.id]=e
 end
end
local function call(op,side,id,token,x)
 local out=kcd2vr_corpse_d72('D72|'..op..'|22_1_1|'..(side or 0)..'|1|'..(id or 0)..'|'..(token or 0)..'|'..(x or 0)..'|0|0')
 local t={};for s in out:gmatch('[^|]+')do t[#t+1]=s end
 assert(t[1]=='D72'and t[2]=='1',out);return t,out
end
assert(counts.scans==0 and counts.bones==0 and counts.writes==0) -- loaded but never called
setup(32,1.0);local far,farWire=call('q');assert(far[9]=='NO_NEAR_DEAD_BONE'and counts.writes==0)
local farScans,farBones,farBounds=counts.scans,counts.bones,counts.bounds
local improved=farWire:find('|BBOX|',1,true)~=nil
if improved then assert(farBones==0 and farBounds==32)else assert(farBones==32*31 and farBounds==0)end
setup(32,1.0);call('q',0);call('q',1)
local pairedScans,pairedBones=counts.scans,counts.bones
local pairedBounds,pairedPositions,pairedLookups,pairedDeath,pairedClock=counts.bounds,counts.positions,counts.lookups,counts.death,counts.clock
assert(pairedScans==(improved and 1 or 2)and pairedBones==(improved and 0 or 1984))
setup(0,0);call('Q',0);call('Q',1)
local emptyScans,emptyClock=counts.scans,counts.clock
assert(emptyScans==(improved and 1 or 2)and counts.bones==0 and counts.bounds==0)
setup(1,1.0);System.GetCurrTime=nil;call('Q',0);call('Q',1);assert(counts.scans==2)
setup(1,1.0);call('Q',0);clockTime=1.031;call('Q',1);assert(counts.scans==2)
setup(1,1.0);call('Q',0);call('Q',1,0,0,1.0);assert(counts.scans==2)
setup(1,.1);call('Q',0);entities[101]=nil
local replaced=call('Q',1);assert(replaced[9]=='NO_NEAR_DEAD_BONE'and counts.writes==0)
-- The expanded list cannot spend the 32-body budget on bodies outside the
-- original sphere, even if their skeleton point itself happens to be close.
setup(33,.1)
for i=1,32 do list[i].origin={x=2,y=0,z=0} end
local budgeted=call('Q',0);assert(budgeted[9]=='CANDIDATE'and tonumber(budgeted[3])==133)
-- An enlarged sphere overflowing the cap falls back to the original query.
setup(1,.1);local original=System.GetEntitiesInSphere
System.GetEntitiesInSphere=function(p,r)
 if r==2.4 then counts.scans=counts.scans+1;local dense={};for i=1,4097 do dense[i]=list[1]end;return dense end
 return original(p,r)
end
local dense=call('q');assert(dense[9]=='CANDIDATE'and counts.scans==(improved and 2 or 1))
-- A position failure in a widened discovery never denies the old query.
setup(1,.1);list[1].GetWorldPos=nil
local missing=call('q');assert(missing[9]=='CANDIDATE'and counts.scans==(improved and 2 or 1))
setup(1,.1);local near=call('Q');assert(near[9]=='CANDIDATE'and counts.bones==31 and counts.writes==0)
local id,token=near[3],near[4]
local v=call('V',0,id,token);assert(v[9]=='CANDIDATE'and counts.writes==0)
local a=call('A',0,id,token);assert(a[9]=='ACQUIRED'and counts.writes==0)
local other=call('Q',1);assert(other[9]=='NO_NEAR_DEAD_BONE'and counts.writes==0)
local d=call('D',0,id,token,.2);assert(d[9]=='VELOCITY_RETURNED'and counts.writes==1)
local release=call('R',0);assert(release[9]=='RELEASED'and counts.writes==1)
local left=call('Q',1);assert(left[9]=='CANDIDATE')
local held=call('A',1,left[3],left[4]);assert(held[9]=='ACQUIRED'and counts.writes==1)
entities[tonumber(left[3])]=nil;local gone=call('D',1,left[3],left[4],.2)
assert(gone[9]=='OWNER_CHANGED'and counts.writes==1)
for _,kind in ipairs({'missing','error','nan','zero'})do
 setup(1,.1,kind);local t=call('Q');assert(t[9]=='CANDIDATE'and counts.bones==31 and counts.writes==0)
end
setup(1,.1);call('Q');local plain={counts.scans,counts.bones,counts.bounds,counts.writes}
setup(1,.1);call('q');assert(counts.scans==plain[1]and counts.bones==plain[2]and counts.bounds==plain[3]and counts.writes==plain[4])
RESULTS=string.format('{"far_32":{"sphere_calls":%d,"bone_calls":%d,"bounds_calls":%d},"far_32_both_hands":{"sphere_calls":%d,"bone_calls":%d,"bounds_calls":%d,"position_calls":%d,"entity_lookups":%d,"death_calls":%d,"clock_calls":%d},"empty_both_hands":{"sphere_calls":%d,"clock_calls":%d},"shared_list_age_distance_identity_checks":true,"original_recipient_budget_preserved":true,"missing_clock_position_dense_fallback":true,"near_candidate_before_grip":true,"both_hands":true,"no_acquire_release_velocity_write":true,"lost_owner_stops_drive":true,"unknown_bounds_preserve_grab":true,"f9_same_gameplay_calls":true}',farScans,farBones,farBounds,pairedScans,pairedBones,pairedBounds,pairedPositions,pairedLookups,pairedDeath,pairedClock,emptyScans,emptyClock)
'''
STATIC = r'''
local physicalize,wake,impulse=0,0,0
local e={id=8,class='BasicEntity',Properties={object_Model='objects/household/cooking_eating/tankards/test.cgf'}}
function e:GetMass()return self.mass or 0 end
function e:GetWorldPos()return {x=0,y=0,z=0}end
function e:Physicalize(slot,kind,p)physicalize=physicalize+1;self.mass=p.mass end
function e:AwakePhysics()wake=wake+1 end
function e:AddImpulse()impulse=impulse+1 end
System={GetEntity=function(id)return id==8 and e or nil end,GetCVar=function()return 0 end,GetCurrTime=function()return 1 end}
PE_RIGID=2
assert(KCD2VR_Lab.Wake({id=8,d={x=1,y=0,z=0},p={x=0,y=0,z=0},speed=1})~=true)
assert(physicalize==0 and wake==0 and impulse==0)
e.mass=1
assert(KCD2VR_Lab.Wake({id=8,d={x=1,y=0,z=0},p={x=0,y=0,z=0},speed=1})==true)
assert(physicalize==0 and wake==1 and impulse==1)
RESULTS='{"static_BasicEntity":{"physicalize_calls":0,"wake_calls":0,"result":"REFUSED_UNFINISHED"},"already_rigid_BasicEntity":{"physicalize_calls":0,"wake_calls":1,"impulse_calls":1}}'
'''


def execute_lua(source, fixture):
    lib = ctypes.CDLL(ctypes.util.find_library('lua5.4'))
    lib.luaL_newstate.restype = ctypes.c_void_p
    lib.luaL_openlibs.argtypes = [ctypes.c_void_p]
    lib.luaL_loadbufferx.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_size_t, ctypes.c_char_p, ctypes.c_char_p]
    lib.lua_pcallk.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_longlong, ctypes.c_void_p]
    lib.lua_getglobal.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    lib.lua_tolstring.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p]
    lib.lua_tolstring.restype = ctypes.c_char_p
    lib.lua_close.argtypes = [ctypes.c_void_p]
    state = lib.luaL_newstate()
    lib.luaL_openlibs(state)
    try:
        for text in [source.read_bytes(), fixture.encode()]:
            code = lib.luaL_loadbufferx(state, text, len(text), b'corpse-idle-test', None)
            if not code:
                code = lib.lua_pcallk(state, 0, 0, 0, 0, None)
            if code:
                raise RuntimeError(lib.lua_tolstring(state, -1, None).decode())
        lib.lua_getglobal(state, b'RESULTS')
        return json.loads(lib.lua_tolstring(state, -1, None).decode())
    finally:
        lib.lua_close(state)


def function(source, signature):
    start = source.index(signature)
    at = source.index('{', start) + 1
    depth = 1
    while depth:
        depth += (source[at] == '{') - (source[at] == '}')
        at += 1
    return source[start:at]


def native(out):
    source = (ROOT / 'src/subsystems/physics/saved_corpse_drag_d72.inl').read_text()
    start = source.index('   const auto queryStartD85R5R2=')
    timed = source[start:source.index('   if(!queried', start)].replace('std::chrono::steady_clock', 'CountedClock')
    cpp = r'''
#include "subsystems/physics/corpse_search_budget_d85r5r2.h"
#include "subsystems/physics/corpse_grip_cg1.h"
#include "vrik/lab_prop58.h"
#include <cstdio>
#include <cassert>
#include <cstring>
#include <chrono>
namespace L {enum class Sub{CorpseDrag};static bool selected=false;bool active(Sub){return selected;}}
static std::uint64_t clockNow=1;
static std::uint64_t GetTickCount64(){return clockNow;}
static std::atomic<bool> owners[2]{},g_savedClearD72[2]{};
static std::atomic<std::uint64_t> g_savedClearDueD72[2]{},g_savedSearchDueD85R5R2[2]{};
static bool SavedDragOwnsD72(int side){return owners[side];}
// R90: 'Corpse search' route switch and per-hand squeeze window (R89) leaves.
static bool squeezeR90=false,windowR90[2]{};
static bool SavedSqueezeRouteR90(){return squeezeR90;}
static bool SavedSearchWindowR90(int side,std::uint64_t){return windowR90[side];}
''' + function(source, 'static bool SavedTakeSearchD85R5R2(') + '\n' + function(source, 'static bool SavedWantedD72(') + r'''
static unsigned clocks=0,queryCount=0,queryReceipts=0;static char lastOp=0;
struct CountedClock {
 using time_point=std::chrono::steady_clock::time_point;
 static time_point now(){++clocks;return std::chrono::steady_clock::now();}
};
static bool SavedCallD72(int,int,char op,int,int&){++queryCount;lastOp=op;return true;}
static void SavedQueryAuditD85R5R1(int,int,int,std::uint64_t,double elapsed){assert(std::isfinite(elapsed)&&elapsed>=0);++queryReceipts;}
static void RunQueryTiming(bool recordQueryD85R5R1){int context=0,side=0,hand=0,answer=0;std::uint64_t now=1;
''' + timed + r'''
}
int main(){
 RunQueryTiming(false);assert(clocks==0&&queryCount==1&&queryReceipts==0&&lastOp=='Q');
 RunQueryTiming(true);assert(clocks==2&&queryCount==2&&queryReceipts==1&&lastOp=='q');
 unsigned off=0,idle=0,idleHands=0,held=0,otherSearches=0;
 for(clockNow=1;clockNow<=1000;++clockNow)if(SavedWantedD72())++off;
 assert(off==0);L::selected=true;
 for(clockNow=1001;clockNow<=2000;++clockNow)if(SavedWantedD72()){
  ++idle;for(int side=0;side<2;++side)if(SavedTakeSearchD85R5R2(side,clockNow))++idleHands;
 }
 assert(idle==10&&idleHands==20);
 owners[0]=true;
 for(clockNow=2001;clockNow<=3000;++clockNow)if(SavedWantedD72()){
  ++held;if(SavedTakeSearchD85R5R2(1,clockNow))++otherSearches;
 }
 assert(held==1000&&otherSearches==10);owners[0]=false;L::selected=false;
 assert(!SavedWantedD72());g_savedClearD72[1]=true;g_savedClearDueD72[1]=clockNow;
 assert(SavedWantedD72());g_savedClearD72[1]=false;assert(!SavedWantedD72());
 // R90 ONLY ON SQUEEZE: open hands cost nothing; a hand inside its squeeze
 // window keeps the R2 10 Hz budget; owners/cleanup keep their cadence.
 unsigned squeezeIdle=0,squeezeWindow=0,squeezeHeld=0;
 L::selected=true;squeezeR90=true;
 for(clockNow=5001;clockNow<=6000;++clockNow)if(SavedWantedD72())++squeezeIdle;
 assert(squeezeIdle==0);windowR90[1]=true;
 for(clockNow=6001;clockNow<=7000;++clockNow)if(SavedWantedD72()){
  ++squeezeWindow;assert(SavedTakeSearchD85R5R2(1,clockNow));}
 assert(squeezeWindow==10);windowR90[1]=false;owners[0]=true;
 for(clockNow=7001;clockNow<=8000;++clockNow)if(SavedWantedD72())++squeezeHeld;
 assert(squeezeHeld==1000);owners[0]=false;squeezeR90=false;L::selected=false;
 using namespace kcd2vr::corpse_search_d85r5r2;
 const float box[]{-.2f,-.2f,-.2f,.2f,.2f,.2f};
 const float world[]{1,0,0,0,0,1,0,0,0,0,1,0};
 assert(outsideWorldBounds(box,world,1,0,0));assert(!outsideWorldBounds(box,world,.3,0,0));
 const float rotated[]{0,-2,0,10,2,0,0,20,0,0,2,30};
 assert(!outsideWorldBounds(box,rotated,10.3,20,30));assert(outsideWorldBounds(box,rotated,12,20,30));
 const float zero[6]{};assert(!outsideWorldBounds(zero,world,10,10,10));
 const float collapsed[12]{};assert(!outsideWorldBounds(box,collapsed,10,10,10));
 const float collinear[]{1,2,0,0,2,4,0,0,0,0,1,0};assert(!outsideWorldBounds(box,collinear,10,10,10));
 float invalid[6]{-.2f,-.2f,-.2f,.2f,.2f,.2f};invalid[0]=NAN;
 assert(!outsideWorldBounds(invalid,world,10,10,10));
 namespace C=kcd2vr::corpse_grip_cg1;C::Gate gate;C::Key key{};
 key.actor=1;key.identity=2;key.root=3;key.epoch=1;key.serial=1;key.entity=4;key.part=0;key.physicsEpoch=1;
 C::Candidate candidate{key,4000,false};
 gate.update(4000,true,true,0,candidate);auto grab=gate.update(4001,true,true,1,candidate);
 assert(grab.suppressLeftUse&&C::permits(grab.native,candidate,4001));
 auto release=gate.update(4002,true,true,0,candidate);assert(!release.suppressLeftUse&&!release.native.press);
 namespace P=kcd2vr::lab_prop58;P::Contact contact{};
 contact.active=contact.pakPresent=contact.luaRoute=contact.distKnown=true;contact.entity=8;contact.speed=1;contact.dist=0;
 contact.body=P::Physical;assert(!std::strcmp(P::refuse(contact),"STATIC_BODY"));
 contact.body=P::Rigid;assert(!P::refuse(contact));
 std::printf("{\"off_service_calls_per_1000_callbacks\":%u,\"enabled_idle_service_calls_per_second\":%u,\"enabled_idle_hand_checks_per_second\":%u,\"held_service_calls_per_1000_callbacks\":%u,\"other_idle_hand_checks_per_second\":%u,\"release_cleanup_with_feature_off\":true,\"fast_grip_current_candidate\":true,\"bounds_rotation_invalid_fallback\":true,\"sample_timing_clock_calls_off\":0,\"sample_timing_clock_calls_on\":2,\"r90_squeeze_open_hand_calls_per_1000\":%u,\"r90_squeeze_window_calls_per_second\":%u,\"r90_squeeze_held_calls_per_1000\":%u}\n",off,idle,idleHands,held,otherSearches,squeezeIdle,squeezeWindow,squeezeHeld);
}
'''
    path = out / 'corpse_idle_native.cpp'
    path.write_text(cpp)
    exe = out / 'corpse_idle_native'
    subprocess.run(['g++', '-std=c++17', '-O2', '-I' + str(ROOT / 'src'), str(path), '-o', str(exe)], check=True, capture_output=True, text=True)
    result = subprocess.run([str(exe)], check=True, capture_output=True, text=True)
    exe.unlink()
    return json.loads(result.stdout)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--baseline', type=Path)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    report = {'runtime': 'Lua 5.4 and host C++17; modeled native leaves',
              'current': execute_lua(ROOT / 'companion/source/zzkcd2vr_corpse_d72.lua', LUA),
              'native': native(args.out),
              'static_prop_actual_path': execute_lua(ROOT / 'companion/Mods/kcd2vr_lab/Scripts/Startup/zzkcd2vr_lab.lua', STATIC)}
    if args.baseline:
        report['baseline'] = execute_lua(args.baseline / 'companion/source/zzkcd2vr_corpse_d72.lua', LUA)
    (args.out / 'corpse_idle_result.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
