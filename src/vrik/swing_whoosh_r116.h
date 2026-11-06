#pragma once
// R116 (Miki 6 Nov: "the swing sound comes later - like 0.5 s after I start the swing - so it feels disconnected").
// the weapon whoosh is an animevent of the NATIVE attack clip (audio_trigger c_w_sword_flick at normalized
// t=0.46-0.64 of 1d-combat_*_natk_slash_lngsw.bspace, R95 log 18:10:31-18:11:14). in VR the clip runs hidden
// behind the tracked arms, so the whoosh plays when the CLIP gets there: 160-380 ms after the hand's peak speed,
// 270-650 ms after the hand passed 2.5 m/s. the clip time is not touched here (hit window / native admission
// stay exactly as they are). instead:
//  1. at the physical RELEASE (the commit edge we already send to the game, NativePhysicalAction117) we play the
//     same authored trigger through the player's audio proxy (same Lua leaves as the D80 alchemy sounds),
//  2. Henry's skeleton anim-event callback (the R59.16C10R11 tap) skips the clip's own weapon-flick event for
//     ~0.9 s after that, so you hear one whoosh, with your swing, not two.
// kicks, punches, pommel strikes: untouched (no request, kick release cancels the mute window).
// INI VrSwingWhooshR116 (true) = Experiments VRIK+ row 1. OFF: the clip's own whoosh only (R115 behaviour).
#include "../native_script_t2.h"
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace kcd2vr::swing_whoosh_r116 {
inline std::atomic<bool> enabled{true};
enum class Kind:unsigned char {Cut=0,Stab=1};
inline constexpr std::uint64_t kMuteMs=900;   // longest measured release->clip whoosh gap is ~650 ms
inline constexpr std::uint64_t kStaleMs=150;  // a request the script update did not reach in time is dropped
inline constexpr unsigned kNameCap=40;

// weapon_class.xml: 1 sword, 2 sabre, 3 axe, 4 longsword, 5 mace, 7 halberd, 16 hunting sword.
// names = the audio_trigger params of the shipped male.animevents (AUTHORED_EVENT_JOIN_D60.json:
// longsword combos c_w_sword_flick(_stab), shortsword c_w_sword_short_flick(_stab), blunt c_w_mace_flick,
// halberd c_w_halberd_flick). a name the game itself plays for this class replaces it (learn()).
inline const char* authored(int classId,Kind k) noexcept {
    const bool stab=k==Kind::Stab;
    switch(classId){
    case 4:return stab?"c_w_sword_flick_stab":"c_w_sword_flick";
    case 1:case 2:case 16:return stab?"c_w_sword_short_flick_stab":"c_w_sword_short_flick";
    case 3:case 5:return "c_w_mace_flick";
    case 7:return "c_w_halberd_flick";
    default:return nullptr;
    }
}
// the clip's weapon whoosh: audio_trigger c_w_<weapon>_flick[...] (not hits, pommel, stab in/out, foley).
inline bool weaponFlick(const char* name,const char* param) noexcept {
    return name&&param&&!std::strcmp(name,"audio_trigger")&&!std::strncmp(param,"c_w_",4)&&std::strstr(param,"flick")&&
        std::strlen(param)<kNameCap;
}
inline bool validName(const char* s) noexcept {
    if(!s||std::strncmp(s,"c_w_",4)||!std::strstr(s,"flick"))return false;
    std::size_t n=0;for(;s[n];++n){const char c=s[n];if(n>=kNameCap-1||!((c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_'))return false;}
    return n>4;
}

class State {
    struct Pending {std::uint64_t serial=0,tick=0;char name[kNameCap]{};};
    struct Learned {int classId=-1;char name[2][kNameCap]{};};
    mutable std::atomic_flag busy_=ATOMIC_FLAG_INIT;
    Pending pending_{};Learned learned_{};
    std::uint64_t serial_=0;
    std::atomic<bool> wanted_{false};
    std::atomic<int> lastClass_{-1};
    std::atomic<std::uint64_t> lastRelease_{0},muteUntil_{0};
    struct Lock {const State& s;explicit Lock(const State& x):s(x){while(s.busy_.test_and_set(std::memory_order_acquire)){}}~Lock(){s.busy_.clear(std::memory_order_release);}};
public:
    std::atomic<std::uint64_t> played{0},muted{0},refused{0},passed{0}; // R124 passed: a clip flick that was NOT skipped (switch ON)
    // physical release of a cut / stab with a melee weapon (game thread, after the native release was dispatched)
    bool request(int classId,Kind k,std::uint64_t now) noexcept {
        if(!enabled.load(std::memory_order_acquire)||!now)return false;
        lastClass_.store(classId,std::memory_order_release);lastRelease_.store(now,std::memory_order_release);
        // the previous swing's window ends here: this swing's clip whoosh is muted only after OUR whoosh played
        muteUntil_.store(0,std::memory_order_release);
        const char* pick=nullptr;char copy[kNameCap]{};
        {Lock l(*this);
            const char* table=authored(classId,k);
            if(learned_.classId==classId){
                const unsigned i=unsigned(k);
                if(learned_.name[i][0])pick=learned_.name[i];
                // no stab-specific authored name for this class: the learned cut whoosh is the better guess
                else if(k==Kind::Stab&&learned_.name[0][0]&&!(table&&std::strstr(table,"_stab")))pick=learned_.name[0];
            }
            if(!pick)pick=table;
            if(!pick||!validName(pick))return false;
            std::snprintf(copy,sizeof(copy),"%s",pick);
            pending_.serial=++serial_;pending_.tick=now;std::memcpy(pending_.name,copy,sizeof(copy));
        }
        wanted_.store(true,std::memory_order_release);return true;
    }
    // a kick (or anything that is not our weapon swing) ends the mute window at once
    void cancel() noexcept {muteUntil_.store(0,std::memory_order_release);}
    bool wanted() const noexcept {return wanted_.load(std::memory_order_acquire);}
    bool take(char (&name)[kNameCap],std::uint64_t& serial,std::uint64_t& tick) noexcept {
        Lock l(*this);if(!pending_.serial)return false;
        std::memcpy(name,pending_.name,kNameCap);serial=pending_.serial;tick=pending_.tick;pending_={};
        wanted_.store(false,std::memory_order_release);return true;
    }
    // our whoosh really went to the audio system: mute the clip's own one for this swing
    void dispatched(std::uint64_t releaseTick) noexcept {
        if(releaseTick&&releaseTick==lastRelease_.load(std::memory_order_acquire))
            muteUntil_.store(releaseTick+kMuteMs,std::memory_order_release);
    }
    // Henry's anim-event callback (any thread): true = do not run the original for this event
    bool onEvent(const char* name,const char* param,std::uint64_t now) noexcept {
        if(!weaponFlick(name,param))return false;
        const auto at=lastRelease_.load(std::memory_order_acquire);const int cls=lastClass_.load(std::memory_order_acquire);
        if(at&&now>=at&&now-at<=1500&&cls>=0&&validName(param)){ // learn what THIS weapon's clips really play
            Lock l(*this);const unsigned i=std::strstr(param,"_stab")?1u:0u;
            if(learned_.classId!=cls){learned_=Learned{};learned_.classId=cls;}
            std::snprintf(learned_.name[i],kNameCap,"%s",param);
        }
        const auto until=muteUntil_.load(std::memory_order_acquire);
        if(!enabled.load(std::memory_order_acquire))return false;
        if(!until||now>until){passed.fetch_add(1,std::memory_order_relaxed);return false;}
        muted.fetch_add(1,std::memory_order_relaxed);return true;
    }
    bool wantsTap() const noexcept {return enabled.load(std::memory_order_acquire);}
};
inline State state;
inline bool Wanted() noexcept {return state.wanted();}
inline bool WantsTap() noexcept {return state.wantsTap();}
inline void Request(int classId,Kind k,std::uint64_t now) noexcept {(void)state.request(classId,k,now);}
inline void Cancel() noexcept {state.cancel();}
inline bool MuteEvent(const char* name,const char* param,std::uint64_t now) noexcept {return state.onEvent(name,param,now);}

// one queued whoosh per script update (native_supply_t2 SupplyScriptServiceT2, the same original-first
// CScriptSystem callback that already plays the D80 alchemy sounds). reply text goes to the caller's trace.
inline bool Service(const native_script_t2::Context& ctx,char* reply,std::size_t replyCap,char (&name)[kNameCap]) noexcept {
    name[0]=0;
    if(reply&&replyCap)reply[0]=0;
    if(!Wanted()||!ctx.Current())return false;
    std::uint64_t serial=0,tick=0;
    if(!state.take(name,serial,tick))return false;
    if(!enabled.load(std::memory_order_acquire)||ctx.now<tick||ctx.now-tick>kStaleMs){state.refused.fetch_add(1);return false;}
    char input[96]{};
    const int n=std::snprintf(input,sizeof(input),"W116|%llu|%s",(unsigned long long)serial,name);
    if(n<=0||std::size_t(n)>=sizeof(input))return false;
    char output[96]{};
    const bool called=ctx.CallPacket("kcd2vr_swing_whoosh_r116",input,output,sizeof(output));
    if(reply&&replyCap)std::snprintf(reply,replyCap,"%s",called?output:"UNAVAILABLE");
    const char* ok=std::strstr(output,"|DISPATCHED");
    if(!called||!ok||ok[11]){state.refused.fetch_add(1);return false;}
    state.dispatched(tick);state.played.fetch_add(1);return true;
}
}
