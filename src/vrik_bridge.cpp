#include "vrik_bridge.h"
#include "xr_manager.h"
#include "debug_trace.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace kcd2vr {
namespace {
struct V3 { float x,y,z; };
static V3 Add(V3 a,V3 b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
static V3 Sub(V3 a,V3 b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
static V3 Mul(V3 a,float s){return {a.x*s,a.y*s,a.z*s};}
static float Dot(V3 a,V3 b){return a.x*b.x+a.y*b.y+a.z*b.z;}
static float Len(V3 a){return std::sqrt(std::max(0.0f,Dot(a,a)));}
static V3 Norm(V3 a){const float l=Len(a); return l>1.0e-5f?Mul(a,1.0f/l):V3{0,0,-1};}
static V3 QRotate(const XrQuaternionf& q,V3 v){
    // q * v * q^-1, expanded to avoid introducing another XR call/thread owner.
    const V3 qv{q.x,q.y,q.z};
    const V3 t{2.0f*(qv.y*v.z-qv.z*v.y),2.0f*(qv.z*v.x-qv.x*v.z),2.0f*(qv.x*v.y-qv.y*v.x)};
    const V3 c{qv.y*t.z-qv.z*t.y,qv.z*t.x-qv.x*t.z,qv.x*t.y-qv.y*t.x};
    return {v.x+q.w*t.x+c.x,v.y+q.w*t.y+c.y,v.z+q.w*t.z+c.z};
}

static VrikBridge::ArmChain SolveArm(const XrPosef& head,const ControllerState& hand,bool left)
{
    VrikBridge::ArmChain out{};
    if(!hand.active) return out;

    // Primitive diagnostic anthropometric estimate only. Native KCD2 bone IDs,
    // owner pointers and spaces must be proven before any skeleton write.
    constexpr float upper=0.30f;
    constexpr float fore=0.27f;
    const V3 localShoulder{left?-0.18f:0.18f,-0.22f,0.025f};
    const V3 localPole{left?-0.70f:0.70f,-0.45f,-0.15f};
    const V3 hp{head.position.x,head.position.y,head.position.z};
    const V3 shoulder=Add(hp,QRotate(head.orientation,localShoulder));
    const V3 target{hand.pose.pos_x,hand.pose.pos_y,hand.pose.pos_z};
    const V3 delta=Sub(target,shoulder);
    const float raw=Len(delta);
    const float d=std::clamp(raw,0.04f,upper+fore-0.005f);
    const V3 dir=Norm(delta);

    float cosA=(upper*upper+d*d-fore*fore)/(2.0f*upper*d);
    cosA=std::clamp(cosA,-1.0f,1.0f);
    const float along=upper*cosA;
    const float perpendicular=upper*std::sqrt(std::max(0.0f,1.0f-cosA*cosA));

    V3 pole=QRotate(head.orientation,localPole);
    pole=Sub(pole,Mul(dir,Dot(pole,dir)));
    pole=Norm(pole);
    const V3 elbow=Add(shoulder,Add(Mul(dir,along),Mul(pole,perpendicular)));

    out.active=true;
    out.sx=shoulder.x; out.sy=shoulder.y; out.sz=shoulder.z;
    out.ex=elbow.x; out.ey=elbow.y; out.ez=elbow.z;
    out.hx=target.x; out.hy=target.y; out.hz=target.z;
    out.reach=raw;
    return out;
}
}

std::atomic<bool> VrikBridge::enabled_{false};
VrikBridge::Target VrikBridge::left_{};
VrikBridge::Target VrikBridge::right_{};
VrikBridge::ArmChain VrikBridge::left_arm_{};
VrikBridge::ArmChain VrikBridge::right_arm_{};
std::uint64_t VrikBridge::samples_=0;

void VrikBridge::SetEnabled(bool e)
{
    enabled_.store(e,std::memory_order_release);
    debug_trace::Write(e
        ? "V58-VRIK primitive arm-chain observer ON (estimated shoulder/elbow; NO native skeleton writes)"
        : "V58-VRIK primitive arm-chain observer OFF");
}
bool VrikBridge::Enabled(){return enabled_.load(std::memory_order_acquire);}
VrikBridge::Target VrikBridge::Left(){return left_;}
VrikBridge::Target VrikBridge::Right(){return right_;}
VrikBridge::ArmChain VrikBridge::LeftArm(){return left_arm_;}
VrikBridge::ArmChain VrikBridge::RightArm(){return right_arm_;}

void VrikBridge::Update(XrManager& xr)
{
    if(!Enabled()) return;
    const auto& l=xr.LeftController();
    const auto& r=xr.RightController();
    auto cv=[](const ControllerState& c){
        Target t{}; t.active=c.active;
        // Preserve the historical target coordinate conversion for downstream
        // diagnostics; the arm chain below is explicitly OpenXR LOCAL space.
        t.px=c.pose.pos_x; t.py=c.pose.pos_z; t.pz=c.pose.pos_y;
        t.qx=c.pose.qx; t.qy=c.pose.qz; t.qz=c.pose.qy; t.qw=c.pose.qw;
        t.trigger=c.trigger; t.grip=c.grip; return t;
    };
    left_=cv(l); right_=cv(r);

    XrManager::RenderFrameState frame{};
    if(xr.CopyActiveRenderFrameState(frame) && frame.head_pose_valid) {
        left_arm_=SolveArm(frame.head_pose,l,true);
        right_arm_=SolveArm(frame.head_pose,r,false);
    } else {
        left_arm_={}; right_arm_={};
    }

    ++samples_;
    if(samples_<=8 || samples_%90==0){
        char m[1024]{};
        std::snprintf(m,sizeof(m),
            "V58-VRIK-CHAIN sample=%llu L={active:%d S:(%.3f,%.3f,%.3f) E:(%.3f,%.3f,%.3f) H:(%.3f,%.3f,%.3f) reach:%.3f} R={active:%d S:(%.3f,%.3f,%.3f) E:(%.3f,%.3f,%.3f) H:(%.3f,%.3f,%.3f) reach:%.3f} nativeWrites=0",
            (unsigned long long)samples_,
            left_arm_.active,left_arm_.sx,left_arm_.sy,left_arm_.sz,left_arm_.ex,left_arm_.ey,left_arm_.ez,left_arm_.hx,left_arm_.hy,left_arm_.hz,left_arm_.reach,
            right_arm_.active,right_arm_.sx,right_arm_.sy,right_arm_.sz,right_arm_.ex,right_arm_.ey,right_arm_.ez,right_arm_.hx,right_arm_.hy,right_arm_.hz,right_arm_.reach);
        debug_trace::Write(m);
    }
}
}
