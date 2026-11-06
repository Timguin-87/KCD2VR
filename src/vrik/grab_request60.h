#pragma once
#include <cstdint>
#include <cstddef> // DS-F66: std::size_t for the reason copy
// DS-F63: the grab REQUEST policy - the last piece of the grab pipeline. It
// turns the grip evidence (an F5-style grip on a rigid PROP, not a door/NPC)
// into the request the machine/adapter consume: the resolved prop body
// (8A49FC, the source lease identity), the proxy as the buddy, and the stage
// handoffs. The native producer feeds it; the machine's acquire + the
// executor's typed-Present ACK drive Requested/Held/Released. Pure logic,
// host-tested.
namespace kcd2vr::grab_request60 {
enum class Stage : unsigned char {Idle,GripObserved,BodyResolved,ProxyDriving,Requested,Held,Releasing,Released,Fault};
inline const char* stageName(Stage s) noexcept {
    switch(s){case Stage::Idle:return "IDLE";case Stage::GripObserved:return "GRIP_OBSERVED";case Stage::BodyResolved:return "BODY_RESOLVED";
              case Stage::ProxyDriving:return "PROXY_DRIVING";case Stage::Requested:return "REQUESTED";case Stage::Held:return "HELD";
              case Stage::Releasing:return "RELEASING";case Stage::Released:return "RELEASED";default:return "FAULT";}
}
struct Request {
    std::uint32_t entity=0;int side=-1;
    std::uintptr_t body=0;std::uint64_t bodyGeneration=0; // the resolved prop IPhysicalEntity + its generation (the source lease identity)
    std::uint64_t tick=0;
    Stage stage=Stage::Idle;
};
struct Policy {
    Stage stage=Stage::Idle;Request req{};
    char reason[64]{}; // DS-F66 (Claude review 16): the fault reason is KEPT for the one visible line
    // A grip on a rigid prop: the producer resolved the body already.
    Stage observeGrip(std::uint32_t entity,int side,std::uintptr_t body,std::uint64_t generation,std::uint64_t now) noexcept {
        if(stage!=Stage::Idle&&stage!=Stage::Released)return stage; // one grab at a time
        if(!entity||!body||!generation||side<0||side>1||!now)return fault("INVALID_GRIP");
        req={entity,side,body,generation,now,Stage::BodyResolved};stage=Stage::BodyResolved;return stage;
    }
    Stage proxyReady(std::uint64_t now) noexcept {
        if(stage!=Stage::BodyResolved)return stage;
        req.tick=now;stage=Stage::ProxyDriving;return stage;
    }
    Stage requested(std::uint64_t now) noexcept { // the machine's acquire was admitted
        if(stage!=Stage::ProxyDriving)return stage;
        req.tick=now;stage=Stage::Requested;return stage;
    }
    Stage held(std::uint64_t now) noexcept { // the typed-Present ACK (paired with the measured follow)
        if(stage!=Stage::Requested)return stage;
        req.tick=now;stage=Stage::Held;return stage;
    }
    Stage release(std::uint64_t now) noexcept {
        if(stage!=Stage::Held&&stage!=Stage::Requested)return stage;
        req.tick=now;stage=Stage::Releasing;return stage;
    }
    Stage released(std::uint64_t now) noexcept { // the typed-Absent ACK for the ORIGINAL id
        if(stage!=Stage::Releasing)return stage;
        req.tick=now;stage=Stage::Released;return stage;
    }
    Stage reset() noexcept {if(stage==Stage::Released||stage==Stage::Fault){stage=Stage::Idle;req={};reason[0]=0;}return stage;}
    Stage fault(const char* why) noexcept { // DS-F66: the reason survives for the visible line
        stage=Stage::Fault;
        if(why){std::size_t i=0;for(;why[i]&&i<sizeof(reason)-1;++i)reason[i]=why[i];reason[i]=0;}
        return stage;
    }
    // DS-F66 (Claude review 16): the requalification window applies ONLY to
    // Requested (waiting for Present). Held/Releasing use the identity
    // (entity/side/body/generation) - a grab held longer than 5 s must still
    // release through the Remove path.
    bool current(const Request& other,std::uint64_t now) const noexcept {
        const bool identity=other.entity==req.entity&&other.side==req.side&&
            other.body==req.body&&other.bodyGeneration==req.bodyGeneration;
        if(!identity)return false;
        if(stage==Stage::Held||stage==Stage::Releasing)return true; // no time window on the hold
        return stage==Stage::Requested&&req.tick&&now>=req.tick&&now-req.tick<=5000;
    }
};
}
