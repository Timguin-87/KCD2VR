#pragma once
// C59a P1 (rung 1 = HYBRID): bounded wait for the struck actor to appear in the observed actor map, instead of a terminal
// TARGET_UNKNOWN on the first look. Pure decision logic, no native reads and no writes of any kind.
// What it may NOT do (hard rules): switch to another body, write a target, lengthen the 160 ms contact lease, resolve a
// contact whose own collider is unknown (then the retained body can never be proven, so it stays terminal like before).
// The caller (geometry_hit178r27.inl Drain) feeds it facts it already reads; DIRECT keeps AwaitDirectTarget19c3 unchanged.
#include <cstdint>
namespace kcd2vr::hybrid_wait58 {
enum class Decision : unsigned char { Ready, Wait, Terminal };
enum class Why : unsigned char { Ready, Waiting, Disabled, NoEntity, NotOwned, NoCollider, BudgetSpent, IdentityInvalid, BodyNotRetained };
// Backstop only: Drain runs about once per Henry combat Update (about 15 times in 160 ms). The 160 ms lease is the real bound.
inline constexpr unsigned kMaxWaits=32;
struct Input {
    bool enabled=true;          // INI VrHybridTargetWait58
    bool entityValid=false;     // contact carries a struck entity id
    bool contactOwned=false;    // OriginalContactOwned19c3: same recovery epoch, inside the 160 ms lease, owner key still current
    bool targetFound=false;     // map (or the current opponent with the SAME entity id) gave an actor
    bool identityOk=false;      // that actor is not Henry and its own entity id equals the contact entity id
    bool colliderKnown=false;   // the contact recorded the collider it touched (needed to prove the same body later)
    bool alreadyWaited=false;   // this contact already waited at least once
    bool bodyRetained=false;    // RetainedContactBody19c3: proxy owner/body still equals the contact's own collider
    unsigned waits=0;           // waits already spent by this contact
};
struct Result { Decision decision=Decision::Terminal; Why why=Why::Disabled; };
inline Result decide(const Input& in) noexcept {
    if(in.targetFound){
        if(!in.identityOk)return {Decision::Terminal,Why::IdentityInvalid};             // a different actor is never accepted
        if(in.alreadyWaited&&!in.bodyRetained)return {Decision::Terminal,Why::BodyNotRetained}; // waited contacts must still hit the same body
        return {Decision::Ready,Why::Ready};
    }
    if(!in.enabled)return {Decision::Terminal,Why::Disabled};   // exactly the C59 behaviour
    if(!in.entityValid)return {Decision::Terminal,Why::NoEntity};
    if(!in.contactOwned)return {Decision::Terminal,Why::NotOwned};
    if(!in.colliderKnown)return {Decision::Terminal,Why::NoCollider};
    if(in.waits>=kMaxWaits)return {Decision::Terminal,Why::BudgetSpent};
    return {Decision::Wait,Why::Waiting};
}
inline const char* name(Why w) noexcept {
    switch(w){case Why::Ready:return "READY";case Why::Waiting:return "WAITING";case Why::Disabled:return "WAIT_DISABLED";case Why::NoEntity:return "NO_ENTITY";
    case Why::NotOwned:return "CONTACT_NOT_OWNED_OR_EXPIRED";case Why::NoCollider:return "NO_CONTACT_COLLIDER";case Why::BudgetSpent:return "WAIT_BUDGET_SPENT";
    case Why::IdentityInvalid:return "TARGET_IDENTITY_INVALID";case Why::BodyNotRetained:return "BODY_NOT_RETAINED";}
    return "?";
}
} // namespace kcd2vr::hybrid_wait58
