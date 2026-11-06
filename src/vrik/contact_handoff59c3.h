#pragma once
#include "contact_authority176.h"
#include <array>

namespace kcd2vr::contact59c3 {
// Gesture identity originates in the completed pose producer. Input policy is
// deliberately not identity: preparing B must not erase the contact/release of A.
// A Bound join still requires native admission of this same release/current
// helper. C54 HYBRID can instead use an independently claimed physical attempt
// after its bounded grace; it must never manufacture or borrow a release here.
inline constexpr std::uint64_t maximumContactAge=160;
struct Key {
    std::uint64_t gesture=0,key=0,generation=0,epoch=0,request=0,recovery=0;
};
inline bool matches(const Key& k,const contact176::Ticket& t) noexcept {
    return k.gesture&&t.serial&&t.player&&t.entity&&k.gesture==t.gesture59c3&&
        k.key==t.key&&k.generation==t.generation&&k.epoch==t.epoch&&
        k.request==t.request&&k.recovery==t.recovery59c3;
}
struct Releases {
    std::array<core::SnapshotMailbox<contact176::Ticket>,8> slots;
    bool publish(const contact176::Ticket& t) noexcept {
        return t.gesture59c3&&t.serial&&slots[t.serial%slots.size()].publish(t);
    }
    bool find(const Key& key,contact176::Ticket& t) const noexcept {
        for(const auto& slot:slots){contact176::Ticket candidate{};
            if(slot.read(candidate)&&matches(key,candidate)){t=candidate;return true;}}
        return false;
    }
};
inline Releases releases;
enum class Join { Wait,Bound,Expired,Identity,Recovery,ReleaseTooLate };
inline Join join(const Key& key,const contact176::Ticket& release,std::uint64_t impact,
                 std::uint64_t now,std::uint64_t recovery) noexcept {
    if(key.recovery!=recovery)return Join::Recovery;
    if(!contact176::fresh(impact,now,maximumContactAge))return Join::Expired;
    if(!release.serial)return key.gesture?Join::Wait:Join::Identity;
    if(!matches(key,release))return Join::Identity;
    if(!contact176::fresh(release.tick,now,contact176::kReleaseSafetyCapMs))return Join::ReleaseTooLate;
    return Join::Bound;
}
} // namespace kcd2vr::contact59c3
