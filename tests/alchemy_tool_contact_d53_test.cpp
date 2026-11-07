// Actual tool discovery/presentation and actual D53 contact adapter. The world
// query/native memory are explicit leaves; no game impact or recipe is asserted.
#define KCD2VR_ALCHEMY_PRESENTATION_NO_MAIN
#include "alchemy_tool_presentation_d39_test.cpp"
#include "vrik/station_ws_j27.h"
using V3=pc::Vec3;using Q4=pc::Quat;
static V3 Add(V3 a,V3 b){return a+b;}static V3 Sub(V3 a,V3 b){return a-b;}static V3 Mul(V3 a,float v){return a*double(v);}
static Q4 QFromQuatT(const QuatT28& q){return {q.q[0],q.q[1],q.q[2],q.q[3]};}
static Q4 QNormalize(Q4 q){const double n=std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);return {q.x/n,q.y/n,q.z/n,q.w/n};}
static V3 QRotateV(Q4 q,V3 v){return pc::rotate(q,v);}
static void QToQuatT(QuatT28& t,Q4 q){t.q[0]=float(q.x);t.q[1]=float(q.y);t.q[2]=float(q.z);t.q[3]=float(q.w);}
static void ToT(QuatT28& t,V3 p){t.t[0]=float(p.x);t.t[1]=float(p.y);t.t[2]=float(p.z);}
static bool ReadModelWorld6692(void*,ModelWorld6692& x){x={};x.pose.p=x.t=attachmentRootD47;return true;}
static V3 ModelToWorld6692(const ModelWorld6692& x,V3 p){return {p.x*x.scale+x.pose.p.x,p.y*x.scale+x.pose.p.y,p.z*x.scale+x.pose.p.z};}
static V3 WorldToModel6692(const ModelWorld6692& x,V3 p){return (p-x.t)*(1.0/double(x.scale));}
static kcd2vr::core::SnapshotMailbox<CommittedFrame6695> g_committed_frame6695;
namespace wc {enum class Result {Clear,Hit,Unknown};}
struct Hit53 {wc::Result state=wc::Result::Clear;float distance=.01f;bool plane=false;V3 normal{},point{},nativeNormalD63{};};
static Hit53 hit53{};static unsigned rays53=0;static std::vector<V3> origins53;static void(*onRay53)()=nullptr;
static Hit53 WorldRayDistance6692(V3 start,V3 sweep){
 ++rays53;origins53.push_back(start);if(onRay53)onRay53();auto h=hit53;
 const double n=pc::length(sweep);if(n>0)h.point=start+sweep*(h.distance/n);
 h.nativeNormalD63=h.normal;
 if(h.normal.x*sweep.x+h.normal.y*sweep.y+h.normal.z*sweep.z>0)h.normal=Mul(h.normal,-1);
 return h;
}
static std::atomic<bool> g_phys_fault6698{false};
static float bounds53[6]{};static bool boundsAvailable53=true;static unsigned boundsReads53=0;
static void(*onBounds53)()=nullptr;
static bool PhysBounds6698(void* p,float* out){++boundsReads53;if(p!=entity.data()||!boundsAvailable53)return false;
 std::copy_n(bounds53,6,out);if(onBounds53)onBounds53();return true;}
