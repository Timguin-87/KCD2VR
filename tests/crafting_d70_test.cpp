#define KCD2VR_MORTAR69_NO_MAIN
#include "subsystems/alchemy/mortar_input69_test.cpp"
static void box70(Fixture& q,int slot,double x,double y=0){
 auto& o=q.in.nativeFrame.offers[slot];o={{slot,unsigned(100+slot)},{x,y,0},q.in.now,true};o.boundsValid=true;
 for(int j=0;j<3;++j){o.bounds.local[j]=-.01f;o.bounds.local[j+3]=.01f;o.bounds.world[j*4+j]=1;}
 o.bounds.world[3]=float(x);o.bounds.world[7]=float(y);
}
static void aim70(Fixture& q){for(int h=0;h<2;++h)q.in.aimH21[h]={{1,0,0},q.in.palm[h].tick,4,5,6,h,true};}
static void turn70(Fixture& q,int side){q.f.hand[side].rotation[0]=std::sin(.6f);q.f.hand[side].rotation[3]=std::cos(.6f);}
int main(){
 // Current native geometry: forward20cm, not a20cm sphere. The same helper
 // is rechecked by the live selector before any native candidate override.
 {Fixture q;box70(q,4,.15);q.in.palm[0].point={0,0,0};aim70(q);
  auto& o=q.in.nativeFrame.offers[4];check(n::palmCorridorD70(o,q.in.palm[0],q.in.aimH21[0],q.in.now));
  for(double y:{.060001,.10,.19}){box70(q,4,.05,y);check(!n::palmCorridorD70(o,q.in.palm[0],q.in.aimH21[0],q.in.now));}
  box70(q,4,.05,.059);check(n::palmCorridorD70(o,q.in.palm[0],q.in.aimH21[0],q.in.now));
  box70(q,4,-.05);check(!n::palmCorridorD70(o,q.in.palm[0],q.in.aimH21[0],q.in.now));
  box70(q,4,.22);check(!n::palmCorridorD70(o,q.in.palm[0],q.in.aimH21[0],q.in.now));
  box70(q,4,.15);q.in.aimH21[0].tick--;check(!n::palmCorridorD70(o,q.in.palm[0],q.in.aimH21[0],q.in.now));
 }
 {Fixture q;q.in.proximityCraft1=q.in.handsOnlyD53=q.in.palmRayD70=true;
  for(auto& o:q.in.nativeFrame.offers)o.valid=false;box70(q,4,.15);box70(q,0,.01,.10);
  q.in.palm[0].point={0,0,0};q.in.palm[1].point={2,0,0};aim70(q);
  auto o=q.step();check(o.nomination.decision.ref.slot==4&&o.nomination.decision.palmRayD70&&!o.use);
  q.advance();aim70(q);q.in.aimH21[0].direction={-1,0,0};o=q.step();check(!n::valid(o.nomination.decision.ref));
 }
 // Plate is bound to either actual native hand, including recorded LEFT.
 // Busy pickup retains one local intent; never spends the only pulse early.
 for(int side:{0,1}){Fixture q;q.in.proximityCraft1=q.in.handsOnlyD53=true;q.in.gestureTransferD59=false;
  q.in.nativeFrame.table.actionKnown=true;q.admit(0,side);q.tool(side,0);
  q.in.tool[side].binding={1,2,888,777,100,side,0,0,0,666,3};q.step();q.advance(90);
  q.in.action.parent=q.in.nativeFrame.table.action=900;q.step();box70(q,6,.15);q.selected(6);q.step();q.advance();turn70(q,side);
  auto o=q.step();check(!o.use&&q.state.ordinaryTransferTiltD59&&o.transferStateD59==2);
  q.advance();q.in.action.parent=q.in.nativeFrame.table.action=0;q.selected(6);o=q.step();
  check(o.use&&o.ordinaryReturnRequested&&q.state.ordinaryTiltUsedD56);
  q.advance(90);o=q.step();check(!o.use&&!o.ordinaryReturnRequested);
 }
 // Mortar destination within the actual mode3 region must not be nominated
 // at15cm but silently rejected by a second hidden10cm check.
 {Fixture q;pair69(q);q.in.proximityCraft1=q.in.handsOnlyD53=true;q.advance();
  box70(q,6,.65);q.in.palm[1].point={.5,0,0};q.selected(6);q.step();q.advance();turn70(q,1);
  const auto o=q.step();check(o.mortarStop69.serial&&!o.use&&q.state.mortarDestination69.slot==6);
 }
 // Exact left flask leaves RIGHT retort selection available, without an
 // invented right-hand attachment, flask release, or semantic effect.
 {Fixture q;q.in.proximityCraft1=q.in.handsOnlyD53=true;q.admit(5,1);q.tool(1,5);
  q.step();q.advance(90);box70(q,4,2.05);q.grip(0,1);q.selected(4);
  auto o=q.step();check(o.nomination.decision.source==n::Source::Right&&o.nomination.decision.ref.slot==4);
  check(o.control.held[1]&&!o.control.held[0]&&!o.use&&q.state.admittedRef.slot==5);
  q.advance();q.in.tool[1].tick=q.in.now-81;o=q.step();check(o.nomination.decision.ref.slot!=4);
 }
 // Recorded WORLD-palm publication loss: no pose/use/work during loss, but
 // an exact native pair may resume on current palms without another pickup.
 {Fixture q;pair69(q);q.advance();q.in.palm[1].valid=false;auto o=q.step();
  check(q.state.admitted&&o.admitted&&!o.use&&!o.control.held[0]&&!o.control.held[1]&&o.semiControl.manualUse);
  q.advance(200);o=q.step();check(q.state.admitted&&!o.use&&!o.control.held[1]); // no pose grace extended
  q.advance();q.in.palm[1].valid=true;q.in.work={};q.in.action.parent=0;
  for(auto& t:q.in.tool){t.parent=0;t.binding.action=0;}
  o=q.step();check(o.admitted&&o.control.held[1]&&!o.use);
 }
 for(int fault=0;fault<4;++fault){Fixture q;pair69(q);q.advance();q.in.palm[1].valid=false;
  if(fault==0)q.in.tool[1].binding.object++;
  if(fault==1)q.in.tool[1].tick=q.in.now-81;
  if(fault==2)q.f.sourceEpoch++;
  if(fault==3)q.in.action.parent++;
  auto o=q.step();check(!o.use&&!o.control.held[0]&&!o.control.held[1]&&!q.state.admitted);
 }
 std::printf("D70 actual crafting owner: %u assertions PASS; no native effects synthesized\n",checks);
}
