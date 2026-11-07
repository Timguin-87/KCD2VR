// Procedural foot contact never substitutes an admitted native action.
// Even a legacy ON setting and historical macro leave the real production drain observation-only.
#include "../src/vrik/kick_hit34.h"
#include "../src/vrik/kick_style26.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
static unsigned long long now=1200;
static auto GetTickCount64(){return now;}
namespace diagnostics::capture168 {enum class Domain{Combat};static bool active(Domain,unsigned long long){return true;}}
namespace diagnostic_trace {static std::vector<std::string> lines;static void Write(const char* s){lines.emplace_back(s);}}
static const void* henry=reinterpret_cast<void*>(0x1000);
static bool CombatActor(void* a,const void*& e,const void*& s,bool){e=a;s=reinterpret_cast<void*>(0x2000);return a!=nullptr;}
static bool IsPlayer(const void* e){return e==henry;}
#define KCD2VR_KICK_APPLY37C2 1
namespace geom27 {static int applied=0;static void KickApply37c2(void*) noexcept {++applied;(void)kcd2vr::kick34::ledger.clear();}}
#include "../src/diagnostics/kick_drain37.inl"
int main(){
 namespace k=kcd2vr::kick34;auto& l=k::ledger;l.clear();
 assert(!kcd2vr::kick26::realHitTest37c2.load()); // default OFF
 k::Binding b{};b.serial=1;b.tick=1000;b.poseEpoch=7;b.actor=0x1000;b.state=0x2000;b.player=0x3000;
 k::Contact c{};c.serial=1;c.tick=1150;c.poseEpoch=7;c.entity=44;c.extend=c.living=true;
 assert(l.open(b)&&l.offer(c,1160)==k::Why::None);
 geom27::KickDrain37(const_cast<void*>(henry));
 assert(geom27::applied==0&&diagnostic_trace::lines.size()==1&&diagnostic_trace::lines[0].find("damage=NONE stamina=NONE")!=std::string::npos);
 kcd2vr::kick26::realHitTest37c2.store(true);
 ++b.serial;++c.serial;assert(l.open(b)&&l.offer(c,1160)==k::Why::None);
 geom27::KickDrain37(const_cast<void*>(henry));
 assert(geom27::applied==0&&diagnostic_trace::lines.size()==2&&diagnostic_trace::lines.back().find("damage=NONE stamina=NONE")!=std::string::npos);
 geom27::KickDrain37(const_cast<void*>(henry)); // idle: nothing pending -> no second apply
 assert(geom27::applied==0&&diagnostic_trace::lines.size()==2);
 std::puts("PASS kick contact drain: default and legacy ON observe only, one receipt per contact, no direct hit/cost");
}
