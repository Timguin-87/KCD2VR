#pragma once
#include <cstdint>
// Map presentation only. These are existing owners of THIS completed hand;
// the mask grants no pose, attack, item, input or native callback authority.
namespace kcd2vr::map_hand_owner_m34 {
inline constexpr std::uint32_t Unknown=~std::uint32_t{0};
constexpr std::uint32_t mask(bool physical,bool prop,bool door,bool passenger,
                            bool reading,bool bowString,bool nativeCapture,
                            bool objectFingers,bool qualifiedFist,bool retainedSupport) noexcept {
 return (physical?1u:0u)|(prop?2u:0u)|(door?4u:0u)|(passenger?8u:0u)|
        (reading?16u:0u)|(bowString?32u:0u)|(nativeCapture?64u:0u)|
        (objectFingers?128u:0u)|(qualifiedFist?256u:0u)|(retainedSupport?512u:0u);
}
}
