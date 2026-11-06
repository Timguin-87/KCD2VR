// R90: "no screen messages without F2". Host test of the actual visible_failure
// gate: with F2 (Bug) off nothing reaches the on-screen mailbox, whatever other
// capture (F3/F5/F7/F9/F11) is running; the log mailbox still gets the line.
// g++ -std=c++20 -I src tests/screen_f2_gate_r90_test.cpp -o /tmp/sf2 && /tmp/sf2
#include "vrik/visible_failure.h"
#include <cstdio>
#include <cstring>
namespace cap=kcd2vr::diagnostics::capture168;
namespace vf=kcd2vr::visible_failure;
static int fails=0,checks=0;
static void check(bool ok,const char* what){++checks;if(!ok){++fails;std::printf("FAIL %s\n",what);}}
static std::uint32_t screenSerial(){vf::Message m{};return vf::mailbox.read(m)?m.serial:0u;}
static std::uint32_t logSerial(){vf::Message m{};return vf::logMailboxD80.read(m)?m.serial:0u;}
int main(){
    std::uint64_t now=1000000;
    // 1) nothing on: log only
    auto s0=screenSerial(),l0=logSerial();
    check(!vf::publish("kick: wait 2 s between kicks",now),"no capture: publish refused");
    check(screenSerial()==s0,"no capture: screen mailbox untouched");
    check(logSerial()!=l0,"no capture: log mailbox receives the line");
    check(!vf::screenAllowedR90(now),"no capture: screenAllowed false");
    // 2) every non-F2 capture on: still nothing on screen
    for(auto d:{cap::Domain::Combat,cap::Domain::Interaction,cap::Domain::Reload,cap::Domain::Vrik,cap::Domain::Physics}){
        now+=3000;cap::gate.select(d,true,now);
        const auto session=cap::gate.session[unsigned(cap::canonical(d))].load();
        s0=screenSerial();
        check(!vf::publish("crossbow: refused",now,session,d),"other capture: session publish refused");
        check(!vf::publishDiagnosticD74("crossbow: refused",now,d),"other capture: publishDiagnostic refused");
        check(screenSerial()==s0,"other capture: screen mailbox untouched");
        check(!vf::screenAllowedR90(now),"other capture: screenAllowed false");
    }
    // 3) F2 on: a line from a still-live F11 lease is shown under the F2 session
    now+=3000;cap::gate.select(cap::Domain::Bug,true,now);
    const auto f2=cap::gate.session[unsigned(cap::Domain::Bug)].load();
    check(vf::screenAllowedR90(now),"F2: screenAllowed");
    const auto f11=cap::gate.session[unsigned(cap::Domain::Reload)].load();
    check(vf::publish("crossbow: refused",now,f11,cap::Domain::Reload),"F2+F11: shown");
    vf::Message m{};check(vf::mailbox.read(m)&&m.diagnosticSessionD74==f2&&m.diagnosticDomainD74==cap::Domain::Bug,"F2+F11: carries the F2 session");
    check(vf::diagnosticAllowedD74(m,now),"F2: drain allowed");
    // 4) session-0 status lines stay log-only even with F2 on (unchanged quiet rule)
    now+=3000;s0=screenSerial();
    check(!vf::publish("kick: lower your guard first",now),"F2: session-0 line log only");
    check(screenSerial()==s0,"F2: session-0 not on screen");
    // 5) F2 stopped: a queued line is not drained any more
    cap::gate.select(cap::Domain::Bug,false,now+10);
    check(!vf::diagnosticAllowedD74(m,now+10),"F2 off: queued line dropped at drain");
    check(!vf::screenAllowedR90(now+10),"F2 off: screenAllowed false");
    now+=3000;
    check(!vf::publishDiagnosticD74("F2 2: timing trial suspended",now,cap::Domain::Bug),"F2 off: Bug publish refused");
    // 6) F2 lease expired by time
    now+=3000;cap::gate.select(cap::Domain::Bug,true,now);
    check(vf::screenAllowedR90(now),"F2 re-armed");
    check(!vf::screenAllowedR90(now+cap::Duration+1),"F2 expired: screenAllowed false");
    std::printf("screen_f2_gate_r90: %d/%d checks passed\n",checks-fails,checks);
    return fails?1:0;
}
