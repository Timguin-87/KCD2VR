#pragma once
#include <cmath>
namespace kcd2vr::release_rearm_t15 {
// The authoritative collider gate must be empty. F9 may stay enabled only
// after a measured retirement plus current empty-hand/open-Grip proof.
inline bool allow(bool busy,bool enabled,bool emptyKnown,bool empty,bool tracked,float grip,bool retired) noexcept {
 if(busy||!std::isfinite(grip))return false;
 return (!enabled&&grip>=.55f)||(tracked&&emptyKnown&&empty&&grip<.25f&&retired);
}
}
