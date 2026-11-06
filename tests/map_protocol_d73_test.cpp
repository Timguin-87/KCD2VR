// Production protocol bodies; only VM, clock and diagnostics leaves substituted.
#include "subsystems/vrmap/runtime/held_map_m1.h"
#include "subsystems/vrmap/static_paper_m41.h"
#include "core/snapshot_mailbox.h"
#include <cstdio>
#include <cstring>
#include <charconv>
#include <cassert>
#include <string>
static std::uint64_t testNow=1000;
static std::uint64_t GetTickCount64(){return testNow;}
static unsigned calls=0,markers=0;static int callbackMode=0;
static bool current=true;static std::string lastPacket;
namespace kcd2vr::diagnostic_trace {void Write(const char*){}}
namespace kcd2vr::diagnostics::capture168 {enum class Domain{Interaction};bool active(Domain,std::uint64_t){return false;}}
namespace kcd2vr::native_script_t2 {
struct Context {std::uint64_t now=0,generation=1;
 bool Current()const noexcept{return current;}
 bool CallPacket(const char*,const char*,char*,std::size_t)const noexcept;
};
}
namespace kcd2vr::held_map_feed_t2 {
static std::atomic<bool> ready{true};
#include "map_protocol_d73.inc"
}
namespace kcd2vr::native_script_t2 {
bool Context::CallPacket(const char* name,const char* input,char* output,std::size_t cap)const noexcept {
 lastPacket=input;
 if(!std::strcmp(name,"kcd2vr_map_markers_d64")){++markers;std::snprintf(output,cap,"OK");return true;}
 assert(!std::strcmp(name,"kcd2vr_map_t3_ready"));++calls;
 char prefix[16]{};unsigned long long serial=0,epoch=0;unsigned actor=0,helper=0;
 assert(std::sscanf(input,"%15[^|]|%llu|%llu|%u|%u",prefix,&serial,&epoch,&actor,&helper)==5);
 switch(callbackMode){
 case 1: held_map_m1::SetNativePaperM43(!held_map_m1::NativePaperSelectedM43());break;
 case 2: held_map_m1::SetRequested31(false,testNow);held_map_m1::SetRequested31(true,testNow);break;
 case 3: held_map_feed_t2::readinessVm=2;break;
 case 4: current=false;break;
 case 5: testNow+=81;break;
 case 6: std::strcpy(prefix,!std::strcmp(prefix,"R3")?"R3N44":"R3");break;
 case 7: ++actor;break;
 case 8: held_map_feed_t2::RequestReadiness(actor,epoch+1,testNow,testNow,helper);break;
 case 9: return false;
 }
 std::snprintf(output,cap,"%s|41.0-static-paper|%u|%llu|%llu|1|1|HELPER_READY|trosecko|%u",prefix,actor,epoch,serial,helper);
 return true;
}
}
static unsigned checks=0;
static void Check(bool b){++checks;assert(b);}
static void Reset(bool native=false){using namespace kcd2vr;
 testNow+=20000;callbackMode=0;current=true;calls=markers=0;
 held_map_m1::SetNativePaperM43(native);held_map_m1::Select(true);held_map_m1::PublishAllowed(true,testNow);
 held_map_feed_t2::ready=true;held_map_feed_t2::readinessVm=1;
 held_map_feed_t2::readinessAck.publish({});held_map_feed_t2::readinessTerminal.publish({});held_map_feed_t2::missingSerial=0;
 held_map_feed_t2::RequestReadiness(42,7,testNow,testNow,1042);
}
int main(){using namespace kcd2vr;using namespace held_map_feed_t2;
 Check(!held_map_m1::NativePaperSelectedM43());char path[160]{};
 for(bool native:{false,true}){
  Reset(native);const native_script_t2::Context c{testNow,1};ServiceReadiness(c);
  Check(calls==1&&lastPacket.starts_with(native?"R3N44|":"R3|"));
  Check(StaticModelM41(42,7,held_map_m1::activation.load(),1042,path));
  Check(std::string(path)=="objects/kcd2vr/staticmap40/trosecko.cgf");
  RequestReadiness(42,7,testNow,testNow,1042);Check(readinessUntil.load()==0);
  ServiceMarkersD64(c);Check(markers==1&&lastPacket.starts_with(native?"D64N44|":"D64|"));
  ServiceMarkersD64(c);Check(markers==1);
  Check(!StaticModelM41(43,7,held_map_m1::activation.load(),1042,path));
  Check(!StaticModelM41(42,8,held_map_m1::activation.load(),1042,path));
  Check(!StaticModelM41(42,7,held_map_m1::activation.load(),2042,path));
  const auto serial=held_map_m1::activation.load();Check(ReportHelperMissing(42,7,serial));
  Check(!ReportHelperMissing(42,7,serial));Check(!StaticModelM41(42,7,serial,1042,path));
  for(int mode=1;mode<=8;++mode){Reset(native);callbackMode=mode;const auto original=held_map_m1::activation.load();
   ServiceReadiness({testNow,1});Check(calls==1);ReadinessAck ack{};
   Check(!readinessAck.read(ack)||ack.activation!=original||ack.actor!=42);
   Check(!StaticModelM41(42,7,original,1042,path));
  }
  Reset(native);callbackMode=9;
  for(unsigned i=0;i<25;++i){held_map_m1::PublishAllowed(true,testNow);RequestReadiness(42,7,testNow,testNow,1042);ServiceReadiness({testNow,1});testNow+=500;}
  Check(calls==24);Check(!readinessUntil.load());
 }
 std::printf("D73 production map protocol %u checks PASS; VM leaves substituted\n",checks);
}
