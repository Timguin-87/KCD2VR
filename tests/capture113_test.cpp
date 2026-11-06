#include "diagnostics/combat_capture.h"
#include <cassert>
int main() {
    namespace c=kcd2vr::diagnostics::combat_capture;
    namespace cap=kcd2vr::diagnostics::capture168;
    assert(c::sources(1,1,0)==3);
    c::selectSources(c::sources(1,1,0),100);
    // Current capture authority is the explicit recorder, not legacy source tags.
    assert(!c::active(101));
    cap::gate.select(cap::Domain::Combat,true,100);
    cap::gate.select(cap::Domain::Interaction,true,100);
    assert(c::active(180099) && !c::active(180100));
    const auto session=c::session.load();
    c::selectSources(c::sources(1,1,0),200); // unrelated F10 mode edge
    assert(c::session==session && c::until==180100);
    c::selectSources(c::sources(0,1,0),300);
    cap::gate.select(cap::Domain::Combat,false,300);
    assert(c::active(301) && c::sourceMask==c::InteractionReference);
    c::Event e{};e.tick=301;e.kind=c::Kind::Input;c::name(e,"contextual_superaction");
    assert(c::push(e)); c::Event got{};assert(c::pop(got));
    assert(got.source==c::InteractionReference && got.session==c::session);
    c::selectSources(0,302);assert(c::active(302)); // provenance does not switch recorders
    cap::gate.select(cap::Domain::Interaction,false,302);assert(!c::push(e));
    c::selectSources(c::CombatReference,500);
    cap::gate.select(cap::Domain::Combat,true,500);
    e.tick=501;
    for(unsigned n=0;n<511;++n)assert(c::push(e));
    const auto dropped=c::dropped.load(); assert(!c::push(e) && c::dropped==dropped+1);
    for(unsigned n=0;n<511;++n)assert(c::pop(got));
    assert(!c::pop(got));
    c::selectSources(c::CombatReference,600,true);assert(c::until==180600);
    assert(cap::gate.until[unsigned(cap::Domain::Combat)]==180500); // no hidden lease renewal
}
