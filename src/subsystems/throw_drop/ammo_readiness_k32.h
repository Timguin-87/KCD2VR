#pragma once
// K32 (Claude, R8b, from Codex 25 Oct knife review): two small rules of the left-chest knife service.
//  1 readiness: the ammo mask census (ObserveAmmoMasksT20) ran ONCE per load episode and a failed scan was spent
//    for the whole episode, also when it only failed because the game was not ready yet (not-ready, load bit,
//    script context changed, registry changed/empty while loading). now a transient failure may retry, bounded:
//    on a real lifecycle state change, or after 1 s, at most 4 times per episode. a permanent failure (fault latch,
//    retail pins, table too large/unknown class/bit0) stays spent. K39 separately waits for the native pickup
//    owner to clear its recoverable fault; only then one new census is allowed. A ready scan never rescans in its episode.
//  2 pouch: a squeeze at the empty pouch was silent. now one short line says why.
#include <cstdint>
namespace kcd2vr::ammo_readiness_k32 {
enum class Scan:unsigned char {Ready,Transient,Permanent,OwnerFault};
inline constexpr unsigned kMaxRetries=4;
inline constexpr std::uint64_t kRetryMs=1000;
struct Retry {std::uint64_t episode=~std::uint64_t(0),failTick=0,lifecycle=0;unsigned retries=0;bool transient=false,ownerFault=false;};
// may the existing script client run the census now?
inline bool wanted(const Retry& r,std::uint64_t episode,std::uint64_t now,std::uint64_t lifecycle,bool nativeFault=true) noexcept {
    if(r.episode!=episode)return true;
    // K39: the ordinary native pickup owner can positively settle quarantine
    // and clear its fault. That edge earns ONE new census, never forced READY.
    if(r.ownerFault)return !nativeFault;
    if(!r.transient)return false;
    // R8b (Codex): a REAL lifecycle transition always earns a census, also after the timed retries ran out.
    if(lifecycle!=r.lifecycle)return true;
    return r.retries<kMaxRetries&&now>=r.failTick&&now-r.failTick>=kRetryMs;
}
// result of one census in this episode.
inline Retry record(const Retry& r,std::uint64_t episode,std::uint64_t now,std::uint64_t lifecycle,Scan s) noexcept {
    Retry n{};n.episode=episode;n.failTick=now;n.lifecycle=lifecycle;n.transient=s==Scan::Transient;n.ownerFault=s==Scan::OwnerFault;
    // a lifecycle transition resets the timed retry budget (the bound only limits retries inside one stable state).
    n.retries=s==Scan::Transient?((r.episode==episode&&r.transient&&r.lifecycle==lifecycle)?r.retries+1:1):0;return n;
}
// hand claim of a knife record in cleanup: free the hand as soon as the native knife is retired (thrown flight is
// its own record list and never set a hand bit), never for a quarantined (world changed) or still held record.
inline bool release_hand_early(bool nativeRetired,bool quarantined,bool held) noexcept {return nativeRetired&&!quarantined&&!held;}
// R114 (Miki 6 Nov: "I lost the control for my left hand"): R113 log 00:41:02.330 left pouch knife draw serial 1000122
// died at PREVIEW_HIDE_UNTIL_FIRST_PLACEMENT (reason=MODEL), its cleanup never finished ("previous knife still being
// retired" 00:42:49 / 00:42:59) and the left hand claim stayed up to the end of the log (86x H29 FIST_BLOCK
// why=COMPETING_HAND_CLAIM_T24, 1847/1847 left F7 HAND_OWNER rows fingerOwner=UNREACHED, HOLSTER_ATTEMPT side=L
// claim=hand_claim_t24). a WORLD route knife (left chest/pouch knife) never enters Henry's attachment table, so when
// the attach leaf was not entered there is nothing native in the hand: give the hand back at once, also while the
// native preview cleanup still retries, also for a quarantined (world changed) record. the record itself stays
// current until its cleanup is done, so a second knife can't start on top of it.
inline bool release_world_hand_r114(bool worldRoute,bool attachEntered) noexcept {return worldRoute&&!attachEntered;}
// R114: a knife record whose cleanup is not finished 2 s after it started (5 s when the native attach leaf was
// entered = Henry model slot) is force-retired, so the pouch can never stay "previous knife still being retired".
inline bool force_retire_due_r114(std::uint64_t since,std::uint64_t now,bool attachEntered) noexcept {
    return since&&now>=since&&now-since>(attachEntered?5000u:2000u);
}
// the pouch line for a NEW squeeze edge of an empty left hand inside the pouch. nullptr = the draw handles it.
// R9 lane A: the left-chest refresh never got READY in R8d2 (and H50): preflight 111 = selected, pins, source and
// daggers=5 but reservationReady=0, so every squeeze said "knife data not ready" with no reason. the line now names
// what the game script answered to the last refresh. status nullptr = no refresh reply yet.
enum class Refresh:unsigned char {NoInput,CallFailed,Status,Rejected};
// R11: the refresh only asks the game script for Henry's knife counts - it needs the current OWNER, not an empty hand.
// the old gate needed a free, tracked, unclaimed hand at the 1 s refresh instant (AmmoInputT18), so a sword in the right
// hand + a left hand that was busy/untracked/claimed at that instant left the pouch "not ready" for good. the hand that
// throws is still checked at the draw. ask with a free hand if one is there, else the left (chest pouch) hand.
inline int refreshSide(bool ownerOk,bool rightFree,bool leftFree) noexcept {return !ownerOk?-1:rightFree?0:leftFree?1:1;}
inline const char* refreshText(Refresh r,const char* status) noexcept {
    if(r==Refresh::NoInput)return "knives: Henry not ready for the knife check yet";
    if(r==Refresh::CallFailed||!status||!*status)return "knives: game script did not answer (knife script not loaded?)";
    if(status[0]=='W')return "knives: game script waits for Henry (WAIT)";
    if(status[0]=='B'&&status[1]=='U')return "knives: a knife is still reserved (BUSY)";
    if(status[0]=='R'&&r==Refresh::Rejected)return "knives: game script ready, but Henry changed during the check";
    if(status[0]=='R')return nullptr; // READY
    return "knives: game script refused the check";
}
inline const char* pouchSay(bool edge,bool insidePouch,bool handEmpty,int daggers,bool selected,bool dataReady) noexcept {
    if(!edge||!insidePouch||!handEmpty)return nullptr;
    if(!selected||!dataReady)return "knives: knife data not ready"; // the dagger count is only fresh when ready
    if(daggers<=0)return "knives: none in inventory - Immersion: REFILL KNIVES"; // R90 (R88 interact)
    return nullptr;
}
// Readiness of native code is separate from an observed finite stock count.
// Zero is meaningful only after a current accepted inventory refresh.
inline const char* stockStateD86(int daggers,bool currentReply) noexcept {
    return !currentReply||daggers<0?"UNCONFIRMED":daggers>0?"AVAILABLE":"NO_SUPPORTED_STOCK";
}
}
