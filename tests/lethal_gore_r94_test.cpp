// Actual candidate production prefix. Native observations are leaves; no attachment/game writes.
#include "vrik/contact_receipt58c1.h"
#include "subsystems/gore/gore_policy.h"
#include <cstdio>
#include <cstring>
static std::uint64_t GetTickCount64(){return 1000;}
namespace diagnostic_trace{static unsigned lines=0;static void Write(const char*){++lines;}}
namespace impulse596{template<class T>static void NoteNativeDeath56(const T&,std::uint64_t,std::uint64_t,std::uint64_t){}}
struct Pending {struct Metadata{kcd2vr::gore_cut_d56::Evidence goreCutD56;}metadata59c7g;
 std::uint64_t gesture59c3=1,key=2,generation=3,epoch=4,request=5,sample=6,tick=1000,recovery55c1=1;};
#include "actual_lethal_gore_r94.inc"
static unsigned checks=0,failures=0;
static void ck(bool x,const char* w){++checks;if(!x){++failures;std::printf("FAIL %s\n",w);}}
int main(){namespace g=kcd2vr::gore32;namespace cut=kcd2vr::gore_cut_d56;
 Pending p{};p.metadata59c7g.goreCutD56={1,2,3,4,5,6,1000,.4,.5,12,13,1.2,0,true};
 kcd2vr::contact58c1::Receipt r{};r.serial=100;r.tick=1000;r.attempt=1;r.target=42;r.targetIdentity=0x123;r.collider=0x234;r.rawPart=7;
 r.key=2;r.generation=3;r.epoch=4;r.request=5;r.continuation=r.seen=r.accepted=r.applied=r.fields=r.bodyContact=true;
 r.kind=0;r.feature=int(kcd2vr::bodycombo59c7g::Feature::Blade);r.weaponClass=4;r.blocked=r.perfect=r.parry=0;r.damage=10;r.bodyMaterial=16;
 g::decap=true;g::decapMinPower=.9f;kcd2vr::gorepak50::dismember=true;
 g::CutCandidate sentinel{};sentinel.serial=77;g::lethalCut.publish(sentinel);r.death=0;decap59c7e::Evaluate(p,r,16,32);g::CutCandidate got{};
 ck(g::lethalCut.read(got)&&got.serial==77,"living limb never overwrites queued lethal cut");ck(!g::KnownDead(42,0x123,1000),"living contact never creates death proof");
 r.death=1;decap59c7e::Evaluate(p,r,16,32);ck(g::lethalCut.read(got)&&got.serial==100&&got.nativeLethal42&&!got.nonLethalLimb60,"native killing full stroke still publishes");
 ck(g::KnownDead(42,0x123,1000),"actual native receipt still proves death");
 for(int negative=0;negative<5;++negative){auto q=r;auto x=p;g::lethalCut.publish(sentinel);
  if(negative==0)q.blocked=1;if(negative==1)q.applied=false;if(negative==2)x.metadata59c7g.goreCutD56.power=.1;
  if(negative==3)x.metadata59c7g.goreCutD56.gesture=9;if(negative==4)q.kind=1;
  decap59c7e::Evaluate(x,q,16,32);ck(g::lethalCut.read(got)&&got.serial==77,"native/gesture exclusions retained");}
 r.death=0;r.serial=200;g::lethalCut.publish(sentinel);
 decap59c7e::Evaluate(p,r,16,32);ck(g::lethalCut.read(got)&&got.serial==77,"living lane remains unpublished on repeated receipt");
 std::printf("lethal_gore_r94 %u/%u\n",checks-failures,checks);return failures?1:0;
}
