// Current production query/projection pipeline, explicit synthetic native geometry.
// Recorded D78 clock/correction values replay the contact-return kernel only;
// the mount's unrecorded full mesh/feature offsets are NOT invented here.
#define KCD2VR_WORLD_ADAPTER_CALLER_ONLY_D56 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_CALLER_ONLY_D56
#include "vrik/hand_present_d62.h"
#include "data/contact_release_d78_reference.h"
namespace settingsD79 {
namespace ph {static std::atomic<bool> preciseContacts44{false};static std::atomic<int> handsModeDs2{0};}
static std::atomic<std::uint64_t> g_contact_control_epoch6695{1};
#include "generated_contact_settings_d79.inc"
static unsigned actualPolicy(bool stationContactD75=false){
#include "generated_contact_policy_d79.inc"
 return unsigned(preciseContactModeR24)|(unsigned(balancedContactD79)<<1)|(unsigned(perfContactModeDs2)<<2);
}
}
int main(){
 namespace bc=kcd2vr::contact_balanced_d79;
 {settingsD79::SetHandsModeDs2(2);settingsD79::SetPreciseContacts44(true);
  Check(settingsD79::HandsModeDs2()==2&&settingsD79::PreciseContacts44()&&settingsD79::actualPolicy()==2,
   "actual legacy precise setter cannot make displayed Balanced use precise contact recipe");
  Check(settingsD79::actualPolicy(true)==0,"actual station policy overrides both saved contact selections");
  settingsD79::SetHandsModeDs2(1);Check(settingsD79::actualPolicy()==1,"explicit precise mode still selects existing precise recipe");
  settingsD79::SetHandsModeDs2(0);Check(settingsD79::actualPolicy()==0,"explicit coarse mode still selects existing palm recipe");
 }
 {V3 palm[6]{},tips[6]{},out[7]{};for(int i=0;i<6;++i){palm[i]={float(i),0,0};tips[i]={0,float(i),0};}
  bc::compose(palm,tips,out);for(int i=0;i<5;++i)Check(out[i].x==i,"balanced retains each palm identity");
  Check(out[5].y==1&&out[6].y==2,"stable thumb/index tips, no changing farthest-tip identity");
  Check(bc::selected(2,false)&&!bc::selected(2,true)&&!bc::selected(1,false),"mode2 owns balanced, station keeps its own policy");}
 // A contact-only high-resolution release must progress under duplicate coarse
 // ticks without moving a free hand or spending a16mm minimum step.
 {V3 raw{.3f,0,0},correction{-.1f,0,0};
  auto first=kcd2vr::contact_release_d45::candidate(raw,raw,correction,1,1000,1000,1011111111,1000000000);
  const auto old=kcd2vr::contact_release_d78_reference::candidate(raw,raw,correction,1,1015,1000);
  Check(std::abs(old.x-.216f)<1e-6f,"frozen D78 has16mm minimum release on a distinct coarse tick");
  Check(std::abs(first.x-.2111111f)<1e-6f,"90Hz release advances11.11mm on shared coarse tick rather than freezing");
  auto next=kcd2vr::contact_release_d45::candidate(raw,raw,correction,1,1015,1000,1001000000,1000000000);
  Check(std::abs(next.x-.201f)<1e-6f,"1ms current interval never becomes16mm step");
  auto zero=kcd2vr::contact_release_d45::candidate(raw,raw,correction,1,1000,1000,1000000000,1000000000);
  Check(std::abs(zero.x-.2f)<1e-6f,"identical high-resolution source never integrates twice");
  auto withdrawal=kcd2vr::contact_release_d45::candidate(V3{.1f,0,0},raw,correction,1,1011,1000,1011111111,1000000000);
  Check(withdrawal.x==.1f,"withdrawing free of measured correction is immediate");
  auto expired=kcd2vr::contact_release_d45::candidate(raw,raw,correction,1,1081,1000,1081000000,1000000000);
  Check(expired.x==raw.x,"80ms geometry lifetime is not extended");
 }
 // Actual native adapter: balanced fingertip catches an obstacle missed by
 // coarse palm, while neither input finger curl nor query count is amplified.
 for(bool balanced:{false,true}){
  Init();mockMode=4;wallPoint={.25f,0,0};wallNormal={-1,0,0};
  V3 features[7]{};features[5]=features[6]={.14f,0,0};HandContact6695 c{};V3 out{};std::uint64_t prev=0;
  {ContactPass6695 pass(true,1000,prev,1,1,1000000000);
   Check(ContactHand6695(c,reinterpret_cast<void*>(character),1,{0,0,0},{.14f,0,0},out,features,nullptr,nullptr,false,balanced),"actual sparse query completes");}
  Check(balanced?c.blocked&&!c.completeClear49:!c.blocked&&out.x==.14f,"selected tips add real world contact beyond coarse palm");
  Check(mockCalls<=(balanced?7:5)&&c.featureCount==(balanced?7:5),"fixed mandatory feature budget");
  for(int i=0;i<5;++i)Check(!c.fingerContact[i],"balanced never owns finger curls");
  if(balanced)Check(out.x<=.1081f&&out.x>=.1078f,"balanced uses2mm positive surface clearance");
 }
 // Actual production caller, two free hands: fourteen maximum mandatory rays,
 // one shared18ray budget, duplicate source spends none. No extra fit passes.
 {Init();mockMode=0;SchedulingStateH24 s{};HandContact6695 c[2]{};V3 goals[2]{{.2f,0,0},{-.2f,0,0}};
  SchedulingTrackingH24 t{};t.sequence=1;t.tick=1000;bool valid[2]{true,true},free[2]{true,true};std::uint64_t prev=0;
  {ContactPass6695 pass(true,1000,prev,1,1,1000000000);SchedulingCallerH24(s,c,goals,t,true,valid,free,1,true);
   Check(mockCalls<=14&&g_contact_budget6695.remaining>=4,"two balanced hands stay within existing budget");}
  const int calls=mockCalls;
  {ContactPass6695 pass(true,1000,prev,1,1,1000000000);SchedulingCallerH24(s,c,goals,t,true,valid,free,1,true);}
  Check(mockCalls==calls,"duplicate consumer cannot replenish or repeat balanced rays");
  Check(c[0].balancedD79&&c[1].balancedD79&&c[0].featureCount==7,"actual caller selects balanced layout");
 }
 // A current wall must remain current even when release travel is shorter than
 // clearance. At240Hz there is no16mm step and no false-clear/stick oscillation.
 for(bool balanced:{false,true}){
  Init();mockMode=4;wallPoint={.25f,0,0};wallNormal={-1,0,0};V3 f[7]{},out{};HandContact6695 c{};std::uint64_t prev=0;
  for(unsigned frame=0;frame<15;++frame){
   ContactPass6695 pass(true,1000+frame*4,prev,frame+1,1,1000000000ull+frame*4166667ull);
   Check(ContactHand6695(c,reinterpret_cast<void*>(character),1,{0,0,0},{.35f,0,0},out,f,nullptr,nullptr,false,balanced)&&c.blocked,
    "sub-skin release travel still rediscovers current wall with same mandatory rays");
   Check(std::abs(out.x-(balanced?.248f:.242f))<.0002f,"resting contact remains stable with positive clearance");
  }
  mockMode=0;V3 prior=out;
  for(unsigned frame=15;frame<45;++frame){
   ContactPass6695 pass(true,1000+frame*4,prev,frame+1,1,1000000000ull+frame*4166667ull);
   Check(ContactHand6695(c,reinterpret_cast<void*>(character),1,{0,0,0},{.35f,0,0},out,f,nullptr,nullptr,false,balanced),"cleared edge continues existing query path");
   Check(out.x>=prior.x-.0001f&&out.x-prior.x<.0063f,"cleared contact return has no16mm minimum jump");prior=out;
  }
  Check(out.x==.35f&&!c.releaseActiveD45,"edge release ends at exact controller pose");
 }
 // Earlier recorded BASE/entity class at the bench had a lost fingertip
 // while palm stayed clear. Sparse geometry is synthetic; actual adapter and
 // source admission run unchanged, with no optional recovery rays available.
 for(float tangent:{0.f,.00246f}){
  Init();g_census_mode=4;mockBodies172=true;mockPrepareNpcH9=true;mockMode=4;
  const std::uintptr_t body=0x180004000ull,entity=0x180006000ull;
  mockCollider172=reinterpret_cast<void*>(body);Store(body,base+0x3a578c0);Store(base+0x3a578c0+0x50,base+0x5d6f44);
  Store(body+0x20,entity);Store(entity,base+0x4072c78);Store(entity+0xC,std::uint32_t(952226));
  V3 features[7]{};for(auto& f:features)f={.04f,0,0};features[5]=features[6]={.14f,0,0};
  HandContact6695 c{};V3 raw{.14f,0,0},out{};std::uint64_t previous=0;wallPoint={.145f,0,0};
  {ContactPass6695 pass(true,1000,previous,238014,3,1000000000);
   Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},raw,out,features,nullptr,nullptr,false,true)&&c.blocked&&c.fresh169r1,
    "balanced selected tip establishes measured BASE contact");}
  auto accepted=out;const auto calls=mockCalls;wallPoint.x=.136f;raw.y+=tangent;
  {ContactPass6695 pass(true,1011,previous,238018,3,1011111111);HandContactPending6695 pending{};
   const bool queried=ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},raw,out,features,nullptr,&pending,false,true);
   for(int p=0;p<pending.queried.planeCount;++p){const auto& plane=pending.queried.planes[p];std::printf("TIPPLANE feature=%d p=%g,%g n=%g,%g\n",plane.feature,plane.point.x,plane.point.y,plane.normal.x,plane.normal.y);}
   Check(queried&&CompleteHandContact6695(c,pending,out)&&c.blocked&&c.fresh169r1&&
       out.x+.14f<=wallPoint.x-.0019f&&Len(Sub(out,accepted))<.0101f,"overtaken selected tip stays current with clear palm and small tangential motion");}
  std::printf("BALANCED_TIP tangent=%g out=(%g,%g) accepted=(%g,%g) fresh=%d blocked=%d planes=%d calls=%d\n",tangent,out.x,out.y,accepted.x,accepted.y,c.fresh169r1,c.blocked,c.planeCount,mockCalls-calls);
  Check(mockCalls-calls<=7,"balanced lost-tip requalification reuses mandatory rays only");
  mockMode=0;const auto old=out;
  {ContactPass6695 pass(true,1022,previous,238022,3,1022222222);
   Check(ContactHand6695(c,reinterpret_cast<void*>(character),5,{-.2f,0,0},raw,out,features,nullptr,nullptr,false,true)&&!c.blocked&&c.planeCount==0&&
       Len(Sub(out,old))<.0113f,"fresh CLEAR retires tip plane without full correction jump");}
 }
 // D80: transient native budget loss immediately retires contact authority;
 // a later fresh query reacquires it without inventing a clear receipt.
 {Init();mockMode=4;wallPoint={.25f,0,0};V3 f[7]{},out{};HandContact6695 c{};std::uint64_t prev=0;
  {ContactPass6695 pass(true,1000,prev,1,1,1000000000);ContactHand6695(c,reinterpret_cast<void*>(character),1,{0,0,0},{.35f,0,0},out,f,nullptr,nullptr,false,true);}
  const auto accepted=out;const auto stamp=c.lastCompleteTickH18;
  {ContactPass6695 pass(true,1011,prev,2,1,1011111111);g_contact_budget6695.remaining=0;
   Check(!ContactHand6695(c,reinterpret_cast<void*>(character),1,{0,0,0},{.35f,.001f,0},out,f,nullptr,nullptr,false,true)&&
     !c.fresh169r1&&!c.lastCompleteTickH18&&!c.planeCount&&Len(Sub(out,{.35f,.001f,0}))<.00001f,"brief exhausted query follows current request without claiming clearance");}
  mockMode=0;
  {ContactPass6695 pass(true,1022,prev,3,1,1022222222);
   Check(ContactHand6695(c,reinterpret_cast<void*>(character),1,{0,0,0},{.35f,.001f,0},out,f,nullptr,nullptr,false,true)&&
      c.fresh169r1&&!c.blocked&&Len(Sub(out,{.35f,.001f,0}))<.00001f,"return from brief query loss rechecks current geometry without old correction");}
 }
 // D78 recorded mixed articulated-face receipts: actual completed projection
 // must not call their far tangent intersection a fresh balanced endpoint.
 {Init();kcd2vr::contact_handover_d71::SetContactContinuity(true);g_contact_pass6695=true;
  g_contact_budget_sample65=401894;g_contact_query_tick_h18=213238093;
  HandContactPending6695 p{};p.ready=true;p.worldRotation={0,0,0,1};p.worldScale=1;
  p.before.have=p.before.rawFeatureReady55=true;p.before.balancedD79=true;p.before.lastCompleteTickH18=213238078;
  p.before.character=reinterpret_cast<void*>(character);p.before.epoch=6;p.before.worldAllowed={3169.953f,496.4583f,42.68269f};
  p.before.blocked=true;p.before.completeClear49=false;p.before.planeCount=1;
  p.requested=p.desired=p.rawDesiredD45={3169.9198f,496.492f,42.7934f};
  p.queried=p.before;p.queried.planeCount=1;p.queried.planes[0].feature=0;
  p.queried.planes[0].point={3169.919f,496.4896f,42.7603f};p.queried.planes[0].normal={.2505205f,-.2515042f,-.9348717f};
  auto& w=p.queried.planes[0].native172;w.mask=npc172::BodiesAndWorld;w.body=npc172::Body::Articulated;w.vft=0x3a48b30;
  w.collider=0x18F680E10C0ull;w.owner=0x18D136CF200ull;w.ownerRead=true;w.entityId=946001;w.part=67;
  // This exact observed plane has bodySample0. Missing front face must not be invented.
  HandContact6695 c{};V3 out{};Check(!CompleteHandContact6695(c,p,out)&&!c.fresh169r1&&!c.planeCount&&Len(Sub(out,p.requested))<.001f,
    "recorded unqualified articulated plane cannot produce fresh remote wrist push");
 }
 {using namespace kcd2vr::hand_present_d62;Basis a{},b{};a.palmCount[0]=b.palmCount[0]=7;
  Check(Same(a,b),"optional display validates all7 current features");b.palm[0][6].x=.001;
  Check(!Same(a,b),"changed selected tip retires optional display clearance");}
 std::printf("D79 balanced/contact release %d/%d; actual adapter, native geometry synthetic; no headset claim\n",assertions-failures,assertions);
 return failures?1:0;
}
