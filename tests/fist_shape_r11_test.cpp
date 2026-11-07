// Actual production callback and publisher, extracted at build time. Native memory and engine services are explicit test leaves.
// Deadline crossing in the simulation is not an accepted hit or headset result.
#include "vrik/fist_clock_r11.h"
#include "vrik/fist_action_h12.h"
#include "vrik/fist_reach_r10.h"
#include "vrik/passive_contact169r1.h"
#include "vrik/npc_contact172.h"
#include <map>
#include <cstring>
#include <cstdio>
#include <cstdlib>
namespace pm=kcd2vr::pose_core;namespace ph {using pm::Vec3;struct Pose {pm::Quat q{};Vec3 p{};};}
namespace ac50=kcd2vr::attackclock50;namespace fh=kcd2vr::fist_action_h12;namespace fc=kcd2vr::fist_clock_r11;
static unsigned checks=0;static void ck(bool yes,const char* why){++checks;if(!yes){std::printf("FAIL %s\n",why);std::abort();}}
static std::uint64_t now=1000;static std::uint64_t GetTickCount64(){return now;}static std::uintptr_t WhBase(){return 0x70000000;}
static bool selected=true,blocked=false,resolveOk=true,emptyOk=true,trackingOk=true;
static bool FistSelected101(){return selected;}static bool HandClaimBlockedT24(int,std::uintptr_t,std::uint64_t,std::uint64_t){return blocked;}
static std::atomic<int> g_weapon_control_mode{7};
static std::atomic<bool> g_motion_clock_ready50{true},g_motion_clock_fault50{false},g_gameplay_allowed{true};
static std::atomic<bool> g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_scripted{false};
static std::atomic<std::uint64_t> g_fist_epoch101{3},g_pose_epoch6689{2},g_attack_timing_policy22{4},g_weapon_request_epoch6694{5};
namespace diagnostics::lab {static bool native=false;static bool NativeReference(){return native;}}
namespace diagnostics::capture168 {enum class Domain{Combat};static bool active(Domain,std::uint64_t){return false;}}
namespace diagnostic_trace {static void Write(const char*){}}
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};static bool required=false;static bool PlayerNativeBodyRequiredForMotion122(){return required;}}
namespace retail_build112 {static bool ready=true;static bool Ready(std::uintptr_t){return ready;}}
struct NativeOwner104 {void* player=nullptr;void* actor=nullptr;void* state=nullptr;void* entity=nullptr;std::uint64_t epoch=0;};
struct FistOwner101 {void* player=nullptr;void* manager=nullptr;std::uint64_t tick=0,epoch=0,request=0;bool empty=false;};
struct TrackingSnapshot {bool allowed=true,head_valid=true,head_held=false,right_valid=true,left_valid=true,right_held=false,left_held=false;float left_grip=1,right_grip=1,left_trigger=1,right_trigger=1;std::uint64_t tick=1000;};
static NativeOwner104 owner{};static FistOwner101 empty{};static TrackingSnapshot tracking{};
static bool NativeResolve104(void*,NativeOwner104& o){o=owner;return resolveOk;}static bool FistReadOwner101(FistOwner101& e){e=empty;return emptyOk;}static bool ReadTrackingSnapshot(TrackingSnapshot& t){t=tracking;return trackingOk;}
static std::map<std::uintptr_t,unsigned char> memory;static unsigned writes=0;static int failWrite=-1;
template<class T> static void put(std::uintptr_t a,const T& v){auto* p=reinterpret_cast<const unsigned char*>(&v);for(std::size_t i=0;i<sizeof(v);++i)memory[a+i]=p[i];}
static bool ClockRead50(std::uintptr_t a,void* out,std::size_t n){auto* p=static_cast<unsigned char*>(out);for(std::size_t i=0;i<n;++i){auto it=memory.find(a+i);if(it==memory.end())return false;p[i]=it->second;}return true;}
static bool ClockWrite50(std::uintptr_t a,const void* src,std::size_t n){if(int(writes++)==failWrite)return false;auto* p=static_cast<const unsigned char*>(src);for(std::size_t i=0;i<n;++i)memory[a+i]=p[i];return true;}
#include "fist_clock_r11_frame.inc"
#include "fist_clock_r11_publisher.inc"
#include "fist_clock_r11_callback.inc"
struct Fixture {
 const std::uintptr_t player=0x10000,entity=0x11000,actor=0x20000,state=0x30000,manager=0x40000,slots=0x41000,action=0x50000,helper=0x60000,row=0x70000,type=0x71000,clock=0x80000,records=0x81000,gate=0x82000;
 fh::Binding binding{};FistContactFrameH11 f{};kcd2vr::passive169r1::Sample sample{};kcd2vr::npc_contact172::Witness target{};
 static inline unsigned generation=10;
 Fixture(){
  memory.clear();writes=0;failWrite=-1;now=1000;selected=true;blocked=false;resolveOk=emptyOk=trackingOk=true;g_weapon_control_mode=7;g_motion_clock_ready50=true;g_motion_clock_fault50=false;g_gameplay_allowed=true;
  g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=false;g_fist_epoch101=3;g_pose_epoch6689=2;g_attack_timing_policy22=4;g_weapon_request_epoch6694=5;
  diagnostics::lab::native=false;animation_filter::nativeBodyOwned112=false;animation_filter::required=false;retail_build112::ready=true;fc::enabled=true;fc::maxHoldMs=500;
  owner={reinterpret_cast<void*>(player),reinterpret_cast<void*>(actor),reinterpret_cast<void*>(state),reinterpret_cast<void*>(entity),2};empty={owner.player,reinterpret_cast<void*>(manager),now,2,3,true};tracking={};tracking.tick=now;
  auto ptr=[&](std::uintptr_t a,std::uintptr_t b){put(a,b);};auto i=[&](std::uintptr_t a,int b){put(a,b);};auto by=[&](std::uintptr_t a,unsigned char b){put(a,b);};
  ptr(actor,WhBase()+0x3A80610);ptr(actor+0x1F8,player);ptr(player,WhBase()+0x3AA4B98);ptr(player+0x38,entity);ptr(actor+0x210,state);ptr(state+0xCE0,actor);ptr(actor+0x3C0,clock);ptr(clock+8,actor);
  ptr(actor+0x200,manager);ptr(manager+0x38,slots);ptr(slots,action);ptr(action,WhBase()+0x3A61C58);ptr(action+0x40,manager);ptr(action+0x70,actor);ptr(action+0xA0,helper);ptr(action+0x58,row);i(action+0x3C,0);by(action+0x28,1);
  ptr(helper,WhBase()+0x3A61040);ptr(helper+8,actor);ptr(helper+0x20,action+0x88);ptr(action+0x88,WhBase()+0x3A61440);by(clock+0x14,1);
  ptr(row+0x148,type);i(type+8,6);i(state+0x218,6);i(state+0x38,4);i(row+0x224,0);i(state+0x1B8,0);ptr(actor+0x220,gate);by(gate+0x28,0);ptr(clock+0x58,records);ptr(clock+0x60,records+3*sizeof(ac50::Record));
  put(clock+0x10,2.f);put(clock+0x50,.022f);put(clock+0x54,.011f);ac50::Record rows[3]{{0,.052f,.104f,.041f,.082f},{1,.109f,.218f,.098f,.196f},{2,.476f,.952f,.465f,.93f}};put(records,rows);
  binding.intent.player=player;binding.intent.actor=actor;binding.intent.state=state;binding.intent.manager=manager;binding.intent.kind=kcd2vr::attack_timing178r22c2::Kind::Punch;binding.intent.serial=++generation;binding.intent.policy=4;binding.intent.epoch=2;binding.intent.request=3;binding.intent.sample=generation;binding.intent.tick=now;binding.intent.side=0;binding.intent.speed=2;
  binding.action=action;binding.helper=helper;binding.row=row;binding.generation=generation;binding.nativeType=6;binding.nativeHand=0;fh::generation=generation;fh::published=generation;fh::actor=actor;fh::action=action;ck(fh::mailbox.publish(binding),"binding mailbox");
  f.player=owner.player;f.request=3;f.epoch=2;f.tick=now;f.sample=generation+1;f.character=0x90000;f.completedH31=true;f.valid[0]=f.pairH31[0]=true;f.gestureSampleH31[0]=generation;f.previous[0]={0,.2,1};f.palm[0]={0,.3,1};f.gestureDirectionH31[0]={0,1,0};publishFrame();
  sample.source=kcd2vr::passive169r1::Source::HandWorld;sample.status=kcd2vr::passive169r1::Status::Contact;sample.eligible=sample.worldCoherent=true;sample.sequence=f.sample;sample.actor=f.character;sample.epoch=2;sample.side=0;sample.point=f.palm[0];
  target.ownerRead=true;target.owner=0xA0000;target.collider=0xB0000;target.entityId=77;target.body=kcd2vr::npc_contact172::Body::Living;
 }
 void publishFrame(){ck(g_fist_contact_h11.publish(f),"current frame publishes");}
 float get(std::uintptr_t a){float x=-1;ck(ClockRead50(a,&x,4),"native field reads");return x;}
 void tick(float dt=.011f){(void)FistClockMidR11(clock,dt);}
 void contact(std::uint64_t q=0){FistPublishContactR11(sample,target,q?q:now);}
 void quiet(){tick();ck(writes==0,"refused clock writes none");}
};

