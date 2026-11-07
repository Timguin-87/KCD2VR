// Actual native adapter, passenger gate and completed-source producer. Retail
// memory, scheduler, native events, equipment and tracked pose remain fixtures.
// check_crossbow_d82.py extracts the named production functions unchanged.
#define KCD2VR_XR34_NATIVE_NO_MAIN
#include "crossbow_native_xr34_test.cpp"
#include "../src/subsystems/ranged_reload/reload_throw_claim_t25.h"
#include "../src/vrik/physical_weapon.h"
namespace xcXR34=x;
namespace ph=kcd2vr::physical;
namespace pm=kcd2vr::pose_core;
static unsigned checksD82=0;
#define CHECK82(c) do{++checksD82;if(!(c)){std::cerr<<"FAIL_D82 line="<<__LINE__<<" " #c "\n";std::exit(1);}}while(0)

// Explicit native/context leaves around the unchanged passenger consumer.
struct ReloadEquipmentXR34{};
static bool ReloadEquipmentCurrentXR34(const x::Owner& o,ReloadEquipmentXR34&,std::uint64_t) noexcept{return XR34Current(o);}
static std::uintptr_t WhBase() noexcept{return base;}
static bool RangedReadXR34(std::uintptr_t at,void* out,std::size_t n) noexcept{return XR34ReadBytes(at,out,n);}
template<class T>static bool Read(const void* at,T& out) noexcept{return SehCopy(at,&out,sizeof(out));}
namespace animation_filter {static bool ReloadPassengerOnlyXR34(void*,std::uintptr_t,int,bool=false) noexcept{return true;}}
static bool RangedItemEntityXR34(void* item,std::uint32_t& id,void*& entity) noexcept {
    const auto& b=f->spatial;if(reinterpret_cast<std::uintptr_t>(item)!=b.toolItem||!b.toolId)return false;
    id=b.toolId;entity=reinterpret_cast<void*>(b.toolEntity);return true;
}
static bool RangedBoundToolIdentityD72(const x::Owner& o,unsigned id,void* entity,int& side,void*& attachment) noexcept {
    const auto& b=f->spatial;if(!b.toolId||id!=b.toolId||reinterpret_cast<std::uintptr_t>(entity)!=b.toolEntity)return false;
    side=1-o.source.primary;attachment=reinterpret_cast<void*>(b.toolAttachment);return true;
}
static bool RangedBoundHandIdentityXR34(const x::Owner& o,unsigned id,void* entity,int& side,void*& attachment) noexcept {
    const auto& b=f->spatial;if(!b.boltId||id!=b.boltId||reinterpret_cast<std::uintptr_t>(entity)!=b.boltEntity)return false;
    side=1-o.source.primary;attachment=reinterpret_cast<void*>(b.boltAttachment);return true;
}
static bool PhysVft6698(void* ptr,std::uintptr_t rva) noexcept {std::uintptr_t v=0;return Read(ptr,v)&&v==base+rva;}
static void* EntityById6694(unsigned id) noexcept {
    return id&&id==f->spatial.boltId?reinterpret_cast<void*>(f->spatial.boltEntity):nullptr;
}
static bool ReloadNativeOwnerXR34(const x::Owner& o,bool) noexcept{return XR34Current(o);}
// Native binding completion is a fixture leaf. The separate R95R1 bolt
// adapter suite executes its actual retail identity/80ms checks.
static bool pendingBoltLeafReadyR95=true;
static int pendingBoltLeafSideR95=-1;
static bool RangedReadBoltBindingR95(const x::Owner& o,unsigned id,void*& entity,int& side,
    void*& attachment,bool& pending) noexcept {
    entity=nullptr;side=-1;attachment=nullptr;pending=false;
    const auto& b=f->spatial;
    if(!pendingBoltLeafReadyR95||!XR34Current(o)||!id||id!=b.boltId||!b.boltEntity)return false;
    entity=reinterpret_cast<void*>(b.boltEntity);side=pendingBoltLeafSideR95>=0?pendingBoltLeafSideR95:1-o.source.primary;
    pending=b.handBoltPendingR95;attachment=reinterpret_cast<void*>(b.boltAttachment);return true;
}
#include "passenger_consumer_d82_actual.inc"

