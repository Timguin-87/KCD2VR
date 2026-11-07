// Actual service body. Only the VM/native read leaves and existing map owner
// query APIs are substituted; packet encoding is production code.
#include "subsystems/vrmap/quest_packet_d75.h"
#include <atomic>
#include <string>
#include <cassert>
#include <cstdio>
static unsigned reads=0,calls=0,flags=0,checks=0;
static bool wanted=true,allowed=true,current=true,valid=true;
static std::string packet;
namespace kcd2vr::held_map_m1 {
struct Intent{std::uint64_t serial=1;bool native=false;};static Intent intent;
Intent RendererIntent43(){return intent;}bool Requested(){return wanted;}bool Allowed(std::uint64_t){return allowed;}
bool RendererIntentMatches43(Intent i){return i.serial==intent.serial&&i.native==intent.native;}
}
namespace kcd2vr::diagnostic_trace {void Write(const char*){}}
namespace kcd2vr::diagnostics::capture168 {enum class Domain{Interaction};bool active(Domain,std::uint64_t){return false;}}
namespace kcd2vr::native_script_t2 {
struct Context {std::uint64_t now=1000,generation=1;bool Current()const{return current;}
bool CallPacket(const char* name,const char* value,char* out,std::size_t cap)const{
 assert(std::string(name)=="kcd2vr_map_quests_d75");++calls;packet=value;std::snprintf(out,cap,"Q75|LIVE|1");return true;}};
}
namespace kcd2vr::held_map_feed_t2 {
std::atomic<bool> ready{true};std::uintptr_t base=0;std::atomic<std::uint64_t> missingSerial{0};
struct ReadinessAck{std::uint64_t activation=1,epoch=7,vm=1,serial=1;unsigned actor=42,helper=100;bool native=false;};
struct Mailbox{ReadinessAck value{};bool read(ReadinessAck& a){a=value;return true;}}readinessAck;
struct Reader{};
bool Capture(Reader&,std::uintptr_t,NativeSnapshot& s){++reads;s={};s.compassKnown=true;s.count=1;s.markers[0]={1000,2000,0,2,0,Kind::Quest,1,2,3};return valid;}
bool Flag(std::uintptr_t,float(&xy)[2]){++flags;xy[0]=1200;xy[1]=2000;return true;}
#include "subsystems/vrmap/runtime/quest_service_d75.inl"
}
static void check(bool b){++checks;assert(b);}
int main(){using namespace kcd2vr;using namespace held_map_feed_t2;native_script_t2::Context c;
 ServiceQuestsD75(c);check(reads==1&&calls==1&&flags==1&&packet.find("2,2,3,1000.000,2000.000;")!=std::string::npos);
 for(c.now=1001;c.now<1500;++c.now)ServiceQuestsD75(c);check(reads==1&&calls==1);
 ServiceQuestsD75(c);check(reads==2&&calls==2);
 wanted=false;c.now=2000;ServiceQuestsD75(c);check(reads==2);wanted=true;
 allowed=false;ServiceQuestsD75(c);check(reads==2);allowed=true;
 current=false;ServiceQuestsD75(c);check(reads==2);current=true;
 held_map_m1::intent.native=true;ServiceQuestsD75(c);check(reads==2);held_map_m1::intent.native=false;
 readinessAck.value.actor=0;ServiceQuestsD75(c);check(reads==2);readinessAck.value.actor=42;
 c.now=2500;valid=false;ServiceQuestsD75(c);check(reads==3&&calls==3&&flags==2&&packet=="Q75|1|7|42|100|0||");
 c.now=3000;missingSerial=1;ServiceQuestsD75(c);check(reads==3);missingSerial=0;
 c.now=3500;c.generation=2;ServiceQuestsD75(c);check(reads==3);
 std::printf("D75 quest service actual body: %u checks PASS; max2Hz, no closed/native/stale-owner scans\n",checks);
}
