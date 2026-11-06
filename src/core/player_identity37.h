#pragma once
#include <cstdint>
// C37 identity ledger: the ONE place that decides which character/entity is "Henry" for the skinning
// correlation (g_last_player_pose) and the entity consumers (g_last_player_entity).
// Pure logic, no native calls. The DLL wraps it with a lock and feeds it from two native sources:
//  - Prime : CCryAction->ActionGame(+0xA40)->C_Player(exact vft)->CEntity(+0x38)->GetCharacter(0), pose cross-checked.
//            This is the authority for which character belongs to the local player entity.
//  - Solver: SetIK weapon-solver callback whose solver[0] is C_Player (RTTI) -> +0x38 CEntity; the pose's
//            character comes from the callback pose. It may only refresh what Prime qualified, and may only
//            (re)qualify on its own when no fresh Prime record exists.
// Generic SetIK callbacks of other characters (NPC CPoseData) are candidates: counted, never published.
namespace kcd2vr::player_identity37 {
enum class Source : int {None=0,Prime=1,Solver=2};
enum class Verdict : int {
    RejectedCandidate=0,   // callback not owned by C_Player: NPC or other character
    RejectedMismatch=1,    // C_Player owner, but entity/character disagrees with a fresh Prime record
    Refreshed=2,           // same qualified entity+character, lease renewed, no edge
    Qualified=3,           // first qualification or replacement (new generation, edge)
    Cleared=4,             // Prime proved the local player is absent (new generation, edge)
    Ignored=5              // invalid input / nothing to do
};
inline const char* name(Verdict v) noexcept {
    switch(v){case Verdict::RejectedCandidate:return "REJECTED_CANDIDATE";case Verdict::RejectedMismatch:return "REJECTED_MISMATCH";
    case Verdict::Refreshed:return "REFRESHED";case Verdict::Qualified:return "QUALIFIED";case Verdict::Cleared:return "CLEARED";default:return "IGNORED";}
}
inline const char* name(Source s) noexcept {return s==Source::Prime?"PRIME":(s==Source::Solver?"SOLVER":"NONE");}
inline constexpr std::uint64_t kLeaseMs=3000; // two missed 1 Hz primes; consumers fail closed after this
// Lock-free consumer form (the DLL mirrors qualified character + lease tick into two atomics).
inline bool accepts(std::uintptr_t qualified,std::uint64_t leaseTick,std::uintptr_t candidate,std::uint64_t now) noexcept {
    return qualified&&candidate==qualified&&leaseTick&&(now<leaseTick?leaseTick-now<=50:now-leaseTick<=kLeaseMs); /* a lease renewed by another thread just after 'now' was read is fresh (<=50 ms) */
}
struct Record {std::uintptr_t entity=0,character=0;std::uint64_t generation=0,tick=0;Source source=Source::None;bool valid=false;};
struct Ledger {
    Record current{};
    std::uint64_t generation=0,primeTick=0;
    std::uint64_t rejectedCandidates=0,rejectedMismatches=0,candidatesSinceEdge=0,mismatchesSinceEdge=0;
    std::uintptr_t lastCandidate=0;std::uint64_t candidateChanges=0; // bounded evidence: how often the generic candidate flipped
    bool primeFresh(std::uint64_t now) const noexcept {return primeTick&&now>=primeTick&&now-primeTick<=kLeaseMs;}
    bool fresh(std::uint64_t now) const noexcept {return current.valid&&current.tick&&now>=current.tick&&now-current.tick<=kLeaseMs;}
    // Consumer gate: the skinning character is Henry only if it is the qualified character and the lease is fresh.
    bool accepts(std::uintptr_t character,std::uint64_t now) const noexcept {return current.valid&&player_identity37::accepts(current.character,current.tick,character,now);}
    Verdict qualify(Source s,std::uintptr_t entity,std::uintptr_t character,std::uint64_t now) noexcept {
        const bool same=current.valid&&current.entity==entity&&current.character==character;
        current.tick=now;current.source=s;
        if(same)return Verdict::Refreshed;
        current.entity=entity;current.character=character;current.valid=true;current.generation=++generation;
        candidatesSinceEdge=0;mismatchesSinceEdge=0;
        return Verdict::Qualified;
    }
    Verdict prime(std::uintptr_t entity,std::uintptr_t character,std::uint64_t now) noexcept {
        if(!entity||!character||!now)return Verdict::Ignored;
        primeTick=now;return qualify(Source::Prime,entity,character,now);
    }
    // Prime saw a valid ActionGame whose local player module/entity is null (unload/load screen).
    Verdict primeAbsent(std::uint64_t now) noexcept {
        primeTick=0;
        if(!current.valid)return Verdict::Ignored;
        current={};current.generation=++generation;current.tick=now;
        candidatesSinceEdge=0;mismatchesSinceEdge=0;
        return Verdict::Cleared;
    }
    Verdict solver(bool exactOwner,std::uintptr_t entity,std::uintptr_t character,std::uint64_t now) noexcept {
        if(!exactOwner||!entity||!character){
            ++rejectedCandidates;++candidatesSinceEdge;
            if(character&&character!=lastCandidate){lastCandidate=character;++candidateChanges;}
            return Verdict::RejectedCandidate;
        }
        if(current.valid&&current.source==Source::Prime&&primeFresh(now)&&(entity!=current.entity||character!=current.character)){
            ++rejectedMismatches;++mismatchesSinceEdge;return Verdict::RejectedMismatch;
        }
        // Same pair refreshes. A replacement is accepted from the solver only when Prime is not currently
        // authoritative (prime stale/unavailable); otherwise the next 1 Hz prime publishes the replacement.
        if(current.valid&&current.source==Source::Prime&&primeFresh(now)){current.tick=now;return Verdict::Refreshed;}
        return qualify(Source::Solver,entity,character,now);
    }
};
}
