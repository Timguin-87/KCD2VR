// Real production input state machine. Native attachments/action observations
// are explicit fixture inputs; no recipe completion is invented by the tests.
#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
#include <cstring>

static unsigned repairChecks=0,repairFailures=0;
static void expectRepair(bool ok,const char* message){
 ++repairChecks;if(!ok){++repairFailures;std::fprintf(stderr,"FAIL: %s\n",message);}
}
static void pairRepair(Fixture& q){
 q.in.controllerMortar70=true;q.in.nativeFrame.offers[14].valid=false;
 for(int slot:{2,6}){
  auto& offer=q.in.nativeFrame.offers[slot];offer={{slot,unsigned(100+slot)},{0,0,0},q.in.now,true};
  offer.boundsValid=true;for(int j=0;j<3;++j){offer.bounds.local[j]=-.01f;offer.bounds.local[j+3]=.01f;}
  offer.bounds.world[0]=offer.bounds.world[5]=offer.bounds.world[10]=1;offer.bounds.world[3]=slot==6?.52f:0.f;
 }
 q.in.palm[1].point={0,0,0};q.in.palm[0].point={2,0,0};q.grip(1,1);q.step();q.advance();q.selected(2);
 check(q.step().admitted&&q.state.mortarHome70.valid);
 q.grip(0,1);q.tool(0,3);q.tool(1,2);for(int h=0;h<2;++h){auto& t=q.in.tool[h];
  t.binding={1,2,888,std::uintptr_t(800+h),t.entity,h,t.type};t.binding.object=900+h;t.binding.lifetime=3;}
 q.step();q.advance(81);q.step();
}
static void ordinaryDelayedAttach(){
 Fixture q;q.admit(0,0);q.advance();q.grip(0,0);q.step();
 q.advance(100);q.tool(0,0);q.selected(0);auto o=q.step();
 expectRepair(o.ordinaryReturnRequested&&o.use,"opening Grip before native attach retains the return request");
 q.advance(100);o=q.step();expectRepair(!o.use,"delayed release emits one finite native input, not a held repeat");
 q.advance(300);o=q.step();expectRepair(!o.use,"an unacknowledged native return is not retried automatically");
}
static void ordinarySelectorLag(){
 Fixture q;q.admit(0,0);q.tool(0,0);q.step();q.advance(100);
 q.grip(0,0);q.in.nativeSelectedKnown=false;auto o=q.step();
 expectRepair(!o.use,"a missing native target cannot be treated as a completed return");
 q.advance();q.selected(0);o=q.step();
 expectRepair(o.ordinaryReturnRequested&&o.use,"same open Grip survives a one-frame native selector delay");
}
static void ordinaryRegripCancels(){
 Fixture q;q.admit(0,0);q.advance();q.grip(0,0);q.step();
 q.advance();q.grip(0,1);q.step();q.advance();q.tool(0,0);q.selected(0);auto o=q.step();
 expectRepair(!o.ordinaryReturnRequested&&o.ordinaryHeld,"a deliberate regrip cancels a release that has not been dispatched");
}
static void ordinaryDestinationStaysPinned(){
 Fixture q;q.admit(5,0);q.tool(0,5);q.step();q.advance(100);
 q.in.nativeFrame.offers[6].point={.05,0,0};q.selected(6);q.step();q.advance();
 q.grip(0,0);q.in.nativeSelectedKnown=false;auto o=q.step();
 expectRepair(!o.use&&q.state.ordinaryReleasePending,"a near physical destination remains pending during native observation lag");
 q.advance();q.in.nativeFrame.offers[6].point={1,0,0};q.selected(5);o=q.step();
 expectRepair(!o.use&&!q.state.ordinaryReleasePending,"moving the release target away cancels instead of retargeting the open hand");
 q.advance();q.in.nativeFrame.offers[6].point={.05,0,0};q.selected(6);o=q.step();
 expectRepair(!o.use,"returning to a cancelled destination does not synthesize another release edge");
}
static void mortarReleasedDish(){
 Fixture q;pairRepair(q);q.advance();
 q.in.nativeFrame.offers[0]=q.in.nativeFrame.offers[6];
 q.in.nativeFrame.offers[0].ref={0,100};q.in.nativeFrame.offers[0].bounds.world[3]=.52f;
 q.in.nativeFrame.offers[6].bounds.world[3]=1.5f;
 q.in.palm[1].point={.5,0,0};q.selected(0);q.step();q.advance();q.grip(1,0);auto o=q.step();
 expectRepair(o.use&&q.state.mortarReturning69,"release over the dish admits exactly the native placement action");
 q.advance();q.in.action.parent=777;q.in.action.placeMortar69=true;
 for(auto& t:q.in.tool){t.parent=777;t.binding.action=777;}
 q.in.work={1,777,888,3,q.in.now,66,s::MortarPlace70,0,3,true,false};
 o=q.step();q.advance(20);o=q.step();
 expectRepair(o.semiControl.totalSeconds>.019&&!o.passenger&&o.control.trackedPresentation71,
              "release at the dish earns placement without squeezing Grip again or playing the arm animation");
 const auto before=o.semiControl.totalSeconds;
 q.advance(20);q.in.work.phase=1;
 // Home tracks the pot's real transform; it moved by .98m in this fixture.
 q.in.palm[1].point={.98,0,0};o=q.step();
 expectRepair(o.semiControl.totalSeconds>before&&!o.passenger&&o.control.trackedPresentation71,
              "an open hand at native home keeps tracked placement until the real detach marker");
 q.advance(20);q.in.work.phase=2;o=q.step();
 expectRepair(o.semiControl.manualUse,"actual native detach resumes cleanup without a synthetic completion");
}
int main(int argc,char** argv){
 if(argc==1||!std::strcmp(argv[1],"ordinary")){ordinaryDelayedAttach();ordinarySelectorLag();ordinaryRegripCancels();ordinaryDestinationStaysPinned();}
 if(argc==1||!std::strcmp(argv[1],"mortar"))mortarReleasedDish();
 std::printf("physical_release_repair: %u checks, %u failures\n",repairChecks,repairFailures);
 return repairFailures?1:0;
}
