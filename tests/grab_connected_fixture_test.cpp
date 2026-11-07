// DS-F69: the CONNECTED proxy fixture - the real ServiceStepCore path driven
// end-to-end through an injectable MOCK seam (no phase::Context). This is the
// fixture Codex asked for: acquire -> create -> adopt -> ready -> measured
// drive -> retire -> destroy -> registry-retired -> release -> off, plus the
// edge cases (loss before creation, invalid input in Creating, read failure,
// world replacement, uncertain destroy).
#include "subsystems/physics/runtime/native_hand_proxy_owner60.h"
#include "native_physics_phase_t4.h"
#include "vrik/phys_bodies59c10r8.h"
#include "retail_build112.h"
#include "subsystems/physics/runtime/native_weapon_clearance_t4.h"
#include "subsystems/physics/runtime/native_hand_presence_pose_h24.h"
#include "subsystems/physics/runtime/dialogue_hand_presence_h24.h"
#include <windows.h>
#include <atomic>
#include <cstring>
#include <cmath>
#include <cstdio>

// --- stubs for the phase:: API (never invoked in test mode) ---
namespace kcd2vr::native_physics_phase_t4 {
bool Context::Current() const noexcept{return true;}
void* Context::World() const noexcept{return nullptr;}
std::uintptr_t Context::ModuleBase() const noexcept{return base_;}
std::uint64_t Context::Generation() const noexcept{return generation_;}
std::uint64_t Context::Serial() const noexcept{return serial_;}
Phase Context::Kind() const noexcept{return phase_;}
bool Preflight() noexcept{return false;}
const char* InstallFailureStage() noexcept{return "";}
bool RegisterClient(Client) noexcept{return false;}
bool Install() noexcept{return false;}
void RequestShutdown() noexcept{}
Receipt ReadReceipt() noexcept{return Receipt{};}
bool AcquireRigid(Context&,int,void*,Lease&) noexcept{return false;}
bool AdoptCreatedRigid(Context&,void*,const void*,int,Lease&) noexcept{return false;}
Registry ObserveRegistry(Context&,const Lease&) noexcept{return Registry::Unknown;}
bool ReadyRigid(Context&,Lease&) noexcept{return false;}
bool Release(Context&,Lease&) noexcept{return false;}
bool AbandonInvalidated(Context&,Lease&) noexcept{return false;}
bool ReadRigid(Context&,Lease&,RigidSnapshot&) noexcept{return false;}
bool SetVelocity(Context&,Lease&,V3,V3) noexcept{return false;}
bool ReadConstraint(Context&,Lease&,int,ConstraintSnapshot&) noexcept{return false;}
bool ObserveSuspended(Context&,int,void*,const void*) noexcept{return false;}
bool ObserveLivingExcluded(Context&,int,void*,const void*) noexcept{return false;}
bool AcquireLivingH24(Context&,const LivingSourceH24&,LivingLeaseH24&) noexcept{return false;}
bool CurrentLivingH24(Context&,LivingLeaseH24&,const LivingSourceH24&) noexcept{return false;}
bool ReleaseLivingH24(Context&,LivingLeaseH24&) noexcept{return false;}
bool AbandonLivingH24(Context&,LivingLeaseH24&) noexcept{return false;}
ConstraintResult AddSelfIgnorePairH24(Context&,Lease&,LivingLeaseH24&) noexcept{return {};}
ConstraintResult RemoveSelfIgnorePairH24(Context&,Lease&,std::uint32_t) noexcept{return {};}
bool SelfIgnorePairPresentH24(Context&,Lease&,LivingLeaseH24&,std::uint32_t) noexcept{return false;}

void* NativeHandle(Context&,Lease&,bool) noexcept{return nullptr;}
}
namespace kcd2vr::native_weapon_clearance_t4 {bool Install() noexcept{return false;}Result Query(native_physics_phase_t4::Context&,Capsule,void*) noexcept{return Result::Unknown;}}
namespace kcd2vr::retail_build112 {bool Ready(std::uintptr_t) noexcept{return true;}} // DS-F69: stub
#include "subsystems/physics/runtime/native_hand_proxy_owner60.inl"

namespace hp=kcd2vr::physics::hand_proxy60;
namespace hpt4=kcd2vr::native_hand_proxy_t4;
namespace kcd2vr::native_physics_phase_t4 {
struct Access {static void resetFixture(Phase kind,std::uint64_t generation){Context c;c.phase_=kind;c.generation_=generation;kcd2vr::physics::hand_proxy60::ResetProxy(c,nullptr);}};
}
static int fails=0,checks=0;
#define CK(c) do{++checks;if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)

