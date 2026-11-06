// Public physical kernel -> actual current intent adapter -> complete original-
// first grace callbacks. Native eligibility/result/timer calls are counted host
// leaves. This is not the final retail NativePhysicalAction117/recipe proof.
#define main retained_combo_lineage19_main
#include "combo_lineage19_test.cpp"
#undef main
#include "gesture_recovery_h30_fixture.h"
namespace currentIntentAdapterH8 {
struct PhysFrame6698 {ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};std::uint64_t request=0;};
kcd2vr::core::SnapshotMailbox<PhysFrame6698> g_phys_frame6698;
static std::atomic<std::uint64_t> g_phys_ready_sample6698{0},g_pose_epoch6689{4},g_weapon_request_epoch6694{5};
static std::atomic<int> g_geometry_owner_stage49{0};
static std::atomic<bool> g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_scripted{false},g_ctx_reading{false};
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};}
static bool head=true,hand[2]{true,true},held=true;
static void (*onHeldReadH8)()=nullptr;
static bool NativeHeldContactsCurrentT4(){if(onHeldReadH8){const auto callback=onHeldReadH8;onHeldReadH8=nullptr;callback();}return held;}
static bool TrackingHeadUsable111(std::uint64_t){return head;}
static bool TrackingHandUsable111(int s,std::uint64_t){return hand[s];}
#include "combo_intent_owner_current.inc"
static void publish(const RigH30& r){PhysFrame6698 f{r.in,r.out,r.profiles,r.in.epoch};g_phys_frame6698.publish(f);g_phys_ready_sample6698=r.in.sample;now=r.in.tick;}
}
static cg::Owner publicOwnerH8(const RigH30& r,const ph::Command& release){
 auto o=owner();o.key={release.gesture59c3,release.key,release.generation,release.bodyEpoch,release.epoch,6,reinterpret_cast<std::uintptr_t>(player)};
 o.tick=now;return o;
}
#ifndef KCD2VR_PUBLIC_REARM_PREFIX_ONLY
int main(){
 assert(retained_combo_lineage19_main()==0);
 for(bool stab:{false,true})for(unsigned withdrawal=0;withdrawal<9;++withdrawal){
  setup();combo59c17::g_teardown_pending19=false;combo59c17::g_ready=true;
  currentIntentAdapterH8::g_pose_epoch6689=4;currentIntentAdapterH8::g_weapon_request_epoch6694=5;
  currentIntentAdapterH8::g_gameplay_allowed=true;currentIntentAdapterH8::g_ctx_menu=false;
  currentIntentAdapterH8::animation_filter::nativeBodyOwned112=false;
  currentIntentAdapterH8::head=currentIntentAdapterH8::hand[0]=currentIntentAdapterH8::hand[1]=currentIntentAdapterH8::held=true;
  RigH30 r(false,stab?ph::Vec3{0,1,0}:ph::Vec3{1,0,0});
  r.targetId=11; // explicit collision-query leaf joins the same native target
  r.state={};r.in.bodyEpoch=4;r.in.epoch=5;r.profiles[0].key=2;r.profiles[0].generation=3;
  for(unsigned i=0;i<30;++i)r.step();r.commands.clear();moveH30(r,14);
  const auto kind=stab?ph::CommandKind::Thrust:ph::CommandKind::Slash;
  const auto* found=firstH30(r,kind);assert(found);const auto release=*found;
  assert(r.count(ph::CommandKind::Prepare)==1&&r.count(kind)==1);
  r.blocked=r.actor=true;r.step({0,.009,0});const auto impact=r.out.impactRecord178r30[0];
  assert(impact.valid&&impact.gesture59c3==release.gesture59c3&&impact.entityId==11&&impact.part==1&&impact.tick==r.in.tick&&impact.sample==r.in.sample);
  r.blocked=r.actor=false;
  currentIntentAdapterH8::publish(r);currentIntentFrameH30=&currentIntentAdapterH8::GeometryIntentOwnerH30;
  currentIntentStateFrameH30=&currentIntentAdapterH8::GeometryIntentOwnerStateH30;
  assert(frik_pose_probe::GeometryIntentOwnerH30(2,3,4,5));
  auto paid=publicOwnerH8(r,release);
  // Native APPLIED/OPEN is an explicit original-call leaf; no outcome is forced
  // by production here. The same-stack offer is the real grace consumer input.
  alignas(16) std::array<unsigned char,128> trigger{};triggerMemory=trigger.data();
  std::memcpy(trigger.data()+8,&actor,sizeof(actor));trigger[0x18]=trigger[0x19]=trigger[0x1B]=1;
  float duration=.3f;std::memcpy(trigger.data()+0x1C,&duration,4);
  {cg::OfferScope offer(&paid);kcd2vr::geometry40c1::Scope nativeStack(actor,reinterpret_cast<void*>(1),targetA);
   geom27::g_inject_result178r35={true,true,true,1,11};
   combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,16);}
  cg::SlotLeaseF1 opened{};std::uint64_t consumed=0;
  assert(cg::nativeSlot.read(opened,consumed)&&opened.owner.key.gesture==release.gesture59c3&&timers==1);
  const auto nativeAnchor=opened.owner.tick;const auto nativeWindow=opened.window;
  const auto aborts=r.count(ph::CommandKind::Abort);
  for(unsigned i=0;i<12;++i)r.step();
  for(unsigned i=0;i<12;++i)r.step({0,-.025,0});
  for(unsigned i=0;i<12;++i)r.step();
  ph::Command follow{};
  for(unsigned i=0;i<25&&!follow.gesture59c3;++i){r.step({0,.025,0});
   for(unsigned j=0;j<r.out.commandCount;++j)if(r.out.commands[j].kind==ph::CommandKind::Prepare)follow=r.out.commands[j];}
  currentIntentAdapterH8::publish(r);
  assert(follow.gesture59c3&&follow.gesture59c3!=release.gesture59c3&&follow.sample>release.sample&&follow.tick>release.tick);
  assert(r.count(ph::CommandKind::Abort)==aborts);
  combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,1);
  assert(managerCalls.empty());
  cg::SlotLeaseF1 after{};assert(cg::nativeSlot.read(after,consumed));
  assert(after.owner.tick==nativeAnchor&&after.window==nativeWindow&&after.owner.release==paid.release&&timers==1);
  assert(!kcd2vr::contact176::fresh(impact.tick,now,kcd2vr::contact59c3::maximumContactAge));
  assert(r.count(kind)==1); // rearming a follow-up cannot replay the old release
  // Actual current frame/context withdrawal revokes the old grace; it cannot
  // turn the new command into a fresh old-release/native timer receipt.
  if(withdrawal==0)++currentIntentAdapterH8::g_weapon_request_epoch6694;
  if(withdrawal==1)++currentIntentAdapterH8::g_pose_epoch6689;
  if(withdrawal==2)currentIntentAdapterH8::g_ctx_menu=true;
  if(withdrawal==3)currentIntentAdapterH8::head=false;
  if(withdrawal==4)currentIntentAdapterH8::hand[0]=false;
  if(withdrawal==5)currentIntentAdapterH8::held=false;
  if(withdrawal==6)now+=81; // original completed frame is now stale, not restamped
  if(withdrawal==7)currentIntentAdapterH8::g_gameplay_allowed=false;
  if(withdrawal==8)currentIntentAdapterH8::animation_filter::nativeBodyOwned112=true;
  combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,1);
  assert(managerCalls==std::vector<int>({1})&&!cg::nativeSlot.read(after,consumed));
  currentIntentAdapterH8::g_weapon_request_epoch6694=5;currentIntentFrameH30=nullptr;currentIntentStateFrameH30=nullptr;triggerMemory=nullptr;
 }
 std::puts("PASS public H8 cut/stab rearm -> actual H30 intent adapter -> actual grace callbacks; native APPLIED/OPEN leaves modeled; final input/recipe equivalence NOT_PROVEN");
}
#endif
