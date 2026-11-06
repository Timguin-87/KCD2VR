#define main H18PreviousReplayUnusedMain
#include "impale47_test.cpp"
#undef main
#include "subsystems/gore/impale_successor_h18.h"
namespace h18=kcd2vr::impale_successor_h18;
int main(){
 h18::Source owner{1,2,3,4,5,6,2};h18::Wait wait;
 ck(wait.begin(7,1000,0,owner,{0,1,0},{0,1,0}),"immutable native victim wait admitted");
 ck(!wait.current(5499,owner,{0,1,0},true,true),"last millisecond inside original4500ms");
 ck(wait.current(5500,owner,{0,1,0},true,true)!=nullptr,"original4500ms expires before a new contact");
 ck(wait.current(999,owner,{0,1,0},true,true)!=nullptr,"clock rewind retires victim wait");
 ck(wait.current(1100,owner,{0,.74,0},true,true)!=nullptr,"actual blade withdrawal over25cm cancels");
 ck(!wait.current(1100,owner,{.3,1,0},true,true),"victim motion cannot itself count as blade withdrawal");
 ck(wait.current(1100,owner,{0,1,0},false,true)!=nullptr,"drop/sheathe cannot retain wait");
 ck(wait.current(1100,owner,{0,1,0},true,false)!=nullptr,"tracking invalid cannot retain wait");
 for(int n=0;n<7;++n){auto changed=owner;
  if(n==0)++changed.key;if(n==1)++changed.generation;if(n==2)++changed.epoch;if(n==3)++changed.request;
  if(n==4)++changed.tracking;if(n==5)++changed.recovery;if(n==6)changed.policy+=2;
  ck(wait.current(1100,changed,{0,1,0},true,true)!=nullptr,"any original weapon/source epoch change refuses");}
 // Actual physical kernel, no new hit/damage/command path is added by waiting.
 Replay r(true);r.in.receipt58c1=r.receipt(801,r.tip());r.move({});ck(r.out.impaleLatchRequest,"actual weapon admits original native lethal receipt");
 r.rest(3.1);ck(r.out.impaleLatchRequest&&r.out.impaleSerial==801,"actual weapon still accepts first pin after3s successor transition");
 r.rest(1.7);ck(!r.out.impaleLatchRequest,"actual weapon stops at original4700ms without renewed receipt");
 Replay current(true);current.move({});auto contactOut=current.out;
 auto& c=contactOut.contact119[0];c.known=c.plane=true;c.fraction=.5;
 c.point=point(contactOut.weapons[0],current.profiles[0].tip*.8);
 auto& w=c.native172;w.mask=kcd2vr::npc_contact172::BodiesAndWorld;w.body=kcd2vr::npc_contact172::Body::Articulated;
 w.ownerRead=true;w.owner=0x420;w.entityId=42;w.collider=0x990;w.part=379;
 ck(h18::currentBladeContact(current.in,contactOut,current.profiles[0],0),"NEW current checked contact lies on actual allowed blade");
 for(int n=0;n<14;++n){auto out=contactOut;auto in=current.in;
  if(n==0)out.contact119[0].point.x+=.1;if(n==1)out.contact119[0].limited115=true;
  if(n==2)out.contact119[0].known=false;if(n==3)out.contact119[0].plane=false;
  if(n==4)out.contact119[0].fraction=1;if(n==5)out.contact119[0].native172.part=-1;
  if(n==6)out.sweepDeferred115[0]=true;if(n==7)out.handlingOnly55c1[0]=true;
  if(n==8)out.solverOwnedT4[0]=true;if(n==9)out.ownShieldBlocked60[0]=true;
  if(n==10)out.active[0]=false;if(n==11)out.contactKnown[0]=false;
  if(n==12)in.contact=false;if(n==13)in.tracked[0]=false;
  ck(!h18::currentBladeContact(in,out,current.profiles[0],0),"unknown/partial/off-blade/recovery/solver/untracked contact refuses");}
 std::printf("impale_successor_h18: %u checks passed\n",checks);
}
