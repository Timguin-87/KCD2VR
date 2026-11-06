"""Execute the actual map ink services; engine/VM leaves are substituted."""
from m41_test_support import options, compile_and_run
import json
args=options('map_snapshot_d81');root=args.source_root
feed=(root/'src/subsystems/vrmap/runtime/held_map_feed_t2.cpp').read_text()
a=feed.index('void ServiceMarkersD64(');b=feed.index('#include "quest_service_d75.inl"',a)
services=feed[a:b]+(root/'src/subsystems/vrmap/runtime/quest_service_d75.inl').read_text()
prefix=r"""
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cassert>
unsigned checks=0,scans=0,questCalls=0,playerCalls=0;
#define CHECK(x) do{assert(x);++checks;}while(0)
bool requested=true,allowed=true,current=true,known=true,valid=true,callOK=true,replaceAck=false;
const char* questReply="Q75|LIVE|2"; const char* playerReply="D64|PLAYER";
struct Intent{std::uint64_t serial=1;bool native=false;};Intent intent;
namespace held_map_m1{
Intent RendererIntent43(){return intent;}bool Requested(){return requested;}bool Allowed(std::uint64_t){return allowed;}
bool RendererIntentMatches43(Intent v){return requested&&v.serial==intent.serial&&v.native==intent.native;}}
struct ReadinessAck{std::uint64_t activation=1,epoch=7,vm=1,serial=1;unsigned actor=42,helper=100;bool native=false;};
struct Mailbox{ReadinessAck value;bool read(ReadinessAck& a){a=value;return true;}}readinessAck;
std::atomic<bool> ready{true};std::atomic<std::uint64_t> missingSerial{0};std::uintptr_t base=0;
struct Reader{};struct NativeSnapshot{bool compassKnown=false;unsigned count=0;std::uintptr_t apse=0;};
bool Capture(Reader&,std::uintptr_t,NativeSnapshot& n){++scans;n.compassKnown=known;n.count=2;return valid;}
bool Flag(std::uintptr_t,float(&)[2]){return false;}
bool EncodeQuestD75(const NativeSnapshot&,bool,bool,const float(&)[2],std::uint64_t,std::uint64_t,unsigned,unsigned,char(&)[4096]){return true;}
namespace native_script_t2{struct Context{
 std::uint64_t now=1000,generation=1;
 bool Current()const{return current;}
 bool CallPacket(const char* name,const char*,char* out,std::size_t size)const{
  bool quest=std::strcmp(name,"kcd2vr_map_quests_d75")==0;
  if(quest)++questCalls;else ++playerCalls;
  std::snprintf(out,size,"%s",quest?questReply:playerReply);
  if(replaceAck){++readinessAck.value.serial;replaceAck=false;}
  return callOK;
 }
};}
namespace diagnostics::capture168{enum class Domain{Interaction};bool active(Domain,std::uint64_t){return false;}}
namespace diagnostic_trace{void Write(const char*){}}
"""
main=r"""
int main(){
 native_script_t2::Context c;
 auto step=[&](){c.now+=600;ServiceQuestsD75(c);ServiceMarkersD64(c);};
 step();CHECK(scans==1&&questCalls==1&&playerCalls==1);
 for(int i=0;i<1000;++i)step();CHECK(scans==1&&questCalls==1&&playerCalls==1);
 // Reopening refreshes even if the original helper survives.
 ++intent.serial;readinessAck.value.activation=intent.serial;++readinessAck.value.serial;
 step();CHECK(scans==2&&playerCalls==2);
 // A hidden or failed callback must retry, not freeze an empty map.
 ++readinessAck.value.serial;questReply="Q75|HIDDEN";playerReply="D64|HIDDEN";
 step();auto q=questCalls,p=playerCalls;step();CHECK(questCalls==q+1&&playerCalls==p+1);
 questReply="Q75|LIVE|0";playerReply="D64|PLAYER";step();q=questCalls;p=playerCalls;step();CHECK(questCalls==q&&playerCalls==p);
 ++readinessAck.value.serial;known=false;step();q=questCalls;step();CHECK(questCalls==q+1);
 known=true;step();q=questCalls;step();CHECK(questCalls==q);
 ++readinessAck.value.serial;valid=false;step();q=questCalls;step();CHECK(questCalls==q+1);
 valid=true;callOK=false;step();q=questCalls;step();CHECK(questCalls==q+1);
 callOK=true;step();q=questCalls;step();CHECK(questCalls==q);
 // A callback retiring the receipt cannot complete the new owner's snapshot.
 ++readinessAck.value.serial;replaceAck=true;step();q=questCalls;step();CHECK(questCalls==q+1);
 q=questCalls;step();CHECK(questCalls==q);
 // VM, body and helper recovery all issue fresh receipts.
 ++c.generation;readinessAck.value.vm=c.generation;++readinessAck.value.serial;step();CHECK(questCalls==q+1);q=questCalls;
 ++readinessAck.value.epoch;++readinessAck.value.serial;step();CHECK(questCalls==q+1);q=questCalls;
 ++readinessAck.value.helper;++readinessAck.value.serial;step();CHECK(questCalls==q+1);q=questCalls;
 missingSerial=readinessAck.value.serial;step();CHECK(questCalls==q);
 ++readinessAck.value.serial;step();CHECK(questCalls==q+1);q=questCalls;
 current=false;++readinessAck.value.serial;step();CHECK(questCalls==q);current=true;
 allowed=false;step();CHECK(questCalls==q);allowed=true;
 requested=false;step();CHECK(questCalls==q);requested=true;
 // No quest scan in native-document mode. Preserve its existing marker cadence.
 intent.native=true;readinessAck.value.native=true;p=playerCalls;
 step();step();CHECK(questCalls==q&&playerCalls==p+2);
 std::printf("PASS %u assertions; actual services, substituted engine leaves; no headset\n",checks);
}
"""
cpp=args.out/'snapshot.cpp';cpp.write_text(prefix+services+main)
run,log=compile_and_run(args,cpp,'snapshot');print(log);assert run.returncode==0
