#pragma once
#include <cstdint>

namespace kcd2vr::diagnostics::interaction_receipt59c10 {
// Per-producer accounting. The caller checks the finite F5 lease BEFORE doing
// any diagnostic work. Coalesce rapid target/reason churn, but retain its count.
// No allocations, native reads, locks or work queued after the recorder ends.
struct Key {
    std::uintptr_t object=0;
    std::uint32_t target=0;
    int side=-1,reason=0;
    bool operator==(const Key& b) const noexcept {
        return object==b.object&&target==b.target&&side==b.side&&reason==b.reason;
    }
};
struct Gate {
    std::uint64_t session=0,last=0,seen=0,changes=0,coalesced=0;
    unsigned lines=0,overBudgetLines=0;
    bool dirty=false,exhausted=false;
    static constexpr std::uint64_t kOverBudgetMs=10000;
    Key previous{};
    bool observe(std::uint64_t nextSession,std::uint64_t now,Key key,
                 std::uint64_t spacing=100,unsigned budget=256,std::uint64_t heartbeatMs=1000) noexcept {
        if(!nextSession||!now)return false;
        if(session!=nextSession){*this={};session=nextSession;}
        const bool changed=!seen||!(previous==key);
        ++seen;if(changed){++changes;dirty=true;}previous=key;
        // W4 (Codex: G2 went silent after 160 lines, later actions never logged): past the budget a
        // receipt keeps ONE summary line per 10 s instead of going silent. exhausted tells the caller.
        if(lines>=budget){
            exhausted=true;
            if(last&&now>=last&&now-last>=kOverBudgetMs){last=now;++overBudgetLines;dirty=false;return true;}
            ++coalesced;return false;
        }
        const bool elapsed=!last||now<last||now-last>=spacing;
        const bool heartbeat=last&&now>=last&&now-last>=heartbeatMs;
        if(!last||(elapsed&&(dirty||heartbeat))){last=now;++lines;dirty=false;return true;}
        ++coalesced;return false;
    }
};
inline std::uint64_t age(std::uint64_t now,std::uint64_t tick) noexcept {
    return tick&&now>=tick?now-tick:UINT64_MAX;
}
// Diagnostic state only. Input attempts are squeeze edges, never accepted actions.
// A new recorder/station lifetime discards history; no session is renewed by sampling.
struct CraftGate50 {
 std::uint64_t session=0,life=0,last=0,serial=0,attempt[2]{};
 std::uintptr_t instance=0;bool held[2]{};unsigned signature=~0u;
 bool observe(std::uint64_t next,std::uint64_t now,std::uintptr_t owner,std::uint64_t lifetime,
              float right,float left,unsigned bits) noexcept {
  if(!next||!now)return false;
  if(next!=session||owner!=instance||lifetime!=life){*this={};session=next;instance=owner;life=lifetime;signature=~0u;}
  const float grip[2]{right,left};bool edge=false;
  for(int h=0;h<2;++h){const bool down=grip[h]>=(held[h]?.35f:.55f);
   if(down&&!held[h]){attempt[h]=++serial;edge=true;}if(down!=held[h])edge=true;held[h]=down;}
  const bool changed=bits!=signature;signature=bits;
  if(!last||now<last||edge||(changed&&now-last>=100)||now-last>=500){last=now;return true;}
  return false;
 }
};

}
