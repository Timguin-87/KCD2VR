#include "diagnostics/physics_stage_d42.h"
#include "diagnostics/passive_contact169r1.h"
#include <cstdio>
#include <string>
#include <vector>
namespace cap=kcd2vr::diagnostics::capture168;
namespace stage=kcd2vr::diagnostics::physics_d42;
namespace trace=kcd2vr::diagnostics::passive169r1;
namespace p=kcd2vr::passive169r1;
namespace npc172=kcd2vr::npc_contact172;
static std::uint64_t now=10000;
static unsigned queryStateReads=0;
struct Row {cap::Domain domain;std::uint64_t session;std::string text;};
static std::vector<Row> rows;
namespace diagnostic_trace {
static void Write(const char* text){rows.push_back({cap::Domain::Bug,0,text});}
static void WriteReceiptFor(cap::Domain d,std::uint64_t session,const char* text){
 if(cap::gate.receipt(d,session,now))rows.push_back({d,session,text});
}
}
namespace kcd2vr::diagnostics::bow169r2 {template<class Write>static void drain(std::uint64_t,Write&&){}}
static std::uint64_t GetTickCount64(){return now;}
static bool NpcQuerySelected172(){++queryStateReads;return false;}
static std::atomic<npc172::Gate> g_npc_gate172{};
static std::atomic<std::uint64_t> g_npc_queries172{0},g_npc_blocked172{0},g_npc_self172{0},g_npc_hits172[5]{};
#include "passive_contact_service_source.inc"
static int failures=0,checks=0;
static void check(bool ok,const char* reason){++checks;if(!ok){++failures;std::printf("FAIL D84 %s\n",reason);}}
static void stageDrain(bool close=false){stage::drain(now,[](cap::Domain d,std::uint64_t s,const char* row){diagnostic_trace::WriteReceiptFor(d,s,row);},close);}
static std::size_t count(const char* pattern){std::size_t n=0;for(const auto& row:rows)if(row.text.find(pattern)!=std::string::npos)++n;return n;}
static bool typed(const char* pattern,cap::Domain domain,std::uint64_t session){
 bool found=false;for(const auto& row:rows)if(row.text.find(pattern)!=std::string::npos){found=true;if(row.domain!=domain||row.session!=session)return false;}return found;
}
static void reset(){
 for(const auto d:{cap::Domain::Bug,cap::Domain::Combat,cap::Domain::Interaction,cap::Domain::Vrik,cap::Domain::Physics})cap::gate.select(d,false,now);
 stageDrain(true);ServicePassiveContact169R1();now+=6001;rows.clear();queryStateReads=0;
 trace::Record record{};while(trace::pop(record)){}trace::monitors={};for(auto&t:trace::lastRayTick)t=0;
}
static p::Sample sample(){p::Sample s{};s.source=p::Source::HandWorld;s.side=0;s.tick=now;s.sequence=now;
 s.actor=1;s.key=11;s.generation=s.epoch=s.request=1;s.eligible=s.worldCoherent=true;s.normal={-1,0,0};s.status=p::Status::Clear;return s;}
