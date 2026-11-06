#include "diagnostics/capture168.h"
#include <cassert>
#include <cstdio>
int main(){
    namespace cap=kcd2vr::diagnostics::capture168;
    for(unsigned i=0;i<cap::DomainCount;++i)cap::gate.select(cap::Domain(i),false,1000);
    const char* actions[]{"C46 DOOR_USE target=42", "C46 PAGE_INPUT source=FLICK",
        "C46 SEMI_ALCHEMY phase=HAND_STOPPED", "C46 SEMI_SMITH phase=HAND_ADVANCING"};
    for(const char* text:actions){assert(cap::domain(text)==cap::Domain::Interaction);assert(!cap::accept(text,1001));}
    cap::gate.select(cap::Domain::Combat,true,1000);
    for(const char* text:actions)assert(!cap::accept(text,1001));
    cap::gate.select(cap::Domain::Interaction,true,1000);
    for(const char* text:actions)assert(cap::accept(text,1001));
    cap::gate.select(cap::Domain::Combat,false,1002);cap::gate.select(cap::Domain::Interaction,false,1002);
    const char* setup[]{"C46 DOOR_SERVICE bridge=READY", "C46 DOOR_SERVICE bridge=UNAVAILABLE",
        "C46 DOOR_COMPANION observed revision=46", "C46 SEMI_ALCHEMY READY work=BELLOWS_MORTAR",
        "C46 SEMI_ALCHEMY INSTALL_REFUSED nativeFallback=1", "C46 SEMI_SMITH READY exact=1",
        "C46 SEMI_SMITH INSTALL_REFUSED nativeFallback=1", "C46-IMMERSION VrMinigameHands=1",
        "C46-ACTIONS VrSmithingAutoHold=ON"};
    for(const char* text:setup)assert(cap::accept(text,1003));
    std::puts("interaction_receipts46: F5 door/page/work routing, F3 isolation, quiet bounded setup receipts PASS");
}
