#pragma once
#include "hand_select_d85.h"
#include "../../vrik/visual_ray_d75.h"
#include <algorithm>

namespace kcd2vr::dice_marker_r92 {
// Native game-thread selection only. No cursor writes, RNG or scoring here.
struct Selection { float world[3]{}; std::uintptr_t instance=0; std::uint64_t visit=0,lifetime=0,tick=0; int index=-1; bool valid=false; };
inline core::SnapshotMailbox<Selection> selected;
struct Presented {std::uintptr_t instance=0;std::uint64_t visit=0,lifetime=0,tick=0;int index=-1;bool valid=false;};
inline core::SnapshotMailbox<Presented> presented;
inline core::SnapshotMailbox<Presented> prepared; // current pose + atlas available; no rendered-success claim
inline core::SnapshotMailbox<Presented> hidden; // exact native cursor hidden before replacement is allowed
inline std::atomic<bool> flatOwnerReady{false};
inline bool fresh(const Selection& s,std::uint64_t now) noexcept {
    return s.valid&&s.instance&&s.visit&&s.tick&&now>=s.tick&&now-s.tick<=80;
}
inline Selection selection(const dice_hands_d85::Turn& t,std::uint64_t visit,std::uint64_t lifetime,std::uint64_t now) noexcept {
    Selection s{};
    if(t.cursor<0||unsigned(t.cursor)>=t.count||unsigned(t.cursor)>=dice_hands_d85::kMaxDice)return s;
    const auto& d=t.dice[t.cursor];if(!d.pos||d.index!=t.cursor||d.state==2)return s;
    std::copy_n(d.p,3,s.world);s.instance=t.instance;s.visit=visit;s.lifetime=lifetime;s.tick=now;s.index=t.cursor;s.valid=true;return s;
}
inline bool ownsFlatCursor(const Selection& s,const Presented& p,std::uint64_t now) noexcept {
    return fresh(s,now)&&p.valid&&p.tick&&now>=p.tick&&now-p.tick<=80&&
        p.instance==s.instance&&p.visit==s.visit&&p.lifetime==s.lifetime&&p.index==s.index;
}
// Convert a real world point into the *recorded* center camera's OpenXR axes.
// The caller then uses that camera's exact XR pose frame, not a newer head pose.
inline bool viewPoint(const Selection& s,const visual_ray_d75::Sample& c,float out[3]) noexcept {
    if(!s.valid||!c.valid||!c.poseFrame||!std::isfinite(c.worldScaleR92)||c.worldScaleR92<=0.f)return false;
    float d[3]{};for(unsigned i=0;i<3;++i){d[i]=s.world[i]-c.origin[i];if(!std::isfinite(d[i]))return false;}
    out[0]=out[1]=out[2]=0;
    for(unsigned i=0;i<3;++i){out[0]+=d[i]*c.rightR92[i];out[1]+=d[i]*c.upR92[i];out[2]-=d[i]*c.forward[i];}
    // World distances include the captured HMD/world scale; OpenXR local space does not.
    for(unsigned i=0;i<3;++i)out[i]/=c.worldScaleR92;
    return std::isfinite(out[0])&&std::isfinite(out[1])&&std::isfinite(out[2])&&out[2]<-.05f;
}
inline constexpr int atlasX=8,atlasY=0,atlasSize=32;
inline void writeRingBgra(std::uint8_t* pixels,int width,int height) noexcept {
    if(!pixels||width<atlasX+atlasSize||height<atlasSize)return;
    for(int y=0;y<atlasSize;++y)for(int x=0;x<atlasSize;++x){
        const float dx=x+.5f-atlasSize*.5f,dy=y+.5f-atlasSize*.5f;
        const float r=std::sqrt(dx*dx+dy*dy);
        const float coverage=std::clamp(std::min(r-11.5f,15.5f-r),0.f,1.f);
        auto* p=pixels+4*((y+atlasY)*width+x+atlasX);
        // Premultiplied alpha, matching the other menu compositor samples.
        p[0]=std::uint8_t(76*coverage);p[1]=std::uint8_t(157*coverage);p[2]=std::uint8_t(194*coverage);p[3]=std::uint8_t(255*coverage);
    }
}
}
