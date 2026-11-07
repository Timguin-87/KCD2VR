// Actual native passenger/source functions plus exact read-only identity leaves.
// Existing explicit fixture-owned native memory, lookup, PRS and clock leaves
// do not execute the retail game or emulate an accepted native reload result.
#define KCD2VR_XR34_POSE_MEMORY_SIZE 0x1200
#define KCD2VR_XR34_POSE_EXTERNAL_RELOAD
#define KCD2VR_XR34_POSE_NO_MAIN
#include "crossbow_pose_xr34_test.cpp"
#include "../src/subsystems/interactions/native_passenger_a34.h"
#include "../src/tracking_sample59c6.h"
struct ReloadItemFixtureXR34 {ph::Profile profile{};void* item=nullptr;void* worldLink54=nullptr;void* entity=nullptr;};
using PhysEquipmentItem6698=ReloadItemFixtureXR34;
struct PhysEquipment6698 {std::array<ReloadItemFixtureXR34,8>items{};int count=0;void* player=nullptr;void* manager=nullptr;
    std::uint64_t request=0,epoch=0,tick=0,handKey[2]{};bool valid=false;};
static core::SnapshotMailbox<PhysEquipment6698> g_phys_equipment6698;
static std::atomic<void*> g_last_player_entity{nullptr},g_henry_item_attach_manager{nullptr};
static std::atomic<std::uintptr_t> g_player_char37{0};
static std::atomic<bool>g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_scripted{false},g_ctx_reading{false};
static std::atomic<bool>g_head_valid{true},g_left_valid{true},g_right_valid{true};
static int ReloadMode47C2(){return staged?2:0;}
static bool CrossbowReloadEnabledD65C() noexcept{return ReloadMode47C2()==2||kcd2vr::ranged_options_d65c::crossbowReload.load();}
namespace diagnostics::lab {static bool NativeReference(){return false;}}
namespace retail_build112 {static bool verified=true;static bool Ready(std::uintptr_t){return verified;}}
namespace animation_filter {static bool clipOnly=true;static bool ReloadPassengerOnlyXR34(void*,std::uintptr_t,int){return clipOnly;}}
static bool NativePassengerCurrentA34(std::uint64_t tick){return kcd2vr::native_passenger_a34::current(
    g_player_char37.load(),g_pose_epoch6689.load(),tick);}
static unsigned nativeEquipmentCallsXR34=0,reenterEquipmentAtXR34=0;
static void* ManualEquipmentPtr59c9(void* p){
    if(++nativeEquipmentCallsXR34==reenterEquipmentAtXR34)xcXR34::passengerPublication.fetch_add(1);
    return p==player.data()?equipment.data():nullptr;
}
#include "reload_native_identity_xr34_actual.inc"
struct TrackingSnapshot {std::uint64_t sequence=0,tick=0,source_epoch=0;bool head_valid=true,left_valid=true,right_valid=true;bool allowed=true,head_held=false,right_held=false,left_held=false;
    float right_grip=0,left_grip=0;};
static bool freshTrackingXR34=true;static std::uint64_t trackingTickXR34=0;
static bool ReadTrackingSnapshot(TrackingSnapshot& t){t.sequence=now;t.tick=trackingTickXR34?trackingTickXR34:now;t.source_epoch=kcd2vr::tracking59c6::epoch.load();
    t.allowed=freshTrackingXR34;return true;}
