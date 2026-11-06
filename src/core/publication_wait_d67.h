#pragma once
#include <algorithm>
#include <condition_variable>
#include <cstdint>
#include <mutex>
namespace kcd2vr::publication_wait_d67 {
// CPU producer-token ordering only; never a render/scanout identity claim.
inline std::uint64_t floor(std::uint64_t consumed,std::uint64_t atArrival,bool freshAfterArrival) noexcept {
    return freshAfterArrival?std::max(consumed,atArrival):consumed;
}
inline bool ready(std::uint64_t published,std::uint64_t floorToken) noexcept {return published>floorToken;}
// D67R1: modifying an atomic predicate outside the wait mutex still loses a
// notification between the waiter's false predicate and its atomic unlock/wait.
// This protects the transition only; callers must not hold another frame lock
// or do native/XR work in change(). Notify after releasing the predicate mutex.
template<class Change> inline void notifyChange(std::mutex& mutex,std::condition_variable& cv,Change&& change) {
    {std::lock_guard<std::mutex> lock(mutex);change();}
    cv.notify_all();
}
}
