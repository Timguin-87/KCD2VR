// Executes the actual presentation owner/transaction/helper consumer. Only
// engine lookups, current-packet provider and entity/child PRS are explicit leaves.
#include <array>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <thread>
#include <string>
#include <vector>
#include "vrik/physical_weapon.h"
#include "vrik/staged_hold596c2.h"
#include "subsystems/ranged_reload/crossbow_native_xr34.h"
#include "subsystems/ranged_reload/reload_throw_claim_t25.h"
#include "core/snapshot_mailbox.h"
#include "core/haptics.h"
#include "hand_claim_fixture_t24.h"
namespace core=kcd2vr::core;namespace pm=kcd2vr::pose_core;namespace ph=kcd2vr::physical;
// Match the native translation unit: V3 is a distinct float-valued engine-side
// type, not an alias of the double-valued physical/pose-core vector.
struct V3 {float x{},y{},z{};};
struct QuatT28 {float q[4]{0,0,0,1},t[3]{};};
static V3 FromT(const QuatT28& a){return {a.t[0],a.t[1],a.t[2]};}
static V3 Sub(V3 a,V3 b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
static double Len(V3 a){return std::sqrt(double(a.x)*a.x+double(a.y)*a.y+double(a.z)*a.z);}
static ph::Pose PhysPose6698(const QuatT28& a){return {{a.q[0],a.q[1],a.q[2],a.q[3]},{a.t[0],a.t[1],a.t[2]}};}
static QuatT28 qt(ph::Pose p){return {{float(p.q.x),float(p.q.y),float(p.q.z),float(p.q.w)},{float(p.p.x),float(p.p.y),float(p.p.z)}};}
static QuatT28 AbsoluteFromRelative(const QuatT28& a,const QuatT28& b){return qt(ph::compose(PhysPose6698(a),PhysPose6698(b)));}
static QuatT28 RelativeFromAbsolute(const QuatT28& a,const QuatT28& b){return qt(ph::compose(ph::inverse(PhysPose6698(a)),PhysPose6698(b)));}
static bool ValidPose6689(const QuatT28& q){return ph::finite(PhysPose6698(q));}

namespace xcXR34=kcd2vr::crossbow_native_xr34;
#include "crossbow_pose_types_r95_actual.inc"
static xcXR34::Source source{};
static bool coherentBodySourceFixtureR95=true;
struct CommittedFrame6695 {std::uint64_t publicationR24=2,doorTrackingEpoch66=1;kcd2vr::crossbow_native_xr34::Source reloadSourceXR34{};RangedTargetXR34 reloadTargetXR34{};pm::Stamp stamp{};QuatT28 native[2]{},allowed[2]{};bool valid[2]{};float world[8]{0,0,0,1,0,0,0,1};};
struct ManualBow596 {void* player=nullptr;void* entity=nullptr;std::uint32_t hand=0;int side=0;
    std::uint64_t weapon=0,ammo=0,generation=0,epoch=0,request=0,arrowSerial64=0;};
struct PhysFrame6698 {ph::Input input{};ph::Output output{};ph::Profile profiles[2]{};float world[8]{0,0,0,1,0,0,0,1};};
static std::uint64_t now=1000;static std::uint64_t GetTickCount64(){return now;}
static std::atomic<std::uint64_t> g_pose_epoch6689{1},g_weapon_request_epoch6694{1};
static std::atomic<bool> g_manual_bow_fault596{false},g_gameplay_allowed{true};
static std::atomic<void*> g_henry_char_attachment_manager{nullptr};
static bool staged=true,tracked=true,contract=true,helperOK=true,bodyOK=true,readbackOK=true,child=false;
#ifndef KCD2VR_XR34_POSE_MEMORY_SIZE
#define KCD2VR_XR34_POSE_MEMORY_SIZE 0x300
#endif
static std::array<unsigned char,KCD2VR_XR34_POSE_MEMORY_SIZE> player{},model{},entity{},object{},attachment{},manager{},character{},other{},
    controller{},context{},scope{},mainAction{},parent{},nativeChild{},equipment{},toolItem{},pickable{};
static constexpr std::uintptr_t base=0x10000000;
static void* entityLookup=nullptr;static const char* ammoName="entity_right";
static unsigned placed=0,childPlaced=0,helperCalls=0,checks=0;static float naturalScale=1;
struct Matrix34V6550 {float m[12]{};};
static Matrix34V6550 entityWorld{};static QuatT28 helper{};
static std::uintptr_t WhBase(){return base;}
template<class T>static void put(auto& a,std::size_t n,const T& value){std::memcpy(a.data()+n,&value,sizeof(value));}
static bool (*extraReadXR34)(std::uintptr_t,void*,std::size_t)=nullptr;
template<class T>static bool Read(const void* p,T& out){
    if(extraReadXR34&&extraReadXR34(reinterpret_cast<std::uintptr_t>(p),&out,sizeof(out)))return true;
    if(reinterpret_cast<std::uintptr_t>(p)==base+0x40AC298){if constexpr(std::is_same_v<T,const char*>)out=ammoName;else return false;return true;}
    const auto address=reinterpret_cast<std::uintptr_t>(p);
    for(auto* block:{&player,&model,&entity,&object,&attachment,&manager,&character,&other,&controller,&context,&scope,&mainAction,&parent,&nativeChild,&equipment,&toolItem,&pickable}){
        const auto lo=reinterpret_cast<std::uintptr_t>(block->data());
        if(address>=lo&&address-lo<=block->size()&&sizeof(T)<=block->size()-(address-lo)){
            std::memcpy(&out,p,sizeof(T));return true;}}
    return false;
}
static bool PhysVft6698(void* p,std::uintptr_t rva){std::uintptr_t actual=0;return Read(p,actual)&&actual==base+rva;}
static void* (*extraEntityXR34)(unsigned)=nullptr;
static void* EntityById6694(unsigned id){return id==77?entityLookup:(extraEntityXR34?extraEntityXR34(id):nullptr);}
static bool ManualPlayer596(void* p){return p==player.data();}
static bool StagedBowEnabled59(){return staged;}
static bool TrackingHeadUsable111(std::uint64_t){return tracked;}
static bool TrackingHandUsable111(int,std::uint64_t){return tracked;}
static void* (*extraAttachmentXR34)(void*,const char*)=nullptr;
static void* ManualFindHandAttachment19c4r4(void* m,const char* name){return m==model.data()&&name==ammoName?attachment.data():
    (extraAttachmentXR34?extraAttachmentXR34(m,name):nullptr);}
// Actual native-binding readers generated from the current production source.
#include "manual_arrow_binding64.inc"
static bool NativeHelper(void*,const char*,QuatT28* out,bool){++helperCalls;*out=helper;return helperOK;}
static bool ManualArrowHelper64(void*,QuatT28& out) noexcept {++helperCalls;out=helper;return helperOK;}
static auto g_attachment_helper_transform_original=&NativeHelper;
static bool PhysSocketContract101(std::uintptr_t){return contract;}
static CommittedFrame6695 body{};
// The legacy socket fixture supplies a coherent current body/source as one
// provider leaf. The R95 publication adversary disables this and supplies the
// exact independently staged packet; no production function is replaced.
static bool ReadCommittedPacket6695(CommittedFrame6695& out){out=body;
    if(coherentBodySourceFixtureR95){out.reloadSourceXR34=source;out.reloadSourceXR34.bodyPublicationR95=out.publicationR24;}
    return bodyOK&&pm::fresh(out.stamp,g_pose_epoch6689.load(),reinterpret_cast<std::uintptr_t>(character.data()),now,80);}
struct ModelWorld6692 {pm::Quat q{};V3 t{};float scale=1;};
static ModelWorld6692 world{};
static bool PhysAttachmentWorld164(void* a,std::uintptr_t c,ModelWorld6692& out){out=world;return a==attachment.data()&&c==reinterpret_cast<std::uintptr_t>(character.data());}
static ph::Pose PhysCurrentWorld164(ph::Pose p,const ModelWorld6692& w){p.p=p.p*w.scale;return ph::compose({w.q,{w.t.x,w.t.y,w.t.z}},p);}
static bool ReadAttachmentQs6689(void*,float (&q)[8]){q[0]=q[1]=q[2]=0;q[3]=1;q[7]=naturalScale;q[4]=q[5]=q[6]=0;return true;}
static void (*placementReentryXR34)()=nullptr;
static bool PlaceBothConsumers6689(void* self,void*,const float (&q)[8],bool& haveChild){
    ++placed;haveChild=child;if(child)++childPlaced;
    const pm::Quat r{q[0],q[1],q[2],q[3]};
    const pm::Vec3 axes[3]{pm::rotate(r,{1,0,0}),pm::rotate(r,{0,1,0}),pm::rotate(r,{0,0,1})};
    for(int c=0;c<3;++c){float scale=0;if(!Read(static_cast<unsigned char*>(self)+0x0C+c*4,scale))return false;
        entityWorld.m[c]=float(axes[c].x*q[7]*scale);entityWorld.m[4+c]=float(axes[c].y*q[7]*scale);entityWorld.m[8+c]=float(axes[c].z*q[7]*scale);}
    entityWorld.m[3]=q[4];entityWorld.m[7]=q[5];entityWorld.m[11]=q[6];if(placementReentryXR34)placementReentryXR34();return true;
}
static bool PlacementReadback6689(void*,void*,unsigned,const float (&)[8],bool& checked){checked=child;return readbackOK;}
static bool GetEntityWorldTmV6552(void* e,unsigned id,Matrix34V6550& out){out=entityWorld;return e==entity.data()&&id==77;}
namespace diagnostics::capture168 {enum class Domain{Reload};static bool active(Domain,std::uint64_t){return false;}}
namespace diagnostic_trace {static void Write(const char*){} static void WriteFor(diagnostics::capture168::Domain,const char*){}}
#ifndef EXCEPTION_EXECUTE_HANDLER
#define EXCEPTION_EXECUTE_HANDLER 1
#endif

static bool FixtureRegistrationXR34() noexcept{return true;}
static PhysFrame6698 frame{};static bool currentSourceAvailableR92=true;
static bool RangedSourceXR34(xcXR34::Source& out,PhysFrame6698* f=nullptr) noexcept {out=source;if(f)*f=frame;return currentSourceAvailableR92&&tracked&&staged&&g_gameplay_allowed&&xcXR34::fresh(source,now);}
static bool PhysEmpty6698(std::uint64_t key){return !key;}
static bool Safe(void* p,std::size_t n){return p&&n;}
static bool ResolveJointByNameExactV6545(void*,const char*,int&){return false;}
static ph::Pose PhysWorld6698(const float* w){return {{w[0],w[1],w[2],w[3]},{w[4],w[5],w[6]}};}
struct ManualItems596 {void* weapon=nullptr;void* component=nullptr;bool loaded=false;std::uint64_t weaponKey=0,nockedKey=0,ammoKey=0;};
static bool ManualItemsRead596(void*,ManualItems596&){return false;} // native inventory leaf; not used in these pose tests
static bool ManualNockAttached19c4r4(const ManualItems596&){return false;}
static bool EntityHiddenFlag59c14(void*,bool&){return false;}
// Existing qualified bow visibility leaf: fixture counts its exact recipient.
static bool boltVisibleR95=true;static unsigned boltShowCallsR95=0;
static bool ManualArrowVisible42(const ManualBow596& m,bool* =nullptr,unsigned* =nullptr) noexcept {
    ++boltShowCallsR95;return boltVisibleR95&&ManualHandAttached19c4r4(m);
}
// Publisher raw-input leaf is not exercised by the attachment fixtures; the
// separate source-gap suite supplies real snapshot transitions and rejection.
struct TrackingSnapshot {
    std::uint64_t sequence=0,tick=0,source_epoch=0;
    float right_grip=0,left_grip=0;
    bool allowed=false,head_valid=false,right_valid=false,left_valid=false;
    bool head_held=false,right_held=false,left_held=false;
};
static bool ReadTrackingSnapshot(TrackingSnapshot&) noexcept{return false;}
template<class Snapshot> static void ChestFlagsD66C(xcXR34::Source& out,const Snapshot&) noexcept {out.quiverKnownD72=false;}
static void RangedReloadEquipmentXR34() noexcept;
static bool ReloadContextXR34(const xcXR34::Owner&,std::uint64_t) noexcept;
static bool ReloadNativeOwnerXR34(const xcXR34::Owner&,bool) noexcept;
static bool ReloadPoseWriterCurrentXR34(const xcXR34::Owner&,std::uint64_t=0) noexcept;
#ifdef KCD2VR_XR34_ACTUAL_INCLUDE
#include KCD2VR_XR34_ACTUAL_INCLUDE
#else
#include "crossbow_pose_xr34_actual.inc"
#endif
#ifndef KCD2VR_XR34_POSE_EXTERNAL_RELOAD
static void RangedReloadEquipmentXR34() noexcept{}
static bool ReloadContextXR34(const xcXR34::Owner&,std::uint64_t) noexcept{return true;}
static bool ReloadNativeOwnerXR34(const xcXR34::Owner&,bool) noexcept{return true;}
static bool ReloadPoseWriterCurrentXR34(const xcXR34::Owner&,std::uint64_t) noexcept{return true;}
#endif
// A test commit stamps both payloads exactly where coherent_pose does.
static void CommitReloadBodyFixtureR95(CommittedFrame6695& packet){
    if(coherentBodySourceFixtureR95)packet.reloadSourceXR34=source;
    packet.reloadSourceXR34.bodyPublicationR95=packet.publicationR24;
    packet.reloadTargetXR34.publicationR95=packet.publicationR24;
}
static xcXR34::Status state{};static QuatT28 original[1]{};
static void ck(bool b,const char* why){++checks;if(!b){std::fprintf(stderr,"FAIL %s\n",why);std::exit(1);}}
static bool near(pm::Vec3 a,pm::Vec3 b,double e=1e-5){return pm::length(a-b)<e;}
static void reset(bool bolt){
    hand_claim_fixture_t24::Reset(player.data(),reinterpret_cast<std::uintptr_t>(character.data()));
    xcXR34::registrationReader=&FixtureRegistrationXR34;xcXR34::RefreshRegistration();
    for(auto* b:{&player,&model,&entity,&object,&attachment,&manager,&character,&other,&controller,&context,&scope,&mainAction,&parent,&nativeChild,&equipment,&toolItem,&pickable})b->fill(0);
    g_ranged_bolt_join_r95={};boltShowCallsR95=0;boltVisibleR95=true;
    now=1000;coherentBodySourceFixtureR95=true;currentSourceAvailableR92=true;bodyOK=readbackOK=staged=tracked=helperOK=true;child=false;naturalScale=1;placed=childPlaced=0;world={};source={};
    source.player=reinterpret_cast<std::uintptr_t>(player.data());source.manager=reinterpret_cast<std::uintptr_t>(model.data());
    source.equipment=reinterpret_cast<std::uintptr_t>(equipment.data());source.character=reinterpret_cast<std::uintptr_t>(character.data());
    source.key=20;source.ammoKey=30;source.weaponItem=reinterpret_cast<std::uintptr_t>(other.data());source.ammoItem=reinterpret_cast<std::uintptr_t>(other.data())+0x80;
    source.generation=source.body=source.tracking=source.request=1;source.sample=10;source.tick=now;source.actor=66;source.primary=1;source.classId=14;source.allowed=true;
    state={};state.owner.source=source;state.owner.serial=1;state.owner.controller=reinterpret_cast<std::uintptr_t>(controller.data());
    state.owner.main=reinterpret_cast<std::uintptr_t>(mainAction.data());state.owner.parent=reinterpret_cast<std::uintptr_t>(parent.data());
    state.owner.child=reinterpret_cast<std::uintptr_t>(nativeChild.data());state.owner.context=reinterpret_cast<std::uintptr_t>(context.data());state.owner.token=17;
    state.sourceEstablished=true;xcXR34::completedNativeLease=1;state.clockOwned=true;state.placeTool=!bolt;state.placeBolt=bolt;state.cocking=!bolt;state.loading=bolt;state.phase=bolt?xcXR34::Phase::PlaceBolt:xcXR34::Phase::Span;
    put(controller,0,base+0x40519E8);put(controller,0x34,1u);put(controller,0x38,static_cast<void*>(scope.data()));
    put(scope,0x148,static_cast<void*>(nativeChild.data()));put(nativeChild,0x30,static_cast<void*>(scope.data()));
    put(nativeChild,8,static_cast<void*>(context.data()));put(nativeChild,0x54,17u);
    put(equipment,0x50,static_cast<void*>(mainAction.data()));put(mainAction,0x98,static_cast<void*>(parent.data()));
    put(parent,0,base+0x3E78CF8);put(parent,0x70,static_cast<void*>(nativeChild.data()));
    if(bolt)put(parent,0x88,77u);else put(mainAction,0x80,static_cast<void*>(toolItem.data()));
    put(toolItem,0,base+0x3A2DA88);put(toolItem,0x30,std::uint64_t(123));put(toolItem,0x88,static_cast<void*>(pickable.data()));
    put(pickable,0x30,77u);put(pickable,0x38,static_cast<void*>(entity.data()));
    put(player,0x1B0,static_cast<void*>(model.data()));put(model,0x20,static_cast<void*>(player.data()));put(model,0x2C,77u);
    put(entity,0,base+0x4072C78);put(object,0,base+0x3A68F68);put(object,8,77u);for(int i=0;i<3;++i)put(object,0x0C+i*4,1.f);
    put(attachment,0,base+0x3A657C0);put(attachment,0x20,static_cast<void*>(object.data()));put(attachment,0x28,static_cast<void*>(manager.data()));
    put(attachment,0x78,std::int16_t(0));QuatT28 rel{};rel.t[0]=.01f;put(attachment,0x9C,rel);
    put(manager,0x70,static_cast<void*>(character.data()));g_henry_char_attachment_manager=manager.data();entityLookup=entity.data();
    body={};body.stamp={1,10,now,reinterpret_cast<std::uintptr_t>(character.data())};body.valid[0]=body.valid[1]=true;
    body.allowed[0].t[0]=1;body.allowed[0].t[1]=2;body.allowed[0].t[2]=3;original[0]={};original[0].t[0]=.1f;
    helper={};helper.t[1]=-.645f;g_pose_epoch6689=1;g_gameplay_allowed=true;
    xcXR34::activeNativeLease=1;xcXR34::revokedNativeLease=0;xcXR34::statusMailbox.publish(state);xcXR34::PublishSource(source);
    g_ranged_grip_xr34={};g_ranged_socket_xr34.publish({});g_ranged_primary_placed_r95.publish({});g_ranged_placed_xr34.publish({});
}
static void bootstrap(){ck(!CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==0,"first exact native binding waits for original-joint transaction");
    BuildCrossbowPosesXR34(body,original,1);CommitReloadBodyFixtureR95(body);ck(CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==1,"same native object+child receive one achieved placement");}
#ifndef KCD2VR_XR34_POSE_NO_MAIN
int main(){
 for(bool bolt:{false,true}){reset(bolt);bootstrap();RangedPlacedXR34 out{};g_ranged_placed_xr34.read(out);
    ck(out.valid&&near(out.worldGrip,{1,2,3}),"real tool grip/bolt slt_0 reaches committed working wrist");
    ck(out.bolt==bolt&&out.entity==entity.data()&&out.owner.source.ammoKey==30,"receipt retains actual entity/owner and original ammo identity");
    state.placeBolt=state.placeTool=false;xcXR34::statusMailbox.publish(state);
    ck(!CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==1,"pre-native transfer/despawn withdrawal prevents another placement");}
 reset(true);const double q=std::sqrt(.5);world.q={0,0,q,q};world.t={10,20,30};world.scale=2;naturalScale=2;
 put(object,0x0C,1.2f);put(object,0x10,.8f);put(object,0x14,.9f);bootstrap();RangedPlacedXR34 out{};g_ranged_placed_xr34.read(out);
 ck(out.valid&&near(out.worldGrip,{6,22,36}),"anisotropic bolt scale + rotated/scaled parent applied exactly once");
 reset(false);bootstrap();put(object,0x10,0.f);ck(!CrossbowAttachmentXR34(object.data(),attachment.data()),"invalid current scale refuses pose");g_ranged_placed_xr34.read(out);ck(!out.valid,"invalid current scale withdraws prior achieved proof");
 reset(true);bootstrap();readbackOK=false;ck(CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==2,"partial write/readback failure consumes callback");g_ranged_placed_xr34.read(out);ck(!out.valid,"partial failure cannot retain achieved proof");
 reset(false);bootstrap();put(toolItem,0x30,std::uint64_t(124));ck(!CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==1,"same entity+new tool WUID cannot borrow earlier target");
 reset(true);bootstrap();entityLookup=other.data();ck(!CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==1,"entity-ID reuse refuses foreign object");
 reset(true);bootstrap();put(nativeChild,0x54,18u);ck(!CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==1,"replacement child token refuses old pose");
 reset(true);bootstrap();xcXR34::RevokeNativeParent(parent.data());ck(!CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==1,"atomic native lifetime revocation beats stale mailbox");
 reset(false);bootstrap();source.tick=919;ck(!CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==1,"old frame expires without a renewed tick");
 reset(true);bootstrap();tracked=false;ck(!CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==1,"tracking loss yields native without a fresh global actor");

 reset(true);ManualBow596 claimBinding{};claimBinding.player=player.data();claimBinding.entity=entity.data();claimBinding.hand=77;claimBinding.side=0;claimBinding.epoch=claimBinding.request=1;
 hand_claim_fixture_t24::Publish(0);ck(!ManualHandAttached19c4r4(claimBinding),"T24 XR34 included actual right binding refuses same-hand claim");
 reset(true);hand_claim_fixture_t24::Publish(1);ck(ManualHandAttached19c4r4(claimBinding),"T24 XR34 included actual right binding preserves opposite claim");
 std::printf("PASS %u actual XR34 attachment/pose checks; native current source/PRS leaves explicit, no game execution\n",checks);
}

#endif
