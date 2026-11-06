// Actual frik producer + correction callback, no native API leaves invoked.
#include "vrik/physical_weapon.h"
#include "vrik/bow_camera35.h"
#include "dialogue_state59c10r2.h"
#include "activity_eye59c19.h"
#include "dialogue_smooth38.h"
#include "core/snapshot_mailbox.h"
#include <atomic>
#include <mutex>
#include <cstdio>
#include <cstdlib>
#include <limits>
namespace ph=kcd2vr::physical;namespace pm=kcd2vr::pose_core;
static std::uint64_t clock35=1000;
static std::uint64_t GetTickCount64(){return clock35;}
static std::uintptr_t WhBase(){return 0x10000000;}
static bool retail35=true;
namespace retail_build112 {static bool Ready(std::uintptr_t){return retail35;}}
namespace diagnostics::lab {static bool reference35=false;static bool NativeReference(){return reference35;}}
static bool BowHandlingMode59c9(int mode){return mode>=4;}
static int player35=0,manager35=0,other35=0;
static std::atomic<int> g_weapon_control_mode{4};
static std::atomic<bool> g_gameplay_allowed{true},g_phys_fault6698{false};
static std::atomic<void*> g_henry_item_attach_manager{&manager35};
static std::atomic<std::uint64_t> g_phys_ready_sample6698{1},g_weapon_request_epoch6694{1},g_pose_epoch6689{1};
struct PhysFrame6698 {ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};void* player=nullptr;void* manager=nullptr;std::uint64_t request=0;};
static PhysFrame6698 frame35;
static bool readable35=true;static unsigned reads35=0;
struct Box35 {bool read(PhysFrame6698& out){++reads35;out=frame35;return readable35;}};
static Box35 g_phys_frame6698;
struct PhysEquipmentItem6698 {ph::Profile profile{};bool held=false;};
struct PhysEquipment6698 {std::array<PhysEquipmentItem6698,8> items{};int count=0,heldIndex[2]{-1,-1};
 void* player=nullptr;void* manager=nullptr;std::uint64_t request=0,tick=0,epoch=0,handKey[2]{};bool valid=false,handKnown[2]{};};
static PhysEquipment6698 equipment35;static bool haveEquipment35=false;
struct EquipmentBox35 {bool read(PhysEquipment6698& out){out=equipment35;return haveEquipment35;}};
static EquipmentBox35 g_phys_equipment6698;
#include "generated_bow_camera35.inc"
namespace ds=kcd2vr::dialogue_state59c10r2;namespace core=kcd2vr::core;
struct Matrix34f {float m[3][4]{};};
static ds::State dialog35{};static ds::NativeHead head35{};
static kcd2vr::activity_eye59c19::State activity35{};
namespace activity_lens59c10r1 {
static ds::State Dialogue(std::uint64_t){return dialog35;}
static kcd2vr::activity_eye59c19::State EyeState(std::uint64_t){return activity35;}
}
namespace frik_pose_probe {
static bool BowCameraNeutral35(std::uint64_t now){return ::BowCameraNeutral35(now);}
static bool NativeDialogueHead59c10r2(ds::NativeHead& out,std::uint64_t now){out=head35;return ds::fresh(out,now);}
static bool NativeDialogueViewHead28(ds::NativeHead&,const ds::State&,std::uint64_t){return false;}
}
namespace dsm38=kcd2vr::dialogue_smooth38;static std::uint64_t DialogueSmoothNowNs38(){return clock35*1000000ull;}
#include "generated_dialogue_camera19c2.inc"
static Matrix34f native35(float z){Matrix34f m{};for(unsigned i=0;i<3;++i)m.m[i][i]=1;m.m[2][3]=z;return m;}

