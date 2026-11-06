// Current complete native admission reader, extracted without altering its
// logic. The entity/physics/render read leaves below are explicit fixtures.
#define KCD2VR_PICKUP_BUILD_FIXTURE_ONLY_D81
#include "pickup_admission_d78_test.cpp"
#include "vrik/lab58.h"
#include "vrik/lab_prop58.h"
#include "vrik/instant_activities_d72.h"
#include "tracking_sample59c6.h"
namespace tracking59c6=kcd2vr::tracking59c6;
static std::atomic<bool> g_instant_pickup_ready_d72{true},g_committed_pose_valid6689{true};
static std::atomic<unsigned> g_lifecycle_coverage175{1};
static std::atomic<std::uint64_t> g_pose_epoch6689{1};
static std::atomic<void*> g_last_player_entity{nullptr},g_committed_character6689{nullptr};
static std::array<unsigned char,128> henry81{},definition81{},link81{},body81{};
static bool outstanding81=false,held81=false,bounds81=true,bodyRead81=true;
namespace action_time_owner59c11 {static bool NativePickupOutstandingD72(){return outstanding81;}
static bool Installed(){return true;}}
// Installation is a native hook leaf; the reader below remains production.
static bool InstallInstantPickupD72(){return true;}
static bool HeldInteractionVisual43(){return held81;}
static bool TrackingHeadUsable111(std::uint64_t){return tracking.head_valid&&!tracking.head_held;}
static bool TrackingHandUsable111(int side,std::uint64_t){return TrackingHandUsable111(side);}
static void* SmallPickupItemR24(std::uint64_t key){return key==12?nativeItem.data():nullptr;}
static void* EntityById6694(std::uint32_t id){return id==10?nativeEntity.data():id==11?henry81.data():nullptr;}
static bool SmallPickupBodyR24(void*,void*& body){body=body81.data();return bodyRead81;}
static bool SmallPickupBoundsR24(void*,void*& proxy,float (&b)[6]){
 proxy=nativeEntity.data();for(int i=0;i<3;++i){b[i]=-.1f;b[i+3]=.1f;}return bounds81;
}
static unsigned contactCalls81=0;
static std::uint32_t SmallContactAdmissionR24(std::uint64_t) noexcept {++contactCalls81;return 0;}
namespace kcd2vr::door_signal19c4r4 {
using Reader81=std::uint32_t(*)(std::uint64_t,bool) noexcept;
static unsigned registrations81=0;static Reader81 reader81=nullptr;
static bool SetSmallPickupReaderR24(Reader81 r){++registrations81;reader81=r;return true;}
}
#include "small_pickup_d81.inc"
template<class T>static void put81(void* p,unsigned offset,T value){std::memcpy(static_cast<unsigned char*>(p)+offset,&value,sizeof(value));}
static unsigned checks81=0;
static void ck81(bool v){++checks81;if(!v)std::fprintf(stderr,"small pickup check%u\n",checks81);assert(v);}
static void setup81(){
 prepare();g_grab_mode60=0;kcd2vr::lab58::rung=0;kcd2vr::lab58::f9Rung=0;kcd2vr::lab58::uiBlocked=false;kcd2vr::lab58::propWake=false;
 kcd2vr::instant_d72::activities=kcd2vr::instant_d72::interactions=true;g_instant_pickup_ready_d72=true;
 outstanding81=held81=false;bounds81=bodyRead81=true;g_lifecycle_coverage175=1;g_committed_pose_valid6689=true;
 g_last_player_entity=henry81.data();g_committed_character6689=reinterpret_cast<void*>(frame.stamp.actor);g_pose_epoch6689=frame.stamp.epoch;
 put81(henry81.data(),0xC,11u);put81(nativeItem.data(),0x48,static_cast<void*>(definition81.data()));
 put81(nativeItem.data(),0x88,static_cast<void*>(link81.data()));put81(link81.data(),0x30,10u);put81(body81.data(),0,WhBase()+0x3A578C0);
 const unsigned char milk[16]{0x71,0x50,0x55,0x51,0x55,0x7C,0xA1,0x4D,0x9B,0x61,0xEE,0x3C,0x14,0xFD,0xE1,0x8B};
 std::memcpy(definition81.data()+8,milk,16);kcd2vr::tracking59c6::epoch=1;
}
int main(){
 setup81();frame.stamp.tick=1001;readClockR96=1001;ck81(SmallPickupAdmissionR24(12,false)==1000); // source published during read
 setup81();frame.stamp.tick=1001;ck81(SmallPickupAdmissionR24(12,false)==0); // genuinely future source
 setup81();ck81(SmallPickupAdmissionR24(12,false)==1000); // known milk, ordinary gameplay, F9 OFF
 for(int side=0;side<2;++side){setup81();frame.mapGripM39[1-side]=0;
  frame.propRequested5916c1[side].p={.05,0,0};frame.allowed[side].p={.4,0,0};
  ck81(SmallPickupAdmissionR24(12,false)==1000);}
 setup81();const unsigned char plate[16]{0x09,0xA2,0x39,0xC0,0x75,0x01,0x6A,0x4B,0x95,0xDC,0xC5,0xA6,0xB0,0xB2,0x8E,0x6F};
 std::memcpy(definition81.data()+8,plate,16);ck81(SmallPickupAdmissionR24(12,false)==300);
 for(int fault=0;fault<18;++fault){setup81();switch(fault){
  case 0:kcd2vr::instant_d72::interactions=false;break;case 1:g_instant_pickup_ready_d72=false;break;
  case 2:g_ctx_menu=true;break;case 3:outstanding81=true;break;case 4:held81=true;break;
  case 5:bodyRead81=false;break;case 6:put81(body81.data(),0,WhBase()+0x3A48DC0);break;
  case 7:put81(body81.data(),0,WhBase()+0x123456);break;case 8:bounds81=false;break;
  case 9:std::memset(definition81.data()+8,0,16);break;case 10:frame.stamp.tick=now-81;break;
  case 11:frame.propRequested5916c1[0].p={.3,0,0};break;case 12:frame.mapGripM39[0]=0;break;
  case 13:handFree=false;break;case 14:selectedFistsD80=true;frame.pickupTriggerD80[0]=1;break;
  case 15:frame.propRequestedValid5916c1[0]=false;break;case 16:frame.mapFoldTrackedM39=false;break;
  case 17:g_committed_pose_valid6689=false;break;}
  ck81(SmallPickupAdmissionR24(12,false)==0);
 }
 setup81();ck81(SmallPickupAdmissionR24(12,true)==0&&contactCalls81==1); // contact path unchanged
 namespace signal=kcd2vr::door_signal19c4r4;
 UpdateSmallPickupReaderR24(false);ck81(signal::registrations81==1&&signal::reader81);
 for(int i=0;i<100;++i)UpdateSmallPickupReaderR24(false);ck81(signal::registrations81==1);
 kcd2vr::instant_d72::interactions=false;UpdateSmallPickupReaderR24(false);ck81(signal::registrations81==2&&!signal::reader81);
 std::printf("D81 actual static small-pickup admission: %u checks PASS; known milk/plate, either hand, F9 OFF; no native physics executed\n",checks81);
}