// Shape-compatible completed native wrist/goal packet. The actual source
// producer below performs the ownership/stamp/relative-pose calculations.
struct QuatT28 {float q[4]{0,0,0,1},t[3]{};};
struct CommittedFrame6695 {std::uint64_t publicationR24=2;x::Source reloadSourceXR34{};std::uint64_t doorTrackingEpoch66=4;pm::Stamp stamp{};QuatT28 native[2]{},allowed[2]{},propRequested5916c1[2]{};
    bool valid[2]{true,true},propRequestedValid5916c1[2]{true,true};float world[8]{0,0,0,1,0,0,0,1};};
struct TrackingSnapshot {std::uint64_t sequence=0,tick=0,source_epoch=0;bool allowed=true,head_held=false,right_held=false,left_held=false;
    bool head_valid=true,right_valid=true,left_valid=true;
    float right_grip=0,left_grip=0;};
static ph::Pose PhysPose6698(const QuatT28& p) noexcept{return {{p.q[0],p.q[1],p.q[2],p.q[3]},{p.t[0],p.t[1],p.t[2]}};}
static bool ValidPose6689(const QuatT28& p) noexcept{return ph::finite(PhysPose6698(p));}
// Chest/quiver geometry is an explicit tracking leaf; these tests do not
// measure headset anatomy or change the already-captured entry gesture.
static void ChestFlagsD66C(x::Source& out,const TrackingSnapshot&) noexcept{
    out.chestKnown=f->source.chestKnown;out.quiverKnownD72=f->source.quiverKnownD72;
    for(int side=0;side<2;++side){out.handInChest[side]=f->source.handInChest[side];out.handInQuiverD72[side]=f->source.handInQuiverD72[side];}
}
#include "completed_source_d82_actual.inc"

