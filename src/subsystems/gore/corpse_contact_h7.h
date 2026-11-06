#pragma once
#include <cstdint>
namespace kcd2vr::corpse_contact_h7 {
// SAME fresh contact/body only. No joint translation or corpse successor mapping.
struct Evidence {
 std::uintptr_t contactBody=0,currentMain=0;
 int rawPart=-1,returnedPart=-1;
 bool nativeDead=false,ownerStable=false,relinquished=false,articulated=false,partReadable=false;
};
inline bool qualified(const Evidence& e) noexcept {
 return e.nativeDead&&e.ownerStable&&e.relinquished&&e.articulated&&e.partReadable&&
   e.contactBody&&e.contactBody==e.currentMain&&e.rawPart>=0&&e.returnedPart==e.rawPart;
}
}
