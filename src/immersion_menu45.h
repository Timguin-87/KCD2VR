#pragma once
// IMMERSION layout: H18 exposes both saved crafting owners directly.
// Compact buttons preserve the existing controls and 22px sliders/25px labels; bottom665 stays inside the672 clip.
// It also retires two C44 collisions: the Dialogue text size slider sat on PRECISE CONTACTS (418..440 vs 418..450, the slider
// drag won the click) and the TARGET SWITCH label sat on the PALM AUTO-ALIGN row. Pure constants, host tested (combat_menu49_test).
#include "combat_menu49.h"
#include "vrik/crafting_menu_h18.h"
namespace kcd2vr::immersion_menu45 {
using combat_menu49::Rect;
// H18: saved crafting owners are visible directly on IMMERSION. A compact
// 24px button grid preserves every existing control plus the details link.
constexpr Rect kNative{620,112,824,142},kFirstPerson{834,112,1038,142};
constexpr Rect kNote{620,147,824,173},kShowHintsD80{834,147,1038,173};
constexpr Rect kMinigameHands{620,178,824,202},kSmithingModeH18{834,178,1038,202};
constexpr Rect kHmdHeight54{620,207,824,231},kCraftingDetailsH18{834,207,1038,231};
constexpr Rect kWeaponRelease{620,236,1038,260};
constexpr Rect kSmoothDialogue{620,265,824,289},kHeadDodge54{834,265,1038,289};
constexpr Rect kHolsters{620,294,824,318},kBowReload{834,294,1038,318};
constexpr Rect kPreciseContacts{620,323,824,347},kBloodyFights{834,323,1038,347};
constexpr Rect kDecapitation{620,352,824,376},kRagdoll{834,352,1038,376};
constexpr Rect kWeaponGrip{620,406,1038,428}; // label381..403
constexpr Rect kSupportSlot{620,458,824,480},kSupportSword{834,458,1038,480};
constexpr Rect kSupportPole{620,510,1038,532};
constexpr Rect kAutoPalm{620,537,824,561},kExperiments{834,537,1038,561};
constexpr Rect kRiposte=kWeaponRelease;
constexpr Rect kTargetDwell{620,591,1038,613};
constexpr Rect kDialogueUiScale{620,643,1038,665};
struct Item { Rect r; bool slider; };
constexpr Item kPage[]{{kNative, false}, {kFirstPerson, false}, {kNote, false}, {kShowHintsD80, false}, {kExperiments, false}, {kHmdHeight54, false}, {kMinigameHands, false}, {kSmithingModeH18, false}, {kCraftingDetailsH18, false},
    {kSmoothDialogue, false}, {kHeadDodge54, false}, {kHolsters, false}, {kBowReload, false}, {kPreciseContacts, false}, {kBloodyFights, false}, {kDecapitation, false}, {kRagdoll, false},
    {kWeaponGrip, true}, {kSupportSlot, false}, {kSupportSword, true}, {kSupportPole, true}, {kAutoPalm, false}, {kRiposte, false},
    {kTargetDwell, true}, {kDialogueUiScale, true}};
constexpr unsigned kCount = sizeof(kPage) / sizeof(kPage[0]);
// compile-time guard shared by vr_menu.cpp (static_assert) and the host test: nothing overlaps, nothing leaves the clip or enters the header
constexpr bool clean() noexcept {
    for (unsigned i = 0; i < kCount; ++i) {
        const Rect e = combat_menu49::extent(kPage[i].r, kPage[i].slider);
        if (!(e.l < e.r && e.t < e.b) || !combat_menu49::inside(e, combat_menu49::kClip) || e.t < combat_menu49::kHeader.b) return false;
        for (unsigned j = i + 1; j < kCount; ++j) if (combat_menu49::overlap(e, combat_menu49::extent(kPage[j].r, kPage[j].slider))) return false;
    }
    return true;
}
}
