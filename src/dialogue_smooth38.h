#pragma once
#include <cmath>
#include <cstdint>
#include "dialogue_state59c10r2.h"

// C38 smooth dialogue eye BASE (pure, host tested).
// The headset view is still composed from the HMD on every render publication
// (camera_hook, once, after this base). Only the low-frequency native anchor -
// Henry's model transform sampled by the skinning callback at game/animation
// rate - is followed with a short time-based low-pass, so its 25-60 Hz steps and
// sample-phase jitter no longer reach the eyes. A short hold keeps the last
// smoothed eye of the SAME active dialogue while the head/policy/state sample is
// merely late (a game hitch), instead of alternating to the native animated view
// for a frame (the C37C2 log: EMBODIED <-> NO_FRESH_HEAD flips, 13:58:59).
// Visual base only: never gameplay/input/body permission, never a native write.
namespace kcd2vr::dialogue_smooth38 {
namespace ds=dialogue_state59c10r2;
struct Config {
    float tauMs=70.f;             // low-pass time constant of the anchor
    float maxStepMs=34.f;         // a late frame is smoothed like ~2 frames, not snapped
    float snapMeters=.45f;        // a real teleport/reposition is taken at once
    float snapCos=.82f;           // ~35 degrees per basis axis
    std::uint64_t gapNs=300000000ull; // no sample for 300 ms: restart from the target
    std::uint64_t holdMs=60;     // max age of the last applied eye that may be held
};
struct State {
    bool valid=false;
    std::uintptr_t entity=0,character=0,instance=0;
    std::uint64_t ns=0,appliedMs=0;
    ds::Eye eye{};
    std::uint32_t holds=0,snaps=0,steps=0;
    float lagM=0;
};
inline float len(ds::V a) noexcept {return std::sqrt(ds::dot(a,a));}
inline bool normalize(ds::V& a) noexcept {const float l=len(a);if(!std::isfinite(l)||l<1e-6f)return false;a=ds::mul(a,1/l);return true;}
// Gram-Schmidt keeping basis[0] (forward) first; right-handed like ds::valid.
inline bool orthonormal(ds::Eye& e) noexcept {
    ds::V b0=e.basis[0],b1=e.basis[1];
    if(!normalize(b0))return false;
    b1=ds::sub(b1,ds::mul(b0,ds::dot(b0,b1)));if(!normalize(b1))return false;
    e.basis[0]=b0;e.basis[1]=b1;e.basis[2]=ds::cross(b0,b1);return ds::valid(e);
}
enum class Step : unsigned {Snap,Smoothed};
inline Step follow(State& s,const ds::Eye& target,std::uintptr_t entity,std::uintptr_t character,std::uintptr_t instance,
    std::uint64_t nowNs,std::uint64_t nowMs,const Config& c,ds::Eye& out) noexcept {
    bool snap=!s.valid||s.entity!=entity||s.character!=character||s.instance!=instance||nowNs<s.ns||nowNs-s.ns>c.gapNs||
        !ds::valid(s.eye)||len(ds::sub(target.position,s.eye.position))>c.snapMeters;
    for(unsigned i=0;!snap&&i<3;++i)if(ds::dot(target.basis[i],s.eye.basis[i])<c.snapCos)snap=true;
    ds::Eye next=target;
    if(!snap){
        float dtMs=float(double(nowNs-s.ns)/1e6);if(dtMs>c.maxStepMs)dtMs=c.maxStepMs;
        const float a=c.tauMs>0?1.f-std::exp(-dtMs/c.tauMs):1.f;
        next.position=ds::add(s.eye.position,ds::mul(ds::sub(target.position,s.eye.position),a));
        for(unsigned i=0;i<3;++i)next.basis[i]=ds::add(s.eye.basis[i],ds::mul(ds::sub(target.basis[i],s.eye.basis[i]),a));
        if(!orthonormal(next)){next=target;snap=true;}
    }
    if(snap)++s.snaps;else ++s.steps;
    s.valid=true;s.entity=entity;s.character=character;s.instance=instance;s.ns=nowNs;s.appliedMs=nowMs;s.eye=next;
    s.lagM=len(ds::sub(target.position,next.position));
    out=next;return snap?Step::Snap:Step::Smoothed;
}
// Only lateness may be bridged: the dialogue must still be the same, active,
// first-person/dialog-camera owned one and VRIK dialogue policy still enabled.
inline bool hold(State& s,const ds::State& st,const ds::Policy& policy,std::uint64_t nowMs,const Config& c,ds::Eye& out) noexcept {
    if(!s.valid||!st.known||!st.active||!st.entity||st.entity!=s.entity||st.instance!=s.instance||
       (st.camera!=ds::Camera::FirstPerson&&st.camera!=ds::Camera::Dialogue)||!ds::age(st.tick,nowMs,c.holdMs)||
       !policy.enabled||!ds::age(policy.tick,nowMs,c.holdMs)||!ds::age(s.appliedMs,nowMs,c.holdMs)||!ds::valid(s.eye))return false;
    ++s.holds;out=s.eye;return true; // appliedMs is NOT renewed: a hold cannot extend itself
}
}
