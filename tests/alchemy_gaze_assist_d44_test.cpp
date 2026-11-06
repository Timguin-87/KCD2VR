// Real input owner and native-selection bridge; native final selection and
// native geometry/attachment publications are explicit fixture evidence.
#define main inherited_acquisition_owner_d44
#include "subsystems/alchemy/acquisition_owner_h5_test.cpp"
#undef main
static unsigned checksD44=0;
static void requireD44(bool value,const char* reason){
 ++checksD44;if(!value){std::fprintf(stderr,"FAIL D44 %s\n",reason);std::exit(1);}
}
static void setupD44(Fixture& q,int mode=1){
 beginH5(q);q.in.proximityCraft1=true;q.in.pickupReachD39=mode;
 kcd2vr::subsystems::alchemy::reach_craft1::setPickupReachD39(mode);
 offerH5(q,13,.65);pointH5(q,1,0);pointH5(q,0,2);q.grip(1,1);q.selected(13);
 q.in.nativeSelectionTick=q.in.now;
}
static void nextD44(Fixture& q){q.advance();q.in.nativeSelectionTick=q.in.now;}
int main(){
 {Fixture q;setupD44(q);auto out=q.step();
  requireD44(!out.use&&!out.admitted&&out.nomination.decision.source==n::Source::Gaze&&
             !out.nomination.decision.gripSelectionOwnedCraft1,
             "assisted empty hand yields actual native gaze before any distant request");
  requireD44(nativeSourceH5(q,out,13)==-1,"native source bridge does not create a distant hand ray or facing override");
  nextD44(q);out=q.step();
  requireD44(out.admitted&&out.use&&q.state.admittedSide==1&&q.state.admittedRef.slot==13&&
             !out.control.held[0]&&!out.control.held[1],
             "fresh native-selected carafe can request native Use with LEFT Grip, without fabricated ownership");
  nextD44(q);out=q.step();
  requireD44(out.use&&out.nomination.decision.source==n::Source::Gaze,
             "native selected source stays available through the accepted input pulse and delayed Attach");
  nextD44(q);q.selected(14);out=q.step();
  requireD44(q.state.admittedRef.slot==13&&!out.use,"later gaze cannot redirect an already admitted native operation");
 }
 {Fixture q;setupD44(q,0);q.step();nextD44(q);auto out=q.step();
  requireD44(!out.admitted&&!out.use&&out.nomination.decision.source==n::Source::Left&&
             out.nomination.decision.gripSelectionOwnedCraft1,
             "explicit saved CloseBounds preserves physical-only empty-hand selection");}
 {Fixture q;setupD44(q);offerH5(q,14,.04);q.selected(13);auto out=q.step();
  requireD44(!out.use&&!out.admitted&&out.nomination.decision.ref.slot==14,
             "real nearby source wins before a different native gaze target");}
 {Fixture q;setupD44(q);q.grip(1,0);q.grip(0,1);q.step();nextD44(q);auto out=q.step();
  requireD44(!out.use&&!out.admitted,"RIGHT Grip cannot acquire the game's LEFT-only carafe");
  q.grip(1,1);nextD44(q);out=q.step();
  requireD44(out.admitted&&q.state.admittedSide==1,"two closed hands still admit the exact native eligible hand");}
 for(int fault=0;fault<15;++fault){Fixture q;setupD44(q);q.step();nextD44(q);
  switch(fault){
   case 0:q.in.nativeSelectedKnown=false;break;
   case 1:q.in.nativeSelected.entity++;break;
   case 2:q.in.nativeSelected.type=14;break;
   case 3:q.in.nativeSelected.tick=q.in.now-81;break;
   case 4:q.in.nativeSelectionTick=1000;break;
   case 5:q.in.nativeSelectionTick=q.in.now+1;break;
   case 6:q.in.nativeFrame.table.refs[13].entity++;break;
   case 7:q.in.nativeFrame.offers[13].boundsValid=false;break;
   case 8:q.in.nativeFrame.offers[13].tick=q.in.now-81;break;
   case 9:q.f.hand[1].valid=false;break;
   case 10:q.f.hand[1].grip=.54f;break;
   case 11:q.in.palm[1].sourceEpoch++;break;
   case 12:q.in.nativeFrame.table.tick=q.in.now-81;break;
   case 13:q.tool(1,13);break;
   case 14:q.in.context.tableGeneration++;break;
  }
  auto out=q.step();requireD44(!out.admitted&&!out.use&&!out.control.held[1],
     "stale/current-source/geometry/topology/hand/context or already-held mismatch cannot grant gaze acquisition");
 }
 {Fixture q;setupD44(q);q.step();q.in.qualified=false;nextD44(q);q.step();
  q.in.qualified=true;nextD44(q);q.step();nextD44(q);auto out=q.step();
  requireD44(!out.admitted&&!out.use,"retired source generation quarantines the still-held Grip");
  q.grip(1,0);nextD44(q);q.step();q.grip(1,1);nextD44(q);q.step();nextD44(q);out=q.step();
  requireD44(out.admitted&&out.use,"deliberate fresh hand input can acquire after actual source re-entry");}
 {Fixture q;setupD44(q);q.in.nativeA=true;auto out=q.step();
  requireD44(out.passenger&&!out.admitted&&!out.use,"explicit native A retains original passenger behavior");}
 std::printf("alchemy_gaze_assist_d44: %u checks PASS; real native selection bridge/input, no game effect/attachment inferred\n",checksD44);
}
