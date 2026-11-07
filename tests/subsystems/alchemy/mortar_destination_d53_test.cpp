// Actual input67 adapter: right-pestle source -> left-mortar destination.
// Copied native identity/selection leaves are explicit; no recipe effect is fabricated.
#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
#include <cstring>
static unsigned checks53=0,failed53=0;
static void expect53(bool ok,const char* why){++checks53;if(!ok){++failed53;std::fprintf(stderr,"D53 FAIL: %s\n",why);}}
static void bounds53(Fixture& q,int slot,double x){
 auto& o=q.in.nativeFrame.offers[slot];o={{slot,unsigned(100+slot)},{x,0,0},q.in.now,true};o.boundsValid=true;
 for(int j=0;j<3;++j){o.bounds.local[j]=-.02f;o.bounds.local[j+3]=.02f;o.bounds.world[j*4+j]=1;}
 o.bounds.world[3]=float(x);
}
static a::Output acquire53(Fixture& q,int sourceSide,bool proximity){
 for(int h=0;h<2;++h)q.f.hand[h].rotation[3]=1;
 q.admit(sourceSide==0?3:2,sourceSide);q.in.proximityCraft1=proximity;
 q.advance(81);q.grip(0,1);q.grip(1,1);bounds53(q,2,0);bounds53(q,3,0);bounds53(q,6,.5);
 for(int h=0;h<2;++h){q.tool(h,h==0?3:2);auto& t=q.in.tool[h];t.parent=999;
  t.binding={1,2,888,std::uintptr_t(800+h),t.entity,h,t.type};t.binding.object=900+h;t.binding.lifetime=3;t.binding.action=999;}
 q.in.action.parent=999;q.in.work={1,999,555,3,q.in.now,44,s::Mortar,3,3,true,false};
 return q.step();
}
int main(){
 for(bool proximity:{false,true})for(int sourceSide:{0,1})for(bool release:{false,true}){
  Fixture q;auto out=acquire53(q,sourceSide,proximity);
  expect53(out.admitted&&q.state.mortarPair69,"actual joined pair preserves the physical source admission");
  expect53(n::same(q.state.selection.context,q.in.context),"deliberate destination handoff preserves native table context");
  if(proximity){q.advance();q.in.palm[1].point={5,0,0};out=q.step();
   expect53(out.admitted&&out.nomination.decision.source==n::Source::Left&&!n::valid(out.nomination.decision.ref),
    "carried mortar's empty destination remains left-owned instead of becoming a distant gaze pickup");}
  q.advance();q.in.palm[1].point={.5,0,0};q.in.palm[0].point={2,0,0};q.selected(6);out=q.step();
  expect53(out.admitted&&out.nomination.decision.source==n::Source::Left&&out.nomination.decision.ref.slot==6,
   "left mortar selects destination even when right pestle Grip remains held");
  q.advance();if(release)q.grip(1,0);else{q.f.hand[1].rotation[2]=.70710678f;q.f.hand[1].rotation[3]=.70710678f;}
  out=q.step();expect53(out.mortarStop69.serial&&out.mortarStop69.parent==999&&!out.use,
   "release or flip requests the existing qualified native stop, never a fabricated effect");
  if(out.mortarStop69.serial){
   q.advance();q.in.work={};q.in.action.parent=0;for(auto& t:q.in.tool){t.parent=0;t.binding.action=0;}
   q.in.mortarResult69={out.mortarStop69.serial,q.in.now,1,3,kcd2vr::subsystems::alchemy::mortar69::Status::ReadyForNativePlace};
   out=q.step();expect53(out.use&&q.state.mortarReturning69,"qualified stop receipt permits the original native contextual PlaceMortar");
  }
 }
 for(int fault=0;fault<5;++fault){Fixture q;auto out=acquire53(q,0,true);q.advance();
  if(fault==0)q.in.tool[0].binding.object++;
  if(fault==1)q.f.sourceEpoch++;
  if(fault==2)q.in.palm[1].valid=false;
  if(fault==3)q.in.nativeFrame.table.refs[3].entity++;
  if(fault==4)q.in.tool[1].joined=false;
  q.in.palm[1].point={.5,0,0};q.selected(6);q.grip(1,0);out=q.step();
  expect53(!out.use&&!out.mortarStop69.serial,"real identity/epoch/palm/source/join loss cannot borrow destination handoff");
 }
 std::printf("mortar_destination_d53: %u checks, %u failures; actual input67, native leaves explicit\n",checks53,failed53);
 return failed53?1:0;
}
