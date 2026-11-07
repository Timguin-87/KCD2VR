// Actual station pad adapter -> copied command -> staged shared frame -> camera.
// Native activity and tracking publications are explicit inputs. No native
// player movement, renderer, collision mesh, or headset execution is claimed.
#include "vrik/semi_workstations46.h"
#include "vrik/tracked_frame51.h"
#include <cstdio>
#include <cstdlib>
#include <limits>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace sr=kcd2vr::station_reposition_d53;
namespace tf=kcd2vr::tracked_frame51;
namespace pm=kcd2vr::pose_core;
namespace sw=kcd2vr::semi_workstations46;
namespace ai=kcd2vr::activity_input163;
namespace mh=kcd2vr::minigame_hands45;
namespace ws=kcd2vr::workstation178r20c1;
static unsigned checks=0;
static void ck(bool b,const char* text){++checks;if(!b){std::fprintf(stderr,"FAIL %u: %s\n",checks,text);std::exit(1);}}
static bool close(pm::Vec3 a,pm::Vec3 b,double e=1e-8){return pm::length(a-b)<e;}
struct Pad {bool a=false,b=false,x=false;float lx=.7f,ly=.8f,lt=0,rt=0;};
struct Fixture {
 sw::State sender{};sw::Context c{};kcd2vr::workstation_sample46::Frame tracking{};
 tf::Input fi{};tf::State state{};tf::Frame frame{};tf::EyeState eye{};sr::Input move{};Pad pad{};
 Fixture(unsigned kind=mh::Alchemy){
  auto& w=ai::witness175;w.ready=true;w.transitionDepth175=0;w.Clear();
  const auto nativeKind=kind==mh::Alchemy?ai::Kind175::Alchemy:ai::Kind175::Smithing;
  w.Begin(nativeKind,0x2000);mh::setMode(kind,3);kcd2vr::combat_rules178r10::workstationEnabled=true;
  ws::alchemy_book_as1::enabled=true;kcd2vr::alchemy_semi46::hooksReady=true;
  c.enabled=c.qualified=c.moveQualifiedD53=c.alchemyTracked=c.smithTracked=true;
  if(kind==mh::Alchemy)c.alchemy=0x2000;else{c.smith=0x2000;c.smithPhase=8;}
  c.lifetime=w.lifetime175.load();
  tracking.sequence=10;tracking.tick=1000;tracking.timeNs=1000000000;tracking.poseEpoch=7;tracking.sourceEpoch=9;
  tracking.allowed=tracking.headValid=true;tracking.head[1]=1.7f;
  for(auto& h:tracking.hand){h.valid=true;h.position[1]=1.2f;}
  fi.stamp={7,10,1000,0x100};fi.control=2;fi.timeNs=tracking.timeNs;fi.allowed=fi.hmdHeight=true;
  fi.nativeHead=fi.restHead=fi.calibratedHead={0,0,1.7};fi.nativePelvis=fi.restPelvis={0,0,.9};
  auto neutral=fi;neutral.eyeNeutral=true;tf::Frame n{};tf::State ns{};pm::Vec3 eyeOut{};
  ck(tf::advance(ns,neutral,n),"ordinary shared frame exists before station entry");n.root={10,20,0};
  ck(tf::eye(eye,n,n.stamp,n.control,1000,{10,20,1.756},eyeOut),"actual eye adapter learns prior neutral anatomy");
  move.forward={0,1,0};move.active=true;
  send(0,0);solve();
 }
 unsigned kind()const{return c.alchemy?unsigned(mh::Alchemy):unsigned(mh::Smithing);}
 void next(unsigned ms=20){++tracking.sequence;tracking.tick+=ms;tracking.timeNs+=std::uint64_t(ms)*1000000;}
 void send(float x,float y,bool publishKind=true){
  if(publishKind)mh::publish(kind(),tracking.tick);
  c.lifetime=ai::witness175.lifetime175.load();c.moveXD53=x;c.moveYD53=y;
  pad=Pad{};sender.apply(c,tracking,tracking.tick,pad);
  ck(sr::command.read(move.command),"real final pad adapter publishes a copied command");
  ws::WorkstationOwnerH51 o{};const bool owner=ws::workstationOwnerH51(tracking.tick,o);
  fi.stamp={tracking.poseEpoch,tracking.sequence,tracking.tick,0x100};fi.timeNs=tracking.timeNs;
  fi.workstationKindH51=owner?o.kind:0;fi.workstationModeEpochH51=o.modeEpoch;
  fi.workstationLifetimeH51=o.lifetime;fi.workstationInstanceH51=o.instance;
  move.owner={0x100,o.instance,tracking.poseEpoch,tracking.sourceEpoch,fi.control,o.modeEpoch,o.stationLifetimeH46,o.kind};
  move.sample=tracking.sequence;move.tick=move.now=tracking.tick;move.scale=fi.scale;
 }
 void solve(){ck(tf::advance_station_d53(state,fi,move,frame),"actual shared-frame station adapter completes");frame.root={10,20,0};}
 void step(float x,float y,unsigned ms=20){next(ms);send(x,y);solve();}
};
int main(){
 for(unsigned kind:{unsigned(mh::Alchemy),unsigned(mh::Smithing)}){
  Fixture f(kind);f.step(1,0);
  ck(f.move.command.enabled&&!f.pad.lx&&!f.pad.ly,"raw mode3 movement is reserved from native workpiece axes");
  ck(close(f.frame.stationOffsetD53,{.006,0,0}),"fresh 20ms raw command moves the shared presentation by 6mm");
  ck(close(f.frame.anchor-f.fi.restHead,f.frame.stationOffsetD53)&&close(f.frame.bodyOffset,f.frame.stationOffsetD53),
   "camera anchor and pelvis compensation carry exactly one identical displacement");
  pm::Vec3 eye{};ck(tf::eye(f.eye,f.frame,f.frame.stamp,f.frame.control,f.move.now,{10,20,1.756},eye)&&
   close(eye,{10.006,20,1.756}),"actual camera adapter applies the same station displacement once");
  const auto offset=f.frame.stationOffsetD53;f.solve();
  ck(close(f.frame.stationOffsetD53,offset)&&close(f.frame.anchor,{.006,0,1.7}),"duplicate native callback never integrates or stacks another offset");
  f.fi.nativeHead.y=.2;f.solve();
  ck(close(f.frame.anchor,{.006,0,1.7}),"same-XR native head animation cannot double-apply station movement");
  ++f.fi.stamp.sample;++f.move.sample;f.fi.stamp.tick+=5;f.move.tick=f.move.now=f.fi.stamp.tick;
  f.solve();ck(close(f.frame.stationOffsetD53,offset),"new body callback cannot spend the same input command twice");
 }
 {
  Fixture f;for(int n=0;n<100;++n)f.step(1,1);
  ck(std::fabs(pm::length(f.frame.stationOffsetD53)-sr::kRadiusM)<1e-8,"diagonal travel stops at the 40cm radius, not a square or faster diagonal");
  const auto held=f.frame.stationOffsetD53;for(int n=0;n<25;++n)f.step(.1f,-.1f);
  ck(close(held,f.frame.stationOffsetD53),"radial deadzone causes no stationary drift");
  f.step(-1,-1);ck(pm::length(f.frame.stationOffsetD53)<sr::kRadiusM,"opposite input immediately withdraws from the limit");
 }
 {
  Fixture f;f.move.forward={1,0,0};f.step(0,1);
  ck(close(f.frame.stationOffsetD53,{.006,0,0}),"forward input follows the current horizontal head direction");
  f.step(1,0);ck(close(f.frame.stationOffsetD53,{.006,-.006,0}),"right strafe stays perpendicular to head-relative forward");
  const auto offset=f.frame.stationOffsetD53;f.next(100);f.send(1,0);f.move.command.tick-=100;f.move.command.timeNs-=100000000;f.solve();
  ck(close(f.frame.stationOffsetD53,offset),"stale copied input retains the location but earns zero movement");
  f.step(1,0);ck(close(f.frame.stationOffsetD53,offset),"input recovery seeds without replaying an unobserved interval");
  f.step(1,0);ck(f.frame.stationOffsetD53.y<offset.y,"a later distinct fresh command resumes bounded movement");
 }
 {
  Fixture f;f.step(0,1);const auto previous=f.frame.stationOffsetD53;const auto oldGlobal=f.c.lifetime;
  ai::witness175.Begin(ai::Kind175::Reading,0x3000);mh::publish(mh::None,f.tracking.tick);
  f.c.qualified=false;f.next();f.send(0,0,false);f.solve();
  ck(f.c.lifetime!=oldGlobal&&f.move.command.enabled&&close(previous,f.frame.stationOffsetD53),
   "nested recipe book preserves exact station position despite global activity/input-witness changes");
  ck(sr::pageStick(true,1,0)==0&&sr::pageStick(true,1,-.7f)==-.7f&&sr::pageStick(false,1,-.7f)==1,
   "left reposition input cannot also become native page input; right stick and ordinary reading retain their routes");
 }
 {
  Fixture f;f.step(1,0);const auto held=f.frame.stationOffsetD53;
  // Ordinary turn is an entity-world transform, not the calibrated tracking
  // basis. Changing the root must carry both eye and body by the same offset.
  f.step(0,0);f.frame.rotation={0,0,.7071067811865476,.7071067811865476};
  pm::Vec3 eye{};
  ck(close(held,f.frame.stationOffsetD53)&&
   tf::eye(f.eye,f.frame,f.frame.stamp,f.frame.control,f.move.now,{10,20,1.756},eye)&&
   close(eye,{10,20.006,1.756}),
   "world yaw preserves the station visit and rotates shared eye/body displacement exactly once");
  f.move.forward={1,0,0};f.step(0,1);
  ck(close(f.frame.stationOffsetD53,{.012,0,0}),
   "current HMD heading changes the next motion direction without erasing prior movement");
  ai::witness175.Begin(ai::Kind175::Reading,0x3000);mh::publish(mh::None,f.tracking.tick);
  f.c.qualified=false;f.next();f.send(0,1,false);f.solve();
  ck(close(f.frame.stationOffsetD53,{.018,0,0}),
   "move after a turn remains live inside the nested recipe book with the same station owner");
 }
 for(int fault=0;fault<12;++fault){
  Fixture f;f.step(1,0);f.next();f.send(1,0);
  switch(fault){
   case 0:++f.fi.stamp.actor;++f.move.owner.character;break;
   case 1:++f.fi.stamp.epoch;++f.move.owner.pose;++f.move.command.pose;break;
   case 2:++f.move.owner.tracking;++f.move.command.tracking;break;
   case 3:++f.fi.control;++f.move.owner.control;break;
   case 4:++f.fi.workstationModeEpochH51;++f.move.owner.mode;++f.move.command.mode;break;
   case 5:++f.move.owner.stationLifetime;++f.move.command.stationLifetime;break;
   case 6:++f.fi.workstationInstanceH51;++f.move.owner.instance;++f.move.command.instance;break;
   case 7:f.fi.workstationKindH51=f.move.owner.kind=f.move.command.kind=mh::Smithing;break;
   case 8:f.move.trackingBasis={0,0,.7071067811865476,.7071067811865476};break;
   case 9:f.move.scale=f.fi.scale=2;break;
   case 10:f.fi.workstationKindH51=0;break;
   case 11:f.move.active=false;break;
  }
  f.solve();ck(close(f.frame.stationOffsetD53,{}),"actor/pose/tracking/control/mode/station/basis/scale changes or exit cannot borrow old displacement");
 }
 {
  Fixture f;const auto committed=f.state;f.next();f.send(1,0);
  auto rejected=committed;tf::Frame candidate{};
  ck(tf::advance_station_d53(rejected,f.fi,f.move,candidate)&&candidate.stationOffsetD53.x>.005,
   "candidate solve stages virtual movement");
  ck(close(committed.stationMoveD53.offset,{})&&committed.stationMoveD53.sample==10,
   "discarded transaction cannot advance committed movement or input credit");
  auto accepted=committed;ck(tf::advance_station_d53(accepted,f.fi,f.move,candidate)&&std::fabs(candidate.stationOffsetD53.x-.006)<1e-8,
   "retry from committed state admits precisely one movement step");
  f.c.moveQualifiedD53=false;f.send(1,0);
  ck(!f.move.command.enabled,"menu/system/context revocation clears the input mailbox immediately");
  f.sender.revoke(f.tracking.tick);sr::Command empty{};ck(sr::command.read(empty)&&!empty.enabled,"explicit input revocation leaves no queued station movement");
 }
 for(int mode:{0,1,2}){
  Fixture f;mh::setMode(mh::Alchemy,mode);f.next();f.send(1,0);
  ck(!f.move.command.enabled&&f.pad.lx==.7f&&f.pad.ly==.8f,"native/semi/free modes preserve native stick policy");
 }
 std::printf("station_reposition_d53: %u checks PASS; actual pad/shared-frame/camera, no native player teleport\n",checks);
}
