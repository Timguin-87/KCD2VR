// DS-F81: the master "our outcome" fallback (window closed -> major row).
#include "../src/subsystems/direct/counter_stab_policy.h"
#include <cstdio>
#include <cmath>
namespace cs=kcd2vr::counter_stab40;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static cs::In in(){cs::In i{};i.enabled=true;i.thrust=true;i.sameTarget=true;i.npcPhase=4;
    i.origin=1;i.offered=2;i.ratio=.8f;i.minRatio=.6f;i.sinceStartMs=200;i.windowMs=500;
    i.needsPerk=true;i.perk=true;return i;}
int main(){
    // window CLOSED + opposite stab on an attacker -> our outcome
    {auto i=in();i.windowOpen=false;
     CK(cs::counterOutcome(i)==cs::Why::Qualified);
     CK(cs::qualify(i)==cs::Why::NoWindow);} // the native path refused (window closed)
    // window OPEN -> the native master owns it (our outcome refuses)
    {auto i=in();i.windowOpen=true;
     CK(cs::counterOutcome(i)==cs::Why::NoWindow);
     CK(cs::qualify(i)==cs::Why::Qualified);}
    // no perk -> neither path qualifies
    {auto i=in();i.windowOpen=false;i.perk=false;
     CK(cs::counterOutcome(i)==cs::Why::NoPerk);}
    // wrong side / low energy still refuse the fallback
    {auto i=in();i.windowOpen=false;i.origin=1;i.offered=1;
     CK(cs::counterOutcome(i)==cs::Why::Side);}
    {auto i=in();i.windowOpen=false;i.ratio=.3f;
     CK(cs::counterOutcome(i)==cs::Why::LowEnergy);}
    if(fails)return 1;std::puts("counter outcome F81 OK");return 0;
}
