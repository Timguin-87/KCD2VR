// Synthetic geometry executing the production projection. Not a mesh replay.
#define KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53
int main(){
 Init();
 for(int features: {6,7})for(float overlap: {.020818f,.051890f,.114540f}){
  HandContact6695 c{};c.have=c.rawFeatureReady55=true;c.lastCompleteTickH18=1000;
  c.featureCount=features;c.planeCount=1;c.worldAllowed={0,0,0};
  auto& p=c.planes[0];p.feature=features-1;p.normal={-1,0,0};
  p.point={.1f-overlap,0,0};p.native172.mask=npc172::StaticTerrain;p.spatialProbeD81=true;
  V3 offsets[7]{};offsets[features-1]={.1f,0,0};
  for(int i=0;i<features;++i){c.offset[i]=offsets[i];c.rawFeatureWorld55[i]=offsets[i];}
  g_contact_query_tick_h18=1011;bool resolved=false;
  V3 raw{.002f,.003f,0};
  auto out=ProjectPalmPlanes6697(c,raw,offsets,0,&raw,nullptr,&resolved);
  Check(resolved&&std::abs(out.x)<1e-6f&&std::abs(out.y-.003f)<1e-6f,"spatial tip overlap stops entry without remote wrist push; tangent remains live");
  raw={-.002f,.003f,0};out=ProjectPalmPlanes6697(c,raw,offsets,0,&raw,nullptr,&resolved);
  Check(resolved&&Len(Sub(out,raw))<1e-6f,"spatial overlap cannot glue withdrawal to surface");
  p.spatialProbeD81=false;raw={.002f,0,0};out=ProjectPalmPlanes6697(c,raw,offsets,0,&raw,nullptr,&resolved);
  Check(resolved&&std::abs(out.x+overlap)<1e-6f,"actual temporal sweep retains native measured surface authority");
 }
 std::printf("D81 spatial contact %d/%d\n",assertions-failures,assertions);return failures?1:0;
}
