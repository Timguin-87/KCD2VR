#pragma once
// R141 Minimal HUD (Miki 7 Nov: "a version of disabled HUD with only interaction prompts (like control prompts) and
// health and compass by the hand looking and dialogue text, but everything other disabled ... it still comes back,
// so we need a permanent option").
//
// why hidden elements came back: the game's HUD manager re-evaluates every HUD element on its own state changes
// (WHGame 0x1F7C940 walks all 28 elements -> 0x555A10 refresh -> lambda 0x556164: FindChild + SetDisplayInfo with the
// visible flag 0x400). the mod's native writer hooks only filtered SetVisible (0x1C51240) and the ALPHA field of
// SetDisplayInfo (NativeHudDisplay54), never its visible flag, and the mod's own visibility controller runs only on
// its own events. so after a dialogue, a menu, a save load or a new HUD the manager simply showed them again.
//
// the game's own permanent switch: one int cvar per HUD element, "wh_ui_Show" + element name (0x1F5E3F0 builds the
// name with "%s%s", default 1, help "Toggles element visibility"). the refresh ANDs it into every evaluation and the
// event queues (FancyEvent, CommonEvent, GameLog, SkillCheck, ItemTransfer, InfoText, Tutorial) get
// SetPlaying...(false) from it, so their popups never play. the mod already uses one of them (wh_ui_ShowHints, D80;
// R107 log: R105 wrote it 0 and the A-grab/open hints stayed gone). element enum (0x555AC0):
//  0 Compass 1 Stats 2 QAMWeapon 3 QAMFood 4 Subtitles 5 InfoText 6 GameLog 7 Hints 8 DialogLeft 9 DialogRight
// 10 Cursor 11 Crime 12 Wanted 13 PopUpBackground 14 TutorialMessage 15 FancyEvent 16 SkillCheck 17 ItemTransfer
// 18 Buffs 19 CommonEvent 20 DiceCursor 21 Trespassing 22 RatioStrips 23 ShootingContest 24 Bubbles
// 25 TutorialInDialog 26 DiceContainer 27 Vignette
//
// Minimal HUD ON keeps: Hints (button/interaction prompts), TutorialMessage + TutorialInDialog (control prompts),
// InfoText (text at the cursor), Subtitles, DialogLeft/Right (dialogue lines + choices), Bubbles, the minigame
// elements (dice, shooting contest), the full-screen faders/vignette, and the palm glance: compass on the left palm,
// health/stamina on the right palm (R105/D84 palm HUD, forced on while Minimal HUD is on).
// hidden: status icons (Buffs, Crime, Wanted, Trespassing), quest/xp/money/perk/item popups (FancyEvent, CommonEvent,
// GameLog, SkillCheck, ItemTransfer), the whole action cursor (centre point, combat direction cross with its lock and
// perfect block / master strike icons), compass bar and health bar when you do not look at the palm.
// quick weapon/potion slots stay as R115 made them (only while you select, 3 s): they are a selection prompt.
// tutorial popups (wh_ui_TutorialsEnabled) are off too while ON, see TutorialLease below.
// INI VrMinimalHudR141 (default false), Experiments GEAR 3 row 6, VR HUD page, release page 3 row 4.
#include "ui_contextual_hud162r1.h"
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

