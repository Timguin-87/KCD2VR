#pragma once
#include <cmath>
namespace kcd2vr::physical_handoff132 {
inline bool allow_bridge(int mode,bool same_attachment,bool qualified_grip,bool gameplay,
                         bool head_tracked,bool primary_tracked,double primary_grip) noexcept {
    if(mode<4||mode>7||!same_attachment||!qualified_grip||!gameplay||!head_tracked||!primary_tracked||!std::isfinite(primary_grip))return false;
    // Full combat rungs require an intentional physical hold. Handling/contact
    // rungs 4/5 preserve their historical no-grip takeover semantics.
    if((mode==6||mode==7)&&primary_grip<.25)return false;
    return true;
}
}
