#pragma once
#include "ui_native_catalogue.h"
#include <algorithm>
#include <atomic>
#include <array>
#include <cmath>
#include <cstdint>
namespace kcd2vr::ui_surface_geometry {
inline constexpr float StageWidth=static_cast<float>(ui_native_catalogue::HudStageWidth);
inline constexpr float StageHeight=static_cast<float>(ui_native_catalogue::HudStageHeight);
struct Vec {float x=0,y=0,z=0;};
struct Quat {float x=0,y=0,z=0,w=1;};
struct Fov {float left=-.8f,right=.8f,up=.8f,down=-.8f;};
struct Eye {Vec position{};Quat orientation{};Fov fov{};};
struct Rect {float left=0,top=0,right=StageWidth,bottom=StageHeight;};
struct Surface {float width=0,height=0,distance=1.55f,x=0,y=-.02f;bool valid=false;};
struct Hud {Surface surface{};Rect safe{};};
// R110 (Miki: "eye strain, i must refocus on the HUD all the time - far away but the same size"): the readable HUD
// plane distance in metres (was a fixed 1.55). The surface is computed AT this distance to cover the same eye frusta,
// so width/height grow with the distance = the same angular size, only the focus/vergence depth changes.
// Slider: VR HUD > INTERACTION / PROMPTS page "HUD convergence", INI VrHudDistanceR110 (0.75..10.0, default 1.2 since R160 UI = his INI; was 3.0).
// R127 (Miki: "is it then like convergence? a slider for HUD convergence"): yes - the size is angular, so this value only
// sets where both eyes meet on the HUD (vergence). range widened from 1..5 to 0.75..10 m, log scale on the slider.
inline constexpr float kHudConvergenceMinR127=0.75f,kHudConvergenceMaxR127=10.0f;
inline std::atomic<float> hudDistanceR110{1.2f}; // R160 UI: was 3.0
inline float HudDistanceR110() noexcept {const float d=hudDistanceR110.load(std::memory_order_relaxed);return std::isfinite(d)?std::clamp(d,kHudConvergenceMinR127,kHudConvergenceMaxR127):1.2f;}
// R125 (Miki 6 Nov: "the HUD is still slightly double, i need to look closer to see it ... it should look like in
// infinity, flat"). one quad seen by both eyes at D metres has a disparity of IPD/D (69 mm / 5 m = 0.8 deg): while you
// look at the world further away the HUD is out of fusion = double, you must converge on it to read it. ON = xr_manager
// submits two copies of the HUD quad, LEFT eye only and RIGHT eye only, each moved sideways with its own eye, so every
// HUD pixel is in the same direction for both eyes = zero disparity = optical infinity. size, layout, palm reveal and
// the laser stay the same (the angular layout does not change). OFF = the old single quad at the slider distance.
// INI VrHudInfinityR125 (default false since R126: INF gave Miki more eye strain), VR HUD > INTERACTION / PROMPTS distance slider end "INF", Experiments GEAR 3.
inline std::atomic<bool> hudInfinityR125{false};
inline Vec Rotate(Quat q,Vec p) noexcept {
    const Vec t{2*(q.y*p.z-q.z*p.y),2*(q.z*p.x-q.x*p.z),2*(q.x*p.y-q.y*p.x)};
    return {p.x+q.w*t.x+q.y*t.z-q.z*t.y,p.y+q.w*t.y+q.z*t.x-q.x*t.z,p.z+q.w*t.z+q.x*t.y-q.y*t.x};
}
inline bool Valid(const Eye&e) noexcept {
    const auto& f=e.fov;
    const float n=e.orientation.x*e.orientation.x+e.orientation.y*e.orientation.y+e.orientation.z*e.orientation.z+e.orientation.w*e.orientation.w;
    return std::isfinite(n)&&std::fabs(n-1)<.02f&&std::isfinite(e.position.x)&&std::isfinite(e.position.y)&&std::isfinite(e.position.z)&&
        std::isfinite(f.left)&&std::isfinite(f.right)&&std::isfinite(f.up)&&std::isfinite(f.down)&&f.left<0&&f.right>0&&f.down<0&&f.up>0&&f.left>-1.5f&&f.right<1.5f&&f.down>-1.5f&&f.up<1.5f;
}
inline bool Inside(const Eye&e,Vec point,float comfortX,float comfortY) noexcept {
    const Vec p=Rotate({-e.orientation.x,-e.orientation.y,-e.orientation.z,e.orientation.w},{point.x-e.position.x,point.y-e.position.y,point.z-e.position.z});
    if(p.z>=-.05f)return false;
    const float x=p.x/-p.z,y=p.y/-p.z;
    return x>=std::tan(e.fov.left)*comfortX&&x<=std::tan(e.fov.right)*comfortX&&y>=std::tan(e.fov.down)*comfortY&&y<=std::tan(e.fov.up)*comfortY;
}
inline bool BoxInside(const std::array<Eye,2>&eyes,float w,float h,float d,float cx,float cy,float comfortX,float comfortY) noexcept {
    for(const auto&e:eyes)for(int x:{-1,1})for(int y:{-1,1})if(!Inside(e,{cx+x*w*.5f,cy+y*h*.5f,-d},comfortX,comfortY))return false;
    return true;
}
inline Surface Fit(const std::array<Eye,2>&eyes,float aspect,float distance,float maximumWidth,float comfortX=.72f,float comfortY=.60f) noexcept {
    Surface s{};s.distance=distance;s.y=0;
    if(!Valid(eyes[0])||!Valid(eyes[1])||!std::isfinite(aspect)||aspect<.2f||aspect>6||!std::isfinite(distance)||distance<.3f||distance>8||!std::isfinite(maximumWidth)||maximumWidth<=0)return s;
    if(!BoxInside(eyes,0,0,distance,0,0,comfortX,comfortY))return s;
    float low=0,high=maximumWidth;
    for(int i=0;i<24;++i){const float mid=(low+high)*.5f;if(BoxInside(eyes,mid,mid/aspect,distance,0,0,comfortX,comfortY))low=mid;else high=mid;}
    s.width=low*.995f;s.height=s.width/aspect;s.valid=s.width>.15f&&s.height>.1f;return s;
}
inline Hud ComputeHud(const std::array<Eye,2>& eyes,float aspect) noexcept {
    Hud h{};auto&s=h.surface;s.distance=HudDistanceR110(); // R110
    const float scaleR110=s.distance/1.55f; // the old bounds were tuned at 1.55 m: keep their ANGULAR meaning
    if(!Valid(eyes[0])||!Valid(eyes[1])||!std::isfinite(aspect)||aspect<.2f||aspect>6)return h;
    float halfW=.2f,halfH=.2f;
    for(const auto&e:eyes)for(float tx:{std::tan(e.fov.left),std::tan(e.fov.right)})for(float ty:{std::tan(e.fov.down),std::tan(e.fov.up)}){
        const auto ray=Rotate(e.orientation,{tx,ty,-1});if(ray.z>=-.02f)return h;
        const float t=(-s.distance-e.position.z)/ray.z;if(t<=0||!std::isfinite(t))return h;
        halfW=std::max(halfW,std::fabs(e.position.x+t*ray.x-s.x));halfH=std::max(halfH,std::fabs(e.position.y+t*ray.y-s.y));
    }
    s.width=2*std::max(halfW,halfH*aspect)*1.035f;s.height=s.width/aspect;
    if(!std::isfinite(s.width)||s.width>30*scaleR110||s.height>30*scaleR110)return h;
    s.valid=true;
    // Readable content uses a binocular comfort box on THIS panel. Effects retain
    // the full panel coverage. No source-camera FOV, world zoom, or world UV center.
    // V66.96b headset result: the old 72%/60% comfort box made readable
    // HUD and loot panels unnecessarily small. Use a still binocular-verified,
    // but less conservative box; BoxInside continues to enforce visibility in
    // both eyes and field effects still use the full surface.
    const auto fitted=Fit(eyes,StageWidth/StageHeight,s.distance,8.f*scaleR110,.88f,.78f);
    if(!fitted.valid){s.valid=false;return h;}
    h.safe.left=(.5f+(-fitted.width*.5f-s.x)/s.width)*StageWidth;
    h.safe.right=(.5f+(fitted.width*.5f-s.x)/s.width)*StageWidth;
    h.safe.top=(.5f-(fitted.height*.5f-s.y)/s.height)*StageHeight;
    h.safe.bottom=(.5f-(-fitted.height*.5f-s.y)/s.height)*StageHeight;
    s.valid=h.safe.left>=0&&h.safe.right<=StageWidth&&h.safe.top>=0&&h.safe.bottom<=StageHeight;
    return h;
}
// Physical head movement is not a HUD resize event. Cache optical eye geometry
// for the session; only source aspect or a real eye-FOV change invalidates it.
// Keep the same latched surface for drawing, layout AND pointer inverse mapping.
class HudLatch {
    Hud value_{};std::array<Fov,2> fov_{};float aspect_=0,distance_=0;bool ready_=false;
public:
    const Hud& Read() const noexcept {return value_;}
    bool Update(const std::array<Eye,2>& eyes,float aspect) noexcept {
        if(!Valid(eyes[0])||!Valid(eyes[1]))return false; // tracking gap is not a resize
        bool changed=!ready_||std::fabs(aspect-aspect_)>.0005f||std::fabs(HudDistanceR110()-distance_)>.005f; // R110: slider moves rebuild
        for(unsigned i=0;i<2;++i){const auto&a=eyes[i].fov;const auto&b=fov_[i];
            changed|=std::fabs(a.left-b.left)>.002f||std::fabs(a.right-b.right)>.002f||
                     std::fabs(a.up-b.up)>.002f||std::fabs(a.down-b.down)>.002f;}
        if(!changed)return false;
        const auto candidate=ComputeHud(eyes,aspect);if(!candidate.surface.valid)return false;
        value_=candidate;aspect_=aspect;distance_=candidate.surface.distance;for(unsigned i=0;i<2;++i)fov_[i]=eyes[i].fov;ready_=true;return true;
    }
    void Reset() noexcept {ready_=false;value_={};}
};
} // namespace
