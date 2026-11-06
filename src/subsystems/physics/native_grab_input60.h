#pragma once
#include "runtime/native_grab_producer60.h" // DS-F88: the producer API (runtime/ holds the producer/backend)
#include "throw_policy60.h" // DS-F92: the release-velocity throw
#include "core/snapshot_mailbox.h"
#include "core/collab_test_h21.h"
#include "retail_build112.h"
#include <excpt.h> // DS-F88: SEH constants (__try/__except) WITHOUT windows.h's min/max macro pollution
#include <atomic>
#include <cstdint>
#include <cstring>
// DS-F88 (Claude's proven input helpers): the game-thread grip-evidence
// resolver. Header-only (inline state shared across the physics TU, the
// game-thread tick and the pickup producer). It reads the POD grip evidence
// (published by the pickup producer, no native derefs there), resolves the
// loose prop's exact body identity on the game thread (the same pins
// native_held_owner_t4.inl NativeHeldBodyT4 proves: CEntity vft 4072C78,
// GetProxy 5D70C4, proxy vft 3AC3478, +38 body, +20 foreign==entity, +28
// foreignType2, +44 signed-23-bit physicsId), and drives
// grab_producer60::PublishGrip / Cancel. Suspended native-held props are
// REFUSED (no second physical writer).
namespace kcd2vr::physics::grab_input60 {
namespace gp=kcd2vr::physics::grab_producer60;
struct GripEvidence { // POD, published by the pickup producer
    std::uint32_t entity=0;int side=-1;
    std::uint64_t press=0,tick=0,poseEpoch=0,trackingEpoch=0;
    std::uintptr_t player=0;
    float palm[3]{},quat[4]{0,0,0,1};
    bool held=false,eligible=false,object=false;
};
// Explicit loose-world classes only. A native-selected NPC, door, torch,
// inventory Item/PickableItem or another equipment class is not a free prop.
inline bool looseClassD40(const char* cls) noexcept {
    return cls&&(!std::strcmp(cls,"BasicEntity")||!std::strcmp(cls,"RigidBody")||
        !std::strcmp(cls,"RigidBodyEx")||!std::strcmp(cls,"DestroyableObject"));
}
inline bool nearSelectionD40(const char* cls,float distance,std::uint64_t tick,std::uint64_t now) noexcept {
    return looseClassD40(cls)&&std::isfinite(distance)&&distance>=0&&distance<=.10f&&
        tick&&now>=tick&&now-tick<=80;
}
inline std::uintptr_t g_base=0;
inline core::SnapshotMailbox<GripEvidence> g_evidence[2];
inline void*(*resolveEntity)(std::uint32_t) noexcept=nullptr;
// D40: one source per hand. A second hand must neither overwrite the first
// mailbox nor cancel/throw its body. These fields are game-thread owned.
inline std::uintptr_t g_lastEntity[2]{},g_lastBody[2]{};
inline std::uint64_t g_lastPress[2]{},g_retiredPressD40[2]{};
inline std::uintptr_t g_lastPlayerD40[2]{};
inline std::uint64_t g_lastPoseEpochD40[2]{},g_lastTrackingEpochD40[2]{};
// DS-F92: the release-velocity capture (the throw) - per-side previous palm.
inline float g_prevPalm[2][3]{{},{}};inline std::uint64_t g_prevTick[2]{};inline bool g_wasHeld[2]{false,false};
inline float g_releaseVelocityH21[2][3]{{},{}};inline std::uint64_t g_prevPressH21[2]{};
inline std::uint64_t g_motionThrowEpochH21[2]{},g_motionPhysicsEpochH21[2]{};

inline bool Install(std::uintptr_t base) noexcept {
    if(!base||!retail_build112::Ready(base))return false;
    g_base=base;return true;
}
inline bool PublishGripEvidence(const GripEvidence& e) noexcept {
    return e.side>=0&&e.side<=1&&g_evidence[e.side].publish(e);
}
inline bool ReadAt(void* at,std::uintptr_t& v) noexcept {
    __try{v=*reinterpret_cast<std::uintptr_t*>(at);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// The exact entity -> physicsId/body identity (Claude's NativeHeldBodyT4, loose props only).
inline bool ResolveBody(const GripEvidence& e,void*& body,int& physicsId) noexcept {
    body=nullptr;physicsId=-1;
    if(!g_base||!e.entity||!e.object)return false;
    void* entity=resolveEntity?resolveEntity(e.entity):nullptr;
    __try{
        std::uintptr_t vt=0;
        if(!ReadAt(entity,vt)||vt!=g_base+0x4072C78)return false; // CEntity vft
        std::uintptr_t id=0;if(!ReadAt(static_cast<unsigned char*>(entity)+0xC,id)||std::uint32_t(id)!=e.entity)return false;
        // GetProxy(ENTITY_PROXY_PHYSICS=1) must be the exact retail leaf
        std::uintptr_t getProxy=0;
        if(!ReadAt(reinterpret_cast<void*>(g_base+0x4072C78+0x250),getProxy)||getProxy!=g_base+0x5D70C4)return false;
        void* proxy=reinterpret_cast<void*(__fastcall*)(void*,int)>(g_base+0x5D70C4)(entity,1);
        if(!proxy||!ReadAt(proxy,vt)||vt!=g_base+0x3AC3478)return false;
        std::uintptr_t scratch=0;
        if(!ReadAt(static_cast<unsigned char*>(proxy)+0x28,scratch))return false;
        const unsigned flags=static_cast<unsigned>(scratch);
        if(flags&0x40)return false; // suspended native-held prop: MUST NOT be a second writer
        std::uintptr_t bodyp=0;
        if(!ReadAt(static_cast<unsigned char*>(proxy)+0x38,bodyp)||!bodyp)return false;
        body=reinterpret_cast<void*>(bodyp);
        std::uintptr_t foreign=0;
        if(!ReadAt(static_cast<unsigned char*>(body)+0x20,foreign)||foreign!=reinterpret_cast<std::uintptr_t>(entity))return false;
        if(!ReadAt(static_cast<unsigned char*>(body)+0x28,scratch))return false;
        const short foreignType=static_cast<short>(scratch);
        if(foreignType!=2)return false;
        if(!ReadAt(static_cast<unsigned char*>(body)+0x44,scratch))return false;
        const std::uint32_t packed=static_cast<std::uint32_t>(scratch);
        physicsId=static_cast<std::int32_t>(packed<<9)>>9;
        return physicsId>=0;
    }__except(EXCEPTION_EXECUTE_HANDLER){body=nullptr;return false;}
}
inline void CancelSideD40(int side) noexcept {
    if(side<0||side>1)return;
    if(g_lastPress[side]){gp::Cancel(side);g_retiredPressD40[side]=g_lastPress[side];}
    g_lastEntity[side]=g_lastBody[side]=g_lastPress[side]=0;
    g_lastPlayerD40[side]=g_lastPoseEpochD40[side]=g_lastTrackingEpochD40[side]=0;
    g_wasHeld[side]=false;g_prevTick[side]=g_prevPressH21[side]=0;
    for(auto& v:g_releaseVelocityH21[side])v=0;
}
// Native body resolution is the only injectable leaf. Production uses the
// exact pinned resolver above; fixtures execute this same input/lifetime code.
using BodyResolverD40=bool(*)(const GripEvidence&,void*&,int&) noexcept;
inline void ConsumeEvidenceD40(const GripEvidence& e,std::uint64_t now,BodyResolverD40 resolve=&ResolveBody) noexcept {
    if(e.side<0||e.side>1)return;
    const int side=e.side;
    const bool active=collab_h21::enabled.load(std::memory_order_acquire)&&e.held&&e.eligible&&e.object&&e.entity&&e.press&&
        e.tick&&now>=e.tick&&now-e.tick<=80;
    if(!active){
        // A release packet need not carry a pose; use only the last two HELD
        // samples of this exact source. Tracking/menu/epoch loss never throws.
        gp::Status status{};
        if(collab_h21::throwing.load(std::memory_order_acquire)&&collab_h21::enabled.load(std::memory_order_acquire)&&
           e.eligible&&e.object&&!e.held&&e.tick&&now>=e.tick&&now-e.tick<=80&&g_wasHeld[side]&&
           e.press==g_lastPress[side]&&e.entity==g_lastEntity[side]&&e.press==g_prevPressH21[side]&&
           g_prevTick[side]&&now>=g_prevTick[side]&&now-g_prevTick[side]<=80&&
           g_motionThrowEpochH21[side]==collab_h21::throwEpoch.load(std::memory_order_acquire)&&
           g_motionPhysicsEpochH21[side]==collab_h21::epoch.load(std::memory_order_acquire)&&
           gp::ReadStatus(side,status)&&status.state==gp::State::Held&&status.owner.side==side&&
           status.owner.press==e.press&&status.owner.entity==e.entity&&status.owner.player==g_lastPlayerD40[side]&&
           status.owner.poseEpoch==g_lastPoseEpochD40[side]&&status.owner.trackingEpoch==g_lastTrackingEpochD40[side]&&
           status.tick&&now>=status.tick&&now-status.tick<=80&&status.owner.world&&status.owner.worldGeneration&&g_lastBody[side]&&
           throw60::shouldThrow(g_releaseVelocityH21[side])){
            throw60::ThrowRequest tr{};tr.press=e.press;tr.side=side;tr.entity=e.entity;tr.body=g_lastBody[side];
            tr.entityPointer=reinterpret_cast<std::uintptr_t>(resolveEntity?resolveEntity(e.entity):nullptr);
            tr.world=status.owner.world;tr.worldGeneration=status.owner.worldGeneration;tr.tick=g_prevTick[side];
            tr.poseEpoch=status.owner.poseEpoch;tr.trackingEpoch=status.owner.trackingEpoch;
            tr.throwEpoch=collab_h21::throwEpoch.load(std::memory_order_acquire);tr.physicsEpoch=collab_h21::epoch.load(std::memory_order_acquire);
            throw60::throwVelocity(g_releaseVelocityH21[side],tr.v);(void)throw60::publish(tr);
        }
        CancelSideD40(side);return;
    }
    if(e.press==g_retiredPressD40[side])return; // cancelled ownership cannot revive until a new squeeze
    void* body=nullptr;int physicsId=-1;
    if(!e.player||!e.poseEpoch||!e.trackingEpoch||!resolve||!resolve(e,body,physicsId)){
        CancelSideD40(side);return; // a prior body's lost identity immediately withdraws its drive
    }
    if(g_lastPress[side]==e.press&&(g_lastEntity[side]!=e.entity||g_lastBody[side]!=reinterpret_cast<std::uintptr_t>(body)||
       g_lastPlayerD40[side]!=e.player||g_lastPoseEpochD40[side]!=e.poseEpoch||g_lastTrackingEpochD40[side]!=e.trackingEpoch)){
        CancelSideD40(side);return; // same press cannot transfer itself to another native resource
    }
    // Duplicate packets do not advance velocity; reversed source time revokes
    // the old source instead of teaching a discontinuity as a release throw.
    if(g_prevTick[side]&&e.tick<g_prevTick[side]){CancelSideD40(side);return;}
    gp::Intent in{};
    in.player=e.player;in.expectedBody=reinterpret_cast<std::uintptr_t>(body);
    in.press=e.press;in.poseEpoch=e.poseEpoch;in.trackingEpoch=e.trackingEpoch;
    in.sourceGeneration=e.press;in.tick=e.tick;in.poseTick=e.tick;
    in.entity=e.entity;in.side=side;in.physicsId=physicsId;
    for(int j=0;j<3;++j)in.p[j]=e.palm[j];for(int j=0;j<4;++j)in.q[j]=e.quat[j];
    in.held=true;in.allowed=true;in.nativeAttached=false;
    if(!gp::PublishGrip(in))return;
    const auto throwEpoch=collab_h21::throwEpoch.load(std::memory_order_acquire),physicsEpoch=collab_h21::epoch.load(std::memory_order_acquire);
    if(g_motionThrowEpochH21[side]!=throwEpoch||g_motionPhysicsEpochH21[side]!=physicsEpoch){
        g_prevPressH21[side]=g_prevTick[side]=0;for(auto& v:g_releaseVelocityH21[side])v=0;
        g_motionThrowEpochH21[side]=throwEpoch;g_motionPhysicsEpochH21[side]=physicsEpoch;
    }
    if(e.tick>g_prevTick[side]){
        if(g_prevPressH21[side]==e.press)throw60::velocity(g_prevPalm[side],e.palm,g_prevTick[side],e.tick,g_releaseVelocityH21[side]);
        else for(auto& v:g_releaseVelocityH21[side])v=0;
        for(int j=0;j<3;++j)g_prevPalm[side][j]=e.palm[j];
        g_prevTick[side]=e.tick;g_prevPressH21[side]=e.press;
    }
    g_wasHeld[side]=true;g_lastEntity[side]=e.entity;g_lastBody[side]=in.expectedBody;g_lastPress[side]=e.press;
    g_lastPlayerD40[side]=e.player;g_lastPoseEpochD40[side]=e.poseEpoch;g_lastTrackingEpochD40[side]=e.trackingEpoch;
}
inline void GameThreadTick(std::uint64_t now) noexcept {
    if(!g_base)return;
    for(int side=0;side<2;++side){GripEvidence e{};
        if(!g_evidence[side].read(e)||e.side!=side){CancelSideD40(side);continue;}
        ConsumeEvidenceD40(e,now);
    }
}

}
