#include <atomic>
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
bool ReadHeldMapLeftPoseM1(float (&out)[8],unsigned* refusal,std::uintptr_t* sourceEntity,std::uint64_t* sourceEpoch,std::uint64_t* sourceTick,HeldMapPairM39* pair) noexcept {
 return ReadHeldMapCommittedPoseM41(1,out,refusal,sourceEntity,sourceEpoch,sourceTick,pair);
}
bool ReadHeldMapRightPoseM41(float (&out)[8],unsigned* refusal,std::uintptr_t* sourceEntity,std::uint64_t* sourceEpoch,std::uint64_t* sourceTick,HeldMapPairM39* pair) noexcept {
 return ReadHeldMapCommittedPoseM41(0,out,refusal,sourceEntity,sourceEpoch,sourceTick,pair);
}

// Optional second hand drives ONLY own parchment fold fraction. It never
// places Henry's right hand, occupies equipment or bypasses another hand owner.
bool ReadHeldMapFreeRightPoseM39(float (&out)[8],float& grip,const HeldMapPairM39* pair,bool* pairMismatch) noexcept {
    grip=0;if(pairMismatch)*pairMismatch=false;
    if(!pair||!pair->valid)return false; // M39P: never an unpaired fold end
    if(g_census_mode.load()<2||diagnostics::lab::NativeReference()||!CalibrationReady()||
       animation_filter::nativeBodyOwned112.load()||ExactHandItemState(0)!=0||
       NativeHeadAimEquipment120R3()==1)return false;
    CommittedFrame6695 p{};const auto now=std::uint64_t(GetTickCount64());
    if(!ReadCommittedPacket6695(p)||!p.mapFoldTrackedM39||!p.valid[0]||!TrackingHandUsable111(0,now)||
       !p.stamp.tick||now<p.stamp.tick||now-p.stamp.tick>80||p.stamp.epoch!=g_pose_epoch6689.load()||
       p.mapPresentationOwnerM34[0]||HandClaimBlockedT24(0,p.stamp.actor,p.stamp.epoch,g_weapon_request_epoch6694.load())||
       kcd2vr::native_passenger_a34::current(p.stamp.actor,p.stamp.epoch,now))return false;
    // M39P: same completed publication as the LEFT end (full stamp and world frame),
    // otherwise common movement between two packets reads as a false fold.
    if(p.stamp.actor!=pair->actor||p.stamp.epoch!=pair->epoch||p.stamp.sample!=pair->sample||p.stamp.tick!=pair->tick||
       std::memcmp(p.world,pair->world,sizeof(pair->world))!=0){if(pairMismatch)*pairMismatch=true;return false;}
    // Display grace cannot earn fold authority. The LEFT hand supplies the
    // other end of the distance, so both current hands and head must be tracked.
    TrackingSnapshot live{};
    if(!ReadTrackingSnapshot(live)||!live.allowed||!live.head_valid||
       !live.right_valid||!live.left_valid||live.head_held||live.right_held||
       live.left_held||!live.tick||now<live.tick||now-live.tick>80)return false;
    const auto pose=HandWorld6694(p.allowed[0],p.world);
    for(unsigned i=0;i<4;++i)out[i]=pose.q[i];for(unsigned i=0;i<3;++i)out[i+4]=pose.t[i];out[7]=1;
    for(float f:out)if(!std::isfinite(f))return false;
    grip=p.mapGripM39[0];return std::isfinite(grip)&&grip>=0&&grip<=1;
}

