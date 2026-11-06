#pragma once
// R90 (from R88 ranged 00200c6): option VrTwoHandWeightFeelR90, Experiments >
// GEAR page, slot 8 "Two-hand weight feel". OFF (default) = R2 two-hand heft
// (D86 removed the extra polearm penalty). ON = VrWeaponWeightFeel (lower =
// heavier) also scales the two-hand hybrid rotation drive, so the existing
// weight slider changes how heavy a weapon held with both hands turns.
#include <atomic>
namespace kcd2vr::two_hand_weight_r90 {
inline std::atomic<bool> enabled{false};
}
