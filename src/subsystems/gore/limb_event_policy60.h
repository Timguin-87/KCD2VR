#pragma once
// C60 GORE DISMEMBER event policy: the decision layer between a qualified
// corpse limb-cut candidate (the C60 read-only limb receipts: corpse + limb
// material + blade speed + dedupe) and the gorepak50 limb event. Input: the
// receipt's CutCandidate, the receipt's blade speed (m/s) and the caller's
// joint classification witness. Output: the EVENT REQUEST - the gorepak50
// limb kind plus the struck limb (part) id - or one named refusal.
// FAIL CLOSED: every existing receipt is re-verified here; a LIVING actor is
// never dismembered (the corpse flag AND a native death proof - the lethal
// receipt itself or the dead-by-game list with the same entity identity -
// are both required). Only the exact limb kinds (hand/forearm/foot/calf)
// fire; anything unknown, non-human or unresolved is refused.
// Pure logic (host tested). NOT WIRED: there is no proven production
// callsite yet - the rawPart->skeleton-joint mapping stays UNPROVEN and the
// visual route does not exist, so nothing sends an event.
#include "gore_policy.h"          // CutCandidate, LimbCutLedger60, ReceiptBudget60, LimbFromMaterial60, KnownDead, limbCorpseMinSpeed
#include "../../vrik/gorepak50.h" // Cut kind + the VrDismemberment event switch (DS-F8 comment; gore_policy.h already includes it)

namespace kcd2vr::gore32 {
// Why no event request was produced. Named refusals only: the caller logs the
// name and never fabricates a fallback (no "guess a limb" path exists).
enum class LimbEventRefusal60 : int {
    None=0,      // the request is valid
    BadRequest,  // structural receipt failure: no now/session/target/identity/struck part, or a non-finite speed
    SwitchOff,   // gorepak50::dismember off (VrDismemberment): no limb events at all
    Living,      // not a proven native-dead corpse: corpse flag off / no lethal receipt / not dead-by-game - a living actor is never dismembered
    NoMaterial,  // LimbFromMaterial60 did not resolve the struck material to a limb kind (unknown/0/-1/non-human fail closed)
    Mismatch,    // the caller's joint witness resolved a DIFFERENT limb than the material kind: two receipts disagree, nothing fires
    NoSpeed,     // blade speed below VrLimbCorpseMinSpeed
    Dedupe,      // LimbCutLedger60: this cut kind already fired on this entity within 2 s
    Budget,      // ReceiptBudget60: this F3 capture session is exhausted
};
inline const char* name(LimbEventRefusal60 r) noexcept {
    constexpr const char* n[]{"NONE","BAD_REQUEST","SWITCH_OFF","LIVING","NO_MATERIAL","MISMATCH","NO_SPEED","DEDUPE","BUDGET"};
    return n[unsigned(r)<sizeof(n)/sizeof(n[0])?unsigned(r):0];
}
// The event request: the gorepak50 limb kind + the struck limb (part) id.
struct LimbEventRequest60 {
    kcd2vr::gorepak50::Cut kind=kcd2vr::gorepak50::Cut::None;
    int limbId=-1;
};
struct LimbEventDecision60 {
    LimbEventRequest60 request{};
    LimbEventRefusal60 refusal=LimbEventRefusal60::None;
    bool fire() const noexcept {
        return refusal==LimbEventRefusal60::None&&request.kind!=kcd2vr::gorepak50::Cut::None&&request.limbId>=0;
    }
};
// The pure decision. bladeSpeed is the receipt's blade speed in m/s (the
// impactRecord speed; CutCandidate.power is a normalized energy, NOT a speed).
// jointKind is the caller's joint classification witness (classifyLimb /
// LimbCutReceipt60); Cut::None = unresolved, the material kind stands alone.
// Checks, in order (the budget is only consumed by a complete pass, so a
// refused candidate never costs a later candidate or a later session):
//  1. BadRequest: !now || !session || !c.target || !c.identity || struck part < 0 || non-finite bladeSpeed
//  2. SwitchOff:  gorepak50::dismember off
//  3. Living:     !c.corpse || (!c.nativeLethal42 && !KnownDead(c.target,c.identity,now))
//  4. NoMaterial: LimbFromMaterial60(c.bodyMaterial) == Cut::None
//  5. Mismatch:   jointKind resolved and != the material kind
//  6. NoSpeed:    bladeSpeed < limbCorpseMinSpeed
//  7. Dedupe:     !ledger.available(...) - inspection only: the ledger is
//                 admitted by the CALLER after the actual event queue
//                 succeeded (mirrors StartLimb50: failed pins, missing
//                 attachments or a Lua-queue refusal may retry immediately)
//  8. Budget:     !budget.admit(session) - consumed here: producing the
//                 request IS the receipt admission.
inline LimbEventDecision60 limbEvent60(const CutCandidate& c,double bladeSpeed,
    kcd2vr::gorepak50::Cut jointKind,std::uint64_t now,std::uint64_t session,
    LimbCutLedger60& ledger,ReceiptBudget60& budget) noexcept {
    LimbEventDecision60 d{};
    const int limbId=c.rawPart47>=0?c.rawPart47:c.part44; // the struck part id (the same choice as the native receipt lane)
    if(!now||!session||!c.target||!c.identity||limbId<0||!std::isfinite(bladeSpeed)){d.refusal=LimbEventRefusal60::BadRequest;return d;}
    if(!kcd2vr::gorepak50::dismember.load(std::memory_order_relaxed)){d.refusal=LimbEventRefusal60::SwitchOff;return d;}
    if(!c.corpse||(!c.nativeLethal42&&!KnownDead(c.target,c.identity,now))){d.refusal=LimbEventRefusal60::Living;return d;}
    const auto kind=LimbFromMaterial60(c.bodyMaterial);
    if(kind==kcd2vr::gorepak50::Cut::None){d.refusal=LimbEventRefusal60::NoMaterial;return d;}
    if(jointKind!=kcd2vr::gorepak50::Cut::None&&jointKind!=kind){d.refusal=LimbEventRefusal60::Mismatch;return d;}
    if(bladeSpeed<limbCorpseMinSpeed.load(std::memory_order_relaxed)){d.refusal=LimbEventRefusal60::NoSpeed;return d;}
    if(!ledger.available(c.target,c.identity,now,int(kind))){d.refusal=LimbEventRefusal60::Dedupe;return d;}
    if(!budget.admit(session)){d.refusal=LimbEventRefusal60::Budget;return d;}
    d.request.kind=kind;d.request.limbId=limbId;return d;
}
} // namespace kcd2vr::gore32
