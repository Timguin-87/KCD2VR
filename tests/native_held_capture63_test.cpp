// Verbatim production source capture/body guards. Only native functions,
// readable memory, current-state atomics and diagnostic sink are providers.
#include "native_weapon_body_t4.h"
#include "vrik/native_held_owner_t4_status.h"
#include "vrik/physical_weapon.h"
#include "core/native_grasp13.h"
#include "core/snapshot_mailbox.h"
#include "tracking_sample59c6.h"
#include "subsystems/experimental/deepseek_options_h29.h"
#include "../src/subsystems/physics/native_attachment_slots_f2.inl"
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>
namespace heldT4=kcd2vr::native_weapon_body_t4;
namespace ph=kcd2vr::physical;
namespace pm=kcd2vr::pose_core;
using kcd2vr::frik_pose_probe::HeldPhysicsSourceT4;
namespace fixture63 {inline bool enabled=true;}
namespace kcd2vr::native_weapon_body_t4 {
bool Enabled() noexcept{return fixture63::enabled;}
bool SetSourcePredicate(SourcePredicate) noexcept{return true;}
}
#include "generated_held_capture63_types.inc"
static kcd2vr::core::SnapshotMailbox<NativeHeldSourceT4> g_native_held_source_t4;
static std::atomic<bool> g_native_held_valid_t4{false};
static std::atomic<bool> g_native_held_equipment_pending_h36{false}; // explicit current native-owner service input
static std::atomic<std::uint64_t> g_native_held_lease_t4{1},g_weapon_request_epoch6694{2},g_pose_epoch6689{7},g_pose_policy_generation57{2};
static std::atomic<float> g_right_grip{1};
static std::atomic<HeldPhysicsSourceT4> g_native_held_reason_t4{HeldPhysicsSourceT4::Disabled};
static NativeHeldProfileReceipt62 g_native_held_profile_receipt62;
static bool currentContext=true,retired=true,retail=true;
static std::uintptr_t WhBase(){return 0x10000000;}
namespace retail_build112 {static bool Ready(std::uintptr_t){return retail;}}
namespace diagnostic_trace {static void Write(const char*){}}
static void NativeHeldInvalidateT4(){g_native_held_valid_t4=false;}
static bool NativeHeldSourceCurrentT4(const heldT4::Input&) noexcept{return currentContext;}
static bool LegacyPhysBodiesRetiredT4(){return retired;}
struct Matrix34V6550 {float m[12]{};};
struct PhysItem63 {ph::Profile profile{};bool held=false,boundsValid=false;void* item=nullptr;void* entity=nullptr;void* worldLink54=nullptr;float bounds[6]{};};
struct PhysEquipment6698 {bool valid=false;bool handKnown[2]{};std::uint64_t handKey[2]{},request=2,epoch=7,leftNativeMembershipH36=19;int heldIndex[2]{0,-1},count=1;void* player=nullptr;void* manager=nullptr;std::array<PhysItem63,2> items{};};
static std::array<unsigned char,0xB00> player{},item{},entity{},link{},model{},attachment{},object{},manager{},character{},weaponProxy{},weaponBody{},playerEntity{},playerProxy{},playerBody{};
static std::map<std::uintptr_t,std::vector<unsigned char>> immutable;
static unsigned proxyCalls=0;
template<class T>static void put(auto& storage,std::size_t offset,T value){std::memcpy(storage.data()+offset,&value,sizeof value);}
template<class T>static bool Read(const void* address,T& out){
 const auto p=reinterpret_cast<std::uintptr_t>(address);
 if(auto i=immutable.find(p);i!=immutable.end()&&i->second.size()==sizeof(T)){std::memcpy(&out,i->second.data(),sizeof out);return true;}
 for(auto* a:{&player,&item,&entity,&link,&model,&attachment,&object,&manager,&character,&weaponProxy,&weaponBody,&playerEntity,&playerProxy,&playerBody}){
  const auto begin=reinterpret_cast<std::uintptr_t>(a->data());
  if(p>=begin&&p-begin<=a->size()&&sizeof(T)<=a->size()-(p-begin)){std::memcpy(&out,address,sizeof out);return true;}
 }
 return false;
}
template<class T>static void fixed(std::uintptr_t at,T value){auto& bytes=immutable[at];bytes.resize(sizeof value);std::memcpy(bytes.data(),&value,sizeof value);}
static bool PhysVft6698(void* p,std::uintptr_t rva){std::uintptr_t v=0;return p&&Read(p,v)&&v==WhBase()+rva;}
static void* EntityById6694(std::uint32_t id){return id==42?entity.data():nullptr;}
static void* NativeHeldProxyT4(void* p) noexcept {++proxyCalls;return p==entity.data()?weaponProxy.data():p==playerEntity.data()?playerProxy.data():nullptr;}
static void* NativeHeldAttachmentFindT4(void* p,const char* n) noexcept{return p==model.data()&&n?attachment.data():nullptr;}
static bool GetEntityWorldTmV6552(void* p,std::uint32_t id,Matrix34V6550& tm){
 if(p!=entity.data()||id!=42)return false;tm={};tm.m[0]=tm.m[5]=tm.m[10]=1;return true;
}
#include "generated_held_capture63.inc"
static PhysEquipment6698 setup(unsigned profile){
 for(auto* p:{&player,&item,&entity,&link,&model,&attachment,&object,&manager,&character,&weaponProxy,&weaponBody,&playerEntity,&playerProxy,&playerBody})p->fill(0);
 immutable.clear();proxyCalls=0;retail=currentContext=retired=fixture63::enabled=true;g_native_held_valid_t4=false;
 PhysEquipment6698 e{};e.valid=e.handKnown[0]=e.handKnown[1]=true;e.handKey[0]=77;e.player=player.data();e.manager=manager.data();
 auto& x=e.items[0];x.held=x.boundsValid=true;x.item=item.data();x.entity=entity.data();x.worldLink54=link.data();
 auto& p=x.profile;p.key=77;p.generation=8;p.entity=42;p.classId=4;p.side=0;p.weight=1.5;
 x.bounds[0]=-.02f;x.bounds[1]=0;x.bounds[2]=-.02f;x.bounds[3]=.02f;x.bounds[4]=1;x.bounds[5]=.02f;
 put(player,0,WhBase()+0x3AA4B98);put(item,0,WhBase()+0x3A2DA88);put(entity,0,WhBase()+0x4072C78);put(playerEntity,0,WhBase()+0x4072C78);
 put(item,0x88,static_cast<void*>(link.data()));put(link,0,WhBase()+0x3EC85E0);put(link,0x38,static_cast<void*>(entity.data()));
 put(link,0x58,static_cast<void*>(item.data()));put(link,0x30,std::uint32_t(42));put(link,0x78,profile);put(link,0x7C,profile);put(link,0x75,std::uint8_t(0));
 put(player,0x1B0,static_cast<void*>(model.data()));put(model,0x20,static_cast<void*>(player.data()));put(model,0x2C,std::uint32_t(42));
 fixed(WhBase()+0x40AC288,static_cast<const char*>("hand_right"));fixed(WhBase()+0x4072C78+0x250,WhBase()+0x5D70C4);
 put(attachment,0,WhBase()+0x3A657C0);put(attachment,0x20,static_cast<void*>(object.data()));put(attachment,0x28,static_cast<void*>(manager.data()));
 put(manager,0x70,static_cast<void*>(character.data()));put(character,0,WhBase()+0x40602F8);put(object,0,WhBase()+0x3A68F68);put(object,8,std::uint32_t(42));
 for(unsigned i=0;i<3;++i)put(object,0xC+4*i,1.f);
 put(player,0x38,static_cast<void*>(playerEntity.data()));
 put(weaponProxy,0,WhBase()+0x3AC3478);put(weaponProxy,0x28,0x40u);put(weaponProxy,0x38,static_cast<void*>(weaponBody.data()));
 put(weaponBody,0x44,23u);put(weaponBody,0x20,static_cast<void*>(entity.data()));put(weaponBody,0x28,std::int16_t(2));
 put(playerProxy,0,WhBase()+0x3AC3478);put(playerProxy,0x38,static_cast<void*>(playerBody.data()));
 put(playerBody,0x44,24u);put(playerBody,0x20,static_cast<void*>(playerEntity.data()));put(playerBody,0x28,std::int16_t(2));
 return e;
}
static unsigned checks=0;static void check(bool v){++checks;assert(v);}
int main(){
 {auto e=setup(0);g_native_held_equipment_pending_h36=true;NativeHeldCaptureT4(e,1000);
  check(!g_native_held_valid_t4&&g_native_held_reason_t4==HeldPhysicsSourceT4::Context&&proxyCalls==0);
  g_native_held_equipment_pending_h36=false;}
 for(unsigned profile:{0u,2u,4u}){
  auto e=setup(profile);NativeHeldCaptureT4(e,1000);NativeHeldSourceT4 source{};
  check(g_native_held_valid_t4&&g_native_held_source_t4.read(source)&&source.valid);
  check(source.leftNativeMembershipH36==19&&source.requested==profile&&source.applied==profile&&source.input.nativeBody==reinterpret_cast<std::uintptr_t>(weaponBody.data()));
  check(source.input.nativeBodyId==23&&source.input.nativeColliderExcluded&&g_native_held_reason_t4==HeldPhysicsSourceT4::Ready);
 }
 // Stable0 does not create a synthetic proof. An absent suspended source body,
 // absent suspension bit, foreign entity/type or bad ID refuses immediately.
 for(unsigned variant=0;variant<5;++variant){
  auto e=setup(0);
  if(variant==0)put(weaponProxy,0x38,static_cast<void*>(nullptr));
  if(variant==1)put(weaponProxy,0x28,0u);
  if(variant==2)put(weaponBody,0x20,static_cast<void*>(playerEntity.data()));
  if(variant==3)put(weaponBody,0x28,std::int16_t(1));
  if(variant==4)put(weaponBody,0x44,0x7FFFFFu);
  NativeHeldCaptureT4(e,1000);check(!g_native_held_valid_t4&&g_native_held_reason_t4==HeldPhysicsSourceT4::NativeProxySuspension);
 }
 for(unsigned unsupported:{1u,3u,5u,6u,255u,~0u}){
  auto e=setup(unsupported);NativeHeldCaptureT4(e,1000);
  check(!g_native_held_valid_t4&&g_native_held_reason_t4==HeldPhysicsSourceT4::NativeProfile&&proxyCalls==0);
 }
 auto e=setup(0);put(link,0x75,std::uint8_t(1));NativeHeldCaptureT4(e,1000);check(!g_native_held_valid_t4&&proxyCalls==0);
 e=setup(0);put(link,0x78,2u);NativeHeldCaptureT4(e,1000);check(!g_native_held_valid_t4&&proxyCalls==0);
 e=setup(0);put(attachment,0x20,static_cast<void*>(nullptr));NativeHeldCaptureT4(e,1000);check(!g_native_held_valid_t4&&g_native_held_reason_t4==HeldPhysicsSourceT4::Attachment);
 e=setup(0);currentContext=false;NativeHeldCaptureT4(e,1000);check(g_native_held_reason_t4==HeldPhysicsSourceT4::Context);
 std::printf("native_held_capture63: %u actual profile0/body/identity capture checks PASS; native physics-phase postcondition remains separate\n",checks);
}
