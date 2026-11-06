#pragma once
#include <algorithm>
#include <cmath>
namespace kcd2vr::core {
// Shared by the gameplay input route and VRIK finger publication. UI receives
// its original raw value. Losing tracking is NOT a physical release.
struct TriggerOwnership {
    bool blocked=false;
    float sample(bool ui_or_activation_owner,bool active,float raw) noexcept {
        const bool valid=active&&std::isfinite(raw);
        if(ui_or_activation_owner)blocked=true;
        else if(blocked&&valid&&raw<=.15f)blocked=false;
        return !valid||blocked?0.f:std::clamp(raw,0.f,1.f);
    }
};
}