#include "../src/vrik/alchemy_tool_contact_d53.inl"
static void setup53(){
 kcd2vr::alchemy_tool_contact_d55::enabled=true;setup39(3,1);commit39();g_committed_frame6695.publish(frame);rays53=0;origins53.clear();hit53={};onRay53=nullptr;
 boundsAvailable53=true;boundsReads53=0;g_phys_fault6698=false;onBounds53=nullptr;
 const float bounds[]{-.02f,-.02f,0,.02f,.02f,.3f};std::copy_n(bounds,6,bounds53);
 ab39::offers[0].publish({}); // native held geometry does not depend on a selection candidate
}
static AlchemyContactResultD80 result80{};
static std::uint64_t sample80=11;
static void solve53(V3& p,Q4& q){AlchemyToolContactD53(character.data(),7,0,sample80,9,p,q,result80);}
int main(){
 setup53();V3 p{2.2,.2,0};Q4 q{0,.5,0,.8660254038};const Q4 requestedQ=q;
 hit53={wc::Result::Hit,.01f,true,{-1,0,0}};solve53(p,q);
 require39(rays53==3&&p.x>=2.17999&&p.x<2.2&&std::fabs(p.y-.2)<1e-6&&q.y==requestedQ.y,
   "current inward correction is bounded20mm; lateral movement and controller rotation remain free");
 require39(std::fabs(origins53[0].x-12.1)<1e-5,"probe starts at committed tool geometry, not source selection point");
 const auto accepted=result80;const V3 acceptedGoal=p;
 frame.alchemyContactD80[0]=accepted;g_committed_frame6695.publish(frame);rays53=0;boundsReads53=0;
 p={2.2,.2,0};q=requestedQ;hit53.state=wc::Result::Unknown;solve53(p,q);
 require39(rays53==0&&boundsReads53==0&&pc::length(p-acceptedGoal)<1e-6,
   "committed exact same-sample result reuses no native geometry and cannot flip to raw on spent budget");
 ++sample80;p={2.2,.2,0};q=requestedQ;solve53(p,q);
 require39(rays53==1&&p.x==2.2&&p.y==.2&&q.y==requestedQ.y&&result80.valid,
   "new sample unavailable coverage is permissive, never a stale pin or fake clear");--sample80;
 setup53();hit53={wc::Result::Hit,0.f,true,{-1,0,0}};p={2.2,.2,0};q=requestedQ;solve53(p,q);
 require39(p.x==2.2&&p.y==.2&&q.y==requestedQ.y,"zero-distance initial overlap lacks an origin-side witness and never freezes");
 setup53();hit53={wc::Result::Hit,.01f,true,{1,0,0}};p={2.2,0,0};q={};solve53(p,q);
 require39(p.x==2.2,"positive-distance exit retains controller pose despite the query normal flipping toward origin");
 setup53();hit53.state=wc::Result::Clear;p={2.3,0,0};q={};solve53(p,q);
 require39(std::fabs(p.x-2.3)<1e-6&&rays53==3,"clear world follows current controller without smoothing");
 setup53();hit53={wc::Result::Hit,.01f,true,{-1,0,0}};p={2,0,0};q=requestedQ;solve53(p,q);
 require39(q.y==requestedQ.y&&pc::length(p-V3{2,0,0})<=.020001,
   "rotated bounds endpoints constrain only bounded wrist translation; rotation is not frozen");
 for(int fault=0;fault<14;++fault){setup53();p={2.2,0,0};q={};hit53.state=wc::Result::Hit;
  switch(fault){
   case 0:mh39::alchemyMode67=mh39::Gesture;break;
   case 1:++mh39::modeEpoch67;break;
   case 2:boundsAvailable53=false;break;
   case 3:identity=false;break;
   case 4:g_phys_fault6698=true;break;
   case 5:Put(alchemy,0x150,std::uint32_t(43));break;
   case 6:bounds53[3]=std::numeric_limits<float>::quiet_NaN();break;
   case 7:presentation39.trackedPresentation71=false;ab39::control.publish(presentation39);break;
   case 8:tool39.joined=false;ab39::tools[0].publish(tool39);break;
   case 9:frame.stamp.tick=now-81;g_committed_frame6695.publish(frame);break;
   case 10:frame.tools59c10[0].socket.alchemy18.object++;g_committed_frame6695.publish(frame);break;
   case 11:kcd2vr::activity_input163::witness175.transitionDepth175=1;break;
   case 12:Put(attachment,0x20,reinterpret_cast<std::uintptr_t>(entity.data()));break;
   case 13:p.x=3;break;
  }
  const double requested=p.x;solve53(p,q);require39(rays53==0&&p.x==requested,"wrong mode, stale/changed owner, invalid/missing bounds or tracking jump cannot constrain a prop");
 }
 setup53();hit53.state=wc::Result::Hit;onRay53=+[](){ab39::control.publish({});};p={2.2,0,0};q={};solve53(p,q);
 require39(rays53>0&&p.x==2.2,"owner loss during external query revokes correction before arm publication");
 setup53();onBounds53=+[](){Put(attachment,0x20,reinterpret_cast<std::uintptr_t>(entity.data()));};p={2.2,0,0};q={};solve53(p,q);
 require39(boundsReads53==1&&rays53==0&&p.x==2.2,"attachment change inside native bounds read is rejected before any world queries");
 setup53();hit53.state=wc::Result::Hit;onRay53=+[](){identity=false;};p={2.2,0,0};q={};solve53(p,q);
 require39(rays53>0&&p.x==2.2,"entity identity loss during world query revokes the final correction");
 setup53();kcd2vr::alchemy_tool_contact_d55::enabled=false;p={2.2,0,0};q={};solve53(p,q);
 require39(boundsReads53==0&&rays53==0&&p.x==2.2,"explicit off spends no native bounds or world queries and does not hold an old constraint");
 setup53();hit53.state=wc::Result::Hit;onRay53=+[](){kcd2vr::alchemy_tool_contact_d55::enabled=false;};p={2.2,0,0};q={};solve53(p,q);
 require39(rays53>0&&p.x==2.2,"toggle off during a world query retires correction before wrist publication");
 // D56: even an otherwise joined, settled packet cannot constrain retort4.
 setup53();Put(alchemy,0x2A0,std::uint32_t(4));Put(alchemy,0x150+4*4,std::uint32_t(42));
 kcd2vr::alchemy_tool18::Binding retortD56{};
 require39(ReadAlchemyTool18(attachment.data(),42,frame.stamp.actor,retortD56)&&retortD56.type==4,"retort test has an actual native type4 binding");
 tool39.type=4;tool39.binding=retortD56;ab39::tools[0].publish(tool39);
 table39.refs[0]={0,0};table39.refs[4]={4,42};ab39::table.publish(table39);
 ToolSocket59c10 retortSocketD56{};g_tool_socket59c10[0].read(retortSocketD56);retortSocketD56.alchemy18=retortD56;
 g_tool_socket59c10[0].publish(retortSocketD56);frame.tools59c10[0].socket=retortSocketD56;g_committed_frame6695.publish(frame);
 hit53.state=wc::Result::Hit;p={2.2,0,0};q={};solve53(p,q);
 require39(boundsReads53==0&&rays53==0&&p.x==2.2,"native retort fixture is excluded before contact queries even with opt-in enabled");
 std::printf("D53 alchemy bounds contact: %u checks PASS; sparse world probes, no mesh/tool-to-tool or native effect claim\n",checks39);
}
