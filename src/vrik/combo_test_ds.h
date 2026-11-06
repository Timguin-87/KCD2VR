#pragma once
#include <atomic>
// DS-F6 VrComboTestForce: TEST mode for the VR combo window. When ON the combo
// window is held at kWindowMs (5000) so the native slot stays open between slow
// VR swings, the Henry hit-flag write gate is enabled, and every attack press
// logs C69 COMBO_TEST with the live combo slot (+0x868) and Henry hit flag
// (+0x480). Receipts + one window extension only: the game still decides recipe
// matching, advance and the finisher. Nothing native is written.
namespace kcd2vr::combo_test_ds {
inline std::atomic<bool> force{false};
inline constexpr unsigned kWindowMs=5000;
}
