// Analytic native descriptors and explicit engine leaves around literal
// production BuildCrossbowPosesXR34 + CrossbowAttachmentXR34. No game emulation.
#include <array>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <mutex>
#include "vrik/pose_math.h"
#include "subsystems/ranged_reload/crossbow_native_xr34.h"
#include "spanner_kernel_actual.inc"
namespace ph=kcd2vr::physical;
namespace pm=kcd2vr::pose_core;
namespace core=kcd2vr::core;
namespace xcXR34=kcd2vr::crossbow_native_xr34;
struct QuatT28 {float q[4];float t[3];};
struct V3 {float x{},y{},z{};};
struct Q4 {float x{},y{},z{},w{1};};
struct ModelWorld6692 {Q4 q{};V3 t{};float scale=1;};
struct Matrix34V6550 {float m[12]{};};
#include "spanner_math_actual.inc"
#include "spanner_types_actual.inc"
struct CommittedFrame6695 {
    pm::Stamp stamp{};QuatT28 native[2]{},allowed[2]{};bool valid[2]{};
    std::uint64_t publicationR24=2,doorTrackingEpoch66=1;
    xcXR34::Source reloadSourceXR34{};RangedTargetXR34 reloadTargetXR34{};
};
static std::recursive_mutex g_ranged_pose_mutex_xr34;
static core::SnapshotMailbox<RangedSocketXR34> g_ranged_socket_xr34;
static core::SnapshotMailbox<RangedPlacedXR34> g_ranged_placed_xr34;
static std::atomic<bool> g_committed_pose_valid6689{true};
static std::atomic<void*> g_henry_char_attachment_manager{nullptr};
static std::uint64_t now=1000,completed=2,serial=1;
static unsigned checks=0,writes=0,heavyCalls=0;
static bool ownerOK=true,writerOK=true,bodyOK=true,passengerOK=true,readbackOK=true,bindingOK=true,unclaimed=true;
static void (*beforeOwnerCheck)()=nullptr;
static std::array<unsigned char,256> item{},action{},parent{},object{},attachment{},manager{},entity{},character{};
static RangedSocketXR34 socketFixture{};
static xcXR34::Status status{};
static xcXR34::Passenger passenger{};
static RangedMechanismD78 mechanism{};
static CommittedFrame6695 committed{};
static ModelWorld6692 bodyWorld{};
static float natural[8]{0,0,0,1,0,0,0,1},written[8]{};
static Matrix34V6550 achieved{};
static std::array<QuatT28,8> original{},reference{};
static constexpr int rightHand=2,leftHand=3,toolSlot=4;
static ph::Pose authoredGrip{};
static void check(bool yes,const char* label){++checks;if(!yes){std::fprintf(stderr,"FAIL_R108 %s\n",label);std::exit(1);}}
// R108: the grip the production builder must take for class 14 = the animated
// hand-slot relation of the cocking clip at the spawn edge (R103), never the
// rig rest-pose relation (R105). Computed from the SAME inputs the builder sees.
static ph::Pose spawnEdgeGrip(){
    const auto nativeEntity=AbsoluteFromRelative(original[toolSlot],socketFixture.relative);
    return PhysPose6698(RelativeFromAbsolute(nativeEntity,committed.native[0]));}
