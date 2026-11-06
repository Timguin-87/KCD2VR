#pragma once
#include <array>
#include <cstdint>
#include <cstdio>
namespace kcd2vr::master_eligibility_h21 {
enum class Term : unsigned { Ability,OpponentPhase,MutualPair,DodgeExclusion,OwnPhase,Zone,Context,OwnerActionExclusion,Candidates,Count };
enum class Result : unsigned char { NotReached,Unknown,Pass,Fail };
inline const char* name(Result r) noexcept {return r==Result::Pass?"PASS":r==Result::Fail?"FAIL":r==Result::Unknown?"UNKNOWN":"NOT_REACHED";}
inline const char* name(Term t) noexcept {
    constexpr const char* names[]{"ABILITY_WEAPON","OPPONENT_PHASE","MUTUAL_PAIR","HENRY_DODGE_EXCLUSION","HENRY_CANDIDATE_PHASE","STANCE_ZONE","NATIVE_CONTEXT","OWNER_ACTION_EXCLUSION","NATIVE_CANDIDATE_ROWS"};
    return unsigned(t)<unsigned(Term::Count)?names[unsigned(t)]:"UNKNOWN";
}
struct Attempt {
    std::array<Result,unsigned(Term::Count)> result{};
    std::uintptr_t trigger=0,actor=0,state=0,opponent=0,caller=0;
    std::uint64_t serial=0,tick=0;
    int countBefore=-1,countAfter=-1,stance=-1,offered=-1;
    bool native=false;
    void observed(Term t,bool pass) noexcept {result[unsigned(t)]=pass?Result::Pass:Result::Fail;}
    void pending(Term t) noexcept {result[unsigned(t)]=Result::Unknown;}
    void complete(bool value) noexcept {
        native=value;
        // The last native call's normalized AL is the function result, but only
        // after every preceding observed term passed. Never infer skipped terms.
        bool prefix=true;for(unsigned i=0;i<unsigned(Term::Candidates);++i)prefix=prefix&&result[i]==Result::Pass;
        if(prefix)observed(Term::Candidates,value);
        // A later observed node proves its prefix was reached; an absent
        // observation there is UNKNOWN, never a fabricated skip or PASS.
        unsigned reached=0;bool any=false;
        for(unsigned i=0;i<result.size();++i)if(result[i]!=Result::NotReached){reached=i;any=true;}
        if(value){reached=unsigned(result.size()-1);any=true;}
        if(any)for(unsigned i=0;i<=reached;++i)if(result[i]==Result::NotReached)result[i]=Result::Unknown;
    }
    const char* first() const noexcept {
        for(unsigned i=0;i<result.size();++i){if(result[i]==Result::Fail)return name(Term(i));
            if(result[i]!=Result::Pass)return result[i]==Result::Unknown?"UNKNOWN_OBSERVATION":"NOT_REACHED";}
        return native?"NONE":"UNKNOWN_OBSERVATION";
    }
};
inline bool sameVerdict(const Attempt& a,const Attempt& b) noexcept {
    return a.trigger==b.trigger&&a.actor==b.actor&&a.opponent==b.opponent&&a.caller==b.caller&&
        a.result==b.result&&a.native==b.native&&a.countBefore==b.countBefore&&a.countAfter==b.countAfter;
}
inline int format(const Attempt& a,char* out,std::size_t cap) noexcept {
    return std::snprintf(out,cap,
        "H21 MS_ELIGIBILITY attempt=%llu trigger=%llX actor=%llX opponent=%llX caller=+%llX source=%s native=%d first=%s "
        "ability=%s oppPhase=%s mutual=%s dodge=%s ownPhase=%s zone=%s context=%s ownerAction=%s candidates=%s "
        "stance=%d offered=%d ms7D8=%d/%d evidence=NATIVE_EXECUTED_CHECKS skipped=NOT_REACHED writes=NONE",
        (unsigned long long)a.serial,(unsigned long long)a.trigger,(unsigned long long)a.actor,
        (unsigned long long)a.opponent,(unsigned long long)a.caller,a.caller==0x909C6D?"AUTHORED_EVENT":"NATIVE_ELIGIBILITY",
        a.native?1:0,a.first(),name(a.result[0]),name(a.result[1]),name(a.result[2]),name(a.result[3]),
        name(a.result[4]),name(a.result[5]),name(a.result[6]),name(a.result[7]),name(a.result[8]),
        a.stance,a.offered,a.countBefore,a.countAfter);
}
}
