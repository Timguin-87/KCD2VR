// Real source arbitration/input/consumer path; native-selected mailbox is an
// explicit host leaf. These tests do not claim a game action or headset result.
#define main ProximityD44PriorMain
#include "proximity_craft1_test.cpp"
#undef main
static void selected44(Fixture& q,int type){
 q.selected(type);kcd2vr::alchemy_input19c4::selected.publish({q.in.context.instance,q.in.context.lifetime,q.in.now,unsigned(100+type),unsigned(type)});
}
static void water44(Fixture& q){
 policy1(q);q.in.pickupReachD39=0;kcd2vr::subsystems::alchemy::reach_craft1::setPickupReachD39(0);
 box1(q,16,0);palm1(q,1,.077294);palm1(q,0,3);selected44(q,16);
}
int main(){
 {Fixture q;water44(q);auto out=q.step();
  expect1(!n::valid(out.nomination.decision.ref)&&!out.use,"native selection does not widen idle contact hover");
  q.grip(1,1);out=q.step();
  expect1(out.nomination.decision.ref.slot==16&&out.nomination.decision.selectedPickupD44,"same native water becomes nominee on deliberate left Grip at recorded7.23cm");
  expect1(out.admitted&&out.use&&owner1(q,out,16)==1,"producer and consumer independently join current native selected water");
  refresh1(q,81);out=q.step();expect1(!out.use,"one native release-hint pulse only");}
 for(int fault=0;fault<10;++fault){Fixture q;water44(q);q.grip(1,1);
  switch(fault){
   case 0:selected44(q,15);break;
   case 1:q.in.nativeSelected.tick=q.in.now-81;break;
   case 2:q.in.nativeSelectedKnown=false;break;
   case 3:palm1(q,1,.206);break;
   case 4:q.grip(1,.54f);break;
   case 5:q.in.palm[1].valid=false;break;
   case 6:q.in.nativeFrame.offers[16].tick=q.in.now-81;break;
   case 7:q.in.nativeFrame.table.refs[16].entity++;break;
   case 8:q.in.context.poseEpoch++;break;
   case 9:q.grip(1,0);q.grip(0,1);palm1(q,0,.077294);palm1(q,1,3);break;
  }
  auto out=q.step();expect1(!out.admitted&&!out.use,"different/stale/unregistered/outside/wrong-hand source cannot borrow local native selection");
 }
 for(int fault=0;fault<6;++fault){Fixture q;water44(q);q.grip(1,1);auto out=q.step();publish1(q,out);
  if(fault==0)kcd2vr::alchemy_input19c4::selected.publish({1,3,q.in.now,115,15});
  if(fault==1)kcd2vr::alchemy_input19c4::selected.publish({1,3,q.in.now-81,116,16});
  if(fault==2)kcd2vr::alchemy_input19c4::selected.publish({1,4,q.in.now,116,16});
  if(fault==3)liveFrame1.hand[1].grip=.54f;
  if(fault==4){auto o=q.in.nativeFrame.offers[16];o.bounds.world[3]=2;b1::offers[16].publish({1,2,3,o});}
  if(fault==5)kcd2vr::alchemy_input19c4::selected.publish({1,3,q.in.now,116,15});
  bool chosen=false;(void)b1::selection(2,116,q.in.now,chosen);
  expect1(!chosen,"consumer rechecks current native selection, Grip, identity and bounds before redirecting selector");
 }
 // Native selected is unnecessary for the original genuine contact hover.
 {Fixture q;water44(q);q.in.nativeSelectedKnown=false;palm1(q,1,.025);q.grip(1,0);auto out=q.step();
  expect1(out.nomination.decision.ref.slot==16&&!out.nomination.decision.selectedPickupD44&&owner1(q,out,16)==1,"original contact path retained");}
 std::printf("D44 selected pickup: %u checks, %u failed; engine leaves explicit\n",total1,bad1);return bad1?1:0;
}
