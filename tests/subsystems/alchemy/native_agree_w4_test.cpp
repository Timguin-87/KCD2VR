// R8b (replaces W4): the HAND's item wins. the game's own selection never changes what the hand nominates; the hand's
// nominee is steered into the native selector (interaction_native_selection68.inl) and the squeeze admits only once the
// native selection IS the hand's item. + steering WHY (bridge67::steerWhy68) and the type-only mounted bellows selection.
#include "../../../src/subsystems/alchemy/near_selection67.h"
#include "../../../src/subsystems/alchemy/bridge67.h"
#include "../../../src/vrik/alchemy_input19c4.h"
#include <cstdio>
namespace n=kcd2vr::subsystems::alchemy::near67;
namespace b=kcd2vr::subsystems::alchemy::bridge67;
namespace al=kcd2vr::alchemy_input19c4;
static unsigned total=0,failed=0;static void check(bool v,const char* m){++total;if(!v)++failed;std::printf("%s %s\n",v?"PASS":"FAIL",m);}
static n::Context context(){return {1,2,3,4,5,6,7};}
static n::Completed frame(){n::Completed f{};f.context=context();f.tick=1000;f.sequence=10;f.complete=true;f.count=2;
 f.table[0]={0,100};f.table[1]={1,101};f.offers[0]={{0,100},{.02,0,0},1000,true};
 f.offers[1]={{1,101},{.06,0,0},1000,true};return f;}
static n::Input input(){n::Input in{};in.context=context();in.now=1000;in.currentFrameSequence=11;
 in.palms[0]={{0,0,0},1000,4,5,6,0,0,true};in.palms[1]={{3,0,0},1000,4,5,6,1,0,true};return in;}
int main(){
 {auto f=frame();auto in=input();n::State s{};auto d=s.step(f,in);
  check(d.ref.entity==100&&!d.nativeAgreeW4,"no native selection: nearest object (2 cm) nominated");}
 {auto f=frame();auto in=input();in.selected={in.context,{1,101},in.now,-1,true};n::State s{};auto d=s.step(f,in);
  check(d.ref.entity==100&&!d.nativeAgreeW4,"game selected the 6 cm object (also in reach): hand KEEPS its 2 cm item (no substitution)");
  in.palms[0].grip=1;d=s.step(f,in);
  check(d.ref.entity==100&&!d.admissionReady,"squeeze while the game still selects another item: not admitted");
  in.selected={in.context,{0,100},in.now,-1,true};d=s.step(f,in);
  check(d.ref.entity==100&&d.gripping&&d.admissionReady,"selection followed the hand (steered): squeeze admits the hand's item");}
 {auto f=frame();f.offers[1].point={.5,0,0};auto in=input();in.selected={in.context,{1,101},in.now,-1,true};n::State s{};auto d=s.step(f,in);
  check(d.ref.entity==100&&!d.nativeAgreeW4,"game selection out of reach: hand keeps its own nearest object");}
 {auto f=frame();auto in=input();n::State s{};in.palms[0].grip=1;auto d=s.step(f,in);check(d.gripping&&d.ref.entity==100,"latched on own object");
  in.selected={in.context,{1,101},in.now,-1,true};d=s.step(f,in);check(d.ref.entity==100&&!d.admissionReady,"after a latch the game cannot swap the held object");}
 // steering WHY
 check(b::steerWhy68(0,0,0,0,1000)==b::Steer68::Unknown,"no nominee: unknown");
 check(b::steerWhy68(952233,0,0,0,1000)==b::Steer68::NeverCandidate,"bellows never a native candidate (session 7 case)");
 check(b::steerWhy68(952233,952224,990,0,1000)==b::Steer68::NeverCandidate,"another item was the last candidate: never offered");
 check(b::steerWhy68(952233,952233,990,0,1000)==b::Steer68::CandidateNotFacing,"candidate seen, facing not forced");
 check(b::steerWhy68(952233,952233,990,995,1000)==b::Steer68::ForcedButRefused,"candidate + forced, native still picked other");
 check(b::steerWhy68(952233,952233,500,500,1000)==b::Steer68::NeverCandidate,"stale receipts (500 ms) count as never offered");
 // mounted bellows: native selects TYPE 18 with entity 0
 {al::selected.publish(al::Selection{0x1000,7,1000,0,18});
  check(al::selectedTypeOnly(0x1000,7,1010)==18u,"type-only selection 18 read (entity 0)");
  check(al::selectedTypeOnly(0x1000,8,1010)==21u,"other lifetime: none");
  check(al::selectedTypeOnly(0x1000,7,1200)==21u,"older than 80 ms: none");
  al::Observation o{};check(!al::lookupSelected(0x1000,7,1010,o),"entity lookup still refuses entity 0");
  al::selected.publish(al::Selection{0x1000,7,1000,952224,1});
  check(al::selectedTypeOnly(0x1000,7,1010)==21u,"an entity selection is not type-only");}
 std::printf("native_agree_w4 %u/%u\n",total-failed,total);return failed?1:0;}
