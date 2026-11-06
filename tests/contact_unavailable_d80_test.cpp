// Actual production contact kernel; native memory and ray geometry are synthetic leaves.
#define KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53
static bool Near(V3 a,V3 b){return Len(Sub(a,b))<.00001f;}
int main(){
 for(bool balanced:{false,true}){
  Init();mockMode=4;HandContact6695 c{};V3 out{},features[7]{};std::uint64_t previous=0;
  const auto ch=reinterpret_cast<void*>(character);const V3 blocked{.35f,0,0};
  {ContactPass6695 pass(true,1000,previous,1,7,1000000000);
   Check(ContactHand6695(c,ch,1,{0,0,0},blocked,out,features,nullptr,nullptr,false,balanced)&&c.blocked&&c.fresh169r1,"current measured wall still owns normal correction");
   ContactCommit6695(c,ch,out,1000);}
  const auto accepted=out;const auto queried=mockCalls;
  {ContactPass6695 pass(true,1000,previous,1,7,1000000000);
   Check(ContactHand6695(c,ch,1,{0,0,0},blocked,out,features,nullptr,nullptr,false,balanced)&&Near(out,accepted)&&mockCalls==queried,"same XR publication reuses admitted endpoint without new rays");}
  // A fresh tracked withdrawal/tangent request with exhausted shared coverage.
  // The old implementation freezes all coordinates for <=80ms here.
  const V3 withdrawn{.18f,.05f,.03f};
  {ContactPass6695 pass(true,1011,previous,2,7,1011000000);g_contact_budget6695.remaining=0;
   const bool constrained=ContactHand6695(c,ch,1,{.8f,.8f,.8f},withdrawn,out,features,nullptr,nullptr,false,balanced);
   Check(!constrained&&Near(out,withdrawn),"incomplete coverage immediately follows current withdrawal and tangent");
   Check(!c.have&&!c.blocked&&!c.fresh169r1&&!c.completeClear49&&c.planeCount==0&&!c.lastCompleteTickH18&&!c.releaseActiveD45,"unknown coverage retires geometry and timer without fabricating clear");
   Check(mockCalls==queried,"exhausted coverage adds no native ray");
   ContactCommit6695(c,ch,out,1011);}
  // A moving door/body might occupy the OLD plane now. It has no authority
  // until a fresh query: neither its old transform nor identity is dereferenced.
  const V3 moved{.15f,.08f,.02f};
  {ContactPass6695 pass(true,1022,previous,3,7,1022000000);g_contact_budget6695.remaining=0;
   Check(!ContactHand6695(c,ch,1,{.8f,.8f,.8f},moved,out,features,nullptr,nullptr,false,balanced)&&Near(out,moved)&&!c.planeCount,"repeated unknown coverage cannot repin a new live request");
   ContactCommit6695(c,ch,out,1022);}
  mockMode=0;mockOrigins55.clear();const V3 next{.14f,.10f,.01f};
  {ContactPass6695 pass(true,1033,previous,4,7,1033000000);
   Check(ContactHand6695(c,ch,1,{.8f,.8f,.8f},next,out,features,nullptr,nullptr,false,balanced)&&c.fresh169r1&&c.completeClear49&&Near(out,next),"fresh native clearance reacquires ordinary current motion");
   Check(!mockOrigins55.empty()&&Near(mockOrigins55.front(),moved),"reacquisition starts at latest displayed wrist, not authored native wrist");}
  // Current valid contact is NOT made permissive by this fallback change.
  mockMode=4;
  {ContactPass6695 pass(true,1044,previous,5,7,1044000000);
   Check(ContactHand6695(c,ch,1,{0,0,0},blocked,out,features,nullptr,nullptr,false,balanced)&&c.blocked&&out.x<.25f,"a later verified wall still blocks fresh normal penetration");}
  // Exact current owner invalidation stays fail-closed, independent of Busy policy.
  memory.erase(character+0xA08);
  {ContactPass6695 pass(true,1055,previous,6,7,1055000000);
   Check(!ContactHand6695(c,ch,2,{0,0,0},next,out,features,nullptr,nullptr,false,balanced)&&Near(out,next)&&!c.have&&!c.planeCount,"invalid current model/owner releases only contact, never reuses stale surface");}
 }
 // If a new publication is not the budget owner, it may not borrow another
 // publication's admitted endpoint. Repeated callbacks of the SAME sample above may.
 {Init();mockMode=4;HandContact6695 c{};V3 out{},f[7]{};std::uint64_t previous=0;const auto ch=reinterpret_cast<void*>(character);
  {ContactPass6695 pass(true,1000,previous,1,7);ContactHand6695(c,ch,1,{0,0,0},{.35f,0,0},out,f);ContactCommit6695(c,ch,out,1000);}
  previous=2;g_contact_budget_sample65=2;g_contact_budget6695.used=1;
  const V3 current{.17f,.04f,0};
  {ContactPass6695 pass(true,1011,previous,2,7);Check(!g_contact_pass6695,"fixture uses previously consumed current publication budget");
   Check(!ContactHand6695(c,ch,1,{0,0,0},current,out,f)&&Near(out,current)&&!c.planeCount,"different publication without coverage cannot pin to prior sample");}
 }
 // Same XR sequence is insufficient if the requested world feature or model
 // basis changed. These cases spend no extra native query and release geometry.
 for(int change=0;change<3;++change){
  Init();mockMode=4;HandContact6695 c{};V3 out{},f[7]{};std::uint64_t previous=0;
  const auto ch=reinterpret_cast<void*>(character);V3 raw{.35f,0,0};
  {ContactPass6695 pass(true,1000,previous,1,7);ContactHand6695(c,ch,1,{0,0,0},raw,out,f);ContactCommit6695(c,ch,out,1000);}
  const int calls=mockCalls;
  if(change==0)raw.y=.04f;
  if(change==1)f[1].y=.02f; // current hand geometry/orientation changed
  if(change==2){const float movedRoot[]{0,0,0,1,.1f,0,0,1};Store(character+0xA08,movedRoot);}
  {ContactPass6695 pass(true,1000,previous,1,7);
   Check(!ContactHand6695(c,ch,1,{0,0,0},raw,out,f)&&Near(out,raw)&&!c.planeCount&&mockCalls==calls,
      "same publication with changed current request/feature/root does not reuse old endpoint");}
 }
 {Init();mockMode=4;HandContact6695 c{};V3 out{},f[7]{};std::uint64_t previous=0;const auto ch=reinterpret_cast<void*>(character);const V3 raw{.35f,0,0};
  {ContactPass6695 pass(true,1000,previous,1,0);ContactHand6695(c,ch,1,{0,0,0},raw,out,f);}
  {ContactPass6695 pass(true,1000,previous,1,0);
   Check(!ContactHand6695(c,ch,1,{0,0,0},raw,out,f)&&Near(out,raw)&&!c.planeCount,"unknown tracking epoch cannot claim exact publication reuse");}
 }
 {Init();mockMode=4;HandContact6695 c{};V3 out{},f[7]{};std::uint64_t previous=0;const auto ch=reinterpret_cast<void*>(character);const V3 raw{.35f,0,0};
  FingerContactGeometry103 geometry{};geometry.orientation={0,0,0,1};
  for(int digit=1;digit<5;++digit)geometry.curve[digit].count=3;
  {ContactPass6695 pass(true,1000,previous,1,7);ContactHand6695(c,ch,1,{0,0,0},raw,out,f,&geometry);}
  Check(c.featureCount==7,"fixture has completed precise knuckle feature");
  geometry.curve[1].count=0;
  {ContactPass6695 pass(true,1011,previous,2,7);ContactHand6695(c,ch,1,{0,0,0},raw,out,f,&geometry);}
  Check(c.featureCount==6&&c.fresh169r1,"fixture completes six-feature precise layout retaining unused old slot");
  geometry.curve[1].count=3;const int calls=mockCalls;
  {ContactPass6695 pass(true,1011,previous,2,7);
   Check(!ContactHand6695(c,ch,1,{0,0,0},raw,out,f,&geometry)&&Near(out,raw)&&!c.planeCount&&mockCalls==calls,
     "readded seventh feature cannot inherit old same-publication six-feature authority");}
 }
 // Qualified entity-backed contact (e.g. a moving door leaf) becomes invalid
 // while coverage is unavailable. Do not replay its old world plane or owner.
 {Init();g_census_mode=4;mockBodies172=true;mockPrepareNpcH9=true;mockMode=4;
  const std::uintptr_t body=0x180004000ull,entity=0x180006000ull;mockCollider172=reinterpret_cast<void*>(body);
  Store(body,base+0x3a578c0ull);Store(base+0x3a578c0ull+0x50,base+0x5d6f44);Store(body+0x20,entity);
  Store(entity,base+0x4072c78);Store(entity+0xC,std::uint32_t(777));
  HandContact6695 c{};V3 out{},f[7]{};std::uint64_t previous=0;const auto ch=reinterpret_cast<void*>(character);
  {ContactPass6695 pass(true,1000,previous,1,7);ContactHand6695(c,ch,1,{0,0,0},{.35f,0,0},out,f);ContactCommit6695(c,ch,out,1000);}
  Check(c.blocked&&c.planeCount&&c.planes[0].native172.entityId==777,"fixture has actual qualified entity-backed plane");
  memory.erase(body);memory.erase(entity);const int calls=mockCalls;const V3 live{.18f,.07f,0};
  {ContactPass6695 pass(true,1011,previous,2,7);g_contact_budget6695.remaining=0;
   Check(!ContactHand6695(c,ch,1,{0,0,0},live,out,f)&&Near(out,live)&&!c.planeCount&&mockCalls==calls,
      "invalidated/moving entity plus unavailable coverage cannot lend old plane or pin hand");}
 }
 std::printf("%d checks, %d failures\n",assertions,failures);return failures?1:0;
}