#include "../src/subsystems/ranged_reload/reload_passenger_xr34.inl"
static const char* weaponNameXR34="current_native_weapon";
static void* weaponEntityXR34(){return other.data()+0x900;}
static void* weaponAttachmentXR34(){return other.data()+0x500;}
static void* EntityLeafXR34(unsigned id){return id==88?weaponEntityXR34():nullptr;}
static void* AttachmentLeafXR34(void* m,const char* name){return m==model.data()&&name==weaponNameXR34?weaponAttachmentXR34():nullptr;}
static bool ReadLeafXR34(std::uintptr_t at,void* out,std::size_t n){
    if(at==base+0x40AC2B0&&n==sizeof(const char*)){std::memcpy(out,&weaponNameXR34,n);return true;}return false;
}
static xcXR34::Passenger nativePassengerXR34{};
// C_Player and CEntity are different native objects. Keep the actual producer's
// C_Player identity in Source, and put its entity in a distinct fixture region.
static void* HenryEntityXR34(){return character.data()+0x1000;}
static void FreezePassengerXR34(){
    xcXR34::Source current{};ck(xcXR34::ReadSource(current),"source publication is actual coherent source");
    auto& p=nativePassengerXR34;p.frame=current;p.owner=state.owner;p.workingHand=0;p.childStatus=2;
    p.root=reinterpret_cast<std::uintptr_t>(scope.data());p.publication=xcXR34::passengerPublication.load();p.valid=true;
    p.clip={};p.clip.qualified=true;p.clip.id=11;p.clip.segment=0;p.clip.count=2;
    p.clip.animation=reinterpret_cast<std::uintptr_t>(other.data()+0xC00);p.clip.list=p.clip.animation+0x48;
    p.clip.entries=reinterpret_cast<std::uintptr_t>(other.data()+0xD00);
    put(other,0xC48,base+0x3A7D9D0);put(other,0xC50,other.data()+0xD00);put(other,0xCFC,2u);
    p.clock={};p.clock.entry=reinterpret_cast<std::uintptr_t>(other.data()+0xB00);p.clock.qualified=true;
    p.clock.segment=0;p.clock.normalized=.2f;p.clock.playbackScale=1;p.clock.activeTime=.1f;
    p.clock.controllerScale=p.clock.characterScale=1;
    put(other,0xAFC,1u);put(other,0xB10,11u);put(other,0xB16,static_cast<unsigned char>(0));
    put(other,0xB44,.2f);put(other,0xB2C,1.f);put(nativeChild,0x10,.1f);put(controller,0x9C,1.f);put(character,0xA9C,1.f);
    put(character,0x110+0x48,other.data()+0xB00);put(scope,0x20,context.data());put(context,0x10,character.data());
    put(scope,0x30,0u);put(scope,0x34,1u);
    p.binding={};p.binding.qualified=p.binding.toolBound=true;p.binding.serial=state.owner.serial;
    p.binding.sample=current.sample;p.binding.tick=current.tick;p.binding.toolId=77;
    p.binding.toolItem=reinterpret_cast<std::uintptr_t>(toolItem.data());p.binding.toolKey=123;
    p.binding.toolEntity=reinterpret_cast<std::uintptr_t>(entity.data());p.binding.toolAttachment=reinterpret_cast<std::uintptr_t>(attachment.data());
    xcXR34::passengerMailbox.publish(p);
}
static void SetupNativeXR34(){
    nativeEquipmentCallsXR34=reenterEquipmentAtXR34=0;freshTrackingXR34=true;trackingTickXR34=0;
    reset(false);placementReentryXR34=nullptr;checks=0;xcXR34::RefreshRegistration();xcXR34::passengerPublication=1;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=g_ctx_reading=false;
    retail_build112::verified=true;animation_filter::clipOnly=true;g_head_valid=g_left_valid=g_right_valid=true;
    extraEntityXR34=&EntityLeafXR34;extraAttachmentXR34=&AttachmentLeafXR34;extraReadXR34=&ReadLeafXR34;
    g_last_player_entity=HenryEntityXR34();g_player_char37=reinterpret_cast<std::uintptr_t>(character.data());
    g_henry_item_attach_manager=model.data();g_weapon_request_epoch6694=1;kcd2vr::tracking59c6::epoch=1;
    kcd2vr::native_passenger_a34::witness.publish({});g_reload_equipment_xr34.publish({});g_reload_equipment_seed_xr34.publish({});body.reloadSourceXR34={};
    put(model,0,base+0x3EC9958);put(model,0x38,player.data());put(model,0x40,1);
    put(player,0,base+0x3AA4B98);put(player,0x30,66u);put(player,0x9D0,model.data());put(player,0x990,other.data()+0x200);
    put(player,0x38,HenryEntityXR34());put(character,0x1000,base+0x4072C78);
    put(other,0x460,std::uint64_t(0));put(other,0x468,std::uint64_t(20));
    put(equipment,0x40,other.data());put(equipment,0x48,other.data()+0x80);
    put(other,0,base+0x3A2DA88);put(other,0x30,std::uint64_t(20));put(other,0x88,other.data()+0x400);
    put(other,0x48,other.data()+0xE00);put(other,0xE00,base+0x3A4D890);put(other,0xEE8,other.data()+0xF00);
    put(other,0xF08,14);put(other,0xF25,static_cast<unsigned char>(0));
    put(other,0x80,base+0x3A2DA88);put(other,0xB0,std::uint64_t(30));
    put(other,0x430,88u);put(other,0x438,weaponEntityXR34());put(other,0x900,base+0x4072C78);
    put(other,0x500,base+0x3A657C0);put(other,0x520,other.data()+0xA00);put(other,0x528,manager.data());
    put(other,0x578,std::int16_t(0));put(other,0x57A,static_cast<unsigned char>(0));put(other,0x59C,QuatT28{});
    put(other,0xA00,base+0x3A68F68);put(other,0xA08,88u);
    put(mainAction,0,base+0x3E7B1B0);put(parent,0x80,mainAction.data());
    put(nativeChild,0,base+0x3A450B0);put(nativeChild,0x28,2);
    put(controller,0x20,context.data());put(controller,0x28,pickable.data()+0x200);put(pickable,0x210,character.data());
    PhysEquipment6698 metadata{};metadata.valid=true;metadata.player=player.data();metadata.manager=model.data();
    metadata.request=metadata.epoch=1;metadata.tick=now;metadata.count=1;
    auto& item=metadata.items[0];item.profile.key=20;item.profile.generation=1;item.profile.classId=14;item.profile.side=1;
    item.item=other.data();item.worldLink54=other.data()+0x400;item.entity=weaponEntityXR34();item.profile.entity=88;
    for(int side=0;side<2;++side){std::memcpy(source.nativeWrist[side].q,body.native[side].q,sizeof(body.native[side].q));
        std::memcpy(source.nativeWrist[side].p,body.native[side].t,sizeof(body.native[side].t));
        std::memcpy(source.allowedWrist[side].q,body.allowed[side].q,sizeof(body.allowed[side].q));
        std::memcpy(source.allowedWrist[side].p,body.allowed[side].t,sizeof(body.allowed[side].t));}
    xcXR34::PublishSource(source);
    g_phys_equipment6698.publish(metadata);RangedReloadEquipmentXR34();FreezePassengerXR34();
}
static void AssertCurrentXR34(bool value,const char* why){ck(ReloadPassengerCurrentXR34(nativePassengerXR34,
    source.character,source.body,now)==value,why);}
