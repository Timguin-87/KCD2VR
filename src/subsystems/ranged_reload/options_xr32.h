#pragma once
#include <atomic>
#include "options_d65c.h"
#include "weapon_kind_xr32.h"
namespace kcd2vr::ranged_options_xr32 {
// Only the exact Henry aim-camera zoom consumer may read this preference.
inline std::atomic<bool> crossbowNoZoom{true};
}
