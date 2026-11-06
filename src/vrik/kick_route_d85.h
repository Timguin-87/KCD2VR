#pragma once
// D85 kick: route choice, 2 s cooldown and the one visible reason line for R3.
// Pure policy; every native read/call stays in native_bridges168.inl.
//
// Static facts (WHGame 0x6A350E20, Tables.pak, companion combat_action_attack.xml):
//  - 687BBC builds the attack query: actor vtable 0x5E0 (1315CB4) true ->
//    CombatAttack (action 3), false -> FreeAttack (26). Without an opponent
//    (state+CF8 null) 1315CB4 returns only the +450 object's verdict.
//  - FreeAttack copies the REQUESTED attack type from 12EE05C: the held weapon
//    class PRIMARY type, or its SECONDARY type while the block modifier getter
//    1A7DDF0 (state+64D) is non-zero. weapon_class.xml: unarmed 5 punch / 4 kick.
//  - CombatAttack opponent-class filtering is conditional on a present
//    opponent. Without one, the actor's actual query-family verdict matters;
//    the presence of an actor+450 object or combat-looking guard is not enough.
//  - The old R19 route releases the player's guard first (modifier 0), so
//    without an opponent it can only produce the free PUNCH (D80 22:01:54.142
//    combat_fatksw_nw_start_player_over, type=5). The D85 selector review
//    described that older selector regression; D86 separately replays the current query verdict.
#include <atomic>
#include <cstdint>
#include "native_bridges168.h"
namespace kcd2vr::kick_route_d85 {
enum class Route : int {Combat=0,Free=1};
// NativeFreeKickObjectD86 first recognizes the exact standard retail verdict:
// actor vf5E0=1315CB4 -> vf580=1315CF8 -> +450/vf18=9D3514. With no opponent,
// that object returns false for every guard, so the native query is FreeAttack.
// The pinned functions are replayed independently by test_native_kick_query_d86.py.
// This older guard policy remains only for object kinds not established by those
// pins. No object is certainly free; a known FreeGuard/no combat guard keeps the
// existing free route; an unknown or combat guard keeps its prior combat route.
constexpr bool combatGuard(int guard) noexcept {return guard==0||guard==1||guard==2;}
constexpr bool freeContext(bool opponent,bool combatObject,bool guardKnown,int guard) noexcept {
    return !opponent&&(!combatObject||(guardKnown&&!combatGuard(guard)));
}
// opponent: native state+CF8 is set. free: freeContext() plus transport3 and
// a current loadout snapshot (NativeFreeKickContextR24).
constexpr Route choose(bool opponent,bool free) noexcept {
    return free?Route::Free:Route::Combat;
}
inline const char* name(Route r) noexcept {
    return r==Route::Free?"FREE_GUARD":"COMBAT_DOWN5";
}
// One kick per 2 s only after a returned R3 release also observes native
// attack type 4. A returned OnAction handler alone does not mean the game
// selected a kick (D85R5 F3 captures observed type 0); it earns no cooldown.
inline constexpr std::uint64_t kCooldownMs=2000;
struct Cooldown {
    std::atomic<std::uint64_t> last{0};
    bool ready(std::uint64_t now) const noexcept {
        const auto t=last.load(std::memory_order_acquire);
        return !t||now<t||now-t>=kCooldownMs; // clock moved backwards: admit
    }
    std::uint64_t remaining(std::uint64_t now) const noexcept {
        const auto t=last.load(std::memory_order_acquire);
        return ready(now)?0:kCooldownMs-(now-t);
    }
    void dispatched(std::uint64_t now) noexcept {if(now)last.store(now,std::memory_order_release);}
    void reset() noexcept {last.store(0,std::memory_order_release);}
};
inline Cooldown cooldown;
// Terminal R3 outcome -> cooldown start and the short line the player sees
// (visible_failure keeps it to one line per 2 s). nullptr = no new line:
// gates already explained at their own site (sayCLE, loadout) or a wait edge.
inline const char* reasonText(unsigned reason) noexcept {
    using G=kcd2vr::bridges168::KickGateH31;
    switch(G(reason)){
    case G::CooldownD85:return "kick: wait 2 s between kicks";
    case G::FreeGuardExpiredD84:return "kick: guard did not come up in time";
    case G::GuardWaitOwner:case G::AfterCancelOwner:case G::ZoneOwner:return "kick: cancelled, guard or weapon changed";
    case G::PressFailed:case G::ReleaseFailed:case G::BeginNotObserved:case G::ReleaseNotObserved:return "kick: refused by the game";
    case G::GuardAckUnknown:return "kick: guard state unreadable";
    default:return nullptr;
    }
}
struct Terminal {bool startCooldown=false;const char* text=nullptr;};
inline Terminal onTerminal(unsigned reason,bool releaseReturned,bool nativeKickObserved=false) noexcept {
    using G=kcd2vr::bridges168::KickGateH31;
    Terminal t{};
    if(G(reason)==G::FreeGuardPendingD84)return t; // wait edge, not an outcome
    t.startCooldown=reason==0&&releaseReturned&&nativeKickObserved;
    if(!t.startCooldown)t.text=reasonText(reason);
    return t;
}
}