static void ReenterPostUpdateXR34(){++xcXR34::passengerPublication;FreezePassengerXR34();}
static unsigned WriterScenarioXR34(){
    SetupNativeXR34();bootstrap();RangedPlacedXR34 achieved{};g_ranged_placed_xr34.read(achieved);
#ifndef KCD2VR_XR34_REENTRY_ONLY
    ck(achieved.valid&&RangedPlacementStillCurrentXR34(achieved),"achieved receipt still matches actual native entity grip");
    entityWorld.m[3]+=.2f;
    ck(!RangedPlacementStillCurrentXR34(achieved),"native fallback motion cannot borrow old achieved placement readiness");
    entityWorld.m[3]-=.2f;
    ++xcXR34::passengerPublication;
    ck(!CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==1,"no tool drive before a new original post-Update witness");
    FreezePassengerXR34();
#endif
    placementReentryXR34=&ReenterPostUpdateXR34;
    ck(CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==2,"first attempted write stays consumed across native reentry");
    g_ranged_placed_xr34.read(achieved);
    ck(!achieved.valid,"new same-owner post-Update packet cannot relabel an in-flight old placement");
    placementReentryXR34=nullptr;return checks;
}
static unsigned FirstPoseAfterMetadataExpiryXR34(){
    SetupNativeXR34();SeedReloadEquipmentXR34(source);g_reload_equipment_xr34.publish({});
    now=1110;RangedReloadEquipmentXR34();ReloadEquipmentXR34 equipmentNow{};ReloadEquipmentSeedXR34 originalSeed{};
    ck(ReloadEquipmentCurrentXR34(state.owner,equipmentNow,now)&&equipmentNow.equipment.tick==1110,
        "first owned equipment refresh revalidates the pre-admission metadata seed after old100ms cache expiry");
    ck(g_reload_equipment_seed_xr34.read(originalSeed)&&originalSeed.equipment.tick==1000&&source.tick==1000,
        "metadata reacquisition never renews the old physics/pose or metadata seed stamp");
    state.sourceEstablished=false;state.started=1060;state.firstPoseDeadline=1140;
    xcXR34::completedNativeLease=0;xcXR34::statusMailbox.publish(state);
    xcXR34::Admission a{};a.owner=state.owner;a.root=nativePassengerXR34.root;a.clip=nativePassengerXR34.clip;a.clock=nativePassengerXR34.clock;
    a.binding=nativePassengerXR34.binding;a.publication=xcXR34::passengerPublication.load();a.tick=1110;a.started=1060;a.deadline=1140;a.childStatus=2;a.valid=true;
    xcXR34::admissionMailbox.publish(a);xcXR34::passengerMailbox.publish({});xcXR34::Passenger first{};
    ck(ReadReloadPassengerXR34(first,source.character,source.body,now)&&first.bootstrap&&first.frame.tick==1000,
        "actual typed reader admits current native action without a prior passenger or a fabricated fresh pose");
    body.stamp.tick=1110;body.stamp.sample=11;body.allowed[0].t[0]+=.04f;
    TrackingSnapshot tracking{};tracking.sequence=11;tracking.tick=1110;tracking.source_epoch=1;
    PrepareReloadPoseXR34(first,body,original,1,tracking);xcXR34::Source fresh{};
    ck(RangedReloadSourceXR34(fresh)&&fresh.tick==1110&&fresh.sample==11&&fresh.completedLease==state.owner.serial,
        "actual native socket/commit source producer establishes the first fresh wrist from current original joints");
    ck(first.frame.tick==1000&&state.owner.source.tick==1000,"completed first wrist preserves immutable original action/source clocks");
    xcXR34::PublishSource(fresh);
    ck(!xcXR34::ReadAdmission(a,now),"publishing the actual first source closes native admission bootstrap immediately");
    return checks;
}
int main(){
#ifdef KCD2VR_XR34_REENTRY_ONLY
    const auto count=WriterScenarioXR34();std::printf("PASS %u production XR34 writer checks\n",count);return 0;
#else
    unsigned total=0;
    SetupNativeXR34();AssertCurrentXR34(true,"exact current installed child/root/CAF/tool native owner qualifies");
    for(unsigned step=1;step<=4;++step){
        now=1000+step*40;RangedReloadEquipmentXR34();
        body.stamp.tick=now;body.stamp.sample=10+step;body.allowed[0].t[0]=1.f+step*.04f;
        TrackingSnapshot tracking{};tracking.sequence=body.stamp.sample;tracking.tick=now;tracking.source_epoch=1;
        tracking.right_grip=.8f;
        PrepareReloadPoseXR34(nativePassengerXR34,body,original,1,tracking);
        xcXR34::Source result{};
        ck(RangedReloadSourceXR34(result)&&result.sample==tracking.sequence&&result.tick==tracking.tick,
            "new actual committed wrist refreshes XR34 source beyond original physical/equipment frame age");
        ck(result.allowedWrist[0].p[0]==body.allowed[0].t[0]&&result.allowedWrist[1].p[0]==body.native[1].t[0]&&
            state.owner.source.tick==1000,"working wrist advances while primary stays native and original owner clock is not restamped");
        source=result;xcXR34::PublishSource(source);FreezePassengerXR34();AssertCurrentXR34(true,"next original post-Update receipt joins new source");
    }
    total+=checks;
    for(unsigned kind=0;kind<36;++kind){SetupNativeXR34();
        if(kind==0)put(nativeChild,0x54,18u);
        if(kind==1)put(scope,0x148,static_cast<void*>(nullptr));
        if(kind==2)put(nativeChild,0x28,1);
        if(kind==3)put(controller,0x20,other.data());
        if(kind==4)g_pose_epoch6689=2;
        if(kind==5)g_weapon_request_epoch6694=2;
        if(kind==6)kcd2vr::tracking59c6::epoch=2;
        if(kind==7)put(other,0x30,std::uint64_t(21));
        if(kind==8)put(other,0xB0,std::uint64_t(31));
        if(kind==9)put(toolItem,0x30,std::uint64_t(124));
        if(kind==10)put(other,0xB44,.3f);
        if(kind==11)animation_filter::clipOnly=false;
        if(kind==12)g_ctx_cutscene=true;
        if(kind==13)g_ctx_menu=true;
        if(kind==14)now+=81;
        if(kind==15)retail_build112::verified=false;
        if(kind==16)xcXR34::RevokeNativeParent(parent.data());
        if(kind==17){auto replacement=source;replacement.sample++;xcXR34::PublishSource(replacement);}
        if(kind==18)put(other,0xF08,15);
        if(kind==19)put(other,0xF25,static_cast<unsigned char>(1));
        if(kind==20)freshTrackingXR34=false;
        if(kind==21)trackingTickXR34=now-81;
        if(kind==22){nativeEquipmentCallsXR34=0;reenterEquipmentAtXR34=2;}
        if(kind==23)g_last_player_entity=nullptr;
        if(kind==24)g_last_player_entity=player.data(); // former collapsed fixture must fail
        if(kind==25)put(player,0x38,weaponEntityXR34());
        if(kind==26)put(character,0x1000,base+0x3AA4B98); // wrong native entity type
        if(kind==27)g_henry_item_attach_manager=nullptr;
        if(kind==28)g_henry_item_attach_manager=attachment.data();
        if(kind==29)put(model,0x38,other.data());
        if(kind==30)put(model,0,base+0x4072C78);
        if(kind==31)put(player,0,base+0x4072C78);
        if(kind==32)put(player,0x9D0,attachment.data());
        if(kind==33)put(player,0x30,67u);
        if(kind==34)put(model,0x38,reinterpret_cast<void*>(1)); // unreadable current player
        if(kind==35){g_last_player_entity=reinterpret_cast<void*>(1);put(player,0x38,reinterpret_cast<void*>(1));}
        AssertCurrentXR34(false,"changed identity/body/request/source/clock/context revokes exact passenger");total+=checks;
    }
    for(auto crc:{0xeed2e729u,0x591f4107u,0x735721aau,0x04ab0eefu,0xc630055bu,0xd860d990u}){
        SetupNativeXR34();kcd2vr::native_passenger_a34::Root root{};
        root.controller=1;root.character=source.character;root.action=2;root.scope=3;root.context=4;root.epoch=1;
        root.sourceTick=root.tick=now;root.serial=5;root.crc=crc;root.fragment=1;root.status=2;root.layer=0;
        kcd2vr::native_passenger_a34::witness.publish(root);AssertCurrentXR34(false,"all six existing A34 activities retain full native ownership");total+=checks;
    }
    SetupNativeXR34();const auto before=body.allowed[0];TrackingSnapshot tracking{};tracking.sequence=body.stamp.sample;
    tracking.tick=body.stamp.tick;tracking.source_epoch=1;put(other,0x578,std::int16_t(9));
    PrepareReloadPoseXR34(nativePassengerXR34,body,original,1,tracking);xcXR34::Source result{};
    ck(!RangedReloadSourceXR34(result)&&std::memcmp(&before,&body.allowed[0],sizeof(before))==0,
        "replaced native weapon socket withdraws source without migrating the wrist or native action");total+=checks;
    total+=WriterScenarioXR34();total+=FirstPoseAfterMetadataExpiryXR34();
    std::printf("PASS %u production XR34 native passenger/source checks (explicit engine leaves, no native execution)\n",total);
#endif
}
