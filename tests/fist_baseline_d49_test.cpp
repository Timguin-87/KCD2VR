// Compare the OFF path with the exact, independently extracted user-D48 header.
#include "vrik/fist_motion.h"
#include "fist_baseline_d49.h"
#include <cstdio>
namespace now=kcd2vr::fists;
namespace old=kcd2vr::fistsBaselineD49;
static unsigned failed=0,total=0;
int main(){
 now::State ns{};old::State os{};now::Sample n{};old::Sample o{};
 n.epoch=n.sequence=1;n.tick=1000;n.actor=123;n.valid=n.empty[0]=n.empty[1]=true;n.head={0,1.6,0};n.controller[0]=n.allowed[0]={.2,1.2,-.3};n.controller[1]=n.allowed[1]={-.2,1.2,-.3};
 for(unsigned i=0;i<2400;++i){
  ++n.sequence;n.tick+=i%61==0?0:10;n.grip[0]=n.trigger[0]=(i%100>12)?1.:0.;n.grip[1]=n.trigger[1]=(i%140>70)?1.:0.;
  n.controller[0].z+=(i%100<45?-.009:.007);n.allowed[0]=n.controller[0];n.controller[1].y+=(i%140<100?.007:-.008);n.allowed[1]=n.controller[1];
  n.empty[1]=i%313!=0;n.valid=i%397!=0;n.blocked[0]=i%79>72;n.freshContact174[0]=i%79==75;n.contactStop175=true;n.responsiveCombatH27=i>1200;
  if(i%277==0)++n.epoch;if(i%229==0){n.head.z-=.02;n.controller[0].z-=.02;n.allowed[0].z-=.02;}
  o.epoch=n.epoch;o.sequence=n.sequence;o.tick=n.tick;o.actor=n.actor;o.headOrientation=n.headOrientation;o.head=n.head;o.valid=n.valid;o.boxing167=n.boxing167;o.contactStop175=n.contactStop175;o.openPalm44=n.openPalm44;o.responsiveCombatH27=n.responsiveCombatH27;o.timeNs=n.timeNs;
  for(int h=0;h<2;++h){o.controller[h]=n.controller[h];o.allowed[h]=n.allowed[h];o.grip[h]=n.grip[h];o.trigger[h]=n.trigger[h];o.empty[h]=n.empty[h];o.blocked[h]=n.blocked[h];o.claimedT24[h]=n.claimedT24[h];o.freshContact174[h]=n.freshContact174[h];}
  now::Trace nt{};old::Trace ot{};const auto a=now::step(ns,n,&nt);const auto b=old::step(os,o,&ot);
  bool same=a.valid==b.valid&&a.side==b.side&&a.sequence==b.sequence&&a.tick==b.tick&&a.epoch==b.epoch&&a.actor==b.actor&&a.speed==b.speed&&a.travel==b.travel&&a.motion167==b.motion167&&nt.invalidInputs==ot.invalidInputs&&nt.valid==ot.valid&&nt.continuity==ot.continuity;
  for(int h=0;h<2;++h)same=same&&unsigned(ns.hand[h].phase)==unsigned(os.hand[h].phase)&&ns.hand[h].released==os.hand[h].released&&ns.hand[h].start==os.hand[h].start&&ns.hand[h].quiet==os.hand[h].quiet&&nt.hand[h].reject==ot.hand[h].reject&&nt.hand[h].qualified==ot.hand[h].qualified;
  ++total;if(!same){++failed;std::printf("FAIL off-path divergence sample %u\n",i);}
 }
 std::printf("D49 OFF vs exact user-D48 baseline: %u/%u matched input decisions; no native execution\n",total-failed,total);return failed?1:0;
}
