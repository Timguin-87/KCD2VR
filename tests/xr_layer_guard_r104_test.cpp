// R104 host test: overlay layer repair + quarantine (src/xr_layer_guard_r104.h). g++ -std=c++20 -I src -I tests/openxr_sdk
#include <openxr/openxr.h>
#include "xr_layer_guard_r104.h"
#include <cstdio>
#include <cmath>
#include <limits>
namespace g=xr_layer_guard_r104;
static int fails=0,checks=0;
#define CHECK(c) do{++checks;if(!(c)){++fails;std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);}}while(0)
static XrSwapchain SC(){return reinterpret_cast<XrSwapchain>(0x1234);}
static XrSpace SP(){return reinterpret_cast<XrSpace>(0x77);}
static XrCompositionLayerQuad Q(){XrCompositionLayerQuad q{XR_TYPE_COMPOSITION_LAYER_QUAD};q.space=SP();q.subImage.swapchain=SC();q.subImage.imageRect={{0,0},{3,3}};
    q.pose.orientation={0,0,0,1};q.pose.position={0,0,-1};q.size={1.f,0.0015f};return q;}
int main(){
    // good quad untouched
    {auto q=Q();unsigned f=0;CHECK(g::fixQuad(q,1100,680,&f)==g::DropNone);CHECK(f==0);CHECK(q.size.height==0.0015f);}
    // unnormalized quaternion is normalized (SteamVR POSE_INVALID)
    {auto q=Q();q.pose.orientation={0.1f,0.2f,0.3f,1.2f};unsigned f=0;CHECK(g::fixQuad(q,1100,680,&f)==g::DropNone);CHECK(f&g::FixQuat);
     const auto&o=q.pose.orientation;CHECK(std::fabs(o.x*o.x+o.y*o.y+o.z*o.z+o.w*o.w-1.f)<1e-5f);}
    // NaN pose dropped, zero quaternion dropped
    {auto q=Q();q.pose.position.x=std::numeric_limits<float>::quiet_NaN();unsigned f=0;CHECK(g::fixQuad(q,1100,680,&f)==g::DropPose);}
    {auto q=Q();q.pose.orientation={0,0,0,0};unsigned f=0;CHECK(g::fixQuad(q,1100,680,&f)==g::DropPose);}
    // R105: no space -> dropped
    {auto q=Q();q.space=XR_NULL_HANDLE;unsigned f=0;CHECK(g::fixQuad(q,1100,680,&f)==g::DropNoSpace);}
    // no swapchain / empty swapchain
    {auto q=Q();q.subImage.swapchain=XR_NULL_HANDLE;unsigned f=0;CHECK(g::fixQuad(q,1100,680,&f)==g::DropNoSwapchain);}
    {auto q=Q();unsigned f=0;CHECK(g::fixQuad(q,0,0,&f)==g::DropEmptySwapchain);}
    // rect clamped into the swapchain (SteamVR SWAPCHAIN_RECT_INVALID)
    {auto q=Q();q.subImage.imageRect={{-5,10},{4000,4000}};unsigned f=0;CHECK(g::fixQuad(q,2270,2429,&f)==g::DropNone);CHECK(f&g::FixRect);
     CHECK(q.subImage.imageRect.offset.x==0&&q.subImage.imageRect.offset.y==10&&q.subImage.imageRect.extent.width==2270&&q.subImage.imageRect.extent.height==2419);}
    {auto q=Q();q.subImage.imageRect={{0,0},{0,0}};unsigned f=0;CHECK(g::fixQuad(q,10,10,&f)==g::DropNone);CHECK(q.subImage.imageRect.extent.width==1);}
    // size: zero -> min, NaN dropped
    {auto q=Q();q.size={0.f,-1.f};unsigned f=0;CHECK(g::fixQuad(q,10,10,&f)==g::DropNone);CHECK(f&g::FixSize);CHECK(q.size.width>0&&q.size.height>0);}
    {auto q=Q();q.size.width=std::numeric_limits<float>::infinity();unsigned f=0;CHECK(g::fixQuad(q,10,10,&f)==g::DropSize);}
    // cylinder
    {XrCompositionLayerCylinderKHR c{XR_TYPE_COMPOSITION_LAYER_CYLINDER_KHR};c.space=SP();c.subImage.swapchain=SC();c.subImage.imageRect={{0,0},{100,100}};
     c.pose.orientation={0,0,0,2};c.radius=0;c.centralAngle=0.5f;c.aspectRatio=1.5f;unsigned f=0;CHECK(g::fixCylinder(c,100,100,&f)==g::DropNone);
     CHECK(c.radius>=0.05f);CHECK(std::fabs(c.pose.orientation.w-1.f)<1e-6f);}
    // quarantine: single kind failing -> 10 s, then back, second failure 20 s, never permanent
    {g::Quarantine z;std::uint64_t t=1000;CHECK(z.admit(g::Hud,t));
     CHECK(z.onResult(1u<<g::Hud,false,t)==10000u);CHECK(z.lastBlamed==g::Hud);
     CHECK(!z.admit(g::Hud,t+9999));CHECK(z.admit(g::Hud,t+10000));
     CHECK(z.onResult(1u<<g::Hud,false,t+10011)==20000u);
     t+=10011;for(int i=0;i<6;++i){t+=60001;CHECK(z.admit(g::Hud,t));CHECK(z.onResult(1u<<g::Hud,false,t)<=60000u);}
     t+=60001;CHECK(z.admit(g::Hud,t));CHECK(z.onResult(1u<<g::Hud,true,t+20000)==0);CHECK(z.strikes[g::Hud]==0);}
    // unknown culprit in a mixed frame: all sit out 1 s and return one by one; the returning culprit is blamed
    {g::Quarantine z;std::uint64_t t=5000;const unsigned m=(1u<<g::HandLaser)|(1u<<g::Hud)|(1u<<g::Menu);
     CHECK(z.onResult(m,false,t)==0);CHECK(z.lastBlamed==g::Count);
     CHECK(!z.admit(g::HandLaser,t+999));CHECK(z.admit(g::HandLaser,t+1000));CHECK(!z.admit(g::Hud,t+1000));
     CHECK(z.onResult(1u<<g::HandLaser,true,t+1000)==0);
     CHECK(z.admit(g::Hud,t+1300));CHECK(z.onResult((1u<<g::HandLaser)|(1u<<g::Hud),false,t+1311)==10000u);CHECK(z.lastBlamed==g::Hud);
     CHECK(z.admit(g::HandLaser,t+1320));CHECK(z.admit(g::Menu,t+1600));CHECK(!z.admit(g::Hud,t+5000));}
    // R106: only layer error codes blame; TIME_INVALID (-30) / SESSION_LOST (-17) never put anything in the sit-out
    {g::Quarantine z;CHECK(z.onResult(1u<<g::Hud,false,100,-30)==0);CHECK(z.admit(g::Hud,101));CHECK(z.onResult(1u<<g::Hud,false,200,-17)==0);CHECK(z.admit(g::Hud,201));
     CHECK(z.onResult(1u<<g::Hud,false,300,-25)==10000u);CHECK(!z.admit(g::Hud,301));CHECK(g::layerError(-1)&&g::layerError(-23)&&g::layerError(-24)&&g::layerError(-25)&&g::layerError(-39)&&!g::layerError(-42)&&!g::layerError(-26)&&!g::layerError(-2)&&!g::layerError(0));}
    // success never blames; empty mask failure does nothing
    {g::Quarantine z;CHECK(z.onResult(0,false,10)==0);CHECK(z.admit(g::Menu,10));}
    std::printf("xr_layer_guard_r104: %d/%d %s\n",checks-fails,checks,fails?"FAIL":"PASS");return fails?1:0;
}
