#pragma once
#include <atomic>
#include <algorithm>
#include <cmath>
#include "../../combat_menu49.h"
namespace kcd2vr::claude_h8 {
inline std::atomic<bool> impaleMainProbe{false};
inline int assist(float unit) noexcept {return std::isfinite(unit)?std::clamp(int(std::lround(unit*30.f)),0,30):0;}
using Rect=combat_menu49::Rect;
constexpr Rect kMore{620,596,824,626},kOpenCombat{834,596,1038,626},kOpenImmersion{620,626,1038,658};
constexpr Rect kPommel{620,134,1038,170},kKickDamage{620,202,1038,238},kAssist{620,312,1038,334},kImpale{620,404,1038,440},kKickSetup{620,352,1038,384};
constexpr Rect kHelp{620,464,1038,640};
constexpr bool clean() noexcept {
 constexpr Rect rows[]{kPommel,kKickDamage,{620,287,1038,334},kKickSetup,kImpale,kHelp};
 for(unsigned i=0;i<6;++i){if(!combat_menu49::inside(rows[i],combat_menu49::kClip)||rows[i].t<110)return false;
  for(unsigned j=i+1;j<6;++j)if(combat_menu49::overlap(rows[i],rows[j]))return false;}
 return !combat_menu49::overlap(kMore,kOpenCombat)&&kOpenImmersion.b<=672;
}
static_assert(clean());
}
