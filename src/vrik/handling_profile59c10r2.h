#pragma once
#include <array>
#include <cstdint>
#include <algorithm>
#include <cmath>
namespace kcd2vr::handling59c10r2 {
// Pose-thread diagnostic gate. Two held items must not invalidate each other's
// throttle. Readiness changes are useful edges; generation churn is not.
struct GripTraceGate {
    struct Slot { std::uint64_t key=0,tick=0; unsigned readiness=0; };
    std::array<Slot,2> slots{};
    bool due(int side,std::uint64_t key,unsigned readiness,std::uint64_t now) noexcept {
        if(side<0||side>1||!key)return false;
        auto& s=slots[side];
        if(s.key==key&&s.readiness==readiness&&now>=s.tick&&now-s.tick<10000)return false;
        s={key,now,readiness};return true;
    }
};
inline bool swordFallback(int classId,bool v120) noexcept {return v120&&classId==4;}
inline double supportDistance(int classId,bool synthetic,double configured,double extent) noexcept {
    // Polearms must not inherit a longsword's adjacent-wrist 7.5--12 cm fallback.
    if(classId==7||synthetic)return std::clamp(configured,.06,.60);
    return std::clamp(extent*.14,.075,.12);
}
}
