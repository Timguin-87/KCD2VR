import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib,re
r=Path(__file__).resolve().parents[1]
source=read_source(r/'src/frik_pose_probe.cpp');begin=source.index('bool ReadHeldMapLeftPoseM1(');end=source.index('bool ReadInteractionHandWorld123R1',begin);function=source[begin:end].strip()
test='''#include <atomic>
#include <cstdint>
#include <cmath>
#include <cassert>
#include <cstdio>
#include "../src/subsystems/interactions/native_passenger_a34.h"
struct Stamp{std::uint64_t tick=1000,epoch=1;std::uintptr_t actor=0x2345;};
struct Pose{float q[4]{0,0,0,1},t[3]{1,2,3};};
struct CommittedFrame6695{Stamp stamp{};bool valid[2]{true,true};Pose allowed[2]{};float world[8]{};std::uintptr_t doorPlayer38=0x1234;std::uint32_t mapPresentationOwnerM34[2]{~0u,~0u};};
std::atomic<int> g_mode{0},g_census_mode{2};
std::atomic<std::uint64_t> g_pose_epoch6689{1};std::atomic<void*> g_last_player_entity{reinterpret_cast<void*>(0x1234)};
namespace diagnostics::lab {bool reference=false;bool NativeReference(){return reference;}}
namespace animation_filter {std::atomic<bool> nativeBodyOwned112{false};}
bool calibration=true,packetOK=true,tracking=true;int item=0,equipment=0;std::uint64_t clockNow=1000;unsigned worldCalls=0;
CommittedFrame6695 packet{};
bool CalibrationReady(){return calibration;}int ExactHandItemState(int side){assert(side==1);return item;}
int NativeHeadAimEquipment120R3(){return equipment;}
std::uint64_t GetTickCount64(){return clockNow;}
bool ReadHeldMapLeftPoseM1(float (&out)[8],unsigned* refusal=nullptr,std::uintptr_t* sourceEntity=nullptr,std::uint64_t* sourceEpoch=nullptr,std::uint64_t* sourceTick=nullptr) noexcept;
bool ReadCommittedPacket6695(CommittedFrame6695& p){p=packet;return packetOK;}
bool TrackingHandUsable111(int side,std::uint64_t){assert(side==1);return tracking;}
Pose HandWorld6694(Pose p,const float*){++worldCalls;return p;}
'''+function+'''
int main(){float out[8]{};
unsigned ownerRefusal=0;assert(!ReadHeldMapLeftPoseM1(out,&ownerRefusal)&&ownerRefusal==9);
packet.mapPresentationOwnerM34[0]=1;packet.mapPresentationOwnerM34[1]=0; // current right weapon does not claim left

assert(ReadHeldMapLeftPoseM1(out)&&out[4]==1&&out[5]==2&&out[6]==3&&out[7]==1&&worldCalls==1);
item=1;assert(!ReadHeldMapLeftPoseM1(out));item=-1;assert(!ReadHeldMapLeftPoseM1(out));item=0;
equipment=1;assert(!ReadHeldMapLeftPoseM1(out));equipment=0;
packet.stamp.tick=919;assert(!ReadHeldMapLeftPoseM1(out));packet.stamp.tick=1001;assert(!ReadHeldMapLeftPoseM1(out));packet.stamp.tick=1000;
packet.valid[1]=false;assert(!ReadHeldMapLeftPoseM1(out));packet.valid[1]=true;
packetOK=false;assert(!ReadHeldMapLeftPoseM1(out));packetOK=true;
tracking=false;assert(!ReadHeldMapLeftPoseM1(out));tracking=true;
calibration=false;assert(!ReadHeldMapLeftPoseM1(out));calibration=true;
diagnostics::lab::reference=true;assert(!ReadHeldMapLeftPoseM1(out));diagnostics::lab::reference=false;
animation_filter::nativeBodyOwned112=true;assert(!ReadHeldMapLeftPoseM1(out));animation_filter::nativeBodyOwned112=false;
// Exercise the tuples the real mutually-exclusive setters actually produce.
unsigned reason=99;g_mode=0;g_census_mode=4;assert(ReadHeldMapLeftPoseM1(out,&reason)&&reason==0);
g_census_mode=0;g_mode=4;assert(!ReadHeldMapLeftPoseM1(out,&reason)&&reason==1);
g_mode=0;g_census_mode=1;assert(!ReadHeldMapLeftPoseM1(out,&reason)&&reason==1);g_census_mode=2;
assert(worldCalls==2);
std::uintptr_t entity=0;std::uint64_t epoch=0,tick=0;
assert(ReadHeldMapLeftPoseM1(out,&reason,&entity,&epoch,&tick)&&entity==0x1234&&epoch==1&&tick==1000);
g_pose_epoch6689=2;assert(ReadHeldMapLeftPoseM1(out,&reason,&entity,&epoch)&&!entity&&!epoch);g_pose_epoch6689=1;
g_last_player_entity=nullptr;assert(ReadHeldMapLeftPoseM1(out,&reason,&entity,&epoch)&&!entity&&!epoch);g_last_player_entity=reinterpret_cast<void*>(0x1234);
item=1;entity=99;epoch=99;assert(!ReadHeldMapLeftPoseM1(out,&reason,&entity,&epoch)&&!entity&&!epoch);
item=0;
for(unsigned bit=1;bit<=512;bit*=2){packet.mapPresentationOwnerM34[1]=bit;assert(!ReadHeldMapLeftPoseM1(out,&ownerRefusal)&&ownerRefusal==9);}
packet.mapPresentationOwnerM34[1]=0;assert(ReadHeldMapLeftPoseM1(out));
namespace passenger=kcd2vr::native_passenger_a34;
passenger::Root native{};native.controller=1;native.character=packet.stamp.actor;native.action=2;native.scope=3;native.context=4;
native.epoch=packet.stamp.epoch;native.sourceTick=native.tick=clockNow;native.serial=1;native.fragment=6;native.status=2;native.layer=0;native.crc=0xeed2e729u;
assert(passenger::witness.publish(native));const auto callsBeforePassenger=worldCalls;
assert(!animation_filter::nativeBodyOwned112.load()&&!ReadHeldMapLeftPoseM1(out,&reason)&&reason==3&&worldCalls==callsBeforePassenger);
native.character+=8;assert(passenger::witness.publish(native));assert(ReadHeldMapLeftPoseM1(out));native.character=packet.stamp.actor;
++native.epoch;assert(passenger::witness.publish(native));assert(ReadHeldMapLeftPoseM1(out));native.epoch=packet.stamp.epoch;
assert(passenger::witness.publish(native));clockNow=1081;packet.stamp.tick=clockNow;assert(ReadHeldMapLeftPoseM1(out));
clockNow=packet.stamp.tick=1000;native.sourceTick=native.tick=1001;assert(passenger::witness.publish(native));assert(ReadHeldMapLeftPoseM1(out));
assert(passenger::witness.publish({}));assert(ReadHeldMapLeftPoseM1(out));
std::puts("PASS complete production left-map pose getter: occupied/unknown, stale/future, protected/native, calibration/tracking and same-packet current identity gates");}
'''
(r/'tests/held_map_getter_m1_test.cpp').write_text(test)
(r/'review/C7FM1').mkdir(parents=True,exist_ok=True)
(r/'review/C7FM1/GETTER_EXTRACTION.txt').write_text('Whole production function SHA256 '+hashlib.sha256(function.encode()).hexdigest()+'\nReadCommittedPacket and HandWorld native/frame leaves explicitly substituted. No actual game calls.\n')
print('complete map getter fixture regenerated')
