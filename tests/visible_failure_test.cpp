// DS-F17: visible failure line - thread-safe publish + limiter + mailbox.
#include "../src/vrik/visible_failure.h"
#include <cstdio>
#include <cstring>
namespace vf=kcd2vr::visible_failure;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
int main(){
    vf::lastDispatched.store(0,std::memory_order_release);
    vf::lastDrainedSerial.store(0,std::memory_order_release);
    // publish admits when the limiter is clear; rejects unsafe/oversized text
    CK(!vf::publish("Pickup blocked: OWNER_NOT_IDLE",1000));
    vf::Message quiet{};CK(vf::logMailboxD80.read(quiet)&&quiet.tick==1000);
    CK(!vf::diagnosticAllowedD74(quiet,1000));
    namespace cap=kcd2vr::diagnostics::capture168;
    cap::gate.select(cap::Domain::Bug,true,1000);
    CK(vf::publishDiagnosticD74("Pickup blocked: OWNER_NOT_IDLE",1000,cap::Domain::Bug));
    CK(!vf::safe("" )&&!vf::safe(nullptr));
    CK(!vf::safe("a\"b")&&!vf::safe("a\\b")&&!vf::safe("a\nb")&&!vf::safe("a\x01" "b"));
    {char big[200]{};std::memset(big,'x',150);CK(!vf::safe(big));} // over kMaxText=96
    // a successful dispatch arms the limiter: further publishes refused
    vf::lastDispatched.store(1000,std::memory_order_release);
    CK(!vf::publish("Pickup blocked: X",1500));
    CK(vf::publishDiagnosticD74("Pickup blocked: X",3001,cap::Domain::Bug)); // after 2 s
    // mailbox last-wins and carries a serial; read does not clear
    vf::Message m{};CK(vf::mailbox.read(m)&&m.serial&&m.text[0]);
    CK(vf::mailbox.read(m)); // same entry stays readable until the next publish
    CK(vf::diagnosticAllowedD74(m,3001));
    cap::gate.select(cap::Domain::Bug,false,3002);CK(!vf::diagnosticAllowedD74(m,3002));
    // due() clock-backwards admission
    vf::lastDispatched.store(9000,std::memory_order_release);
    CK(vf::due(1000));
    if(fails)return 1;std::puts("visible_failure F17 OK");return 0;
}
