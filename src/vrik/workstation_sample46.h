#pragma once
#include <cmath>
#include <cstdint>

namespace kcd2vr::workstation_sample46 {
// A single copied tracking frame for native activity gestures. Positions are
// OpenXR metres, +Y up; hand 0 is right and hand 1 is left. These are input
// observations, not collision, attachment or gameplay authority.
struct Hand {
    bool valid=false;
    float position[3]{},rotation[4]{0,0,0,1};
    float grip=0,trigger=0;
};
struct Frame {
    std::uint64_t sequence=0,tick=0,timeNs=0,poseEpoch=0,sourceEpoch=0;
    bool allowed=false,headValid=false;
    float head[3]{},headQ[4]{0,0,0,1};
    Hand hand[2]{};
};
inline bool finite(const Hand& h) noexcept {
    if(!std::isfinite(h.grip)||!std::isfinite(h.trigger))return false;
    for(float v:h.position)if(!std::isfinite(v))return false;
    double n=0;for(float v:h.rotation){if(!std::isfinite(v))return false;n+=double(v)*v;}
    return n>.90&&n<1.10;
}
inline bool fresh(const Frame& f,std::uint64_t now) noexcept {
    return f.sequence&&f.tick&&f.timeNs&&f.poseEpoch&&f.sourceEpoch&&
        now>=f.tick&&now-f.tick<=80&&f.headValid;
}
// Production adapter, also exercised with the real tracking POD in host tests.
// Never promotes the presentation-only short dropout hold into new gesture work.
template<class Tracking>
inline bool copy(const Tracking& t,std::uint64_t poseEpoch,std::uint64_t sourceEpoch,
                 std::uint64_t now,Frame& out) noexcept {
    out=Frame{};
    if(!poseEpoch||!sourceEpoch||t.source_epoch!=sourceEpoch||!t.sequence||!t.tick||
       !t.timeNs||now<t.tick||now-t.tick>80||!t.head_valid||t.head_held)return false;
    Frame f{};f.sequence=t.sequence;f.tick=t.tick;f.timeNs=t.timeNs;
    f.poseEpoch=poseEpoch;f.sourceEpoch=sourceEpoch;f.allowed=t.allowed;
    f.head[0]=t.head.x;f.head[1]=t.head.y;f.head[2]=t.head.z;
    f.headQ[0]=t.hq.x;f.headQ[1]=t.hq.y;f.headQ[2]=t.hq.z;f.headQ[3]=t.hq.w;
    double qn=0;for(float v:f.headQ){if(!std::isfinite(v))return false;qn+=double(v)*v;}
    for(float v:f.head)if(!std::isfinite(v))return false;
    if(qn<=.90||qn>=1.10)return false;
    f.headValid=true;
    for(int side=0;side<2;++side){
        const auto& p=side?t.left:t.right;const auto& q=side?t.lq:t.rq;
        auto& h=f.hand[side];h.position[0]=p.x;h.position[1]=p.y;h.position[2]=p.z;
        h.rotation[0]=q.x;h.rotation[1]=q.y;h.rotation[2]=q.z;h.rotation[3]=q.w;
        h.grip=side?t.left_grip:t.right_grip;h.trigger=side?t.left_trigger:t.right_trigger;
        h.valid=(side?(t.left_valid&&!t.left_held):(t.right_valid&&!t.right_held))&&finite(h);
        if(!h.valid)h=Hand{};
    }
    out=f;return true;
}
}
