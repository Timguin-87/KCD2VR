// Executes the actual native-side read-only capture include, with explicit memory/actor mocks.
#include "vrik/direct_response36.h"
#include "vrik/defense_observe37.h"
#include "vrik/direct_counter39.h" // C39 counter candidate receipt inside Begin
#include <array>
#include <atomic>
#include <mutex>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <cassert>
namespace diagnostics::capture168 {
enum class Domain {Combat};
struct Gate {std::atomic<std::uint64_t> session[1]{};} gate;
bool on=true;bool active(Domain,std::uint64_t){return on;}
}
namespace diagnostic_trace {std::vector<std::string> lines;void Write(const char* s){lines.emplace_back(s);}}
static std::uintptr_t g_base=0x10000000,kCombatActorVft=0x3E3FAC8;
struct Block {std::array<unsigned char,0x1000> b{};};
static Block actorMem,stateMem,henryMem,entityMem;static unsigned reads=0;
template<class T> void set(Block& b,std::size_t off,T value){std::memcpy(b.b.data()+off,&value,sizeof(value));}
template<class T> T Rd(const void* addr,std::size_t off,T fallback){
 ++reads;const auto p=reinterpret_cast<std::uintptr_t>(addr)+off;
 for(auto* b:{&actorMem,&stateMem,&henryMem,&entityMem}){
  const auto start=reinterpret_cast<std::uintptr_t>(b->b.data());
  if(p>=start&&p-start<=b->b.size()-sizeof(T)){T v{};std::memcpy(&v,reinterpret_cast<const void*>(p),sizeof(v));return v;}
 }return fallback;
}
static const void *actor=actorMem.b.data(),*state=stateMem.b.data(),*entity=entityMem.b.data();
static bool identity=true;
bool CombatActor(const void* a,const void*& e,const void*& s,bool){e=entity;s=state;return identity&&a==actor;}
std::uint32_t EntityId(const void* e){return e==entity?7:0;}
struct Pending {std::uint32_t serial=1,entityId=7;};
// C37 reads go through one SEH-safe copy in production; here only the mock blocks are readable.
static bool Copy37(std::uintptr_t p,void* out,std::size_t n){
 for(auto* b:{&actorMem,&stateMem,&henryMem,&entityMem}){
  const auto start=reinterpret_cast<std::uintptr_t>(b->b.data());
  if(p>=start&&n<=b->b.size()&&p-start<=b->b.size()-n){std::memcpy(out,reinterpret_cast<const void*>(p),n);return true;}
 }return false;
}
#include "diagnostics/counter_response36.inl"
int main(){
 using namespace counter36;
 diagnostics::capture168::gate.session[0]=1;
 set(actorMem,0,g_base+kCombatActorVft);set(stateMem,0x38,8);set(stateMem,0x218,0);
 set(stateMem,0x860,g_base+std::uintptr_t(0x3B0A6A0));set(stateMem,0x868,std::uint8_t(1));
 set(stateMem,0x278,2);set(stateMem,0x158,1);set(henryMem,0x7D8,1);
 const auto before=Capture(henryMem.b.data(),state,1,100);assert(before.captured&&before.window==1&&before.origin==1&&before.npc.combo==1);
 Begin(Pending{},actor,state,before,1,100);assert(watches[0].actor==actor);
 {bool c39=false;for(auto& l:diagnostic_trace::lines)if(l.find("C39 DIRECT_COUNTER edge=CANDIDATE")!=std::string::npos){c39=true;
   assert(l.find("npcPhase=8 offered158=1 startOffered158=-1 msStance=2 origin=1 counterSide=0 ms7D8=1")!=std::string::npos);
   assert(l.find("window=0 sinceStartMs=-1")!=std::string::npos&&l.find("admission=NOT_REQUESTED")!=std::string::npos);}
  assert(c39);}
 set(stateMem,0x38,64);set(stateMem,0x868,std::uint8_t(0));ActorUpdated(actor,120);
 assert(diagnostic_trace::lines.back().find("hitPhaseTransition=1")!=std::string::npos);
 assert(diagnostic_trace::lines.back().find("comboWindowClosed=1")!=std::string::npos);
 assert(diagnostic_trace::lines.back().find("interruptCause=UNPROVEN")!=std::string::npos);
 const auto count=diagnostic_trace::lines.size();ActorUpdated(actor,130);assert(diagnostic_trace::lines.size()==count);
 diagnostics::capture168::on=false;const auto priorReads=reads;ActorUpdated(actor,150);assert(reads==priorReads);
 diagnostics::capture168::on=true;diagnostics::capture168::gate.session[0]=2;ActorUpdated(actor,160);assert(!watches[0].actor);
 auto b2=Capture(henryMem.b.data(),state,1,200);Begin(Pending{},actor,state,b2,1,200);
 identity=false;ActorUpdated(actor,220);assert(!watches[0].actor);assert(diagnostic_trace::lines.back().find("IDENTITY_LOST")!=std::string::npos);
 identity=true;Begin(Pending{},actor,state,b2,2,300);ActorUpdated(actor,801);assert(!watches[0].actor);
 // C37: the dispatch bracket is emitted once for the SAME target and later NPC slot edges stay bounded.
 {kcd2vr::defense37::Dispatch d{};d.captured=true;d.result=1;d.target=reinterpret_cast<std::uintptr_t>(actor);d.reaction.ptr=0x1234;d.reaction.vft=0x3A61C58;
  d.before.slotCount=1;d.after.slotCount=1;d.after.slots[0].ptr=0x1234;
  const auto n0=diagnostic_trace::lines.size();Begin(Pending{},actor,state,b2,1,850,&d);
  bool seen=false;for(auto i=n0;i<diagnostic_trace::lines.size();++i)seen=seen||diagnostic_trace::lines[i].find("C37 REACTION_DISPATCH")!=std::string::npos;
  assert(seen);assert(diagnostic_trace::lines.back().find("landing=EMPTY_SLOT")!=std::string::npos);
  d.target=1;const auto n1=diagnostic_trace::lines.size();Begin(Pending{},actor,state,b2,1,851,&d);
  for(auto i=n1;i<diagnostic_trace::lines.size();++i)assert(diagnostic_trace::lines[i].find("C37")==std::string::npos);
  for(auto& w:watches)w={};}
 // C39: an opposite-side counter inside the published enemy attack start carries the window serial and its age; an idle NPC none.
 {for(auto& w:watches)w={};namespace dc=kcd2vr::direct_counter39;dc::Attack a{};
  a.observe(1,reinterpret_cast<std::uintptr_t>(state),7,true,2,-1,880);a.observe(1,reinterpret_cast<std::uintptr_t>(state),7,true,4,1,890);dc::publish(a);
  set(stateMem,0x38,4);auto b3=Capture(henryMem.b.data(),state,2,895);const auto n2=diagnostic_trace::lines.size();Begin(Pending{},actor,state,b3,1,895);
  bool c39=false;for(auto i=n2;i<diagnostic_trace::lines.size();++i)if(diagnostic_trace::lines[i].find("C39 DIRECT_COUNTER edge=CANDIDATE")!=std::string::npos){c39=true;
   assert(diagnostic_trace::lines[i].find("window=1 sinceStartMs=5 npcPhase=4 offered158=1 startOffered158=1 msStance=2 origin=2 counterSide=1")!=std::string::npos);}
  assert(c39);for(auto& w:watches)w={};
  a.observe(1,reinterpret_cast<std::uintptr_t>(state),7,true,2,-1,896);dc::publish(a);set(stateMem,0x38,2);
  auto b4=Capture(henryMem.b.data(),state,2,897);const auto n3=diagnostic_trace::lines.size();Begin(Pending{},actor,state,b4,1,897);
  for(auto i=n3;i<diagnostic_trace::lines.size();++i)assert(diagnostic_trace::lines[i].find("C39")==std::string::npos);
  for(auto& w:watches)w={};}
 // No unbounded log stream under oscillating phases; end summary remains possible.
 Begin(Pending{},actor,state,b2,1,900);const auto startLines=diagnostic_trace::lines.size();
 for(int i=0;i<20;++i){set(stateMem,0x38,(i%2)?8:64);ActorUpdated(actor,910+i);}
 assert(diagnostic_trace::lines.size()<=startLines+8);
}
