#pragma once
// D32 (Miki: "f11 diagnostics for the missing reloads for rifles"; D30 found the F11 rung-2 rifle reload waits for
// hand stages nobody is told about). pure: which one-line hint to show, when a stage counts as done (haptic tick),
// and the silent native fallback deadline (was 12 s).
#include <cstdint>
namespace kcd2vr::rifle_hint_d32 {
inline constexpr std::uint64_t kDeadlineMs=4000; // silent fallback: native reload after 4 s without the hand stages
enum class Hint:int {None=0,Grip=1,Pull=2,Place=3};
// emptyStage: machine still in Empty (span) stage; held: grip closed near the grip anchor in the span stage;
// working: machine in Working (place) stage.
inline Hint hintFor(bool neutral,bool emptyStage,bool held,bool working) noexcept {
    if(working)return Hint::Place;
    if(emptyStage)return held?Hint::Pull:Hint::Grip;
    (void)neutral;return Hint::None;
}
inline const char* text(Hint h) noexcept {
    switch(h){
    case Hint::Grip:return "rifle reload: grip at the barrel (other hand)";
    case Hint::Pull:return "rifle reload: pull back 10 cm and let go";
    case Hint::Place:return "rifle reload: grip again at the barrel and let go";
    default:return nullptr;}
}
struct State {Hint shown=Hint::None;bool held=false;int stagesDone=0;};
struct Tick {const char* show=nullptr;bool haptic=false;};
// call once per service tick with the current stage facts; stageAdvanced = the machine moved Empty->Working or ->Loaded
inline Tick step(State& s,Hint now,bool held,bool stageAdvanced) noexcept {
    Tick t{};
    if(now!=s.shown){s.shown=now;t.show=text(now);}
    if(held&&!s.held){t.haptic=true;} // grip taken at the barrel
    if(stageAdvanced){t.haptic=true;++s.stagesDone;}
    s.held=held;return t;
}
}
