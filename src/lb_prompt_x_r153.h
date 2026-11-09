#pragma once
// R153 (Miki 8 Nov: "I could not progress the quest, because Henry uses LB to look too - without LB I could not
// progress (again only a contextual LB) - so we need to change the LB for good to something else, like X").
// the game's "hold to look / watch" prompt is the infinite hold interaction: actionmaps infinite_hold_interaction_init
// (press) and infinite_hold_interaction_inprogress (hold, release ends it), both LB only in the retail profile. the VR
// grip never sends LB in normal play, so the prompt could not be used. the companion defaultProfile.xml now binds
// both actions to X (PS Square) as well as LB (R156: X first, LB second - see the profile comment. R157: the Follow row
// is retail LB-only again, Follow on X is mod-side only - ride_follow_r147.h).
// the left X button normally sends nothing while held (short tap = 120 ms Quick Speak X pulse after the release,
// 0.5 s hold = torch / inventory), which would start the interaction and stop it at once. so: on a new X press the
// mod asks the game's hint rows (same reader as R25 carry / R147 follow) whether an infinite_hold_interaction_*
// prompt is listed. listed -> this press is passed through as a real held X (press, hold, release), no Quick Speak
// pulse, no torch, no inventory. not listed / not readable within 150 ms -> the press goes on exactly as before.
// decided per press, nothing is remembered between presses (no latch).
// R155 (Miki 8 Nov, R153 build, quest still stuck): the hint rows did NOT list the prompt (no "held through" row at his
// three X presses 12:59:06 / :24 / :25), but the log shows the game dispatching infinite_hold_interaction_init_press for
// each of those X taps - the hold map WAS enabled, the prompt is just not an interactor row of the focused entity. so a
// second source: the mod's own OnAction detour (fist_native.inl) notes the tick of every infinite_hold_interaction_*
// dispatch; a new X press within kMapSeenMs of it counts as "listed" and is held through. the first tap at such a prompt
// still goes the old way (its 120 ms X pulse is what reveals the map), the next press holds. no latch: the memory is
// 4 s and every press decides again.
// R156 (Miki 8 Nov, R155 build: "before it showed 'look' - now not any more", quest still stuck): third source, BEFORE the
// first tap: the game enables the "infinite_hold_interaction_init" action map exactly while it offers the prompt. the player
// view lens (activity_lens59c10r1.cpp, game thread, every frame) reads that map's enabled byte with the verified named-map
// walk of dialogue_input_owner19c3.h and stores the tick here; a new X press within kMapEnabledMs of it is "listed" at
// once, so the FIRST press already holds (no wasted 120 ms pulse that starts and ends the look). no latch: the tick ages
// out in 300 ms when the map is disabled again, every press decides again. the profile rows go back to the R153 order
// (X first, LB second): that order showed the prompt (as LB) in his R153 run, the R155 order (LB first, X second) showed
// nothing - so the HUD draws LB for the prompt, and X (held) is the VR button for it.
#include <atomic>
#include <cstdint>
namespace kcd2vr::lb_prompt_x_r153 {
inline constexpr std::uint64_t kDecideMs=150; // well under the 300 ms tap (R147) and the 500 ms torch/inventory hold
inline constexpr std::uint64_t kMapSeenMs=4000; // R155: a hold action dispatched this recently = the prompt is offered
inline constexpr std::uint64_t kMapEnabledMs=300; // R156: the hold map was read ENABLED this recently = the prompt is offered now
inline std::atomic<std::uint64_t> holdMapSeenMs{0}; // R155: tick of the last infinite_hold_interaction_* dispatch (OnAction detour)
inline std::atomic<std::uint64_t> holdMapEnabledMs{0}; // R156: tick of the last frame the hold-init action map was read enabled (view lens)
inline constexpr char kPrefix[]="infinite_hold_interaction_";
// R155: rows = hint row answer (1 listed, 0 not, -1 unknown); seen = holdMapSeenMs. a recent dispatch upgrades 0 / -1 to 1.
// R156: enabled = holdMapEnabledMs; the map read enabled within kMapEnabledMs upgrades too (before any tap).
constexpr int Listed(int rows,std::uint64_t now,std::uint64_t seen,std::uint64_t enabled=0) noexcept {
    return rows==1||(seen&&now>=seen&&now-seen<=kMapSeenMs)||(enabled&&now>=enabled&&now-enabled<=kMapEnabledMs)?1:rows;
}
enum class Route : unsigned char {Idle,Asking,Normal,Pass};
// one step per frame. held = the gameplay X press is still down. answer: 1 listed, 0 not listed, -1 unknown.
constexpr Route Next(Route r,bool held,bool newPress,std::uint64_t heldMs,int answer) noexcept {
    if(!held)return Route::Idle;
    if(newPress)r=Route::Asking;
    if(r!=Route::Asking)return r;
    if(heldMs>=kDecideMs)return Route::Normal; // too late: never switch a press half-way
    return answer==1?Route::Pass:Route::Asking;
}
// hint row action name starts with kPrefix (both infinite hold actions).
constexpr bool Matches(const char* name) noexcept {
    if(!name)return false;
    for(unsigned k=0;kPrefix[k];++k)if(name[k]!=kPrefix[k])return false;
    return true;
}
} // namespace kcd2vr::lb_prompt_x_r153
