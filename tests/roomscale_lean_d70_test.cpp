// Actual camera XR input/drive callers are extracted by tools/extract_roomscale_d53.py.
#include "vrik/roomscale58.h"
#include "vrik/pose_math.h"
#include <atomic>
#include <mutex>
#include <cstdio>
#include <cmath>
#include <limits>
using namespace kcd2vr;
struct Quatf{float x=0,y=0,z=0,w=1;};struct Vec3f{float x=0,y=0,z=0;};
struct XrPose{float pos_x=0,pos_y=0,pos_z=0,qx=0,qy=0,qz=0,qw=1;};
static Quatf Normalize(Quatf q){auto n=pose_core::unit({q.x,q.y,q.z,q.w}).value_or(pose_core::Quat{});return {float(n.x),float(n.y),float(n.z),float(n.w)};}
static Quatf Conjugate(Quatf q){return {-q.x,-q.y,-q.z,q.w};}
static Vec3f Rotate(Quatf q,Vec3f v){auto r=pose_core::rotate({q.x,q.y,q.z,q.w},{v.x,v.y,v.z});return {float(r.x),float(r.y),float(r.z)};}
static std::uint64_t currentTick=1000;
static std::uint64_t GetTickCount64(){return currentTick;}
static std::mutex g_roomscale_mutex58;
static roomscale58::State g_roomscale58{};
static std::atomic<float>g_roomscale_rem_x58{0},g_roomscale_rem_y58{0},g_recenter_anchor_x{0},g_recenter_anchor_y{0};
static std::atomic<std::uint64_t>g_roomscale_tick58{0};static std::atomic<bool>g_roomscale_enabled58{true};
struct CameraHook{static roomscale58::Drive RoomscaleDrive(std::uint64_t,bool,bool,bool);};
#include "actual_roomscale_xr_d53.inc"
static int checks=0,failed=0;static void ck(bool v,const char*n){++checks;if(!v){++failed;std::printf("FAIL %s\n",n);}}
static roomscale58::Drive sample(double right,double forward,double height,double heading=0,bool combat=false,bool dodge=false){
 XrPose origin{},p{};origin.pos_y=1.7f;p.pos_y=float(1.7+height);
 p.qy=float(-std::sin(heading/2));p.qw=float(std::cos(heading/2));
 p.pos_x=float(right*std::cos(heading)+forward*std::sin(heading));p.pos_z=float(right*std::sin(heading)-forward*std::cos(heading));
 currentTick+=20;RoomscaleXrSample47(p,origin);return CameraHook::RoomscaleDrive(currentTick,true,combat,dodge);
}
static void reset(){g_roomscale58={};g_roomscale_tick58=0;g_recenter_anchor_x=0;g_recenter_anchor_y=0;}
int main(){
 using namespace roomscale58;
 ck(LeanHeightD70()==.10f,"D74 requested lean height defaults10cm");
 for(double heading:{0.,1.5707963267948966,-1.5707963267948966}){
  reset();auto d=sample(0,.4,-.09,heading);ck(d.following&&!d.forwardLeanHeldD70,"9cm height change retains forward walking with requested10cm threshold");
  d=sample(0,.4,-.1001,heading);ck(!d.following&&d.forwardLeanHeldD70,"more than10cm height drop admits forward lean without walking");
  d=sample(0,.4,-.06,heading);ck(!d.following&&d.forwardLeanHeldD70,"10cm threshold retains5cm recovery hysteresis");
  d=sample(0,.4,-.049,heading);ck(d.following&&!d.forwardLeanHeldD70,"upright recovery resumes same-sample walking");
  reset();for(int n=0;n<=5;++n)d=sample(0,.04*n,0,heading,true,true);
  ck(!d.dodge,"rapid20cm forward step never emits a forward dodge");
 }
 // Existing explicit4cm preference remains supported; exercise the original
 // tuning independently of the newly requested default.
 SetLeanHeightD70(.04f);
 for(double heading:{0.,.5,1.5707963267948966,3.141592653589793,-1.5707963267948966}){
  reset();auto d=sample(0,.4,0,heading);ck(d.following&&!d.forwardLeanHeldD70,"upright forward step remains immediate");
  d=sample(0,.4,-.06,heading);ck(!d.following&&d.forwardLeanHeldD70&&std::abs(d.lx)<1e-5&&std::abs(d.ly)<1e-5,"lowered forward reach injects no walk at any physical heading");
  for(int i=0;i<5;++i){d=CameraHook::RoomscaleDrive(currentTick,true,false,false);ck(!d.following&&d.forwardLeanHeldD70,"combat-history retirement or repeated drive cannot erase exact tracked height");}
  d=sample(0,.4,-.03,heading);ck(!d.following&&d.forwardLeanHeldD70,"3cm recovery stays inside height hysteresis");
  d=sample(0,.4,-.019,heading);ck(d.following&&!d.forwardLeanHeldD70,"upright recovery restores follow on this sample without smoothing");
  reset();d=sample(.4,0,-.08,heading);ck(d.following&&!d.forwardLeanHeldD70,"sideways crouched travel remains allowed");
  reset();d=sample(0,-.4,-.08,heading);ck(d.following&&!d.forwardLeanHeldD70,"backward crouched travel remains allowed");
  reset();d=sample(.4,.4,-.08,heading);const double fx=std::sin(heading),fy=std::cos(heading);ck(d.following&&d.forwardLeanHeldD70&&std::abs(d.lx*fx+d.ly*fy)<1e-6,"diagonal keeps lateral input and removes only current forward component");
  ck(g_recenter_anchor_x==0&&g_recenter_anchor_y==0&&g_roomscale58.anchorX==0&&g_roomscale58.anchorY==0,"classification never changes camera anchors");
 }
 // Config OFF equals the preexisting translation-only formula for all directions.
 SetLeanHeightD70(0);
 for(int x=-5;x<=5;++x)for(int y=-5;y<=5;++y){reset();auto d=sample(x*.1,y*.1,-.2);double len=std::hypot(x*.1,y*.1),mag=len>.12?std::min(.65,(len-.12)/.35):0;ck(std::abs(d.lx-(len>0?x*.1/len*mag:0))<1e-6&&std::abs(d.ly-(len>0?y*.1/len*mag:0))<1e-6&&!d.forwardLeanHeldD70,"zero retains exact old follow geometry");}
 SetLeanHeightD70(.04f);reset();auto d=sample(0,.4,-.1);ck(!d.following,"lean fixture blocked before turning feature off");SetLeanHeightD70(0);d=CameraHook::RoomscaleDrive(currentTick,true,false,false);ck(d.following&&!d.forwardLeanHeldD70,"turning OFF restores same-sample baseline without stale latch");
 SetLeanHeightD70(.04f);reset();d=sample(0,.4,-.1);currentTick+=61;d=CameraHook::RoomscaleDrive(currentTick,true,false,false);ck(!d.forwardLeanHeldD70,"stale height is not used as authoritative lean metadata");
 // D75: upright and sub-threshold travel retains current-heading side/back classification.
 for(double h:{0.,1.5707963267948966})for(double x:{-.2,0.,.2})for(double y:{-.2,0.,.2}){
  State a{},b{};Config old{},now{};old.forwardLeanHeightD70=0;
  for(int k=0;k<=5;++k){const double xx=(x*std::cos(h)+y*std::sin(h))*k/5,yy=(-x*std::sin(h)+y*std::cos(h))*k/5;observeHmd(a,2000+20*k,xx,yy,std::sin(h),std::cos(h),-.09,true);observeHmd(b,2000+20*k,xx,yy,std::sin(h),std::cos(h),-.09,true);}
  const auto& p=a.followPoseD70;auto da=drive(a,2100,p.x,p.y,false,true,true,old),db=drive(b,2100,p.x,p.y,false,true,true,now);ck(da.dodge==db.dodge&&da.dodgeLx==db.dodgeLx&&da.dodgeLy==db.dodgeLy&&db.dodgeLy<=0,"upright/sub-threshold lean retains side/back dodge direction and never admits forward");
 }
 SetLeanHeightD70(std::numeric_limits<float>::quiet_NaN());ck(LeanHeightD70()==.10f,"invalid configuration restores requested10cm default");SetLeanHeightD70(.9f);ck(LeanHeightD70()==.12f,"maximum12cm bounded");SetLeanHeightD70(-.1f);ck(LeanHeightD70()==0,"negative clamps OFF");
 std::printf("D70 actual roomscale producer/consumer %d/%d; no native acceptance or footsteps inferred\n",checks-failed,checks);return failed?1:0;
}
