// Complete production frik TU. Native leaves are linked as neutral stubs;
// public ingress, fresh tracking checks, serial ownership and cancel are actual.
#include "../src/frik_pose_probe.cpp"
#include <cstdio>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace kcd2vr::retail_build112 {extern bool hostRetailVerified112;}
using namespace kcd2vr::frik_pose_probe;
static unsigned checks=0,failures=0;
static unsigned nativeSpecialEdgesH17=0;
static void __fastcall NativeSpecialLeafH17(void*,const std::uint64_t*,int,float){++nativeSpecialEdgesH17;}
template<class T>static void putH17(unsigned char* p,T value){std::memcpy(p,&value,sizeof(value));}
static unsigned char* nativeKickStateR19=nullptr;
static void __fastcall NativeKickZoneR19(void*,int zone,std::int64_t,bool){putH17(nativeKickStateR19+0xF8,zone);}
static void check(bool pass,const char* why){++checks;if(!pass){++failures;std::printf("FAIL %s\n",why);}}
static TrackingSnapshot fresh(){TrackingSnapshot t{};t.sequence=1;t.tick=GetTickCount64();
    t.source_epoch=kcd2vr::tracking59c6::epoch.load();
    t.head_good_tick=t.left_good_tick=t.right_good_tick=t.tick;
    t.allowed=t.head_valid=t.left_valid=t.right_valid=true;return t;}
