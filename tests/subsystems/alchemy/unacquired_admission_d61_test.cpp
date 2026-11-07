// Existing D55 frame sequence: unacquired mortar blocks valid plate after
// release/regrip. Executes the production adapter; native facts are doubles.
#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
#ifndef D61_BASELINE
#include "../../../src/subsystems/alchemy/capture_d42.h"
#include <string>
#endif
static void idleD61(Fixture& q){
 auto& t=q.in.nativeFrame.table;t.actionKnown=t.handTypesKnown68=true;
 t.action=q.in.action.parent=0;t.handType68[0]=t.handType68[1]=21;
}
static void waitingD61(Fixture& q,int type=2,int side=0){
 q.in.proximityCraft1=q.in.handsOnlyD53=true;q.admit(type,side);idleD61(q);
 q.advance(300);q.step(); // request pulse expired; native did not accept it
 q.grip(side,0);q.step(); // physical release observed, cannot trust earlier idle
 q.advance();
}
static void recoveredPlateD61(){
 Fixture q;waitingD61(q);auto o=q.step();
 check(!o.admitted&&!o.use); // unchanged D60 must fail here, not at compilation
#ifndef D61_BASELINE
 check(o.unacquiredRetiredD61);
#endif
 for(int j=0;j<4;++j){q.advance();o=q.step();check(!o.admitted&&!o.use);}
 q.in.nativeFrame.offers[2].valid=false;q.in.nativeFrame.offers[0]={{0,100},{0,0,0},q.in.now,true};
 q.grip(0,1);q.advance();q.selected(0);o=q.step();
 if(!o.admitted){q.advance();q.selected(0);o=q.step();}
 check(o.admitted&&q.state.admittedRef.slot==0&&o.use);
 q.advance();q.in.action.parent=q.in.nativeFrame.table.action=900;q.tool(0,0);
 q.in.nativeFrame.table.handType68[0]=0;o=q.step();check(o.admitted&&o.ordinaryHeld);
 q.advance(300);q.in.action.parent=q.in.nativeFrame.table.action=0;o=q.step();
 check(o.admitted&&o.ordinaryHeld&&!o.use); // real attachment stays owned
}
#ifndef D61_BASELINE
static void retainCounterexamplesD61(){
 for(int fault=0;fault<20;++fault){Fixture q;waitingD61(q);
  switch(fault){
   case 0:q.in.nativeFrame.table.handTypesKnown68=false;break;
   case 1:q.in.nativeFrame.table.handType68[0]=0;break;
   case 2:q.in.action.parent=999;break;
   case 3:q.in.action.valid=false;break;
   case 4:q.in.action.tick=q.in.now-81;break;
   case 5:q.in.nativeFrame.table.actionKnown=false;break;
   case 6:q.in.nativeFrame.table.action=999;break;
   case 7:q.in.nativeFrame.table.tick=q.state.unacquiredReleaseTickD61;break;
   case 8:q.tool(0,2);break;
   case 9:q.in.work={1,999,555,3,q.in.now,44,s::Resource,1,2,true,false};break; // independently current work blocks idle retirement
   case 10:q.grip(0,1);break;
   case 11:q.state.previousUse=true;break;
   case 12:q.state.operationParent=999;q.in.action.valid=false;break;
   case 13:q.in.action.tick=q.state.unacquiredReleaseTickD61;break;
   case 14:q.in.nativeFrame.table.sequence=0;break;
   case 15:q.in.currentNativeSequence=q.in.nativeFrame.table.sequence;break;
   case 16:q.f.hand[0].grip=.4f;break;
   case 17:q.f.hand[0].valid=false;break;
   case 18:q.in.palm[0].valid=false;break;
   case 19:q.in.palm[0].tick=q.in.now-81;break;
  }
  auto o=q.step();check(!o.unacquiredRetiredD61&&!o.use&&(fault==14||fault==15||fault==18||fault==19||o.admitted)); // invalid frame retains existing fail-closed retirement
 }
 // Still-held intent has no automatic repeat, however long no native result.
 {Fixture q;q.in.proximityCraft1=q.in.handsOnlyD53=true;q.admit(2,0);idleD61(q);for(int i=0;i<20;++i){q.advance(1000);auto o=q.step();check(o.admitted&&!o.use&&!o.unacquiredRetiredD61);}}
 // A delayed native acknowledgement keeps its original request, even when
 // the user's Grip opened before native attachment arrived.
 {Fixture q;waitingD61(q);q.in.action.parent=q.in.nativeFrame.table.action=900;
  auto o=q.step();check(o.admitted&&!o.use&&q.state.operationParent==900);
  q.advance(1000);o=q.step();check(o.admitted&&!o.unacquiredRetiredD61&&!o.use);}
 // Zero receipts from BEFORE release must not cancel an unacquired request.
 {Fixture q;waitingD61(q);q.in.nativeFrame.table.tick=q.in.action.tick=q.state.unacquiredReleaseTickD61;
  auto o=q.step();check(o.admitted&&!o.unacquiredRetiredD61);}
 // Release does not expire a valid native-owned ordinary tool or logical herb.
 for(int type:{0,7}){Fixture q;q.admit(type,0);idleD61(q);q.advance();
  q.in.nativeFrame.table.handType68[0]=unsigned(type);if(type==0)q.tool(0,0);q.step();
  q.grip(0,0);q.advance(300);auto o=q.step();check(o.admitted&&!o.unacquiredRetiredD61);}
 // The whole pending-admission interval is preserved, with evidence after it.
 {Fixture q;q.in.proximityCraft1=q.in.handsOnlyD53=true;q.admit(2,0);idleD61(q);q.advance(249);q.grip(0,0);auto o=q.step();check(o.admitted);
  q.advance(1);o=q.step();check(o.admitted);q.advance(1);o=q.step();check(!o.admitted&&o.unacquiredRetiredD61);}
 // Diagnostics remain session-gated, emit the actual retirement and empty
 // hand/director proof, and do not claim native completion.
 {Fixture q;waitingD61(q);auto o=q.step();kcd2vr::subsystems::alchemy::capture_d42::Alchemy c;std::string line;
  auto writer=[&](const char* s){line=s;};c.record(0,q.in,o,q.state,q.f,{},writer);check(line.empty());
  c.record(1,q.in,o,q.state,q.f,{},writer);check(line.find("unacquiredRetiredD61=1")!=std::string::npos&&
   line.find("nativeHandsKnown=1 nativeHands=21,21 tableActionKnown=1 tableAction=0")!=std::string::npos&&
   line.find("nativeCompletion=NOT_INFERRED")!=std::string::npos);}
}
#endif
int main(){recoveredPlateD61();
#ifndef D61_BASELINE
 retainCounterexamplesD61();
#endif
 std::printf("unacquired_admission_d61: %u checks PASS (native completion not inferred)\n",checks);return 0;}
