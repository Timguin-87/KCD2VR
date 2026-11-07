#include "diagnostic_trace.h"
#include "diagnostics/unified_log_d81.h"
#include "diagnostics/capture_control168.h"
#include <windows.h>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <cstdio>
int main(){
 assert(kcd2vr::diagnostics::unified_log_d81::Initialize());
 namespace c=kcd2vr::diagnostics::capture168;
 namespace d=kcd2vr::diagnostic_trace;
 const auto now=std::uint64_t(GetTickCount64());
 const char* hit="C54R3 HIT_CHAIN reason=INPUT_OWNER_DOOR_FAULT HUD=OTHER_DOMAIN";
 const char* punch="C54R3 PUNCH_PATH reason=INPUT_OWNER_DOOR_FAULT";
 const char* door="C54R3 F5_DOOR_CHAIN reason=BOXING_HUD_FAULT";
 assert(!c::essential(hit)&&!c::essential(punch)&&!c::essential(door));
 assert(c::essential("C54R3 F5_DOOR_CAPTURE_END counters=FINAL"));
 assert(!c::essential("C54R3 F5_DOOR_CAPTURE_ENDLESS counters=FAKE"));
 c::gate.mask=0;assert(!c::accept(hit,now)&&!c::accept(door,now));
 c::gate.select(c::Domain::Combat,true,now);
 assert(c::accept(hit,now)&&c::accept(punch,now)&&!c::accept(door,now));
 assert(!c::accept(hit,now+c::Duration));
 c::gate.select(c::Domain::Combat,false,now);
 c::gate.select(c::Domain::Interaction,true,now);
 assert(!c::accept(hit,now)&&!c::accept(punch,now)&&c::accept(door,now));
 c::gate.select(c::Domain::Interaction,false,now);
 d::SetEnabled(true);assert(d::Enabled());
 d::Write("C54R3 HIT_CHAIN marker=QUIET_MUST_NOT_APPEAR");
 c::select(c::Domain::Combat,true,now,"HOST_HEALTH_TEST");d::Drain(true);
 // Actual production queue and Windows writer: deterministic fill without
 // draining, including one clipped payload. No game/native fixture here.
 std::string large="C54R3 HIT_CHAIN marker=LONG_PAYLOAD ";large.append(2200,'x');d::Write(large.c_str());
 for(int i=0;i<400;++i)d::Write("C54R3 HIT_CHAIN marker=FILL_QUEUE");
 d::Drain(true);
 c::select(c::Domain::Combat,false,now,"HOST_HEALTH_TEST");d::Drain(true);
 d::Write("C54R3 HIT_CHAIN reason=FAULT marker=OFF_MUST_NOT_APPEAR");
 c::select(c::Domain::Interaction,true,now,"HOST_HEALTH_TEST");
 d::Write("C54R3 HIT_CHAIN reason=DOOR_FAULT marker=WRONG_DOMAIN_MUST_NOT_APPEAR");
 d::Write("C54R3 F5_DOOR_CHAIN marker=EXPECTED_F5");d::Drain(true);
 c::select(c::Domain::Interaction,false,now,"HOST_HEALTH_TEST");d::SetEnabled(false);
 DetourModKit::log().shutdown(); // worker/test thread: drain the shared async writer before inspecting bytes
 wchar_t module[MAX_PATH]{};assert(GetModuleFileNameW(nullptr,module,MAX_PATH)>0);
 std::ifstream f(std::filesystem::path(module).parent_path()/L"KCD2VR.log",std::ios::binary);
 const std::string text{std::istreambuf_iterator<char>(f),{}};
 assert(text.find("MUST_NOT_APPEAR")==std::string::npos);
 assert(text.find("EXPECTED_F5")!=std::string::npos);
 assert(text.find("droppedTotal=")!=std::string::npos);
 assert(text.find("truncated:1")!=std::string::npos);
 assert(text.find("sinkRejectedBatches:0")!=std::string::npos);
 assert(text.find("scope=SHARED_PROCESS_TOTALS")!=std::string::npos);
 std::puts("PASS production Windows diagnostics writer: exact recorder gating, quiet off, finite expiry, queue overflow, truncation and capture health");
}
