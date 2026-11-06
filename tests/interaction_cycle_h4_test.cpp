#include "core/door_use46.h"
#include "core/native_door_motion66.h"
#include "diagnostics/interaction_cycle_h4.h"
#include <vector>
#include <string>
#include <cstdio>
namespace h=kcd2vr::diagnostics::interaction_cycle_h4;namespace cap=kcd2vr::diagnostics::capture168;namespace du=kcd2vr::door_use46;
static unsigned total=0,fail=0;static void ck(bool v,const char* why){++total;if(!v){++fail;std::printf("FAIL %s\n",why);}}
static std::vector<std::string> lines;static void sink(const char* s){lines.emplace_back(s);}static bool has(const char* s){for(const auto& l:lines)if(l.find(s)!=std::string::npos)return true;return false;}
static void drain(std::uint64_t n){lines.clear();h::service(n,sink);}
struct IO {
 std::uint64_t now=1000;unsigned publishes=0,retired=0;bool ackReady=false;kcd2vr::door_script19c4r4::Ack a{};kcd2vr::door_hold65::Hand hand{};
 bool control65(const kcd2vr::door19c4::Request&,kcd2vr::door_hold65::Hand& out){out=hand;return true;}
 bool ack(kcd2vr::door_script19c4r4::Ack& out){out=a;return ackReady;}
 bool publish(const kcd2vr::door_script19c4r4::Command&){++publishes;return true;}
 void retire66(const kcd2vr::door19c4::Request& r){if(r.serial)++retired;}
 void detach66(const kcd2vr::door19c4::Request&){}
 int drive66(const kcd2vr::door19c4::Request&,const kcd2vr::door_hold65::Hand&,float,bool,float& out){out=.9995f;return 1;}
 void receipt(const kcd2vr::door19c4::Request& r,const char* why,float f,bool v){h::observe(h::doorEvent(r,h::Stage::DoorProgress,now,why,f,v),now);}
};
int main(){
 cap::gate.select(cap::Domain::Interaction,true,1000);drain(1000);
 du::Input i{};i.now=i.poseTick=1000;i.available=i.held=i.scoped54=true;i.side=0;i.press=7;i.poseEpoch=11;i.trackingEpoch=12;i.sourceGeneration=13;i.player=14;i.experimentEpoch64=15;
 du::Selection s{};s.ordinary=true;s.target=42;s.side=0;s.tick=s.poseTick=1000;s.poseEpoch=11;s.sourceGeneration=13;s.player=14;s.distance=.1f;
 du::Controller c{};const auto plan=c.update(i,s);h::ProducerExtra x{};x.contextKnown=x.enabled=x.tracked=x.empty=x.rayPublished=true;x.grip=.8f;x.gripBlocked=1; // OTHER hand is UI-blocked
 h::observeProducer(i,s,plan,c,x,false);drain(1000);ck(!has("UI_GRIP_BLOCKED"),"other-hand left UI bit cannot reject selected right hand");ck(has("press=7")&&has("sourceTick=1000"),"actual Controller input original press and clock are copied");
 x.gripBlocked=2;i.now=1001;h::observeProducer(i,s,plan,c,x,false);drain(1001);ck(has("UI_GRIP_BLOCKED"),"selected right UI bit is diagnosed");
 x.gripBlocked=0;c.complete(plan,true);c.transport62(i,true,false);x.sentPulse=true;h::observeProducer(i,s,plan,c,x,true);drain(1001);ck(has("USE_PULSE_SENT_NOT_NATIVE_ACCEPTANCE"),"real Controller transport is not labelled native acceptance");
 ck(c.transportedSerial62>0,"fixture actually executes Controller transport62");
 h::Event req{};req.stage=h::Stage::ActionRequest;req.tick=1002;req.actionSeq=1;req.target=42;req.player=14;req.poseEpoch=11;req.side=0;h::observe(req,1002);
 auto ret=req;ret.stage=h::Stage::ActionReturn;ret.tick=1003;ret.action=0x1234;ret.code=1;h::observe(ret,1003);
 kcd2vr::door19c4::Request r{};r.serial=9;r.tick=1003;r.id=42;r.side=0;r.player=14;r.epoch=11;r.trackingEpoch46=12;r.sourceGeneration=13;r.inputPress65=7;r.inputSettingsEpoch46=1;r.mode69=2;
 auto admission=h::doorEvent(r,h::Stage::DoorAdmission,1003,"PUBLISHED",0,true);admission.actionSeq=1;h::observe(admission,1003);drain(1003);
 ck(has("actionSeq=1 door=9"),"published door joins explicit originating action sequence");
 kcd2vr::door_script19c4r4::MotionOwner66 owner;IO io{};io.hand.available=true;io.hand.settingsEpoch=1;io.hand.side=0;io.hand.player=14;io.hand.poseEpoch=11;io.hand.trackingEpoch=12;io.hand.sourceGeneration=13;io.hand.press=7;io.hand.grip=.8f;io.hand.tick=io.hand.poseTick=1003;io.now=1003;
 owner.tick(io,r,1003);ck(owner.pending&&!owner.active&&io.publishes==1,"actual MotionOwner waits for native Lua pause ACK");
 io.ackReady=true;io.now=1004;io.hand.tick=io.hand.poseTick=1004;io.a={2,9,1,0,1004};owner.tick(io,r,1004);drain(1004);ck(owner.active&&has("NATIVE_PAUSE_CONFIRMED"),"actual native ACK produces pause receipt");
 ck(h::consumer.cycles[0].accepted&&!h::consumer.cycles[0].terminal,"native action return and projected endpoint do not prove completion");
 io.now=1005;io.hand.tick=io.hand.poseTick=1005;io.a={4,9,1,.5f,1005};owner.tick(io,r,1005);drain(1005);ck(has("LUA_PROGRESS_READBACK")&&has("sourceTick=1003"),"original request clock is preserved through actual native progress");
 io.now=1006;io.hand.tick=io.hand.poseTick=1006;io.a={6,9,2,.9995f,1006};owner.tick(io,r,1006);drain(1006);ck(has("NATIVE_OPERATION_COMPLETED")&&!owner.active&&io.retired==1,"only actual matching state2 ACK completes native door operation");ck(h::consumer.cycles[0].terminal,"exact door progress terminal closes its diagnostic cycle");
 // Wrong owner/source cannot borrow the existing diagnostic link.
 auto wrong=h::doorEvent(r,h::Stage::DoorProgress,1007,"NATIVE_OPERATION_COMPLETED",1,true);wrong.player=99;h::observe(wrong,1007);drain(1007);ck(has("actionSeq=0 door=9"),"different player does not inherit original door/action link");
 // Unknown-hand idle is bounded and still carries coalescing counts.
 h::Recorder q;h::Event idle{};idle.stage=h::Stage::Grip;idle.tick=1010;idle.side=-1;ck(q.offer(idle,1010),"first unknown-hand input is recorded");for(unsigned n=1;n<=100;++n){idle.tick=1010+n;q.offer(idle,idle.tick);}ck(q.coalesced==100,"unknown-hand idle uses bounded heartbeat instead of burning event budget");
 // Verbose observations cannot occupy the reserved action/terminal capacity.
 h::Recorder full;h::Event detail{};detail.stage=h::Stage::Selector;detail.tick=1200;for(unsigned n=0;n<300;++n){detail.target=n+1;full.offer(detail,1200);}auto edge=detail;edge.stage=h::Stage::ActionReturn;ck(full.offer(edge,1200),"detail saturation preserves queue room for native action edge");ck(full.dropped>0,"dropped verbose evidence is counted");
 h::Event e{};unsigned pops=0;while(full.pop(e))++pops;ck(pops<=256,"queue capacity stays fixed");e.tick=1500;ck(!q.offer(e,1400)&&q.badClock>0,"future copied observation is refused without relabelling clock");
 // A target clip start is contextual evidence, never native action acceptance.
 auto second=req;second.tick=1300;second.actionSeq=2;h::observe(second,1300);auto clip=second;clip.stage=h::Stage::Clip;clip.code=1;h::observe(clip,1300);drain(1300);ck(!h::consumer.cycles[1].terminal&&!h::consumer.cycles[1].accepted,"clip start cannot close or accept a gameplay cycle");
 cap::gate.select(cap::Domain::Interaction,false,1400);drain(1400);ck(has("RECORDER_ENDED_BEFORE_COMPLETION")&&has("H29R4 F5_END "),"OFF explicitly closes incomplete evidence and emits health");
 ck(!h::observe(second,1400),"F5 off grants no logging lease");
 cap::gate.select(cap::Domain::Interaction,true,1500);auto fresh=req;fresh.tick=1500;fresh.actionSeq=3;h::observe(fresh,1500);drain(1500);bool open=false;for(auto& c:h::consumer.cycles)if(c.action==3&&c.session==cap::gate.session[1])open=!c.terminal;ck(open,"ending old capture does not terminate new session cycle");
 cap::gate.select(cap::Domain::Interaction,false,1501);drain(1501);ck(has("actionSeq=3"),"new-session incomplete cycle remains visible at its own end");
 // Original queued observations survive OFF with a finite essential end-family row.
 cap::gate.select(cap::Domain::Interaction,true,1600);auto queued=req;queued.tick=1600;queued.actionSeq=4;h::observe(queued,1600);cap::gate.select(cap::Domain::Interaction,false,1601);drain(1601);ck(has("H29R4 F5_END detail=QUEUED")&&has("actionSeq=4"),"queued original capture evidence survives OFF without a new logging lease");
 // Detail session budget is independent of the real native edge budget.
 cap::gate.select(cap::Domain::Interaction,true,1700);h::Recorder budget;detail.tick=1700;for(unsigned n=0;n<4300;++n){detail.target=n+1;budget.offer(detail,1700);while(budget.pop(e)){};}edge.tick=1700;ck(budget.offer(edge,1700)&&budget.budgetLost>0,"verbose session budget cannot exhaust reserved native action evidence");
 // Old explicit capture IDs may not be silently restamped into this session.
 auto stale=req;stale.tick=1700;stale.session=1;ck(!budget.offer(stale,1700),"old session payload cannot be retagged as current capture");
 cap::gate.select(cap::Domain::Interaction,false,1701);drain(1701);
 std::printf("interaction_cycle_h4=%u/%u nativeExecution=NONE\n",total-fail,total);return fail?1:0;
}
