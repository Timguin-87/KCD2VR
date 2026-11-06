// Current State::step requests the existing native actions exactly once.
// No ingredient, sound, particle or native action completion is synthesized.
#define KCD2VR_MORTAR69_NO_MAIN
#include "subsystems/alchemy/mortar_input69_test.cpp"
static void boundsD56(Fixture& q,int slot,float x){
 auto& o=q.in.nativeFrame.offers[slot];o={{slot,unsigned(100+slot)},{x,0,0},q.in.now,true};o.boundsValid=true;
 for(unsigned j=0;j<3;++j){o.bounds.local[j]=-.01f;o.bounds.local[j+3]=.01f;o.bounds.world[j*4+j]=1;}o.bounds.world[3]=x;
}
static void tiltD56(Fixture& q,int side,float radians=1.2f){q.f.hand[side].rotation[0]=std::sin(radians*.5f);q.f.hand[side].rotation[3]=std::cos(radians*.5f);}
static void plateD56(Fixture& q,int destination){
 q.in.proximityCraft1=q.in.handsOnlyD53=true;q.in.nativeFrame.table.actionKnown=true;
 q.admit(0,0);q.tool(0,0);q.in.tool[0].binding={1,2,888,777,100,0,0,0,0,666,3};
 q.step();q.advance(81);boundsD56(q,destination,.05f);q.selected(destination);q.step();q.advance();
}
static void plateCasesD56(){
 for(int destination:{2,6}){Fixture q;plateD56(q,destination);tiltD56(q,0);auto o=q.step();
  check(o.use&&o.ordinaryReturnRequested&&q.state.ordinaryTiltUsedD56&&q.state.ordinaryReturning);
  const auto at=q.state.ordinaryReturnTick;o=q.step();check(o.use&&!o.ordinaryReturnRequested&&q.state.ordinaryReturnTick==at);
  q.advance(81);o=q.step();check(!o.use&&q.state.ordinaryReturning);
  q.advance(300);o=q.step();check(!o.use&&!q.state.ordinaryReturning&&q.state.ordinaryTiltUsedD56);
  q.advance();o=q.step();check(!o.use&&!o.ordinaryReturnRequested); // failed native request cannot spam while tilted
 }
 for(int fault=0;fault<10;++fault){Fixture q;plateD56(q,6);tiltD56(q,0);
  switch(fault){case 0:q.in.nativeSelectedKnown=false;break;case 1:q.in.palm[0].point={1,0,0};break;
   case 2:q.in.tool[0].joined=false;break;case 3:q.in.tool[0].binding.object=0;break;
   case 4:q.in.tool[0].binding.type=1;break;case 5:q.in.action.parent=999;break;
   case 6:q.in.nativeFrame.table.actionKnown=false;break;case 7:q.in.handsOnlyD53=false;break;
   case 8:tiltD56(q,0,.1f);break;case 9:q.f.hand[0].valid=false;break;}
  auto o=q.step();check(!o.use&&!o.ordinaryReturnRequested);
 }
}
static void pairPourD56(Fixture& q){
 q.in.proximityCraft1=q.in.handsOnlyD53=q.in.controllerMortar70=true;q.in.nativeFrame.offers[14].valid=false;
 boundsD56(q,2,0);boundsD56(q,6,.52f);q.in.palm[1].point={0,0,0};q.in.palm[0].point={2,0,0};
 q.grip(1,1);q.step();q.advance();q.selected(2);check(q.step().admitted&&q.state.mortarHome70.valid);
 q.grip(0,1);q.tool(0,3);q.tool(1,2);
 for(int h=0;h<2;++h){auto& t=q.in.tool[h];t.binding={1,2,888,std::uintptr_t(800+h),t.entity,h,t.type};t.binding.object=900+h;t.binding.lifetime=3;}
 q.step();q.advance(81);q.step();
}
static void mortarCasesD56(){
 // Active grind first goes through the already-qualified Stop transaction.
 for(int destination:{0,6}){Fixture q;pair69(q);q.in.proximityCraft1=q.in.handsOnlyD53=true;
  q.advance();boundsD56(q,destination,.85f);q.in.palm[1].point={.85,0,0};q.selected(destination);q.step();q.advance();tiltD56(q,1);
  const auto o=q.step();check(o.mortarStop69.serial&&!o.use&&q.state.mortarDestination69.slot==destination);
 }
 Fixture q;pairPourD56(q);q.advance();boundsD56(q,0,.85f);q.in.palm[1].point={.85,0,0};q.selected(0);q.step();q.advance();tiltD56(q,1);
 auto o=q.step();check(o.use&&q.state.mortarReturning69&&q.state.mortarTiltToDishD56&&!o.passenger);
 q.advance();q.in.action.parent=777;q.in.action.placeMortar69=true;
 for(auto& t:q.in.tool){t.parent=777;t.binding.action=777;}
 q.in.work={1,777,888,3,q.in.now,66,s::MortarPlace70,0,3,true,false};o=q.step();check(o.semiControl.totalSeconds==0);
 q.advance(20);o=q.step();check(o.gesture.status==g::Status::Pouring&&o.semiControl.totalSeconds>.019&&!o.use);
 const auto earned=o.semiControl.totalSeconds;o=q.step();check(o.semiControl.totalSeconds==earned);
 q.advance(20);tiltD56(q,1,0);o=q.step();check(o.semiControl.totalSeconds==earned&&o.semiControl.clockStop);
 q.advance(20);tiltD56(q,1);q.in.work.phase=1;o=q.step();check(o.semiControl.totalSeconds==earned&&o.semiControl.clockStop);
 q.advance(20);tiltD56(q,1,0);q.in.palm[1].point={0,0,0};o=q.step();check(o.semiControl.totalSeconds>earned&&!o.semiControl.clockStop);
 q.advance();q.in.work.phase=2;o=q.step();check(o.semiControl.manualUse); // real native detach owns cleanup
}
int main(){plateCasesD56();mortarCasesD56();std::printf("D56 alchemy tilt: %u checks PASS; native transfers/effects remain native\n",checks);}
