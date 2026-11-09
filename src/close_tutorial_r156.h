#pragma once
// R156 (Miki 8 Nov, R155 build, Quest Touch, layout 0): "I could not close the full-screen tutorial screens - they need
// the 'home' button (xi_back / view button) and B or holding B was not working (Escape on PC worked)".
// the game's close_tutorial action map (priority minigames, companion defaultProfile.xml) has ONE pad input: xi_back,
// onPress/onHold/onRelease with holdTriggerDelay 0.25 (the "hold to close" bar). nothing in VR ever sends xi_back
// outside the authored minigames (there the left Menu button is xi_back for Help); in gameplay a physical B press
// publishes nothing at all (R156 log 14:18:54 / 14:20:00 / 14:39:03: bursts of B_TRANSACTION PRESS/RELEASE rows with
// published=NONE while he tried to close the screen; the left Menu press at 14:18:58 opened the Inventory instead).
// so: the player view lens (activity_lens59c10r1.cpp, game thread, every frame - it keeps running while the game is
// paused by a screen, log 14:19:06 pause menu view=94/s) reads the close_tutorial map's enabled byte with the verified
// named-map walk of dialogue_input_owner19c3.h and stores the tick here. while that tick is fresh (kEnabledMs) the pad
// premap sends the right B (PS Circle) as a REAL held xi_back (press, hold, release - the game makes its own
// hold / holdTriggerDelay from the held state). nothing else changes when the map is not enabled, no latch: the tick
// ages out 300 ms after the map is disabled again and B goes up with the button either way.
#include <atomic>
#include <cstdint>
namespace kcd2vr::close_tutorial_r156 {
inline constexpr char kMapName[]="close_tutorial"; // exact lowercase action map name (defaultProfile.xml)
inline constexpr std::uint64_t kEnabledMs=300; // the map was read ENABLED this recently = a tutorial screen is open now
inline std::atomic<std::uint64_t> mapEnabledMs{0}; // tick of the last frame the map was read enabled (view lens)
// the screen is open: the map was read enabled within kEnabledMs of now (0 = never read enabled).
constexpr bool Open(std::uint64_t now,std::uint64_t enabledTick) noexcept {
    return enabledTick&&now>=enabledTick&&now-enabledTick<=kEnabledMs;
}
// this frame's xi_back: the screen is open and the right B is physically down (press -> hold -> release follow the button).
constexpr bool Back(bool open,bool bDown) noexcept {return open&&bDown;}
} // namespace kcd2vr::close_tutorial_r156
