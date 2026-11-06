#pragma once
#include <cstring>
namespace kcd2vr::vr_combat111 {
// Retail HoldItemLeft scope is layer3. These same aliases also occur as
// secondary overlays in protected actions on other layers; do not exempt them.
// R108: combat_idle01_shsw_torch_over is the HoldItemLeft (layer 3) overlay the game loops on Henry for
// the whole time he stands in combat stance with a torch in the left hand (kcd_male_database.adb
// HoldItemLeft tags "l_torch+combat+player" / "l_torch+combat", any right-hand weapon). it is in the
// native-body list only because the same clip is the 2nd FullBody layer of the CombatAttackMercy
// finishers (layer 1, still protected there through combat_finish_shsw_0x on layer 0). without this
// layer-3 exemption every sword+torch fight handed the whole body to the native animation
// (BODY_GATE PROTECTED_ACTION) = "lost VRIK while holding them both in combat".
inline bool LeftHoldingIdleB2(const char* name,int layer) noexcept {
 return layer==3&&name&&(!std::strcmp(name,"holditem_idle_torch_lhand_over")||
                        !std::strcmp(name,"combat_idle01_shsw_torch_over")||
                        !std::strcmp(name,"holditem_idle_shield_lhand"));
}
}
