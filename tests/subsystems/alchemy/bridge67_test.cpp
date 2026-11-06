#include "../../../src/subsystems/alchemy/bridge67.h"
#include <cassert>
#include <cstdio>
#include <limits>
namespace b=kcd2vr::subsystems::alchemy::bridge67;
namespace n=kcd2vr::subsystems::alchemy::near67;
namespace g=kcd2vr::subsystems::alchemy::geometry68;
namespace kcd2vr::frik_pose_probe {
static workstation_sample46::Frame observedFrame67{};static bool frameReadable67=true;
bool WorkstationFrame46(workstation_sample46::Frame& out) noexcept {out=observedFrame67;return frameReadable67;}
}
static unsigned checks=0;
static void check(bool v){++checks;if(!v)std::fprintf(stderr,"bridge67 check%u FAILED\n",checks);assert(v);}
static b::NativeTable table(std::uint64_t tick=1000){b::NativeTable t{};t.instance=1;t.player=2;t.playerEntity=9;t.lifetime=3;t.tick=tick;t.sequence=1;
 for(int i=0;i<21;++i)t.refs[i]={i,unsigned(100+i)};return t;}
static n::Context context(){return {1,2,3,4,5,6,3};}
static n::Completed copy(const b::NativeFrame& f){n::Completed c{};c.context=context();c.tick=f.table.tick;c.sequence=f.table.sequence;
 c.table=f.table.refs;c.offers=f.offers;c.count=21;c.complete=true;return c;}
static n::Input input(std::uint64_t tick){n::Input in{};in.context=context();in.now=tick;
 in.palms[0]={{2,0,0},tick,4,5,6,0,0,true};in.palms[1]={{0,0,0},tick,4,5,6,1,0,true};return in;}
static b::Control control(){b::Control c{};c.instance=1;c.parent=999;c.lifetime=3;c.tick=1002;c.poseEpoch=4;c.sourceEpoch=5;c.modeEpoch=6;
 c.entity=114;c.type=14;c.side=1;c.enabled=true;c.held[1]=true;return c;}
static g::Bounds bounds(double x=0){g::Bounds v{};for(int j=0;j<3;++j){v.local[j]=-.005f;v.local[j+3]=.005f;v.world[j*4+j]=1;}v.world[3]=float(x);return v;}
static void offer(const b::NativeTable& t,int slot,double x,std::uint64_t now){b::selectionBounds68(t,t.refs[slot],bounds(x),now);}
static void currentFrame(){auto& f=kcd2vr::frik_pose_probe::observedFrame67;f=kcd2vr::workstation_sample46::Frame{};f.sequence=10;f.tick=1002;f.timeNs=1002000000;
 f.poseEpoch=4;f.sourceEpoch=5;f.allowed=f.headValid=true;for(auto& h:f.hand){h.valid=true;h.grip=1;}kcd2vr::frik_pose_probe::frameReadable67=true;}
static b::NativeTool tool(){b::NativeTool t{};t.instance=1;t.parent=999;t.character=77;t.lifetime=3;t.tick=1002;t.poseEpoch=4;
 t.entity=114;t.side=1;t.type=14;t.joined=true;t.binding={1,2,77,88,114,1,14};return t;}
static void reset(){b::enabled.store(true);kcd2vr::minigame_hands45::modeEpoch67.store(6);kcd2vr::tracking59c6::epoch.store(5);currentFrame();b::table.publish({});b::completed.publish({});
 b::nomination.publish({});b::control.publish({});for(auto& m:b::offers)m.publish({});for(auto& m:b::tools)m.publish({});}
