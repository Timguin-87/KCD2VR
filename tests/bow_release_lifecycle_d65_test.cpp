#include "subsystems/ranged_reload/bow_physical_aim_d65c.h"
#include "subsystems/ranged_reload/owned_inline_d65.h"
#include <cstdio>
#include <cstring>
#include <array>
namespace a=kcd2vr::bow_physical_aim_d65c;
namespace o=kcd2vr::ranged_options_d65c;
static int checks=0,failures=0;
#define CHECK(x) do{++checks;if(!(x)){++failures;std::printf("FAIL line%d %s\n",__LINE__,#x);}}while(0)
static bool context=true;
static bool current(const a::Snapshot&) noexcept{return context;}
static void* ptr(unsigned n){return reinterpret_cast<void*>(std::uintptr_t(n));}
static a::Snapshot make(){a::Snapshot s{};s.player=ptr(1);s.ownerInterface=ptr(2);s.launcher=ptr(3);s.key=4;s.generation=5;s.body=6;s.request=7;s.tracking=8;s.sample=9;s.tick=1000;s.dir[1]=1;s.up[2]=1;return s;}
static a::Snapshot bind(void* action=ptr(11)){a::ReleaseScope r(make());a::bindConstructed(action,ptr(2));CHECK(r.finish(true));return r.value;}
int main(){
 a::current=&current;a::hooksReady=true;CHECK(o::bowPhysicalAim.load());CHECK(o::crossbowReload.load());
 // D72 accepted defaults ON; explicitly compare the OFF refusal path.
 o::SetBowPhysicalAim(false);
 a::Snapshot out{};CHECK(a::enter(ptr(11),1010,out)==a::Refusal::Disabled);
 o::SetBowPhysicalAim(true);auto s=bind();CHECK(a::take(ptr(1),ptr(3),out)==a::Refusal::NoRelease);
 CHECK(a::enter(ptr(12),1010,out)==a::Refusal::WrongAction);CHECK(a::enter(ptr(11),1010,out)==a::Refusal::None);
 {a::ShotScope scope(&out);a::Snapshot result{};
  CHECK(a::take(ptr(7),ptr(3),result)==a::Refusal::ActorMismatch);CHECK(a::take(ptr(1),ptr(7),result)==a::Refusal::LauncherMismatch);
  {a::ShotScope unrelated(nullptr);CHECK(a::take(ptr(1),ptr(3),result)==a::Refusal::NoRelease);}
  CHECK(a::take(ptr(1),ptr(3),result)==a::Refusal::None);CHECK(result.action==ptr(11)&&result.key==4);
  CHECK(a::take(ptr(1),ptr(3),result)==a::Refusal::Consumed);
 }
 CHECK(a::take(ptr(1),ptr(3),out)==a::Refusal::NoRelease);
 bind();o::SetBowPhysicalAim(false);o::SetBowPhysicalAim(true);CHECK(a::enter(ptr(11),1020,out)==a::Refusal::NoRelease);
 bind();{auto bad=make();bad.dir[1]=0;a::ReleaseScope r(bad);a::bindConstructed(ptr(12),ptr(2));CHECK(!r.finish(true));}CHECK(a::enter(ptr(11),1020,out)==a::Refusal::NoRelease);
 bind();{a::ReleaseScope r(make());a::bindConstructed(ptr(12),ptr(2));CHECK(!r.finish(false));}CHECK(a::enter(ptr(12),1020,out)==a::Refusal::NoRelease);
 {a::ReleaseScope r(make());CHECK(!r.finish(true));}CHECK(a::enter(ptr(11),1020,out)==a::Refusal::NoRelease);
 bind();context=false;CHECK(a::enter(ptr(11),1020,out)==a::Refusal::ContextLost);context=true;CHECK(a::enter(ptr(11),1020,out)==a::Refusal::NoRelease);
 bind();a::retire();CHECK(a::enter(ptr(11),1020,out)==a::Refusal::NoRelease);
 bind();a::bindConstructed(ptr(11),ptr(99));CHECK(a::enter(ptr(11),1020,out)==a::Refusal::NoRelease); // recycled allocation/native action
 {a::ReleaseScope r(make());a::bindConstructed(ptr(12),ptr(99));CHECK(!r.finish(true));}
 {a::ReleaseScope r(make());a::bindConstructed(ptr(12),ptr(2));a::bindConstructed(ptr(13),ptr(2));CHECK(!r.finish(true));}
 bind();CHECK(a::enter(ptr(11),2500,out)==a::Refusal::None); // bounded release vector, NOT held tracking
 bind();CHECK(a::enter(ptr(11),2501,out)==a::Refusal::Stale);CHECK(a::enter(ptr(11),1020,out)==a::Refusal::NoRelease);
 bind();CHECK(a::enter(ptr(11),999,out)==a::Refusal::Stale);
 a::hooksReady=false;{a::ReleaseScope r(make());a::bindConstructed(ptr(11),ptr(2));CHECK(!r.finish(true));}a::hooksReady=true;
 // Same-thread immediate native execution also finishes correctly; no reused token.
 {a::ReleaseScope r(make());a::bindConstructed(ptr(11),ptr(2));CHECK(a::enter(ptr(11),1001,out)==a::Refusal::None);
  {a::ShotScope scope(&out);a::Snapshot q{};CHECK(a::take(ptr(1),ptr(3),q)==a::Refusal::None);}a::retire(out.serial);CHECK(r.finish(true));}
 CHECK(a::enter(ptr(11),1002,out)==a::Refusal::NoRelease);
 // Retiring an older callback must not cancel a newer exact release.
 auto old=bind();auto next=bind(ptr(12));a::retire(old.serial);CHECK(a::enter(ptr(12),1020,out)==a::Refusal::None);
 // Actual committed jump route: target E9 -> relay FF25 -> our callback.
 std::array<unsigned char,512> mem{};const std::uintptr_t base=0x10000,target=base+16,relay=base+128,detour=0x12345678;
 mem[16]=0xe9;std::int32_t d=std::int32_t(relay-target-5);std::memcpy(mem.data()+17,&d,4);
 mem[128]=0xff;mem[129]=0x25;std::memcpy(mem.data()+134,&detour,8);
 const auto read=[&](std::uintptr_t p,void* out,std::size_t n){if(p<base||p-base+n>mem.size())return false;std::memcpy(out,mem.data()+p-base,n);return true;};
 kcd2vr::owned_inline_d65::Route route;CHECK(route.capture(target,detour,read));CHECK(route.current(read));
 // K42: neighbouring relay allocation bytes are outside the owned branch.
 mem[142]=0x48;mem[143]=0x8b;CHECK(route.current(read));
 mem[21]=0xcc;CHECK(route.current(read));
 mem[16]=0x90;CHECK(!route.current(read));mem[16]=0xe9;CHECK(route.current(read));
 mem[134]^=1;CHECK(!route.current(read));kcd2vr::owned_inline_d65::Route foreign;CHECK(!foreign.capture(target,detour,read));mem[134]^=1;
 CHECK(!foreign.capture(target,detour+1,read));CHECK(!foreign.capture(base+500,detour,read));
 CHECK(!kcd2vr::owned_inline_d65::Route{}.current(read));
 std::printf("D65 bow release lifecycle/owned route %d/%d PASS\n",checks-failures,checks);return failures?1:0;
}
