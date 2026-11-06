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
 namespace c=kcd2vr::diagnostics::capture168;namespace d=kcd2vr::diagnostic_trace;
 const auto now=std::uint64_t(GetTickCount64());c::gate.mask=0;
 const char* labels[]={"D42_F2 FRAME", "D42_F3 FINISHER_CANDIDATE", "D42_F5 STAGE", "D42_F7 T4_STAGE"};
 const c::Domain domains[]={c::Domain::Bug,c::Domain::Combat,c::Domain::Interaction,c::Domain::Vrik};
 for(unsigned i=0;i<4;++i){
  assert(!c::essential(labels[i])&&!c::accept(labels[i],now));
  c::gate.select(domains[i],true,now);
  for(unsigned j=0;j<4;++j)assert(c::accept(labels[j],now)==(i==j));
  assert(!c::accept(labels[i],now+c::Duration));
  c::gate.select(domains[i],false,now);
 }
 assert(!c::essential("D42_F5_ENDLESS fake=1"));
 assert(c::essential("D42_F5_END account=1"));
 c::gate.select(c::Domain::Combat,true,now);
 assert(!c::accept("A34 NATIVE_PASSENGER active=1",now));
 assert(!c::accept("D42_F5 STAGE reason=FAULT HUD=OTHER_DOMAIN",now));
 c::gate.mask=0;c::gate.select(c::Domain::Interaction,true,now);
 assert(c::accept("A34 NATIVE_PASSENGER active=1",now));
 c::gate.mask=0;for(unsigned i=0;i<c::DomainCount;++i)c::gate.session[i]=(i+1)*10;
 d::SetEnabled(true);assert(d::Enabled());
 c::select(c::Domain::Bug,true,now,"D42_WINDOWS_WRITER_TEST");
 c::select(c::Domain::Vrik,true,now,"D42_WINDOWS_WRITER_TEST");
 d::Write("D42_F5 STAGE reason=FAULT marker=WRONG_DOMAIN_MUST_NOT_APPEAR");
 d::Write("D42_F7 T4_STAGE marker=EXPECTED_F7");d::Drain(true);
 c::select(c::Domain::Bug,false,now,"D42_WINDOWS_WRITER_TEST");
 c::select(c::Domain::Vrik,false,now,"D42_WINDOWS_WRITER_TEST");
 d::Write("D42_F7 T4_STAGE marker=OFF_MUST_NOT_APPEAR");
 d::Write("D42_F7_END marker=EXPECTED_END");
 d::Write("D42_F7_ENDLESS marker=QUIET_MUST_NOT_APPEAR");
 c::select(c::Domain::Interaction,true,now,"D42_WINDOWS_WRITER_TEST");
 d::Write("A34 NATIVE_PASSENGER marker=EXPECTED_PASSENGER");d::Drain(true);
 c::select(c::Domain::Interaction,false,now,"D42_WINDOWS_WRITER_TEST");d::SetEnabled(false);
 DetourModKit::log().shutdown(); // worker/test thread: drain the shared async writer before inspecting bytes
 wchar_t path[MAX_PATH]{};assert(GetModuleFileNameW(nullptr,path,MAX_PATH));
 std::ifstream f(std::filesystem::path(path).parent_path()/L"KCD2VR.log",std::ios::binary);
 const std::string s{std::istreambuf_iterator<char>(f),{}};
 assert(s.find("MUST_NOT_APPEAR")==std::string::npos);
 for(const auto* label:{"EXPECTED_F7","EXPECTED_END","EXPECTED_PASSENGER","F2:81","F3:10","F5:20","F7:61","HUD:30","F11:40","F10:50","HAND_REFERENCE:70","SOURCE_CAPABILITY_NOT_LIVE_READINESS"})assert(s.find(label)!=std::string::npos);
 assert(s.find("sinkRejectedBatches:0")!=std::string::npos);
 std::puts("PASS D42 actual Windows logger: correct eight domain sessions, strict four-domain routing, A34 F5, quiet off, finite expiry, bounded END and capability inventory");
}
