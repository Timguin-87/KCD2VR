#pragma once
#include "selection_marker_r92.h"
#include "../../ui_surface_geometry.h"

namespace kcd2vr::dice_hud_depth_r92r3 {
// Dice-only compositor data. No native Flash transforms, camera writes, or HUD
// layout/latch changes. The caller joins the source XR pose before changing depth.
inline ui_surface_geometry::Surface computeSurface(const std::array<ui_surface_geometry::Eye,2>& eyes,
                                                   float aspect,float depth) noexcept {
    namespace ui=ui_surface_geometry;
    ui::Surface s{};
    if(!ui::Valid(eyes[0])||!ui::Valid(eyes[1])||!std::isfinite(aspect)||aspect<.2f||aspect>6.f||
       !std::isfinite(depth)||depth<=.05f)return s;
    s.distance=std::clamp(depth,.45f,1.55f);
    const float ratio=s.distance/1.55f;
    s.y=-.02f*ratio;
    float halfW=.2f*ratio,halfH=.2f*ratio;
    // Recompute coverage at the new depth: proportional width alone would miss
    // the non-scaling eye separation and can expose full-field mask edges.
    for(const auto& e:eyes)
        for(float tx:{std::tan(e.fov.left),std::tan(e.fov.right)})
            for(float ty:{std::tan(e.fov.down),std::tan(e.fov.up)}){
                const auto ray=ui::Rotate(e.orientation,{tx,ty,-1});
                if(ray.z>=-.02f)return s;
                const float t=(-s.distance-e.position.z)/ray.z;
                if(t<=0.f||!std::isfinite(t))return s;
                halfW=std::max(halfW,std::fabs(e.position.x+t*ray.x-s.x));
                halfH=std::max(halfH,std::fabs(e.position.y+t*ray.y-s.y));
            }
    s.width=2.f*std::max(halfW,halfH*aspect)*1.035f;s.height=s.width/aspect;
    s.valid=std::isfinite(s.width)&&std::isfinite(s.height)&&s.width>0.f&&s.height>0.f&&s.width<=30.f&&s.height<=30.f;
    return s;
}

struct Receipt {
    std::uintptr_t instance=0;
    std::uint64_t visit=0,lifetime=0,tick=0;
    float depth=0,width=0,height=0;
    bool valid=false;
};
inline core::SnapshotMailbox<Receipt> applied;
inline bool joined(const dice_marker_r92::Selection& s,const Receipt& r,std::uint64_t now) noexcept {
    return dice_marker_r92::fresh(s,now)&&r.valid&&r.tick&&now>=r.tick&&now-r.tick<=80&&
        r.instance==s.instance&&r.visit==s.visit&&r.lifetime==s.lifetime&&
        std::isfinite(r.depth)&&r.depth>=.45f&&r.depth<=1.55f&&
        std::isfinite(r.width)&&std::isfinite(r.height)&&r.width>0&&r.height>0;
}
}
