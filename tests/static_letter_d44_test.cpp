#define KCD2VR_MAP_T2_HOST_TEST 1
#include "../src/held_map_feed_t2.cpp"
#include <cassert>
#include <cstdio>
#include <limits>
#include <map>
#include <vector>
#include <string>
using namespace kcd2vr::held_map_feed_t2;
struct Memory {
 std::map<std::uintptr_t,std::vector<unsigned char>> blocks;
 bool replace=false;unsigned registryReads=0;
 bool operator()(std::uintptr_t address,void* to,std::size_t n){
  if(address==0x200040&&++registryReads==2&&replace){Span changed{0x900000,0x900010};std::memcpy(to,&changed,n);return true;}
  auto i=blocks.upper_bound(address);if(i==blocks.begin())return false;--i;
  if(address-i->first+n>i->second.size())return false;std::memcpy(to,i->second.data()+address-i->first,n);return true;
 }
 template<class T>void put(std::uintptr_t address,const T& value){auto i=blocks.upper_bound(address);assert(i!=blocks.begin());--i;assert(address-i->first+sizeof(value)<=i->second.size());std::memcpy(i->second.data()+address-i->first,&value,sizeof(value));}
};
Memory fixture(){Memory m;const std::uintptr_t b=0x180000000ull;
 for(auto [address,size]:std::initializer_list<std::pair<std::uintptr_t,std::size_t>>{{b+0x549D388,8},{0x100000,0x100},{0x200000,0x100},{0x300000,64},{0x400000,0x100},{0x500000,64},{0x600000,0x100},{0x700000,0x100},{0x800000,0x1B00}})m.blocks[address].resize(size);
 m.put(b+0x549D388,std::uintptr_t(0x100000));m.put(0x1000E8,std::uintptr_t(0x200000));m.put(0x200040,Span{0x300000,0x300030});
 m.put(0x300000,std::uintptr_t(0));m.put(0x300010,std::uintptr_t(0x400000));m.put(0x300020,std::uintptr_t(0x800000));
 m.put(0x400000,b+0x3C118B8);m.put(0x400060,Span{0x500000,0x500030});m.put(0x800000,b+0x3ED9140);
 m.put(0x500000,std::uintptr_t(0));m.put(0x500010,std::uintptr_t(0x600000));m.put(0x500020,std::uintptr_t(0x700000));
 for(auto a:{0x600000ull,0x700000ull}){m.put(a,b+0x3A6B4C8);m.put(a+0x10,b+0x3A6B4F0);m.put(a+0x50,float(1200));m.put(a+0x54,float(2400));m.put(a+0x58,float(10));}
 m.put(0x60004C,int(2));m.put(0x70004C,int(69));return m;
}
static Memory* activeMemory=nullptr;static unsigned sent=0,flagCalls=0;static std::string lastPayload,playerReply="T2|actor42|trosecko|1200|2400|20|1.57|1";
static unsigned intentSent=0;static bool intentCallback=true,intentWrongSerial=false;static int reenterIntent=0;static std::string lastIntent;
static unsigned readySent=0;static bool readyCallback=true,readyWrongActor=false;
static std::vector<std::string> readinessMessages;
static const char* closePhase39="CLOSED";
static const char* openPhaseD44="STATIC_READY";static bool oldCompanionD44=false;
namespace kcd2vr::held_map_feed_t2 {namespace {
bool HostRead(std::uintptr_t from,void* to,std::size_t n) noexcept{return activeMemory&&(*activeMemory)(from,to,n);}
void HostFlag(void* self,float* xy) noexcept {assert(self==reinterpret_cast<void*>(0x801508));++flagCalls;xy[0]=1600;xy[1]=2600;}
}}
namespace kcd2vr::retail_build112 {bool Verify(std::uintptr_t) noexcept{return true;}}
namespace kcd2vr::diagnostic_trace {void Write(const char* message) noexcept{
 if(std::strncmp(message,"T5 MAP_READINESS ",17)==0)readinessMessages.emplace_back(message);
}}
namespace kcd2vr::native_script_t2 {
bool Register(Client,Wanted) noexcept{return true;}bool Install() noexcept{return true;}
bool Context::CallPacket(const char* name,const char* packet,char* out,std::size_t cap) const noexcept {
 if(std::string(name)=="kcd2vr_map_t3_ready"){
  ++readySent;if(!readyCallback)return false;
  unsigned long long activation=0,epoch=0;unsigned actor=0;assert(std::sscanf(packet,"R3|%llu|%llu|%u",&activation,&epoch,&actor)==3);
  std::snprintf(out,cap,"R3|%s|%u|%llu|%llu|1|2|HELPERS_READY",oldCompanionD44?"33.0-request-owner":"44.0-static-letter",readyWrongActor?actor+1:actor,epoch,activation);return true;
 }
 if(std::string(name)=="kcd2vr_map_m31_intent"){
  ++intentSent;lastIntent=packet;if(!intentCallback)return false;
  unsigned long long serial=0;int open=0;assert(std::sscanf(packet,"I31|%llu|%d",&serial,&open)==2);
  if(!open&&reenterIntent){const auto mode=reenterIntent;reenterIntent=0;kcd2vr::held_map_m1::SetRequested31(true,now);if(mode==2)kcd2vr::held_map_m1::SetRequested31(false,now);}
  std::snprintf(out,cap,"I31|%llu|%s",intentWrongSerial?serial-1:serial,open?openPhaseD44:closePhase39);return true;
 }
 assert(std::string(name)=="kcd2vr_map_t2_feed");++sent;lastPayload=packet;
 assert(playerReply.size()<cap);std::memcpy(out,playerReply.c_str(),playerReply.size()+1);return true;
}
}

