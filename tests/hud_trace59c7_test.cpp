
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cassert>
#include "diagnostics/capture168.h"
#include "ui_hud_work59c7.h"
using namespace kcd2vr;
std::uint64_t tick=1000;std::uint64_t GetTickCount64(){return tick;}
std::atomic<unsigned> g_transition_hidden170{0};std::atomic<bool>g_vitals_pin59c7{false};
bool capture=false, g_display_verified170=true;
unsigned acquireCalls=0, finds=0,reads=0, emitted=0,written=0;
struct HudHolder{};
bool AcquireHudDirect(HudHolder&,void*&p){++acquireCalls;p=(void*)1;return true;}
void ReleaseHudHolder(HudHolder&){}
unsigned HudGeneration(){return 1;}
bool FindMovieClip(void*,const char*,void*&p,void*&g,void*&s){++finds;p=g=s=(void*)1;return true;}
struct HudVisibilityBackend170{
 static bool Call(void*,void*,void*buffer){
  ++reads; auto*b=(unsigned char*)buffer;std::memset(b,0,0x38);
  float f=100;std::memcpy(b+20,&f,4);std::memcpy(b+12,&f,4);std::memcpy(b+16,&f,4);
  std::uint16_t flags=0x43b;std::memcpy(b+52,&flags,2);b[40]=1;return true;
 }
};
namespace diagnostic_trace{void Write(const char*s){
 ++written;if(kcd2vr::diagnostics::capture168::accept(s,1000))++emitted;}}

#include "generated_hud_trace59c7.inc"
int main(){
 for(int i=0;i<16;++i)TraceHudVisibility57c1();
 assert(acquireCalls==0&&finds==0&&reads==0&&written==0);
 namespace c=kcd2vr::diagnostics::capture168;
 c::gate.select(c::Domain::Hud,true,1000);
 TraceHudVisibility57c1(false);assert(acquireCalls==1&&finds==11&&emitted==11);
 for(int i=0;i<10000;++i)TraceHudVisibility57c1(false);
 assert(acquireCalls==1&&finds==11);
 for(int i=0;i<20;++i)TraceHudVisibility57c1(true);
 assert(acquireCalls==8&&reads==88);
 c::gate.select(c::Domain::Hud,false,1000);TraceHudVisibility57c1();assert(reads==88);
 c::gate.select(c::Domain::Hud,true,1000);TraceHudVisibility57c1(false);assert(reads==99&&emitted==99);
 tick=200000;TraceHudVisibility57c1();assert(reads==99);
 puts("PASS full trace: zero OFF native work; initial unchanged-state snapshot; session budget reset; expiry");
}
