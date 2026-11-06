#pragma once
#include <cmath>
#include <cstdint>

namespace kcd2vr::petting_d59 {
// Input snapshots only. No native ABI, pose writes, Grip consumption or physics.
struct Hand {
    bool tracked=false,empty=false,free=false;
    float grip=0,trigger=0,palm[3]{};
    std::uint32_t target=0;std::uint64_t candidateTick=0,candidateGeneration=0,candidateContext=0;
    float distance=0;bool dogCandidate=false;
};
struct Frame {
    std::uint64_t tick=0,body=0,tracking=0,policy=0;
    std::uintptr_t playerIdentity=0;std::uint32_t player=0;
    bool permitted=false;Hand hand[2]{};
};
inline bool Fresh(std::uint64_t then,std::uint64_t now) noexcept {
    return then&&now>=then&&now-then<=80;
}
inline bool Same(const Frame&a,const Frame&b) noexcept {
    return a.player&&a.playerIdentity&&a.body&&a.tracking&&a.policy&&
        a.player==b.player&&a.playerIdentity==b.playerIdentity&&a.body==b.body&&
        a.tracking==b.tracking&&a.policy==b.policy;
}
inline bool Point(const Hand& h) noexcept {
    for(float v:h.palm)if(!std::isfinite(v))return false;
    return h.tracked;
}
inline float Distance(const float*a,const float*b) noexcept {
    float d=0;for(int i=0;i<3;++i){const float v=a[i]-b[i];d+=v*v;}return std::sqrt(d);
}
inline bool Eligible(const Frame&f,int side,std::uint64_t now) noexcept {
    if(side<0||side>1||!f.permitted||!Same(f,f)||!Fresh(f.tick,now))return false;
    const auto&h=f.hand[side];
    return Point(h)&&h.empty&&h.free&&std::isfinite(h.grip)&&std::isfinite(h.trigger)&&
        h.grip>=0&&h.grip<.35f&&h.trigger>=0&&h.trigger<.35f&&h.dogCandidate&&h.target&&
        h.candidateGeneration&&h.candidateContext&&Fresh(h.candidateTick,now)&&std::isfinite(h.distance)&&h.distance>=0&&h.distance<=.20f;
}
struct Stroke {
    std::uint64_t start=0,last=0,generation=0,context=0;std::uint32_t target=0;
    float first[3]{},previous[3]{},travel=0;
    void reset() noexcept {*this={};}
    bool update(const Frame&f,int side) noexcept {
        if(!Eligible(f,side,f.tick)){reset();return false;}
        const auto&h=f.hand[side];
        if(target!=h.target||generation!=h.candidateGeneration||context!=h.candidateContext||
           !start||f.tick<last||f.tick-last>80||f.tick-start>1500){
            reset();target=h.target;generation=h.candidateGeneration;context=h.candidateContext;start=last=f.tick;
            for(int i=0;i<3;++i)first[i]=previous[i]=h.palm[i];return false;
        }
        if(f.tick==last)return false;
        const auto step=Distance(previous,h.palm);
        const float seconds=float(f.tick-last)*.001f;
        if(!std::isfinite(step)||step>.12f||step/seconds>1.75f){reset();return false;}
        last=f.tick;for(int i=0;i<3;++i)previous[i]=h.palm[i];
        // Integrate actual travel, but require net movement as well so tracking
        // jitter or tiny oscillations cannot masquerade as a caress.
        travel+=step;
        return f.tick-start>=120&&travel>=.08f&&Distance(first,h.palm)>=.06f;
    }
};
struct Recognizer {
    Frame identity{};Stroke strokes[2]{};
    bool latched=false;float anchor[3]{};std::uint64_t awaySince=0,awayLast=0;
    void reset() noexcept {*this=Recognizer{};}
    int update(const Frame&f) noexcept {
        // A refused body read publishes an all-zero identity. It cancels an
        // unfinished stroke, but cannot undo an already consumed native request
        // or count unobserved time toward withdrawal. Only a positively valid
        // new identity/policy or tracked withdrawal may rearm the episode.
        if(!f.permitted||!Same(f,f)||!Fresh(f.tick,f.tick)){
            strokes[0].reset();strokes[1].reset();awaySince=awayLast=0;return -1;
        }
        if(!Same(identity,f)){reset();identity=f;}
        if(latched){
            // Both hands may be offered the same dog. A consumed episode is
            // global, and only positive tracked withdrawal rearms it. A stale
            // selector, tracking loss or native animation cannot replay Bond.
            bool saw=false,away=true;
            for(const auto&h:f.hand){if(!Point(h)){away=false;continue;}
                saw=true;if(Distance(anchor,h.palm)<.32f)away=false;}
            if(away&&saw&&Fresh(f.tick,f.tick)){
                // D60: silence between valid endpoints is not observed withdrawal.
                // Native ownership/stalls may skip the entire input callback.
                if(!awayLast||f.tick<awayLast||f.tick-awayLast>80)awaySince=f.tick;
                awayLast=f.tick;
                if(f.tick>=awaySince&&f.tick-awaySince>=180){latched=false;awaySince=awayLast=0;}
            }else awaySince=awayLast=0;
            return -1;
        }
        for(int side=0;side<2;++side)if(strokes[side].update(f,side)){
            latched=true;awaySince=0;
            for(int i=0;i<3;++i)anchor[i]=f.hand[side].palm[i];
            strokes[0].reset();strokes[1].reset();return side;
        }
        return -1;
    }
};
}
