#pragma once
#include <atomic>
#include <cstdint>
namespace kcd2vr::contact_handover_d71 {
// Independent opt-in trials. Revisions retire only trial-owned pending state.
inline std::atomic<bool> contactContinuity{false},itemHandover{false};
inline std::atomic<std::uint64_t> contactRevision{1},itemRevision{1};
inline bool ContactContinuity() noexcept {return contactContinuity.load(std::memory_order_acquire);}
inline bool ItemHandover() noexcept {return itemHandover.load(std::memory_order_acquire);}
inline std::uint64_t ContactRevision() noexcept {return contactRevision.load(std::memory_order_acquire);}
inline std::uint64_t ItemRevision() noexcept {return itemRevision.load(std::memory_order_acquire);}
inline void SetContactContinuity(bool on) noexcept {if(contactContinuity.exchange(on,std::memory_order_acq_rel)!=on)contactRevision.fetch_add(1,std::memory_order_acq_rel);}
inline void SetItemHandover(bool on) noexcept {if(itemHandover.exchange(on,std::memory_order_acq_rel)!=on)itemRevision.fetch_add(1,std::memory_order_acq_rel);}
}
