#include "../src/vrik/kick_hit34.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
static unsigned long long now=1200;
static auto GetTickCount64(){return now;}
namespace diagnostics::capture168 {enum class Domain{Combat};static bool enabled=true;static bool active(Domain,unsigned long long){return enabled;}}
namespace diagnostic_trace {static std::vector<std::string> lines;static void Write(const char* s){lines.emplace_back(s);}}
static const void* henry=reinterpret_cast<void*>(0x1000);
static bool CombatActor(void* a,const void*& e,const void*& s,bool){e=a;s=reinterpret_cast<void*>(0x2000);return a!=nullptr;}
static bool IsPlayer(const void* e){return e==henry;}
#include "../src/diagnostics/kick_drain37.inl"
int main(){
 namespace k=kcd2vr::kick34;auto& l=k::ledger;l.clear();
 k::Binding b{};b.serial=1;b.tick=1000;b.poseEpoch=7;b.actor=0x1000;b.state=0x2000;b.player=0x3000;
 k::Contact c{};c.serial=1;c.tick=1150;c.poseEpoch=7;c.entity=44;c.extend=c.living=true;
 assert(l.open(b)&&l.offer(c,1160)==k::Why::None);
 geom27::KickDrain37(reinterpret_cast<void*>(0x5000));assert(l.pending());
 geom27::KickDrain37(const_cast<void*>(henry));assert(!l.pending()&&l.counters().claimed==1&&l.counters().applied==0);
 assert(l.stage()==k::Stage::Refused&&diagnostic_trace::lines.size()==1);
 assert(diagnostic_trace::lines[0].find("nativeAdmission=UNPROVEN damage=NONE stamina=NONE")!=std::string::npos);
 geom27::KickDrain37(const_cast<void*>(henry));assert(diagnostic_trace::lines.size()==1);
 ++b.serial;++c.serial;assert(l.open(b)&&l.offer(c,1160)==k::Why::None);
 diagnostics::capture168::enabled=false;geom27::KickDrain37(const_cast<void*>(henry));
 assert(l.counters().applied==0&&diagnostic_trace::lines.size()==1);
 std::puts("PASS actual kick drain: exact owner, once-only observation, no stamina/damage native leaves");
}
