#pragma once
#include <cstdint>
namespace kcd2vr::tracking_grace111 {
inline bool KeepLastGood(bool live,std::uint64_t now,std::uint64_t lastGood,std::uint64_t graceMs=80) noexcept {
    return live||(lastGood&&now>=lastGood&&now-lastGood<=graceMs);
}
// Run on the single tracking producer, before publishing one coherent packet.
// Held coordinates retain their original age; repeated invalid frames cannot
// extend grace. A gameplay boundary discards the previous session's poses.
template<class Snapshot> inline void HoldCoherent112(Snapshot& next,const Snapshot& prev) noexcept {
    auto hold=[&](bool& valid,bool& held,auto& pos,auto& rot,std::uint64_t& good,
                  bool oldValid,const auto& oldPos,const auto& oldRot,std::uint64_t oldGood,unsigned grace){
        held=false;good=next.allowed&&valid?next.tick:0;
        if(!valid&&next.allowed&&prev.allowed&&oldValid&&KeepLastGood(false,next.tick,oldGood,grace)){
            valid=held=true;pos=oldPos;rot=oldRot;good=oldGood;
        }
    };
    hold(next.head_valid,next.head_held,next.head,next.hq,next.head_good_tick,
         prev.head_valid,prev.head,prev.hq,prev.head_good_tick,60); // bounded HMD continuity, matching gameplay consumers
    hold(next.left_valid,next.left_held,next.left,next.lq,next.left_good_tick,
         prev.left_valid,prev.left,prev.lq,prev.left_good_tick,80);
    hold(next.right_valid,next.right_held,next.right,next.rq,next.right_good_tick,
         prev.right_valid,prev.right,prev.rq,prev.right_good_tick,80);
}
} // namespace