namespace kcd2vr::ui_minimal_hud_r141 {
inline std::atomic<bool> enabled{false};
// game cvars written 0 while ON (hold, re-checked every second and on every HUD/menu lifetime change).
inline constexpr std::array<const char*,9> kCvars{{
    "wh_ui_ShowBuffs","wh_ui_ShowCrime","wh_ui_ShowWanted","wh_ui_ShowTrespassing",
    "wh_ui_ShowFancyEvent","wh_ui_ShowCommonEvent","wh_ui_ShowGameLog","wh_ui_ShowSkillCheck","wh_ui_ShowItemTransfer"}};
// our HUD.xml names, same order as kCvars: also set invisible every HUD generation and every 500 ms (an ActionScript
// re-show inside hud.gfx does not ask the cvar). kCursorClip = cccursor.actioncursor, only in free gameplay.
inline constexpr std::array<const char*,9> kClips{{
    "Buffs","Crime","Wanted","Trespassing","FancyEvent","CommonEvent","GameLog","SkillCheck","ItemTransfer"}};
inline constexpr const char* kCursorClip="Cursor";
inline constexpr std::uint64_t kRecheckMs=1000,kRehideMs=500;
// saved HUD preferences while ON: crosshair, compass, vitals and status off. the palm HUD (VisibilityMask) still
// turns the compass/vitals groups into the left/right palm reveal; status stays off even on the right palm.
inline constexpr unsigned kPreferences=ui_contextual_hud162r1::HideCrosshair163|ui_contextual_hud162r1::HideCompass163|
    ui_contextual_hud162r1::HideLower163|ui_contextual_hud162r1::HideStatus163;
inline constexpr unsigned Preferences(unsigned saved,bool on) noexcept {return on?(saved|kPreferences):saved;}
inline constexpr bool PalmEnabled(bool contextualHud,bool on) noexcept {return contextualHud||on;}

struct Report {bool ran=false;unsigned held=0,writes=0,failed=0,restored=0;};
// Holds the cvars at 0. original = the value seen before our first write, written back when OFF (only if the cvar
// is still our 0: a value somebody else set later is theirs). a refused write / failed readback is retried on the
// next check (1 s) and on every lifetime change: no latch. IO: bool Read(unsigned,int&), bool Write(unsigned,int).
template<std::size_t N> class CvarLeaseT {
    struct Entry {bool owned=false,zero=false;int original=1;};
    std::array<Entry,N> e_{};
    bool have_=false,want_=false;std::uint64_t epoch_=0,last_=0;
public:
    bool Owned(unsigned i) const noexcept {return i<e_.size()&&e_[i].owned;}
    bool AnyOwned() const noexcept {for(const auto&x:e_)if(x.owned)return true;return false;}
    // bit i: cvar i was 0 at the last check while ON (ours or the player's own 0) = the game itself keeps it hidden.
    unsigned ZeroMask() const noexcept {unsigned m=0;for(unsigned i=0;i<e_.size();++i)if(e_[i].zero)m|=1u<<i;return m;}
    bool Due(bool want,std::uint64_t epoch,std::uint64_t now) const noexcept {
        if(!want)return AnyOwned();
        return !have_||!want_||epoch!=epoch_||now<last_||now-last_>=kRecheckMs;
    }
    template<class IO> Report Update(IO& io,bool want,std::uint64_t epoch,std::uint64_t now) noexcept {
        Report r{};if(!Due(want,epoch,now)){want_=want;return r;}
        r.ran=true;have_=true;want_=want;epoch_=epoch;last_=now;
        for(unsigned i=0;i<e_.size();++i){auto&x=e_[i];int current=0;
            if(want){
                if(!io.Read(i,current)){x.zero=false;++r.failed;continue;}
                if(current==0){x.zero=true;++r.held;continue;} // ours, or the player's own 0 (then not ours to restore)
                if(!x.owned)x.original=current;
                int check=1;
                if(io.Write(i,0)&&io.Read(i,check)&&check==0){x.owned=true;x.zero=true;++r.writes;++r.held;}
                else {x.zero=false;++r.failed;} // refused: next check tries again
            } else if(x.owned){
                if(!io.Read(i,current)){++r.failed;continue;} // keep the restore debt, retry next call
                if(current!=0){x.owned=false;x.zero=false;continue;} // changed by someone else: theirs now
                if(io.Write(i,x.original)){x.owned=false;x.zero=x.original==0;++r.restored;} else ++r.failed;
            }
        }
        return r;
    }
};
using CvarLease=CvarLeaseT<kCvars.size()>;

// R141 tutorials (Miki 7 Nov: "we need in HUD options to disable tutorial messages too"). the game's own option cvar
// wh_ui_TutorialsEnabled ("Toggles tutorials. Default is 1.", registered at WHGame 0x600777, read by the tutorial
// check 0x822A60 = GetCVar(name)->GetIVal()>0 on every tutorial, loaded from the game options at 0x79C70F next to
// wh_ui_SubtitlesEnabled / wh_ui_BubblesEnabled) held at 0 = the same as the game's own Tutorials OFF: no tutorial
// popups. the HUD clip tl.tutorial itself is NOT hidden: crafting / station control prompts use it too.
// it is a saved game option: in the main menu and the pause menu (where the options screen is) the player's own
// value is given back, so the game never saves our 0; back in the game it is held again. no latch.
// INI VrHideTutorialsR141 (default false = shown). also hidden while Minimal HUD is on.
inline std::atomic<bool> hideTutorials{false};
inline constexpr std::array<const char*,1> kTutorialCvar{{"wh_ui_TutorialsEnabled"}};
using TutorialLease=CvarLeaseT<1>;
inline constexpr bool TutorialsHidden(bool hide,bool minimal) noexcept {return hide||minimal;}
inline constexpr bool TutorialHold(bool hidden,bool optionsReachable) noexcept {return hidden&&!optionsReachable;}

// 500 ms / new-HUD schedule for the _visible re-hide (same rule as R125 UpdateCombatCrossArtR125).
class Rehide {
    std::uint64_t generation_=0,last_=0;bool have_=false;
public:
    bool Due(std::uint64_t generation,std::uint64_t now) const noexcept {
        return !have_||generation!=generation_||now<last_||now-last_>=kRehideMs;
    }
    void Done(std::uint64_t generation,std::uint64_t now) noexcept {have_=true;generation_=generation;last_=now;}
    void Reset() noexcept {have_=false;}
};
} // namespace kcd2vr::ui_minimal_hud_r141
