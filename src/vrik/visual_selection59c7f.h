#pragma once
#include "vrik/hand_present_d61.h"
#include "vrik/hand_present_d62.h" // namespace-owning helper, before private pose adapter includes
namespace kcd2vr::visual_selection59c7f {
// The production census solver and the retired skin lab are mutually exclusive.
// This is selection only; tracking, UI and native paired-action guards still own
// permission to write a pose. Reading these copied mode values does no native work.
inline bool Selected(bool calibrated,int census,int skin) noexcept {
    return calibrated&&(census>=2||skin>=3);
}
}

// Optional visual tracking policy shares this owner; raw tracking is unchanged.
// Optional one-euro filter for a free hand's visual controller target (default OFF).
// It runs before visual goal/contact/reach/wrist solves. Raw tracking, physical
// kernel targets, interaction requests, H30 intent, HMD and body policy stay raw.
// History belongs to one coherent body, is staged per transaction, and advances
// only after native restoration and accepted paired publication. Current acquired/native owners bypass it.
// Legacy H50 fast motion bypasses on its first sample. The explicit adaptive
// experiment keeps its response at speed; its existing lag caps remain.
#include <atomic>
#include <cmath>
#include <cstdint>
namespace kcd2vr::hand_smooth_h50 {
inline std::atomic<std::uint64_t> policyEpoch{1};
inline std::atomic<bool> enabled{false}; // live experiment preference; persisted as VrHandSmoothH50
inline std::atomic<float> minCutoffHz{1.5f};   // cutoff at rest (lower = smoother, more lag on slow moves)
inline std::atomic<float> beta{20.f};          // + Hz per m/s of hand speed
inline std::atomic<float> rotBeta{5.f};        // + Hz per rad/s of wrist turn speed
constexpr float kDerivCutoffHz=5.f;            // speed estimate filter
constexpr float kBypassRawSpeed=1.0f;          // m/s, RAW sample speed: above = this sample is raw (stroke onset)
constexpr float kBypassRawTurn=4.f;            // rad/s, RAW sample turn rate
constexpr float kMaxLagM=.02f;                 // never more than 2 cm behind the controller
constexpr float kMaxLagRad=.1745f;             // never more than 10 degrees behind
constexpr float kResetJumpM=.25f;              // one-sample jump = recalibration/teleport -> restart from raw
constexpr std::uint64_t kResetGapNs=100000000; // 100 ms without samples -> restart from raw

struct State {bool ready=false;std::uint64_t t=0,epoch=0,policy=0;float raw[3]{},out[3]{},dv[3]{};float rawQ[4]{0,0,0,1},outQ[4]{0,0,0,1};float dw=0;};

inline float Alpha(float hz,float dt) noexcept {const float tau=1.f/(6.2831853f*hz);return 1.f/(1.f+tau/dt);}
inline float QDot(const float*a,const float*b) noexcept {return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3];}
inline float QAngle(const float*a,const float*b) noexcept {const float d=std::fabs(QDot(a,b));return 2.f*std::acos(d>1.f?1.f:d);}
inline void QToward(const float*a,const float*b,float t,float*o) noexcept {
    float bb[4]{b[0],b[1],b[2],b[3]};if(QDot(a,b)<0)for(float&v:bb)v=-v;
    const float d=std::fabs(QDot(a,b));
    if(d>.9995f){for(int i=0;i<4;++i)o[i]=a[i]+(bb[i]-a[i])*t;}
    else{const float th=std::acos(d),sn=std::sin(th),wa=std::sin((1-t)*th)/sn,wb=std::sin(t*th)/sn;for(int i=0;i<4;++i)o[i]=a[i]*wa+bb[i]*wb;}
    const float n=std::sqrt(QDot(o,o));if(n>1e-6f)for(int i=0;i<4;++i)o[i]/=n;
}

