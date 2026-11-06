#pragma once
#include "native_combat104.h"
namespace kcd2vr::native_admission147 {
// R44C1: an ordinary action/type still being present is not an input veto.
// D4AAC4/D4A99C arbitrate follow-up inputs through the native action selector.
// A held request belongs to the existing input owner; do not steal that edge.
// The caller separately excludes paired/native-body ownership and stale state.
inline bool preparationBlocked44c1(native_combat104::Observed s) noexcept {
    return !native_combat104::valid(s)||s.requested||
        !(s.type==-1||s.type==0||s.type==1||s.type==2)||
        !(s.phase==1||s.phase==2||s.phase==4||s.phase==8||s.phase==16||s.phase==128);
}
// C56: input readiness, not native acceptance or direction-write authority.
// An exact owned posture block may still be in the observed free-block phase
// after our input release. Let the original selector arbitrate the follow-up;
// foreign guards, held requests and all other protected phases remain busy.
inline bool preparationBlockedOwnedGuard56(native_combat104::Observed s,bool owned) noexcept {
    // Native block metadata is readable context, never a new direction or LT
    // authority. Caller supplies an exact CURRENT held owner for this tuple;
    // the historical released-guard witness cannot qualify it.
    if(native_combat104::blockMetadataH14(s))return !owned;
    if(!preparationBlocked44c1(s))return false;
    return !(owned&&native_combat104::valid(s)&&!s.requested&&
        (s.type==-1||s.type==1)&&s.phase==256);
}
// Called only inside the original retail predictor, never from an input ticket.
// A7CDE2 sets wrapper+10 to 1 immediately before A7CDE9 calls A7C624;
// A7CDF6 clears it on return. A7EBBC qualifies the action's +411/+2D0.
inline bool predictor(unsigned admitted,unsigned actionEnabled,unsigned actionMode,
                      unsigned hit,int type) noexcept {
    return admitted==1&&actionEnabled==1&&(actionMode==0||actionMode==2)&&
           hit==0&&(type==0||type==1);
}
// Direction writes are allowed only in observed neutral/guard state. A cleared
// request-candidate property is NOT evidence that a native action has finished.
inline bool guardWritable(native_combat104::Observed s) noexcept {
    return native_combat104::valid(s)&&s.type==-1&&!s.requested&&!s.requestCandidate&&
           (s.phase==1||s.phase==2||s.phase==128);
}
// R99 kick: R3 input readiness for the kick routes only (no direction write).
// R97 run 00:36:34-00:37:47: our own block-modifier press for the free kick
// puts Henry synchronously into the free block phase 256 with zone -1
// (V66.104 row: phase=256 type=-1 zone=-1 hand=1 flags=0, H15 TRIGGER
// phase=256 on the same tick). guardWritable() has no 256 and
// blockMetadataH14() wants zone 7, so every R3 died as INPUT_NOT_WRITABLE.
// Neutral, guard and free-block phases are ready; any attack/recovery phase,
// a held request or an attack candidate stays busy. The game still decides
// the kick through FreeGuard 23 and its own selector.
inline bool kickInputReadyR99(native_combat104::Observed s) noexcept {
    return (s.type==-1||s.type==1)&&!s.requested&&!s.requestCandidate&&
           (s.phase==1||s.phase==2||s.phase==128||s.phase==256)&&
           s.input>=-1&&s.input<256&&s.zone>=-1&&s.zone<=7&&s.hand>=0&&s.hand<=2;
}
// R40C1: requesting/holding the proven native block INPUT is not a stance write.
// During ordinary attack/recovery the game may reject or defer it. Do not drop
// an owned held button merely because a request/phase has advanced. Protected
// body, tracking, owner and equipment gates remain in the caller; other action
// types and unclassified phases remain excluded.
inline bool guardInput(native_combat104::Observed s) noexcept {
    return native_combat104::valid(s)&&(s.type==-1||s.type==0||s.type==1)&&
        (s.phase==1||s.phase==2||s.phase==4||s.phase==8||s.phase==16||s.phase==128);
}
// R44C2: the 15:07 R44C1 capture shows our press entering phase 256,
// combat_free_blk_*_over, then our whitelist releasing it one frame later.
// Retain ONLY an already-owned held input in this observed free-block state.
// This grants no new press, direction write, accepted block or timing flag.
// R59.16C7E-DECAP1 (13 Oct log): with no locked opponent the game runs our press as the BLOCK ACTION (type 1) in
// phase 256 ("combat_free_blk"). type==-1 alone rejected it, so the whitelist released one frame after every press:
// 40 presses/2 s, Henry's inCombat flag flickering 0/1 at 40 Hz, native phase 128/256 live whenever a swing began
// (83+42 YIELDs), the two-handed block weapon flicker. A held input stays held while the block action runs.
inline bool freeGuardHeld44c2(native_combat104::Observed s,bool owned) noexcept {
    return owned&&native_combat104::valid(s)&&(s.type==-1||s.type==1)&&s.phase==256;
}
inline bool guardHeldInput44c2(native_combat104::Observed s,bool owned) noexcept {
    return guardInput(s)||freeGuardHeld44c2(s,owned);
}
}
