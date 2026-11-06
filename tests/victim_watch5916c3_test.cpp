#include <array>
#include <mutex>
#include <atomic>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "diagnostics/capture168.h"
#include "vrik/contact_authority176.h"
static std::vector<std::string> notes;namespace diagnostic_trace{static void Write(const char*s){notes.emplace_back(s);}}
struct Actor{void* entity;void* state;unsigned id;int combat,phase;};
static unsigned reads=0;static bool identity=true;static constexpr unsigned kInCombat=0;
static bool CombatActor(const void*a,const void*& e,const void*& s,bool){++reads;auto p=static_cast<const Actor*>(a);e=p->entity;s=p->state;return identity;}
static unsigned EntityId(const void* e){return static_cast<const Actor*>(e)->id;}
static int B(const void* s,unsigned){return static_cast<const Actor*>(s)->combat;}
static int I(const void* s,unsigned){return static_cast<const Actor*>(s)->phase;}
#include "../src/diagnostics/native_victim_watch5916c3.inl"
static unsigned checks=0;static void ck(bool b,const char*m){++checks;if(!b){std::fprintf(stderr,"FAIL %s\n",m);std::exit(1);}}
int main(){namespace c=kcd2vr::diagnostics::capture168;namespace w=victim5916c3;
 Actor a{};a.entity=a.state=&a;a.id=3;a.phase=1;
 w::Start(&a,3,1,1000,0,false);ck(reads==0&&w::count==0,"recording off no target read or watch");
 c::gate.select(c::Domain::Combat,true,1000);w::Start(&a,3,1,1001,0,false);ck(w::count==1,"bounded registered target");
 a.combat=1;a.phase=256;w::Observe(&a,1011);ck(notes.back().find("observedCombat=1")!=std::string::npos,"actual post-update state observed");
 ck(notes.back().find("consumer=UNOBSERVED")!=std::string::npos,"state not mislabeled as event consumer");
 unsigned old=reads;Actor other{};w::Observe(&other,1020);ck(reads==old,"unrelated actor no decode");
 for(unsigned i=0;i<20;++i)w::Observe(&a,1022+i);ck(notes.size()<=5,"bounded transition output");
 c::gate.select(c::Domain::Combat,false,1100);w::Expire(1100);ck(w::count==0,"off clears watches");
 c::gate.select(c::Domain::Combat,true,1200);w::Start(&a,3,2,1201,1,true);identity=false;w::Observe(&a,1211);identity=true;
 ck(w::count==0&&notes.back().find("OWNER_REPLACED")!=std::string::npos,"owner replacement retires without write");
 w::Start(&a,3,3,1300,0,false);w::Expire(2501);ck(w::count==0&&notes.back().find("NO_MATCHED_ACTOR_UPDATE")!=std::string::npos,"no update labelled not forced");
 w::Start(&a,3,4,2600,0,false);auto size=notes.size();c::gate.select(c::Domain::Combat,false,2601);c::gate.select(c::Domain::Combat,true,2602);w::Observe(&a,2611);
 ck(notes.size()==size,"old session cannot publish into new capture");w::Clear();
 std::printf("%u victim observation checks passed; no game setters/calls\n",checks);
}
