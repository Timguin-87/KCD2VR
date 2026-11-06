#pragma once
// Actual production Dog observer/read implementation. Only native memory,
// current-palm publication and its existing backend flags are fixture leaves.
#include "../src/interaction_alignment.h"
#include "../src/core/snapshot_mailbox.h"
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <mutex>

namespace kcd2vr::interaction_alignment {
namespace {
inline std::mutex g_hand_candidate_mutex178r14;
inline std::atomic<bool> g_enabled{true},g_split_ready57c1{true},g_visual_fallback_enabled{false};
inline std::uint64_t g_door_acquisition_generation65=1;
inline core::SnapshotMailbox<palm59c9::Pair> g_palm_packets59c9;
struct Candidate57c1 {std::uintptr_t player=0;std::uint64_t tick=0;interaction57c1::Kind kind=interaction57c1::Kind::Unknown;std::uint32_t id=0;char cls[32]{};};
inline std::uintptr_t testNativePlayerD59R1=200,testPlayerEntityD59R1=100;
inline bool testReadsD59R1=true,testReplacePlayerD59R1=false;
inline unsigned testNativeReadsD59R1=0;
inline bool SelectionVft68(std::uintptr_t ptr,std::uintptr_t vft) noexcept {
    ++testNativeReadsD59R1;
    return testReadsD59R1&&((ptr==testNativePlayerD59R1&&vft==0x3AA4B98)||
        (ptr==testPlayerEntityD59R1&&vft==0x4072C78));
}
inline bool SelectionRead68(std::uintptr_t ptr,std::uintptr_t& out) noexcept {
    ++testNativeReadsD59R1;
    if(!testReadsD59R1||ptr!=testNativePlayerD59R1+0x38)return false;
    out=testPlayerEntityD59R1;return true;
}
inline bool SelectionCopy68(std::uintptr_t ptr,void* out,std::size_t size) noexcept {
    ++testNativeReadsD59R1;
    if(!testReadsD59R1||!ptr)return false;
    std::memcpy(out,reinterpret_cast<const void*>(ptr),size);
    if(testReplacePlayerD59R1)++testPlayerEntityD59R1;
    return true;
}
#include "../src/subsystems/interactions/dog_candidate_d59r1.inl"
}
inline void SetDogGestureObservationD59R1(bool on) noexcept {SetDogObservationImplD59R1(on);}
inline bool ReadDogHandCandidateD59R1(int side,std::uint64_t now,DogHandCandidateD59R1& out) noexcept {
    return ReadDogCandidateImplD59R1(side,now,out);
}
}
namespace kcd2vr::test_dog_d59r1 {
inline void reset() {
    using namespace interaction_alignment;
    SetDogGestureObservationD59R1(false);g_dog_candidates_d59r1.clear();
    g_enabled=true;g_split_ready57c1=true;g_visual_fallback_enabled=false;
    ++g_door_acquisition_generation65;
    testNativePlayerD59R1=200;testPlayerEntityD59R1=100;
    testReadsD59R1=true;testReplacePlayerD59R1=false;testNativeReadsD59R1=0;
    g_palm_packets59c9.publish(palm59c9::Pair{});
}
inline void palms(const palm59c9::Pair& p){interaction_alignment::g_palm_packets59c9.publish(p);}
inline void observe(std::uint64_t now,const float* point,std::uint32_t target=123,const char* cls="Dog",
                    std::uint64_t nativeTick=0){
    using namespace interaction_alignment;
    Candidate57c1 candidate{};candidate.player=testNativePlayerD59R1;candidate.tick=nativeTick?nativeTick:now;
    candidate.id=target;candidate.kind=interaction57c1::Classify(cls);std::snprintf(candidate.cls,sizeof(candidate.cls),"%s",cls?cls:"");
    // The real callback reaches this observer only after its policy/actor join.
    assert(candidate.kind==interaction57c1::Kind::Actor||std::strcmp(candidate.cls,"Dog"));
    DogCandidateObservationD59R1(candidate,reinterpret_cast<std::uintptr_t>(point),now);
}
}
