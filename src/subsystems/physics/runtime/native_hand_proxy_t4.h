#pragma once
#include "vrik/pose_math.h"
#include <cmath>
#include <cstdint>
// DS-F55 (after Codex F47 review): the hand grip proxy owner, restructured on
// the audited T4 record/lifetime protocol. The POLICY only REQUESTS
// Create/Retire; Active/Off require EXPLICIT native progress receipts:
// - adopted  = AdoptCreatedRigid succeeded for THIS incarnation;
// - ready    = ReadyRigid + a context.Serial strictly past the create serial;
// - retired  = the destroy/release completed (registry absent).
// Every receipt carries the incarnation it belongs to - a delayed receipt of
// an OLD incarnation can never activate a NEW one. A source change RETIRES the
// old incarnation first; the new source waits until the retire receipt
// arrives (the old lease is retained through the whole teardown). Faults
// quarantine WITH the live incarnation retained until a cleanup receipt.
// Input gates: nonzero generations, fresh ticks (80 ms), finite pose; the
// native seams are the existing phase:: helpers (8463B8 factory,
// AdoptCreatedRigid, ReadyRigid, SetVelocity, retire/destroy).
namespace kcd2vr::native_hand_proxy_t4 {
using Vec3=pose_core::Vec3;using Quat=pose_core::Quat;
struct Pose {Quat q{};Vec3 p{};};
struct Source { // the committed controller pose identity
    std::uint64_t trackingEpoch=0,sourceGeneration=0,cancelGeneration=0,poseTick=0; // poseTick is per-frame metadata, NOT identity
    std::uintptr_t player=0;int side=-1;bool physicalPresenceH24=false;std::uintptr_t characterH24=0;
    friend bool operator==(const Source& a,const Source& b) noexcept {
        return a.trackingEpoch==b.trackingEpoch&&a.sourceGeneration==b.sourceGeneration&&
            a.cancelGeneration==b.cancelGeneration&&a.player==b.player&&a.side==b.side&&a.physicalPresenceH24==b.physicalPresenceH24&&a.characterH24==b.characterH24;
    }
};
struct Input {Source source{};Pose target{};std::uint64_t tick=0;
    std::uintptr_t playerBodyH24=0,playerEntityH24=0;int playerBodyIdH24=-1;
    bool emptyBothHandsH24=false; // narrow presence slice; no competing native held/grab body
};
enum class State : unsigned {Off,Requested,Creating,Active,Retiring,Quarantined};
struct NativeReceipt { // mockable on host; produced by the native seams in game
    bool adopted=false,ready=false,retired=false,cleanedUp=false;
    std::uint64_t incarnation=0;
};
struct Achieved {
    Source source{};Pose pose{};bool valid=false,retired=true;State state=State::Off;
    std::uint64_t incarnation=0,tick=0;
    // DS-F59: the body handle is a SAME-STACK observation published from the
    // step callback (the lease stays retained in the owner record); the
    // game-thread executor copies it only while valid&&!retired and the tick
    // is fresh. Never a retained raw pointer lease.
    std::uintptr_t bodyHandle=0;
};
struct ProxyPolicy {
    Source source{};State state=State::Off;
    std::uint64_t incarnation=0,admission=0,tick=0;
    bool adoptRequested=false,retireRequested=false,fault=false;

    static bool validInput(const Input& in,std::uint64_t now) noexcept {
        if(!in.tick||now<in.tick||now-in.tick>80)return false; // fresh input tick
        if(!in.source.poseTick||now<in.source.poseTick||now-in.source.poseTick>80)return false; // DS-F67 (Codex F54): the pose itself must be CURRENT, not merely <= tick
        if(!in.source.trackingEpoch||!in.source.sourceGeneration||!in.source.cancelGeneration)return false;
        if(!in.source.player||in.source.side<0||in.source.side>1)return false;
        if(in.source.physicalPresenceH24&&(!in.emptyBothHandsH24||!in.playerBodyH24||
           !in.playerEntityH24||!in.source.characterH24||in.playerBodyIdH24<0))return false;
        if(!std::isfinite(in.target.p.x)||!std::isfinite(in.target.p.y)||!std::isfinite(in.target.p.z))return false;
        if(!std::isfinite(in.target.q.x)||!std::isfinite(in.target.q.y)||!std::isfinite(in.target.q.z)||!std::isfinite(in.target.q.w))return false;
        const float qn=std::sqrt(in.target.q.x*in.target.q.x+in.target.q.y*in.target.q.y+in.target.q.z*in.target.q.z+in.target.q.w*in.target.q.w);
        if(qn<.99f||qn>1.01f)return false; // DS-F60 (Codex F50): the quaternion must be normalized (zero is not identity)
        return true;
    }
    // The policy step. The native owner calls it every frame with the latest
    // input and the receipts from its seams; it then EXECUTES the requested
    // adopt/retire and reports the progress receipts back.
    State update(const Input& in,std::uint64_t now,const NativeReceipt& r) noexcept {
        if(fault){
            if(state!=State::Quarantined)state=State::Quarantined;
            if(r.cleanedUp&&r.incarnation==incarnation){fault=false;state=State::Off;incarnation++;source={};adoptRequested=retireRequested=false;} // the live lease was audited away
            return state;
        }
        const bool valid=validInput(in,now);
        if(state==State::Active&&!valid){retireRequested=true;return state=State::Retiring;} // DS-F60: one-shot retire request, drive stops immediately
        if(state==State::Retiring){
            // DS-F55: the OLD lease is retained until the retire receipt for
            // the OLD incarnation arrives; a new source waits, never replaces.
            if(r.retired&&r.incarnation==incarnation){
                state=State::Off;source={};adoptRequested=retireRequested=false;tick=now; // DS-F60: the retire request is ACKed
            }
            return state;
        }
        if(!valid){
            if(state==State::Off)return state;
            // DS-F69 (Codex F50): invalid input with a requested/creating
            // incarnation RETIRES it (no body is left pending forever); the
            // retire receipt still gates Off.
            if(state==State::Requested||state==State::Creating){retireRequested=true;return state=State::Retiring;}
            return state; // Retiring already: handled above
        }
        if(!(source==in.source)){
            if(state==State::Active||state==State::Creating){state=State::Retiring;retireRequested=true;tick=now;return state;} // retire the old incarnation first
            if(state==State::Off){source=in.source;incarnation++;admission++;state=State::Requested;adoptRequested=true;tick=now;return state;}
            // Retiring already: the new source waits (handled above)
            return state;
        }
        switch(state){
        case State::Off: // a matching source in Off cannot happen after release; start fresh
            state=State::Requested;adoptRequested=true;tick=now;break;
        case State::Requested:
            if(r.adopted&&r.incarnation==incarnation){state=State::Creating;adoptRequested=false;tick=now;} // DS-F60: the adopt request is ACKed (no repeated Create)
            break;
        case State::Creating:
            if(r.ready&&r.incarnation==incarnation){state=State::Active;tick=now;}
            break;
        case State::Active:
            tick=now;break; // per-frame drive stays the native owner's job
        default:break;
        }
        return state;
    }
};
}
