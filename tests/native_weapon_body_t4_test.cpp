#define KCD2VR_WEAPON_BODY_T4_HOST_TEST
#include "../src/native_weapon_body_t4.cpp"
#include "../src/native_weapon_clearance_t4.cpp"
#include <cassert>
#include <cstdio>
#include <new>
#include <string>

namespace fixture {
using namespace kcd2vr;
namespace p=native_physics_phase_t4;namespace w=native_weapon_body_t4;
bool phasePreflightOK=true;
std::uint64_t now=1000,serial=1,generation=1;
bool current=true,suspended=true,living=true,predicate=true,pending=true,alive=false,
    geometry=false,collision=false,filters=true,install=true,clearRevokes=false,motionRevokes=false,createDisable=false,
    postEffectRefuse=false,postEffectFault=false;
bool removeOkR96=true,releaseOkR96=true,unregisterOkR96=true;
unsigned creates=0,adds=0,enables=0,removes=0,destroys=0,releases=0,queries=0,velocities=0,unregisters=0;
unsigned livingCalls=0,revokeAt=0,registerCalls=0,phaseInstallCalls=0;
bool registerOK=true,phaseInstallOK=true,clearanceInstallOK=true;
float clearResult=0;double mass=2;w::Pose body{};p::V3 linear{},angular{};
p::Client client{};int bodyCookie=0,geometryCookie=0;
void* bodyPointer(){return &bodyCookie;}void* geometryPointer(){return &geometryCookie;}
unsigned suspendAtSourceH36=0,sourceCallsH36=0;
bool Source(const w::Input&) noexcept {++sourceCallsH36;if(suspendAtSourceH36&&sourceCallsH36==suspendAtSourceH36)w::SetEquipmentAdmissionSuspendedH36(true);return predicate;}
w::Input Input(){w::Input in{};in.owner={1,2,3,4,0x100,0x200,0x300,0x400,0x500,0x600,7,0};
    in.weaponWorld.p={10,20,30};in.targetWorld=in.weaponWorld;in.boundsMin={-.04,-.2,-.03};
    in.boundsMax={.04,.8,.03};in.mass=2;in.scale=1;in.sample=1;in.tick=now;in.trackingEpoch=9;
    in.intentFrameH28.valid=true;in.intentFrameH28.scale=1.25;in.intentFrameH28.modelWorld.p={1,2,3};in.intentFrameH28.trackingFromModel.p={.1,.2,.3};
    in.cancelGeneration=8;in.colliderLease=10;in.classId=1;in.allowed=in.tracked=in.emptyLeft=in.nativeColliderExcluded=true;
    in.nativeBody=0x700;in.nativeProxy=0x800;in.nativeBodyId=11;
    in.playerBody=0x900;in.playerEntityPointer=0xA00;in.playerBodyId=12;return in;}
}
namespace kcd2vr::native_physics_phase_t4 {
struct Access {
    static Context& Ctx(){static Context context;context.world_=reinterpret_cast<void*>(0x10000);
        context.base_=0x180000000;context.generation_=fixture::generation;context.serial_=fixture::serial;
        context.phase_=Phase::Step;return context;}
    static void PhaseOf(Phase phase){Ctx().phase_=phase;}
    static void Set(Lease& l,bool value){l.held_=value;l.pending_=value&&fixture::pending;l.id_=value?42:-1;
        l.body_=value?fixture::bodyPointer():nullptr;l.world_=reinterpret_cast<void*>(0x10000);l.generation_=fixture::generation;}
    static void Pending(Lease& l,bool value){l.pending_=value;}
    static bool Same(const Context& c,const Lease& l){return l.held_&&l.world_==c.world_&&l.generation_==c.generation_;}
};
bool Context::Current()const noexcept{return fixture::current;}
void* Context::World()const noexcept{return world_;}
std::uintptr_t Context::ModuleBase()const noexcept{return base_;}
std::uint64_t Context::Generation()const noexcept{return generation_;}
std::uint64_t Context::Serial()const noexcept{return serial_;}
Phase Context::Kind()const noexcept{return phase_;}
bool RegisterClient(Client c)noexcept{++fixture::registerCalls;if(!fixture::registerOK)return false;fixture::client=c;return true;}
bool Preflight()noexcept{return fixture::phasePreflightOK;}
const char* InstallFailureStage()noexcept{return "TEST";}
bool Install()noexcept{++fixture::phaseInstallCalls;return fixture::phaseInstallOK;}
void RequestShutdown()noexcept{}
Receipt ReadReceipt()noexcept{return {};}
bool AcquireRigid(Context&,int,void*,Lease&)noexcept{return false;}
bool RigidEntityIdentity(Context&,Lease&,const void*,std::uint32_t)noexcept{return false;}
bool AdoptCreatedRigid(Context&,void* b,const void* foreign,int type,Lease& l)noexcept{
    assert(foreign==native_weapon_body_t4::record.foreignCookieH24&&foreign);
    assert(type>=0&&type<=32767);if(b!=fixture::bodyPointer())return false;Access::Set(l,true);return true;}
Registry ObserveRegistry(Context&,const Lease&)noexcept{return fixture::alive?Registry::Same:Registry::Retired;}
bool ReadyRigid(Context&,Lease& l)noexcept {Access::Pending(l,fixture::pending);return l.Held()&&fixture::alive&&!fixture::pending;}
bool Release(Context& c,Lease& l)noexcept {++fixture::releases;if(!Access::Same(c,l)||!fixture::releaseOkR96)return false;Access::Set(l,false);return true;}
bool AbandonInvalidated(Context&,Lease& l)noexcept {Access::Set(l,false);return true;}
bool ReadRigid(Context&,Lease&,RigidSnapshot& out)noexcept{
    if(!fixture::alive||fixture::pending||!fixture::geometry)return false;
    out.position={float(fixture::body.p.x),float(fixture::body.p.y),float(fixture::body.p.z)};
    out.orientation={float(fixture::body.q.x),float(fixture::body.q.y),float(fixture::body.q.z),float(fixture::body.q.w)};
    out.mass=float(fixture::mass);out.scale=1;out.active=true;out.simulationClass=2;
    out.linearVelocity=fixture::linear;out.angularVelocity=fixture::angular;return true;}
bool SetVelocity(Context&,Lease&,V3 v,V3 w)noexcept {
    ++fixture::velocities;fixture::linear=v;fixture::angular=w;return true;}
bool ReadConstraint(Context&,Lease&,int,ConstraintSnapshot&)noexcept{return false;}
bool AcquireLivingH24(Context&,const LivingSourceH24&,LivingLeaseH24&) noexcept{return false;}
bool CurrentLivingH24(Context&,LivingLeaseH24&,const LivingSourceH24&) noexcept{return false;}
bool ReleaseLivingH24(Context&,LivingLeaseH24&) noexcept{return false;}
bool AbandonLivingH24(Context&,LivingLeaseH24&) noexcept{return false;}
ConstraintResult AddSelfIgnorePairH24(Context&,Lease&,LivingLeaseH24&) noexcept{return {};}
ConstraintResult RemoveSelfIgnorePairH24(Context&,Lease&,std::uint32_t) noexcept{return {};}
bool SelfIgnorePairPresentH24(Context&,Lease&,LivingLeaseH24&,std::uint32_t) noexcept{return false;}
ConstraintResult AddConstraint(Context&,Lease&,Lease&,const constraint_packet60::Packet&) noexcept{return {};}
ConstraintResult RemoveConstraint(Context&,Lease&,std::uint32_t) noexcept{return {};}
void* NativeHandle(Context& c,Lease& l,bool allow)noexcept{
    return Access::Same(c,l)&&fixture::alive&&(allow||!fixture::pending)?fixture::bodyPointer():nullptr;}
bool ObserveSuspended(Context&,int id,void* body,const void* entity)noexcept {
    return fixture::suspended&&id==11&&body==reinterpret_cast<void*>(0x700)&&entity==reinterpret_cast<void*>(0x400);}
bool ObserveLivingExcluded(Context&,int id,void* body,const void* entity)noexcept {
    ++fixture::livingCalls;
    if(fixture::motionRevokes&&fixture::livingCalls>=fixture::revokeAt){fixture::predicate=false;fixture::motionRevokes=false;}
    return fixture::living&&id==12&&body==reinterpret_cast<void*>(0x900)&&entity==reinterpret_cast<void*>(0xA00);}
}
namespace kcd2vr::retail_build112 {bool Ready(std::uintptr_t) noexcept{return false;}}
namespace kcd2vr::native_weapon_body_t4 {namespace {
std::uint64_t HostNow()noexcept{return fixture::now;}
bool HostInstall()noexcept{return fixture::install;}
bool HostBind(phase::Context&,void*& m)noexcept{m=reinterpret_cast<void*>(0x20000);return true;}
void* HostCreate(phase::Context&,const Pose& p)noexcept{
    ++fixture::creates;fixture::alive=true;fixture::pending=true;fixture::body=p;
    if(fixture::createDisable){SetEnabled(false);Achieved out{};assert(Read(out)&&!out.safeNativeRestore&&!out.colliderRetired);}
    return fixture::bodyPointer();}
void* HostGeometry(phase::Context&,const policy::Shape&,void*)noexcept{return fixture::geometryPointer();}
bool HostAddGeometry(phase::Context&,void*,void*,const policy::Shape& s,bool c)noexcept{
    ++fixture::adds;if(c)++fixture::enables;fixture::geometry=true;fixture::collision=c;fixture::mass=s.mass;
    if(c&&fixture::postEffectFault){fault.store(true);return false;}
    return !(c&&fixture::postEffectRefuse);}
bool HostRemoveGeometry(phase::Context&,void*)noexcept{++fixture::removes;if(!fixture::removeOkR96)return false;fixture::geometry=false;fixture::collision=false;return true;}
bool HostPart(phase::Context&,void*,void*,bool c,double m)noexcept{return fixture::geometry&&fixture::collision==c&&fixture::mass==m;}
bool HostConfigure(phase::Context&,void*)noexcept{const auto p=ContactFlagsH24();assert(p.type==15&&p.flags==0x80000000u&&p.flagsAND==0xFFFFFFFFu&&ContactFlagsCurrentH24(p.flagsOR));return true;}
bool HostFilters(phase::Context&,void*)noexcept{return fixture::filters;}
bool HostPairClassH5(phase::Context&,void*,unsigned ignore)noexcept{return ignore==4;}
bool HostUnregister(phase::Context&,void*,void*)noexcept{++fixture::unregisters;return fixture::unregisterOkR96;}
int HostDestroy(phase::Context&,void*)noexcept{++fixture::destroys;fixture::alive=false;return 1;}
}}
namespace kcd2vr::native_weapon_clearance_t4 {namespace {
bool HostClearanceInstall()noexcept{return fixture::clearanceInstallOK;}
float HostClearance(native_physics_phase_t4::Context&,Capsule,void*)noexcept {
    ++fixture::queries;if(fixture::clearRevokes)fixture::predicate=false;return fixture::clearResult;}
}}
namespace fixture {
void Tick(){++serial;auto& c=p::Access::Ctx();client.step(c,nullptr);}
void Reset(){auto& c=p::Access::Ctx();p::Access::PhaseOf(p::Phase::WorldReset);client.reset(c,nullptr);alive=false;geometry=false;collision=false;}
void Fresh(w::Input& in){in.tick=now;in.sample++;w::Submit(in);}
void Advance(w::Input& in,unsigned ms=10){now+=ms;Fresh(in);Tick();}
void Cleanup(){w::SetEnabled(false);Tick();pending=false;Tick();Tick();assert(!alive);}
void Restart(){
    Cleanup();predicate=suspended=living=filters=true;clearResult=0;clearRevokes=motionRevokes=createDisable=false;
    postEffectRefuse=postEffectFault=false;pending=true;linear={};angular={};w::SetEnabled(true);
}
void Active(w::Input& in){Fresh(in);Tick();assert(alive&&pending&&!collision);pending=false;
    Advance(in);assert(geometry&&!collision);Advance(in);assert(collision);
    w::Achieved out{};assert(w::Read(out)&&out.poseValid&&out.state==w::State::Active&&!out.safeNativeRestore&&!out.colliderRetired);
    assert(pose_core::length(out.weaponWorld.p-in.weaponWorld.p)<1e-5);
}
}
int main(int argc,char** argv){using namespace fixture;
    // The equipment debt is separate from the user F9 toggle. New working-
    // hand consumers must still see it while physics is OFF.
    assert(!w::EquipmentAdmissionSuspendedH36());
    w::SetEquipmentAdmissionSuspendedH36(true);
    assert(w::EquipmentAdmissionSuspendedH36()&&!w::Enabled());
    w::SetEnabled(false);
    assert(w::EquipmentAdmissionSuspendedH36());
    w::SetEquipmentAdmissionSuspendedH36(false);
    assert(!w::EquipmentAdmissionSuspendedH36());
    if(argc>1){
        const std::string mode=argv[1];
        if(mode=="register-refusal"||mode=="phase-refusal"){
            registerOK=mode!="register-refusal";phaseInstallOK=mode!="phase-refusal";
            assert(!w::Install()&&w::attempted.load());
            assert(registerCalls==1&&phaseInstallCalls==(mode=="phase-refusal"?1u:0u));
            const auto r=registerCalls,p=phaseInstallCalls;
            registerOK=phaseInstallOK=true;assert(!w::Install());
            assert(registerCalls==r&&phaseInstallCalls==p&&creates==0);
        }else if(mode=="fault-refusal"){
            w::fault.store(true);assert(!w::Install());
            assert(registerCalls==0&&phaseInstallCalls==0&&creates==0);
        }else if(mode=="phase-preflight"){
            phasePreflightOK=false;assert(!w::Install()&&!w::attempted.load());
            assert(registerCalls==0&&phaseInstallCalls==0&&creates==0);
            phasePreflightOK=true;assert(w::Install()&&w::attempted.load());
            assert(registerCalls==1&&phaseInstallCalls==1&&creates==0);
        }else if(mode=="clearance-preflight"){
            clearanceInstallOK=false;assert(!w::Install()&&!w::attempted.load());
            assert(registerCalls==0&&phaseInstallCalls==0&&creates==0);
            clearanceInstallOK=true;assert(w::Install()&&w::attempted.load());
            assert(registerCalls==1&&phaseInstallCalls==1&&creates==0);
        }else return 2;
        std::puts("B4 actual Install preflight/mutating boundary PASS");return 0;
    }
    assert(w::SetSourcePredicate(&Source));
    // Failed install never creates native state; exit has an atomic no-body ACK.
    install=false;assert(!w::Install());w::SetEnabled(true);w::SetEnabled(false);
    w::Achieved initial{};assert(w::Read(initial)&&initial.safeNativeRestore);
    assert(!w::attempted.load());assert(!w::Install());assert(!w::attempted.load());
    install=true;assert(w::Install());assert(w::attempted.load());
    assert(client.needsStep&&!client.needsStep(nullptr));
    kcd2vr::collab_h21::setPhysicsMode(3);assert(client.needsStep(nullptr));
    kcd2vr::collab_h21::setPhysicsMode(0);assert(!client.needsStep(nullptr));
    w::SetEnabled(true);assert(client.needsStep(nullptr));w::SetEnabled(false);assert(!client.needsStep(nullptr));
    w::Achieved out{};assert(w::Read(out)&&out.safeNativeRestore);
    auto in=Input();
    const auto s=w::policy::shape(in);assert(s.valid&&s.radius>.05&&s.halfHeight==.5);
    // All eight full native bounds corners lie within the conservative capsule.
    for(int i=0;i<8;++i){const auto p=pose_core::Vec3{i&1?in.boundsMax.x:in.boundsMin.x,i&2?in.boundsMax.y:in.boundsMin.y,i&4?in.boundsMax.z:in.boundsMin.z}-s.bodyLocal.p;
        const double along=std::clamp(pose_core::dot(p,s.axis),-s.halfHeight,s.halfHeight);
        assert(pose_core::length(p-s.axis*along)<=s.radius+1e-8);}
    const auto drive=w::policy::motor(in,s,w::policy::bodyTarget(in,s),{1,0,0},{},2,.01);
    assert(drive.valid&&pose_core::length(drive.linear-pose_core::Vec3{1,0,0})<=.300001);
    // H36 real backend admission barrier: user ON is unchanged, but atomic
    // suspension cannot race a stale source into an owned native Create.
    w::SetEnabled(true);const auto beforeSuspendH36=creates;
    w::SetEquipmentAdmissionSuspendedH36(true);Fresh(in);Tick();
    assert(w::Enabled()&&!w::HasUnretiredCollider()&&creates==beforeSuspendH36);
    w::SetEnabled(false);w::SetEnabled(true);Fresh(in);Tick();
    assert(w::Enabled()&&creates==beforeSuspendH36); // OFF/ON not cleanup
    w::SetEquipmentAdmissionSuspendedH36(false);Cleanup();
    Restart();in=Input();sourceCallsH36=0;suspendAtSourceH36=4;Fresh(in);Tick();
    assert(sourceCallsH36==4&&creates==beforeSuspendH36&&!w::HasUnretiredCollider());
    suspendAtSourceH36=0;w::SetEquipmentAdmissionSuspendedH36(false);Cleanup();
    Restart();in=Input();Active(in);const auto admittedCreatesH36=creates;
    w::SetEquipmentAdmissionSuspendedH36(true);
    assert(w::Enabled()&&w::HasUnretiredCollider()); // cannot mistake existing body for absence
    Tick();pending=false;Tick();Tick();assert(!alive&&!w::HasUnretiredCollider());
    Fresh(in);Tick();assert(creates==admittedCreatesH36); // stays blocked after exact retirement
    w::SetEquipmentAdmissionSuspendedH36(false);Cleanup();
    std::puts("H36 actual backend atomic admission suspension and real retirement PASS");
    const auto admissionBefore=w::AdmissionSerial();
    w::SetEnabled(true);Active(in);assert(w::AdmissionSerial()==admissionBefore+1);
    w::module=p::Access::Ctx().ModuleBase();
    w::ContactOwnerH24 collisionOwner{};assert(w::ReadContactOwnerH24(collisionOwner));
    assert(collisionOwner.active&&collisionOwner.body==reinterpret_cast<std::uintptr_t>(bodyPointer())&&
        collisionOwner.foreign==reinterpret_cast<std::uintptr_t>(w::record.foreignCookieH24)&&collisionOwner.foreignType==0x5634&&
        collisionOwner.bodyId==42&&collisionOwner.owner==in.owner&&collisionOwner.sourceSample==in.sample&&
        collisionOwner.worldGeneration==generation&&collisionOwner.bodyGeneration==w::record.incarnation);
    assert(!w::ContactFlagsCurrentH24(w::ContactFlagsH24().flagsOR&~w::kLogCollisionsH24));
    const auto bodyGenPin=w::record.incarnation;const auto ownerPin=in.owner;
    auto foreignPin=ownerPin;++foreignPin.key;
    assert(!w::CurrentColliderLease(p::Access::Ctx(),foreignPin,bodyGenPin));
    assert(!w::PinColliderConstraint(p::Access::Ctx(),ownerPin,bodyGenPin+1,true));
    assert(w::PinColliderConstraint(p::Access::Ctx(),ownerPin,bodyGenPin,true));
    const auto destroyedPin=destroys;w::Cancel();assert(!w::ReadContactOwnerH24(collisionOwner));Tick();
    assert(destroys==destroyedPin&&w::CleanupColliderLease(p::Access::Ctx(),ownerPin,bodyGenPin));
    assert(!w::CurrentColliderLease(p::Access::Ctx(),ownerPin,bodyGenPin));
    assert(w::PinColliderConstraint(p::Access::Ctx(),ownerPin,bodyGenPin,false));
    Fresh(in); // this isolated buddy test revokes no native feature preference
    // The native solver's achieved movement, not target, drives visual output.
    body.p.x+=.1;Advance(in);assert(w::ReadContactOwnerH24(collisionOwner)&&collisionOwner.sourceSample==in.sample);assert(w::Read(out)&&out.poseValid&&std::abs(out.weaponWorld.p.x-in.weaponWorld.p.x-.1)<1e-5);
        assert(out.intentFrameH28.valid&&out.intentFrameH28.scale==in.intentFrameH28.scale&&
        out.intentFrameH28.modelWorld.p.x==in.intentFrameH28.modelWorld.p.x&&
        out.intentFrameH28.trackingFromModel.p.y==in.intentFrameH28.trackingFromModel.p.y&&
        collisionOwner.intentFrameH28.modelWorld.p.z==in.intentFrameH28.modelWorld.p.z);
    assert(velocities==1);Cleanup();assert(w::Read(out)&&out.safeNativeRestore&&out.colliderRetired);
    // Empty/busy receipt is not retirement when native admission already owns
    // resources, including immediately after the feature is disabled.
    const auto retiredContactOwnerH24=collisionOwner;
    Restart();in=Input();Active(in);assert(w::ReadContactOwnerH24(collisionOwner));
    assert(collisionOwner.body==retiredContactOwnerH24.body&&collisionOwner.bodyId==retiredContactOwnerH24.bodyId&&
        collisionOwner.foreign!=retiredContactOwnerH24.foreign&&collisionOwner.bodyGeneration!=retiredContactOwnerH24.bodyGeneration);
    // Same raw body address/ID after retire cannot match an old queued cookie.
    w::results.~SnapshotMailbox();new(&w::results) core::SnapshotMailbox<w::Achieved>{};
    w::SetEnabled(false);assert(!w::Read(out)&&!out.safeNativeRestore&&!out.colliderRetired);Cleanup();
    // Unsupported/native-enabled/player-filter admission makes no native body.
    unsigned before=creates;Restart();in=Input();in.emptyLeft=false;Fresh(in);Tick();assert(creates==before);
    in=Input();suspended=false;Fresh(in);Tick();assert(creates==before);
    suspended=true;living=false;Fresh(in);Tick();assert(creates==before);living=true;
    // Cancel while factory insertion is pending: late insertion cannot resurrect.
    Restart();in=Input();Fresh(in);Tick();assert(pending);w::Cancel();
    const auto destroysBefore=destroys;Tick();assert(alive&&destroys==destroysBefore&&!collision);
    pending=false;Tick();assert(!collision);Tick();assert(!alive);
    // Read of a stale OFF packet during admitted Create+disable stays blocked.
    Restart();in=Input();createDisable=true;Fresh(in);Tick();assert(!w::Enabled());Tick();pending=false;Tick();Tick();
    assert(w::Read(out)&&out.safeNativeRestore);createDisable=false;
    // Refused overlap never enables, and a stationary hand never churns.
    Restart();in=Input();clearResult=1;Fresh(in);Tick();pending=false;Advance(in);Advance(in);Tick();Tick();
    before=creates;for(int i=0;i<100;++i)Advance(in,20);assert(creates==before&&!collision);
    in.targetWorld.p.x+=.11;Advance(in);assert(creates==before+1);Cleanup();
    // Exact source revocation during full-volume query forbids native enable.
    Restart();in=Input();clearRevokes=true;Fresh(in);Tick();pending=false;Advance(in);
    const auto enabledBefore=enables;Advance(in);assert(enables==enabledBefore&&!collision);Tick();clearRevokes=false;
    // Grip revocation before a motion leaf retires; no stale achieved packet.
    Restart();in=Input();Active(in);const auto velocityBefore=velocities;
    predicate=false;Advance(in);assert(velocities==velocityBefore);
    // Existing result remains readable, but pose authority is revoked immediately.
    assert(w::Read(out)&&!out.poseValid);assert(!w::ReadContactOwnerH24(collisionOwner));Cleanup();
    // Revoke on the final source observation immediately before SetVelocity.
    Restart();in=Input();Active(in);motionRevokes=true;revokeAt=livingCalls+3;
    const auto beforeMotion=velocities;Advance(in);assert(velocities==beforeMotion&&!collision);Cleanup();
    // Reset between Destroy's immediate registry retirement and next observation
    // releases the retained ref without requiring a now-invalid NativeHandle.
    Restart();in=Input();Active(in);w::Cancel();Tick();assert(!alive);
    const auto releasedBefore=releases;Reset();assert(releases==releasedBefore+1);w::SetEnabled(false);
    assert(w::Read(out)&&out.safeNativeRestore);
    // Native Add can apply its solid part before returning refusal. Exact owned
    // part retirement is still attempted; no false ACTIVE/retired receipt.
    Restart();in=Input();postEffectRefuse=true;Fresh(in);Tick();pending=false;Advance(in);
    Advance(in);assert(!collision&&!alive);Tick();w::SetEnabled(false);
    assert(w::Read(out)&&out.safeNativeRestore);postEffectRefuse=false;
    // A post-effect native fault latches and retains the unknown native state.
    Restart();in=Input();postEffectFault=true;Fresh(in);Tick();pending=false;Advance(in);Advance(in);
    w::SetEnabled(false);assert(w::Read(out)&&!out.safeNativeRestore&&!out.colliderRetired&&w::HasUnretiredCollider());
    const auto registeredAtFault=registerCalls,phaseAtFault=phaseInstallCalls;
    assert(!w::Install()&&registerCalls==registeredAtFault&&phaseInstallCalls==phaseAtFault);
    // Default volume classifier never calls an unknown result empty space.
    using r=native_weapon_clearance_t4::Result;
    assert(native_weapon_clearance_t4::Classify(-1)==r::Unknown);
    assert(native_weapon_clearance_t4::Classify(std::nanf(""))==r::Unknown);
    const auto beforeCookieExhaustionH24=w::incarnations;w::incarnations=w::cookiePoolH24.size();
    assert(!w::ReserveCookieH24()&&w::incarnations==w::cookiePoolH24.size());w::incarnations=beforeCookieExhaustionH24;
    assert(std::string(w::ReasonName(w::Reason::ContactIdentityExhaustedH24))=="CONTACT_IDENTITY_EXHAUSTED");
    std::puts("T4 actual native weapon backend lifecycle/clearance/authority/finite motor PASS");
    return 0;
}
