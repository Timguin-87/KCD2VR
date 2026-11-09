#pragma once
#include <atomic>
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace kcd2vr::prompt_policy_d80 {
inline std::atomic<float> minigameScale{.45f};
inline std::atomic<bool> timed{true},showHints{true};
inline std::atomic<std::uint64_t> refreshRevision{0};
inline void Refresh() noexcept {refreshRevision.fetch_add(1,std::memory_order_release);}
// Only preference/UI-lifetime changes touch the exact retail CVar. Turning
// hints off/on has an explicit 0/1 value; shutdown restores only our own write.
class HintsLease {
    bool attempted=false,enabled=false,wanted=true,owned=false;
    std::uint64_t epoch=0;
    int original=0,last=0;
public:
    template<class IO> bool Update(IO& io,bool enable,bool want,std::uint64_t life) noexcept {
        if(attempted&&enable==enabled&&want==wanted&&life==epoch)return false;
        attempted=true;enabled=enable;wanted=want;epoch=life;
        if(!enable&&!owned)return false;
        int current=0;if(!io.Read(current))return true;
        if(!enable){
            if(current!=last){owned=false;return true;}
            if(io.Write(original)){int check=0;if(io.Read(check)&&check==original)owned=false;}
            return true;
        }
        if(owned&&current!=last)owned=false; // external/native changes win until this explicit event
        const int target=want?1:0;
        if(current==target)return true;
        if(!owned)original=current;
        // Keep restore debt if the write succeeds but readback is unavailable.
        if(io.Write(target)){last=target;owned=true;}
        return true;
    }
};
// Coordinate the ordinary C++ scaler with the movie-local station owner.
// One atomic word keeps worker requests and native-VM completion ordered.
class MovieLease {
    // Low bits: requested, restore debt, active dispatch, old native owner released.
    std::atomic<std::uint64_t> state{0};
public:
    void Prepare(bool active,std::uint64_t hudGeneration) noexcept {
        const auto epoch=hudGeneration<<4;auto before=state.load(std::memory_order_acquire);
        for(;;){const auto flags=(before&~std::uint64_t(15))==epoch?(before&15):0;
            const auto after=epoch|(active?(flags|3):(flags&~std::uint64_t(1)));
            if(after==before||state.compare_exchange_weak(before,after,std::memory_order_acq_rel))return;}
    }
    std::uint64_t Epoch()const noexcept{return state.load(std::memory_order_acquire)&~std::uint64_t(15);}
    bool NativeEnabled(bool requested)const noexcept{return requested&&!(state.load(std::memory_order_acquire)&2);}
    bool CanDispatch(bool active)const noexcept {
        const auto value=state.load(std::memory_order_acquire);
        return !active||(value&9)==9;
    }
    void NativeRelinquished(std::uint64_t epoch) noexcept {
        auto before=state.load(std::memory_order_acquire);
        for(;;){if((before&~std::uint64_t(15))!=epoch||!(before&2))return;
            const auto after=before|8;if(after==before||state.compare_exchange_weak(before,after,std::memory_order_acq_rel))return;}
    }
    void Completed(std::uint64_t epoch,bool active,bool dispatched,bool finalAttempt) noexcept {
        auto before=state.load(std::memory_order_acquire);
        for(;;){if((before&~std::uint64_t(15))!=epoch)return;auto after=before;
            if(active&&dispatched)after|=6;
            // Missing companion before any active dispatch has no restore debt.
            // A failed restoration after a dispatch retains debt until a native
            // HUD recreation; never scale an unknown reduced value a second time.
            if(!active&&!(before&1)&&(dispatched||(finalAttempt&&!(before&4))))after&=~std::uint64_t(14);
            if(after==before||state.compare_exchange_weak(before,after,std::memory_order_acq_rel))return;}
    }
};
inline MovieLease movieLease;
struct Frame {
    bool active=false,palm=false,timed=true;
    std::uint64_t lifetime=0,now=0;
    unsigned phase=0;
    float scale=.45f;
    bool alchemy=false;
};
struct Presentation {
    bool active=false,visible=true;
    unsigned percent=100;
    bool operator==(const Presentation& b) const noexcept {
        return active==b.active&&visible==b.visible&&percent==b.percent;
    }
    bool operator!=(const Presentation& b) const noexcept {return !(*this==b);}
};
// R145 (Miki 8 Nov: "the alchemy and smithing prompts and icons over the things are again bigger"): the station size
// was sent only on station entry / new step / palm look (4 tries in 0.4 s). the C++ scaler gives the station children
// back to their native size before that, so a child the game re-shows or re-scales later (world icons, the control
// panel that comes up after the camera move) or a try that came too early stayed at full native size until the next
// step. while a station is active the same request is sent again every kReassertMs: the movie owner re-applies the
// size only to a child whose size is not ours any more (same base, no double scaling), nothing else changes.
inline constexpr std::uint64_t kReassertMs=1000;
struct Publication {
    Presentation previous{};std::uint64_t hudGeneration=0,revision=0,at=0;bool have=false;
    bool Needed(Presentation next,std::uint64_t generation,std::uint64_t refresh)const noexcept {
        return (next.active||have)&&(!have||next!=previous||generation!=hudGeneration||refresh!=revision);
    }
    bool Reassert(Presentation next,std::uint64_t now)const noexcept {
        return have&&next.active&&previous.active&&now&&(now<at||now-at>=kReassertMs);
    }
    void Published(Presentation next,std::uint64_t generation,std::uint64_t refresh,std::uint64_t now=0) noexcept {
        previous=next;hudGeneration=generation;revision=refresh;at=now;have=true;
    }
};
// One worker owns this timer. A held-open hand does not indefinitely extend it;
// closing and opening the palm is a new deliberate reveal. No pose is changed.
struct State {
    bool active=false,palm=false,timed=false;
    std::uint64_t lifetime=0,until=0,lastNow=0;
    unsigned phase=0;
    Presentation update(const Frame& f) noexcept {
        if(!f.active||!f.lifetime||!f.now){*this={};return {};}
        // Alchemy names share the prompt movie children. Fading those children
        // also hides ingredient identification, so only smithing uses the timer.
        const bool effectiveTimed=f.timed&&!f.alchemy;
        const bool edge=!active||lifetime!=f.lifetime||phase!=f.phase||
            (!palm&&f.palm)||(!timed&&effectiveTimed)||f.now<lastNow;
        // R90 (R88 craft): an open-palm glance reveals station hints for 3 s
        // (Miki D85R5); a new station or step still shows them for 10 s.
        if(edge)until=f.now+((!palm&&f.palm&&active&&lifetime==f.lifetime&&phase==f.phase)?3000:10000);
        active=true;palm=f.palm;timed=effectiveTimed;lifetime=f.lifetime;phase=f.phase;lastNow=f.now;
        const float scale=std::isfinite(f.scale)?std::clamp(f.scale,.25f,1.f):.45f;
        return {true,!effectiveTimed||f.now<until,unsigned(std::lround(scale*100.f))};
    }
};
}
