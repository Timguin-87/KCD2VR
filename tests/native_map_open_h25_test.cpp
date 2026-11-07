#include "subsystems/vrmap/runtime/native_map_open_h25.h"
#include "subsystems/vrmap/runtime/native_map_live_h28.h"
#include "native_script_t2.h"
#include <cassert>
#include <cstdio>
#include <cstring>
namespace n=kcd2vr::native_map_open_h25;
static unsigned calls=0;static bool callOk=true;static const char* response="NATIVE_READ_REQUESTED";
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept{}}
bool kcd2vr::native_script_t2::Context::CallPacket(const char* name,const char* input,char* out,std::size_t cap)const noexcept{
 assert(std::strcmp(name,"kcd2vr_map_native_read_h25")==0);++calls;
 std::snprintf(out,cap,"%s|%s",input,response);return callOk;
}
using namespace kcd2vr;
using kcd2vr::native_script_t2::Context;
#include "subsystems/vrmap/runtime/native_map_read_service_h25.inl"
static std::uint64_t begin(std::uint64_t now){n::ObserveChord(false);assert(n::Begin(now,true));return n::pending.load();}
int main(){n::Ticket t{};
 assert(!n::Begin(1000,false));assert(!n::Pending());n::ObserveChord(false);
 auto serial=begin(1000);assert(!n::Begin(1001,true));
 n::PublishUi(1000,false,true,false);ServiceNativeDocumentH25({0,0,1000,1});assert(calls==0&&n::Pending());
 n::PublishUi(1010,true,false,false);ServiceNativeDocumentH25({0,0,1010,1});assert(calls==0);
 n::PublishUi(1020,true,true,false);ServiceNativeDocumentH25({0,0,1121,1});assert(calls==0);
 n::PublishUi(1130,true,true,false);ServiceNativeDocumentH25({0,0,1130,1});assert(calls==1&&!n::Pending());
 ServiceNativeDocumentH25({0,0,1140,1});assert(calls==1);
 assert(!n::Begin(1140,true));
 serial=begin(2000);response="WAIT_FOCUS";
 n::PublishUi(2000,true,true,false);ServiceNativeDocumentH25({0,0,2000,1});assert(calls==2&&n::Pending());
 n::PublishUi(2100,true,true,false);ServiceNativeDocumentH25({0,0,2100,1});assert(calls==2);
 n::PublishUi(2250,true,true,false);ServiceNativeDocumentH25({0,0,2250,2});assert(calls==2&&!n::Pending());
 serial=begin(3000);callOk=false;n::PublishUi(3000,true,true,false);ServiceNativeDocumentH25({0,0,3000,2});assert(calls==3&&!n::Pending());
 callOk=true;serial=begin(4000);
 for(unsigned i=0;i<3;++i){auto now=4000+i*250;n::PublishUi(now,true,true,false);ServiceNativeDocumentH25({0,0,now,2});}
 assert(calls==6&&!n::Pending());
 serial=begin(5000);n::PublishUi(5001,true,true,true);assert(!n::Pending());
 serial=begin(6000);assert(!n::Current(9001,t)&&!n::Pending());
 serial=begin(10000);n::PublishUi(10001,true,true,false);auto newer=serial+1;n::Finish(newer);assert(n::Pending());n::Finish(serial);
 std::puts("PASS production native map UI request lease, focus budget, cancel/expiry/VM/no-replay and chord ownership");
}