static unsigned checksD44=0,failedD44=0;
static void ckD44(bool value,const char* reason){++checksD44;if(!value){++failedD44;std::printf("FAIL %s\n",reason);}}
int main(){
 namespace hm=kcd2vr::held_map_m1;
 const std::uintptr_t b=0x180000000ull;auto m=fixture();activeMemory=&m;base=b;ready=true;
 kcd2vr::native_script_t2::Context context{1,2,1000,1};
 auto frame=[&](std::uint64_t now,bool request=true){
  context.now=now;hm::PublishAllowed(true,now);if(request)RequestReadiness(42,7,now,now);Tick(true,now);Service(context);
 };
 hm::Select(true);openPhaseD44="PREPARING";frame(1000);
 auto a=hm::activation.load();
 ckD44(!LetterReadyD44(42,7,a,1000),"hidden preparation cannot show a folded paper");
 openPhaseD44="OPEN";frame(1100);
 ckD44(!LetterReadyD44(42,7,a,1100),"old animated OPEN reply is not static readiness");
 openPhaseD44="STATIC_READY";frame(1200);
 ckD44(LetterReadyD44(42,7,a,1200),"exact new companion ready receipt permits current letter");
 ckD44(!LetterReadyD44(43,7,a,1200),"wrong actor refused");
 ckD44(!LetterReadyD44(42,8,a,1200),"wrong pose epoch refused");
 ckD44(!LetterReadyD44(42,7,a+1,1200),"wrong activation refused");
 ckD44(!LetterReadyD44(42,7,a,1199),"future receipt refused");
 hm::PublishAllowed(true,1951);ckD44(!LetterReadyD44(42,7,a,1951),"stale preparation receipt refused");
 frame(2000);ckD44(LetterReadyD44(42,7,a,2000),"current acknowledged prepared pose renews");
 hm::PublishAllowed(false,2001);ckD44(!LetterReadyD44(42,7,a,2001),"native UI/tracking admission remains required");
 hm::PublishAllowed(true,2002);
 ckD44(ReportHelperMissing(42,7,a),"exact absent helper invalidates successful preparation");
 ckD44(!LetterReadyD44(42,7,a,2002),"missing helper cannot reuse old ready receipt");
 frame(2500);ckD44(LetterReadyD44(42,7,a,2500),"fresh helper identity and new receipt recover");
 intentCallback=false;frame(3000);
 ckD44(!LetterReadyD44(42,7,a,3000),"callback loss revokes readiness immediately");
 intentCallback=true;intentWrongSerial=true;frame(3100);
 ckD44(!LetterReadyD44(42,7,a,3100),"wrong serial reply revoked");
 intentWrongSerial=false;frame(3200);
 ckD44(LetterReadyD44(42,7,a,3200),"matched serial recovers");
 context.generation=2;frame(3210,false);
 ckD44(!LetterReadyD44(42,7,a,3210),"VM replacement cannot inherit old helper preparation");
 frame(3310);ckD44(LetterReadyD44(42,7,a,3310),"current VM needs its own helper and frozen pose receipt");
 hm::SetRequested31(false,3311);intentCallback=false;
 ckD44(!hm::Present31(3311),"B close hides immediately even before unavailable Lua close ACK");
 ckD44(!LetterReadyD44(42,7,a,3311),"close retires visible letter admission");
 hm::SetRequested31(true,3400);oldCompanionD44=true;intentCallback=true;frame(3400);
 a=hm::activation.load();
 ckD44(!LetterReadyD44(42,7,a,3400),"old companion revision cannot authorize new static presenter");
 oldCompanionD44=false;frame(3900);
 ckD44(LetterReadyD44(42,7,a,3900),"matching revision restores readiness");
 openPhaseD44="FAULT";frame(4400);
 ckD44(!hm::Requested()&&!hm::Present31(4400),"native pose fault closes only current letter intent");
 std::printf("D44 actual native letter feed: %u checks, %u failures; renderer/headset NOT_RUN\n",checksD44,failedD44);
 return failedD44?1:0;
}