static void scope(){stage::Scope trace(now,now,[](){return now;});trace.receipt.outcome=stage::Outcome::Fallback;}
int main(){
 reset();unsigned clocks=0;{stage::Scope off(now,now,[&](){++clocks;return now;});check(!off.on(),"all-off T4 scope inactive");}
 trace::observe(sample(),false);{trace::RayScope ray(sample());check(!trace::ray(reinterpret_cast<const void*>(1),{},{},0),"all-off raw ray returns before reading bytes");}
 stageDrain();ServicePassiveContact169R1();check(!clocks&&rows.empty()&&!queryStateReads,"all-off observers do not read clock, format or request contact summary");
 cap::gate.select(cap::Domain::Physics,true,now);const auto f9=cap::gate.session[unsigned(cap::Domain::Physics)].load();
 scope();trace::observe(sample(),false);stageDrain();ServicePassiveContact169R1();
 check(typed("D42_F9 T4_STAGE",cap::Domain::Physics,f9)&&typed("D42_F9 T4_ACCOUNT",cap::Domain::Physics,f9),"F9 alone records native-held stage and account with original Physics ticket");
 check(typed("CONTACT_EVENT",cap::Domain::Physics,f9)&&typed("CONTACT_COVERAGE",cap::Domain::Physics,f9),"F9 alone records passive hand contacts and coverage");
 check(count("CONTACT_EVENT")==1&&count("D42_F2")==0&&count("D42_F3")==0&&count("D42_F7")==0,"F9-only capture does not impersonate other keys");
 rows.clear();now+=101;unsigned char bytes[80]{};bytes[3]=42;
 {trace::RayScope ray(sample());check(trace::ray(bytes,{},{1,0,0},.3f)>0,"F9 captures existing validated ray bytes");}
 ServicePassiveContact169R1();check(typed("CONTACT_RAY",cap::Domain::Physics,f9),"raw ray retains Physics ticket through actual service");
 reset();for(const auto d:{cap::Domain::Bug,cap::Domain::Physics,cap::Domain::Combat,cap::Domain::Vrik,cap::Domain::Interaction})cap::gate.select(d,true,now);
 scope();trace::observe(sample(),false);stageDrain();ServicePassiveContact169R1();
 check(count(" T4_STAGE ")==4,"F2/F7/F9/F3 independent native-held accounts");
 check(count("CONTACT_EVENT")==1&&rows.back().session!=0,"combined keys share one copied contact");
 check(typed("CONTACT_EVENT",cap::Domain::Physics,cap::gate.session[unsigned(cap::Domain::Physics)].load()),"combined contact selects identified Physics recorder");
 bool mask=false;for(const auto&r:rows)if(r.text.find("CONTACT_EVENT")!=std::string::npos)mask=r.text.find("captureMask=0x123")!=std::string::npos;
 check(mask,"contact records all active F9/F5/F7/F3 keys");
 // Stopping F3 must leave F9 producer and service active.
 cap::gate.select(cap::Domain::Combat,false,now);rows.clear();now+=501;scope();trace::observe(sample(),false);stageDrain();ServicePassiveContact169R1();
 check(count("D42_F9 T4_STAGE")==1&&count("CONTACT_EVENT")==1,"stop F3 preserves F9 stage/contact capture");
 // Already-copied samples keep their original domain during the bounded drain.
 reset();cap::gate.select(cap::Domain::Physics,true,now);cap::gate.select(cap::Domain::Combat,true,now);
 const auto capturedF9=cap::gate.session[unsigned(cap::Domain::Physics)].load();trace::observe(sample(),false);
 cap::gate.select(cap::Domain::Physics,false,now+1);++now;ServicePassiveContact169R1();
 check(typed("CONTACT_EVENT",cap::Domain::Physics,capturedF9),"F9 buffered contact is never relabeled as still-active F3");
 rows.clear();now+=501;trace::observe(sample(),false);ServicePassiveContact169R1();
 check(typed("CONTACT_EVENT",cap::Domain::Combat,cap::gate.session[unsigned(cap::Domain::Combat)].load()),"new contact uses remaining F3 independently");
 // Restarted or expired sessions cannot authorize old captured packets.
 reset();cap::gate.select(cap::Domain::Physics,true,now);trace::observe(sample(),false);
 cap::gate.select(cap::Domain::Physics,true,now+1);++now;ServicePassiveContact169R1();
 check(count("CONTACT_EVENT")==0,"replaced F9 session discards old contact before formatting");
 rows.clear();now+=501;trace::observe(sample(),false);cap::gate.select(cap::Domain::Physics,false,now);now+=5001;ServicePassiveContact169R1();
 check(rows.empty(),"expired receipt window cannot drain or summarize old contact");
 reset();cap::gate.select(cap::Domain::Physics,true,now);scope();const auto original=cap::gate.session[unsigned(cap::Domain::Physics)].load();
 cap::gate.select(cap::Domain::Physics,false,now+1);++now;stageDrain();
 check(!count(" T4_STAGE ")&&typed("D42_F9_END T4_ACCOUNT",cap::Domain::Physics,original),"F9 stop discards detail and ends the original stage account");
 rows.clear();stageDrain();check(rows.empty(),"stage account END is finite and emitted once");
 reset();cap::gate.select(cap::Domain::Vrik,true,now);trace::observe(sample(),false);ServicePassiveContact169R1();
 check(typed("CONTACT_EVENT",cap::Domain::Vrik,cap::gate.session[unsigned(cap::Domain::Vrik)].load()),"F7 alone exposes hand contact evidence");
 std::printf("physics capture D84: %d checks, %d failures\n",checks,failures);return failures?1:0;
}
