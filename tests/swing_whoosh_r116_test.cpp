// R116 host test: weapon whoosh at the physical release, the clip's own whoosh skipped (vrik/swing_whoosh_r116.h).
// names checked against the shipped male.animevents by tests/swing_whoosh_r116_lua_test.py.
#include "vrik/swing_whoosh_r116.h"
#include <cassert>
#include <cstdio>
#include <cstring>
namespace w=kcd2vr::swing_whoosh_r116;
static int checks=0;
#define CK(x) do{++checks;assert(x);}while(0)
static bool takeName(const char* expect,std::uint64_t& tick){
    char n[w::kNameCap]{};std::uint64_t serial=0;tick=0;
    if(!w::state.take(n,serial,tick))return false;
    return !std::strcmp(n,expect)&&serial>0;
}
int main(){
    std::uint64_t t=0;
    // table: longsword cut/stab, short sword, mace/axe, halberd; unknown class (dagger 0, fists -1) = nothing
    CK(!std::strcmp(w::authored(4,w::Kind::Cut),"c_w_sword_flick"));
    CK(!std::strcmp(w::authored(4,w::Kind::Stab),"c_w_sword_flick_stab"));
    CK(!std::strcmp(w::authored(1,w::Kind::Cut),"c_w_sword_short_flick"));
    CK(!std::strcmp(w::authored(16,w::Kind::Stab),"c_w_sword_short_flick_stab"));
    CK(!std::strcmp(w::authored(3,w::Kind::Cut),"c_w_mace_flick"));
    CK(!std::strcmp(w::authored(7,w::Kind::Stab),"c_w_halberd_flick"));
    CK(!w::authored(0,w::Kind::Cut)&&!w::authored(-1,w::Kind::Cut)&&!w::authored(9,w::Kind::Cut));
    // what the tap mutes: only c_w_*flick* audio triggers
    CK(w::weaponFlick("audio_trigger","c_w_sword_flick"));
    CK(!w::weaponFlick("audio_trigger","c_arm_whoosh")&&!w::weaponFlick("audio_trigger","c_dagger_flick")&&
       !w::weaponFlick("audio_trigger","c_w_sword_pommel_hit")&&!w::weaponFlick("foley","c_w_sword_flick")&&!w::weaponFlick("footstep",""));
    // nothing played yet: the clip whoosh is NOT skipped (R115 behaviour until our sound really went out)
    CK(!w::MuteEvent("audio_trigger","c_w_sword_flick",1000));
    // R95 log 18:10:37: release ~37.70, clip whoosh 38.025. request at release -> queued, script update takes it
    w::Request(4,w::Kind::Cut,37700);CK(w::Wanted());
    CK(takeName("c_w_sword_flick",t)&&t==37700&&!w::Wanted());
    CK(!w::MuteEvent("audio_trigger","c_w_sword_flick",37720)); // Lua reply not DISPATCHED yet: not muted
    w::state.dispatched(t);
    CK(w::MuteEvent("audio_trigger","c_w_sword_flick",38025));  // the clip's late whoosh is skipped
    CK(!w::MuteEvent("audio_trigger","c_w_sword_pommel_hit",38030)); // other events always run
    CK(!w::MuteEvent("audio_trigger","c_w_sword_flick",37700+w::kMuteMs+1)); // window ends
    {const auto before=w::state.passed.load();CK(!w::MuteEvent("audio_trigger","c_w_sword_flick",37700+w::kMuteMs+2));
     CK(w::state.passed.load()==before+1);CK(!w::MuteEvent("audio_trigger","c_arm_whoosh",37700+w::kMuteMs+3)&&w::state.passed.load()==before+1);} // R124: heard clip flicks are counted
    // a stale dispatch of an older swing does not arm the window of a newer one
    w::Request(4,w::Kind::Cut,40000);CK(takeName("c_w_sword_flick",t));
    w::Request(4,w::Kind::Cut,40300);w::state.dispatched(40000);
    CK(!w::MuteEvent("audio_trigger","c_w_sword_flick",40400));
    CK(takeName("c_w_sword_flick",t)&&t==40300);w::state.dispatched(t);
    CK(w::MuteEvent("audio_trigger","c_w_sword_flick",40500));
    // a kick right after: the window closes at once (kick clips are never muted by this)
    w::Cancel();CK(!w::MuteEvent("audio_trigger","c_w_sword_flick",40510));
    // stab with a longsword
    w::Request(4,w::Kind::Stab,50000);CK(takeName("c_w_sword_flick_stab",t));
    // learning: the game plays c_w_sword_short_flick for this class-1 weapon; a mace guess is replaced by it
    w::Request(1,w::Kind::Cut,60000);CK(takeName("c_w_sword_short_flick",t));
    (void)w::MuteEvent("audio_trigger","c_w_sword_flick",60400); // the game's clip says longsword flick for class 1
    w::Request(1,w::Kind::Cut,61000);CK(takeName("c_w_sword_flick",t));
    w::Request(1,w::Kind::Stab,62000);CK(takeName("c_w_sword_short_flick_stab",t)); // own stab name still from the table
    // another weapon class: the learned name of class 1 is not used
    w::Request(5,w::Kind::Cut,63000);CK(takeName("c_w_mace_flick",t));
    // unknown weapon class: nothing queued
    w::Request(0,w::Kind::Cut,64000);CK(!w::Wanted());
    // switch OFF: no request, no mute, tap not wanted; ON again works (no latch)
    w::Request(4,w::Kind::Cut,70000);CK(takeName("c_w_sword_flick",t));w::state.dispatched(t);
    w::enabled.store(false);
    CK(!w::WantsTap()&&!w::MuteEvent("audio_trigger","c_w_sword_flick",70300));
    w::Request(4,w::Kind::Cut,71000);CK(!w::Wanted());
    w::enabled.store(true);CK(w::WantsTap());
    w::Request(4,w::Kind::Cut,72000);CK(takeName("c_w_sword_flick",t));
    // names the packet may carry
    CK(w::validName("c_w_sword_short_flick_stab")&&!w::validName("c_w_sword_flick|x")&&!w::validName("f_he_bowl_put")&&!w::validName(nullptr));
    std::printf("R116 swing whoosh: %d checks PASS\n",checks);
    return 0;
}