static unsigned checks35=0;
static void ck(bool ok,const char* why){++checks35;if(!ok){std::fprintf(stderr,"FAIL %s\n",why);std::exit(1);}}
static bool near(double a,double b){return std::fabs(a-b)<1e-5;}
static void reset35(){
 float x,y,z;g_weapon_control_mode=0;(void)BowCameraCorrection127(0,0,0,x,y,z);g_weapon_control_mode=4;
 equipment35={};haveEquipment35=false;retail35=readable35=true;diagnostics::lab::reference35=false;g_gameplay_allowed=true;g_phys_fault6698=false;
 g_henry_item_attach_manager=&manager35;g_weapon_request_epoch6694=g_pose_epoch6689=1;clock35=1000;reads35=0;
 frame35={};frame35.player=&player35;frame35.manager=&manager35;frame35.request=1;
 frame35.input.allowed=frame35.input.haveWorld=frame35.input.bowActions=true;
 frame35.input.tracked[0]=frame35.input.tracked[1]=true;frame35.input.bowGripDraw47c2=true;
 frame35.output.active[1]=frame35.output.owned[1]=true;
 frame35.input.sample=frame35.input.epoch=frame35.input.bodyEpoch=1;frame35.input.tick=clock35;g_phys_ready_sample6698=1;
 frame35.profiles[1].classId=9;frame35.profiles[1].key=20;frame35.profiles[1].generation=2;
}
static void advance35(){++clock35;++frame35.input.sample;frame35.input.tick=clock35;g_phys_ready_sample6698=frame35.input.sample;}
static void idle35(){float x,y,z;ck(!BowCameraCorrection127(10,20,1.75f,x,y,z),"native neutral learns without moving view");ck(near(x,0)&&near(y,0)&&near(z,0),"neutral correction zero");}
int main(){g_dialogue_smooth_on38=false;float x,y,z; // C38 smoothing has its own fixture; this pins the raw base
 reset35();idle35();ck(BowCameraNeutral35(clock35),"undrawn current physical bow is neutral");
 advance35();frame35.input.grip[0]=.8;
 ck(!BowCameraNeutral35(clock35),"Grip draw excludes dialogue eye learning before bowHeld ACK");
 ck(BowCameraCorrection127(10.05f,20.08f,1.55f,x,y,z),"qualified draw has XYZ correction");
 ck(near(10.05+x,10)&&near(20.08+y,20)&&near(1.55+z,1.75),"native authored bow Z drift removed");
 advance35();frame35.input.world.p={2,3,.1};
 ck(BowCameraCorrection127(12.05f,23.08f,1.65f,x,y,z),"current translated body remains owned");
 ck(near(12.05+x,12)&&near(23.08+y,23)&&near(1.65+z,1.85),"real body displacement retained, no frozen world replay");
 // HMD/IPD is not an input to this pre-HMD producer; downstream translation
 // composes once onto the returned base, never absorbed by its idle anchor.
 ck(near(1.65+z+.12,1.97),"independent tracked translation remains additive exactly once");
 advance35();const double h=std::sqrt(.5);frame35.input.world.q={0,0,h,h};
 ck(BowCameraCorrection127(-17.95f,13.08f,1.65f,x,y,z),"rotating current body reconstructs model neutral");
 ck(near(-17.95+x,-18)&&near(13.08+y,13)&&near(1.65+z,1.85),"body turn retained");
 // C63 ammunition/fire readiness must not revoke a current owned bow's eye
 // correction. Reload/empty poses must not retrain it as the neutral eye.
 reset35();idle35();advance35();frame35.input.bowActions=false;frame35.input.grip[0]=.8f;
 ck(BowCameraCorrection127(10.04f,20.05f,1.55f,x,y,z)&&near(1.55+z,1.75),"C63 empty bow with Grip held preserves learned neutral eye");
 advance35();frame35.input.grip[0]=0;
 ck(BowCameraCorrection127(10.04f,20.05f,1.72f,x,y,z)&&near(1.72+z,1.75),"C63 automatic nock/empty pose cannot retrain neutral eye");
 advance35();frame35.input.bowActions=true;frame35.input.grip[0]=.8f;frame35.output.bowHeld=true;
 ck(BowCameraCorrection127(10.04f,20.05f,1.53f,x,y,z)&&near(1.53+z,1.75),"C63 same owned loaded bow resumes draw without camera sway");
 advance35();frame35.input.tracked[0]=false;
 ck(!BowCameraCorrection127(10,20,1.55f,x,y,z),"C63 genuine tracking loss still revokes neutral camera lease");
 advance35();frame35.input.tracked[0]=true;
 ck(!BowCameraCorrection127(10,20,1.55f,x,y,z),"C63 recovery during held draw never fabricates a neutral eye");
 for(unsigned which=0;which<19;++which){reset35();idle35();advance35();frame35.output.bowHeld=true;
  switch(which){case 0:retail35=false;break;case 1:g_gameplay_allowed=false;break;case 2:diagnostics::lab::reference35=true;break;
   case 3:g_phys_fault6698=true;break;case 4:g_phys_ready_sample6698=0;break;case 5:frame35.input.sample=5;break;
   case 6:frame35.request=2;break;case 7:frame35.input.bodyEpoch=2;break;case 8:frame35.input.epoch=2;break;
   case 9:frame35.input.tick=clock35-81;break;case 10:frame35.input.tick=clock35+1;break;
   case 11:frame35.manager=&other35;break;case 12:frame35.input.tracked[0]=false;break;case 13:frame35.input.allowed=false;break;
   case 14:frame35.input.worldScale=0;break;case 15:readable35=false;break;case 16:frame35.input.world.p.x=std::numeric_limits<double>::quiet_NaN();break;
   case 17:frame35.output.active[1]=false;break;case 18:frame35.output.owned[1]=false;break;}
  ck(!BowCameraCorrection127(10,20,1.55f,x,y,z)&&near(z,0),"unqualified owner/stale/unknown preserves native view");
  ck(!BowCameraNeutral35(clock35),"unqualified physical frame cannot train dialogue eye");}
 for(unsigned owner=0;owner<5;++owner){reset35();idle35();advance35();frame35.output.bowHeld=true;
  switch(owner){case 0:++frame35.profiles[1].key;break;case 1:++frame35.profiles[1].generation;break;
   case 2:frame35.player=&other35;break;case 3:++frame35.request;++frame35.input.epoch;++g_weapon_request_epoch6694;break;
   case 4:++frame35.input.bodyEpoch;++g_pose_epoch6689;break;}
  ck(!BowCameraCorrection127(10,20,1.55f,x,y,z),"replaced owner cannot borrow prior neutral eye");}
 reset35();frame35.output.bowHeld=true;ck(!BowCameraCorrection127(10,20,1.55f,x,y,z),"first observed drawn pose cannot fabricate neutral");
 reset35();idle35();advance35();frame35.output.bowHeld=true;
 ck(!BowCameraCorrection127(10,20,.9f,x,y,z),"unbounded authored displacement fails closed");
 reset35();idle35();advance35();frame35.input.bowGripDraw47c2=false;frame35.input.trigger[0]=.8;
 ck(!BowCameraNeutral35(clock35)&&BowCameraCorrection127(10,20,1.55f,x,y,z),"selected physical Trigger draw also excludes authored displacement");
 // Full VRIK mode with bare hands has no physical bow frame yet. Native
 // copied equipment can positively prove neutral without a native poll.
 reset35();g_phys_ready_sample6698=0;haveEquipment35=true;
 equipment35.player=&player35;equipment35.manager=&manager35;equipment35.valid=true;
 equipment35.tick=clock35;equipment35.request=equipment35.epoch=1;equipment35.handKnown[1]=true;
 ck(BowCameraNeutral35(clock35),"full mode known-empty equipment/no weapon frame can learn first dialogue eye");
 equipment35.handKnown[1]=false;ck(!BowCameraNeutral35(clock35),"unknown hand is not proven empty");equipment35.handKnown[1]=true;
 equipment35.tick=clock35-81;ck(!BowCameraNeutral35(clock35),"stale empty witness cannot bless neutral");equipment35.tick=clock35;
 equipment35.handKey[1]=20;equipment35.count=1;equipment35.heldIndex[1]=0;
 equipment35.items[0].held=true;equipment35.items[0].profile.key=20;equipment35.items[0].profile.classId=9;
 ck(!BowCameraNeutral35(clock35),"known equipped bow without frame is not neutral");
 equipment35.items[0].profile.classId=1;ck(BowCameraNeutral35(clock35),"exact fresh held nonbow can learn neutral");
 equipment35.items[0].profile.key=21;ck(!BowCameraNeutral35(clock35),"nonbow mismatched native key cannot bless neutral");
 equipment35.handKey[1]=0;g_phys_ready_sample6698=frame35.input.sample+1;
 ck(!BowCameraNeutral35(clock35),"advertised bow/current-ready mismatch remains fail-closed despite empty fallback");
 // Connect the actual equipment/physical producer to the actual dialogue
 // camera consumer: first unarmed dialogue with full mode/no physical frame.
 reset35();g_phys_ready_sample6698=0;haveEquipment35=true;
 equipment35.player=&player35;equipment35.manager=&manager35;equipment35.valid=true;
 equipment35.tick=clock35;equipment35.request=equipment35.epoch=1;equipment35.handKnown[1]=true;
 dialog35={};dialog35.tick=clock35;dialog35.entity=0x100;dialog35.known=true;dialog35.camera=ds::Camera::FirstPerson;
 head35={clock35,1,0x100,0x200,{0,0,1.7f},{},1,{0,0,0},true};activity35.known=true;
 g_dialogue_eye59c10r2.publish(ds::EyeOffset{});auto native=native35(1.756f);
 ck(!DialogueEyeBase59c10r2(native,false),"unarmed neutral producer does not move native camera");
 ds::EyeOffset measured{};ck(g_dialogue_eye59c10r2.read(measured)&&measured.ready&&measured.modelLocalReady,
    "actual known-empty producer allows first-dialogue neutral eye learning");
 dialog35.active=true;dialog35.instance=0x300;head35.world.z=1.3f;g_dialogue_policy59c10r2.publish({clock35,true});native=native35(1.3f);
 ck(DialogueEyeBase59c10r2(native,false)&&near(native.m[2][3],1.756),"actual first-dialogue body model suppresses authored neck dip");
 reset35();frame35.output.bowHeld=true;dialog35.active=false;g_dialogue_eye59c10r2.publish(ds::EyeOffset{});native=native35(1.3f);
 ck(!DialogueEyeBase59c10r2(native,false)&&g_dialogue_eye59c10r2.read(measured)&&!measured.ready,
    "actual physical drawn-bow producer refuses dialogue neutral training");
 std::printf("bow_camera35: %u actual producer/correction checks PASS; nativeIO=NONE\n",checks35);
}
