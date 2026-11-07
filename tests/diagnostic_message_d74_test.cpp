#include "../src/vrik/visible_failure.h"
#include <cassert>
#include <cstdio>
// Actual extracted caller branches; only recipe text/holster report formatters
// are explicit fixture leaves. No combat/native eligibility or Lua executes.
namespace kcd2vr::gold_fixture_d74 {
namespace vf=kcd2vr::visible_failure;namespace cap=kcd2vr::diagnostics::capture168;
struct Recipe{const char* id="recipe";int steps[1]{0};};
static Recipe recipe;struct Pick{Recipe* recipe;unsigned step=0,required=3;int zone=2;};
static Pick pick{&recipe};
namespace cz {const char* zoneText(int){return "head";}const char* kindText(int){return "stab";}}
namespace hd {struct Diag{};struct Attempt{};static unsigned queries=0;
 bool visible_due(Diag&,std::uint64_t){++queries;return true;}
 void visible_text(const Attempt&,char* out,std::size_t n){std::snprintf(out,n,"Holster found");}}
static hd::Diag diag;static hd::Attempt a;
namespace ca {enum class Stage{Missing,Count};}
static struct Report{ca::Stage firstMissing=ca::Stage::Missing;}r;
static const char* why="alchemy: waiting for native result";
#include "generated/gold_callers_d74.inl"
int run(){
 vf::lastDispatched.store(0);vf::Message m{};
 const auto count=[](){return vf::nextSerial.load();};
 auto before=count();ComboGoldD74(1000);HolsterGoldD74(true,1000);CraftGoldD74(true,1000);assert(count()==before);
 cap::gate.select(cap::Domain::Combat,true,1000);
 ComboGoldD74(1001);assert(count()==before); // F3 alone records combat quietly
 cap::gate.select(cap::Domain::Interaction,true,1010);
 HolsterGoldD74(true,1011);CraftGoldD74(true,1011);
 assert(count()==before&&hd::queries==0); // F5 (+F3) alone emits no gold message
 cap::gate.select(cap::Domain::Bug,true,1020);
 ComboGoldD74(1021);assert(count()==++before);
 assert(vf::mailbox.read(m)&&m.diagnosticDomainD74==cap::Domain::Bug&&vf::diagnosticAllowedD74(m,1022));
 HolsterGoldD74(true,1023);assert(count()==++before&&hd::queries==1);
 assert(vf::mailbox.read(m)&&m.diagnosticDomainD74==cap::Domain::Bug);
 CraftGoldD74(true,1024);assert(count()==++before);
 assert(vf::mailbox.read(m)&&m.diagnosticDomainD74==cap::Domain::Bug);
 cap::gate.select(cap::Domain::Bug,false,1025);
 assert(!vf::diagnosticAllowedD74(m,1026));
 ComboGoldD74(1026);HolsterGoldD74(true,1026);CraftGoldD74(true,1026);assert(count()==before);
 cap::gate.select(cap::Domain::Bug,true,1027);
 assert(!vf::diagnosticAllowedD74(m,1028)); // restarted F2 cannot revive old notice
 cap::gate.select(cap::Domain::Interaction,false,1028);
 CraftGoldD74(true,1029);assert(count()==before); // F5 attempt still needs F5 file producer
 ComboGoldD74(1029);assert(count()==++before);
 assert(vf::mailbox.read(m)&&!vf::diagnosticAllowedD74(m,1027+cap::Duration));
 cap::gate.mask.store(0);
 assert(vf::publish("The game refused to stow this item",1030));
 assert(vf::mailbox.read(m)&&vf::diagnosticAllowedD74(m,1031));
 std::puts("D74 actual gold callers PASS: F3/F5 alone silent; F2 lease required; stop/restart/expiry discard; gameplay refusals preserved; no native Lua executed");
 return 0;
}
}
int main(){return kcd2vr::gold_fixture_d74::run();}
