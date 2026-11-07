#pragma once
#include <cstdint>
// R129 (Miki 6 Nov: "playstation buttons were again not showing by switching in vr menu")
// The Xbox/PlayStation button ART is a Flash-only request: kcd2vr_prompt_d56 (companion Lua) calls
// SetKcd2vrGlyphStyleD68 in the eight companion movies. Up to R128 that request was tied to the
// wh_ui_PreferredInputDevice CVar lease (ApplyF7PromptPresentation): the lease is first tried at mod start,
// the game console is not up yet, the lease fails and
//  - the saved VrPadGlyphStyle=4 was not sent after a restart (CVar missing = return before the art request),
//  - the HUD-reload / save-load re-send in the Run loop was off, it required the lease,
// until some VR menu click happened to take the lease. The art does not need the CVar at all.
// Now the art is queued on every style change, at start and on every native UI/HUD change, and the lease
// is only retried next to it (event driven, no latch, no poll).
namespace kcd2vr::glyph_apply_r129 {
inline constexpr int style(int requested) noexcept {return requested==4?4:2;}
enum class Path : int {None=0,Startup=1,VrMenu=2,UiEdge=3};
inline constexpr const char* name(Path p) noexcept {
    return p==Path::Startup?"STARTUP":p==Path::VrMenu?"VR_MENU":p==Path::UiEdge?"UI_CHANGE":"UNKNOWN";
}
inline constexpr const char* label(int s) noexcept {return style(s)==4?"PLAYSTATION":"XBOX";}
// One apply (start / VR menu ApplyState). applied = style the movies last confirmed (0 = none/failed).
struct Plan {bool queueArt=false,tryLease=false,setPreferred=false;};
inline constexpr Plan plan(bool leaseActive,int requested,int applied) noexcept {
    Plan p{};
    p.queueArt=style(requested)!=applied;     // never behind the CVar lease
    p.tryLease=!leaseActive;                  // retried on this event, no latch
    p.setPreferred=leaseActive&&p.queueArt;   // keep preferred=2 (Xbox semantics) on a style change
    return p;
}
// Run-loop UI edge (HUD recreated, save loaded, native menu opened/closed): always re-send the saved art,
// a recreated movie starts with Xbox art. Old rule: only while the lease was active.
struct Edge {
    std::uint64_t hud=~std::uint64_t(0);unsigned epoch=~0u,visible=~0u;
    bool changed(std::uint64_t h,unsigned e,unsigned v) noexcept {
        if(h==hud&&e==epoch&&v==visible)return false;
        hud=h;epoch=e;visible=v;return true;
    }
};
// KCD2VR.log: one row per change of (style, applied) - the row when the style is applied, not per refresh.
struct Report {
    int style=0,ok=-1;
    bool take(int s,bool applied) noexcept {
        const int o=applied?1:0;const int st=glyph_apply_r129::style(s);
        if(st==style&&o==ok)return false;
        style=st;ok=o;return true;
    }
};
}
