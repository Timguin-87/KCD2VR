#include "subsystems/ranged_reload/reload_diagnostics_d48.h"
#include <cstdio>
#include <string>
#include <vector>

namespace c=kcd2vr::diagnostics::capture168;
namespace r=kcd2vr::reload_diagnostics_d48;
static unsigned checks=0;
#define CHECK(x) do {++checks;if(!(x)){std::printf("FAIL line %d: %s\n",__LINE__,#x);return 1;}} while(false)

int main(){
    constexpr std::uint64_t now=10000;
    std::vector<std::string> lines;
    const auto drain=[&](std::uint64_t tick){r::drain(tick,[&](const char* line){lines.emplace_back(line);});};
    r::recordEarlyPass(false,now,0x1234,21,22,1,0);
    CHECK(r::next.load()==0);
    c::gate.select(c::Domain::Bug,true,now);
    r::recordEarlyPass(true,now,0x1234,21,22,1,0);
    CHECK(r::next.load()==0); // F2 does not authorize reload producers (F5/F7 covered in D84 fixture).
    c::gate.select(c::Domain::Reload,true,now);
    const auto lease=c::gate.until[unsigned(c::Domain::Reload)].load();
    r::recordEarlyPass(false,now,0x1234,21,22,1,0);
    r::recordEarlyPass(true,now,0x5678,22,22,0,1);
    CHECK(r::next.load()==2); // Two requests in one millisecond are distinct.
    CHECK(c::gate.until[unsigned(c::Domain::Reload)].load()==lease);
    c::gate.select(c::Domain::Reload,false,now+1);
    drain(now+2);
    CHECK(lines.size()==2);
    CHECK(lines[0].find("stage=REQUEST ")!=std::string::npos);
    CHECK(lines[0].find("startReason=NON_FRAME_THREAD_NATIVE_PASS ")!=std::string::npos);
    CHECK(lines[0].find("equipment=1234 thread=21 ownerThread=22 requestA=1 requestB=0")!=std::string::npos);
    CHECK(lines[0].find("sourceReason=UNOBSERVED ")!=std::string::npos);
    CHECK(lines[0].find("nativeSuccess=NOT_INFERRED")!=std::string::npos);
    CHECK(lines[1].find("startReason=REPLAY_NATIVE_PASS ")!=std::string::npos);
    CHECK(lines[1].find("equipment=5678 thread=22 ownerThread=22 requestA=0 requestB=1")!=std::string::npos);
    CHECK(c::accept(lines[0].c_str(),now+2)); // Finite stopped-session drain.
    CHECK(!c::accept(lines[0].c_str(),now+6000));
    r::recordEarlyPass(false,now+2,0x1234,21,22,1,0);
    CHECK(r::next.load()==2); // Producers cannot use stopped-session grace.

    c::gate.select(c::Domain::Reload,true,now+3);
    r::recordEarlyPass(false,now+3,0x1234,21,22,1,0);
    c::gate.select(c::Domain::Reload,false,now+4);
    c::gate.select(c::Domain::Reload,true,now+5);
    lines.clear();drain(now+6);
    CHECK(lines.empty()); // A replacement session cannot inherit the old row.

    const auto priorDrop=r::dropped.load();
    for(unsigned i=0;i<100;++i)r::recordEarlyPass(false,now+6,0x1000+i,21,22,1,0);
    CHECK(r::dropped.load()-priorDrop==37);
    lines.clear();drain(now+7);
    CHECK(lines.size()==63); // Existing fixed queue bounds stay in effect.
    const auto beforeExpiry=r::next.load();
    r::recordEarlyPass(false,now+5+c::Duration,0x1234,21,22,1,0);
    CHECK(r::next.load()==beforeExpiry);
    CHECK(c::gate.until[unsigned(c::Domain::Reload)].load()==now+5+c::Duration);
    std::printf("PASS D57 F11 early-request diagnostics %u checks; no game or native reads\n",checks);
}
