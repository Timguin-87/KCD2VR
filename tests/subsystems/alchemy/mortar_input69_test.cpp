// Execute the actual production input owner, reusing its native/tracking
// fixture. Both genuine tools + a current final native destination are required.
#define main priorInput67Main
#include "input67_test.cpp"
#undef main
static void pair69(Fixture& q,bool active=true){
 q.admit(2,1);q.grip(0,1);q.tool(0,3);q.tool(1,2);
 for(int h=0;h<2;++h){auto& t=q.in.tool[h];t.parent=active?999:0;
  t.binding={1,2,888,std::uintptr_t(800+h),t.entity,h,t.type};t.binding.object=900+h;t.binding.lifetime=3;t.binding.action=t.parent;}
 q.in.action.parent=active?999:0;
 if(active)q.in.work={1,999,555,3,q.in.now,44,s::Mortar,3,3,true,false};
 q.step();q.advance(81);q.step();
}
#ifndef KCD2VR_MORTAR69_NO_MAIN
int main(){
 Fixture q;pair69(q);q.advance();q.in.palm[1].point={.5,0,0};q.in.nativeFrame.offers[6].point={.52,0,0};q.selected(6);
 auto o=q.step();check(o.nomination.decision.source==n::Source::Left&&o.nomination.decision.ref.slot==6);
 q.advance();q.grip(1,0);o=q.step();check(o.mortarStop69.serial&&o.semiControl.manualUse&&!o.use);
 const auto serial=o.mortarStop69.serial;const auto req=o.mortarStop69;
 q.advance();q.in.work={};q.in.action.parent=0;for(auto& t:q.in.tool){t.parent=0;t.binding.action=0;}
 q.in.mortarResult69={serial,q.in.now,1,3,kcd2vr::subsystems::alchemy::mortar69::Status::ReadyForNativePlace};
 o=q.step();check(o.use&&o.passenger&&q.state.mortarReturning69&&!o.control.held[0]&&!o.control.held[1]);
 q.advance();q.in.action.parent=777;q.in.action.placeMortar69=true;o=q.step();check(o.passenger&&!o.use&&q.state.mortarReturnParent69==777);
 q.advance();q.in.action.parent=0;q.in.nativeFrame.table.handTypesKnown68=true;q.in.nativeFrame.table.handType68={21,21};
 o=q.step();check(o.mortarPlaced69&&!o.admitted&&!o.use);
 q.advance();o=q.step();check(!o.use&&!o.mortarStop69.serial);
 // A different native parent is never a completed mortar placement.
 Fixture foreign;pair69(foreign,false);foreign.advance();foreign.in.palm[1].point={.5,0,0};
 foreign.in.nativeFrame.offers[0]={{0,100},{.52,0,0},foreign.in.now,true};foreign.selected(0);foreign.step();foreign.advance();foreign.grip(1,0);foreign.step();
 foreign.advance();foreign.in.action.parent=777;foreign.in.action.placeMortar69=false;o=foreign.step();check(!foreign.state.mortarReturnParent69&&!o.mortarPlaced69);
 foreign.advance();foreign.in.action.parent=0;foreign.in.nativeFrame.table.handTypesKnown68=true;foreign.in.nativeFrame.table.handType68={21,21};
 o=foreign.step();check(!o.mortarPlaced69);
 // Natural work end keeps the real pair carried rather than discarding it.
 Fixture carried;pair69(carried);carried.advance();carried.in.work={};carried.in.action.parent=0;
 for(auto& t:carried.in.tool){t.parent=0;t.binding.action=0;}
 o=carried.step();check(o.admitted&&o.control.held[0]&&o.control.held[1]&&o.control.parent==0&&!o.use);
 // A stale selected target, distant palm, changed source or missing hand must
 // not emit a native placement (nor synthesize a processed ingredient).
 for(int fault=0;fault<5;++fault){Fixture v;pair69(v);v.advance();v.in.palm[1].point={.5,0,0};v.in.nativeFrame.offers[6].point={.52,0,0};v.selected(6);v.step();v.advance();
  switch(fault){case 0:v.in.nativeSelectedKnown=false;break;case 1:v.in.palm[1].point={1,0,0};break;
   case 2:v.in.tool[1].joined=false;break;case 3:++v.f.sourceEpoch;break;case 4:v.in.nativeSelected.entity=9999;break;}
  v.grip(1,0);o=v.step();check(!o.use&&!o.mortarStop69.serial);}
 // Held mortar2 is NOT a legal PlaceMortar target. Actual native0 is.
 Fixture back;pair69(back,false);back.advance();back.in.palm[1].point={.5,0,0};
 back.in.nativeFrame.offers[0]={{0,100},{.52,0,0},back.in.now,true};back.selected(0);back.step();
 back.advance();back.grip(1,0);o=back.step();check(o.use&&o.passenger&&back.state.mortarDestination69.slot==0);
 Fixture notHome;pair69(notHome,false);notHome.advance();notHome.selected(2);notHome.grip(1,0);o=notHome.step();
 check(!o.use&&!o.mortarStop69.serial);
 std::printf("mortar input69: %u assertions PASS\n",checks);return 0;
}

#endif
