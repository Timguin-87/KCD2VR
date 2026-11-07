// Actual production publication/getter extracted without substitutions. Only
// the clock, camera lifecycle flags and current tracking generation are leaves.
#include "../src/vrik/visual_ray_d75.h"
#include "../src/core/snapshot_mailbox.h"
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <limits>
#include <thread>

static std::uint64_t testNowD75=1000;
static std::uint64_t GetTickCount64(){return testNowD75;}
namespace kcd2vr {
namespace tracking59c6 {static std::atomic<std::uint64_t> epoch{5};}
namespace roomscale_d76 {static std::atomic<bool> recenterPending{false};}
class CameraHook {
public:
 static inline std::atomic<bool> s_pose_received{true},s_origin_valid{true},s_recenter_requested{false};
 static inline std::atomic<std::uint64_t> generation{7};
 static std::uint64_t CurrentFamilyGenerationR24(){return generation.load(std::memory_order_acquire);}
 static bool GetLatestVisualRayD75(visual_ray_d75::Sample&,std::uint64_t=80);
};
namespace {
struct Matrix34f {float m[3][4]{};};
static core::SnapshotMailbox<visual_ray_d75::Sample> g_visual_ray_d75;
}
#ifndef KCD2VR_VISUAL_RAY_D75_EXTRACT
#error Generate the two actual production functions with extract_visual_ray_d75.py.
#endif
#include KCD2VR_VISUAL_RAY_D75_EXTRACT
}
using namespace kcd2vr;
static unsigned checks75=0;
static void ck75(bool ok,const char* why){++checks75;if(!ok)std::fprintf(stderr,"D75 visual ray FAIL %s\n",why);assert(ok);}
static Matrix34f matrix75(float sign=1){Matrix34f m{};m.m[0][3]=sign*10;m.m[1][3]=sign*20;m.m[2][3]=sign*30;m.m[1][1]=sign*2;return m;}
static void reset75(){
 testNowD75=1000;tracking59c6::epoch=5;CameraHook::generation=7;
 CameraHook::s_pose_received=CameraHook::s_origin_valid=true;CameraHook::s_recenter_requested=false;
 roomscale_d76::recenterPending=false;
 g_visual_ray_d75.publish({});
}
int main(){
 visual_ray_d75::Sample ray{};reset75();
 ck75(!CameraHook::GetLatestVisualRayD75(ray),"no fabricated initial ray");
 const auto matrix=matrix75();auto writable=matrix;
 PublishVisualRayD75(writable,5,7,12);
 ck75(!std::memcmp(&writable,&matrix,sizeof(matrix)),"publication never changes camera matrix");
 ck75(CameraHook::GetLatestVisualRayD75(ray)&&ray.origin[0]==10&&ray.origin[1]==20&&ray.origin[2]==30&&
      ray.forward[0]==0&&ray.forward[1]==1&&ray.forward[2]==0&&ray.sourceEpoch==5&&ray.cameraGeneration==7&&ray.poseFrame==12,
      "one matrix supplies normalized direction, origin and provenance");
 PublishVisualRayD75(writable,5,7,12,nullptr,1.4f);
 ck75(CameraHook::GetLatestVisualRayD75(ray)&&ray.worldScaleR92==1.4f,"world scale is carried by the exact matrix publication");
 ck75(!std::memcmp(&writable,&matrix,sizeof(matrix)),"scale receipt does not rescale the rendered camera");
 testNowD75=1080;ck75(CameraHook::GetLatestVisualRayD75(ray),"exact80ms boundary");
 testNowD75=1081;ck75(!CameraHook::GetLatestVisualRayD75(ray)&&!ray.valid,"stale read clears output");
 testNowD75=999;ck75(!CameraHook::GetLatestVisualRayD75(ray),"future camera timestamp rejected");
 for(int fault=0;fault<6;++fault){
  reset75();PublishVisualRayD75(matrix,5,7,12);
  if(fault==0)CameraHook::s_recenter_requested=true;
  if(fault==1)tracking59c6::epoch=6;
  if(fault==2)CameraHook::generation=8;
  if(fault==3)CameraHook::s_pose_received=false;
  if(fault==4)CameraHook::s_origin_valid=false;
  if(fault==5)roomscale_d76::recenterPending=true;
  ck75(!CameraHook::GetLatestVisualRayD75(ray)&&!ray.valid,"recenter/reset/origin invalidation rejects a still-fresh cached matrix");
 }
 reset75();PublishVisualRayD75(matrix,5,7,12);tracking59c6::epoch=6;CameraHook::generation=8;
 PublishVisualRayD75(matrix75(-1),5,7,13);
 ck75(!CameraHook::GetLatestVisualRayD75(ray),"old camera worker cannot stamp its old matrix as new epoch");
 PublishVisualRayD75(matrix75(-1),6,8,14);
 ck75(CameraHook::GetLatestVisualRayD75(ray)&&ray.origin[0]==-10&&ray.poseFrame==14,"new completed camera restores display focus");
 // A late prior-generation producer may not erase or replace that newer ray.
 PublishVisualRayD75(matrix,5,7,15);
 ck75(CameraHook::GetLatestVisualRayD75(ray)&&ray.poseFrame==14,"late old publication cannot overwrite current display ray");
 for(int fault=0;fault<4;++fault){
  reset75();auto bad=matrix;
  if(fault==0)bad.m[0][3]=std::numeric_limits<float>::quiet_NaN();
  if(fault==1)bad.m[1][1]=std::numeric_limits<float>::infinity();
  if(fault==2)bad.m[1][1]=0;
  if(fault==3)bad.m[1][1]=1.e-6f;
  PublishVisualRayD75(bad,5,7,12);ck75(!CameraHook::GetLatestVisualRayD75(ray),"invalid matrix cannot publish a focus ray");
 }
 reset75();
 // Same physical head, alternating rendered eye matrices. The producer gets
 // the center already computed from the native base/position basis before IPD.
 const float center[3]{1000.5f,2000.25f,3000.75f};
 for(int eye:{-1,1}){
  auto perEye=matrix75();perEye.m[0][3]=center[0]+float(eye)*.032f;
  perEye.m[1][3]=center[1];perEye.m[2][3]=center[2];const auto before=perEye;
  PublishVisualRayD75(perEye,5,7,16,center);
  ck75(CameraHook::GetLatestVisualRayD75(ray)&&!std::memcmp(ray.origin,center,sizeof(center)),"AFR eyes share the exact same center-head display origin");
  ck75(!std::memcmp(&perEye,&before,sizeof(perEye)),"center-head display origin does not change per-eye rendered matrix");
 }
 float badCenter[3]{std::numeric_limits<float>::quiet_NaN(),0,0};
 PublishVisualRayD75(matrix,5,7,17,badCenter);
 ck75(!CameraHook::GetLatestVisualRayD75(ray),"invalid center-head origin cannot publish a name ray");
 reset75();roomscale_d76::recenterPending=true;
 PublishVisualRayD75(matrix,5,7,18);roomscale_d76::recenterPending=false;
 ck75(!CameraHook::GetLatestVisualRayD75(ray),"D76 coordinated recenter cannot publish a transient old-origin ray");
 PublishVisualRayD75(matrix,5,7,19);
 ck75(CameraHook::GetLatestVisualRayD75(ray)&&ray.poseFrame==19,"completed post-recenter camera restores head names");
 reset75(); // prior recovery check used a different poseFrame; start the concurrency case empty
 std::atomic<unsigned> mixed{0},accepted{0},ready{0},finished{0};
 std::atomic<bool> start{false};
 // A very fast optimized reader can otherwise finish its entire empty-mailbox
 // loop before either OS thread starts: a scheduler artefact, not mixed data.
 PublishVisualRayD75(matrix75(),5,7,12);
 auto writer=[&](int sign){const auto m=matrix75(float(sign));++ready;while(!start.load())std::this_thread::yield();for(unsigned i=0;i<20000;++i)PublishVisualRayD75(m,5,7,unsigned(sign>0?12:13));++finished;};
 std::thread positive(writer,1),negative(writer,-1);
 while(ready.load()!=2)std::this_thread::yield();start=true;
 for(unsigned i=0;i<40000||finished.load()!=2;++i){if(CameraHook::GetLatestVisualRayD75(ray)){
  ++accepted;const float sign=ray.forward[1];
  if((sign!=1&&sign!=-1)||ray.origin[0]!=10*sign||ray.origin[1]!=20*sign||ray.origin[2]!=30*sign||
     ray.poseFrame!=std::uint64_t(sign>0?12:13))++mixed;
 }if((i&255u)==0)std::this_thread::yield();}
 positive.join();negative.join();
 ck75(accepted.load()>0&&mixed.load()==0,"concurrent publishers never mix one matrix origin with another direction/frame");
 std::printf("D76R1 actual camera visual-ray functions: %u checks PASS; %u coherent concurrent reads; camera rendering NOT_RUN\n",checks75,accepted.load());
}
