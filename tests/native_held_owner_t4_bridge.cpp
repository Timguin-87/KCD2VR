// Complete production adapter: interleaved coherent/physics receipts, policy
// revocation, publication and writer arbitration. No game/native execution.
#include "../src/frik_pose_probe.cpp"
#include "native_held_backend_t4_stub.h"
#include "../src/subsystems/hybrid/contact_policy.h"
#include <cassert>
#include <cstdio>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
using namespace kcd2vr::frik_pose_probe;
namespace f=kcd2vr::native_weapon_body_t4::fixture;
static void setup(CoherentTransaction6689& tx){
 tx.nativeHeldActiveT4=false;tx.physicalState={};
 hostTickCount64=2000;g_pose_epoch6689=1;g_weapon_request_epoch6694=2;g_pose_policy_generation57=2;
 g_weapon_control_mode=4;g_cal_state=2;g_gameplay_allowed=true;g_phys_fault6698=false;
 g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_reading=g_ctx_scripted=false;
 g_head_valid=g_right_valid=g_left_valid=true;g_head_valid_tick=g_right_valid_tick=g_left_valid_tick=2000;
 g_right_grip=1;g_left_grip=0;g_tool_context19c4r2=2000ull<<8;
 kcd2vr::animation_filter::nativeBodyOwned112=false;
 tx.epoch=1;tx.tracking.source_epoch=kcd2vr::tracking59c6::epoch.load();
 tx.tracking.head_valid=tx.tracking.right_valid=tx.tracking.left_valid=tx.tracking.allowed=true;
 auto& pi=tx.physicalInput;pi.allowed=pi.haveWorld=pi.tracked[0]=pi.intentTracked[0]=true;
 pi.epoch=2;pi.bodyEpoch=1;pi.sample=200;pi.tick=2000;pi.grip[0]=1;pi.reach[0]=1.;pi.worldScale=1;
 pi.world.p={10,20,30};pi.world.q={0,0,.7071067811865475,.7071067811865475};pi.hands[0].p={.3,.1,0};
 auto& p=tx.physicalProfiles[0];p.key=77;p.generation=88;p.entity=99;p.classId=4;p.side=0;
 p.primary=p.geometry=true;p.grip[0].p={0,.1,0};p.grip[0].q={0,0,0,1};
 tx.physicalEquipment.handKnown[1]=true;tx.physicalCorrelation126[0].stamp.actor=0x1234;
 NativeHeldSourceT4 s{};s.valid=true;s.tick=2000;s.input.owner={77,88,2,1,0x2000,0x3000,0x1234,0x4000,0x5000,0x6000,99,0};
 s.input.colliderLease=5;s.input.trackingEpoch=tx.tracking.source_epoch;s.input.cancelGeneration=2;
 s.input.weaponWorld.p={10.2,20.4,30};s.input.weaponWorld.q={0,0,0,1};
 s.input.boundsMin={-.01,0,-.02};s.input.boundsMax={.01,1,.02};s.input.scale=1;s.input.mass=1.5;s.input.classId=4;
 g_native_held_source_t4.publish(s);g_native_held_valid_t4=true;g_native_held_lease_t4=5;
 f::enabled=f::pending=f::read=f::ownerRead=true;f::submits=f::retires=f::cancels=0;
 f::achieved={};auto& a=f::achieved;a.owner=s.input.owner;a.colliderLease=5;a.trackingEpoch=s.input.trackingEpoch;a.cancelGeneration=2;
 a.worldGeneration=3;a.bodyGeneration=7;a.bodyId=120;a.sample=199;a.sourceTick=1995;a.observedTick=2000;
 a.weaponWorld=s.input.weaponWorld;a.colliderRetired=false;a.safeNativeRestore=false;
}
int main(){
 {NativeHeldProfile62 p{};
  assert(!std::strcmp(NativeHeldProfileReason62(p),"REQUEST_UNREADABLE"));p.readMask=1;
  assert(!std::strcmp(NativeHeldProfileReason62(p),"APPLIED_UNREADABLE"));p.readMask=3;
  assert(!std::strcmp(NativeHeldProfileReason62(p),"DIRTY_UNREADABLE"));p.readMask=7;p.requested=2;p.applied=4;
  assert(!std::strcmp(NativeHeldProfileReason62(p),"TRANSITION_PENDING"));p.requested=p.applied=1;p.dirty=0;
  assert(!std::strcmp(NativeHeldProfileReason62(p),"STABLE_PROFILE_UNSUPPORTED"));p.requested=p.applied=2;p.dirty=1;
  assert(!std::strcmp(NativeHeldProfileReason62(p),"PROFILE_DIRTY"));p.dirty=0;assert(!NativeHeldProfileReason62(p));
  p.requested=p.applied=4;assert(!NativeHeldProfileReason62(p));
  p.requested=p.applied=0;assert(!NativeHeldProfileReason62(p)); // observation only; exact suspended body still required
  for(unsigned unsupported:{1u,3u,5u,6u,255u,~0u}){p.requested=p.applied=unsupported;
   assert(!std::strcmp(NativeHeldProfileReason62(p),"STABLE_PROFILE_UNSUPPORTED"));}
  p.requested=p.applied=0;
  NativeHeldProfileReceipt62 receipts{};assert(receipts.take(1,p,1000));assert(!receipts.take(1,p,2000));
  p.dirty=1;assert(!receipts.take(1,p,1999));assert(receipts.take(1,p,2000));
  for(unsigned i=0;i<30;++i)assert(receipts.take(10+i,p,3000+1000*i));
  assert(!receipts.take(99,p,40000)); // finite selected-session budget
 }

 CoherentTransaction6689 tx{};setup(tx);
 for(auto state:{heldT4::State::Creating,heldT4::State::Configuring}){
  f::achieved.state=state;f::achieved.poseValid=false;
  assert(StageNativeHeldT4(tx)==NativeHeldStageResultT4::Waiting);
  assert(!tx.nativeHeldActiveT4&&f::retires==0&&f::cancels==0); // no cancel-on-create loop
 }
 f::achieved.state=heldT4::State::Active;f::achieved.poseValid=true;
 assert(StageNativeHeldT4(tx)==NativeHeldStageResultT4::Applied&&f::submits==3);
 assert(tx.nativeHeldActiveT4&&tx.physicalOutput.solverOwnedT4[0]);
 assert(!tx.physicalInput.dynamics&&!tx.physicalInput.attacks&&!tx.physicalInput.contact);
 assert(tx.physicalOutput.commandCount==0&&!ph::attack_authority55c1(tx.physicalInput,tx.physicalOutput,0));
 auto local=tx.physicalOutput.weapons[0];local.p=local.p*tx.physicalInput.worldScale;
 const auto world=ph::compose(tx.physicalInput.world,local);
 assert(pm::length(world.p-f::achieved.weaponWorld.p)<1e-8&&pm::angle(world.q,f::achieved.weaponWorld.q)<1e-7);
 assert(pm::length(ph::compose(tx.physicalOutput.weapons[0],tx.physicalProfiles[0].grip[0]).p-tx.physicalOutput.hands[0].p)<1e-9);
 assert(tx.physicalOutput.solverSourceTickT4==1995&&tx.physicalOutput.solverSourceSampleT4==199);
 assert(CoherentOwnerCurrent57(tx));
 ++f::achieved.bodyGeneration;assert(!CoherentOwnerCurrent57(tx));--f::achieved.bodyGeneration;
 g_ctx_menu=true;assert(!CoherentOwnerCurrent57(tx));
 assert(StageNativeHeldT4(tx)==NativeHeldStageResultT4::Waiting&&f::cancels==1);g_ctx_menu=false;
 // Contended receipt and owner snapshots never return controller/native grace.
 f::read=false;f::ownerRead=false;
 assert(NativeHeldBusyT4(0));assert(!NativeHeldBusyT4(1));
 assert(NativeHeldAttachmentT4(reinterpret_cast<void*>(0x6000),reinterpret_cast<void*>(0x5000))==PhysicalAttachmentResult101::Busy);
 assert(StageNativeHeldT4(tx)==NativeHeldStageResultT4::Waiting);
 f::read=f::ownerRead=true;f::achieved.state=heldT4::State::Retiring;f::achieved.poseValid=false;
 assert(StageNativeHeldT4(tx)==NativeHeldStageResultT4::Waiting);
 f::pending=false;f::enabled=false;f::achieved.colliderRetired=true;
 assert(StageNativeHeldT4(tx)==NativeHeldStageResultT4::Fallback&&!NativeHeldBusyT4());
 // Old completed ordinary geometry/cost cannot survive a solver admission,
 // including a whole create+retire between native-frame observations.
 setup(tx);f::pending=false;f::admissions=0;g_native_held_admission_seen_t4=0;
 PhysFrame6698 ordinary{};ordinary.input.allowed=ordinary.input.attacks=ordinary.input.physicalCombat177=true;
 ordinary.input.tracked[0]=true;ordinary.input.sample=199;ordinary.input.tick=1995;ordinary.input.bodyEpoch=1;
 ordinary.request=2;ordinary.profiles[0]=tx.physicalProfiles[0];ordinary.output.owned[0]=ordinary.output.active[0]=true;
 g_phys_frame6698.publish(ordinary);g_phys_ready_sample6698=199;
 assert(GeometryOwnerValid178r30(77,88,1,2));
 // No achieved frame ever ran: both the retained pose state and the staged
 // copy contain an old C59 prepared recognizer for the SAME item/request.
 // It must not acquire a new recovery key and synthesize a fresh prepare.
 g_phys_state6698={};g_phys_state6698.epoch=2;g_phys_state6698.bodyEpoch=1;g_phys_state6698.sample=199;
 auto& old=g_phys_state6698.weapon[0];old.key=77;old.generation=88;old.ready=old.prepared=true;
 old.lastSample=199;old.lastTick=1995;old.gesture59c3=51;old.natural118=true;
 old.stroke118.ready=old.stroke118.active=old.stroke118.prepared=true;old.stroke118.kind=kcd2vr::melee118::Kind::Cut;
 tx.physicalState=g_phys_state6698;g_native_held_pose_admission_t4=0;
 f::enabled=f::pending=false;f::admissions=1;
 assert(StageNativeHeldT4(tx)==NativeHeldStageResultT4::Fallback);
 assert(!g_phys_state6698.weapon[0].prepared&&!g_phys_state6698.weapon[0].stroke118.active);
 assert(!tx.physicalState.weapon[0].prepared&&!tx.physicalState.weapon[0].stroke118.active);
 auto ask=tx.physicalInput;ask.allowed=ask.attacks=ask.contact=ask.coherentMelee119=ask.naturalCombat117=true;
 ask.tracked[0]=ask.intentTracked[0]=true;ask.grip[0]=1;ask.sample=200;ask.tick=2000;
 const auto result=ph::step(tx.physicalState,ask,tx.physicalProfiles,[](const ph::Profile&,ph::Pose,ph::Pose,bool){return ph::Contact103{};});
 assert(tx.physicalState.weapon[0].gesture59c3!=51);
 for(unsigned i=0;i<result.commandCount;++i)assert(result.commands[i].gesture59c3!=51);
 // An admission after staging prevents the old transaction from publishing
 // its cached recognizer even when the body has already retired.
 ++f::admissions;assert(!CoherentOwnerCurrent57(tx));
 namespace cost=kcd2vr::swing_effort59c14;namespace hybrid=kcd2vr::hybrid54;
 const auto recovery=kcd2vr::contact176::recoveryEpoch55c1.load();
 kcd2vr::motion59c4::Key key{51,77,88,1,2,recovery,0x2000};
 assert(cost::ledger.publish(key,30.,1.5,false,1995)&&cost::ledger.nativeComplete54(key,1995));
 assert(hybrid::costProof54(cost::ledger,key,1995,2000,recovery)==hybrid::CostProof54::Ready);
 kcd2vr::contact176::Ticket ticket{};ticket.player=0x2000;ticket.entity=0x3000;ticket.serial=100;
 ticket.tick=1995;ticket.sample=199;ticket.epoch=1;ticket.request=2;ticket.key=77;ticket.generation=88;
 ticket.policy=kcd2vr::contact176::policy.load();ticket.recovery59c3=recovery;
 kcd2vr::hybrid_serial59::Token entered{},reserved{},duplicate{};
 auto& ledger=kcd2vr::hybrid_serial59::releases;
 assert(ledger.reserve(ticket,2000,entered)==kcd2vr::hybrid_serial59::Result::Ready&&ledger.tryEnter(entered));
 auto secondTicket=ticket;secondTicket.serial=101;
 assert(ledger.reserve(secondTicket,2000,reserved)==kcd2vr::hybrid_serial59::Result::Ready);
 f::pending=true;f::admissions=1;
 assert(!GeometryOwnerValid178r30(77,88,1,2)&&GeometryOwnerStage49()==10);
 ph::Command command{};assert(!NativePhysicalAction117(nullptr,command,ordinary,true)); // central native leaf also refuses
 f::pending=false; // body already retired, serial still unobserved
 assert(!GeometryOwnerValid178r30(77,88,1,2));
 kcd2vr::attack_xi178r8::rtHeld=true;kcd2vr::attack_xi178r8::pendingStroke=1;
 assert(NativeHeldObserveAdmissionsT4(2000));assert(g_phys_ready_sample6698.load()==0);
 assert(kcd2vr::attack_xi178r8::abortPending.load()&&kcd2vr::attack_xi178r8::pendingStroke.load()==0);
 const auto retiredRecovery=kcd2vr::contact176::recoveryEpoch55c1.load();assert(retiredRecovery==recovery+1);
 assert(!ledger.tryEnter(reserved));
 assert(ledger.reserve(ticket,2000,duplicate)==kcd2vr::hybrid_serial59::Result::Duplicate); // ENTERED survives
 assert(!NativeHeldObserveAdmissionsT4(2000)&&kcd2vr::contact176::recoveryEpoch55c1.load()==retiredRecovery);
 assert(hybrid::costProof54(cost::ledger,key,1995,2000,retiredRecovery)==hybrid::CostProof54::Refuse);
 assert(!GeometryOwnerValid178r30(77,88,1,2));
 // A new completed ordinary packet restores the C59 route; old exact-key cost
 // is still invalid, rather than cleared/redebited or allowed to revive.
 g_phys_frame6698.publish(ordinary);g_phys_ready_sample6698=199;
 assert(GeometryOwnerValid178r30(77,88,1,2));
 std::puts("native_held_owner_t4_bridge: actual adapter create/config/active, world+grip closure, native authority exclusion, body ABA/context revocation and contended retirement gates PASS");
}
