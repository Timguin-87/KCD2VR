#pragma once
#include "ui_contextual_hud162r1.h"
namespace kcd2vr::ui_gameplay_hud54 {
// Saved preferences survive; the current semantic interaction owner determines
// the centre cursor. Native menu/hardware cursors are outside this HUD policy.
inline unsigned Preferences(unsigned saved,bool gameplay,bool semiAlchemyGaze) noexcept {
    using namespace ui_contextual_hud162r1;
    if(semiAlchemyGaze)return saved&~HideCrosshair163;
    return gameplay?(saved|HideCrosshair163):saved;
}
}
