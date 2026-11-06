// Connected fixture: executes the ACTUAL production lexical adapter below.
// Only external engine/XR/read APIs and publication sinks are synthetic.
#include "subsystems/interactions/pet_gesture_d59.h"
#include "dog_candidate_d59r1_fixture.h"
#include "subsystems/interactions/washing_gesture_d59.h"
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <limits>
namespace kcd2vr {
namespace testD59 {
inline std::uint64_t now=1000;inline unsigned bodyReads=0,ensures=0,cameraReads=0,inputReads=0,petPublishes=0,washPublishes=0,inputPublishes=0;
inline bool bodyOkay=true,ensureOkay=true,nativeReference=false,petOn=false,washOn=false;
}
namespace tracking59c6 {struct Stamp {std::uint64_t epoch=0,tick=0;};}
namespace diagnostics::lab {inline bool NativeReference() noexcept {return testD59::nativeReference;}}
namespace frik_pose_probe {
struct MiscGestureBodyD59 {std::uintptr_t player=0,character=0;std::uint32_t playerId=0;std::uint64_t tick=0,body=0,tracking=0;float palm[2][3]{};bool valid[2]{};};
inline MiscGestureBodyD59 source;
inline bool ReadMiscGestureBodyD59(MiscGestureBodyD59& out) noexcept {++testD59::bodyReads;out={};if(!testD59::bodyOkay)return false;out=source;return true;}
}
namespace petting_d59 {inline bool Enabled() noexcept {return testD59::petOn;}inline Frame published;inline void Publish(const Frame&f) noexcept {++testD59::petPublishes;published=f;}}
namespace washing_d59 {inline Frame published;inline void Publish(const Frame&f) noexcept {++testD59::washPublishes;published=f;}}
namespace misc_gestures_d59 {
struct Input {frik_pose_probe::MiscGestureBodyD59 body{};std::uint64_t tick=0,policy=0,inputTick=0;float grip[2]{},trigger[2]{};bool free[2]{},empty[2]{},permitted=false;};
inline std::atomic<std::uint64_t> policy{1};
struct Sink {Input value{};bool publish(const Input& i)noexcept {++testD59::inputPublishes;value=i;return true;}};
inline Sink current;
inline bool Any() noexcept {return testD59::petOn||testD59::washOn;}
inline bool Ensure(std::uint64_t) noexcept {++testD59::ensures;return testD59::ensureOkay;}
}
}
using namespace kcd2vr;
static std::uint64_t GetTickCount64() noexcept {return testD59::now;}
struct CameraHook {
 static bool GetLatestVisualPositionWorld(float&x,float&y,float&z,int age) noexcept {assert(age==60);++testD59::cameraReads;x=0;y=0;z=1.7f;return true;}
 static bool GetLatestVisualForwardWorld(float&x,float&y,float&z,int age) noexcept {assert(age==60);++testD59::cameraReads;x=0;y=1;z=0;return true;}
};
struct Controller {bool active=true,grip_valid=true,trigger_valid=true;float grip=0,trigger=0;};
struct Xr {
 Controller hand[2]{};tracking59c6::Stamp stamp{30,1000};bool stampOkay=true;
 const Controller& RightController()const noexcept{return hand[0];}
 const Controller& LeftController()const noexcept{return hand[1];}
 bool CopyInputSampleStamp59c6(tracking59c6::Stamp&out) noexcept {++testD59::inputReads;out=stamp;return stampOkay;}
};
struct Candidate {bool object=false;std::uintptr_t sourcePlayer=100;std::uint64_t sourceEpoch=20,tick=1000,sourceTick=1000;std::uint32_t entity=123;int owner=0;float distance=.1f;char cls[32]="Dog";};
struct Fixture {
 bool uiBlocked=false,xr_head_valid=true;Xr xr;Xr*xr_manager_=&xr;
 Candidate candidate59c7f;int itemState116[2]{};unsigned reserved114=0,gripMask116=0;
 Fixture()=default;
 Fixture(const Fixture&o) :uiBlocked(o.uiBlocked),xr_head_valid(o.xr_head_valid),xr(o.xr),xr_manager_(o.xr_manager_?&xr:nullptr),candidate59c7f(o.candidate59c7f),reserved114(o.reserved114),gripMask116(o.gripMask116){itemState116[0]=o.itemState116[0];itemState116[1]=o.itemState116[1];}
 void tick(){
  // Real separate Dog producer, including class/owner/native-read qualification.
  // The ordinary candidate remains object=false for actors; it cannot pet.
  interaction_alignment::SetDogGestureObservationD59R1(testD59::petOn);
  palm59c9::Pair pair{};
  for(int side=0;side<2;++side){auto& r=pair.ray[side];const auto& body=frik_pose_probe::source;
   pair.eligible[side]=body.valid[side];r.hand=true;r.owner=side;r.sourcePlayer=candidate59c7f.sourcePlayer;
   r.sourceEpoch=candidate59c7f.sourceEpoch;r.tick=candidate59c7f.sourceTick;
   r.ox=body.palm[side][0];r.oy=body.palm[side][1];r.oz=body.palm[side][2];}
  test_dog_d59r1::palms(pair);interaction_alignment::testPlayerEntityD59R1=candidate59c7f.sourcePlayer;
  const int owner=candidate59c7f.owner==1?1:0;const auto& origin=frik_pose_probe::source.palm[owner];
  const float point[3]{origin[0],origin[1]+candidate59c7f.distance,origin[2]};
  test_dog_d59r1::observe(testD59::now,point,candidate59c7f.entity,candidate59c7f.cls,candidate59c7f.tick);
  #include "subsystems/interactions/misc_gestures_d59_input.inl"
 }
};
static Fixture reset(bool pet=true,bool wash=false){
 using namespace testD59;
 now=1000;bodyReads=ensures=cameraReads=inputReads=petPublishes=washPublishes=inputPublishes=0;
 bodyOkay=ensureOkay=true;nativeReference=false;petOn=pet;washOn=wash;test_dog_d59r1::reset();
 auto&s=frik_pose_probe::source;s={};s.player=100;s.character=101;s.playerId=10;s.tick=990;s.body=20;s.tracking=30;
 s.valid[0]=s.valid[1]=true;s.palm[0][0]=.1f;s.palm[0][1]=.2f;s.palm[0][2]=.6f;
 s.palm[1][0]=-.1f;s.palm[1][1]=.2f;s.palm[1][2]=.6f;
 petting_d59::published=petting_d59::Frame{};washing_d59::published={};misc_gestures_d59::current.value={};
 return {};
}
static bool canPet(int side=0){return petting_d59::Eligible(petting_d59::published,side,testD59::now);}
int main(){
 // Off is a genuine zero-work boundary, including no tracking/body/VM install.
 {auto f=reset(false,false);f.tick();assert(!testD59::bodyReads&&!testD59::ensures&&!testD59::cameraReads&&!testD59::inputReads&&!testD59::petPublishes&&!testD59::washPublishes&&!testD59::inputPublishes);}
 // Feed exact actor/body/target and committed palm, preserving handedness.
 {auto f=reset();assert(f.xr_manager_==&f.xr);f.tick();const auto&p=petting_d59::published;
  if(!canPet())std::fprintf(stderr,"pet permitted=%d id=%u actor=%llu body=%llu track=%llu policy=%llu hand tracked=%d empty=%d free=%d target=%u dog=%d distance=%f inputReads=%u\n",int(p.permitted),p.player,(unsigned long long)p.playerIdentity,(unsigned long long)p.body,(unsigned long long)p.tracking,(unsigned long long)p.policy,int(p.hand[0].tracked),int(p.hand[0].empty),int(p.hand[0].free),p.hand[0].target,int(p.hand[0].dogCandidate),p.hand[0].distance,testD59::inputReads);
  assert(canPet()&&!canPet(1));assert(p.playerIdentity==100&&p.player==10&&p.body==20&&p.tracking==30&&p.hand[0].target==123);
  assert(p.hand[0].palm[2]==.6f&&p.hand[1].palm[0]==-.1f);assert(testD59::bodyReads==1&&testD59::ensures==1&&testD59::inputReads==1);}
 {auto f=reset(false,true);f.candidate59c7f.object=true;std::strcpy(f.candidate59c7f.cls,"WaterTubeActionTrigger");f.tick();
  const auto&w=washing_d59::published;assert(w.allowed&&w.target==123&&w.openEmptyMask==3&&w.handTick[0]==990&&w.hmdTick==1000);
  assert(washing_d59::Current(w,1000));assert(w.hmd[2]==1.7f&&w.forward[1]==1);assert(!canPet());}
 // Snapshot and input validity are independent; no stale/default-zero buttons.
 for(unsigned why=0;why<8;++why){auto f=reset();
  if(why==0)f.xr.hand[0].grip_valid=false;
  if(why==1)f.xr.hand[0].trigger_valid=false;
  if(why==2)f.xr.hand[0].active=false;
  if(why==3)f.xr.stamp.tick=919;
  if(why==4)f.xr.stamp.epoch=31;
  if(why==5)f.xr.stampOkay=false;
  if(why==6)frik_pose_probe::source.valid[0]=false;
  if(why==7)f.xr.hand[0].grip=std::numeric_limits<float>::quiet_NaN();
  f.tick();assert(!canPet());assert((washing_d59::published.openEmptyMask&1)==0);
 }
 // Candidate source must join this current committed actor/body and time.
 for(unsigned why=0;why<6;++why){auto f=reset();
  if(why==0)f.candidate59c7f.tick=919;
  if(why==1)f.candidate59c7f.sourceTick=919;
  if(why==2)f.candidate59c7f.sourcePlayer=200;
  if(why==3)f.candidate59c7f.sourceEpoch=21;
  if(why==4)f.candidate59c7f.entity=0;
  if(why==5)std::strcpy(f.candidate59c7f.cls,"Human");
  f.tick();assert(!canPet());assert(petting_d59::published.hand[0].target==0);
 }
 // UI/native reference/null XR short-circuit before any body or VM work.
 for(unsigned why=0;why<4;++why){auto f=reset();
  if(why==0)f.uiBlocked=true;if(why==1)testD59::nativeReference=true;
  if(why==2)f.xr_head_valid=false;if(why==3)f.xr_manager_=nullptr;
  f.tick();assert(!canPet());assert(!washing_d59::published.allowed);
  assert(!testD59::bodyReads&&!testD59::ensures&&!testD59::inputReads&&!testD59::cameraReads);
 }
 // Reserved/fist/weapon/UI hand ownership cannot be borrowed for a gesture.
 for(unsigned why=0;why<7;++why){auto f=reset();
  if(why==0)f.reserved114=1;
  if(why==1)f.gripMask116=2; // UI bit2 is RIGHT, physics bit1 is RIGHT
  if(why==2)f.itemState116[0]=1;
  if(why==3)f.itemState116[0]=-1;
  if(why==4){f.xr.hand[0].grip=.9f;f.xr.hand[0].trigger=.9f;}
  if(why==5)f.xr.hand[0].grip=-.1f;
  if(why==6)f.xr.hand[0].trigger=-.1f;
  f.tick();assert(!canPet());assert((washing_d59::published.openEmptyMask&1)==0);
 }
 // Left petting remains independent of right reservations, correct UI mask.
 {auto f=reset();f.candidate59c7f.owner=1;f.reserved114=1;f.gripMask116=2;f.tick();assert(!canPet()&&canPet(1));}
 {auto f=reset();f.candidate59c7f.owner=1;f.gripMask116=1;f.tick();assert(!canPet(1));}
 {auto f=reset();testD59::ensureOkay=false;f.tick();assert(!canPet()&&!testD59::bodyReads&&!testD59::inputReads);}
 {auto f=reset();testD59::bodyOkay=false;f.tick();assert(!canPet()&&!testD59::inputReads);}
 return 0;
}
