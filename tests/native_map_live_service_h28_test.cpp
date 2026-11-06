#include "subsystems/vrmap/runtime/native_map_live_h28.h"
#include "held_map_snapshot_t2.h"
#include "native_script_t2.h"
#include <array>
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
namespace kcd2vr::activity_input163 {
enum class Kind175{Reading};struct W {std::atomic<bool> ready{true};std::atomic<unsigned> transitionDepth175{0};std::atomic<std::uint64_t> lifetime175{11};std::array<std::atomic<std::uintptr_t>,1> active{};} witness175;
}
namespace kcd2vr::diagnostic_trace {void Write(const char*)noexcept{}}
static unsigned captures=0,calls=0,offs=0;static bool surface=true;static char last[16384]{};
bool kcd2vr::native_script_t2::Context::CallPacket(const char* name,const char* input,char* out,std::size_t cap)const noexcept {
 assert(std::strcmp(name,"kcd2vr_map_native_live_h28")==0);++calls;
 if(std::strcmp(input,"OFF")==0){++offs;std::snprintf(out,cap,"L8|OFF");return true;}
 std::snprintf(last,sizeof(last),"%s",input);
 std::snprintf(out,cap,"%s",surface?"T2|42|trosecko|1000|1000|10|0|1":"L8|NO_CURRENT_SURFACE_EVENT");return true;
}
namespace kcd2vr::held_map_feed_t2 {
constexpr std::uintptr_t base=0;struct Reader{};
bool Capture(Reader&,std::uintptr_t,NativeSnapshot& n){++captures;n={};n.apse=123;n.compassKnown=true;n.count=1;n.markers[0]={1050,1000,10,1,0xabc,Kind::Quest};return true;}
bool Flag(std::uintptr_t,float(&xy)[2]){xy[0]=1100;xy[1]=1000;return true;}
#include <h28_native_encode.inc>
#include "subsystems/vrmap/runtime/native_map_live_service_h28.inl"
std::atomic<bool> ready{false},clearPending{false};std::atomic<std::uint64_t> readinessVm{0},desiredUntil{0};core::SnapshotMailbox<Snapshot> snapshot;
void ServiceNativeDocumentH25(const native_script_t2::Context&){} void ServiceReadiness(const native_script_t2::Context&){}
namespace held_map_m1 {inline std::atomic<std::uint64_t> activation{0};bool Requested(){return false;}bool Allowed(std::uint64_t){return false;}}
#include <h28_native_combined_service.inc>
}
int main(){using namespace kcd2vr;using namespace native_map_live_h28;using native_script_t2::Context;
auto& w=activity_input163::witness175;w.active[0]=0x55;
Request(1,100,1);Publish(150,0x55,11,true);held_map_feed_t2::Service(Context{0,0,150,1});assert(captures==1&&calls==1&&std::strstr(last,"T2|11|3|1100.000,1000.000|1,1050.000,1000.000,10.000,abc;")!=nullptr);
Publish(200,0x55,11,true);held_map_feed_t2::Service(Context{0,0,200,1});assert(captures==1&&calls==1);
Publish(650,0x55,11,true);held_map_feed_t2::Service(Context{0,0,650,1});assert(captures==2&&calls==2);
w.lifetime175=12;held_map_feed_t2::Service(Context{0,0,651,1});assert(captures==2&&offs==1);Publish(652,0x55,12,true);assert(armed==0);
Request(2,1000,1);surface=false;
for(unsigned i=0;i<=10;++i){const auto t=1001+500*i;Publish(t,0x55,12,true);held_map_feed_t2::Service(Context{0,0,t,1});}
assert(armed==0&&until==0);held_map_feed_t2::Service(Context{0,0,6002,1});assert(!Wanted());
const auto before=captures;Request(3,7000,1);Publish(7001,0x55,12,true);held_map_feed_t2::Service(Context{0,0,7001,2});assert(captures==before&&armed==0);
std::puts("H28 actual native live service+encoder:2Hz, objective/waypoint packet, exact reading, bounded missing-surface and VM retirement PASS");}
