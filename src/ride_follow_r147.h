#pragma once
// R147 (Miki 8 Nov: "in riding or walking sections in some quest you should tap LB for following the NPC... which
// of course is grip. can we change it to X (or PlayStation equivalent) only for riding?").
// the follow prompt is the game's own interactor action focus_follow_init_interactor_press (BasicAIActions.lua,
// actionMap focus_follow_init_interactor, hint class AHC_FOLLOW), bound to LB (xi_shoulderl) in defaultProfile.xml.
// in gameplay the VR pad never sends LB from the grip (grip = weapons/reins), so on the horse follow was not reachable.
// now: while Henry sits on a horse, a short tap of the left X button (PS VR2: Square) sends one LB tap instead of
// the Quick Speak X tap, when the game currently lists the follow prompt (native hint rows, same reader as the R25
// carry hint). no follow prompt -> X tap is Quick Speak as before. prompt list unreadable -> mounted tap = LB.
// X hold (torch / inventory / visor) is unchanged. INI VrRideFollowXR147 (default
// true), Experiments GEAR page 3 row 8, live. decided per press, nothing is remembered between presses.
// R155 (Miki 8 Nov, R153 build: "'follow' was still showing LB and X never worked"): the R151 profile binds Follow to
// X too, and the log shows the game DOES dispatch focus_follow_init_interactor_press for our X tap - but the same X tap
// also dispatches chat_init_press / chat_init_interactor_press (Quick Speak), and Follow never started. LB carries no
// Quick Speak, so: on foot too, a short X tap sends one LB tap when the game lists the follow prompt (hint rows, same
// reader). on foot only a LISTED prompt counts (an unreadable list keeps the X tap = talk); the mounted "unreadable =
// LB" fallback of R147 stays. no follow prompt -> X tap is Quick Speak exactly as before.
// R157: the companion profile row focus_follow_init_interactor_press is retail again (LB / L1 only, no X) and
// BasicAIActions.lua is the pre-R151 file (X Quick talk prompt listed next to Follow like retail). so this X tap -> LB
// is the ONLY way Follow is reached from the VR X button, on foot and mounted. while the NPC waits for a dialogue
// reply AND offers Follow, the X tap is the LB Follow (the reply stays on the pointing gesture = RB).
#include <atomic>
#include <cstdint>
namespace kcd2vr::ride_follow_r147 {
inline std::atomic<bool> enabled{true}; // VrRideFollowXR147
inline constexpr const char* kKey="VrRideFollowXR147";
inline constexpr std::uint64_t kTapMs=300; // press + release shorter than this = tap
// hint: 1 = the follow prompt is listed now, 0 = it is not, -1 = could not read the prompt list.
// R155: listed = LB on foot and mounted; unreadable = LB only mounted (R147 fallback); not listed = talk.
constexpr bool SendLb(bool on,bool mounted,std::uint64_t heldMs,int hint) noexcept {
    return on&&heldMs<kTapMs&&(hint==1||(mounted&&hint==-1));
}
// one press: the best answer seen while the button was held (a listed prompt wins over "not listed").
constexpr int Merge(int seen,int answer) noexcept {return seen==1||answer==1?1:(answer==0||seen==0)?0:-1;}
} // namespace kcd2vr::ride_follow_r147