template<class T> static void put(std::array<unsigned char,256>& p,std::size_t offset,const T& value){std::memcpy(p.data()+offset,&value,sizeof(value));}
template<class T> static bool Read(const void* p,T& value){if(!p)return false;std::memcpy(&value,p,sizeof(value));return true;}
static std::uint64_t GetTickCount64(){return now;}
static bool PhysVft6698(void* p,std::uintptr_t){return p!=nullptr;}
static bool PhysEmpty6698(std::uint64_t key){return !key;}
static void* EntityById6694(unsigned id){return id==socketFixture.id?entity.data():nullptr;}
static bool RangedOwnerXR34(const xcXR34::Owner& o){if(beforeOwnerCheck){auto f=beforeOwnerCheck;beforeOwnerCheck=nullptr;f();}return ownerOK&&xcXR34::sameOwner(o,status.owner);}
static bool ReloadNativeOwnerXR34(const xcXR34::Owner& o,bool){return RangedOwnerXR34(o);}
static bool ReloadPoseWriterCurrentXR34(const xcXR34::Owner& o,std::uint64_t=0){return writerOK&&RangedOwnerXR34(o);}
namespace tracking59c6 {static std::atomic<std::uint64_t> epoch{1};}
namespace kcd2vr::reload_throw_claim_t25 {static bool workingHandUnclaimed(const xcXR34::Source&){return unclaimed;}}
static bool RangedBoundHandXR34(const xcXR34::Owner& o,unsigned id,void* e,int& side,void*& a){
    side=socketFixture.side;a=attachment.data();return bindingOK&&RangedOwnerXR34(o)&&id==socketFixture.id&&e==entity.data();}
static bool RangedBoundToolIdentityD72(const xcXR34::Owner& o,unsigned id,void* e,int& side,void*& a){return RangedBoundHandXR34(o,id,e,side,a);}
static bool ManualArrowHelper64(void*,QuatT28& helper){helper=socketFixture.helper;return true;}
static bool RangedNativeBindingXR34(const xcXR34::Owner& o,xcXR34::Spatial& b){
    if(!bindingOK||!RangedOwnerXR34(o))return false;b={};
    b.toolBound=!socketFixture.bolt;b.handBoltBound=socketFixture.bolt;b.toolWeaponBoundD72=status.owner.source.classId==15;
    b.toolId=b.boltId=socketFixture.id;b.toolEntity=b.boltEntity=reinterpret_cast<std::uintptr_t>(entity.data());
    b.toolItem=reinterpret_cast<std::uintptr_t>(item.data());b.toolKey=socketFixture.toolKey;
    b.toolAttachment=reinterpret_cast<std::uintptr_t>(attachment.data());return true;}
static bool RangedReadMechanismD78(const xcXR34::Owner&,const xcXR34::Source&,const xcXR34::Spatial&,RangedMechanismD78& out){out=mechanism;return passengerOK;}
static bool ReadReloadPassengerXR34(xcXR34::Passenger& out,std::uintptr_t,std::uint64_t,std::uint64_t){out=passenger;return passengerOK;}
static bool ReadCommittedPacket6695(CommittedFrame6695& out){out=committed;return bodyOK;}
static std::uint64_t ReadCompletedPublicationT23(){return completed;}
static bool PhysAttachmentWorld164(void*,std::uintptr_t,ModelWorld6692& out){out=bodyWorld;return true;}
static bool ReadModelWorld6692(void*,ModelWorld6692& out){out=bodyWorld;return true;}
static bool ReadAttachmentQs6689(void*,float (&out)[8]){std::memcpy(out,natural,sizeof(out));return true;}
static bool CrossbowWeaponAttachmentD72(const xcXR34::Status&,void*,void*,unsigned){return false;}
static bool RangedItemEntityXR34(void* tool,unsigned& id,void*& e){id=socketFixture.id;e=entity.data();return tool==item.data();}
static bool PlaceBothConsumers6689(void* self,void*,const float (&qs)[8],bool& child){
    ++writes;child=false;std::memcpy(written,qs,sizeof(qs));
    pm::Quat q{qs[0],qs[1],qs[2],qs[3]};
    for(int axis=0;axis<3;++axis){float scale=0;Read(static_cast<unsigned char*>(self)+0xC+axis*4,scale);
        const pm::Vec3 unit=axis==0?pm::Vec3{1,0,0}:axis==1?pm::Vec3{0,1,0}:pm::Vec3{0,0,1};
        const auto v=pm::rotate(q,unit)*(scale*qs[7]);achieved.m[axis]=float(v.x);achieved.m[axis+4]=float(v.y);achieved.m[axis+8]=float(v.z);}
    achieved.m[3]=qs[4];achieved.m[7]=qs[5];achieved.m[11]=qs[6];return true;}
