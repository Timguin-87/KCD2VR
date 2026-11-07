// Actual extracted adapter/caller. Native rays and clocks are synthetic leaves.
#define KCD2VR_WORLD_ADAPTER_CALLER_ONLY_D56 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_CALLER_ONLY_D56
namespace sh=kcd2vr::station_hand_contact_d75;
static HandContact6695 SeedD75(bool station=true){
 HandContact6695 c{};c.have=true;c.epoch=1;c.character=reinterpret_cast<void*>(character);
 c.stationPermissiveD75=station;c.featureCount=5;c.worldAllowed={};return c;
}
static bool QueryD75(HandContact6695& c,V3 request,V3& out,bool station=true){
 V3 points[5]{};return ContactHand6695(c,reinterpret_cast<void*>(character),1,{0,0,0},request,out,points,nullptr,nullptr,station);
}
// Exact production final-admission include, after simulated host constraints.
static void FinalD75(SchedulingStateH24& s,HandContact6695* stagedContact,V3* goals,const V3* rawGoalsH50,
    const bool* freeContact164R1,bool stationContactD75=true,bool passenger=false,float handover=0){
 const bool nativePassengerHand46[2]{passenger,passenger};const float stagedHandoverD47[2]{handover,handover};
 for(int side=0;side<2;++side){
#include "../src/vrik/station_hand_admission_d75.inl"
 }
}
int main(){
 Check(sh::selected(true,false,true),"only a current owned station with contacts selects the policy");
 Check(!sh::selected(false,false,true)&&!sh::selected(true,true,true)&&!sh::selected(true,false,false),"ordinary/no owner, native reference and no contacts keep old policy");
 Check(!sh::samePublication(0,7,0,7)&&!sh::samePublication(1,0,1,0)&&!sh::samePublication(1,7,2,7)&&!sh::samePublication(1,7,1,8),"zero/replaced sequence/tracking cannot retain a wrist");
 for(bool station:{false,true}){
  Init();mockMode=4;auto c=SeedD75(station);std::uint64_t previous=0;V3 out{};
  {ContactPass6695 pass(true,1000,previous,1,7);
   Check(QueryD75(c,{.3f,0,0},out,station)&&c.blocked&&c.fresh169r1,"real current surface still constrains free hand");
   Check(std::abs(out.x-(station?.248f:.242f))<.00002f,"station positive2mm versus unchanged ordinary8mm clearance");
   Check(mockCalls==5&&c.featureCount==5,"five full temporal palm rays; travel not shortened");
   Check(station?!c.releaseActiveD45:c.releaseActiveD45,"station contact does not start a release/drag filter");
  }
  const auto first=out;const int queries=mockCalls;
  {ContactPass6695 pass(true,1000,previous,1,7);
   Check(QueryD75(c,{.3f,0,0},out,station)&&std::abs(out.x-first.x)<1e-6f&&mockCalls==queries,"same source callbacks reuse the completed endpoint with no duplicate rays");
  }
  mockMode=0;
  {ContactPass6695 pass(true,1011,previous,2,7);
   Check(QueryD75(c,{.29f,0,0},out,station)&&c.fresh169r1&&!c.blocked,"new clear query releases measured obstruction");
   if(station)Check(out.x==.29f,"station returns immediately to controller, no old release lag");
  }
 }
 // A failed new sample cannot reuse a same-tick old budget/sample identity.
 Init();mockMode=4;auto c=SeedD75();std::uint64_t previous=0;V3 out{};
 {ContactPass6695 pass(true,1000,previous,1,7);Check(QueryD75(c,{.3f,0,0},out),"seed measured station surface");}
 {ContactPass6695 pass(true,1000,previous,2,7);g_contact_pass6695=false;g_contact_budget_sample65=1;
  Check(!QueryD75(c,{.31f,0,0},out)&&out.x==.31f&&!c.have&&!c.planeCount&&!c.fresh169r1,"new publication with no query follows controller without false clearance or80ms freeze");}
 // Actual mandatory-stage exhaustion: one hit then insufficient credits.
 Init();mockMode=4;c=SeedD75();previous=0;
 {ContactPass6695 pass(true,1000,previous,1,7);g_contact_budget6695.remaining=1;
  Check(!QueryD75(c,{.3f,0,0},out)&&out.x==.3f&&!c.fresh169r1&&!c.lastCompleteTickH18,"incomplete station ray set never fabricates completed geometry or keeps native seed");}
 // No accepted display origin: no sweep from Henry's unrelated authored reach.
 Init();c={};previous=0;
 {ContactPass6695 pass(true,1000,previous,1,7);
  Check(!QueryD75(c,{.2f,.3f,0},out)&&out.x==.2f&&out.y==.3f&&mockCalls==0&&!c.fresh169r1,"entry follows controller without claiming unqueried clearance");
  ContactCommit6695(c,reinterpret_cast<void*>(character),out,1000);
 }
 {ContactPass6695 pass(true,1011,previous,2,7);
  Check(QueryD75(c,{.21f,.3f,0},out)&&out.x==.21f&&mockCalls==5&&c.fresh169r1,"next sample sweeps from committed displayed wrist");}
 // A layout reset must not erase the station policy before pending completion.
 Init();c=SeedD75();c.featureCount=0;c.presentationWorldD53={};c.presentationTickD53=990;c.presentationTrackingD53=7;previous=0;mockMode=4;
 {ContactPass6695 pass(true,1000,previous,1,7);
  Check(QueryD75(c,{.3f,0,0},out)&&c.stationPermissiveD75&&c.stationSampleD75==1&&c.stationTrackingD75==7&&!c.releaseActiveD45,
   "retiring another feature layout preserves current station policy and receipt");
 }
 // Exit restores original policy and clearance; no station epoch leaks.
 mockMode=4;wallPoint={.25f,0,0};
 {ContactPass6695 pass(true,1022,previous,3,7);
  Check(QueryD75(c,{.3f,.3f,0},out,false)&&!c.stationPermissiveD75&&std::abs(out.x-.242f)<.00002f,"station exit restores ordinary contact policy");}
 // Execute the actual free-hand dispatcher: Performance in stations remains bounded measured palm contact,
 // occupied hands remain outside it, no optional recovery consumes extra rays.
 for(bool performance:{false,true})for(bool occupied:{false,true}){
  Init();std::uint64_t last=0;SchedulingStateH24 state{};HandContact6695 hands[2]{SeedD75(),SeedD75()};
  SchedulingTrackingH24 t{};t.tick=1000;t.sequence=1;const bool valid[2]{true,true},free[2]{!occupied,!occupied};V3 goals[2]{{.1f,0,0},{.1f,0,0}};
  for(auto& point:schedulingOffsetsH24)point={};schedulingGeometryH24={};
  {ContactPass6695 pass(true,1000,last,1,7);SchedulingCallerH24(state,hands,goals,t,true,valid,free,1,performance,false,nullptr,nullptr,true);}
  Check(mockCalls==(!occupied?10:0),"actual station caller runs at most5+5 measured palms and keeps Performance measured and respects held owners");
  if(!occupied)Check(hands[0].fresh169r1&&hands[1].fresh169r1&&hands[0].stationPermissiveD75&&hands[1].stationPermissiveD75,"both station hands complete within shared budget");
 }
 // D79 mode2 now performs bounded world contact; explicit no-contact remains off.
 for(bool collision:{false,true}){
  Init();std::uint64_t last=0;SchedulingStateH24 state{};HandContact6695 hands[2]{SeedD75(false),SeedD75(false)};
  SchedulingTrackingH24 t{};t.tick=1000;t.sequence=1;const bool valid[2]{true,true},free[2]{true,true};V3 goals[2]{{.1f,0,0},{.1f,0,0}};
  {ContactPass6695 pass(true,1000,last,1,7);SchedulingCallerH24(state,hands,goals,t,collision,valid,free,1,true,false,nullptr,nullptr,false);}
  Check(mockCalls==(collision?14:0),"balanced mode measures7+7 features; explicit contacts OFF stays off");
 }
 // Both native dispatcher completion routes and native world scale feed final controller admission.
 for(bool fair:{false,true})for(float scale:{.5f,1.f,2.f}){
  Init();kcd2vr::held_motion_d67::setContactFairness(fair);mockMode=4;wallPoint={.25f*scale,0,0};
  const float transform[]{0,0,0,1,0,0,0,scale};Store(character+0xA08,transform);
  SchedulingStateH24 state{};HandContact6695 hands[2]{SeedD75(),SeedD75()};std::uint64_t last=0;
  SchedulingTrackingH24 t{};t.tick=1000;t.sequence=1;const bool valid[2]{true,true},free[2]{true,true};
  const V3 raw[2]{{.3f,0,0},{.26f,0,0}};V3 goals[2]{raw[0],raw[1]};
  {ContactPass6695 pass(true,1000,last,1,7);SchedulingCallerH24(state,hands,goals,t,true,valid,free,1,false,false,nullptr,nullptr,true);}
  Check(mockCalls==10&&hands[0].fresh169r1&&hands[1].fresh169r1,"both actual completion routes produce measured contacts before final admission");
  const V3 measured[2]{goals[0],goals[1]};
  const V3 outside[2]{hands[0].worldAllowed,hands[1].worldAllowed};
  FinalD75(state,hands,goals,raw,free);
  for(int side=0;side<2;++side){
   const float delta=measured[side].x-raw[side].x;
   const float expected=raw[side].x+std::clamp(delta,-.020f/scale,.020f/scale);
   Check(std::abs(goals[side].x-expected)<.00002f&&hands[side].fresh169r1,
       "station presentation continuously caps current measured correction at20mm world scale");
   Check(Len(Sub(hands[side].worldAllowed,outside[side]))<.000001f,
       "bounded display cannot replace the measured outside sweep origin");
  }
  const V3 displayed[2]{goals[0],goals[1]};
  goals[0]=raw[0];goals[1]=raw[1];
  {ContactPass6695 pass(true,1000,last,1,7);SchedulingCallerH24(state,hands,goals,t,true,valid,free,1,false,false,nullptr,nullptr,true);}
  FinalD75(state,hands,goals,raw,free);
  Check(Len(Sub(goals[0],displayed[0]))<.00002f&&Len(Sub(goals[1],displayed[1]))<.00002f,
      "same source callback retains the same capped display without renewing geometry");
 }
 kcd2vr::held_motion_d67::setContactFairness(true);
 // TOTAL self/body/native-host correction is bounded too, not merely the final world-query delta.
 for(int protectedOwner=0;protectedOwner<4;++protectedOwner){
  SchedulingStateH24 state{};HandContact6695 hands[2]{SeedD75(),SeedD75()};
  for(auto& hand:hands){hand.stationWorldScaleD75=1;hand.fresh169r1=true;}
  const bool free[2]{protectedOwner!=1,protectedOwner!=1};const V3 raw[2]{{.1f,0,0},{.1f,0,0}};V3 goals[2]{{.4f,0,0},{.4f,0,0}};
  FinalD75(state,hands,goals,raw,free,protectedOwner!=2,protectedOwner==3);
  Check(protectedOwner?goals[0].x==.4f:(std::abs(goals[0].x-.12f)<.00001f&&hands[0].fresh169r1&&hands[0].stationPresentationLimitedD81),"total endpoint corrected after all constraints; occupied/ordinary/native passenger owners preserved");
 }
 Check(sh::admitCorrection(.02,1)&&!sh::admitCorrection(.0201,1)&&!sh::admitCorrection(.01,0)&&!sh::admitCorrection(NAN,1),"final correction limit rejects invalid scales/positions without native reads");
 std::printf("D75_STATION_HAND_CONTACT %d/%d; engine/headset=NOT_EXECUTED\n",assertions-failures,assertions);return failures?1:0;
}
