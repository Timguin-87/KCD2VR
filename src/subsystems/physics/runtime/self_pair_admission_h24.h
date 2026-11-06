#pragma once
#include <cstdint>
namespace kcd2vr::physics::self_pair_admission_h24 {
enum class Observation {Unknown,AwaitPresent,Present,Absent,WrongPair};
struct Causality {
    std::uint64_t submittedTick=0;bool presentSeen=false;
    bool timedOut(std::uint64_t now)const noexcept{return submittedTick&&now>=submittedTick&&now-submittedTick>1000;}
    Observation observe(bool known,bool exists,bool exactPair) noexcept {
        if(!known)return Observation::Unknown;
        if(exists){if(!exactPair)return Observation::WrongPair;presentSeen=true;return Observation::Present;}
        // An empty status before insertion is NOT a retirement receipt.
        return presentSeen?Observation::Absent:Observation::AwaitPresent;
    }
};
}
