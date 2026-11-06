#pragma once
#include "../../vrik/contact_handoff59c3.h"
#include "../../vrik/hybrid_serial59.h"
#include "../../vrik/swing_effort59c14.h"
namespace kcd2vr::hybrid54 {
// Routing only. A Physical result never grants an attempt, contact, native
// phase, damage or stamina: the caller must claim the SAME physical ledger and
// then use the verified native builder/continuation. Freshness stays160ms.
enum class Rendezvous {Wait,Bound,Physical,Reject};
inline Rendezvous rendezvous(contact59c3::Join join,bool synchronized,bool fallback,
        std::uint64_t impact,std::uint64_t now,unsigned graceMs) noexcept {
    if(join==contact59c3::Join::Bound)return Rendezvous::Bound;
    if(join!=contact59c3::Join::Wait)return Rendezvous::Reject;
    if(!contact176::fresh(impact,now,contact59c3::maximumContactAge))return Rendezvous::Reject;
    if(!synchronized||!fallback||now-impact<graceMs)return Rendezvous::Wait;
    return Rendezvous::Physical;
}
inline bool ordinaryPhase(int phase,bool exactQuietGuard=false) noexcept {
    return phase==0||phase==1||phase==2||phase==4||phase==8||phase==16||phase==32||
        (exactQuietGuard&&(phase==128||phase==256));
}
enum class CostProof54 {Ready,Wait,Refuse};
inline CostProof54 costProof54(const swing_effort59c14::Ledger& ledger,const motion59c4::Key& key,
        std::uint64_t impact,std::uint64_t now,std::uint64_t currentRecovery) noexcept {
    if(!motion59c4::valid(key)||key.recovery!=currentRecovery||
       !contact176::fresh(impact,now,contact59c3::maximumContactAge))return CostProof54::Refuse;
    const auto proof=ledger.contactProof54(key,now); // one atomic ledger snapshot
    if(proof==swing_effort59c14::ContactProof54::Complete)return CostProof54::Ready;
    return proof==swing_effort59c14::ContactProof54::NativePending?CostProof54::Wait:CostProof54::Refuse;
}
}
