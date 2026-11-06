"""Compile whole actual committed-pose readers, substituting native leaves only."""
from pathlib import Path
import sys,subprocess,json,hashlib
from source_text import read_source
r=Path(__file__).resolve().parents[1];out=Path(sys.argv[1]).resolve();out.mkdir(parents=True,exist_ok=True)
sourceRoot=Path(sys.argv[2]) if len(sys.argv)>2 else r
body=(sourceRoot/'src/subsystems/vrmap/committed_pose_service.inl').read_text() # retain current shared primary reader as well
publisher=(r/'src/vrik/coherent_pose.inl').read_text();pa=publisher.index('    packet.mapFoldTrackedM39=');pb=publisher.index(';',pa)+1;publisher=publisher[pa:pb]
prefix='''#include <atomic>
#include <cstdint>
#include <cmath>
#include <cassert>
#include <cstdio>
#include <cstring>
struct Stamp{std::uint64_t tick=1000,epoch=1,sample=5;std::uintptr_t actor=0x2345;};
struct HeldMapPairM39 {bool valid=false;std::uintptr_t actor=0;std::uint64_t epoch=0,sample=0,tick=0,publicationR24=0;float world[8]{};};
struct Pose{float q[4]{0,0,0,1},t[3]{1,2,3};};
struct TrackingSnapshot{bool allowed=true,head_valid=true,right_valid=true,left_valid=true,head_held=false,right_held=false,left_held=false;std::uint64_t tick=1000;};
struct CommittedFrame6695{bool mapFoldTrackedM39=true;Stamp stamp{};std::uint64_t publicationR24=10;bool valid[2]{true,true};Pose allowed[2]{},fistSlotH31[2]{};bool fistSlotValidH31[2]{true,true};float mapGripM39[2]{.7f,.8f};float world[8]{};std::uintptr_t doorPlayer38=0x1234;std::uint32_t mapPresentationOwnerM34[2]{};};
std::atomic<int> g_census_mode{2};std::atomic<std::uint64_t> g_pose_epoch6689{1},g_weapon_request_epoch6694{1};
std::atomic<void*> g_last_player_entity{reinterpret_cast<void*>(0x1234)};
namespace diagnostics::lab {bool reference=false;bool NativeReference(){return reference;}}
namespace animation_filter {std::atomic<bool> nativeBodyOwned112{false};}
namespace kcd2vr::native_passenger_a34 {bool busy=false;bool current(std::uintptr_t,std::uint64_t,std::uint64_t){return busy;}}
bool calibration=true,packetOK=true,tracking[2]{true,true},claim[2]{};int item[2]{},equipment=0;std::uint64_t clockNow=1000;unsigned worldCalls=0;
CommittedFrame6695 packet{};TrackingSnapshot liveTracking{};bool liveOK=true;
bool ReadTrackingSnapshot(TrackingSnapshot& out){out=liveTracking;return liveOK;}
bool CalibrationReady(){return calibration;}int ExactHandItemState(int side){return item[side];}
int NativeHeadAimEquipment120R3(){return equipment;}
std::uint64_t GetTickCount64(){return clockNow;}
bool ReadHeldMapLeftPoseM1(float (&out)[8],unsigned* refusal=nullptr,std::uintptr_t* sourceEntity=nullptr,std::uint64_t* sourceEpoch=nullptr,std::uint64_t* sourceTick=nullptr,HeldMapPairM39* pair=nullptr) noexcept;
bool ReadHeldMapFreeRightPoseM39(float (&out)[8],float& grip,const HeldMapPairM39* pair=nullptr,bool* pairMismatch=nullptr) noexcept;
bool ReadCommittedPacket6695(CommittedFrame6695& p){p=packet;return packetOK;}
bool TrackingHandUsable111(int side,std::uint64_t){return tracking[side];}
bool HandClaimBlockedT24(int side,std::uintptr_t,std::uint64_t,std::uint64_t){return claim[side];}
Pose HandWorld6694(Pose p,const float*){++worldCalls;return p;}
'''
prefix+='#include \"'+str(r/'src/subsystems/vrmap/map_fold_m39.h').replace('\\','/')+'\"\n'
producer='void publishFoldPermission(){struct{TrackingSnapshot tracking;}tx{liveTracking};'+publisher+'}\n'
test='''
// M39P: the RIGHT end is only read against the LEFT end's exact publication.
static bool RightPaired(float (&out)[8],float& grip){HeldMapPairM39 pair{};float left[8]{};
 (void)ReadHeldMapLeftPoseM1(left,nullptr,nullptr,nullptr,nullptr,&pair);return ReadHeldMapFreeRightPoseM39(out,grip,&pair);}
int main(){float out[8]{},grip=0;unsigned why=0;
{HeldMapPairM39 pair{};bool mismatch=true;assert(!ReadHeldMapFreeRightPoseM39(out,grip)); // unpaired caller refused
 assert(ReadHeldMapLeftPoseM1(out,nullptr,nullptr,nullptr,nullptr,&pair)&&pair.valid&&pair.tick==1000&&pair.sample==5);
 assert(ReadHeldMapFreeRightPoseM39(out,grip,&pair,&mismatch)&&!mismatch);
 packet.stamp.sample=6;assert(!ReadHeldMapFreeRightPoseM39(out,grip,&pair,&mismatch)&&mismatch);packet.stamp.sample=5; // same tick, newer publication
 packet.world[4]=.06f;assert(!ReadHeldMapFreeRightPoseM39(out,grip,&pair,&mismatch)&&mismatch);packet.world[4]=0; // moved world frame
 packet.stamp.tick=1001;clockNow=1001;assert(!ReadHeldMapFreeRightPoseM39(out,grip,&pair,&mismatch)&&mismatch);packet.stamp.tick=clockNow=1000;
 packet.valid[1]=false;assert(!ReadHeldMapLeftPoseM1(out,nullptr,nullptr,nullptr,nullptr,&pair)&&!pair.valid);packet.valid[1]=true;}

packet.allowed[1].t[0]=99;packet.fistSlotH31[1].t[0]=7;
assert(ReadHeldMapLeftPoseM1(out)&&out[4]==7); // actual socket, never wrist fallback
assert(RightPaired(out,grip)&&grip==.7f);
packet.fistSlotValidH31[1]=false;assert(!ReadHeldMapLeftPoseM1(out,&why)&&why==11);packet.fistSlotValidH31[1]=true;
item[0]=1;assert(ReadHeldMapLeftPoseM1(out));assert(!RightPaired(out,grip));item[0]=0;
for(unsigned bit=1;bit<=512;bit*=2){packet.mapPresentationOwnerM34[0]=bit;assert(!RightPaired(out,grip));assert(ReadHeldMapLeftPoseM1(out));}
packet.mapPresentationOwnerM34[0]=0;claim[0]=true;assert(!RightPaired(out,grip));claim[0]=false;
claim[1]=true;assert(!ReadHeldMapLeftPoseM1(out,&why)&&why==10);claim[1]=false;
packet.stamp.tick=919;assert(!RightPaired(out,grip));packet.stamp.tick=1001;assert(!RightPaired(out,grip));packet.stamp.tick=1000;
g_pose_epoch6689=2;assert(!RightPaired(out,grip));g_pose_epoch6689=1;
tracking[0]=false;assert(!RightPaired(out,grip));tracking[0]=true;
kcd2vr::native_passenger_a34::busy=true;assert(!RightPaired(out,grip)&&!ReadHeldMapLeftPoseM1(out));kcd2vr::native_passenger_a34::busy=false;
animation_filter::nativeBodyOwned112=true;assert(!RightPaired(out,grip));animation_filter::nativeBodyOwned112=false;
calibration=false;assert(!RightPaired(out,grip));calibration=true;
packet.mapGripM39[0]=2;assert(!RightPaired(out,grip));packet.mapGripM39[0]=.7f;
assert(RightPaired(out,grip));
// Actual reader -> actual Gesture: a held/lost right pose cannot finish
// the80ms dwell while LEFT moves, even if display grace returns true.
for(int lost=0;lost<5;++lost){
 kcd2vr::map_fold_m39::Gesture fold;
 liveTracking={};packet.stamp.tick=clockNow=liveTracking.tick=1000;publishFoldPermission();
 auto step=[&](std::uint64_t t,float squeeze,float distance){
  clockNow=packet.stamp.tick=liveTracking.tick=t;packet.mapGripM39[0]=squeeze;
  const bool current=RightPaired(out,grip);
  return fold.next(55,t,true,current,grip,.01f,distance);
 };
 step(1000,0,.3f);assert(step(1001,1,.3f).fraction==1);
 assert(!step(1050,1,.065f).close);
 if(lost==0)liveTracking.right_held=true;
 if(lost==1)liveTracking.left_held=true;
 if(lost==2)liveTracking.head_held=true;
 if(lost==3)liveTracking.right_valid=false;
 if(lost==4)liveOK=false;
 // Keep previously completed strict permission true: live veto is necessary.
 assert(!step(1060,1,.065f).close);
 assert(!step(1130,1,.065f).close);
 assert(!step(1140,1,.065f).close);
 liveOK=true;
}
liveTracking={};liveTracking.right_held=true;publishFoldPermission();
liveTracking={};clockNow=packet.stamp.tick=liveTracking.tick=1200;
assert(!RightPaired(out,grip)); // recovered live, held completed pose
publishFoldPermission();assert(RightPaired(out,grip));
std::puts("PASS complete production committed map readers: one-publication LEFT/RIGHT pair, native LEFT socket, RIGHT held/claims/owners, stale/future/epoch/passenger/invalid grip rejection");}
'''
(out/'readers.cpp').write_text(prefix+producer+body+test)
vc='C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
cmd='@echo off\ncall "'+vc+'" >nul\ncd /d "'+str(out)+'"\ncl /nologo /std:c++latest /EHsc /Od /Zi /UNDEBUG /fsanitize=address readers.cpp /Fe:readers.exe\nif errorlevel 1 exit /b %errorlevel%\nreaders.exe\nexit /b %errorlevel%\n'
(out/'readers.cmd').write_text(cmd)
p=subprocess.run(['cmd','/d','/c',str(out/'readers.cmd')],capture_output=True,text=True);(out/'readers.log').write_text(p.stdout+p.stderr)
print(p.stdout+p.stderr)
assert p.returncode==0
(out/'RESULT.json').write_text(json.dumps({'passed':True,'actual_whole_readers_sha256':hashlib.sha256(body.encode()).hexdigest(),'native_leaves':'SUBSTITUTED','headset':False},indent=2))
