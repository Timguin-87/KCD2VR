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
  std::snprintf(out,cap,"R3|33.0-request-owner|%u|%llu|%llu|1|2|HELPERS_READY",readyWrongActor?actor+1:actor,epoch,activation);return true;
 }
 if(std::string(name)=="kcd2vr_map_m31_intent"){
  ++intentSent;lastIntent=packet;if(!intentCallback)return false;
  unsigned long long serial=0;int open=0;assert(std::sscanf(packet,"I31|%llu|%d",&serial,&open)==2);
  if(!open&&reenterIntent){const auto mode=reenterIntent;reenterIntent=0;kcd2vr::held_map_m1::SetRequested31(true,now);if(mode==2)kcd2vr::held_map_m1::SetRequested31(false,now);}
  std::snprintf(out,cap,"I31|%llu|%s",intentWrongSerial?serial-1:serial,open?"OPEN":closePhase39);return true;
 }
 assert(std::string(name)=="kcd2vr_map_t2_feed");++sent;lastPayload=packet;
 assert(playerReply.size()<cap);std::memcpy(out,playerReply.c_str(),playerReply.size()+1);return true;
}
}
int main(){const std::uintptr_t b=0x180000000ull;NativeSnapshot s{};auto m=fixture();
 assert(Capture(m,b,s)&&s.compassKnown&&s.apse==0x800000&&s.count==2&&s.markers[0].kind==Kind::Quest&&s.markers[1].kind==Kind::Poi);
 m=fixture();m.put(0x300010,std::uintptr_t(0));assert(Capture(m,b,s)&&!s.compassKnown&&s.apse==0x800000&&s.count==0); // APSE survives absent/hidden compass.
 m=fixture();m.put(0x600050,std::numeric_limits<float>::infinity());assert(Capture(m,b,s)&&s.count==1);
 m=fixture();m.replace=true;assert(!Capture(m,b,s));
 m=fixture();m.put(0x400060,Span{0x500000,0x500011});assert(!Capture(m,b,s));
 m=fixture();m.put(0x200040,Span{0x300000,0x300000+16*301});assert(!Capture(m,b,s));
 Snapshot packet{};assert(PlayerPacket("T2|actor:42|trosecko|1200|2400|20|1.57|1",packet)&&packet.playerKnown&&packet.region==Region::Trosky);
 const auto first=packet.world;assert(PlayerPacket("T2|actor:42|trosecko|1200|2400|20|1.57|2",packet)&&packet.world!=first);
 assert(!PlayerPacket("T2|actor:42|Previous_Level|1200|2400|20|1.57|1",packet));
 assert(!PlayerPacket("T2|actor:42|trosecko|nan|2400|20|1.57|1",packet));
 assert(!PlayerPacket("T2|actor:42|trosecko|1200|2400|20|1.57|0",packet));
 assert(!PlayerPacket("T2|actor:42|trosecko|1200|2400|20|1.57|1|extra",packet));
 packet.valid=true;packet.tick=1000;packet.activation=9;assert(Fresh(packet,2500,9)&&!Fresh(packet,2501,9)&&!Fresh(packet,999,9)&&!Fresh(packet,1100,10));
 double u=0,v=0;assert(Project(Region::Trosky,496,3392,u,v)&&std::abs(u)<1e-9&&std::abs(v)<1e-9);
 assert(Project(Region::Trosky,3148,740,u,v)&&std::abs(u-1)<1e-9&&std::abs(v-1)<1e-9);
 assert(Project(Region::Kuttenberg,2048,2048,u,v)&&std::abs(u-.5)<1e-9&&std::abs(v-.4)<1e-9);
 assert(Project(Region::Kuttenberg,0,4096,u,v)&&std::abs(u)<1e-9&&std::abs(v-.4)<1e-9);
 assert(!Project(Region::Unknown,1,2,u,v));
 // Actual production Service/Tick/Read, with only native memory/flag/VM leaves
 // substituted: active-only2Hz, APSE without compass, one close-clear edge.
 m=fixture();m.put(0x300010,std::uintptr_t(0));activeMemory=&m;base=b;ready=true;sent=flagCalls=0;
 kcd2vr::held_map_m1::Select(true);kcd2vr::held_map_m1::PublishAllowed(true,1000);Tick(true,1000);
 kcd2vr::native_script_t2::Context context{1,2,1000,1};Service(context);
 assert(sent==1&&flagCalls==1&&lastPayload.find("|1|1600.000,2600.000|")!=std::string::npos);
 Snapshot live{};assert(Read(live,1000)&&live.playerKnown&&live.waypointKnown&&!live.compassKnown&&live.count==0);
 context.now=1400;kcd2vr::held_map_m1::PublishAllowed(true,1400);Tick(true,1400);Service(context);assert(sent==1);
 context.now=1500;Service(context);assert(sent==2&&flagCalls==2);
 Tick(false,1501);context.now=1501;Service(context);assert(sent==3&&lastPayload.find("|0||")!=std::string::npos&&!Read(live,1501)&&!Wanted());
 context.now=1700;Service(context);assert(sent==3); // no idle callback/keepalive.
 // The native waypoint exception disables only this feed. A still-visible
 // paper must publish ONE clear, then stop waking the native Lua scheduler.
 ready=true;kcd2vr::held_map_m1::Select(true);kcd2vr::held_map_m1::PublishAllowed(true,1800);Tick(true,1800);context.now=1800;Service(context);
 assert(sent==4&&Wanted());ready=false;Tick(true,1801);context.now=1801;Service(context);
 assert(sent==5&&lastPayload.find("|0||")!=std::string::npos&&!Read(live,1801)&&!Wanted());
 for(unsigned t=1802;t<1900;++t){Tick(true,t);context.now=t;Service(context);}
 assert(sent==5&&!Wanted());
 // Only explicit successful readiness can resume the visible source; normal
 // active pacing and current native actor/activation checks remain unchanged.
 ready=true;kcd2vr::held_map_m1::PublishAllowed(true,2400);Tick(true,2400);context.now=2400;Service(context);
 assert(sent==6&&Read(live,2400));
 // Readiness has its OWN lease: an absent/nonvisible helper must not prevent
 // calling the exact bootstrap global. Fresh request/current Allowed is required.
 Tick(false,2500);context.now=2500;Service(context);
 kcd2vr::held_map_m1::Select(true);kcd2vr::held_map_m1::PublishAllowed(true,3000);
 RequestReadiness(42,7,3000,3000);assert(Wanted());context.now=3000;Service(context);assert(readySent==1);
 RequestReadiness(42,7,3001,3001);assert(!Wanted()); // helper ACK stops readiness work
 for(unsigned t=3002;t<3050;++t)RequestReadiness(42,7,t,t);
 assert(!Wanted()&&readySent==1); // no steady inventory/VM readiness keepalive
 kcd2vr::held_map_m1::Select(true);kcd2vr::held_map_m1::PublishAllowed(true,4000);RequestReadiness(42,7,4000,4000);
 context.now=4081;Service(context);assert(readySent==1&&!Wanted()); // exact pose lease expired
 kcd2vr::held_map_m1::PublishAllowed(false,4200);RequestReadiness(42,7,4200,4200);assert(!Wanted());
 kcd2vr::held_map_m1::PublishAllowed(true,4300);RequestReadiness(0,7,4300,4300);assert(!Wanted());
 RequestReadiness(42,0,4300,4300);assert(!Wanted());
 kcd2vr::held_map_m1::Select(true);kcd2vr::held_map_m1::PublishAllowed(true,4400);
 RequestReadiness(42,7,4320,4400);assert(Wanted());context.now=4401;Service(context);assert(readySent==1&&!Wanted()); // publishing an80ms-old frame never renews its age
 RequestReadiness(42,7,4401,4400);assert(!Wanted()); // future source packet

 // A malformed/foreign actor receipt cannot acknowledge this request. Missing
 // callbacks retry at most24 times per activation/epoch/VM, with no tight loop.
 kcd2vr::held_map_m1::Select(true);readyWrongActor=true;
 for(unsigned i=0;i<26;++i){const auto t=5000+500*i;kcd2vr::held_map_m1::PublishAllowed(true,t);RequestReadiness(42,8,t,t);context.now=t;Service(context);}
 assert(readySent==25&&!Wanted()); // prior1 + bounded24, never accepted wrong ID
 ReadinessAck exhaustedAck{};
 assert(!(readinessAck.read(exhaustedAck)&&exhaustedAck.actor==42&&exhaustedAck.epoch==8&&
     exhaustedAck.activation==kcd2vr::held_map_m1::activation.load())); // exhaustion is NOT a successful helper ACK
 readyWrongActor=false;readyCallback=false;kcd2vr::held_map_m1::Select(true);
 for(unsigned i=0;i<26;++i){const auto t=20000+500*i;kcd2vr::held_map_m1::PublishAllowed(true,t);RequestReadiness(42,9,t,t);context.now=t;Service(context);}
 assert(readySent==49&&!Wanted());
 readyCallback=true;kcd2vr::held_map_m1::Select(true);kcd2vr::held_map_m1::PublishAllowed(true,40000);RequestReadiness(42,10,40000,40000);context.now=40000;Service(context);assert(readySent==50);
 kcd2vr::held_map_m1::Select(false);RequestReadiness(42,10,40001,40001);context.now=40001;Service(context);assert(!Wanted());
 // Same-owner helper loss invalidates ONE successful ACK, not every ABSENT
 // lookup. It grants no VM call or source-age extension by itself.
 assert(!ReportHelperMissing(42,10,kcd2vr::held_map_m1::activation.load()));
 kcd2vr::held_map_m1::Select(true);kcd2vr::held_map_m1::PublishAllowed(true,50000);
 RequestReadiness(42,20,50000,50000);context.now=50000;Service(context);assert(readySent==51&&!Wanted());
 const auto helperActivation=kcd2vr::held_map_m1::activation.load();
 assert(!ReportHelperMissing(43,20,helperActivation)&&!ReportHelperMissing(42,21,helperActivation)&&
     !ReportHelperMissing(42,20,helperActivation+1));
 assert(ReportHelperMissing(42,20,helperActivation));
 for(unsigned i=0;i<1000;++i)assert(!ReportHelperMissing(42,20,helperActivation));
 assert(readySent==51&&!Wanted());
 kcd2vr::held_map_m1::PublishAllowed(true,50081);
 RequestReadiness(42,20,50000,50081);assert(!Wanted()); // recovery cannot renew stale source
 RequestReadiness(42,20,50082,50081);assert(!Wanted()); // nor future source
 RequestReadiness(42,20,50081,50081);assert(Wanted());context.now=50081;Service(context);
 assert(readySent==52&&!Wanted()); // same actor/epoch/activation, real new readiness ACK
 // A failed recovery has its own24/500ms budget and terminal state. A cached
 // old success cannot repeatedly reset that budget when native Find stays absent.
 assert(ReportHelperMissing(42,20,helperActivation));readyCallback=false;
 const auto beforeRecovery=readySent;
 for(unsigned i=0;i<24;++i){const auto t=51000+500*i;
  kcd2vr::held_map_m1::PublishAllowed(true,t);RequestReadiness(42,20,t,t);context.now=t;Service(context);
  assert(readySent==beforeRecovery+i+1);
  if(i<23){const auto prior=readySent;RequestReadiness(42,20,t+1,t+1);context.now=t+1;Service(context);assert(readySent==prior);}
 }
 assert(readySent==beforeRecovery+24&&!Wanted());
 ReadinessTerminal exhausted{};assert(readinessTerminal.read(exhausted)&&exhausted.actor==42&&exhausted.epoch==20&&
     exhausted.activation==helperActivation&&exhausted.recovery==missingSerial.load());
 for(unsigned i=0;i<1000;++i){const auto t=63000+i;kcd2vr::held_map_m1::PublishAllowed(true,t);
  assert(!ReportHelperMissing(42,20,helperActivation));RequestReadiness(42,20,t,t);context.now=t;Service(context);}
 assert(readySent==beforeRecovery+24&&!Wanted());
 bool exhaustionLogged=false;for(const auto& message:readinessMessages)
  exhaustionLogged=exhaustionLogged||message.find("outcome=RETRY_EXHAUSTED")!=std::string::npos;
 assert(exhaustionLogged);
 // A genuine pose epoch change re-arms; a new active VM invalidates its old
 // readiness only after the shared script owner actually observes that VM.
 readyCallback=true;kcd2vr::held_map_m1::PublishAllowed(true,70000);
 RequestReadiness(42,21,70000,70000);context.now=70000;Service(context);
 const auto beforeVm=readySent;assert(beforeVm==beforeRecovery+25&&!Wanted());
 Tick(true,70000);context.generation=2;context.now=70010;Service(context);
 assert(readySent==beforeVm); // context change never synthesizes a new pose request
 kcd2vr::held_map_m1::PublishAllowed(true,70011);RequestReadiness(42,21,70011,70011);
 assert(Wanted());context.now=70011;Service(context);assert(readySent==beforeVm+1);
 ReadinessAck newVmAck{};assert(readinessAck.read(newVmAck)&&newVmAck.vm==2&&newVmAck.epoch==21);
 Tick(false,70012);context.now=70012;Service(context);assert(!Wanted());
 const auto idleCalls=readySent;for(unsigned i=0;i<50;++i){context.now=70013+i;Service(context);}
 assert(readySent==idleCalls&&!Wanted()); // no idle readiness VM polling/keepalive
 // M34: actual source Service/Wanted must deliver an explicit CLOSE even
 // after transient pose/UI hiding already retired the live feed. No renderer
 // visibility transition is itself an animation intent.
 using namespace kcd2vr::held_map_m1;
 ready=true;PublishAllowed(true,80000);Tick(true,80000);context.now=80000;Service(context);
 const auto openedIntent=intentSent;
 PublishAllowed(false,80001);Tick(false,80001);context.now=80001;Service(context);
 assert(!Wanted()&&intentSent==openedIntent); // transient hide did not close
 const auto hiddenFeed=sent,hiddenCaptures=flagCalls;
 SetRequested31(false,80002);assert(Wanted());context.now=80002;Service(context);
 assert(intentSent==openedIntent+1&&lastIntent.find("|0")!=std::string::npos&&!Wanted());
 assert(sent==hiddenFeed&&flagCalls==hiddenCaptures&&!Present31(80002)); // close needs no live capture/hand
 SetRequested31(true,80010);PublishAllowed(true,80010);Tick(true,80010);context.now=80010;Service(context);
 assert(intentSent==openedIntent+2&&lastIntent.find("|1")!=std::string::npos);
 PublishAllowed(false,80011);Tick(false,80011);context.now=80011;Service(context);assert(!Wanted());
 // A callback can supersede the request synchronously; its old ACK must never
 // retire a newer close or undo a newer open.
 SetRequested31(false,80012);reenterIntent=2;context.now=80012;Service(context);assert(Wanted()&&!Requested());
 context.now=80013;Service(context);assert(!Wanted());
 SetRequested31(true,80014);SetRequested31(false,80015);reenterIntent=1;context.now=80015;Service(context);
 assert(Requested()&&!Wanted());
 // Unknown/wrong-serial replies do not count as CLOSED. A bounded budget stops
 // work after120 calls, retaining the unacknowledged obligation (checked by the
 // source witness below). No idle VM polling or elapsed-only success.
 SetRequested31(false,81000);intentWrongSerial=true;const auto failedStart=intentSent;
 context.now=81000;Service(context);assert(Wanted());
 context.now=81010;Service(context);assert(intentSent==failedStart+1); // bounded33ms
 for(unsigned i=1;i<120;++i){context.now=81000+i*33;Service(context);}
 assert(intentSent==failedStart+120&&!Wanted());
 for(unsigned i=0;i<20;++i){context.now=88000+i*250;Service(context);}assert(intentSent==failedStart+120);
 intentWrongSerial=false;
 // A real newer user intent cancels an obsolete close; a new explicit close
 // receives its own bounded budget and actual matching ACK.
 SetRequested31(true,94000);SetRequested31(false,94001);context.generation=3;context.now=94001;Service(context);
 assert(!Wanted());
 // A missing callback across a real observed VM generation never inherits
 // an old CLOSED. Each VM gets bounded actual calls; matching current ACK only.
 SetRequested31(true,95000);SetRequested31(false,95001);intentCallback=false;context.generation=4;
 const auto vmCloseStart=intentSent;context.now=95001;Service(context);assert(Wanted());
 context.generation=5;context.now=95002;Service(context);assert(Wanted()&&intentSent==vmCloseStart+2);
 intentCallback=true;context.now=95003;Service(context);assert(Wanted()&&intentSent==vmCloseStart+2);
 context.now=95252;Service(context);assert(!Wanted()&&intentSent==vmCloseStart+3);
 // CLOSING is progress, not CLOSED. Poll even after live feed retired.
 SetRequested31(true,96000);SetRequested31(false,96001);closePhase39="CLOSING";
 const auto beforeClose39=intentSent, beforeCapture39=flagCalls;
 context.now=96001;Service(context);assert(Wanted());
 context.now=96034;Service(context);assert(Wanted()&&intentSent==beforeClose39+2);
 closePhase39="CLOSED";context.now=96067;Service(context);
 assert(!Wanted()&&intentSent==beforeClose39+3&&flagCalls==beforeCapture39);
 std::puts("PASS actual native map capture, null/identity/vector replacement/count gates, strict player/world metadata, freshness, rotated cropped native projection");
}