static bool PlacementReadback6689(void*,void*,unsigned,const float (&)[8],bool& child){child=false;return readbackOK;}
static bool GetEntityWorldTmV6552(void*,unsigned,Matrix34V6550& out){out=achieved;return true;}
struct ManualBow596 {void* player=nullptr;void* entity=nullptr;unsigned hand=0;int side=-1;std::uint64_t epoch=0,request=0;};
static bool ManualArrowVisible42(const ManualBow596&){return true;}
namespace diagnostics::capture168 {enum class Domain{Reload};static bool active(Domain,std::uint64_t){return false;}}
namespace diagnostic_trace {static void Write(const char*){} template<class D> static void WriteFor(D,const char*){}}
#include "spanner_path_actual.inc"

static QuatT28 qt(ph::Pose p){return {{float(p.q.x),float(p.q.y),float(p.q.z),float(p.q.w)},{float(p.p.x),float(p.p.y),float(p.p.z)}};}
static ph::Pose pose(double angle,pm::Vec3 axis,pm::Vec3 p){
    axis=*pm::unit(axis);const double s=std::sin(angle*.5);return {{axis.x*s,axis.y*s,axis.z*s,std::cos(angle*.5)},p};}
static bool near(ph::Pose a,ph::Pose b,double tolerance=2e-5){
    return pm::length(a.p-b.p)<tolerance&&pm::angle(a.q,b.q)<tolerance;}
static void publish(){
    check(xcXR34::statusMailbox.publish(status),"status publish");
    xcXR34::activeNativeLease.store(status.owner.serial);xcXR34::revokedNativeLease.store(0);}
static void descriptor(){
    put(attachment,0x20,static_cast<void*>(object.data()));put(attachment,0x28,static_cast<void*>(manager.data()));
    put(manager,0x70,static_cast<void*>(character.data()));put(attachment,0x78,std::int16_t(socketFixture.joint));
    put(attachment,0x9C,socketFixture.relative);put(object,8,socketFixture.id);put(item,0x30,socketFixture.toolKey);
    put(action,0x80,static_cast<void*>(item.data()));put(parent,0x88,socketFixture.bolt?socketFixture.id:0u);
    g_henry_char_attachment_manager.store(manager.data());}
