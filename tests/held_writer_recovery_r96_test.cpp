// Entire production retained-collider attachment adapter. Engine memory,
// collider worker acknowledgments and renderer are explicit host leaves.
#include "vrik/physical_weapon.h"
#include "subsystems/physics/runtime/native_weapon_body_t4.h"
#include "subsystems/ranged_reload/ranged_draw_policy_d72.h"
#include <atomic>
#include <array>
#include <cstring>
#include <cstdio>
#include <cstdlib>
namespace ph=kcd2vr::physical;namespace pm=kcd2vr::pose_core;
namespace heldT4=kcd2vr::native_weapon_body_t4;namespace rdrawD72=kcd2vr::ranged_draw_d72;
static std::uint64_t nowR96=1000;static std::uint64_t GetTickCount64(){return nowR96;}
template<class T>struct MailR96{T value{};bool available=true;std::uint64_t advance=0;
 bool read(T& out){out=value;if(advance)nowR96=advance;return available;}};
struct PhysFrame6698 {ph::Input input{};ph::Output output{};ph::Profile profiles[2]{};
 struct {pm::Stamp stamp{};}correlation126[2];std::uint64_t request=4,publicationT23=2,trackingEpochT23=1;};
struct NativeHeldSourceT4 {heldT4::Input input{};float attachmentScale=1;bool valid=true;};
static MailR96<PhysFrame6698> g_phys_frame6698;static MailR96<NativeHeldSourceT4> g_native_held_source_t4;
static std::atomic<std::uint64_t> g_phys_ready_sample6698{1},g_weapon_request_epoch6694{4},g_pose_epoch6689{3};
static std::atomic<int> g_weapon_control_mode{4};static std::atomic<bool> g_committed_pose_valid6689{true};
static std::atomic<std::uintptr_t> g_native_held_attachment_self_t4{0},g_native_held_attachment_t4{0};
namespace tracking59c6 {static std::atomic<std::uint64_t> epoch{1};}
static std::array<unsigned char,128> objectR96{},attachmentR96{},entityR96{};
template<class T>static bool Read(const void* address,T& out){
 const auto a=reinterpret_cast<std::uintptr_t>(address);
 for(auto* p:{&objectR96,&attachmentR96,&entityR96}){const auto b=reinterpret_cast<std::uintptr_t>(p->data());
  if(a>=b&&a-b+sizeof(T)<=p->size()){std::memcpy(&out,address,sizeof(T));return true;}}
 return false;
}
template<class T>static void putR96(auto& block,std::size_t at,const T& value){std::memcpy(block.data()+at,&value,sizeof(value));}
static bool PhysVft6698(void* p,std::uintptr_t expected){std::uintptr_t actual=0;return Read(p,actual)&&actual==expected;}
static bool enabledR96=true,unretiredR96=true,ownerReadR96=true,receiptR96=true,currentR96=true;
static unsigned cancelsR96=0,retiresR96=0,writesR96=0;static heldT4::Achieved achievedR96{};
namespace kcd2vr::native_weapon_body_t4 {
 bool Enabled() noexcept{return enabledR96;}
 bool HasUnretiredCollider() noexcept{return unretiredR96;}
 bool ReadRetainedOwner(Owner& out) noexcept{out=achievedR96.owner;return ownerReadR96;}
 bool Read(Achieved& out) noexcept{out=achievedR96;return receiptR96;}
 void Retire(const Owner& owner) noexcept{if(owner==achievedR96.owner)++retiresR96;}
}
namespace kcd2vr::native_held_pose_t4 {static bool fresh(std::uint64_t n,std::uint64_t t){return t&&n>=t&&n-t<=80;}}
static bool NativeHeldSourceCurrentT4(const heldT4::Input&){return currentR96;}
static void NativeHeldInvalidateT4(){++cancelsR96;currentR96=false;}
struct PhysTryLock6698 {bool owned=true;};
static bool PlaceBothConsumers6689(void*,void*,const float (&)[8],bool& child){child=false;++writesR96;return true;}
static bool WeaponDrawAcceptedForD75(const rdrawD72::Identity&){return true;}
static bool RangedDrawPendingForD72(const rdrawD72::Identity&){return false;}
static bool PlacementReadback6689(void*,void*,unsigned,const float (&)[8],bool&){return true;}
static std::uint64_t ReadCompletedPublicationT23(){return 2;}
static void RangedDrawPlacedD72(const rdrawD72::Identity&,bool,bool){}
enum class PhysicalAttachmentResult101 {NotMine,Waiting,Placed,Restored,Busy,Fault};
#ifndef KCD2VR_HELD_ATTACHMENT_R96
#define KCD2VR_HELD_ATTACHMENT_R96 "../src/subsystems/physics/held_attachment_owner.inl"
#endif
#include KCD2VR_HELD_ATTACHMENT_R96
static unsigned checksR96=0;
static void needR96(bool x,const char* name){++checksR96;if(!x){std::fprintf(stderr,"FAIL %u %s\n",checksR96,name);std::exit(1);}}
static void setupR96(){
 nowR96=1000;enabledR96=unretiredR96=ownerReadR96=receiptR96=currentR96=true;cancelsR96=retiresR96=writesR96=0;
 g_weapon_control_mode=4;g_weapon_request_epoch6694=4;g_pose_epoch6689=3;g_phys_frame6698={};g_native_held_source_t4={};
 putR96(objectR96,0,std::uintptr_t(0x3A68F68));putR96(objectR96,8,std::uint32_t(7));
 putR96(attachmentR96,0,std::uintptr_t(0x3A657C0));putR96(attachmentR96,0x20,objectR96.data());putR96(entityR96,0,std::uintptr_t(0x4072C78));
 achievedR96={};auto& a=achievedR96;a.owner.key=9;a.owner.generation=10;a.owner.request=4;a.owner.epoch=3;
 a.owner.character=11;a.owner.entityPointer=reinterpret_cast<std::uintptr_t>(entityR96.data());a.owner.entityId=7;
 a.owner.attachment=reinterpret_cast<std::uintptr_t>(attachmentR96.data());a.owner.attachmentObject=reinterpret_cast<std::uintptr_t>(objectR96.data());
 a.colliderRetired=false;a.poseValid=true;a.state=heldT4::State::Active;a.colliderLease=12;a.worldGeneration=13;a.bodyGeneration=14;
 auto& f=g_phys_frame6698.value;f.request=4;f.input.sample=1;f.input.tick=1000;f.input.bodyEpoch=3;
 f.output.solverOwnedT4[0]=true;f.output.solverLeaseT4=12;f.output.solverWorldGenT4=13;f.output.solverBodyGenT4=14;f.output.solverSourceTickT4=1000;
 f.profiles[0].key=9;f.profiles[0].generation=10;f.correlation126[0].stamp.actor=11;
 g_native_held_source_t4.value.input.owner=a.owner;g_native_held_source_t4.value.input.colliderLease=12;
 g_native_held_attachment_self_t4=a.owner.attachmentObject;g_native_held_attachment_t4=a.owner.attachment;
}
static auto tickR96(){return NativeHeldAttachmentT4(objectR96.data(),attachmentR96.data());}
int main(){using R=PhysicalAttachmentResult101;
 setupR96();needR96(tickR96()==R::Placed&&writesR96==1,"qualified native held writer places once");
 receiptR96=false;g_weapon_request_epoch6694=5;
 needR96(tickR96()==R::Busy&&retiresR96==1&&writesR96==1,"positive mode request change retires even when achieved receipt is absent");
 needR96(unretiredR96,"retirement request never invents collider removal or authorizes crossbow overlap");
 unretiredR96=false;needR96(tickR96()==R::NotMine,"real collider retirement releases dispatcher for next writer");
 // Qualified new owner works after actual retirement, without resetting counters.
 achievedR96.owner.request=5;g_native_held_source_t4.value.input.owner=achievedR96.owner;g_phys_frame6698.value.request=5;
 receiptR96=unretiredR96=true;needR96(tickR96()==R::Placed&&writesR96==2,"new request performs real placement after previous retirement");
 setupR96();ownerReadR96=receiptR96=false;g_weapon_control_mode=0;
 needR96(tickR96()==R::Busy&&cancelsR96==1&&writesR96==0,"explicit OFF requests native cancellation even when all owner receipts are unreadable");
 setupR96();receiptR96=false;
 needR96(tickR96()==R::Busy&&!retiresR96&&!cancelsR96&&!writesR96,"temporary receipt gap alone neither cancels nor places");
 receiptR96=true;needR96(tickR96()==R::Placed&&writesR96==1,"same owner resumes placement after receipt recovery");
 setupR96();g_phys_frame6698.value.input.tick=g_phys_frame6698.value.output.solverSourceTickT4=1001;g_phys_frame6698.advance=1001;
 needR96(tickR96()==R::Placed&&writesR96==1&&!retiresR96,"newly read frame uses post-read validation clock");
 setupR96();g_phys_frame6698.value.input.tick=g_phys_frame6698.value.output.solverSourceTickT4=1001;
 needR96(tickR96()==R::Busy&&retiresR96==1&&!writesR96,"genuine future source still refuses placement");
 setupR96();nowR96=1081;needR96(tickR96()==R::Busy&&retiresR96==1&&!writesR96,"expired source still retires rather than renewing authority");
 std::printf("R96 retained native held attachment: %u checks PASS; worker/memory/render leaves substituted\n",checksR96);
}
