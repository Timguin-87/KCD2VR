#pragma once
// R124 F6 (Miki 6 Nov: "we need also F6 back, as emergency switch between theater mode and immersive").
// F6 is a real toggle again. the V65.29 panic (ui_menu_router::Router::Panic) only drops a MENU owner, so
// before R124 F6 did nothing in free gameplay and nothing in an authored cutscene / native dialogue Theater
// (ui_scene_presentation::Resolve keeps those flat, Panic never touched them).
// one small override, no latch: it ends on the next F6, or as soon as the presentation scene key changes
// (a native menu opens/closes, a cutscene/dialogue starts/ends, a loading screen = every save load).
namespace kcd2vr::theater_toggle_r124 {
enum class Force : int { None=0, Theater=1, Immersive=2 };
struct State { Force force=Force::None; int sceneKey=-1; bool capture=false; };
// theaterShown = what the headset showed last tick. allowToTheater=false is the L3+R3 click (release only).
// returns true when the caller must also run the existing router panic (a menu owner must drop too).
inline bool Press(State& s,bool theaterShown,bool allowToTheater) noexcept {
    if(s.force==Force::Theater){ s.force=Force::None; s.capture=false; return false; } // forced flat -> back to immersive
    if(theaterShown){ s.force=Force::Immersive; s.capture=true; return true; }       // real Theater -> immersive
    if(!allowToTheater) return false;
    s.force=Force::Theater; s.capture=true; return false;                               // immersive -> flat Theater
}
// once per presentation tick, after the scene key of THIS tick is known (and after the panic ran).
inline bool Apply(State& s,int sceneKey,bool policyTheater) noexcept {
    if(s.force==Force::None) return policyTheater;
    if(s.capture){ s.sceneKey=sceneKey; s.capture=false; }
    else if(sceneKey!=s.sceneKey){ s.force=Force::None; return policyTheater; }
    return s.force==Force::Theater;
}
inline const char* Name(Force f) noexcept { return f==Force::Theater?"THEATER":f==Force::Immersive?"IMMERSIVE":"NONE"; }
} // namespace kcd2vr::theater_toggle_r124
