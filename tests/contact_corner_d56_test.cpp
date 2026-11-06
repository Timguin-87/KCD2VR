// Exact extracted production plane adapter with synthetic current ray planes.
#define KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53
int main(){
 Init();HandContact6695 c{};c.have=true;c.worldAllowed={0,0,0};c.featureCount=5;c.rawFeatureReady55=true;
 V3 offsets[5]{};for(auto& f:c.rawFeatureWorld55)f={.05f,.06f,0};
 c.planeCount=2;c.planes[0]={{.008f,0,0},{-1,0,0},-1};c.planes[1]={{0,.048f,0},{0,-1,0},-1};
 const auto allowed=ProjectPalmPlanes6697(c,{.055f,.065f,0},offsets,.008f);
 Check(allowed.x<=.00001f&&allowed.y<=.04001f,"D56 local corner continuation satisfies every freshly measured plane");
 Check(Len(Sub(allowed,c.worldAllowed))<=.0071f&&allowed.y>.005f,
       "D56 7mm raw travel cannot cause the former40mm feasible corner jump");
 for(auto& f:c.rawFeatureWorld55)f={.055f,.065f,0};
 const auto stationary=ProjectPalmPlanes6697(c,{.055f,.065f,0},offsets,.008f);
 Check(Len(Sub(stationary,c.worldAllowed))<1e-7f,"D56 stationary intent earns no tangential corner travel");
 c.worldAllowed={0,.035f,0};for(auto& f:c.rawFeatureWorld55)f={.055f,.065f,0};
 const auto withdraw=ProjectPalmPlanes6697(c,{-.03f,-.04f,0},offsets,.008f);
 Check(Len(Sub(withdraw,{-.03f,-.04f,0}))<.00001f,"D56 deliberate withdrawal remains immediate when within actual intent travel");
 c.presentationWorldD53={.3f,.4f,.5f};c.presentationTickD53=100;c.presentationTrackingD53=7;
 c.character=reinterpret_cast<void*>(0x100);c.epoch=4;c.fingerContact[2]=true;c.lastCompleteTickH18=100;
 ResetHandGeometryD56(c);
 Check(!c.have&&!c.planeCount&&!c.fingerContact[2]&&!c.lastCompleteTickH18&&c.presentationTickD53==100&&
       c.presentationTrackingD53==7&&c.epoch==4&&c.character==reinterpret_cast<void*>(0x100),
       "D56 uncertain occupancy preserves accepted restart origin while retiring all collision authority");
 std::printf("contact_corner_d56=%d/%d; native/headset=NOT_EXECUTED\n",assertions-failures,assertions);return failures?1:0;
}
