#pragma once
// R117: table shared by the completed-body publication and the D63 BONE render leaf.
// Plain atomics only; host-tested through tests/stowed_render_native_d63_test.cpp.
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

constexpr std::size_t kStowedExtraR117=32;
struct StowedExtraTableR117 {
    std::array<std::atomic<int>,kStowedExtraR117> joint{}; // -1 free (set by Reset)
    std::atomic<std::uint32_t> torsoMask{0};  // entry sits on spine/spine1-3 (never arm/head/pelvis/legs)
    std::atomic<std::uint32_t> loggedMask{0}; // one KCD2VR.log line per entry and skeleton
    std::atomic<std::uint32_t> evaluatedMask{0}; // entries the completed body has classified
    std::atomic<std::uintptr_t> character{0};
    std::atomic<std::uint64_t> generation{0};
    StowedExtraTableR117() noexcept {Reset(0,0);}
    void Reset(std::uintptr_t ch,std::uint64_t gen) noexcept {
        for(auto& j:joint)j.store(-1,std::memory_order_relaxed);
        torsoMask.store(0,std::memory_order_relaxed);loggedMask.store(0,std::memory_order_relaxed);evaluatedMask.store(0,std::memory_order_relaxed);
        generation.store(gen,std::memory_order_relaxed);character.store(ch,std::memory_order_release);
    }
    // render thread: find this joint, or claim a free entry for it (-1 = table full this skeleton)
    int Find(int j,bool add) noexcept {
        if(j<0)return -1;
        for(std::size_t i=0;i<kStowedExtraR117;++i)if(joint[i].load(std::memory_order_acquire)==j)return int(i);
        if(!add)return -1;
        for(std::size_t i=0;i<kStowedExtraR117;++i){
            int expected=-1;
            if(joint[i].compare_exchange_strong(expected,j,std::memory_order_acq_rel))return int(i);
            if(expected==j)return int(i);
        }
        return -1;
    }
};