// one sample on a STAGED copy. raw = controller position (m, tracking space) + rotation. returns false = use raw
// (disabled/invalid). same sample twice = same answer.
inline bool Step(State&s,std::uint64_t epoch,std::uint64_t timeNs,const float raw[3],const float rawQ[4],float outP[3],float outQ[4],bool adaptiveFast=false) noexcept {
    if(!enabled.load(std::memory_order_relaxed)){s.ready=false;return false;}
    for(int i=0;i<3;++i)if(!std::isfinite(raw[i]))return false;
    for(int i=0;i<4;++i)if(!std::isfinite(rawQ[i]))return false;
    const auto restart=[&]{s.ready=true;s.t=timeNs;s.epoch=epoch;s.policy=policyEpoch.load(std::memory_order_relaxed);s.dw=0;
        for(int i=0;i<3;++i){s.raw[i]=s.out[i]=raw[i];s.dv[i]=0;}for(int i=0;i<4;++i)s.rawQ[i]=s.outQ[i]=rawQ[i];};
    float jump=0;for(int i=0;i<3;++i)jump+=(raw[i]-s.raw[i])*(raw[i]-s.raw[i]);jump=std::sqrt(jump);
    if(!s.ready||s.policy!=policyEpoch.load(std::memory_order_relaxed)||epoch!=s.epoch||timeNs<s.t||timeNs-s.t>kResetGapNs||jump>kResetJumpM)restart();
    else if(timeNs>s.t){
        const float dt=float(timeNs-s.t)*1e-9f;
        const float aD=Alpha(kDerivCutoffHz,dt);
        const float rawSpeed=jump/dt;
        float speed=0;
        for(int i=0;i<3;++i){const float v=(raw[i]-s.raw[i])/dt;s.dv[i]+=(v-s.dv[i])*aD;speed+=s.dv[i]*s.dv[i];}
        speed=std::sqrt(speed);
        const float minHz=std::fmax(.2f,minCutoffHz.load(std::memory_order_relaxed));
        const float b=std::fmax(0.f,beta.load(std::memory_order_relaxed));
        if(!adaptiveFast&&rawSpeed>kBypassRawSpeed)for(int i=0;i<3;++i)s.out[i]=raw[i];
        else{const float a=Alpha(minHz+b*speed,dt);for(int i=0;i<3;++i)s.out[i]+=(raw[i]-s.out[i])*a;}
        float lag=0;for(int i=0;i<3;++i)lag+=(s.out[i]-raw[i])*(s.out[i]-raw[i]);lag=std::sqrt(lag);
        if(lag>kMaxLagM){const float k=kMaxLagM/lag;for(int i=0;i<3;++i)s.out[i]=raw[i]+(s.out[i]-raw[i])*k;}
        const float rawTurn=QAngle(rawQ,s.rawQ)/dt;s.dw+=(rawTurn-s.dw)*aD;
        const float rb=std::fmax(0.f,rotBeta.load(std::memory_order_relaxed));
        if(!adaptiveFast&&rawTurn>kBypassRawTurn)for(int i=0;i<4;++i)s.outQ[i]=rawQ[i];
        else{float q[4];QToward(s.outQ,rawQ,Alpha(minHz+rb*s.dw,dt),q);for(int i=0;i<4;++i)s.outQ[i]=q[i];}
        const float behind=QAngle(s.outQ,rawQ);
        if(behind>kMaxLagRad){float q[4];QToward(s.outQ,rawQ,1.f-kMaxLagRad/behind,q);for(int i=0;i<4;++i)s.outQ[i]=q[i];}
        s.t=timeNs;for(int i=0;i<3;++i)s.raw[i]=raw[i];for(int i=0;i<4;++i)s.rawQ[i]=rawQ[i];
    }
    for(int i=0;i<3;++i)outP[i]=s.out[i];
    for(int i=0;i<4;++i)outQ[i]=s.outQ[i];
    return true;
}
// called only on the committed path. a hand that is not free now restarts from raw next time it is free.
inline void Commit(State state[2],bool freeLast[2],const State staged[2],const bool freeNow[2]) noexcept {
    for(int side=0;side<2;++side){state[side]=staged[side];if(!freeNow[side])state[side].ready=false;freeLast[side]=freeNow[side];}
}
}
