// DS-F80: kick cannot-kill tier + reach gate (pure, host-tested).
#include "../src/subsystems/direct/kick_policy60.h"
#include <cstdio>
namespace k=kcd2vr::kick60;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
int main(){
    CK(k::isKick(4)&&!k::isKick(0)&&!k::isKick(5));
    CK(k::tier(4,5)==5); // DS-F83: the tier is NOT clamped until Claude proves cannot_kill semantics
    CK(k::tier(4,7)==7);CK(k::tier(0,5)==5);CK(k::tier(1,7)==7); // pass-through
    CK(k::reachOk(0.5f)&&k::reachOk(1.0f)&&!k::reachOk(1.5f)&&!k::reachOk(std::nanf("")));
    CK(k::refusalReason(false,false)!=nullptr); // no foot proxy
    CK(k::refusalReason(false,true)!=nullptr);  // out of reach
    CK(k::refusalReason(true,true)==nullptr);   // admitted
    if(fails)return 1;std::puts("kick policy F83 OK");return 0;
}