// D64 second-hand paper support uses the exact same committed frame and yields
// to all existing hand owners. This is presentation only, never item ownership.
bool ReadHeldMapSupportLeftD64(float (&out)[8],float& grip,const HeldMapPairM39& primary) noexcept {
 grip=0;HeldMapPairM39 pair{};std::uintptr_t who=0;std::uint64_t body=0,when=0;
 if(!primary.valid||!primary.publicationR24||!ReadHeldMapCommittedPoseM41(1,out,nullptr,&who,&body,&when,&pair)||
    pair.actor!=primary.actor||pair.epoch!=primary.epoch||pair.sample!=primary.sample||pair.tick!=primary.tick||pair.publicationR24!=primary.publicationR24||
    std::memcmp(pair.world,primary.world,sizeof(pair.world))!=0)return false;
 CommittedFrame6695 p{};const auto now=std::uint64_t(GetTickCount64());
 if(!ReadCommittedPacket6695(p)||!p.mapFoldTrackedM39||p.stamp.actor!=pair.actor||p.stamp.epoch!=pair.epoch||
    p.stamp.sample!=pair.sample||p.stamp.tick!=pair.tick||p.publicationR24!=pair.publicationR24||std::memcmp(p.world,pair.world,sizeof(p.world))!=0)return false;
 TrackingSnapshot live{};
 if(!ReadTrackingSnapshot(live)||!live.allowed||!live.head_valid||!live.left_valid||!live.right_valid||
    live.head_held||live.left_held||live.right_held||!live.tick||now<live.tick||now-live.tick>80)return false;
 grip=p.mapGripM39[1];return std::isfinite(grip)&&grip>=0&&grip<=1;
}
// Included after committed_pose_service.inl. A paper placement needs one
// committed packet, not a new mailbox read for each hand and its grip value.
// Native-boundary ownership rechecks stay in NativeFrameTick.
bool ReadHeldMapSnapshotD78R1(HeldMapSnapshotD78R1& out,bool includeLeft,unsigned* refusal) noexcept {
    out={};
    if(!HeldMapHandAdmittedD78R1(0,refusal))return false;
    CommittedFrame6695 p{};const auto now=std::uint64_t(GetTickCount64());
    if(!ReadCommittedPacket6695(p)){if(refusal)*refusal=6;return false;}
    HeldMapSnapshotD78R1 next{};
    if(!HeldMapPoseFromPacketD78R1(0,p,now,next.right,refusal,&next.sourceEntity,
        &next.sourceEpoch,&next.sourceTick,&next.pair))return false;
    if(includeLeft&&p.mapFoldTrackedM39&&p.publicationR24&&HeldMapHandAdmittedD78R1(1,nullptr)){
        float left[8]{};
        if(HeldMapPoseFromPacketD78R1(1,p,now,left,nullptr,nullptr,nullptr,nullptr,nullptr)){
            // Display grace may keep paper visible, but cannot grant support.
            // This is the same strict live head/both-hand check as D64.
            TrackingSnapshot live{};
            const float grip=p.mapGripM39[1];
            if(ReadTrackingSnapshot(live)&&live.allowed&&live.head_valid&&live.left_valid&&live.right_valid&&
                !live.head_held&&!live.left_held&&!live.right_held&&live.tick&&now>=live.tick&&now-live.tick<=80&&
                std::isfinite(grip)&&grip>=0&&grip<=1){
                std::memcpy(next.left,left,sizeof(left));next.leftGrip=grip;next.haveLeft=true;
            }
        }
    }
    out=next;
    return true;
}
bool ReadMiscGestureBodyD59(MiscGestureBodyD59& out) noexcept {
    out={};const auto now=std::uint64_t(GetTickCount64());
    if(diagnostics::lab::NativeReference()||!CalibrationReady()||!retail_build112::Ready(WhBase())||
       g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_reading.load()||
       animation_filter::nativeBodyOwned112.load()||animation_filter::NativeKickActive175()||
       kcd2vr::activity_input163::witness175.Active())return false;
    CommittedFrame6695 p{};TrackingSnapshot live{};
    if(!ReadCommittedPacket6695(p)||!ReadTrackingSnapshot(live)||!live.allowed||!live.head_valid||live.head_held||
       !live.tick||now<live.tick||now-live.tick>60||!p.stamp.tick||now<p.stamp.tick||now-p.stamp.tick>80||
       !p.doorPlayer38||!p.doorTrackingEpoch66||p.doorTrackingEpoch66!=live.source_epoch||
       live.source_epoch!=tracking59c6::epoch.load(std::memory_order_acquire)||NativePassengerCurrentA34(now)||
       p.doorPlayer38!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire)))return false;
    // Same exact CEntity identity/read used by the D58 map bootstrap. No call
    // through a new native slot and no entity lookup on the input thread.
    std::uintptr_t vt=0;std::uint32_t id=0;
    if(!Read(reinterpret_cast<void*>(p.doorPlayer38),vt)||vt!=WhBase()+0x4072C78||
       !Read(reinterpret_cast<void*>(p.doorPlayer38+0xC),id)||!id)return false;
    MiscGestureBodyD59 next{};next.player=p.doorPlayer38;next.character=p.stamp.actor;next.playerId=id;
    next.tick=p.stamp.tick;next.body=p.stamp.epoch;next.tracking=p.doorTrackingEpoch66;
    const Q4 q{p.world[0],p.world[1],p.world[2],p.world[3]};
    for(int side=0;side<2;++side){
        if(!p.valid[side]||!p.palmValid59c9[side]||(side?!live.left_valid:!live.right_valid)||
           (side?live.left_held:live.right_held)||!TrackingHandUsable111(side,now)||
           HandClaimBlockedT24(side,p.stamp.actor,p.stamp.epoch,g_weapon_request_epoch6694.load()))continue;
        const V3 origin=Add(QRotateV(q,Mul(p.palm59c9[side],p.world[7])),{p.world[4],p.world[5],p.world[6]});
        if(!pc::finite(PV(origin)))continue;
        next.palm[side][0]=origin.x;next.palm[side][1]=origin.y;next.palm[side][2]=origin.z;next.valid[side]=true;
    }
    if(!next.valid[0]&&!next.valid[1])return false;
    if(p.publicationR24!=ReadCompletedPublicationT23()||p.stamp.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       p.doorPlayer38!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire)))return false;
    out=next;return true;
}
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