// --- the mock seam ---
struct Mock {
    bool createCalled=false,adoptCalled=false,readyCalled=false,readOk=true,readFailed=false;
    int registryState=0; // 1=Same,2=Retired
    int destroyResult=1;bool destroyCalled=false,releaseCalled=false,abandonCalled=false;
    int velocityCalls=0,presenceConfig=0,creates=0;bool presence=true,configOk=true;float mass=1.f,readOffset=0;
    float lastVx=0,lastVy=0,lastVz=0;
    int geometryAdds=0,geometryRemoves=0;
    bool geometryOk=true,removeOk=true,releaseOk=true,velocityOk=true,awake=true,readyOk=true;
    hpt4::Pose lastCreatePose{};
    void reset(){*this=Mock{};}
};
static Mock g_mock;
static void* MockCreate(int side,const hpt4::Pose& pose,std::uint64_t) noexcept{
    g_mock.createCalled=true;++g_mock.creates;g_mock.lastCreatePose=pose;return reinterpret_cast<void*>(std::uintptr_t(0x1000+side));
}
static bool MockAdopt(int,void*) noexcept{g_mock.adoptCalled=true;return true;}
static bool MockReady(int) noexcept{g_mock.readyCalled=true;return g_mock.readyOk;}
static bool MockRead(int,hp::phase_rigid_snapshot& out) noexcept{
    if(!g_mock.readOk){g_mock.readFailed=true;return false;}
    out.posX=g_mock.lastCreatePose.p.x+g_mock.readOffset;out.posY=g_mock.lastCreatePose.p.y;out.posZ=g_mock.lastCreatePose.p.z;
    out.mass=g_mock.mass;out.active=g_mock.awake;return true;
}
static bool MockVelocity(int,float vx,float vy,float vz,float,float,float) noexcept{g_mock.velocityCalls++;g_mock.lastVx=vx;g_mock.lastVy=vy;g_mock.lastVz=vz;return g_mock.velocityOk;}
static int MockRegistry(int) noexcept{return g_mock.registryState;}
static bool MockRelease(int) noexcept{g_mock.releaseCalled=true;return g_mock.releaseOk;}
static int MockDestroy(int,void*) noexcept{g_mock.destroyCalled=true;return g_mock.destroyResult;}
static bool MockAbandon(int) noexcept{g_mock.abandonCalled=true;return true;}
static void* MockMakeGeometry(int) noexcept{return reinterpret_cast<void*>(std::uintptr_t(0x2000));} // DS-F77
static bool MockAddGeometry(int,void*) noexcept{++g_mock.geometryAdds;return g_mock.geometryOk;}
static bool MockRemoveGeometry(int,void*) noexcept{++g_mock.geometryRemoves;return g_mock.removeOk;} // DS-F84
namespace presence=kcd2vr::physics::hand_presence_h24;
namespace dialogueHand=kcd2vr::physics::dialogue_hand_h24;
static bool g_dialoguePolicy=false;
static std::uint64_t g_dialogueNow=100;
static presence::Context g_dialogueContext{};
static dialogueHand::Witness g_dialogueWitness{};
static bool MockPresence(int,const hpt4::Input& in) noexcept{
    if(!g_dialoguePolicy)return g_mock.presence;
    auto c=g_dialogueContext;c.dialoguePresence=g_mock.presence&&
        dialogueHand::current(g_dialogueWitness,in,c.cancel,g_dialogueNow);
    return presence::current(in,g_dialogueNow,c);
}
static bool MockConfigure(int) noexcept{++g_mock.presenceConfig;return g_mock.configOk;}
static int pairPrepare=0,pairRetire=0,pairPreparations=0,pairRetirements=0;
static int MockPairPrepare(int) noexcept{++pairPreparations;return pairPrepare;}
static int MockPairRetire(int) noexcept{++pairRetirements;return pairRetire;}
static hp::Seam mockSeam(){hp::Seam s{};s.create=&MockCreate;s.adopt=&MockAdopt;s.ready=&MockReady;s.readRigid=&MockRead;
    s.setVelocity=&MockVelocity;s.observeRegistry=&MockRegistry;s.release=&MockRelease;s.destroy=&MockDestroy;s.abandon=&MockAbandon;
    s.presenceSource=&MockPresence;s.configurePresence=&MockConfigure;
    s.makeGeometry=&MockMakeGeometry;s.addGeometry=&MockAddGeometry;s.removeGeometry=&MockRemoveGeometry;return s;}

