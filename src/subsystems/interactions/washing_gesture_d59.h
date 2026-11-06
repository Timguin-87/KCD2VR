#pragma once
#include <cstdint>
#include <cmath>
#include <algorithm>
namespace kcd2vr::washing_d59 {
struct Frame {
    std::uint32_t player=0,target=0;
    std::uintptr_t actor=0,character=0;
    std::uint64_t body=0,tracking=0,policy=0,tick=0,hmdTick=0,handTick[2]{},targetTick=0;
    float palm[2][3]{},hmd[3]{},forward[3]{};
    unsigned openEmptyMask=0;
    bool allowed=false;
};
struct Volume {
    std::uint32_t target=0,smartObject=0; // Zero SO: retail Animation trigger explicitly has no linked object.
    std::uint64_t tick=0;
    float lo[3]{},hi[3]{};
};
inline bool Fresh(std::uint64_t tick,std::uint64_t now,std::uint64_t age) noexcept {
    return tick&&now>=tick&&now-tick<=age;
}
inline bool Current(const Frame& f,std::uint64_t now) noexcept {
    if(!f.allowed||!f.player||!f.actor||!f.character||!f.body||!f.tracking||!f.policy||
       (f.openEmptyMask&3)!=3||!Fresh(f.tick,now,80)||!Fresh(f.hmdTick,now,60)||
       !Fresh(f.handTick[0],now,80)||!Fresh(f.handTick[1],now,80))return false;
    float n=0;
    for(unsigned j=0;j<3;++j){
        if(!std::isfinite(f.hmd[j])||!std::isfinite(f.forward[j])||
           !std::isfinite(f.palm[0][j])||!std::isfinite(f.palm[1][j]))return false;
        n+=f.forward[j]*f.forward[j];
    }
    return n>.8f&&n<1.2f;
}
inline bool SameOwner(const Frame&a,const Frame&b) noexcept {
    return a.player==b.player&&a.actor==b.actor&&a.character==b.character&&
        a.body==b.body&&a.tracking==b.tracking&&a.policy==b.policy;
}
inline bool Valid(const Volume& v) noexcept {
    // Native ActionTrigger accepts Animation without a linked smart object.
    // The fresh Lua READY lease authorizes that zero; target/bounds still apply.
    if(!v.target||!v.tick)return false;
    for(unsigned j=0;j<3;++j){const float extent=v.hi[j]-v.lo[j];
        if(!std::isfinite(v.lo[j])||!std::isfinite(v.hi[j])||extent<.03f||extent>1.6f)return false;
    }
    return true;
}
inline float DistanceSquared(const float*a,const float*b) noexcept {
    float d=0;for(unsigned j=0;j<3;++j){const float q=a[j]-b[j];d+=q*q;}return d;
}
// This is the upper part of the native authored WaterTubeActionTrigger volume,
// NOT a claimed physical water mesh. Narrow class/descriptor/size qualification
// lives in Lua. No all-world water search or guessed native collision write.
inline bool InScoopVolume(const float* p,const Volume& v) noexcept {
    return p[0]>=v.lo[0]-.04f&&p[0]<=v.hi[0]+.04f&&
           p[1]>=v.lo[1]-.04f&&p[1]<=v.hi[1]+.04f&&
           p[2]>=v.lo[2]+(v.hi[2]-v.lo[2])*.55f-.04f&&p[2]<=v.hi[2]+.08f;
}
inline bool NearFace(const float* p,const Frame& f) noexcept {
    float face[3]{};for(unsigned j=0;j<3;++j)face[j]=f.hmd[j]+.12f*f.forward[j];
    face[2]-=.06f;
    return DistanceSquared(p,face)<=.27f*.27f;
}
enum class Step : unsigned { Waiting, Dipped, Face, Cancelled, Consumed };
struct Gesture {
    Frame owner{};Volume volume{};
    std::uint64_t begun=0,last=0,inside[2]{},dipped[2]{},face=0,dry=0;
    float dipZ[2]{}; bool consumed=false;
    void Reset() noexcept {*this={};}
    bool Begin(const Frame& f,const Volume& v,std::uint64_t now) noexcept {
        Reset();if(!Current(f,now)||!Valid(v)||!Fresh(v.tick,now,200)||f.target!=v.target||
           !Fresh(f.targetTick,now,120))return false;
        owner=f;volume=v;begun=now;return true;
    }
    Step Advance(const Frame& f,const Volume& v,std::uint64_t now) noexcept {
        if(!begun)return Step::Waiting;
        if(!Current(f,now)||!SameOwner(owner,f)||!Valid(v)||!Fresh(v.tick,now,200)||
           volume.target!=v.target||volume.smartObject!=v.smartObject||now<begun||
           now-begun>8000||!Fresh(last?last:begun,now,80)) {Reset();return Step::Cancelled;}
        for(unsigned j=0;j<3;++j)if(std::fabs(volume.lo[j]-v.lo[j])>.03f||std::fabs(volume.hi[j]-v.hi[j])>.03f){Reset();return Step::Cancelled;}
        // Walking away cancels the uncommitted gesture even if the engine's
        // action happens to have an unlimited active distance.
        float centre[3]{};for(unsigned j=0;j<3;++j)centre[j]=(v.lo[j]+v.hi[j])*.5f;
        if(DistanceSquared(f.hmd,centre)>2.0f*2.0f){Reset();return Step::Cancelled;}
        if(f.tick==last)return consumed?Step::Consumed:Step::Waiting;
        last=f.tick;
        if(consumed){
            const bool outside=!InScoopVolume(f.palm[0],v)&&!InScoopVolume(f.palm[1],v)&&
                !NearFace(f.palm[0],f)&&!NearFace(f.palm[1],f);
            if(!outside)dry=0;else if(!dry)dry=now;
            if(dry&&now-dry>=250){Reset();return Step::Cancelled;}
            return Step::Consumed;
        }
        for(unsigned i=0;i<2;++i){
            if(InScoopVolume(f.palm[i],v)){
                if(!inside[i])inside[i]=now;
                if(now-inside[i]>=100){dipped[i]=now;dipZ[i]=f.palm[i][2];}
            }else inside[i]=0;
            if(dipped[i]&&now-dipped[i]>4000){dipped[i]=0;face=0;}
        }
        const bool wet=dipped[0]&&dipped[1];
        const bool atFace=wet&&NearFace(f.palm[0],f)&&NearFace(f.palm[1],f)&&
            f.palm[0][2]-dipZ[0]>=.18f&&f.palm[1][2]-dipZ[1]>=.18f&&
            DistanceSquared(f.palm[0],f.palm[1])<=.60f*.60f;
        if(!atFace)face=0;else if(!face)face=now;
        if(face&&now-face>=70){consumed=true;return Step::Face;}
        return wet?Step::Dipped:Step::Waiting;
    }
};
} // namespace kcd2vr::washing_d59
