// Production projection and query pipeline, synthetic planes. Not a replay of
// unrecorded scene geometry from the observed124mm contact-goal jump.
#define KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53
int main(){
 Init();HandContact6695 c{};c.have=c.rawFeatureReady55=true;c.lastCompleteTickH18=1000;
 c.featureCount=5;c.worldAllowed={0,0,0};c.releaseRawWorldD45={.2f,0,0};
 for(auto& f:c.rawFeatureWorld55)f=c.releaseRawWorldD45;
 c.planeCount=2;c.blocked=true;c.planes[0].feature=c.planes[1].feature=0;
 c.planes[0].normal={1,0,0};c.planes[0].point={-.008f,0,0};
 c.planes[1].normal={-.99995f,.01f,0};c.planes[1].point=Mul(c.planes[1].normal,-.007f);
 for(auto& p:c.planes)p.native172.mask=npc172::StaticTerrain;
 V3 offsets[7]{},raw{.2023f,0,0};g_contact_query_tick_h18=1011;
 auto out=ProjectPalmPlanes6697(c,raw,offsets,.008f,&raw);
 Check(Len(Sub(out,c.worldAllowed))<.020f,"deep raw penetration cannot fund a nonlocal corner jump on2.3mm motion");
 c.planeCount=1;c.planes[0].normal={1,0,0};c.planes[0].point={.001f,0,0};
 out=ProjectPalmPlanes6697(c,raw,offsets,.008f,&raw);
 Check(std::abs(out.x-.009f)<.00001f,"a current moving surface still pushes the full9mm actual penetration");
 c.planes[0].normal={-1,0,0};c.planes[0].point={.008f,0,0};
 c.character=reinterpret_cast<void*>(character);c.epoch=1;c.releaseRawWorldD45=raw;
 for(auto& f:c.rawFeatureWorld55)f=raw;
 std::uint64_t previous=9;V3 features[5];for(auto& f:features)f={.02f,0,0};const int calls=mockCalls;
 {ContactPass6695 pass(true,1011,previous,9,4);
 Check(!ContactHand6695(c,reinterpret_cast<void*>(character),1,{0,0,0},raw,out,features)&&Len(Sub(out,raw))<.000001f&&!c.planeCount,
       "changed feature/no exact publication receipt follows current wrist instead of old-plane fitting");}
 Check(mockCalls==calls&&!c.fresh169r1,"repeat contact is neither a new query nor a fresh geometry witness");
 // A genuinely clear fresh result must preserve the model request exactly.
 Init();g_contact_query_tick_h18=1011;HandContactPending6695 p{};p.ready=true;p.worldRotation={0,0,0,1};p.worldScale=1;p.worldTranslation={3194,1561,7};
 p.requested={.234567f,.345678f,.456789f};p.rawDesiredD45=p.desired=Add(p.requested,p.worldTranslation);p.before.have=p.queried.have=true;
 p.before.worldAllowed=p.queried.worldAllowed=p.rawDesiredD45;p.queried.featureCount=5;
 Check(CompleteHandContact6695(c,p,out)&&out.x==p.requested.x&&out.y==p.requested.y&&out.z==p.requested.z,
       "known clear uncorrected request avoids unnecessary world float roundtrip");
 std::printf("D64 contact locality %d/%d\n",assertions-failures,assertions);return failures?1:0;}
