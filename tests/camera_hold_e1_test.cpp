// E1: arm reach that leans the solved torso/head must not move the camera base.
// tracked_frame51 (body frame) + eye() (camera base) + camera_hold_e1 (gap hold).
#include "vrik/tracked_frame51.h"
#include <cmath>
#include <cstdio>

namespace f=kcd2vr::tracked_frame51;namespace m=kcd2vr::pose_core;namespace ch=kcd2vr::camera_hold_e1;
static int checks=0,failures=0;
static void ck(bool v,const char* l){++checks;if(!v)++failures;std::printf("%s %s\n",v?"PASS":"FAIL",l);}
static double cm(m::Vec3 a,m::Vec3 b){return m::length(a-b)*100;}
static m::Quat yaw(double deg){const double h=deg*3.14159265358979/360;return {0,0,std::sin(h),std::cos(h)};}
int main(){
    // Henry standing; character root in world, turned 30 deg. Eye 9 cm in front of / 6 cm above the head bone.
    const m::Vec3 root{100,200,5};const auto q=yaw(30);const double scale=1;
    const m::Vec3 eyeLocal{0,.09,.06};
    f::Input i{};i.stamp={3,1,1000,0x100};i.control=1;i.timeNs=1000000000;
    i.nativeHead=i.restHead=i.calibratedHead={0,0,1.7};i.nativePelvis=i.restPelvis={0,0,.9};
    i.allowed=i.eyeNeutral=true;i.hmdHeight=true;i.scale=scale;
    f::State fs{};f::EyeState es{};f::Frame fr{};
    const auto nativeEyeFor=[&](m::Vec3 head){return root+m::rotate(q,(head+eyeLocal)*scale);};
    const auto step=[&](m::Vec3 head,m::Vec3 pelvis,bool neutral,m::Vec3& eye)->bool{
        ++i.stamp.sample;i.stamp.tick+=11;i.timeNs+=11000000;i.nativeHead=head;i.nativePelvis=pelvis;i.eyeNeutral=neutral;
        if(!f::advance(fs,i,fr))return false;fr.root=root;fr.rotation=q;
        return f::eye(es,fr,i.stamp,i.control,i.stamp.tick,nativeEyeFor(head),eye);
    };
    m::Vec3 base0{};
    ck(step({0,0,1.7},{0,0,.9},true,base0)&&cm(base0,nativeEyeFor({0,0,1.7}))<.1,"neutral stance: camera base = measured eye");
    // REACH: the hands go forward/down 60 cm; the arm-limited torso solve leans the head bone forward 12 cm and
    // down 9 cm and shifts the pelvis 2 cm (the native/solved eye follows that bone).
    double worstBase=0,worstNative=0;bool all=true;
    for(int n=1;n<=30;++n){const double t=n/30.0;const m::Vec3 head{0,.12*t,1.7-.09*t},pelvis{0,.02*t,.9-.01*t};
        m::Vec3 b{};all=all&&step(head,pelvis,false,b);worstBase=std::fmax(worstBase,cm(b,base0));
        worstNative=std::fmax(worstNative,cm(nativeEyeFor(head),base0));}
    std::printf("  reach: native eye moved %.1f cm, camera base moved %.3f cm\n",worstNative,worstBase);
    ck(all&&worstNative>10,"the reach really leans the solved head (>10 cm native eye)");
    ck(worstBase<.1,"camera base unchanged (<1 mm) through the whole torso lean while the HMD is still");
    // GAP: the leaned pose is rejected for 6 frames (extreme reach / hand tracking hold). Old path = native eye.
    const auto held=ch::remember(0x100,3,1,i.stamp.tick,scale,i.calibratedHead,root,q,base0,0,0);
    ck(held.valid,"accepted base remembered model-local");
    const m::Vec3 leanedEye=nativeEyeFor({0,.12,1.61});
    ck(cm(leanedEye,base0)>10,"old fallback (native eye) would jump >10 cm forward/down");
    double worstHold=0;bool heldAll=true;
    for(int n=1;n<=6;++n){m::Vec3 e{};double hx=0,hy=0;
        heldAll=heldAll&&ch::hold(held,0x100,3,1,scale,i.calibratedHead,root,q,i.stamp.tick+11*n,ch::maxHoldMs.load(),e,hx,hy)==ch::Why::Held;
        worstHold=std::fmax(worstHold,cm(e,base0));}
    ck(heldAll&&worstHold<.1,"rejected frames: camera base held (<1 mm), no forward/down jump");
    {m::Vec3 resumed{};ck(step({0,.12,1.61},{0,.02,.89},false,resumed)&&cm(resumed,base0)<.1,"next accepted frame resumes at the same base (no snap back)");}
    // Travel / turn during the gap stay live (body follow / roomscale root).
    {const m::Vec3 moved=root+m::Vec3{.05,-.03,0};const auto turned=yaw(45);m::Vec3 e{};double hx=0,hy=0;
     ck(ch::hold(held,0x100,3,1,scale,i.calibratedHead,moved,turned,held.tick+30,1500,e,hx,hy)==ch::Why::Held&&
        cm(e,moved+m::rotate(turned,m::rotate(ch::inverse(q),base0-root)))<.01,"held base follows the CURRENT root travel and turn");}
    // Heading (bow / workstation) re-rotated with the current root.
    {const auto h=ch::remember(0x100,3,1,2000,scale,i.calibratedHead,root,q,base0,-0.5,std::sqrt(.75));m::Vec3 e{};double hx=0,hy=0;
     ch::hold(h,0x100,3,1,scale,i.calibratedHead,root,yaw(120),2010,1500,e,hx,hy);
     const auto want=m::rotate(yaw(90),m::Vec3{-0.5,std::sqrt(.75),0});
     ck(std::fabs(hx-want.x)<1e-6&&std::fabs(hy-want.y)<1e-6,"held heading turns with the current root");}
    // Bounds: age, identity, disabled.
    {m::Vec3 e{};double hx=0,hy=0;const auto t=held.tick;const auto c=i.calibratedHead;
     ck(ch::hold(held,0x100,3,1,scale,c,root,q,t+1500,1500,e,hx,hy)==ch::Why::Held,"held up to the limit (1500 ms)");
     ck(ch::hold(held,0x100,3,1,scale,c,root,q,t+1501,1500,e,hx,hy)==ch::Why::Expired,"beyond the limit: game camera again");
     ck(ch::hold(held,0x100,3,1,scale,c,root,q,t-1,1500,e,hx,hy)==ch::Why::Expired,"clock before the sample refuses");
     ck(ch::hold(held,0x200,3,1,scale,c,root,q,t+10,1500,e,hx,hy)==ch::Why::Identity,"other character refuses");
     ck(ch::hold(held,0x100,4,1,scale,c,root,q,t+10,1500,e,hx,hy)==ch::Why::Identity,"new pose epoch (load/body) refuses");
     ck(ch::hold(held,0x100,3,2,scale,c,root,q,t+10,1500,e,hx,hy)==ch::Why::Identity,"mode/control toggle refuses");
     ck(ch::hold(held,0x100,3,1,1.1,c,root,q,t+10,1500,e,hx,hy)==ch::Why::Identity,"scale change refuses");
     ck(ch::hold(held,0x100,3,1,scale,c+m::Vec3{0,0,.01},root,q,t+10,1500,e,hx,hy)==ch::Why::Identity,"recalibration refuses");
     ck(ch::hold(held,0x100,3,1,scale,c,root,q,t+10,0,e,hx,hy)==ch::Why::Disabled,"limit 0 = old behaviour");
     ck(ch::hold(ch::Sample{},0x100,3,1,scale,c,root,q,t+10,1500,e,hx,hy)==ch::Why::None,"nothing accepted yet: no hold");
     ck(!ch::remember(0,3,1,t,scale,c,root,q,base0,0,0).valid,"no actor: not remembered");
     ck(!ch::remember(0x100,3,1,t,scale,c,root,q,{NAN,0,0},0,0).valid,"non-finite eye: not remembered");}
    {// R8d2 (Codex P1): ownership decided on the CURRENT state, the cache is retired when the game owns the camera
     using W=ch::Why;
     auto g=ch::gap(true,true,true,W::Held);ck(g.useHeld&&!g.clearCache,"plain gameplay gap: held base used");
     g=ch::gap(true,true,false,W::Held);ck(!g.useHeld&&g.clearCache,"menu opened DURING the world read: native eye, cache retired");
     g=ch::gap(false,true,true,W::Held);ck(!g.useHeld&&g.clearCache,"core final ownership check failed: no hold (stale admitted bit gone)");
     g=ch::gap(true,false,true,W::Held);ck(!g.useHeld&&g.clearCache,"native body / dialogue / picking / workstation now: no hold, cache retired");
     g=ch::gap(true,true,true,W::Expired);ck(!g.useHeld&&!g.clearCache,"expired: native eye, nothing else changed");
     // native entry then exit with no fresh accepted frame: the entry retired the cache, the exit has nothing to hold
     ch::Sample cache=ch::remember(0x100,3,1,1000,1.0,{0,0,1.6},{0,0,0},{0,0,0,1},{0,0,1.6},1,0);ck(cache.valid,"seed base remembered");
     if(ch::gap(true,false,false,W::Held).clearCache)cache=ch::Sample{};
     m::Vec3 e{};double hx=0,hy=0;ck(ch::hold(cache,0x100,3,1,1.0,{0,0,1.6},{0,0,0},{0,0,0,1},1100,1500,e,hx,hy)==W::None,"after a native owner: no old base comes back");}
    std::printf("%d/%d checks passed\n",checks-failures,checks);return failures?1:0;
}