struct RetailScopeD82 {
    std::array<unsigned char,32> empty{};
    unsigned first=0,layers=1;
    RetailScopeD82(Fixture& a,int kind):first(kind==15?0u:6u),layers(kind==15?3u:2u){
        a.block(empty);a.put(a.p(empty)+12,0u);
        a.put(a.p(a.scope)+0x30,first);a.put(a.p(a.scope)+0x34,layers);
        a.put(a.p(a.character)+0x110+first*0x40+0x48,a.entry());
        for(unsigned i=1;i<layers;++i)a.put(a.p(a.character)+0x110+(first+i)*0x40+0x48,a.p(empty)+16);
    }
};
static bool binding82(const x::Owner& o,x::Spatial& out) noexcept {out={};out.serial=o.serial;out.qualified=XR34Current(o);return out.qualified;}
static bool animateMechanismD87=false;
static bool spatial82(const x::Owner& o,const x::Source& accepted,x::Spatial& out) noexcept {
    if(!spatial(o,accepted,out))return false;x::Clock clock{};
    if(!XR34NativeClock(o,g_crossbow_xr34.clip(),clock))return false;
    // Native mechanism pose is a fixture leaf. Its identity/time join uses the
    // actual adapter clock, not a generated timestamp or an invented event.
    out.stringMechanismD78=true;out.stringClockD78=clock;out.stringTickD78=accepted.tick;
    if(animateMechanismD87){const double distance=f->spatial.travel*clock.normalized/std::max(.001f,g_crossbow_xr34.clip().cock);
        out.stringCurrentD78={0,-distance,0};out.toolWristD78.p[1]=float(distance);out.toolHandleD72={0,distance,0};}
    out.toolMechanismD72=true;out.mechanismClockD78=clock;out.mechanismTickD78=accepted.tick;out.toolMountedPoseD78=true;
    return true;
}
static void start82(Fixture& a,bool load=false,bool transitionD87=false){
    hostOwnerTransactionAvailableD82=true;g_crossbow_xr34=x::Client{};g_crossbow_base_xr34=0;
    x::revokedNativeLease=0;x::completedNativeLease=0;x::admissionMailbox.publish({});x::passengerMailbox.publish({});
    g_henry_ctrl=a.controller.data();x::registrationReader=&FixtureRegistrationXR34;x::identityReader=&FixtureIdentityXR34;
    x::bindingReader=&binding82;x::RefreshRegistration();x::SetSpatialReader(&spatial82);x::PublishSource(a.source);a.events(load);
    if(transitionD87){a.put(a.p(a.eventStorage)+12,1u);a.event(0,.2f,0x12345678u);}
    XR34PostStartNative(a.parent.data(),true);a.activate();CHECK82(g_crossbow_xr34.pending());
    a.put(a.p(a.parent)+(load?0x99:0x9A),(unsigned char)1);
    XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);
    CHECK82(g_crossbow_xr34.status().phase==(transitionD87?x::Phase::NativeTransitionD80:x::Phase::Setup));
    XR34PostControllerNative(a.controller.data());
}
static x::Admission admission82(Fixture& a){
    x::Admission admission{};CHECK82(x::ReadAdmission(admission,a.now));
    CHECK82(admission.clock.entry==a.entry()&&admission.childStatus==2&&admission.root==a.p(a.scope));
    CHECK82(ReloadPassengerCurrentXR34(x::PassengerFromAdmission(admission),a.source.character,a.source.body,a.now));
    return admission;
}
static void completeFirstPose82(Fixture& a,const x::Admission& admission){
    const auto p=x::PassengerFromAdmission(admission);CommittedFrame6695 packet{};TrackingSnapshot tracking{};QuatT28 weapon{};
    ++a.now;tracking.sequence=a.source.sample+1;tracking.tick=a.now;tracking.source_epoch=a.source.tracking;
    tracking.right_grip=float(a.source.grip[0]);tracking.left_grip=float(a.source.grip[1]);
    packet.stamp.actor=a.source.character;packet.stamp.epoch=a.source.body;packet.stamp.sample=tracking.sequence;packet.stamp.tick=tracking.tick;
    const auto next=CompletedReloadPoseSourceXR34(p,packet,weapon,tracking,a.now);
    CHECK82(next.allowed&&next.completedLease==admission.owner.serial&&next.sample==tracking.sequence&&next.tick==tracking.tick);
    CHECK82(next.intentKnownD78&&next.nativePoseClockD78==admission.clock.entry&&next.nativePoseNormalizedD78==admission.clock.normalized);
    CHECK82(next.nativePoseActiveTimeD78==admission.clock.activeTime&&admission.owner.source.completedLease==0);
    a.source=next;x::PublishSource(a.source);x::Admission old{};CHECK82(!x::ReadAdmission(old,a.now));
    tick(a);CHECK82(g_crossbow_xr34.status().sourceEstablished&&g_crossbow_xr34.pending());
}
static void input82(Fixture& a,double grip,x::Vec point={}){
    a.now+=20;x::Passenger p{};p.owner=g_crossbow_xr34.status().owner;p.frame=a.source;p.workingHand=0;
    CHECK82(XR34NativeClock(p.owner,g_crossbow_xr34.clip(),p.clock));
    CommittedFrame6695 packet{};TrackingSnapshot tracking{};QuatT28 weapon{};
    tracking.sequence=a.source.sample+1;tracking.tick=a.now;tracking.source_epoch=a.source.tracking;tracking.right_grip=float(grip);
    packet.stamp.actor=a.source.character;packet.stamp.epoch=a.source.body;packet.stamp.sample=tracking.sequence;packet.stamp.tick=tracking.tick;
    for(auto* wrist:{&packet.allowed[0],&packet.propRequested5916c1[0]}){
        wrist->t[0]=float(point.x);wrist->t[1]=float(point.y);wrist->t[2]=float(point.z);}
    a.source=CompletedReloadPoseSourceXR34(p,packet,weapon,tracking,a.now);CHECK82(a.source.allowed);
    x::PublishSource(a.source);
}
struct ScanInfoD82 {std::uintptr_t list,path;int first,animation,last;float weight;};
static ScanInfoD82 scan82(Fixture& a,bool other=false){return {a.p(a.caf)+0x48,base+0x406B7C0,0,other?99:5,2,1};}
static void observe82(Fixture& a,ScanInfoD82& info){
    a.blocks.emplace_back(reinterpret_cast<std::uintptr_t>(&info),sizeof(info));
    {XR34ScanNative scan(a.character.data()+0x110,false,0,.01f,&info,0);CHECK82(!scan.current);}
    a.blocks.pop_back();
}
#ifndef KCD2VR_SCOPE_NO_MAIN
int main(){
    // Both real authored native profiles must pass both production gates,
    // before any completed-pose lease exists. Previous fixtures were 0+1.
    for(int kind:{10,14,15}){Fixture a;a.source.classId=kind;a.source.physicalD81=true;a.source.instantD72=false;a.source.intentKnownD78=true;
        a.source.entryGripD72=kind==10?1:2;a.source.entryGripTickD72=a.now;a.source.grip[0]=.9;
        a.source.chestKnown=true;a.source.handInChest[0]=true;
        if(kind==15){a.spatial.axis={0,1,0};a.spatial.travel=.08;a.spatial.crankQualified=true;}
        RetailScopeD82 profile(a,kind);start82(a);const auto first=admission82(a);const auto deadline=first.deadline;
        CHECK82(a.source.completedLease==0&&!g_crossbow_xr34.status().sourceEstablished);
        completeFirstPose82(a,first);CHECK82(g_crossbow_xr34.status().firstPoseDeadline==deadline);
        for(int i=0;i<4;++i){input82(a,.9);nativeFrame(a);}
        if(!g_crossbow_xr34.status().toolEngagedD72)std::cerr<<"kind="<<kind<<" phase="<<unsigned(g_crossbow_xr34.status().phase)
            <<" reason="<<unsigned(g_crossbow_xr34.status().reason)<<" progress="<<g_crossbow_xr34.status().progress
            <<" clock="<<a.get<float>(a.entry()+0x44)<<" speed="<<a.get<float>(a.p(a.child)+0x5C)<<"\n";
        CHECK82(g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0);
        for(int i=0;i<8;++i){input82(a,.9);nativeFrame(a);}CHECK82(a.cocks==0&&g_crossbow_xr34.status().progress==0);
        input82(a,.9,{0,kind==15?.09:-.15,0});nativeFrame(a);
        CHECK82(a.cocks==1&&a.loads==0&&g_crossbow_xr34.status().phase==x::Phase::Tail);
        XR34BeforeStopNative(a.parent.data());CHECK82(a.get<float>(a.p(a.child)+0x5C)==2.5f);
    }
    // Cocked player bolt placement is authored on UpperBody, including heavy.
    // Real first-pose source production cannot accidentally enable autoload.
    for(int kind:{10,14,15}){Fixture a;a.source.classId=kind;a.source.physicalD81=true;a.source.instantD72=false;a.source.intentKnownD78=true;
        a.source.quiverKnownD72=true;RetailScopeD82 profile(a,10);start82(a,true);completeFirstPose82(a,admission82(a));
        input82(a,0);nativeFrame(a);for(int i=0;i<5;++i){input82(a,.9);nativeFrame(a);}
        CHECK82(!a.spatial.handBoltBound&&a.loads==0);
        input82(a,0);nativeFrame(a);a.source.handInQuiverD72[0]=true;input82(a,.9);nativeFrame(a);
        a.source.handInQuiverD72[0]=false;input82(a,.9);nativeFrame(a);
        CHECK82(a.spatial.handBoltBound&&g_crossbow_xr34.status().phase==x::Phase::PlaceBolt);
        const auto events=a.originalEvents;const auto serial=g_crossbow_xr34.status().owner.serial;
        for(int i=0;i<12;++i){input82(a,0);nativeFrame(a);}
        CHECK82(g_crossbow_xr34.pending()&&a.loads==0&&a.originalEvents==events&&a.get<float>(a.p(a.child)+0x5C)==0);
        input82(a,.9);nativeFrame(a);a.spatial.handBoltPlaced=true;input82(a,0);nativeFrame(a);
        CHECK82(a.loads==1&&a.spatial.nockBound&&g_crossbow_xr34.status().owner.serial==serial);
        XR34BeforeStopNative(a.parent.data());
    }
    // Native slot count is not an arbitrary multi-layer bypass. Empty slots,
    // exact known profile and unique root CAF remain required on BOTH sides.
    for(int kind:{10,15})for(int bad=0;bad<4;++bad){Fixture a;a.source.classId=kind;RetailScopeD82 profile(a,kind);start82(a);
        auto p=x::PassengerFromAdmission(admission82(a));
        if(bad==0)a.put(a.p(profile.empty)+12,1u);
        if(bad==1)a.put(a.p(a.scope)+0x34,4u);
        if(bad==2)a.put(a.p(a.character)+0x110+(profile.first+1)*0x40+0x48,std::uintptr_t(0));
        if(bad==3)a.put(a.p(a.scope)+0x30,3u);
        x::Clock clock{};CHECK82(!XR34NativeClock(g_crossbow_xr34.status().owner,g_crossbow_xr34.clip(),clock));
        CHECK82(!ReloadPassengerCurrentXR34(p,a.source.character,a.source.body,a.now));XR34BeforeStopNative(a.parent.data());
    }
    // An idle/look/facial scan cannot invalidate the just-issued first-pose
    // publication, and cannot cancel it merely because the owner lock is busy.
    for(bool busy:{false,true}){Fixture a;RetailScopeD82 profile(a,10);start82(a);const auto first=admission82(a);
        const auto publication=x::passengerPublication.load();auto other=scan82(a,true);
        hostOwnerTransactionAvailableD82=!busy;observe82(a,other);hostOwnerTransactionAvailableD82=true;
        CHECK82(x::passengerPublication.load()==publication&&x::revokedNativeLease.load()!=first.owner.serial);
        admission82(a);XR34BeforeStopNative(a.parent.data());}
    // D85: an unobserved scan grants no witness; only an effectful event
    // without a witness revokes. Contention alone is not a native effect.
    for(bool stale:{false,true}){Fixture a;RetailScopeD82 profile(a,10);start82(a);const auto first=admission82(a);
        auto info=scan82(a,stale);if(stale)x::passengerPublication.fetch_add(1);
        hostOwnerTransactionAvailableD82=false;observe82(a,info);hostOwnerTransactionAvailableD82=true;
        CHECK82(x::revokedNativeLease.load()!=first.owner.serial);
        CHECK82(!g_crossbow_scan_xr34.valid);XR34BeforeStopNative(a.parent.data());
        CHECK82(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f);}
    // Execute the registered production observer, including its lock boundary.
    // Unknown CRC and the game's inert heavy typo cannot cancel the lease.
    for(bool busy:{false,true})for(unsigned crc:{0x12345678u,x::kDespawnLeverTypoCrcD66C}){
        Fixture a;RetailScopeD82 profile(a,10);start82(a);const auto first=admission82(a);
        const auto publication=x::passengerPublication.load();std::array<unsigned char,0x58> event{};a.block(event);a.put(a.p(event)+0x1C,crc);
        hostOwnerTransactionAvailableD82=!busy;XR34ExistingEventObserver(a.parent.data(),event.data());hostOwnerTransactionAvailableD82=true;
        CHECK82(g_crossbow_xr34.pending()&&x::passengerPublication.load()==publication&&x::revokedNativeLease.load()!=first.owner.serial);
        admission82(a);XR34BeforeStopNative(a.parent.data());}
    // Every effectful native case needs its exact owned scan, not only the five
    // visible milestones. A busy callback or unobserved event still withdraws.
    for(bool busy:{false,true})for(unsigned crc:{0xAFE0B3FFu,0x34857AFAu,0x3D6870EBu,0x863A26ABu,0x864932ACu,0x8ABB129Au,
                                               0xAE150612u,0xB337FBDCu,0xC14030E2u,0xDF6F00DFu,0xEF667DEAu,0xF04FE686u}){
        Fixture a;RetailScopeD82 profile(a,10);start82(a);const auto first=admission82(a);
        std::array<unsigned char,0x58> event{};a.block(event);a.put(a.p(event)+0x1C,crc);
        hostOwnerTransactionAvailableD82=!busy;XR34ExistingEventObserver(a.parent.data(),event.data());hostOwnerTransactionAvailableD82=true;
        CHECK82(!g_crossbow_xr34.pending()||x::revokedNativeLease.load()==first.owner.serial);
        tick(a);CHECK82(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f);}
    // Inactive global hook adds no native reads, transaction or publication.
    {Fixture a;x::cleanupInterested=false;a.traceReadsD69=true;const auto publication=x::passengerPublication.load();auto info=scan82(a,true);
        observe82(a,info);CHECK82(a.readsD69.empty()&&x::passengerPublication.load()==publication);}
    std::cout<<"D82 crossbow production scope/first-pose/scan: "<<checksD82<<" checks passed; native/tracking leaves mocked\n";
}

#endif