static hpt4::Input in(std::uint64_t tick,std::uint64_t poseTick=0){hpt4::Input i{};i.tick=tick;
    i.source={1,1,1,poseTick?poseTick:tick,0xAAA,0};i.target.p={0.5f,0,0};i.target.q={0,0,0,1};return i;}

int main(){
    {namespace pa=kcd2vr::physics::self_pair_admission_h24;pa::Causality causal{};causal.submittedTick=100;
     CK(causal.observe(true,false,false)==pa::Observation::AwaitPresent); // revoked before queued insertion
     CK(causal.observe(false,false,false)==pa::Observation::Unknown);
     CK(!causal.timedOut(1100)&&causal.timedOut(1101));
     // Timed-out preparation still services the original id: late Present then
     // Remove-pending remains Present; only later original-id Absent can release.
     CK(causal.observe(true,true,true)==pa::Observation::Present&&causal.presentSeen);
     CK(causal.observe(true,true,true)==pa::Observation::Present);
     CK(causal.observe(false,false,false)==pa::Observation::Unknown);
     CK(causal.observe(true,false,false)==pa::Observation::Absent);
     CK(causal.observe(true,true,false)==pa::Observation::WrongPair);
    }

    // 1) full lifecycle: create -> adopt -> ready -> active(measured) -> retire -> destroy -> retired -> release -> off
    {hp::InstallForTest(mockSeam());g_mock.reset();
     hp::SubmitHand(0,in(100));
     hp::ServiceStepFixture(0,7,1,100); // Requested -> create+adopt
     CK(g_mock.createCalled&&g_mock.adoptCalled);
     hp::ServiceStepFixture(0,7,2,110); // Creating -> ready (serial advanced)
     CK(g_mock.readyCalled);
     hp::ServiceStepFixture(0,7,3,120); // Active -> measured drive
     hpt4::Achieved a{};hp::ReadAchieved(0,a);
     CK(a.valid&&g_mock.velocityCalls>=1);
     hp::Retire(0);
     hp::ServiceStepFixture(0,7,4,130); // Retiring -> destroy (pending)
     CK(g_mock.destroyCalled);
     g_mock.registryState=2; // the native destroy completed
     hp::ServiceStepFixture(0,7,5,140); // registry Retired -> release
     CK(g_mock.releaseCalled);
     hp::ServiceStepFixture(0,7,6,150); // the retire receipt is consumed -> Off
     hp::ReadAchieved(0,a);CK(a.retired&&a.state==hpt4::State::Off);}
    // D41: withdrawing a retained grab target is not a target at the origin,
    // a zero-velocity command, or permission to destroy its constrained buddy.
    // This executes the actual ServiceStepCore + ProxyPolicy retire path.
    {hp::InstallForTest(mockSeam());g_mock.reset();
     hp::SubmitHand(0,in(100));hp::ServiceStepFixture(0,7,1,100);
     hp::ServiceStepFixture(0,7,2,110);hp::ServiceStepFixture(0,7,3,120);
     hpt4::Achieved a{};hp::ReadAchieved(0,a);CK(a.valid);
     const int driven=g_mock.velocityCalls;hp::g_proxy[0].constraintPinned=true;
     hp::SubmitHand(0,{});hp::ServiceStepFixture(0,7,4,130);
     hp::ReadAchieved(0,a);CK(!a.valid&&a.state==hpt4::State::Retiring);
     CK(g_mock.velocityCalls==driven&&!g_mock.destroyCalled&&!g_mock.releaseCalled);
     hp::ServiceStepFixture(0,7,5,140);
     CK(g_mock.velocityCalls==driven&&!g_mock.destroyCalled&&!g_mock.releaseCalled);
     hp::g_proxy[0].constraintPinned=false; // exact native constraint Absent
     hp::ServiceStepFixture(0,7,6,150);CK(g_mock.destroyCalled);
     g_mock.registryState=2;hp::ServiceStepFixture(0,7,7,160);
     hp::ServiceStepFixture(0,7,8,170);hp::ReadAchieved(0,a);
     CK(g_mock.releaseCalled&&a.retired&&a.state==hpt4::State::Off);
     CK(g_mock.velocityCalls==driven);}
    // 2) tracking loss BEFORE creation: no create happens
    {hp::InstallForTest(mockSeam());g_mock.reset();
     hp::SubmitHand(0,{}); // invalid
     hp::ServiceStepFixture(0,7,1,100);
     CK(!g_mock.createCalled);}
    // 3) read failure in Active: validity stays false
    {hp::InstallForTest(mockSeam());g_mock.reset();
     hp::SubmitHand(0,in(100));
     hp::ServiceStepFixture(0,7,1,100); // create+adopt
     hp::ServiceStepFixture(0,7,2,110); // ready
     g_mock.readOk=false;
     hp::ServiceStepFixture(0,7,3,120); // Active but read fails
     hpt4::Achieved a{};hp::ReadAchieved(0,a);
     CK(!a.valid&&g_mock.readFailed);}
    // 4) world replacement during retire: abandon, never a forged release
    {hp::InstallForTest(mockSeam());g_mock.reset();
     hp::SubmitHand(0,in(100));
     hp::ServiceStepFixture(0,7,1,100);
     hp::ServiceStepFixture(0,7,2,110);
     hp::Retire(0);
     hp::ServiceStepFixture(0,99,3,120); // generation changed
     CK(g_mock.abandonCalled&&!g_mock.releaseCalled);}
    // 5) uncertain Destroy: faulted, no retired ACK
    {hp::InstallForTest(mockSeam());g_mock.reset();
     hp::SubmitHand(0,in(100));
     hp::ServiceStepFixture(0,7,1,100);
     hp::ServiceStepFixture(0,7,2,110);
     hp::Retire(0);
     g_mock.destroyResult=-999;
     hp::ServiceStepFixture(0,7,3,120);
     hpt4::Achieved a{};hp::ReadAchieved(0,a);
     CK(!a.retired);}
    // No geometry creation/attach before a later ready serial; failed add is never retried blindly.
    {hp::InstallForTest(mockSeam());g_mock.reset();hp::SubmitHand(0,in(100));hp::ServiceStepFixture(0,7,1,100);
     hp::ServiceStepFixture(0,7,1,105);CK(g_mock.geometryAdds==0);
     g_mock.geometryOk=false;hp::ServiceStepFixture(0,7,2,110);hp::ServiceStepFixture(0,7,3,120);hp::ServiceStepFixture(0,7,4,130);
     hpt4::Achieved a{};hp::ReadAchieved(0,a);CK(g_mock.geometryAdds==1&&!a.valid&&!a.retired&&a.state==hpt4::State::Quarantined);}
    // Sleeping proxy can receive its first motor command; failed command cannot become achieved proof.
    {hp::InstallForTest(mockSeam());g_mock.reset();g_mock.awake=false;hp::SubmitHand(0,in(100));
     hp::ServiceStepFixture(0,7,1,100);hp::ServiceStepFixture(0,7,2,110);hp::ServiceStepFixture(0,7,3,120);
     hpt4::Achieved a{};hp::ReadAchieved(0,a);CK(a.valid&&g_mock.velocityCalls==1);
     g_mock.velocityOk=false;hp::ServiceStepFixture(0,7,4,130);hp::ReadAchieved(0,a);CK(!a.valid);}
    // Failed remove/unregister never proceeds to destroy or forges retirement.
    {hp::InstallForTest(mockSeam());g_mock.reset();hp::SubmitHand(0,in(100));
     hp::ServiceStepFixture(0,7,1,100);hp::ServiceStepFixture(0,7,2,110);hp::ServiceStepFixture(0,7,3,120);hp::Retire(0);
     g_mock.removeOk=false;hp::ServiceStepFixture(0,7,4,130);hpt4::Achieved a{};hp::ReadAchieved(0,a);
     CK(!a.retired&&!g_mock.destroyCalled&&hp::g_proxy[0].geometry!=nullptr&&hp::g_proxy[0].bodyHandle!=0);}
    // Real reset core: failures preserve descriptors/references; a true old-world reset may clear them.
    {hp::InstallForTest(mockSeam());g_mock.reset();hp::SubmitHand(0,in(100));
     hp::ServiceStepFixture(0,7,1,100);hp::ServiceStepFixture(0,7,2,110);hp::ServiceStepFixture(0,7,3,120);
     g_mock.removeOk=g_mock.releaseOk=false;kcd2vr::native_physics_phase_t4::Access::resetFixture(kcd2vr::native_physics_phase_t4::Phase::WorldReset,7);
     hpt4::Achieved a{};hp::ReadAchieved(0,a);CK(!a.retired&&a.state==hpt4::State::Quarantined&&hp::g_proxy[0].bodyHandle!=0&&hp::g_proxy[0].geometry!=nullptr);
     g_mock.removeOk=g_mock.releaseOk=true;kcd2vr::native_physics_phase_t4::Access::resetFixture(kcd2vr::native_physics_phase_t4::Phase::WorldReset,7);
     hp::ReadAchieved(0,a);CK(a.retired&&a.state==hpt4::State::Off&&hp::g_proxy[0].bodyHandle==0);}
    // A submitted constraint pins its buddy through input loss. It stops
    // publishing an active hand, but never destroys a still-referenced body.
    {hp::InstallForTest(mockSeam());g_mock.reset();hp::SubmitHand(0,in(100));
     hp::ServiceStepFixture(0,7,1,100);hp::ServiceStepFixture(0,7,2,110);hp::ServiceStepFixture(0,7,3,120);
     hp::g_proxy[0].constraintPinned=true;hp::ServiceStepFixture(0,7,4,300);
     hpt4::Achieved a{};hp::ReadAchieved(0,a);CK(!a.valid&&!g_mock.destroyCalled&&!g_mock.releaseCalled);
     hp::g_proxy[0].constraintPinned=false;hp::ServiceStepFixture(0,7,5,310);CK(g_mock.destroyCalled);
     g_mock.registryState=2;hp::ServiceStepFixture(0,7,6,320);CK(g_mock.releaseCalled);}
    // Replacement and shutdown are NOT proof of native cleanup.
    {hp::InstallForTest(mockSeam());g_mock.reset();hp::SubmitHand(0,in(100));
     hp::ServiceStepFixture(0,7,1,100);hp::ServiceStepFixture(0,7,2,110);hp::ServiceStepFixture(0,7,3,120);
     kcd2vr::native_physics_phase_t4::Access::resetFixture(kcd2vr::native_physics_phase_t4::Phase::WorldReplaced,8);
     hpt4::Achieved a{};hp::ReadAchieved(0,a);CK(!a.retired&&g_mock.abandonCalled&&!g_mock.releaseCalled);}
    {hp::InstallForTest(mockSeam());g_mock.reset();hp::SubmitHand(0,in(100));
     hp::ServiceStepFixture(0,7,1,100);hp::ServiceStepFixture(0,7,2,110);hp::ServiceStepFixture(0,7,3,120);
     kcd2vr::native_physics_phase_t4::Access::resetFixture(kcd2vr::native_physics_phase_t4::Phase::Shutdown,7);
     hpt4::Achieved a{};hp::ReadAchieved(0,a);CK(!a.retired&&!g_mock.releaseCalled&&!g_mock.destroyCalled);}
    // Production queue -> normal shared phase core -> measured feedback ->
    // allowed wrist. No diagnostic rung / SetEnabled dependency in this path.
    {hp::InstallForTest(mockSeam());g_mock.reset();g_mock.mass=.4f;g_mock.readOffset=-.08f;
     auto raw=in(100);raw.source.physicalPresenceH24=true;raw.source.characterH24=0x222;
     raw.playerBodyH24=0x333;raw.playerEntityH24=0x444;raw.playerBodyIdH24=7;raw.emptyBothHandsH24=true;
     CK(hp::SubmitPresenceH24(0,raw));hp::ServiceStepFixture(0,7,1,100);
     hp::ServiceStepFixture(0,7,2,110);hp::ServiceStepFixture(0,7,3,120);
     hp::PresenceFeedbackH24 measured{};CK(hp::ReadPresenceFeedbackH24(0,measured));
     CK(measured.valid&&g_mock.presenceConfig==1&&g_mock.geometryAdds==1&&g_mock.creates==1);
     CK(std::abs(measured.pose.p.x-.42)<1e-5&&g_mock.lastVx>0); // measured, not target (.5)
     hpt4::Pose local{};namespace bridge=kcd2vr::physics::hand_presence_h24;
     CK(bridge::allowed(raw,measured,120,{},1,{},1,local)&&std::abs(local.p.x-.42)<1e-5);
     bridge::Context context{};context.calibrated=context.gameplay=context.tracked=true;
     context.epoch=raw.source.sourceGeneration;context.cancel=raw.source.cancelGeneration=2;
     context.trackingEpoch=raw.source.trackingEpoch;context.character=raw.source.characterH24;context.entity=raw.playerEntityH24;
     // NativeReference is F3=1; normal F3=0 is not a capture authorization.
     CK(context.enabled&&!context.nativeReference&&bridge::current(raw,120,context));
     context.menu=true;CK(!bridge::current(raw,120,context));context.menu=false;
     context.tracked=false;CK(!bridge::current(raw,120,context));context.tracked=true;
     context.grabBusy=true;CK(!bridge::current(raw,120,context));context.grabBusy=false;
     context.nativeUseUntil=130;CK(!bridge::current(raw,120,context));context.nativeUseUntil=0;
     context.enabled=false;CK(!bridge::current(raw,120,context));context.enabled=true;
     context.nativeReference=true;CK(!bridge::current(raw,120,context));context.nativeReference=false;
     // restore original source identity for subsequent measured-join checks.
     raw.source.cancelGeneration=1;

     auto corrupt=measured;corrupt.pose.q.w=2;CK(!bridge::allowed(raw,corrupt,120,{},1,{},1,local));
     const double half=std::sqrt(.5);hpt4::Pose measuredTurn{{0,0,half,half},{.42,0,0}};
     const auto ownedTurn=bridge::wristRotation(true,measuredTurn,{});
     const auto rawTurn=bridge::wristRotation(false,measuredTurn,{});
     CK(std::abs(ownedTurn.z-half)<1e-6&&std::abs(ownedTurn.w-half)<1e-6);
     CK(rawTurn.z==0&&rawTurn.w==1); // actual existing arm rotation selector uses this production helper
     hpt4::Pose model{{0,0,half,half},{10,20,30}};
     auto movedRaw=raw;auto movedMeasured=measured;
     movedRaw.target={model.q,model.p+kcd2vr::pose_core::rotate(model.q,raw.target.p*2)};
     movedMeasured.pose={model.q,model.p+kcd2vr::pose_core::rotate(model.q,measured.pose.p*2)};
     CK(bridge::allowed(movedRaw,movedMeasured,120,model,2,{},1,local)&&std::abs(local.p.x-.42)<1e-5&&std::abs(local.p.y)<1e-5);
     auto wrong=raw;wrong.source.characterH24++;CK(!bridge::allowed(wrong,measured,120,{},1,{},1,local));
     wrong=raw;wrong.source.sourceGeneration++;CK(!bridge::allowed(wrong,measured,120,{},1,{},1,local));
     wrong=raw;wrong.source.trackingEpoch++;CK(!bridge::allowed(wrong,measured,120,{},1,{},1,local));
     CK(!bridge::allowed(raw,measured,201,{},1,{},1,local));
     CK(!bridge::allowed(raw,measured,120,{},1,{},.1,local));
     // Native interaction takes ownership: the existing body must retire
     // before a collision-off grasp buddy can be created.
     hp::SubmitHand(0,in(130));hp::ServiceStepFixture(0,7,4,130);
     CK(g_mock.destroyCalled&&g_mock.creates==1);hp::ReadPresenceFeedbackH24(0,measured);CK(!measured.valid);
     g_mock.registryState=2;hp::ServiceStepFixture(0,7,5,140);hp::ServiceStepFixture(0,7,6,150);
     hp::ServiceStepFixture(0,7,7,160);CK(g_mock.creates==2&&g_mock.presenceConfig==1);
    }
    // Menu/tracking/context predicate loss stops the motor and retires now;
    // failed collision configuration never produces an achieved hand.
    for(bool failedConfig:{false,true}){hp::InstallForTest(mockSeam());g_mock.reset();g_mock.mass=.4f;
     auto raw=in(100);raw.source.physicalPresenceH24=true;raw.source.characterH24=0x222;
     raw.playerBodyH24=0x333;raw.playerEntityH24=0x444;raw.playerBodyIdH24=7;raw.emptyBothHandsH24=true;
     g_mock.configOk=!failedConfig;CK(hp::SubmitPresenceH24(0,raw));hp::ServiceStepFixture(0,7,1,100);
     hp::ServiceStepFixture(0,7,2,110);hp::ServiceStepFixture(0,7,3,120);
     hp::PresenceFeedbackH24 measured{};hp::ReadPresenceFeedbackH24(0,measured);
     if(failedConfig){CK(!measured.valid&&g_mock.presenceConfig==1&&g_mock.velocityCalls==0);
       CK(g_mock.destroyCalled&&g_mock.creates==1);g_mock.registryState=2;
       hp::ServiceStepFixture(0,7,4,130);hp::ServiceStepFixture(0,7,5,140);hp::ServiceStepFixture(0,7,6,150);
       CK(g_mock.creates==1&&g_mock.releaseCalled);}

     else {CK(measured.valid);const auto count=g_mock.velocityCalls;g_mock.presence=false;
       hp::ServiceStepFixture(0,7,4,130);hp::ReadPresenceFeedbackH24(0,measured);
       CK(!measured.valid&&g_mock.destroyCalled&&g_mock.velocityCalls==count);}
     kcd2vr::native_physics_phase_t4::Access::resetFixture(kcd2vr::native_physics_phase_t4::Phase::WorldReset,7);
     hp::ReadPresenceFeedbackH24(0,measured);CK(!measured.valid);
    }
    // Qualified dialogue presentation enables only the same physical hand
    // service. Revoking it stops motor writes and retires the native body.
    {hp::InstallForTest(mockSeam());g_mock.reset();g_mock.mass=.4f;g_dialoguePolicy=true;
     auto raw=in(100);raw.source.physicalPresenceH24=true;raw.source.characterH24=0x222;
     raw.source.cancelGeneration=2;raw.playerBodyH24=0x333;raw.playerEntityH24=0x444;
     raw.playerBodyIdH24=7;raw.emptyBothHandsH24=true;
     auto& c=g_dialogueContext;c={};c.calibrated=c.tracked=c.dialogue=true;
     c.epoch=raw.source.sourceGeneration;c.cancel=2;c.trackingEpoch=raw.source.trackingEpoch;
     c.character=raw.source.characterH24;c.entity=raw.playerEntityH24;
     kcd2vr::dialogue_state59c10r2::State native{};native.tick=100;native.known=native.active=true;
     native.entity=raw.playerEntityH24;native.player=raw.source.player;native.instance=0x555;
     native.camera=kcd2vr::dialogue_state59c10r2::Camera::FirstPerson;
     g_dialogueWitness=dialogueHand::capture(native,true,2,100);
     CK(!c.gameplay&&hp::SubmitPresenceH24(0,raw));g_dialogueNow=100;hp::ServiceStepFixture(0,7,1,100);
     g_dialogueNow=110;hp::ServiceStepFixture(0,7,2,110);
     g_dialogueNow=120;hp::ServiceStepFixture(0,7,3,120);
     hp::PresenceFeedbackH24 measured{};hp::ReadPresenceFeedbackH24(0,measured);
     CK(measured.valid&&g_mock.creates==1&&g_mock.presenceConfig==1&&g_mock.velocityCalls>0);
     CK(!c.gameplay);const auto velocityCalls=g_mock.velocityCalls;
     g_mock.presence=false;g_dialogueNow=125;hp::ServiceStepFixture(0,7,4,125);
     hp::ReadPresenceFeedbackH24(0,measured);CK(!measured.valid&&g_mock.destroyCalled);
     CK(g_mock.velocityCalls==velocityCalls&&g_mock.creates==1);
     g_mock.registryState=2;hp::ServiceStepFixture(0,7,5,130);CK(g_mock.releaseCalled);
     g_dialoguePolicy=false;
    }
    // Exact production Creating/Active/Retiring dispatcher with pending pair seam.
    // No simulated native success can make the owner publish before Present.
    {auto seam=mockSeam();seam.preparePresencePair=&MockPairPrepare;seam.retirePresencePair=&MockPairRetire;
     hp::InstallForTest(seam);g_mock.reset();g_mock.mass=.4f;pairPrepare=pairRetire=0;pairPreparations=pairRetirements=0;
     auto raw=in(100);raw.source.physicalPresenceH24=true;raw.source.characterH24=0x222;
     raw.playerBodyH24=0x333;raw.playerEntityH24=0x444;raw.playerBodyIdH24=7;raw.emptyBothHandsH24=true;
     auto step=[&](unsigned serial,unsigned tick){raw.tick=raw.source.poseTick=tick;CK(hp::SubmitPresenceH24(0,raw));hp::ServiceStepFixture(0,7,serial,tick);};
     step(1,100);step(2,110);step(3,120);hp::PresenceFeedbackH24 measured{};
     hp::ReadPresenceFeedbackH24(0,measured);CK(!measured.valid&&g_mock.velocityCalls==0);
     CK(g_mock.geometryAdds==1&&g_mock.presenceConfig==0&&pairPreparations==2);
     pairPrepare=1;step(4,130);step(5,140);hp::ReadPresenceFeedbackH24(0,measured);
     CK(measured.valid&&g_mock.velocityCalls==1&&g_mock.presenceConfig==1);
     const int motor=g_mock.velocityCalls;g_mock.presence=false;step(6,150);
     hp::ReadPresenceFeedbackH24(0,measured);CK(!measured.valid&&g_mock.velocityCalls==motor+1&&g_mock.lastVx==0&&g_mock.lastVy==0&&g_mock.lastVz==0);
     CK(pairRetirements==1&&!g_mock.destroyCalled&&g_mock.geometryRemoves==0);
     step(7,160);CK(!g_mock.destroyCalled&&g_mock.geometryRemoves==0); // unknown/Present is not Absent
     pairRetire=1;step(8,170);CK(g_mock.destroyCalled&&g_mock.geometryRemoves==1);
     g_mock.registryState=2;step(9,180);step(10,190);CK(g_mock.releaseCalled);
     // Same-world second presence incarnation must configure its new geometry.
     g_mock.presence=true;g_mock.registryState=1;step(11,200);step(12,210);step(13,220);
     CK(g_mock.creates==2&&g_mock.presenceConfig==2&&g_mock.geometryAdds==2);
     hp::ReadPresenceFeedbackH24(0,measured);CK(measured.valid);
    }
    // Pair refusal never produces an achieved hand or motor; ordinary retirement
    // still cannot remove the geometry until the original pair outcome is known.
    {auto seam=mockSeam();seam.preparePresencePair=&MockPairPrepare;seam.retirePresencePair=&MockPairRetire;
     hp::InstallForTest(seam);g_mock.reset();g_mock.mass=.4f;pairPrepare=-1;pairRetire=0;
     auto raw=in(100);raw.source.physicalPresenceH24=true;raw.source.characterH24=0x222;
     raw.playerBodyH24=0x333;raw.playerEntityH24=0x444;raw.playerBodyIdH24=7;raw.emptyBothHandsH24=true;
     CK(hp::SubmitPresenceH24(0,raw));hp::ServiceStepFixture(0,7,1,100);hp::ServiceStepFixture(0,7,2,110);
     hp::ServiceStepFixture(0,7,3,120);hp::PresenceFeedbackH24 measured{};hp::ReadPresenceFeedbackH24(0,measured);
     CK(!measured.valid&&g_mock.velocityCalls==0&&!g_mock.destroyCalled);
     pairRetire=-1;hp::ServiceStepFixture(0,7,4,130);CK(!g_mock.destroyCalled&&g_mock.geometryRemoves==0);
    }
    {auto seam=mockSeam();seam.preparePresencePair=&MockPairPrepare;seam.retirePresencePair=&MockPairRetire;
     hp::InstallForTest(seam);g_mock.reset();g_mock.mass=.4f;pairPrepare=0;pairRetire=0;
     auto raw=in(100);raw.source.physicalPresenceH24=true;raw.source.characterH24=0x222;
     raw.playerBodyH24=0x333;raw.playerEntityH24=0x444;raw.playerBodyIdH24=7;raw.emptyBothHandsH24=true;
     CK(hp::SubmitPresenceH24(0,raw));hp::ServiceStepFixture(0,7,1,100);hp::ServiceStepFixture(0,7,2,110);
     raw.tick=raw.source.poseTick=120;raw.playerBodyH24=0x555;raw.playerBodyIdH24=8;pairPrepare=1;
     CK(hp::SubmitPresenceH24(0,raw));hp::ServiceStepFixture(0,7,3,120);
     hp::PresenceFeedbackH24 measured{};hp::ReadPresenceFeedbackH24(0,measured);
     CK(!measured.valid&&g_mock.velocityCalls==0&&g_mock.presenceConfig==0&&!g_mock.destroyCalled);
    }
    // D80 actual proxy admission: idle OFF has no native work, fresh presence
    // wakes independently of F9, cancellation cannot suppress retained debt.
    {hp::InstallForTest(mockSeam());g_mock.reset();CK(!hp::NeedsPhysicsStepD80(100));
     auto raw=in(100);raw.source.physicalPresenceH24=true;raw.source.characterH24=0x222;
     raw.playerBodyH24=0x333;raw.playerEntityH24=0x444;raw.playerBodyIdH24=7;raw.emptyBothHandsH24=true;
     CK(hp::SubmitPresenceH24(0,raw));CK(hp::NeedsPhysicsStepD80(100));
     CK(!hp::NeedsPhysicsStepD80(181)); // expired mailbox is not a forever wake
     hp::CancelPresenceH24(0);CK(!hp::NeedsPhysicsStepD80(100));
     hp::g_proxy[0].bodyHandle=0x123;CK(hp::NeedsPhysicsStepD80(181));hp::g_proxy[0].bodyHandle=0;
     hp::g_proxy[1].geometry=reinterpret_cast<void*>(0x234);CK(hp::NeedsPhysicsStepD80(181));hp::g_proxy[1].geometry=nullptr;
     hp::g_proxy[0].constraintPinned=true;CK(hp::NeedsPhysicsStepD80(181));hp::g_proxy[0].constraintPinned=false;
     hp::g_proxy[1].policy.state=hpt4::State::Quarantined;CK(hp::NeedsPhysicsStepD80(181));hp::g_proxy[1].policy.state=hpt4::State::Off;
     CK(!hp::NeedsPhysicsStepD80(181));CK(g_mock.creates==0&&g_mock.velocityCalls==0&&g_mock.geometryAdds==0);
    }
    if(fails)return 1;std::printf("connected proxy fixture F69 + H24 physical presence OK checks=%d\n",checks);return 0;
}
