#pragma once
// R147 (Miki 8 Nov: "in riding or walking sections in some quest you should tap LB for following the NPC... which
// of course is grip. can we change it to X (or PlayStation equivalent) only for riding?").
// the follow prompt is the game's own interactor action focus_follow_init_interactor_press (BasicAIActions.lua,
// actionMap focus_follow_init_interactor, hint class AHC_FOLLOW), bound to LB (xi_shoulderl) in defaultProfile.xml.
// in gameplay the VR pad never sends LB from the grip (grip = weapons/reins), so on the horse follow was not reachable.
// now: while Henry sits on a horse, a short tap of the left X button (PS VR2: Square) sends one LB tap instead of
// the Quick Speak X tap, when the game currently lists the follow prompt (native hint rows, same reader as the R25
// carry hint). no follow prompt -> X tap is Quick Speak as before. prompt list unreadable -> mounted tap = LB.
// X hold (torch / inventory / visor) and everything off the horse are unchanged. INI VrRideFollowXR147 (default
// true), Experiments GEAR page 3 row 8, live. decided per press, nothing is remembered between presses.
#include <atomic>
#include <cstdint>
namespace kcd2vr::ride_follow_r147 {
inline std::atomic<bool> enabled{true}; // VrRideFollowXR147
inline constexpr const char* kKey="VrRideFollowXR147";
inline constexpr std::uint64_t kTapMs=300; // press + release shorter than this = tap
// hint: 1 = the follow prompt is listed now, 0 = it is not, -1 = could not read the prompt list.
constexpr bool SendLb(bool on,bool mounted,std::uint64_t heldMs,int hint) noexcept {
    return on&&mounted&&heldMs<kTapMs&&hint!=0;
}
// one press: the best answer seen while the button was held (a listed prompt wins over "not listed").
constexpr int Merge(int seen,int answer) noexcept {return seen==1||answer==1?1:(answer==0||seen==0)?0:-1;}
} // namespace kcd2vr::ride_follow_r147
