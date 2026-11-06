#pragma once
#include "ui_contextual_hud162r1.h"
#include <array>
#include <cmath>
#include <cstdint>
namespace kcd2vr::ui_palm_alpha44c2 {
// Parent alpha multiplies descendant animation; never changes native _visible.
// No stored GFx pointers. Backend reacquires a retained HUD only at an edge.
struct Gate {const char* name;unsigned group;bool revealAtFullAlpha=false;};
// R59.16C10 (Codex HUD review 3c): gate the Stats CHILD, never bc.mc_StatsContainer - that parent also holds qamWeapon/qamFood, so
// alpha 0 on it blanked an open native quick selector. Buffs/Crime/Wanted/Trespassing belong to the RIGHT reveal (group 2); they were
// group 4, which no branch ever showed. GameLog/FancyEvent/CommonEvent/ShootingContest are native (group 0 = never gated).
inline constexpr std::array<Gate,11> Gates{{
    {"Compass",ui_contextual_hud162r1::CompassGroupD84,true},{"bc.mc_StatsContainer.mc_StatsEle",ui_contextual_hud162r1::VitalsGroupD84,true},{"Buffs",2},
    {"Crime",2},{"Wanted",2},{"Trespassing",2},{"bbc.vignetteHolder",0},
    {"GameLog",0},{"FancyEvent",0},{"CommonEvent",0},{"ShootingContest",0}
}};
// Right palm reveals the vital-bar container (also kept readable during native
// quick selection). Left palm reveals compass. Full-field vignette remains
// native. Names, hints, dialogue and cursor are not gates.
struct Input {bool active=false,left=false,right=false,qam=false;std::uint64_t generation=0,hudLifetime54=0;};
inline std::uint64_t Lifetime54(Input in) noexcept{return in.hudLifetime54?in.hudLifetime54:in.generation;}
inline bool Same(Input a,Input b){return a.active==b.active&&a.left==b.left&&a.right==b.right&&a.qam==b.qam&&a.generation==b.generation&&a.hudLifetime54==b.hudLifetime54;}
struct Report {bool changed=false;unsigned reads=0,writes=0,failures=0;};
template<class Backend>auto NativeAlpha54(Backend& io,unsigned i,float current,float& native,int)
 -> decltype(io.NativeAlpha54(i,current,native),bool()){return io.NativeAlpha54(i,current,native);}
template<class Backend>bool NativeAlpha54(Backend&,unsigned,float,float&,long){return false;}
class Controller {
    struct Saved {float alpha=100,target=100;bool captured=false,owned=false;};
    std::array<Saved,Gates.size()> saved_{};
    Input previous_{};bool have_=false;
public:
    bool Needs(Input in)const noexcept{return !have_||!Same(in,previous_);}
    template<class Backend> Report Update(Backend& io,Input in) {
        Report r{};if(have_&&Same(in,previous_))return r;
        r.changed=true;
        // A child revision requests a bounded refresh, not abandonment of every
        // gate we still own. Only replacement of the actual HUD lifetime makes
        // the saved ownership inapplicable; unchanged gates restore on exit.
        if(!have_||Lifetime54(in)!=Lifetime54(previous_))saved_={};
        if(!in.active){bool owned=false;for(auto&s:saved_)owned|=s.owned;
            if(!owned){previous_=in;have_=true;return r;}}
        if(!io.Acquire()){r.failures=1;previous_=in;have_=true;return r;}
        for(unsigned i=0;i<Gates.size();++i){auto&s=saved_[i];
            const bool shown=Gates[i].group==0||!in.active||(Gates[i].group==1&&in.left)||(Gates[i].group==2&&in.right);
            // Native Stats can be alpha=0 before palm mode starts. A reveal is
            // an explicit vital display, not restoration of that hidden alpha.
            // Keep ownership through reveal so exit still restores native state.
            const bool explicitReveal=in.active&&Gates[i].revealAtFullAlpha;
            if(shown&&!s.owned&&!explicitReveal)continue;
            if(!io.Resolve(i,Gates[i].name)){++r.failures;continue;}
            float live=0;++r.reads;if(!io.Read(i,live)||!std::isfinite(live)||live<0||live>100){++r.failures;continue;}
            // Exact writer observations preserve native intent even while our
            // scoped alpha gate keeps its requested value from reaching pixels.
            float native=live;const bool known=NativeAlpha54(io,i,live,native,0);
            if(known||!s.captured||(s.owned&&std::fabs(live-s.target)>.01f)){s.alpha=known?native:live;s.captured=true;}
            const float target=shown?(explicitReveal?100.f:s.alpha):0.f;
            if(std::fabs(live-target)>.01f){++r.writes;s.target=target;s.owned=true;
                if(!io.Write(i,target)){++r.failures;continue;}
                float back=0;++r.reads;if(!io.Read(i,back)||!std::isfinite(back)||std::fabs(back-target)>.01f){
                    // Retain restoration ownership if the setter partly changed the value.
                    s.target=target;s.owned=true;++r.failures;continue;}
            }
            s.target=target;s.owned=!shown||explicitReveal;
            if(!s.owned)s.captured=false;
        }
        io.Release();previous_=in;have_=true;return r;
    }
    // R59.16C10R16 (ChatGPT/Miki 15 Oct "one owner": hidden elements came back - the game's adaptive fader and its own alpha
    // writes re-show them, and the edge-only owner above never noticed). While palm mode is active, a gate WE hide (owned, target
    // 0) that the game made visible again is hidden again; a FULL native value (>= 99) is adopted as the one restored on exit (a
    // sample in the middle of a native fade is not). Reads every
    // call, writes only on a mismatch; revealed gates and native gates (group 0) are never touched. Caller paces it (1 s).
    template<class Backend> Report Reassert(Backend& io) {
        Report r{};if(!have_||!previous_.active)return r;
        bool any=false;for(const auto& s:saved_)any=any||(s.owned&&s.target==0.f);
        if(!any)return r;
        if(!io.Acquire()){r.failures=1;return r;}
        for(unsigned i=0;i<Gates.size();++i){auto& s=saved_[i];
            if(!s.owned||s.target!=0.f||Gates[i].group==0)continue;
            if(!io.Resolve(i,Gates[i].name)){++r.failures;continue;}
            float live=0;++r.reads;if(!io.Read(i,live)||!std::isfinite(live)||live<0||live>100){++r.failures;continue;}
            if(live<=.01f)continue;
            if(live>=99.f){s.alpha=live;s.captured=true;}
            r.changed=true;++r.writes;
            if(!io.Write(i,0.f))++r.failures;
        }
        io.Release();return r;
    }
};
}
