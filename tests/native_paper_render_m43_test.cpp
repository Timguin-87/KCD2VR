#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
#include "subsystems/vrmap/native_paper_m43.h"
namespace held_map_m1 {static std::atomic<std::uint64_t> activation{1};static bool requested=true,allowed=true,native=true;static bool Requested(){return requested;}static bool Allowed(std::uint64_t){return allowed;}static bool NativePaperSelectedM43(){return native;}}
struct QuatT28 {float q[4]{0,0,0,1},t[3]{};};
using Mat=kcd2vr::stowed_render_pose_d63::Mat34;
struct StowedRenderContextD63 {void* hideMask=nullptr;void* params=nullptr;void* parent=nullptr;void* passInfo=nullptr;};
struct StowedRenderHeaderD63 {void* binding=nullptr;char name[40]{};};
using StowedRenderFnD63=void(*)(void*,void*);
static std::uint64_t now=1000;static auto GetTickCount64(){return now;}
static std::atomic<std::uint64_t> g_pose_epoch6689{7},g_pose_policy_generation57{2};
static auto ReadCompletedPublicationT23(){return 4ull;}
static bool authority=true,tracking=true,inputs=true;static int held=0,calls=0,authorityCalls=0,loseOnCall=0;
static bool TrackingHandUsable111(unsigned,std::uint64_t){return tracking;}
static int ExactHandItemState(unsigned){return held;}
static bool StowedRenderAuthorityD63(std::uintptr_t,void*,void*,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t){++authorityCalls;return authority&&(!loseOnCall||authorityCalls<loseOnCall);}
static QuatT28 nativeCache{},relative{},additional{};
static bool StowedRenderInputsD63(void* c,void*,StowedRenderContextD63& out,Mat& parent,QuatT28& n,QuatT28& r){if(!inputs)return false;out=*static_cast<StowedRenderContextD63*>(c);parent=*static_cast<Mat*>(out.parent);n=nativeCache;r=relative;return true;}
static auto StowedRenderPoseD63(const QuatT28& p){return kcd2vr::stowed_render_pose_d63::Pose{{p.q[0],p.q[1],p.q[2],p.q[3]},{p.t[0],p.t[1],p.t[2]}};}
static std::uintptr_t bindingVft=0;
static auto StowedRenderBindingVftD63(void*){return bindingVft;}
template<class T>static bool Read(void* from,T& out){std::memcpy(&out,from,sizeof(out));return true;}
#include "subsystems/vrmap/native_paper_render_m43.inl"
static Mat rendered{};
static Mat mul(const Mat& a,const Mat& b){Mat c{};for(int i=0;i<3;++i)for(int j=0;j<4;++j){double v=j==3?a.m[4*i+3]:0;for(int k=0;k<3;++k)v+=double(a.m[4*i+k])*b.m[4*k+j];c.m[4*i+j]=float(v);}return c;}
static Mat rigid(const QuatT28& p){float qs[]{p.q[0],p.q[1],p.q[2],p.q[3],p.t[0],p.t[1],p.t[2],1};Mat m{};assert(kcd2vr::native_paper_m43::matrix(qs,m));return m;}
static void Original(void* context,void*){++calls;rendered=mul(mul(*static_cast<Mat*>(static_cast<StowedRenderContextD63*>(context)->parent),rigid(nativeCache)),rigid(additional));}
int main(){
 namespace p=kcd2vr::native_paper_m43;constexpr std::uintptr_t base=0x100000,character=0x200000,manager=character+8;
 alignas(16) std::uintptr_t binding[3]{0,0x300000,0};int bone=0;
 p::Frame f{};f.actor=0x210000;f.actorId=42;f.helperId=1042;f.character=character;f.manager=manager;f.epoch=7;f.activation=1;f.tick=now;
 f.slots[0]={reinterpret_cast<std::uintptr_t>(&bone),reinterpret_cast<std::uintptr_t>(binding),binding[1]};
 StowedRenderHeaderD63 h{};h.binding=binding;std::strcpy(h.name,"kcd2vr_paper_m43");bindingVft=base+0x3E5F4C0;
 Mat originalParent{};originalParent.m[3]=777;StowedRenderContextD63 context{&bone,&bone,&originalParent,&bone};
 const auto call=[&](){return NativePaperRenderM43(&context,&bone,h,base,reinterpret_cast<void*>(manager),reinterpret_cast<void*>(character),Original);};
 unsigned checks=0;const auto check=[&](bool b){++checks;assert(b);};
 float qs[]{0,0,0,1,100,200,300,1};check(p::matrix(qs,f.paper));check(p::frames.publish(f));p::visible=true;
 check(call()&&calls==1);check(context.parent==&originalParent&&originalParent.m[3]==777);
 // Exercise actual render branch across paper sizes, world rotations, native
 // cached rotations and marker offsets. Native additional is composed once.
 for(float scale:{.4f,.65f,1.f,1.5f})for(int angle=0;angle<360;angle+=15){
  const float a=float(angle*3.141592653589793/180.);
  qs[1]=std::sin(a*.5f);qs[3]=std::cos(a*.5f);qs[7]=scale;check(p::matrix(qs,f.paper));
  nativeCache={{0,0,.70710677f,.70710677f},{2,3,4}};relative={{0,0,0,1},{.12f,0,-.21f}};f.local[0]=StowedRenderPoseD63(relative);additional={{0,0,0,1},{.005f,.01f,0}};
  check(p::frames.publish(f));const int before=calls;check(call()&&calls==before+1);
  const Mat want=mul(mul(f.paper,rigid(relative)),rigid(additional));
  for(int i=0;i<12;++i)check(std::abs(rendered.m[i]-want.m[i])<.0001f);
 }
 const auto refused=[&](){const int before=calls;check(call()&&calls==before);};
 held_map_m1::requested=false;refused();held_map_m1::requested=true;
 held_map_m1::native=false;refused();held_map_m1::native=true;
 held_map_m1::allowed=false;refused();held_map_m1::allowed=true;
 held_map_m1::activation=2;refused();held_map_m1::activation=1;
 g_pose_epoch6689=8;refused();g_pose_epoch6689=7;
 now=1081;refused();now=999;refused();now=1000;
 tracking=false;refused();tracking=true;held=1;refused();held=0;
 authority=false;refused();authority=true;inputs=false;refused();inputs=true;
 p::visible=false;refused();p::visible=true;
 binding[1]++;refused();binding[1]--;
 bindingVft++;refused();bindingVft--;
 authorityCalls=0;loseOnCall=2;refused();loseOnCall=0;
 h.binding=binding+1;check(!call());h.binding=binding; // foreign binding retains original path
 std::strcpy(h.name,"weapon_shield");check(!call()); // native/stowed consumer unaffected
 std::strcpy(h.name,p::names[0]);check(call());
 qs[7]=0;check(!p::matrix(qs,f.paper));qs[7]=1;qs[0]=10;check(!p::matrix(qs,f.paper));
 std::printf("M43 actual render branch checks=%u failures=0 nativeLeaves=SUBSTITUTED headset=NOT_RUN\n",checks);
}
