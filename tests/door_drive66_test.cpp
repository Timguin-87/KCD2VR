#include "../src/core/native_door19c4.h"
#include "../src/core/door_hold65.h"
#include "../src/core/door_path66.h"
#include "../src/core/snapshot_mailbox.h"
#include "../src/vrik/physical_weapon.h"
#include <cassert>
#include <cstdio>
#include <limits>
namespace pc=kcd2vr::pose_core;namespace ph=kcd2vr::physical;namespace door4=kcd2vr::door19c4;
struct V3 {float x=0,y=0,z=0;};struct Q4 {float x=0,y=0,z=0,w=1;};
struct QuatT28 {float q[4]{0,0,0,1};float t[3]{};};
static pc::Vec3 PV(V3 v){return {v.x,v.y,v.z};}static pc::Quat PQ(Q4 q){return {q.x,q.y,q.z,q.w};}
static V3 FromT(const QuatT28& p){return {p.t[0],p.t[1],p.t[2]};}
static Q4 QFromQuatT(const QuatT28& p){return {p.q[0],p.q[1],p.q[2],p.q[3]};}
static Q4 QNormalize(Q4 q){const auto n=pc::unit(PQ(q));return n?Q4{float(n->x),float(n->y),float(n->z),float(n->w)}:Q4{0,0,0,0};}
static Q4 QMul(Q4 a,Q4 b){const auto q=pc::multiply(PQ(a),PQ(b));return {float(q.x),float(q.y),float(q.z),float(q.w)};}
static Q4 QConj(Q4 q){return {-q.x,-q.y,-q.z,q.w};}
static V3 QRotateV(Q4 q,V3 p){const auto v=pc::rotate(PQ(q),PV(p));return {float(v.x),float(v.y),float(v.z)};}
static V3 Add(V3 a,V3 b){return {a.x+b.x,a.y+b.y,a.z+b.z};}static V3 Sub(V3 a,V3 b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
static V3 Mul(V3 a,float k){return {a.x*k,a.y*k,a.z*k};}
static void QToQuatT(QuatT28& p,Q4 q){p.q[0]=q.x;p.q[1]=q.y;p.q[2]=q.z;p.q[3]=q.w;}
static void ToT(QuatT28& p,V3 v){p.t[0]=v.x;p.t[1]=v.y;p.t[2]=v.z;}
struct CommittedFrame6695 {
 std::uintptr_t doorPlayer38=0;pc::Stamp stamp{};std::uint64_t doorTrackingEpoch66=0;
 bool valid[2]{},propRequestedValid5916c1[2]{},palmValid59c9[2]{};
 QuatT28 propRequested5916c1[2]{},allowed[2]{};V3 palm59c9[2]{};float world[8]{0,0,0,1,0,0,0,1};
};
static std::uint64_t now66=1000;static std::uint64_t GetTickCount64(){return now66;}
static CommittedFrame6695 completed{};static bool packetAvailable=true;static unsigned packetReads=0;
static bool ReadCommittedPacket6695(CommittedFrame6695& p){++packetReads;p=completed;return packetAvailable;}
static std::atomic<std::uint64_t> g_door_live_serial66{0};
#include "door_drive66.inc"
static unsigned checks=0;static void check(bool b){++checks;if(!b)std::fprintf(stderr,"door_drive66 check%u FAILED\n",checks);assert(b);}
static bool near(double a,double b,double e=1e-5){return std::abs(a-b)<e;}
static door4::Request request(){door4::Request r{};r.serial=11;r.player=12;r.entity=13;r.id=14;r.side=0;r.epoch=15;
 r.trackingEpoch46=16;r.sourceGeneration=17;r.inputSettingsEpoch46=18;r.inputExperimentEpoch65=19;r.inputPress65=20;return r;}
static kcd2vr::door_hold65::Hand hand(const door4::Request& r){kcd2vr::door_hold65::Hand h{};h.player=r.player;h.side=r.side;
 h.poseEpoch=r.epoch;h.trackingEpoch=r.trackingEpoch46;h.press=r.inputPress65;h.sourceGeneration=r.sourceGeneration;
 h.settingsEpoch=r.inputSettingsEpoch46;h.experimentEpoch=r.inputExperimentEpoch65;h.tick=h.poseTick=now66;h.grip=1;h.available=true;return h;}
static DoorCurveFrame66 frame(const door4::Request& r,float actual=.05f){DoorCurveFrame66 f{};f.request=r;f.tick=now66;f.time=actual;
 auto& s=f.path.source;s.operation=r.serial;s.player=r.player;s.entity=r.entity;s.target=r.id;s.side=r.side;s.press=r.inputPress65;
 s.poseEpoch=r.epoch;s.trackingEpoch=r.trackingEpoch46;s.generation=r.sourceGeneration;s.settingsEpoch=r.inputSettingsEpoch46;
 s.experimentEpoch=r.inputExperimentEpoch65;s.clip=21;s.topology=22;s.curve=23;
 for(unsigned i=0;i<33;++i){const double p=double(i)/32;f.path.samples[i]={p,{{p,0,0},{}}};}return f;}
static void packet(const door4::Request& r,double palmPhase, double displayed=99){completed={};completed.doorPlayer38=r.player;
 completed.stamp={r.epoch,1,now66,555};completed.doorTrackingEpoch66=r.trackingEpoch46;
 completed.valid[r.side]=completed.propRequestedValid5916c1[r.side]=completed.palmValid59c9[r.side]=true;
 completed.propRequested5916c1[r.side].t[0]=float(palmPhase-.05);
 completed.allowed[r.side].t[0]=float(displayed);completed.palm59c9[r.side]={float(displayed+.05),0,0};}
static void reset(){g_door_curve66.publish({});g_door_display66.publish({});g_door_native_grip66.publish({});
 g_door_projection66={};g_door_wrist_offset66={};g_door_live_serial66=0;
 now66=1000;completed={};packetAvailable=true;packetReads=0;}
int main(){auto r=request();auto h=hand(r);float out=-1;reset();auto f=frame(r);g_door_curve66.publish(f);packet(r,.05);
 check(DriveDoor66(r,h,.05f,true,out)==1&&near(out,.05));
 DoorDisplay66 display{};check(g_door_display66.read(display));check(near(display.wrist.fixed_offset.x,0));
 // Native Lua getter is pre-seek .05; native ACK readback is post-seek .30.
 // A current all-phase curve remains valid without comparing these two clocks.
 now66+=10;f.tick=now66;f.time=.05f;g_door_curve66.publish(f);packet(r,.30);h.tick=h.poseTick=now66;
 check(DriveDoor66(r,h,.30f,false,out)==1&&near(out,.30));
 check(g_door_display66.read(display));check(near(display.handle.x,.30)&&near(display.wrist.fixed_offset.x,.25));
 // Display/allowed hand is deliberately far from raw intent. Its local palm
 // offset, rather than that displayed endpoint, is used in the actual adapter.
 now66+=10;f.tick=now66;g_door_curve66.publish(f);packet(r,.40,-100);h.tick=h.poseTick=now66;
 check(DriveDoor66(r,h,.30f,false,out)==1&&near(out,.40));
 now66+=10;f.tick=now66;g_door_curve66.publish(f);packet(r,.40,123);h.tick=h.poseTick=now66;
 check(DriveDoor66(r,h,.40f,false,out)==1&&near(out,.40));
 now66+=10;f.tick=now66;g_door_curve66.publish(f);packet(r,.20);h.tick=h.poseTick=now66;
 check(DriveDoor66(r,h,.40f,false,out)==1&&near(out,.20));
 check(g_door_display66.read(display)&&near(display.handle.x,.40)); // achieved native handle, not requested .20
 // Repeated native callbacks in the same millisecond cannot advance the clock.
 packet(r,.30);check(DriveDoor66(r,h,.40f,false,out)==1&&near(out,.20));
 DetachDoor66(r);check(g_door_display66.read(display)&&display.request.serial==0&&!g_door_projection66.active);
 // A separately accepted same-door regrip starts from actual native phase and
 // retains its own offset. It does not continue the old hand's displacement.
 now66+=10;f.tick=now66;g_door_curve66.publish(f);h.press=100;packet(r,.26);
 check(DriveDoor66(r,h,.20f,true,out)==1&&near(out,.20));check(g_door_projection66.source.press==100);
 now66+=10;f.tick=now66;g_door_curve66.publish(f);packet(r,.36);
 check(DriveDoor66(r,h,.20f,false,out)==1&&near(out,.30));
 // Current model-world movement: Henry packet and Door model move together.
 reset();f=frame(r);f.world.p={10,20,3};g_door_curve66.publish(f);packet(r,.2);
 completed.world[4]=10;completed.world[5]=20;completed.world[6]=3;
 check(DriveDoor66(r,hand(r),.2f,true,out)==1);check(g_door_display66.read(display));
 check(near(display.handle.x,10.2)&&near(display.wrist.fixed_offset.y,20));
 reset();f=frame(r);const double half=.7071067811865476;f.world={{10,20,3},{0,0,half,half}};
 g_door_curve66.publish(f);packet(r,.2);completed.world[2]=completed.world[3]=float(half);
 completed.world[4]=10;completed.world[5]=20;completed.world[6]=3;
 check(DriveDoor66(r,hand(r),.2f,true,out)==1);check(g_door_display66.read(display));
 check(near(display.handle.x,10)&&near(display.handle.y,20.2)&&near(display.wrist.fixed_offset.y,20.15));
 // The initial wrist offset rotates with the curved native leaf. Display uses
 // the achieved native phase, while the next controller phase is independent.
 reset();f=frame(r);for(auto& sample:f.path.samples){const double a=sample.phase*1.5707963267948966;
  sample.handle={{std::cos(a),std::sin(a),1},{0,0,std::sin(a*.5),std::cos(a*.5)}};}
 g_door_curve66.publish(f);packet(r,1);completed.propRequested5916c1[0].t[2]=1;
 check(DriveDoor66(r,hand(r),0,true,out)==1);
 now66+=10;f.tick=now66;g_door_curve66.publish(f);const double a=.75*1.5707963267948966;
 packet(r,std::cos(a));completed.propRequested5916c1[0].t[1]=float(std::sin(a));completed.propRequested5916c1[0].t[2]=1;
 check(DriveDoor66(r,hand(r),.5f,false,out)==1&&near(out,.75,2e-4));check(g_door_display66.read(display));
 check(near(display.wrist.fixed_offset.x,half*.95)&&near(display.wrist.fixed_offset.y,half*.95));
 check(near(display.wrist.rotation.z,std::sin(3.141592653589793/8)));
 for(unsigned fault=0;fault<11;++fault){reset();f=frame(r);g_door_curve66.publish(f);packet(r,.2);h=hand(r);
  switch(fault){case 0:packetAvailable=false;break;case 1:++completed.doorPlayer38;break;case 2:++completed.stamp.epoch;break;
  case 3:++completed.doorTrackingEpoch66;break;case 4:completed.valid[0]=false;break;case 5:completed.propRequestedValid5916c1[0]=false;break;
  case 6:completed.palmValid59c9[0]=false;break;case 7:completed.stamp.tick=now66-81;break;case 8:++h.poseEpoch;break;
  case 9:completed.propRequested5916c1[0].t[0]=std::numeric_limits<float>::quiet_NaN();break;
  case 10:f.request.entity++;g_door_curve66.publish(f);break;}
  check(DriveDoor66(r,h,.2f,true,out)==-1);}
 reset();f=frame(r);f.tick=now66-81;g_door_curve66.publish(f);packet(r,.2);check(DriveDoor66(r,hand(r),.2f,true,out)==0&&packetReads==0);
 reset();f=frame(r);++f.request.serial;g_door_curve66.publish(f);packet(r,.2);check(DriveDoor66(r,hand(r),.2f,true,out)==0&&packetReads==0);
 // Real clips have flat leading/trailing intervals. There is no requirement
 // to pull through their stationary native time tail, but hold still must not
 // change even a phase that happens to lie inside a flat leading interval.
 reset();f=frame(r);for(auto& sample:f.path.samples)sample.handle.p.x=std::clamp((sample.phase-.25)*2.,0.,1.);
 g_door_curve66.publish(f);packet(r,0);h=hand(r);
 check(DriveDoor66(r,h,.125f,true,out)==1&&near(out,.125));
 now66+=10;f.tick=now66;g_door_curve66.publish(f);packet(r,0);
 check(DriveDoor66(r,h,.125f,false,out)==1&&near(out,.125));
 now66+=10;f.tick=now66;g_door_curve66.publish(f);packet(r,1);
 check(DriveDoor66(r,h,.125f,false,out)==1&&near(out,.9995));
 check(DriveDoor66(r,h,.125f,false,out)==1&&near(out,.9995)); // repeated eye/native callback; ACK still old
 now66+=10;f.tick=now66;g_door_curve66.publish(f);packet(r,1);
 check(DriveDoor66(r,h,.9995f,false,out)==1&&near(out,.9995));
 now66+=10;f.tick=now66;g_door_curve66.publish(f);packet(r,.9);
 check(DriveDoor66(r,h,.9995f,false,out)==1&&near(out,.70));
 now66+=10;f.tick=now66;g_door_curve66.publish(f);packet(r,0);
 check(DriveDoor66(r,h,.70f,false,out)==1&&near(out,0));
 check(DriveDoor66(r,h,.70f,false,out)==1&&near(out,0));
 // The separate native contact/finger qualifier publishes inert grip geometry.
 // Actual Drive adopts this handle-local native wrist instead of retaining a
 // convenience offset up to20cm off the handle; it does not drive the clock.
 reset();f=frame(r);g_door_curve66.publish(f);packet(r,.25);h=hand(r);
 const ph::Pose nativeOffset{{0,0,std::sin(.05),std::cos(.05)},{-.02,.01,0}};
 NativeGrip66 grip{r,f.path.source.curve,nativeOffset,true};g_door_native_grip66.publish(grip);
 check(DriveDoor66(r,h,.25f,true,out)==1&&near(out,.25));check(g_door_display66.read(display));
 check(near(display.wrist.fixed_offset.x,.23)&&near(display.wrist.fixed_offset.y,.01));
 check(near(display.wrist.rotation.z,std::sin(.05))&&near(display.leaf.p.x,.25));
 now66+=10;f.tick=now66;g_door_curve66.publish(f);packet(r,.4);
 check(DriveDoor66(r,h,.35f,false,out)==1&&near(out,.4));check(g_door_display66.read(display)&&near(display.wrist.fixed_offset.x,.33));
 DetachDoor66(r);check(g_door_native_grip66.read(grip)&&grip.valid); // no native Use or new grip discovery
 now66+=10;f.tick=now66;g_door_curve66.publish(f);packet(r,.4);h.press=100;
 check(DriveDoor66(r,h,.35f,true,out)==1&&near(out,.35));check(g_door_display66.read(display)&&near(display.wrist.fixed_offset.x,.33));
 now66+=10;f.tick=now66;g_door_curve66.publish(f);packet(r,.5);
 check(DriveDoor66(r,h,.35f,false,out)==1&&near(out,.45));
 g_door_live_serial66=r.serial;RetireDoor66(r);
 check(g_door_live_serial66==0&&g_door_native_grip66.read(grip)&&!grip.valid&&!g_door_projection66.active);
 // No source, clip-geometry or anatomy mismatch may change the display offset.
 // Press is intentionally absent: accepted same-door regrip reuses the socket.
 for(unsigned fault=0;fault<15;++fault){reset();f=frame(r);g_door_curve66.publish(f);packet(r,.25);
  NativeGrip66 bad{r,f.path.source.curve,nativeOffset,true};
  switch(fault){case 0:++bad.request.serial;break;case 1:++bad.request.entity;break;case 2:++bad.request.player;break;
  case 3:bad.request.side=1;break;case 4:++bad.request.epoch;break;case 5:++bad.request.trackingEpoch46;break;
  case 6:++bad.request.sourceGeneration;break;case 7:++bad.request.inputSettingsEpoch46;break;
  case 8:++bad.request.inputExperimentEpoch65;break;case 9:++bad.curve;break;case 10:bad.valid=false;break;
  case 11:bad.local.q.w=2;break;case 12:bad.local.p.x=std::numeric_limits<double>::quiet_NaN();break;
  case 13:bad.local.q.z=std::numeric_limits<double>::infinity();break;case 14:++bad.request.id;break;}
  g_door_native_grip66.publish(bad);check(DriveDoor66(r,hand(r),.25f,true,out)==1);
  check(g_door_display66.read(display)&&near(display.wrist.fixed_offset.x,.20)&&near(display.wrist.fixed_offset.y,0));
 }
 std::printf("door_drive66: %u checks PASS (actual production adapter; completed-packet/clock leaves doubled)\n",checks);
}