int main(){reset();auto t=table();bool nominated=true;
 // No native table or no nomination leaves the native head/gaze projection.
 check(b::selection(2,114,1000,nominated)==-1&&!nominated);
 b::finish(t);check(b::selection(2,114,1000,nominated)==-1&&!nominated);
 // Legacy point copying remains fixture-compatible but cannot authorize the
 // live selection seam. Actual geometry arrives from the native bounds call.
 b::observe(999,114,{.03,0,0},1001);b::observe(2,999,{.03,0,0},1001);
 b::observe(2,114,{std::numeric_limits<double>::quiet_NaN(),0,0},1001);
 b::NativeOffer offered{};b::offers[14].read(offered);check(!offered.offer.valid);
 b::observe(2,114,{.03,0,0},1001);b::observe(2,113,{.08,0,0},1001);
 offer(t,14,.03,1001);offer(t,13,.08,1001);
 t.tick=1002;++t.sequence;b::finish(t);b::NativeFrame nf{};check(b::completed.read(nf));
 check(nf.offers[14].valid&&nf.offers[14].boundsValid&&nf.offers[14].tick==1001&&std::abs(nf.offers[14].point.x-.03)<1e-7&&nf.offers[14].ref.entity==114);
 n::State owner{};auto ni=input(1002);auto c=copy(nf);auto d=owner.step(c,ni);
 check(d.source==n::Source::Left&&d.ref.entity==114&&d.hapticEdge);
 b::nomination.publish({ni.context,d,ni.now});nominated=false;
 check(b::selection(2,114,1002,nominated)==1&&nominated);
 nominated=true;check(b::selection(2,113,1002,nominated)==1&&!nominated); // one owner, other candidate rejected by host
 check(b::selection(999,114,1002,nominated)==-1&&!nominated);
 d=owner.step(c,ni);check(!d.hapticEdge); // no repeated hover feedback on the same source/ref
 ni.palms[1].grip=1;d=owner.step(c,ni);check(d.gripping&&!d.hapticEdge);
 ni.palms[1].point={3,0,0};ni.palms[0].point={.08,0,0};d=owner.step(c,ni);
 check(d.source==n::Source::Left&&d.ref.entity==114&&d.gripping); // no retarget while held
 b::nomination.publish({ni.context,d,ni.now});check(b::selection(2,114,1002,nominated)==-1&&!nominated); // latch is not a real attachment
 b::control.publish(control());b::tools[1].publish(tool());check(b::selection(2,114,1002,nominated)==1&&nominated);
 b::control.publish({});b::tools[1].publish({});
 ni.palms[1].point={0,0,0};d=owner.step(c,ni);b::nomination.publish({ni.context,d,ni.now});
 // Epoch/lifetime and current ref replacement revoke an old nomination.
 kcd2vr::minigame_hands45::modeEpoch67.store(7);check(b::selection(2,114,1002,nominated)==-1&&!nominated);
 kcd2vr::minigame_hands45::modeEpoch67.store(6);auto changed=t;changed.lifetime=4;b::finish(changed);
 check(b::selection(2,114,1002,nominated)==-1&&!nominated);b::finish(t);
 changed=t;changed.refs[14].entity=999;b::finish(changed);check(b::selection(2,114,1002,nominated)==-1&&!nominated);b::finish(t);
 check(b::selection(2,114,1083,nominated)==-1&&!nominated);
 // Re-finishing a later native frame never refreshes the original point age.
 t.tick=1083;++t.sequence;b::finish(t);b::completed.read(nf);check(!nf.offers[14].valid);
 // An unrelated table replacement also retires the arbiter's held topology.
 t=table(1002);t.sequence=2;b::finish(t);offer(t,14,.03,1002);b::finish(t);b::completed.read(nf);
 c=copy(nf);ni=input(1002);ni.palms[1].grip=1;owner={};owner.step(c,ni);
 ++c.table[20].entity;d=owner.step(c,ni);check(!d.gripping&&d.source==n::Source::Gaze);
 // Exact actual native binding pose/lifetime joins before any tool ownership.
 b::nomination.publish({});auto ctl=control();b::control.publish(ctl);b::finish(t);
 check(b::controllerOwns(1,114,1002,1,3,4));check(!b::controllerOwns(0,114,1002,1,3,4));
 check(!b::controllerOwns(1,113,1002,1,3,4));check(!b::controllerOwns(1,114,1002,2,3,4));
 check(!b::controllerOwns(1,114,1002,1,4,4));check(!b::controllerOwns(1,114,1002,1,3,5));
 check(!b::controllerOwns(1,114,1083,1,3,4));
 changed=t;changed.refs[14].entity=900;b::finish(changed);check(!b::controllerOwns(1,114,1002,1,3,4));b::finish(t);
 ctl.passenger=true;b::control.publish(ctl);check(!b::controllerOwns(1,114,1002,1,3,4)&&b::passenger(1002));
 ctl=control();ctl.type=2;ctl.entity=102;ctl.side=0;ctl.held[0]=ctl.held[1]=true;b::control.publish(ctl);
 check(b::controllerOwns(0,103,1002,1,3,4));check(b::controllerOwns(1,102,1002,1,3,4));
 check(!b::controllerOwns(0,102,1002,1,3,4));check(!b::controllerOwns(1,103,1002,1,3,4));
 ctl.held[1]=false;b::control.publish(ctl);check(!b::controllerOwns(1,102,1002,1,3,4));
 changed=t;changed.refs[2].entity=800;b::finish(changed);check(!b::controllerOwns(0,103,1002,1,3,4));
 b::enabled.store(false);check(!b::controllerOwns(0,103,1002,1,3,4));
 // Exercise the LIVE bridge endpoints with the real pure arbiter, actual
 // copied WORLD bounds, current copied XR epoch and no native calls.
 reset();t=table();b::finish(t);b::observe(2,114,{.03,0,0},1001);
 t.tick=1002;++t.sequence;b::finish(t);b::completed.read(nf);ni=input(1002);owner={};c=copy(nf);
 d=owner.step(c,ni);b::nomination.publish({ni.context,d,ni.now});std::uint32_t id=999;
 check(!b::selectionNear68(t,id,1002)&&id==0); // ray point is not native bounds
 offer(t,14,.03,1002);b::finish(t);b::completed.read(nf);c=copy(nf);owner={};
 d=owner.step(c,ni);b::nomination.publish({ni.context,d,ni.now});
 check(b::selectionNear68(t,id,1002)&&id==114);
 // A compatibility observation cannot erase the exact fresh bound.
 b::observe(2,114,{30,0,0},1002);b::offers[14].read(offered);check(offered.offer.boundsValid);
 // Native geometry is rechecked, rather than trusting a cached decision gap.
 offer(t,14,3,1002);check(!b::selectionNear68(t,id,1002));offer(t,14,.03,1002);
 auto& raw=kcd2vr::frik_pose_probe::observedFrame67;
 ++raw.poseEpoch;check(!b::selectionNear68(t,id,1002));--raw.poseEpoch;
 ++raw.sourceEpoch;check(!b::selectionNear68(t,id,1002));--raw.sourceEpoch;
 kcd2vr::tracking59c6::epoch.store(6);check(!b::selectionNear68(t,id,1002));kcd2vr::tracking59c6::epoch.store(5);
 raw.allowed=false;check(!b::selectionNear68(t,id,1002));raw.allowed=true;
 raw.hand[1].valid=false;check(!b::selectionNear68(t,id,1002));raw.hand[1].valid=true;
 raw.tick=921;check(!b::selectionNear68(t,id,1002));raw.tick=1002;
 kcd2vr::frik_pose_probe::frameReadable67=false;check(!b::selectionNear68(t,id,1002));kcd2vr::frik_pose_probe::frameReadable67=true;
 auto stale=d;stale.palm.tick=921;b::nomination.publish({ni.context,stale,ni.now});check(!b::selectionNear68(t,id,1002));
 stale=d;++stale.palm.sourceEpoch;b::nomination.publish({ni.context,stale,ni.now});check(!b::selectionNear68(t,id,1002));
 b::nomination.publish({ni.context,d,ni.now});changed=t;++changed.refs[20].entity;b::finish(changed);
 check(!b::selectionNear68(changed,id,1002));b::finish(t);
 // A newly captured bound cannot belong to a stale/foreign table scope.
 auto alien=t;++alien.player;b::selectionBounds68(alien,alien.refs[14],bounds(9),1002);
 check(b::selectionNear68(t,id,1002));alien=t;alien.tick=921;b::selectionBounds68(alien,alien.refs[14],bounds(9),1002);
 check(b::selectionNear68(t,id,1002));
 // Hover hysteresis retains the presentation ref to14cm but does not expand
 // actual native hand-selection admission beyond the10cm surface distance.
 ni.palms[1].point={-.10,0,0};d=owner.step(c,ni);check(d.source==n::Source::Left&&d.gap>.10);
 b::nomination.publish({ni.context,d,ni.now});check(!b::selectionNear68(t,id,1002));
 // A bare Grip latch cannot manufacture the actual held-tool exception.
 ni.palms[1].point={0,0,0};ni.palms[1].grip=1;d=owner.step(c,ni);check(d.gripping);
 ni.palms[1].point={3,0,0};d=owner.step(c,ni);b::nomination.publish({ni.context,d,ni.now});
 check(!b::selectionNear68(t,id,1002));b::control.publish(control());check(!b::selectionNear68(t,id,1002));
 auto actualTool=tool();b::tools[1].publish(actualTool);check(b::selectionNear68(t,id,1002)&&id==114);
 ++actualTool.binding.player;b::tools[1].publish(actualTool);check(!b::selectionNear68(t,id,1002));
 std::printf("alchemy bridge67: %u checks PASS (native bounds, current pose epochs, prior-frame nomination and exact tool bindings)\n",checks);
 return 0;
}