static void reset(int classId=14,bool bolt=false,bool physical=true){
    now+=100;ownerOK=writerOK=bodyOK=passengerOK=readbackOK=bindingOK=unclaimed=true;beforeOwnerCheck=nullptr;
    writes=heavyCalls=0;g_ranged_grip_xr34={};g_ranged_heavy_grip_d72={};g_ranged_placed_xr34.publish({});completed+=2;g_committed_pose_valid6689.store(true);
    status={};status.owner.serial=++serial;auto& s=status.owner.source;
    s.player=s.equipment=s.manager=s.weaponItem=s.ammoItem=1;s.character=reinterpret_cast<std::uintptr_t>(character.data());
    s.key=s.ammoKey=s.generation=s.body=s.tracking=s.request=s.actor=1;s.sample=now;s.tick=now;
    s.allowed=true;s.primary=1;s.classId=classId;s.physicalD81=physical;s.instantD72=false;s.bodyPublicationR95=completed;
    status.owner.main=reinterpret_cast<std::uintptr_t>(action.data());status.owner.parent=reinterpret_cast<std::uintptr_t>(parent.data());
    status.clockOwned=true;status.placeTool=!bolt;status.placeBolt=bolt;status.phase=xcXR34::Phase::Setup;
    socketFixture={};socketFixture.owner=status.owner;socketFixture.id=77;socketFixture.self=object.data();
    socketFixture.attachment=attachment.data();socketFixture.entity=entity.data();socketFixture.character=character.data();
    socketFixture.toolItem=bolt?nullptr:item.data();socketFixture.toolKey=bolt?0:9001;socketFixture.side=0;socketFixture.joint=toolSlot;socketFixture.bolt=bolt;
    socketFixture.relative=qt(pose(.27,{1,2,-1},{.01,-.025,.015}));socketFixture.helper=qt(pose(.4,{1,0,0},{.035,-.06,.02}));
    authoredGrip=pose(.83,{1,-2,3},{.13,-.08,.04});
    for(auto& p:original)p=qt(ph::Pose{});for(auto& p:reference)p=qt(ph::Pose{});
    reference[rightHand]=qt(pose(.67,{2,1,-1},{.2,.4,1.1}));reference[leftHand]=qt(pose(-.4,{1,2,2},{-.2,.2,1.2}));
    const auto refEntity=ph::compose(PhysPose6698(reference[rightHand]),ph::inverse(authoredGrip));
    reference[toolSlot]=qt(ph::compose(refEntity,ph::inverse(PhysPose6698(socketFixture.relative))));
    original=reference;
    committed={};committed.stamp={s.body,s.sample,s.tick,s.character};committed.publicationR24=completed;committed.doorTrackingEpoch66=s.tracking;
    committed.valid[0]=committed.valid[1]=true;committed.native[0]=original[rightHand];committed.native[1]=original[leftHand];
    committed.allowed[0]=qt(pose(-1.1,{2,-1,3},{.3,-.1,1.35}));committed.allowed[1]=qt(pose(.3,{1,0,0},{-.3,.4,1.2}));
    committed.reloadSourceXR34=s;
    passenger={};passenger.owner=status.owner;passenger.publication=2;passenger.clip.cocking=!bolt;
    passenger.clip.spawn=.2057f;passenger.clock.qualified=true;passenger.clock.normalized=passenger.clip.spawn-1e-4f;
    xcXR34::passengerPublication.store(passenger.publication);
    bodyWorld={{0,0,0,1},{0,0,0},1};natural[7]=1;
    for(int axis=0;axis<3;++axis)put(object,0xC+axis*4,1.f);
    descriptor();publish();
}
static void build(bool withReference=true,int right=rightHand,int left=leftHand){
    if(withReference&&right==rightHand&&left==leftHand){
        struct Rig {int hand_r,hand_l;};
        struct CoherentCallFixture {
            std::array<QuatT28,8>& original;std::array<QuatT28,8>& rest;std::size_t count;Rig rig;
        } s{original,reference,original.size(),{rightHand,leftHand}};
        auto& packet=committed;
#include "spanner_caller_actual.inc"
        return;
    }
#ifdef SPANNER_REFERENCE_R105
    BuildCrossbowPosesXR34(committed,original.data(),original.size(),withReference?reference.data():nullptr,right,left);
#else
    (void)withReference;(void)right;(void)left;
    BuildCrossbowPosesXR34(committed,original.data(),original.size());
#endif
}
static void nextSample(){++committed.stamp.sample;committed.reloadSourceXR34.sample=committed.stamp.sample;}
static void ready(bool withReference=true){build(withReference);nextSample();build(withReference);}
static void place(){committed.reloadTargetXR34.publicationR95=completed;check(CrossbowAttachmentXR34(object.data(),attachment.data()),"attachment consumed");}
static void animatedOffset(){
    // A legitimate hand-slot animation differs from the rig's native grip.
    // This is an analytic stress transform, not an observed asset correction.
    const auto motion=pose(2.4,{0,1,1},{.16,-.09,.11});
    original[toolSlot]=qt(ph::compose(PhysPose6698(reference[toolSlot]),motion));
    original[rightHand]=reference[rightHand];committed.native[0]=original[rightHand];}
