#include "../src/diagnostics/capture168.h"
#include "../src/diagnostics/combat_capture.h"
#include "../src/diagnostics/optional_groups175.h"
#include "../src/vrik/user_modes175.h"
#include <array>
#include <cassert>
#include <cstdio>
namespace cap=kcd2vr::diagnostics::capture168;
namespace cc=kcd2vr::diagnostics::combat_capture;
int main(){
    cap::Gate gate;
    for(unsigned selected=0;selected<cap::DomainCount;++selected){
        gate.select(cap::Domain(selected),true,1000);
        for(unsigned i=0;i<cap::DomainCount;++i)
            assert(gate.active(cap::Domain(i),1001)==(i<=selected));
    }
    const auto session=gate.session[unsigned(cap::Domain::Reload)].load();
    gate.select(cap::Domain::Combat,false,1002);
    assert(gate.active(cap::Domain::Reload,1003));
    assert(gate.active(cap::Domain::Interaction,1003));
    assert(gate.session[unsigned(cap::Domain::Reload)].load()==session);
    assert(!gate.active(cap::Domain::Reload,999));
    assert(gate.poll(181000)==((1u<<cap::DomainCount)-2u)); // all domains except stopped Combat
    assert(!gate.any(181001));
    assert(cap::domain("V66.175 F11-RELOAD SNAPSHOT")==cap::Domain::Reload);
    assert(cap::domain("WEAPON_LIFECYCLE phase=LOAD")==cap::Domain::Reload);

    // Legacy comparison/source flags cannot arm hidden producers.
    cc::Event event{};event.tick=2000;cc::selectSources(cc::F11Stabs167,1999,true);
    assert(!cc::active(2000)&&!cc::push(event));
    cap::gate.select(cap::Domain::Reload,true,1999);
    assert(!cc::active(2000)&&!cc::push(event)); // reload owns no combat enqueue
    cap::gate.select(cap::Domain::Hands,true,1999);
    assert(!cc::active(2000)&&!cc::push(event)); // F10 does not arm expensive combat capture
    cap::gate.select(cap::Domain::Combat,true,1999);
    assert(cc::active(2000)&&cc::push(event));assert(cc::pop(event));
    cap::gate.select(cap::Domain::Combat,false,2001);
    event.tick=2002;assert(!cc::active(2002)&&!cc::push(event));

    // An optional group failure only rolls back that group's installed hooks.
    std::array<bool,23> installed{};unsigned attempts=0,removed=0;
    const auto ready=kcd2vr::diagnostics::optional175::arm(
        [](auto){return true;},[&](std::size_t i){++attempts;if(i==0)return false;installed[i]=true;return true;},
        [&](std::size_t i){installed[i]=false;++removed;});
    assert(ready==62u&&attempts==20&&removed==4);
    for(unsigned i=0;i<23;++i)assert(installed[i]==(i>=4));
    assert(kcd2vr::user175::configuration(kcd2vr::user175::Mode::Off).pose==0);
    assert(kcd2vr::user175::configuration(kcd2vr::user175::Mode::Calibrated).weapon==6);
    assert(kcd2vr::user175::hands(5,0)==kcd2vr::user175::Mode::Off);
    assert(kcd2vr::user175::weapons(7,false)==kcd2vr::user175::Mode::Calibrated);
    assert(kcd2vr::user175::combat(true)==kcd2vr::user175::Mode::Calibrated);
    std::puts("PASS recorder domains, finite expiry, mode-independent enqueue, and partial optional groups");
}
