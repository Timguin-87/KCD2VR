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
static unsigned checks=0;static void ck(bool yes,const char* why){++checks;if(!yes){std::printf("FAIL %s\n",why);std::fflush(stdout);std::abort();}}
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
// Only the body-solver's frame transport is a leaf here. The production
// commit order, contact qualification and native clock callback are extracted.
static bool publishFistFrameD53=true;
static void FistPublish101(const TrackingSnapshot&,const FistContactFrameH11& packet,const bool*){
 if(publishFistFrameD53)ck(g_fist_contact_h11.publish(packet),"solver frame transport");
}
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
 void commit(bool paired=true,bool reloading=false,bool sameWorld=true,bool publish=true){
  struct Transaction {
   bool reloadWorkingHandXR34=false;unsigned consumersD49=0;TrackingSnapshot tracking{};
   kcd2vr::passive169r1::Sample passive169r1[2]{};
   struct Target {kcd2vr::npc_contact172::Witness contact{};} passiveTargetsH21[2]{};
   std::uint64_t passiveQueryTickR11[2]{};
  } tx{};
  tx.reloadWorkingHandXR34=reloading;tx.passive169r1[0]=sample;tx.passiveTargetsH21[sample.side].contact=target;
  tx.passiveQueryTickR11[sample.side]=now;
  const auto packet=f;struct {bool handBlocked[2]{};} s{};
  const bool pairedFramePublishedR38=paired,contactWorldSame169r1=sameWorld;
  publishFistFrameD53=publish;
#include "fist_clock_r11_commit.inc"
  publishFistFrameD53=true;
 }
 void quiet(){tick();ck(writes==0,"refused clock writes none");}
};
static double runTimeline(double contactMs,double forwardMs,unsigned budget,float dt){
 Fixture q;fc::maxHoldMs=budget;double elapsed=0;bool contactSent=false;
 for(unsigned n=0;n<200;++n){now=1000+std::uint64_t(elapsed+.5);tracking.tick=now;q.f.tick=now;++q.f.sample;q.publishFrame();q.sample.sequence=q.f.sample;
  if(elapsed<=forwardMs){FistPeakR10 peak{};peak.side=0;peak.peak.valid=true;peak.peak.gesture=q.binding.intent.sample;peak.peak.tick=now;peak.palms=q.f;ck(FistPeakMailboxR10(0).publish(peak),"forward evidence publishes");}
  if(!contactSent&&contactMs>=0&&elapsed>=contactMs){q.contact();contactSent=true;}
  q.tick(dt);
  float rate=0,scaled=0,plain=0;ac50::Record rows[3]{};int phase=0;
  ClockRead50(q.clock+0x10,&rate,4);ClockRead50(q.clock+0x50,&scaled,4);ClockRead50(q.clock+0x54,&plain,4);ClockRead50(q.records,rows,sizeof(rows));ClockRead50(q.state+0x38,&phase,4);
  scaled+=dt*rate;plain+=dt;for(auto& row:rows){row.left=std::max(0.f,row.left-dt);row.scaledLeft=std::max(0.f,row.scaledLeft-dt*rate);}
  put(q.clock+0x50,scaled);put(q.clock+0x54,plain);put(q.records,rows);if(phase==4&&rows[0].left<=0)put(q.state+0x38,8);
  elapsed+=double(dt)*1000.;if(rows[1].left<=0)return elapsed;
 }
 return -1;
}
int main(){
 {Fixture q;++q.f.sample;q.sample.sequence=q.f.sample;
  q.contact();ck(!FistTouchedR11(q.binding,q.f.character,now),"previous-frame mailbox rejects the new committed contact");
  q.commit();ck(FistTouchedR11(q.binding,q.f.character,now),"actual producer publishes current fist before its contact");
  q.tick();ck(writes==8&&fc::lastWhy==1,"committed contact reaches native catch-up without a second damage request");}
 for(int denied=0;denied<4;++denied){Fixture q;++q.f.sample;q.sample.sequence=q.f.sample;
  q.commit(denied!=0,denied==1,denied!=2,denied!=3);
  ck(!FistTouchedR11(q.binding,q.f.character,now),"no contact from failed pair, reload hand, changed WORLD, or unavailable current mailbox");}
 {Fixture q;const auto before=memory;q.tick();ck(writes==0&&memory==before&&fc::lastWhy==9,"travelling admitted fist leaves native time progressing");}
 {Fixture q;q.contact();ck(FistTouchedR11(q.binding,q.f.character,now),"actual publisher joins exact binding");q.tick();ck(writes==8&&fc::lastWhy==1,"contact invokes native clock catch-up");ck(q.get(q.records+0xC)==0&&q.get(q.records+0x14+0xC)<.0131,"original next native step crosses prepare without writing phase");int phase=0;ClockRead50(q.state+0x38,&phase,4);ck(phase==4,"no phase write");}
 {Fixture q;++g_fist_epoch101;q.quiet();}{Fixture q;++g_pose_epoch6689;q.quiet();}{Fixture q;++g_attack_timing_policy22;q.quiet();}
 {Fixture q;g_ctx_menu=true;q.quiet();}{Fixture q;diagnostics::lab::native=true;q.quiet();}{Fixture q;animation_filter::required=true;q.quiet();}{Fixture q;retail_build112::ready=false;q.quiet();}
 {Fixture q;empty.empty=false;q.quiet();}{Fixture q;tracking.right_grip=.6f;q.quiet();}{Fixture q;tracking.right_trigger=.6f;q.quiet();}{Fixture q;tracking.right_held=true;q.quiet();}{Fixture q;tracking.tick=939;q.quiet();}
 {Fixture q;blocked=true;q.quiet();}{Fixture q;q.f.request=4;q.publishFrame();q.quiet();}{Fixture q;q.f.gestureSampleH31[0]++;q.publishFrame();q.quiet();}
 {Fixture q;put(q.action+0x28,(unsigned char)0);q.quiet();}{Fixture q;put(q.clock+0x14,(unsigned char)0);q.quiet();}{Fixture q;put(q.state+0x1B8,1);q.quiet();}{Fixture q;put(q.gate+0x28,(unsigned char)1);q.quiet();}
 {Fixture q;put(q.slots,std::uintptr_t(0xDEAD));q.quiet();}{Fixture q;put(q.state+0x38,16);q.quiet();}{Fixture q;put(q.type+8,4);q.quiet();}
 {Fixture q;q.contact(939);ck(!FistTouchedR11(q.binding,q.f.character,now),"old query cannot refresh contact age");}
 {Fixture q;q.contact(1001);ck(!FistTouchedR11(q.binding,q.f.character,now),"future query rejected");}
 {Fixture q;q.f.gestureDirectionH31[0]={0,0,0};q.publishFrame();q.contact();ck(!FistTouchedR11(q.binding,q.f.character,now),"missing travel direction rejected");}
 {Fixture q;q.contact();fc::Contact got{};ck(g_fist_contact_r11[0].read(got)&&got.tick==1000,"original query timestamp");now=1010;q.sample.point={0,.31,1};q.target.entityId=88;q.contact();ck(g_fist_contact_r11[0].read(got)&&got.entity==77&&got.tick==1000,"first contact cannot retarget or refresh");}
 {Fixture q;q.contact();auto b=q.binding;b.intent.request++;ck(!FistTouchedR11(b,q.f.character,now),"request change rejects old contact");b=q.binding;b.intent.player++;ck(!FistTouchedR11(b,q.f.character,now),"player change rejects old contact");ck(!FistTouchedR11(q.binding,q.f.character+1,now),"skeleton owner change rejects old contact");}
 {Fixture q;fc::enabled=false;q.contact();q.quiet();ck(!FistTouchedR11(q.binding,q.f.character,now),"feature OFF no publisher or writer");}
 {Fixture q;q.contact();auto before=memory;failWrite=3;q.tick();ck(g_motion_clock_fault50&&memory==before,"partial contact write rolls back and disables clock");}
 for(float dt:{.008f,.011f,.016f,.022f}){
  const double native=runTimeline(-1,0,0,dt);
  const double advancing=runTimeline(330,330,500,dt);ck(advancing==native,"current forward motion no longer stalls native hit window");
  const double legacyBudget=runTimeline(-1,2000,1000,dt);ck(legacyBudget==native,"legacy hold setting cannot restore a one-second strike delay");
  const double catchUp=runTimeline(22,200,500,dt);ck(catchUp<native&&catchUp>=22,"real current contact still catches up before natural peak");
 }
 {Fixture q;q.f.previous[0]={0,.4,1};q.f.palm[0]={0,.3,1};q.publishFrame();q.sample.point=q.f.palm[0];q.contact();
  ck(FistTouchedR11(q.binding,q.f.character,now),"curved current contact is not rejected by old onset direction");q.tick();ck(writes==8&&fc::lastWhy==1,"curved current contact catches up existing native action");}
 {Fixture q;q.tick(.2f);ck(writes==0,"large hitch leaves native clock untouched");}
 std::printf("R11 actual publisher and clock callback: %u checks PASS; native memory/owner leaves mocked, clock and identity code actual\n",checks);
}