static void testSpawnEdgeGrip(){
    reset();animatedOffset();ready();check(committed.reloadTargetXR34.valid,"native spanner target exists");
    const auto edge=spawnEdgeGrip();
    check(near(PhysPose6698(committed.reloadTargetXR34.grip),edge),"spawn edge grip is the clip's animated hand-slot relation");
    check(!near(PhysPose6698(committed.reloadTargetXR34.grip),authoredGrip),"spawn edge grip is not the rig rest-pose relation (R105 reverted)");
    check(near(ph::compose(PhysPose6698(committed.reloadTargetXR34.model),edge),PhysPose6698(committed.allowed[0])),"spawn edge grip maps to authoritative wrist");
    // Without any slot animation the clip relation and the rest relation agree.
    reset();ready();check(committed.reloadTargetXR34.valid&&near(PhysPose6698(committed.reloadTargetXR34.grip),authoredGrip),"unanimated slot: clip relation equals rest relation");
}
static void testTimingAndIdentity(){
    reset();passenger.clock.normalized=0;build();check(!committed.reloadTargetXR34.valid&&!g_ranged_grip_xr34.valid,"pre-spawn no calibration");
    passenger.clock.normalized=passenger.clip.spawn-1e-4f;nextSample();build();check(!committed.reloadTargetXR34.valid,"first edge sample waits");
    build();check(!committed.reloadTargetXR34.valid,"same sample cannot finish edge latch");
    nextSample();build();check(committed.reloadTargetXR34.valid,"second edge sample can place in Setup");
    auto remembered=g_ranged_grip_xr34.grip;auto preserved=status;
    original[toolSlot]=qt(pose(2.1,{1,1,0},{.4,.3,.8}));committed.allowed[0]=qt(pose(.7,{-1,1,3},{.1,.5,1}));nextSample();build();
    check(near(PhysPose6698(committed.reloadTargetXR34.grip),PhysPose6698(remembered)),"same native item retains capture through animation");
    place();check(writes==1,"one placement writer");
    check(status.phase==preserved.phase&&status.clockOwned==preserved.clockOwned&&status.speed==preserved.speed&&
          status.toolTakenD72==preserved.toolTakenD72&&status.progress==preserved.progress,"pose path does not advance reload state or clock");
    reset();writerOK=false;ready();check(!committed.reloadTargetXR34.valid&&!g_ranged_grip_xr34.valid,"writer withdrawal rejects capture");
    reset();ownerOK=false;ready();check(!committed.reloadTargetXR34.valid,"owner withdrawal rejects capture");
    reset();ready();++status.owner.serial;publish();build();check(!committed.reloadTargetXR34.valid,"new owner cannot reuse old socket");
    reset();ready();socketFixture.toolKey++;descriptor();nextSample();build();check(!committed.reloadTargetXR34.valid,"new native tool needs new edge sample");
    reset();ready();committed.stamp.epoch++;build();check(!committed.reloadTargetXR34.valid,"body epoch revokes target");
    reset();ready();committed.valid[0]=false;build();check(!committed.reloadTargetXR34.valid,"invalid authoritative hand revokes target");
    reset();passengerOK=false;ready();check(!committed.reloadTargetXR34.valid,"unavailable passenger cannot first capture");
    reset();status.phase=xcXR34::Phase::Span;publish();build();check(committed.reloadTargetXR34.valid,"Span calibration remains available");
    reset();status.phase=xcXR34::Phase::Tail;publish();build();check(committed.reloadTargetXR34.valid,"Tail calibration remains available");
}
static void testReferenceValidity(){
#ifdef SPANNER_REFERENCE_R105
    // R108: the rig reference is printed in the F11 row only; it never gates or
    // changes the spawn-edge grip (a missing/invalid reference is not a refusal).
    reset();animatedOffset();ready(false);check(committed.reloadTargetXR34.valid&&g_ranged_grip_xr34.valid,"missing reference does not block the spawn edge grip");
    check(near(PhysPose6698(committed.reloadTargetXR34.grip),spawnEdgeGrip()),"missing reference: same spawn edge grip");
    reset();animatedOffset();reference[rightHand].q[0]=std::numeric_limits<float>::quiet_NaN();ready();
    check(committed.reloadTargetXR34.valid&&near(PhysPose6698(committed.reloadTargetXR34.grip),spawnEdgeGrip()),"invalid reference wrist: same spawn edge grip");
    reset();animatedOffset();reference[toolSlot].q[3]=0;ready();
    check(committed.reloadTargetXR34.valid&&near(PhysPose6698(committed.reloadTargetXR34.grip),spawnEdgeGrip()),"invalid reference socket: same spawn edge grip");
    reset();animatedOffset();build(true,-1);nextSample();build(true,-1);check(committed.reloadTargetXR34.valid,"unknown hand joint does not block");
    reset();animatedOffset();build(true,8);nextSample();build(true,8);check(committed.reloadTargetXR34.valid,"out of bounds hand joint does not block");
    reset();ready();const auto saved=g_ranged_grip_xr34.grip;nextSample();build(false);
    check(committed.reloadTargetXR34.valid&&near(PhysPose6698(saved),PhysPose6698(committed.reloadTargetXR34.grip)),"same calibrated binding survives reference miss");
#endif
}
static void testHeavyMountedPath(){
    reset(15);auto& s=status.owner.source;s.intentKnownD78=true;
    const auto nativeWeapon=pose(.3,{1,2,1},{-.3,.2,.8});
    const auto nativeToolLocal=pose(-.8,{3,-1,2},{.04,.1,.12});
    const auto nativeWrist=ph::compose(ph::compose(nativeWeapon,nativeToolLocal),authoredGrip);
    const auto trackedWeapon=pose(-.6,{2,1,3},{.3,.4,.9});
    auto wrist=[](ph::Pose p){return xcXR34::Wrist{{float(p.q.x),float(p.q.y),float(p.q.z),float(p.q.w)},{float(p.p.x),float(p.p.y),float(p.p.z)}};};
    s.nativeWeaponD72=wrist(nativeWeapon);s.nativeWrist[0]=wrist(nativeWrist);s.weaponModel=wrist(trackedWeapon);
    passenger.clock.entry=0x100;passenger.clock.activeTime=1;
    s.nativePoseClockD78=passenger.clock.entry;s.nativePoseNormalizedD78=passenger.clock.normalized;s.nativePoseActiveTimeD78=passenger.clock.activeTime;
    committed.reloadSourceXR34=s;socketFixture.owner=status.owner;passenger.owner=status.owner;publish();
    mechanism={};mechanism.owner=status.owner;mechanism.clock=passenger.clock;mechanism.toolModel=qt(nativeToolLocal);
    mechanism.character=character.data();mechanism.skeleton=manager.data();mechanism.joint=toolSlot;mechanism.relative=socketFixture.relative;
    const auto bw=pose(.9,{1,3,2},{11,7,-3});bodyWorld={{float(bw.q.x),float(bw.q.y),float(bw.q.z),float(bw.q.w)},{11,7,-3},1.3f};
    place();check(g_ranged_heavy_grip_d72.ready,"actual heavy writer captures native pre-spawn grip");
    check(near(g_ranged_heavy_grip_d72.grip,authoredGrip),"heavy animated native grip is unchanged");
    const auto carried=PhysCurrentWorld164(ph::compose(PhysPose6698(committed.allowed[0]),ph::inverse(authoredGrip)),bodyWorld);
    check(near({{written[0],written[1],written[2],written[3]},{written[4],written[5],written[6]}},carried,3e-5),"heavy unmounted actual writer maps native grip to wrist");
    status.toolMountedD78=true;publish();place();
    const auto mounted=PhysCurrentWorld164(ph::compose(trackedWeapon,nativeToolLocal),bodyWorld);
    check(near({{written[0],written[1],written[2],written[3]},{written[4],written[5],written[6]}},mounted,3e-5),"heavy mounted actual writer follows measured native mechanism");
    RangedPlacedXR34 receipt{};check(g_ranged_placed_xr34.read(receipt)&&receipt.valid&&receipt.mountedD78,"heavy mounted receipt retains exact mechanism qualification");
    committed.allowed[0]=qt(pose(2,{1,1,1},{1,2,3}));place();
    check(near({{written[0],written[1],written[2],written[3]},{written[4],written[5],written[6]}},mounted,3e-5),"mounted heavy tool is independent of free wrist motion");
}
static void testWorldTransforms(){
    for(float scale:{.55f,1.f,1.65f})for(float axisScale:{.7f,1.f,1.4f})for(double turn:{-.9,.0,1.7}){
        reset();animatedOffset();ready();const auto bw=pose(turn,{2,1,-3},{128,-32,11});const auto edgeGrip=spawnEdgeGrip();
        bodyWorld={{float(bw.q.x),float(bw.q.y),float(bw.q.z),float(bw.q.w)},{float(bw.p.x),float(bw.p.y),float(bw.p.z)},scale};
        natural[7]=.85f;put(object,0xC,axisScale);put(object,0x10,1.3f);put(object,0x14,.9f);
        place();RangedPlacedXR34 receipt{};check(g_ranged_placed_xr34.read(receipt)&&receipt.valid,"placement receipt valid");
        const auto wrist=PhysCurrentWorld164(PhysPose6698(committed.allowed[0]),bodyWorld);
        check(pm::length(receipt.worldGrip-wrist.p)<3e-5,"achieved spawn edge grip position under body and native scale");
        const pm::Quat writtenQ{written[0],written[1],written[2],written[3]};
        check(pm::angle(pm::multiply(writtenQ,edgeGrip.q),wrist.q)<2e-5,"achieved spawn edge grip orientation under body rotation");
        for(int axis=0;axis<3;++axis){float nativeScale=0;Read(object.data()+0xC+axis*4,nativeScale);
            check(std::abs(pm::length({achieved.m[axis],achieved.m[axis+4],achieved.m[axis+8]})-nativeScale*natural[7])<2e-6,"native axis scale retained");}
        // A new ordinary committed body can transport this exact calibrated
        // item while an auxiliary reload target is absent; no new credit.
        now+=81;committed.reloadTargetXR34={};committed.reloadSourceXR34.allowed=false;
        committed.allowed[0]=qt(pose(.35,{1,3,2},{.24,.02,1.2}));place();
        check(writes==2,"ordinary body wrist hold still writes same native item");
        const auto heldWorld=PhysCurrentWorld164(PhysPose6698(committed.allowed[0]),bodyWorld);
        const ph::Pose held{{written[0],written[1],written[2],written[3]},{written[4],written[5],written[6]}};
        auto scaledGrip=edgeGrip;scaledGrip.p=scaledGrip.p*scale;
        check(near(ph::compose(held,scaledGrip),heldWorld,3e-5),"gap hold keeps spawn edge grip at current hand");
        check(g_ranged_placed_xr34.read(receipt)&&!receipt.valid,"gap hold cannot create event or placement credit");
    }
}
static void testUnchangedBolts(){
    for(int classId:{10,14,15}){reset(classId,true);animatedOffset();build(false);
        check(committed.reloadTargetXR34.valid,"bolt does not acquire new spanner reference requirement");
        const auto native=AbsoluteFromRelative(original[toolSlot],socketFixture.relative);
        const auto expected=RelativeFromAbsolute(native,committed.native[0]);
        check(near(PhysPose6698(committed.reloadTargetXR34.grip),PhysPose6698(expected)),"bolt orientation path unchanged");
        place();check(writes==1&&heavyCalls==0,"every class bolt keeps hand attachment writer");
    }
}
int main(int argc,char** argv){
    const bool retained=argc>1&&!std::strcmp(argv[1],"--retained-only");
    if(!retained)testSpawnEdgeGrip();
    testTimingAndIdentity();testUnchangedBolts();testHeavyMountedPath();
    if(!retained){testReferenceValidity();testWorldTransforms();}
    std::printf("PASS_R108 checks=%u mode=%s\n",checks,retained?"retained_timing":"spanner_spawn_edge_and_placement");
}
