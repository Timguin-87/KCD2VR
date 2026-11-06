#pragma once
// Explicit user trials. Neither switch changes camera time or native scheduling.
#include <atomic>
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace kcd2vr::claude_motion_d69 {
inline std::atomic<bool> handLead{false},elbowHysteresis{false};
inline std::atomic<float> handLeadMs{11.f};
inline std::atomic<std::uint64_t> handPolicy{1},elbowPolicy{1};
inline bool HandLead() noexcept {return handLead.load(std::memory_order_acquire);}
inline float HandLeadMs() noexcept {return handLeadMs.load(std::memory_order_acquire);}
inline bool ElbowHysteresis() noexcept {return elbowHysteresis.load(std::memory_order_acquire);}
inline void SetHandLead(bool v) noexcept {if(handLead.exchange(v,std::memory_order_acq_rel)!=v)handPolicy.fetch_add(1,std::memory_order_acq_rel);}
inline void SetHandLeadMs(float v) noexcept {if(!std::isfinite(v))return;v=std::clamp(v,0.f,33.f);if(handLeadMs.exchange(v,std::memory_order_acq_rel)!=v)handPolicy.fetch_add(1,std::memory_order_acq_rel);}
inline void SetElbowHysteresis(bool v) noexcept {if(elbowHysteresis.exchange(v,std::memory_order_acq_rel)!=v)elbowPolicy.fetch_add(1,std::memory_order_acq_rel);}
inline std::int64_t ExtraNs() noexcept {return HandLead()?std::int64_t(double(HandLeadMs())*1000000.0+.5):0;}
inline bool ExtraActive() noexcept {return ExtraNs()>0;}
}
