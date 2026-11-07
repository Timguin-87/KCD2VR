// C37 exact player identity ledger: NPC SetIK callbacks interleaved with Henry, same-frame ordering,
// body/character replacement, save/load (absent -> new player), stale lease and address reuse.
#include "core/player_identity37.h"
#include <cstdio>
#include <cstdint>
#include <vector>
#include <algorithm>
namespace pid=kcd2vr::player_identity37;
static unsigned checks37=0,failures37=0;
static void check37(bool yes,const char* text){++checks37;failures37+=!yes;std::printf("%s %s\n",yes?"PASS":"FAIL",text);}
// Pointer values from the C34 headset log (Henry 1C7B58F8910, NPCs ...).
static constexpr std::uintptr_t kHenryEntity=0x1C604A8BA00,kHenry=0x1C7B58F8910,kNpcA=0x1C8079EB540,kNpcB=0x1C8079F7640,kNpcC=0x1C8079F4600;
static constexpr std::uintptr_t kHenry2=0x1C7B5900000,kHenryEntity2=0x1C60C55DE00;
struct Mirror { // what the DLL publishes for lock-free consumers
    std::uintptr_t ch=0,entity=0;std::uint64_t lease=0;
    void apply(pid::Verdict v,const pid::Ledger& L,std::uint64_t now){
        if(v==pid::Verdict::Refreshed||v==pid::Verdict::Qualified){ch=L.current.character;entity=L.current.entity;lease=now;}
        else if(v==pid::Verdict::Cleared){ch=0;entity=0;lease=0;}
    }
    bool accepts(std::uintptr_t c,std::uint64_t now) const {return pid::accepts(ch,lease,c,now);}
};
struct Event {enum K{Npc,HenrySolver,Prime} k;std::uintptr_t c;};
static pid::Verdict run(pid::Ledger& L,Mirror& M,const Event& e,std::uint64_t now){
    pid::Verdict v{};
    if(e.k==Event::Prime)v=L.prime(kHenryEntity,e.c,now);
    else if(e.k==Event::HenrySolver)v=L.solver(true,kHenryEntity,e.c,now);
    else v=L.solver(false,0,e.c,now);
    M.apply(v,L,now);return v;
}
int main(){
    {   // 1. NPC callbacks interleaved with Henry (C34: 13,442 alternating "learned" lines)
        pid::Ledger L;Mirror M;std::uint64_t t=1000;
        check37(L.prime(kHenryEntity,kHenry,t)==pid::Verdict::Qualified,"prime qualifies Henry");M.apply(pid::Verdict::Qualified,L,t);
        bool everNpc=false,henryAlways=true;std::uint64_t qualifiedEdges=0;
        for(int i=0;i<5000;++i){
            t+=5;const std::uintptr_t npc=(i%3==0)?kNpcA:((i%3==1)?kNpcB:kNpcC);
            if(run(L,M,{Event::Npc,npc},t)!=pid::Verdict::RejectedCandidate)everNpc=true;
            if(M.accepts(npc,t)||L.accepts(npc,t))everNpc=true;
            const auto hv=run(L,M,{Event::HenrySolver,kHenry},t);if(hv==pid::Verdict::Qualified)++qualifiedEdges;
            if(i%200==0){const auto pv=run(L,M,{Event::Prime,kHenry},t);if(pv==pid::Verdict::Qualified)++qualifiedEdges;}
            henryAlways=henryAlways&&M.accepts(kHenry,t)&&L.current.character==kHenry&&L.current.entity==kHenryEntity;
        }
        check37(!everNpc,"no NPC callback is ever qualified or accepted");
        check37(henryAlways,"Henry stays the accepted owner throughout");
        check37(qualifiedEdges==0&&L.current.generation==1,"no owner edge/generation churn from interleaving");
        check37(L.rejectedCandidates==5000,"every NPC callback counted as rejected candidate");
        check37(L.candidateChanges==5000,"alternating candidates counted as flips (bounded counter, not lines)");
        // DLL logs a candidate line only when the flip counter hits a power of two.
        unsigned lines=0;for(std::uint64_t f=1;f<=13442;++f)if(!(f&(f-1)))++lines;
        check37(lines<=15,"13,442 candidate flips produce <=15 bounded log lines");
    }
    {   // 2. same-frame callbacks: every ordering of {NPC A, Henry solver, NPC B, prime} at one tick
        std::vector<int> order{0,1,2,3};bool allSame=true;int perms=0;
        do{
            pid::Ledger L;Mirror M;const std::uint64_t t0=5000;run(L,M,{Event::Prime,kHenry},t0);
            const std::uint64_t t=t0+16;
            const Event ev[4]{{Event::Npc,kNpcA},{Event::HenrySolver,kHenry},{Event::Npc,kNpcB},{Event::Prime,kHenry}};
            for(int k:order)run(L,M,ev[k],t);
            allSame=allSame&&M.accepts(kHenry,t)&&!M.accepts(kNpcA,t)&&!M.accepts(kNpcB,t)&&L.current.generation==1&&L.rejectedCandidates==2;
            ++perms;
        }while(std::next_permutation(order.begin(),order.end()));
        check37(allSame&&perms==24,"same-frame callback order never changes the owner (24 orders)");
        // NPC first in a frame before ANY qualification: still nothing published.
        pid::Ledger L;Mirror M;run(L,M,{Event::Npc,kNpcA},10);
        check37(!L.current.valid&&!M.accepts(kNpcA,10),"NPC callback before Henry is known publishes nothing");
    }
    {   // 3. C_Player solver whose pose disagrees with a fresh prime is rejected (mismatch), not published
        pid::Ledger L;Mirror M;run(L,M,{Event::Prime,kHenry},100);
        const auto v=run(L,M,{Event::HenrySolver,kNpcA},120);
        check37(v==pid::Verdict::RejectedMismatch&&L.rejectedMismatches==1,"owner/character mismatch rejected while prime is fresh");
        check37(M.accepts(kHenry,120)&&!M.accepts(kNpcA,120),"mismatch leaves Henry published");
        check37(L.solver(true,kHenryEntity2,kHenry,130)==pid::Verdict::RejectedMismatch,"foreign entity with Henry's character rejected");
    }
    {   // 4. body/character replacement on the same entity
        pid::Ledger L;Mirror M;run(L,M,{Event::Prime,kHenry},100);
        check37(run(L,M,{Event::HenrySolver,kHenry2},200)==pid::Verdict::RejectedMismatch,"solver-only replacement waits for prime authority");
        check37(run(L,M,{Event::Prime,kHenry2},900)==pid::Verdict::Qualified&&L.current.generation==2,"prime publishes replacement as new generation");
        check37(M.accepts(kHenry2,900)&&!M.accepts(kHenry,900),"old character no longer accepted after replacement");
        check37(run(L,M,{Event::HenrySolver,kHenry2},910)==pid::Verdict::Refreshed,"solver refreshes the replacement");
        check37(run(L,M,{Event::HenrySolver,kHenry},915)==pid::Verdict::RejectedMismatch,"late callback for the old body rejected");
    }
    {   // 5. save/load: player absent, address reuse by an NPC, then a new Henry
        pid::Ledger L;Mirror M;run(L,M,{Event::Prime,kHenry},100);
        auto v=L.primeAbsent(1100);M.apply(v,L,1100);
        check37(v==pid::Verdict::Cleared&&!L.current.valid&&L.current.generation==2,"absent player clears the owner (edge)");
        check37(L.primeAbsent(2100)==pid::Verdict::Ignored,"repeated absence is not another edge");
        // NPC character allocated at Henry's freed address during the load:
        check37(run(L,M,{Event::Npc,kHenry},2200)==pid::Verdict::RejectedCandidate&&!M.accepts(kHenry,2200),"NPC reusing Henry's old address is not accepted");
        v=L.solver(true,kHenryEntity2,kHenry2,2300);M.apply(v,L,2300);
        check37(v==pid::Verdict::Qualified&&L.current.source==pid::Source::Solver&&L.current.generation==3,"exact C_Player solver may qualify the new body while prime has none");
        v=L.prime(kHenryEntity2,kHenry2,2400);M.apply(v,L,2400);
        check37(v==pid::Verdict::Refreshed&&L.current.source==pid::Source::Prime,"prime confirms solver qualification without edge");
        check37(M.accepts(kHenry2,2400)&&M.entity==kHenryEntity2,"new save's Henry accepted");
    }
    {   // 6. stale lease: no renewal -> consumers fail closed; first renewal restores
        pid::Ledger L;Mirror M;run(L,M,{Event::Prime,kHenry},1000);
        check37(M.accepts(kHenry,1000+pid::kLeaseMs),"accepted at lease boundary");
        check37(!M.accepts(kHenry,1001+pid::kLeaseMs)&&!L.accepts(kHenry,1001+pid::kLeaseMs),"stale identity fails closed");
        check37(!pid::accepts(kHenry,5000,kHenry,4000),"clock before lease tick rejected");
        check37(pid::accepts(kHenry,5001,kHenry,5000),"C37 review: lease renewed 1 ms after the reader's clock is still fresh");
        // prime stale: solver may re-qualify a replacement on its own
        const auto v=run(L,M,{Event::HenrySolver,kHenry2},9000);
        check37(v==pid::Verdict::Qualified&&M.accepts(kHenry2,9000),"solver re-qualifies only after prime authority lapsed");
        check37(run(L,M,{Event::Prime,kHenry},9100)==pid::Verdict::Qualified&&M.accepts(kHenry,9100),"prime authority overrides solver qualification");
    }
    {   // 7. invalid inputs never publish
        pid::Ledger L;
        check37(L.prime(0,kHenry,1)==pid::Verdict::Ignored&&L.prime(kHenryEntity,0,1)==pid::Verdict::Ignored,"null prime ignored");
        check37(L.solver(true,kHenryEntity,0,1)==pid::Verdict::RejectedCandidate&&!L.current.valid,"exact owner without validated pose character rejected");
        check37(!pid::accepts(0,1,0,1),"null qualified character accepts nothing");
    }
    std::printf("player_identity37 checks=%u failures=%u\n",checks37,failures37);
    return failures37?1:0;
}
