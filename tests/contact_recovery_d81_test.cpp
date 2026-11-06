// Current production contact adapter/caller and final station admission.
// Native geometry/memory and clocks are explicit synthetic leaves.
#define KCD2VR_WORLD_ADAPTER_CALLER_ONLY_D56 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_CALLER_ONLY_D56
static void FinalD81(SchedulingStateH24& s,HandContact6695* stagedContact,V3* goals,const V3* rawGoalsH50,
                     const bool* freeContact164R1){
 const bool stationContactD75=true,nativePassengerHand46[2]{};const float stagedHandoverD47[2]{};
 for(int side=0;side<2;++side){
#include "../src/vrik/station_hand_admission_d75.inl"
 }
}
int main(){
 for(float scale:{.5f,1.f,2.f}){
  Init();mockMode=4;wallPoint={.25f*scale,0,0};
  const float transform[]{0,0,0,1,0,0,0,scale};Store(character+0xA08,transform);
  HandContact6695 c[2]{};SchedulingStateH24 s{};V3 f[5]{},out{};std::uint64_t previous=0;
  const auto ch=reinterpret_cast<void*>(character);const bool free[2]{true,false};
  c[0].have=true;c[0].epoch=1;c[0].character=ch;c[0].stationPermissiveD75=true;
  c[0].worldAllowed={.20f*scale,0,0};c[0].featureCount=5;
  V3 preceding{};bool havePreceding=false;
  for(unsigned n=0;n<30;++n){
   const float penetration=.0195f+.0001f*n;
   // Contact skin is two model-local millimetres scaled by the native model;
   // penetration is an independently measured world-space distance.
   V3 raw[2]{{.25f+penetration/scale-.002f,.01f*float(n),0},{}};
   ContactPass6695 pass(true,1000+11*n,previous,n+1,7,1000000000ull+11000000ull*n);
   Check(ContactHand6695(c[0],ch,1,{0,0,0},raw[0],out,f,nullptr,nullptr,true)&&c[0].blocked,
       "each current station pass obtains a real outside surface");
   const V3 measured=c[0].worldAllowed;V3 goals[2]{out,{}};s.allowedReady[0]=true;
   FinalD81(s,c,goals,raw,free);
   const double distance=Len(Sub(goals[0],raw[0]))*scale;
   Check(distance<=.02001&&distance>=std::min(double(penetration),.020)-.00002,
       "crossing20mm keeps a continuous bounded visible correction instead of dropping it");
   Check(std::abs(goals[0].y-raw[0].y)<.00001,"station tangential controller motion remains immediate");
   if(havePreceding)Check(std::abs(goals[0].x-preceding.x)*scale<.0002,
       "one-tenth-mm penetration change never creates20mm threshold flicker");
   ContactCommit6695(c[0],ch,goals[0],1000+11*n);
   Check(c[0].have&&c[0].fresh169r1&&c[0].blocked&&!c[0].completeClear49,
       "bounded visibility preserves actual blocked receipt and never fabricates clearance");
   Check(Len(Sub(c[0].worldAllowed,measured))<.00002,
       "partial visible penetration cannot replace the independent measured sweep origin");
   preceding=goals[0];havePreceding=true;
  }
  const V3 raw[2]{{.20f,.4f,0},{}};V3 goals[2]{};
  {ContactPass6695 pass(true,1400,previous,41,7,1400000000);
   Check(ContactHand6695(c[0],ch,1,{0,0,0},raw[0],goals[0],f,nullptr,nullptr,true),"withdrawal is queried from measured outside origin");
   FinalD81(s,c,goals,raw,free);
   Check(Len(Sub(goals[0],raw[0]))<.00001&&!c[0].blocked,"withdrawal follows exact controller without glue or release delay");}
 }
 // A completed mandatory set already measured its palm. Another feature's
 // optional recovery cannot delete that current surface when credits run out.
 {Init();g_census_mode=4;mockBodies172=true;mockPrepareNpcH9=true;mockMode=4;
  std::uint64_t previous=0;ContactPass6695 pass(true,1000,previous,1,7,1000000000);
  HandContactPending6695 p{};p.ready=true;p.worldRotation={0,0,0,1};p.worldScale=1;p.skin=.008f;
  p.before.have=true;p.before.epoch=1;p.before.character=reinterpret_cast<void*>(character);p.before.featureCount=5;
  p.before.worldAllowed={0,0,0};p.desired=p.rawDesiredD45=p.requested={.35f,0,0};
  p.queried=p.before;p.queried.blocked=true;p.queried.planeCount=1;
  p.queried.planes[0].feature=0;p.queried.planes[0].point={.25f,0,0};p.queried.planes[0].normal={-1,0,0};
  for(int i=1;i<5;++i)p.offsets[i]={.10f,0,0};
  SchedulingReadyH24();g_contact_budget6695.remaining=0;const auto calls=mockCalls;
  HandContact6695 c{};V3 out{};
  Check(CompleteHandContact6695(c,p,out)&&c.fresh169r1&&c.blocked&&c.planeCount==1&&out.x<=.24201f,
      "optional Busy preserves measured current palm instead of snapping through the wall");
  Check(mockCalls==calls&&!c.completeClear49,"optional exhaustion neither adds ray credits nor grants clear scene");
 }
 // A weapon has spent six of the shared18 credits. Each hand still fits its
 // six mandatory sweeps; the first hand must not spend the other hand's set
 // on an extra lost-feature ray. Its current palm is a real stopping surface,
 // while optional fingertip coverage remains unclaimed when credits expire.
 {Init();g_census_mode=4;mockMode=4;mockBodies172=mockPrepareNpcH9=true;
  kcd2vr::held_motion_d67::setContactFairness(true);
  schedulingGeometryH24={};schedulingGeometryH24.orientation={0,0,0,1};
  for(auto& offset:schedulingOffsetsH24)offset={};schedulingOffsetsH24[5]={.10f,0,0};
  SchedulingStateH24 s{};HandContact6695 c[2]{};V3 goals[2]{{.35f,-.2f,0},{.35f,.2f,0}};
  const bool free[2]{true,true};SchedulingTrackingH24 ts{};ts.tick=1000;ts.sequence=1;
  for(int side=0;side<2;++side){auto& h=c[side];h.have=true;h.epoch=1;
   h.character=reinterpret_cast<void*>(character);h.featureCount=6;
   h.worldAllowed={0,side?.2f:-.2f,0};h.offset[5]={.40f,0,0};h.lastCompleteTickH18=900+side;
   s.original[side].q[3]=1;ToT(s.original[side],h.worldAllowed);
  }
  std::uint64_t previous=0;ContactPass6695 pass(true,ts.tick,previous,ts.sequence,7,1000000000);
  SchedulingReadyH24();g_contact_budget6695.remaining=12;g_contact_budget6695.used=6;
  SchedulingCallerH24(s,c,goals,ts,true,free,free);
  Check(c[0].fresh169r1&&c[1].fresh169r1&&s.allowedReady[0]&&s.allowedReady[1],
      "both hands complete mandatory current coverage before optional recovery consumes credits");
  Check(c[0].blocked&&c[1].blocked&&!c[0].completeClear49&&!c[1].completeClear49&&goals[0].x<=.24201f&&goals[1].x<=.24201f,
      "both current measured palms remain blocked without inventing complete fingertip clearance");
  Check(mockCalls==12&&g_contact_budget6695.used==18&&!g_contact_budget6695.remaining,
      "two-hand scheduling respects already spent weapon credits and original shared18 cap");
  Check(mockOrigins55.size()==12&&mockOrigins55[0].y<0&&mockOrigins55[5].y<0&&mockOrigins55[6].y>0,
      "six mandatory right rays precede six left rays with no optional first-hand insertion");
 }
 std::printf("D81 contact threshold/recovery %d/%d; actual adapter, synthetic native leaves, headset NOT_RUN\n",assertions-failures,assertions);
 return failures?1:0;
}