#include "vrik/contact_authority176.h"
struct CommittedFrame6695 {struct Stamp {std::uint64_t epoch=0,sample=0;std::uintptr_t actor=0;} stamp;bool valid[2]{true,true};};
static kcd2vr::core::SnapshotMailbox<CommittedFrame6695> g_committed_frame6695;
static bool FistContactRequestedH31(){return true;}
static bool CombatExperiment175Enabled(){return true;}
static std::atomic<std::uintptr_t> g_prediction_contract120{0};static std::atomic<bool> g_prediction_fault120{false};
// Values supplied by the actual predictor's tested committed-pose adapter.
struct PredictionScope120 {void* collisions=nullptr;void* wrapper=nullptr;void* action=nullptr;bool redirected=false,fistH31=false,fistReplayR10=false;
 int hand=-1,bone=-1;unsigned interpolations=0,sweeps=0;std::uint64_t fistGenerationH31=0,fistGestureSampleH31=0;
 FistContactFrameH11 fistContactH31{};kcd2vr::contact176::Payload authority176{};};
static PredictionScope120* g_prediction_scope120=nullptr;
#include "fist_shape_r11_authority.inc"
#include "fist_shape_r11_receiver.inc"

struct ContactFixture:Fixture {
 const std::uintptr_t collisions=0xB0000,entries=0xC0000,receiver=0xD0000;
 PredictionScope120 prediction{};float point[3]{.1219f,.3f,1.f};int nativeCalls=0;
 ContactFixture(){
  put(state+0x38,8);put(actor+0x218,collisions);put(collisions+8,actor);put(collisions+0x28,entries);put(entries-4,1);
  put(entries,receiver);put(entries+8,WhBase()+0xC5235C);put(helper+0x18,receiver);put(helper+0x10,WhBase()+0x3A61028);
  put(receiver,WhBase()+0x3A60FF0);put(receiver+8,actor);put(receiver+0x10,helper+0x10);put(receiver+0x18,(unsigned char)1);
  g_committed_frame6695.publish({{f.epoch,f.sample,f.character},{true,true}});
  prediction.collisions=reinterpret_cast<void*>(collisions);prediction.wrapper=reinterpret_cast<void*>(0xE0000);prediction.action=reinterpret_cast<void*>(0xF0000);
  prediction.redirected=prediction.fistH31=true;prediction.hand=binding.nativeHand;prediction.bone=417;prediction.interpolations=prediction.sweeps=1;
  prediction.fistGenerationH31=binding.generation;prediction.fistGestureSampleH31=binding.intent.sample;prediction.fistContactH31=f;
  g_prediction_scope120=&prediction;g_prediction_contract120=WhBase();g_prediction_fault120=false;
 }
 kcd2vr::contact176::Payload payload(){return PredictionAuthority176(WhBase()+0xA7C9C1,reinterpret_cast<void*>(collisions),0,417,.011f);}
 int invoke(){int admission=-99;
  fh::run(receiver,collisions+0x170,WhBase(),now,true,ClockRead50,[&]{
    admission=FistContactAdmissionH12(reinterpret_cast<void*>(actor),point,0,reinterpret_cast<void*>(collisions+0x170));
    if(admission==1)++nativeCalls; // original game ProcessHit would run once here; its outcome is not modeled
  });return admission;
 }
};
int main(){
 for(float distance:{.1219f,.1213f}){ContactFixture q;q.point[0]=distance;kcd2vr::contact176::SweepScope scope(q.payload());
  ck(q.invoke()==1&&q.nativeCalls==1&&fh::receiptH31.nativeShapeH31,"after: exact current native shape contact reaches original engine once");

 }
 {ContactFixture q;kcd2vr::contact176::SweepScope scope(q.payload());q.point[0]=.119f;ck(q.invoke()==1,"original inside-palm contact remains eligible");}
 {ContactFixture q;kcd2vr::contact176::SweepScope scope(q.payload());q.point[0]=NAN;ck(q.invoke()==0&&fh::receiptH31.failure==fh::FailureH31::NonFinite,"non-finite native point remains rejected");}
 {ContactFixture q;kcd2vr::contact176::SweepScope scope(q.payload());q.prediction.fistContactH31.palm[0].x=NAN;ck(q.invoke()==0&&fh::receiptH31.failure==fh::FailureH31::NonFinite,"non-finite completed palm remains rejected");}
 {ContactFixture q;ck(q.invoke()==0&&fh::receiptH31.failure==fh::FailureH31::OutsidePalm,"prediction scope alone cannot remove radius bound");}
 {ContactFixture q;q.prediction.fistReplayR10=true;kcd2vr::contact176::SweepScope scope(q.payload());ck(q.invoke()==0&&fh::receiptH31.failure==fh::FailureH31::OutsidePalm,"experimental replay retains original bound");}
 {ContactFixture q;kcd2vr::contact176::SweepScope scope(q.payload());{kcd2vr::contact176::SweepScope nested;ck(q.invoke()==0&&fh::receiptH31.failure==fh::FailureH31::OutsidePalm,"nested unrelated native query masks outer native shape");}
  ck(q.invoke()==1,"outer current native shape restored after nested call");

 }
 for(unsigned corruption=0;corruption<14;++corruption){ContactFixture q;auto p=q.payload();
  if(corruption==0)p.nativeFistShapeH31=false;if(corruption==1)p.redirected=false;if(corruption==2)++p.identity.player;
  if(corruption==3)++p.identity.collisions;if(corruption==4)++p.identity.generation;if(corruption==5)++p.identity.epoch;
  if(corruption==6)++p.identity.request;if(corruption==7)++p.identity.sample;if(corruption==8)++p.identity.tick;
  if(corruption==9)++p.wrapper;if(corruption==10)++p.action;if(corruption==11)++p.hand;if(corruption==12)++p.bone;
  if(corruption==13)q.prediction.fistReplayR10=true;kcd2vr::contact176::SweepScope scope(p);
  ck(q.invoke()==0&&fh::receiptH31.failure==fh::FailureH31::OutsidePalm,"native shape provenance mismatch retains original bound");}
 for(unsigned bad=0;bad<9;++bad){ContactFixture q;auto p=q.payload();kcd2vr::contact176::SweepScope scope(p);
  if(bad==0)g_prediction_fault120=true;if(bad==1)q.prediction.redirected=false;if(bad==2)++q.prediction.fistGenerationH31;
  if(bad==3)++q.prediction.fistGestureSampleH31;if(bad==4)q.prediction.hand=1;if(bad==5)q.prediction.collisions=nullptr;
  if(bad==6)++g_fist_epoch101;if(bad==7)tracking.right_grip=.6f;if(bad==8)put(q.state+0x38,4);
  ck(q.invoke()==0&&q.nativeCalls==0,"current action, source, squeeze and phase refusals retained");}
 for(unsigned bad=0;bad<5;++bad){ContactFixture q;if(bad==0)q.prediction.interpolations=0;if(bad==1)q.prediction.sweeps=0;
  if(bad==2)q.prediction.fistReplayR10=true;
  auto p=PredictionAuthority176(WhBase()+(bad==3?0xA7EB58:0xA7C9C1),q.prediction.collisions,0,417,bad==4?.06f:.011f);
  ck(!p.nativeFistShapeH31,"only actual current native predictor sweep grants geometric provenance");}

 std::printf("Native fist shape actual receiver: %u checks PASS; native geometry/result is a test leaf, not accepted damage\n",checks);
}