static void ready(){
    g_combat_experiment175=true;g_special_allowed168=true;g_gameplay_allowed=true;g_cal_state=2;
    g_head_valid=g_left_valid=g_right_valid=true;g_kick_busy176=0;
    kcd2vr::animation_filter::nativeBodyOwned112=false;kcd2vr::animation_filter::nativeKickActive175=false;
    g_tracking_frames6695.publish(fresh());
}
int main(){
    ready();g_combat_experiment175=false;check(!RequestProceduralKick176()&&!g_kick_busy176,"F4 OFF leaves ingress inactive");
    ready();g_special_allowed168=false;check(!RequestProceduralKick176(),"menu/context quarantine rejects");
    ready();g_gameplay_allowed=false;check(!RequestProceduralKick176(),"gameplay off rejects");
    ready();g_cal_state=0;check(!RequestProceduralKick176(),"uncalibrated body rejects");
    ready();kcd2vr::animation_filter::nativeBodyOwned112=true;check(!RequestProceduralKick176(),"protected native body rejects");
    ready();kcd2vr::animation_filter::nativeKickActive175=true;check(!RequestProceduralKick176(),"native kick owns legs");
    for(int side=0;side<3;++side){ready();auto t=fresh();if(side==0)t.head_valid=false;if(side==1)t.left_valid=false;if(side==2)t.right_valid=false;
        g_tracking_frames6695.publish(t);check(!RequestProceduralKick176(),"invalid tracking rejects");}
    for(int side=0;side<3;++side){ready();auto t=fresh();if(side==0)t.head_held=true;if(side==1)t.left_held=true;if(side==2)t.right_held=true;
        g_tracking_frames6695.publish(t);check(!RequestProceduralKick176(),"held tracking cannot initiate kick");}
    ready();auto t=fresh();t.tick-=81;g_tracking_frames6695.publish(t);check(!RequestProceduralKick176(),"old packet rejects even with fresh coordinates");
    ready();check(RequestProceduralKick176(),"qualified public F4 ingress queues request");
    kcd2vr::kick176::Request a{},b{};check(g_kick_request176.read(a)&&a.serial==g_kick_busy176.load()&&a.cancel==g_kick_cancel176.load(),"mailbox matches current busy serial and cancel generation");
    check(!RequestProceduralKick176()&&g_kick_request176.read(b)&&b.serial==a.serial,"busy R3 cannot queue or replace request");
    CancelProceduralKick176();check(!g_kick_busy176&&g_kick_cancel176.load()!=a.cancel,"public cancel revokes request atomically");
    check(RequestProceduralKick176()&&g_kick_request176.read(b)&&b.serial!=a.serial&&b.tick==a.tick,"same-ms new request has independent serial");
    auto stale=a.serial;check(!g_kick_busy176.compare_exchange_strong(stale,0)&&g_kick_busy176==b.serial,"old completion CAS cannot clear new request");
    check(!RequestNativeSpecial168(),"F4 never silently sends native armed special");
    SetNativeSpecialContext168(false);check(!g_kick_busy176&&!RequestProceduralKick176(),"context loss cancels and quarantines public ingress");
    ready();check(RequestProceduralKick176(),"new eligible context can request again");
    const auto serial=g_kick_busy176.load();CancelProceduralKick176();check(!g_kick_busy176&&serial,"explicit cancellation releases pending ownership");
    ready();auto priorTracking=fresh();g_tracking_frames6695.publish(priorTracking);++hostTickCount64;
    check(RequestProceduralKick176()&&g_kick_request176.read(a)&&a.tick==priorTracking.tick+1,
        "actual public ingress accepts R3 one ms after latest tracking publication");
    kcd2vr::kick176::State scheduled{};kcd2vr::kick176::Input sample{};
    sample.request=a;sample.tick=priorTracking.tick;sample.now=GetTickCount64();sample.sample=priorTracking.sequence;
    sample.cancel=a.cancel;sample.epoch=1;sample.eligible=sample.legsReady=true;sample.forward={0,1,0};
    for(int side=0;side<2;++side){sample.hip[side]={side?-.1:.1,0,.9};sample.ankle[side]={side?-.1:.1,0,.1};sample.reach[side]=.82;}
    const auto waiting=kcd2vr::kick176::advance(scheduled,sample);
    check(!waiting.active&&!waiting.terminal&&!scheduled.seenRequest&&g_kick_busy176==a.serial,
        "actual ingress timestamp remains pending through older tracking callback");
    sample.tick=sample.now;++sample.sample;const auto started=kcd2vr::kick176::advance(scheduled,sample);
    check(started.started&&started.active&&started.serial==a.serial,"first current sample starts actual public request once");
    check(!kcd2vr::kick176::advance(scheduled,sample).started,"same sample never restarts actual public request");
    CancelProceduralKick176();
    // H17: actual service callers, not a model of Note/Service/cancellation.
    // Native request execution is not claimed: seed its original copied ingress
    // identity and exercise the production delayed presentation owner around it.
    auto beginService=[&](std::uint64_t tick){
        CancelProceduralKick176();hostTickCount64=tick;ready();g_pose_epoch6689=1;
        SetNativeSpecialContext168(true);SetKick26(1,0,730,true);
        kcd2vr::kick26::fallbackLeg=true;kcd2vr::kick26::nativeClipStart=0;
        b168::SpecialRequest r{tick,tick,g_special_context168.load(),g_pose_epoch6689.load(),1,2,3};
        g_special_request168.publish(r);g_special_pending168=true;
        NoteKickRequest26(true);
        check(kcd2vr::kick26::requestTick==tick,"H17 service retains original R3 request time");
    };
    auto serviceAt=[&](std::uint64_t tick){hostTickCount64=tick;g_tracking_frames6695.publish(fresh());ServiceKick26(tick);};
    beginService(20000);serviceAt(20260);
    const auto fallbackSerial=g_kick_busy176.load();
    check(fallbackSerial&&kcd2vr::kick26::requestTick==20000,"H17 live original request can draw its one fallback leg");
    serviceAt(20280);check(g_kick_busy176==fallbackSerial,"H17 repeated service never produces a second leg");
    beginService(21000);hostTickCount64=21010;SetNativeSpecialContext168(false);
    hostTickCount64=21100;SetNativeSpecialContext168(true);serviceAt(21260);
    check(!g_kick_busy176&&!kcd2vr::kick26::requestTick&&!kcd2vr::kick26::reachTick,
        "H17 canceled native R3 never revives as fallback after context returns");
    beginService(22000);hostTickCount64=22010;SetNativeSpecialContext168(false);
    hostTickCount64=22100;SetNativeSpecialContext168(true);kcd2vr::kick26::nativeClipStart=22200;serviceAt(22200);
    check(!g_kick_busy176&&!kcd2vr::kick26::reachTick,"H17 late native clip cannot revive a canceled R3");
    beginService(23000);SetKick26(0,0,730,true);SetKick26(1,0,730,true);serviceAt(23260);
    check(!g_special_pending168&&!g_kick_busy176&&!kcd2vr::kick26::reachTick,"H17 style switch away and back retires original native request");
    beginService(24000);hostTickCount64=24010;
    TrackingPose poseH17{};poseH17.qw=1;
    PublishTrackingSnapshot(poseH17,true,poseH17,false,poseH17,true,0,0,0,0,true,
        hostTickCount64,24010000,kcd2vr::tracking59c6::epoch.load());
    ready();serviceAt(24260);
    check(!g_kick_busy176&&!kcd2vr::kick26::requestTick&&!kcd2vr::kick26::reachTick,
        "H17 transient tracking loss retires service even when no leg existed yet");
    beginService(25000);++kcd2vr::tracking59c6::epoch;serviceAt(25260);
    check(!g_kick_busy176&&!kcd2vr::kick26::reachTick,"H17 recenter/source epoch replacement cannot inherit old R3");
    beginService(26000);++g_pose_epoch6689;serviceAt(26260);
    check(!g_kick_busy176&&!kcd2vr::kick26::reachTick,"H17 body epoch replacement retires delayed service");
    beginService(27000);hostTickCount64=27002;NoteKickRequest26(true);
    check(kcd2vr::kick26::requestTick==27000,"H17 late Note cannot freshen original native R3 timestamp");
    beginService(28000);SetKick26(0,0,730,true);
    b168::SpecialRequest gameR3{28001,28000,g_special_context168.load(),g_pose_epoch6689.load(),1,2,3};
    g_special_request168.publish(gameR3);NoteKickRequest26(true);serviceAt(28260);
    check(g_kick_busy176!=0,"H17 GAME style can use its configured visual fallback");
    hostTickCount64=28300;kcd2vr::animation_filter::nativeKickActive175=true;kcd2vr::kick26::nativeClipStart=28300;
    PublishTrackingSnapshot(poseH17,true,poseH17,true,poseH17,true,0,0,0,0,true,
        hostTickCount64,28300000,kcd2vr::tracking59c6::epoch.load());
    ServiceKick26(hostTickCount64);
    check(!g_kick_busy176&&kcd2vr::kick26::requestTick==28000&&kcd2vr::kick26::reachTick==29030,
        "H17 actual native GAME clip retires fallback picture but retains original clip/reach correlation");
    CancelProceduralKick176();
    // Actual native R3 consumer inside its120ms ingress lifetime. A resumed
    // current tracking packet must not revive the pre-loss original request.
    std::vector<unsigned char> imageH17(0x5400000);hostModuleHandleW=imageH17.data();g_wh_pinned104=0;
    const auto baseH17=WhBase();kcd2vr::retail_build112::hostRetailVerified112=true;
    std::array<unsigned char,0xE00> playerH17{},stateH17{};
    std::array<unsigned char,0x500> actorH17{};std::array<unsigned char,0x100> managerH17{},entityH17{},inputH17{};
    putH17(playerH17.data(),baseH17+0x3AA4B98);putH17(playerH17.data()+0x38,entityH17.data());
    putH17(entityH17.data(),baseH17+0x4072C78);putH17(playerH17.data()+0x278,actorH17.data());
    putH17(actorH17.data(),baseH17+0x3A80610);putH17(actorH17.data()+0x1F8,playerH17.data());
    putH17(actorH17.data()+0x210,stateH17.data());putH17(stateH17.data()+0xCE0,actorH17.data());
    putH17(playerH17.data()+0x9D0,managerH17.data());putH17(managerH17.data(),baseH17+0x3EC9958);
    putH17(managerH17.data()+0x38,playerH17.data());putH17(playerH17.data()+0xB88,inputH17.data());
    putH17(inputH17.data(),baseH17+0x3EC99A8);putH17(inputH17.data()+0x10,playerH17.data());
    putH17(stateH17.data()+0x38,1);putH17(stateH17.data()+0x218,-1);putH17(stateH17.data()+0x248,-1);
    putH17(stateH17.data()+0x278,-1);putH17(stateH17.data()+0x1B8,0);
    // R19 now uses the exact neutral-state native zone setter before dispatch.
    // Replace that native leaf explicitly; do not weaken any ingress assertions.
    nativeKickStateR19=stateH17.data();g_native_contract104=baseH17;g_native_zone104=&NativeKickZoneR19;
    g_last_player_entity=entityH17.data();g_henry_item_attach_manager=managerH17.data();
    g_combat_hook_base101=baseH17;g_combat_bypass101=&NativeSpecialLeafH17;
    g_combat_input_original101=&NativeSpecialLeafH17;g_combat_own_patch101.fill(0);
    g_phys_native6698={};g_phys_native6698.actionNames[4]=reinterpret_cast<std::uintptr_t>("attack_special");
    kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,0);
    auto pendingNativeH17=[&](std::uint64_t tick,bool note){
        CancelProceduralKick176();hostTickCount64=tick;ready();g_pose_epoch6689=2;
        SetNativeSpecialContext168(true);SetKick26(0,0,730,true);
        g_tracking_frames6695.publish(fresh());nativeSpecialEdgesH17=0;
        b168::SpecialRequest request{tick,tick,g_special_context168.load(),g_pose_epoch6689.load(),
            reinterpret_cast<std::uintptr_t>(playerH17.data()),reinterpret_cast<std::uintptr_t>(actorH17.data()),
            reinterpret_cast<std::uintptr_t>(stateH17.data())};
        g_special_request168.publish(request);g_special_pending168=true;if(note)NoteKickRequest26(true);return request;
    };
    auto nativeAtH17=[&](std::uint64_t tick){hostTickCount64=tick;g_tracking_frames6695.publish(fresh());NativeSpecialTick168();};
    auto liveH17=pendingNativeH17(30000,true);nativeAtH17(30020);
    check(nativeSpecialEdgesH17==3&&!g_special_pending168,"H17 current native R3 still dispatches one balanced press/hold/release");
    NativeSpecialTick168();check(nativeSpecialEdgesH17==3,"H17 consumed native R3 cannot dispatch twice");
    for(bool noteH17:{false,true}){
        const auto startH17=noteH17?32000ull:31000ull;const auto canceledH17=pendingNativeH17(startH17,noteH17);
        hostTickCount64=startH17+10;
        PublishTrackingSnapshot(poseH17,true,poseH17,false,poseH17,true,0,0,0,0,true,
            hostTickCount64,hostTickCount64*1000,kcd2vr::tracking59c6::epoch.load());
        const bool retiredH17=!g_special_pending168&&
            !b168::fresh(canceledH17,startH17+20,g_special_context168.load(),g_pose_epoch6689.load());
        ready();nativeAtH17(startH17+20);
        check(retiredH17&&nativeSpecialEdgesH17==0,
            noteH17?"H17 loss/recovery inside120ms cancels already-noted native ingress":"H17 loss/recovery inside120ms cancels pending native ingress before Note");
    }
    const auto cancelH17=pendingNativeH17(33000,true);hostTickCount64=33010;CancelProceduralKick176();nativeAtH17(33020);
    check(nativeSpecialEdgesH17==0&&!b168::fresh(cancelH17,33020,g_special_context168.load(),g_pose_epoch6689.load()),
        "H17 explicit cancellation retires copied native R3 before its120ms deadline");
    const auto sourceH17=pendingNativeH17(34000,true);hostTickCount64=34010;++kcd2vr::tracking59c6::epoch;
    PublishTrackingSnapshot(poseH17,true,poseH17,true,poseH17,true,0,0,0,0,true,
        hostTickCount64,34010000,kcd2vr::tracking59c6::epoch.load());nativeAtH17(34020);
    check(nativeSpecialEdgesH17==0&&!b168::fresh(sourceH17,34020,g_special_context168.load(),g_pose_epoch6689.load()),
        "H17 changed raw source epoch cancels native ingress before presentation service runs");
    pendingNativeH17(35000,true);nativeAtH17(35020);
    kcd2vr::animation_filter::nativeKickActive175=true;hostTickCount64=35030;CancelProceduralKick176();
    check(nativeSpecialEdgesH17==3&&kcd2vr::animation_filter::nativeKickActive175,
        "H17 canceling ingress never aborts an already-admitted native GAME kick animation");
    // H19 follow-up: exercise the ACTUAL special consumer with a released
    // owned guard still in the native block animation. Native leaves only are
    // replaced; no host helper supplies the readiness decision.
    putH17(stateH17.data()+0x630,baseH17+0x3B0A968);
    putH17(imageH17.data()+0x3B0A970,baseH17+0x1A7DDE0);
    const unsigned char ackPinH19[]{0x0F,0xB6,0xC2,0x0F,0xB6,0x44,0x08,0x18,0xC3};
    std::memcpy(imageH17.data()+0x1A7DDE0,ackPinH19,sizeof(ackPinH19));
    g_phys_native6698.actionNames[3]=reinterpret_cast<std::uintptr_t>("block");
    NativeOwner104 ownedH19{};check(NativeResolve104(playerH17.data(),ownedH19),"H19 kick owner resolves");
    auto guardH19=[&](std::uint64_t tick,int phase,int type,int zone){
        putH17(stateH17.data()+0x38,phase);putH17(stateH17.data()+0x218,type);
        putH17(stateH17.data()+0x278,zone);putH17(stateH17.data()+0x1B8,1);
        putH17(stateH17.data()+0x2A8,std::uint8_t(0));putH17(stateH17.data()+0x2D8,std::uint8_t(0));
        putH17(stateH17.data()+0x648,std::uint8_t(1));putH17(stateH17.data()+0xF8,0);
        auto request=pendingNativeH17(tick,false);
        g_kick_guard_wait_r19={};g_native_guard112={};g_native_guard112.owner=ownedH19;g_native_guard112.held=true;
        g_guard_manual112=false;g_guard_physical112=true;g_guard_physical_owner112=playerH17.data();
        nativeAtH17(tick+1);
        // The block-up leaf is counted too; only subsequent SPECIAL edges are measured.
        nativeSpecialEdgesH17=0;return request;
    };
    for(int phaseH19:{128,256})for(int typeH19:{-1,1}){
        const int metaH19=typeH19==-1?7:0;
        auto request=guardH19(40000+phaseH19*10+typeH19,phaseH19,typeH19,metaH19);
        check(g_special_pending168&&g_kick_guard_wait_r19.request.serial==request.serial,"H19 R3 retains exact original ingress through owned LT-up");
        nativeAtH17(request.tick+2);
        check(nativeSpecialEdgesH17==0&&g_special_pending168,"H19 R3 never selects a kick before actual LT-up ACK");
        putH17(stateH17.data()+0x648,std::uint8_t(0));nativeAtH17(request.tick+3);
        check(nativeSpecialEdgesH17==3&&!g_special_pending168,"H19 R3 offers one balanced special through residual owned block after ACK");
        int livePhaseH19=0,liveMetaH19=-1,desiredH19=-1;
        check(Read(stateH17.data()+0x38,livePhaseH19)&&Read(stateH17.data()+0x278,liveMetaH19)&&Read(stateH17.data()+0xF8,desiredH19)&&
              livePhaseH19==phaseH19&&liveMetaH19==metaH19&&desiredH19==5,"H19 kick setter changes desired lower sector only, not block metadata/phase");
        nativeAtH17(request.tick+4);check(nativeSpecialEdgesH17==3,"H19 resumed kick cannot replay");
    }
    auto lateH19=guardH19(50000,256,-1,7);putH17(stateH17.data()+0x648,std::uint8_t(0));nativeAtH17(lateH19.tick+121);
    check(!nativeSpecialEdgesH17&&!g_special_pending168,"H19 guard wait never extends original120ms ingress");
    auto hitH19=guardH19(51000,256,-1,7);putH17(stateH17.data()+0x648,std::uint8_t(0));putH17(stateH17.data()+0x38,64);nativeAtH17(hitH19.tick+3);
    check(!nativeSpecialEdgesH17&&!g_special_pending168,"H19 reaction cancels waiting kick without direction input");
    auto manualH19=guardH19(52000,256,-1,7);g_guard_manual_owner112=playerH17.data();g_guard_manual112=true;
    putH17(stateH17.data()+0x648,std::uint8_t(0));nativeAtH17(manualH19.tick+3);
    check(!nativeSpecialEdgesH17&&!g_special_pending168,"H19 manual LT takeover cancels waiting kick");g_guard_manual112=false;
    auto changedH19=guardH19(53000,256,-1,7);auto replacedH19=stateH17;putH17(actorH17.data()+0x210,replacedH19.data());
    nativeAtH17(changedH19.tick+3);check(!nativeSpecialEdgesH17&&!g_special_pending168,"H19 changed native state cannot inherit old guard kick");
    putH17(actorH17.data()+0x210,stateH17.data());
    std::printf("kick176_ingress=%u/%u production_frik=YES native_execution=NONE\n",checks-failures,checks);
    return failures?1:0;
}
