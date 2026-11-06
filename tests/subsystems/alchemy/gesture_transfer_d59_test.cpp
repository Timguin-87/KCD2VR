// Real production State::step; native membership, director and selector are
// explicit observations. Use is a request, never a fabricated recipe result.
#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
#include "../../../src/subsystems/alchemy/capture_d42.h"
#include <cstring>

static void boundsD59(Fixture& q,int slot,float x){
 auto& o=q.in.nativeFrame.offers[slot];o={{slot,unsigned(100+slot)},{x,0,0},q.in.now,true};o.boundsValid=true;
 for(int j=0;j<3;++j){o.bounds.local[j]=-.01f;o.bounds.local[j+3]=.01f;o.bounds.world[j*4+j]=1;}
 o.bounds.world[3]=x;
}
static void herbD59(Fixture& q,bool enabled=true,int type=7){
 q.in.proximityCraft1=q.in.handsOnlyD53=true;q.in.gestureTransferD59=enabled;
 q.admit(type,0);q.advance(90);
 auto& table=q.in.nativeFrame.table;table.handTypesKnown68=true;table.handType68[0]=unsigned(type);
 table.actionKnown=true;table.action=q.in.action.parent=900;
 q.step();boundsD59(q,6,.05f);q.selected(6);q.step();q.advance();
}
static void nativeIdleD59(Fixture& q){q.in.action.parent=q.in.nativeFrame.table.action=0;}
static void releaseBusyD59(){
 // D70 mode3 defers even with the old optional switch OFF; never spend Use while busy.
 {Fixture q;herbD59(q,false);q.grip(0,0);const auto o=q.step();check(!o.use&&!o.ordinaryReturnRequested&&o.transferStateD59==2);}
 for(int type=7;type<=9;++type){Fixture q;herbD59(q,true,type);q.grip(0,0);auto o=q.step();
  check(!o.use&&!o.ordinaryReturnRequested&&q.state.ordinaryReleasePending&&o.transferStateD59==2);
  const auto intent=q.state.ordinaryIntentTickD59;
  for(int j=0;j<9;++j){q.advance(20);o=q.step();check(!o.use&&q.state.ordinaryIntentTickD59==intent);}
  q.advance();nativeIdleD59(q);q.selected(6);o=q.step();
  check(o.use&&o.ordinaryReturnRequested&&o.transferStateD59==4&&q.state.ordinaryReturning);
  o=q.step();check(o.use&&!o.ordinaryReturnRequested); // repeated XR callback is not another press
  q.advance(81);o=q.step();check(!o.use&&!o.ordinaryReturnRequested);
  q.advance(300);o=q.step();check(!o.use&&!o.ordinaryReturnRequested);
  q.advance();o=q.step();check(!o.use&&!o.ordinaryReturnRequested); // refused native request is never retried
 }
}
static void recipientHandoffD59(){
 Fixture q;herbD59(q);q.grip(0,0);q.in.nativeSelectedKnown=false;auto o=q.step();
 check(!o.use&&q.state.ordinaryDestination.slot==6);
 // Another object becomes closer before final selection catches up. Keep the
 // same locally valid release recipient; do not give the open hand another use.
 q.advance();boundsD59(q,2,0);nativeIdleD59(q);q.selected(2);o=q.step();
 check(!o.use&&q.state.ordinaryReleasePending&&o.nomination.decision.ref.slot==6&&o.transferStateD59==3);
 q.advance();q.selected(6);o=q.step();check(o.use&&o.ordinaryReturnRequested&&o.nomination.decision.ref.slot==6);
}
static void cancellationsD59(){
 for(int fault=0;fault<8;++fault){Fixture q;herbD59(q);q.grip(0,0);q.step();q.advance();
  switch(fault){
   case 0:q.grip(0,1);break; // user regrip
   case 1:q.in.palm[0].point={1,0,0};break; // leaves actual recipient
   case 2:q.in.gestureTransferD59=false;q.in.handsOnlyD53=false;break; // actual mode leaves built-in mode3 deferral
   case 3:q.in.nativeFrame.offers[6].boundsValid=false;break;
   case 4:q.in.action.parent=q.in.nativeFrame.table.action=901;break; // unrelated native action
   case 5:q.f.hand[0].valid=false;break;
   case 6:q.in.cancel=true;break;
   case 7:q.in.nativeFrame.table.refs[6].entity++;break;
  }
  const auto o=q.step();check(!o.use&&!o.ordinaryReturnRequested&&!q.state.ordinaryReleasePending);
 }
 // Unknown director is not proof of idle; a stale final selected snapshot is
 // not this release's native acknowledgement. Recovery consumes only once.
 {Fixture q;herbD59(q);q.grip(0,0);q.step();q.advance();nativeIdleD59(q);
  q.in.nativeFrame.table.actionKnown=false;check(!q.step().use);
  q.advance();q.in.nativeFrame.table.actionKnown=true;q.selected(6);q.in.nativeSelectionTick=q.state.ordinaryIntentTickD59-1;
  check(!q.step().use);q.advance();q.in.nativeSelectionTick=q.in.now;check(q.step().use);}
}
static void plateD59(){
 Fixture q;q.in.proximityCraft1=q.in.handsOnlyD53=q.in.gestureTransferD59=true;q.admit(0,0);
 q.tool(0,0);q.in.tool[0].binding={1,2,888,777,100,0,0,0,0,666,3};q.step();q.advance(90);
 q.in.action.parent=q.in.nativeFrame.table.action=900;q.in.nativeFrame.table.actionKnown=true;q.step();
 boundsD59(q,6,.05f);q.selected(6);q.step();q.advance();
 q.f.hand[0].rotation[0]=std::sin(.6f);q.f.hand[0].rotation[3]=std::cos(.6f);auto o=q.step();
 check(!o.use&&q.state.ordinaryTransferTiltD59&&o.transferStateD59==2);
 q.advance();nativeIdleD59(q);q.selected(6);o=q.step();check(o.use&&o.transferStateD59==4&&q.state.ordinaryTiltUsedD56);
 q.advance(400);o=q.step();check(!o.use);q.advance();check(!q.step().use);
 // Righting the same plate before native acceptance cancels the intent.
 Fixture z;z.in.proximityCraft1=z.in.handsOnlyD53=z.in.gestureTransferD59=true;z.admit(0,0);
 z.tool(0,0);z.in.tool[0].binding={1,2,888,777,100,0,0,0,0,666,3};z.step();z.advance(90);
 z.in.action.parent=z.in.nativeFrame.table.action=900;z.in.nativeFrame.table.actionKnown=true;z.step();
 boundsD59(z,6,.05f);z.selected(6);z.step();z.advance();z.f.hand[0].rotation[0]=std::sin(.6f);z.f.hand[0].rotation[3]=std::cos(.6f);z.step();
 z.advance();z.f.hand[0].rotation[0]=0;z.f.hand[0].rotation[3]=1;nativeIdleD59(z);o=z.step();
 check(!o.use&&!z.state.ordinaryReleasePending&&o.transferStateD59==5);
}
static void receiptD59(){
 namespace cap=kcd2vr::subsystems::alchemy::capture_d42;
 Fixture q;herbD59(q);q.grip(0,0);auto o=q.step();cap::Alchemy recorder;
 unsigned lines=0;bool busy=false,dispatched=false;
 const auto writer=[&](const char* line){++lines;busy=busy||std::strstr(line,"transferStateD59=2");
  dispatched=dispatched||std::strstr(line,"transferStateD59=4");};
 recorder.record(0,q.in,o,q.state,q.f,{},writer);check(lines==0); // F5 gate remains authoritative
 recorder.record(1,q.in,o,q.state,q.f,{},writer);check(lines==1&&busy);
 recorder.record(1,q.in,o,q.state,q.f,{},writer);check(lines==1); // unchanged input coalesces
 q.advance();nativeIdleD59(q);q.selected(6);o=q.step();
 recorder.record(1,q.in,o,q.state,q.f,{},writer);check(lines==2&&dispatched);
}
int main(){releaseBusyD59();recipientHandoffD59();cancellationsD59();plateD59();receiptD59();
 std::printf("D59 alchemy gesture handoff: %u checks PASS; native requests only, effects remain native\n",checks);return 0;}
