// C25: pure receipts ledger for the VrCombatNoStep alignment veto.
#include "vrik/align_veto_receipts24.h"
#include <cassert>
#include <cstdio>
#include <cstring>
using namespace kcd2vr::align24;
static Call make(std::uint64_t key,std::uint64_t action,std::uint64_t tick,bool original,bool atVeto,bool vetoed,std::uint8_t why=None,int phase=8){
    Call c{};c.key=key;c.action=action;c.tick=tick;c.original=original;c.atVetoCaller=atVeto;c.vetoed=vetoed;c.refusal=why;c.phase=phase;return c;
}
int main(){
    assert(!std::strcmp(RefusalName(None),"NONE")&&!std::strcmp(RefusalName(Phase),"PHASE"));
    assert(!std::strcmp(RefusalName(RecordRace),"RECORD_RACE")&&!std::strcmp(RefusalName(Count),"UNKNOWN"));
    Ledger l;Attack a{};
    assert(!l.Pop(a));
    // Attack 1: false answers, then vetoed trues, then a refusal (first reason kept), foreign caller ignored.
    l.Observe(make(7,0x100,1000,false,true,false,None,4));
    l.Observe(make(7,0x100,1010,true,true,true,None,8));
    l.Observe(make(7,0x100,1020,true,true,true));
    l.Observe(make(7,0x100,1030,true,true,false,Phase,256));
    l.Observe(make(7,0x100,1040,true,true,false,OrdinaryMotion,16));
    l.Observe(make(7,0x100,1050,true,false,false));
    Call u=make(7,0x100,1060,false,true,false);u.records=3;u.unexpected=2;u.unexpectedType=5;u.unexpectedVft=0x3A62098;l.Observe(u);
    assert(l.current().open&&l.current().calls==7&&!l.pending());
    l.Expire(1200,300);assert(l.current().open); // still inside the idle window
    // A new identity closes attack 1 edge-only.
    l.Observe(make(8,0x100,1100,true,true,false,Mode));
    {Attack none{};assert(l.pending()==1&&l.Pop(a)&&!l.Pop(none));}
    assert(a.key==7&&a.action==0x100&&a.calls==7&&a.trueAnswers==5&&a.vetoed==2&&a.refused==2&&a.foreignCaller==1);
    assert(a.firstRefusal==Phase&&a.unexpectedRecords==2&&a.unexpectedType==5&&a.unexpectedVft==0x3A62098&&a.maxRecords==3);
    assert(a.phaseSeen==(4u|8u|256u|16u)&&a.first==1000&&a.last==1060&&!a.open);
    // Same serial, different native action object is a different attack.
    l.Observe(make(8,0x200,1110,true,true,true));
    assert(l.Pop(a)&&a.key==8&&a.action==0x100&&a.refused==1&&a.firstRefusal==Mode&&a.vetoed==0);
    // Idle expiry ends the last attack; reversed clocks also end it.
    l.Expire(1300,300);assert(!l.pending());
    l.Expire(1411,300);assert(l.pending()==1&&l.Pop(a)&&a.key==8&&a.action==0x200&&a.vetoed==1);
    l.Observe(make(9,1,5000,true,true,false,None));l.Expire(4000,300);
    assert(l.Pop(a)&&a.firstRefusal==Count); // refusal without a reason is never silently NONE
    const auto t=l.totals();
    assert(t.attacks==4&&t.calls==10&&t.trueAnswers==8&&t.vetoed==3&&t.refused==4&&t.unexpectedCalls==1&&t.droppedAttacks==0);
    // Bounded: the ring keeps the oldest closed attacks and counts the loss.
    for(unsigned i=0;i<Ledger::kPending+3;++i)l.Observe(make(100+i,1,6000+i,false,true,false));
    l.Expire(9000,300);
    assert(l.pending()==Ledger::kPending&&l.totals().droppedAttacks==3);
    assert(l.Pop(a)&&a.key==100);
    l.Discard();assert(!l.pending()&&!l.current().open);
    // Line budget: a few lines per second, never per frame.
    LineBudget b;unsigned ok=0;for(std::uint64_t ms=0;ms<1000;ms+=11)ok+=b.Take(20000+ms,3);
    assert(ok==3);assert(b.Take(21001,3));
    // Saturating counters.
    std::uint32_t v=0xFFFFFFFFu;Bump(v);assert(v==0xFFFFFFFFu);
    std::puts("align_veto_receipts24_test passed");return 0;
}
