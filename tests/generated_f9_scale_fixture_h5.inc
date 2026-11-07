// Generated provider composition only; production capture/backend decisions unchanged.
static bool CapturedItemSourceH5(int,void*,const void*) noexcept;
static bool CapturedPlayerSourceH5(int,void*,const void*) noexcept;
#define KCD2VR_WEAPON_BODY_T4_HOST_TEST
#include "../src/native_weapon_body_t4.cpp"
#include "../src/native_weapon_clearance_t4.cpp"
#include <cassert>
#include <cstdio>
#include <new>
#include <string>

namespace fixture {
using namespace kcd2vr;
namespace p=native_physics_phase_t4;namespace w=native_weapon_body_t4;
bool phasePreflightOK=true;
std::uint64_t now=1000,serial=1,generation=1;
bool current=true,suspended=true,living=true,predicate=true,pending=true,alive=false,
    geometry=false,collision=false,filters=true,install=true,clearRevokes=false,motionRevokes=false,createDisable=false,
    postEffectRefuse=false,postEffectFault=false;
unsigned creates=0,adds=0,enables=0,removes=0,destroys=0,releases=0,queries=0,velocities=0,unregisters=0;
unsigned livingCalls=0,revokeAt=0,registerCalls=0,phaseInstallCalls=0;
bool registerOK=true,phaseInstallOK=true,clearanceInstallOK=true;
float clearResult=0;double mass=2;w::Pose body{};p::V3 linear{},angular{};
p::Client client{};int bodyCookie=0,geometryCookie=0;
void* bodyPointer(){return &bodyCookie;}void* geometryPointer(){return &geometryCookie;}
unsigned suspendAtSourceH36=0,sourceCallsH36=0;
bool Source(const w::Input&) noexcept {++sourceCallsH36;if(suspendAtSourceH36&&sourceCallsH36==suspendAtSourceH36)w::SetEquipmentAdmissionSuspendedH36(true);return predicate;}
w::Input Input(){w::Input in{};in.owner={1,2,3,4,0x100,0x200,0x300,0x400,0x500,0x600,7,0};
    in.weaponWorld.p={10,20,30};in.targetWorld=in.weaponWorld;in.boundsMin={-.04,-.2,-.03};
    in.boundsMax={.04,.8,.03};in.mass=2;in.scale=1;in.sample=1;in.tick=now;in.trackingEpoch=9;
    in.intentFrameH28.valid=true;in.intentFrameH28.scale=1.25;in.intentFrameH28.modelWorld.p={1,2,3};in.intentFrameH28.trackingFromModel.p={.1,.2,.3};
    in.cancelGeneration=8;in.colliderLease=10;in.classId=1;in.allowed=in.tracked=in.emptyLeft=in.nativeColliderExcluded=true;
    in.nativeBody=0x700;in.nativeProxy=0x800;in.nativeBodyId=11;
    in.playerBody=0x900;in.playerEntityPointer=0xA00;in.playerBodyId=12;return in;}
}
namespace kcd2vr::native_physics_phase_t4 {
struct Access {
    static Context& Ctx(){static Context context;context.world_=reinterpret_cast<void*>(0x10000);
        context.base_=0x180000000;context.generation_=fixture::generation;context.serial_=fixture::serial;
        context.phase_=Phase::Step;return context;}
    static void PhaseOf(Phase phase){Ctx().phase_=phase;}
    static void Set(Lease& l,bool value){l.held_=value;l.pending_=value&&fixture::pending;l.id_=value?42:-1;
        l.body_=value?fixture::bodyPointer():nullptr;l.world_=reinterpret_cast<void*>(0x10000);l.generation_=fixture::generation;}
    static void Pending(Lease& l,bool value){l.pending_=value;}
};
bool Context::Current()const noexcept{return fixture::current;}
void* Context::World()const noexcept{return world_;}
std::uintptr_t Context::ModuleBase()const noexcept{return base_;}
std::uint64_t Context::Generation()const noexcept{return generation_;}
std::uint64_t Context::Serial()const noexcept{return serial_;}
Phase Context::Kind()const noexcept{return phase_;}
bool RegisterClient(Client c)noexcept{++fixture::registerCalls;if(!fixture::registerOK)return false;fixture::client=c;return true;}
bool Preflight()noexcept{return fixture::phasePreflightOK;}
const char* InstallFailureStage()noexcept{return "TEST";}
bool Install()noexcept{++fixture::phaseInstallCalls;return fixture::phaseInstallOK;}
void RequestShutdown()noexcept{}
Receipt ReadReceipt()noexcept{return {};}
bool AcquireRigid(Context&,int,void*,Lease&)noexcept{return false;}
bool RigidEntityIdentity(Context&,Lease&,const void*,std::uint32_t)noexcept{return false;}
bool AdoptCreatedRigid(Context&,void* b,const void* foreign,int type,Lease& l)noexcept{
    assert(foreign==native_weapon_body_t4::record.foreignCookieH24&&foreign);
    assert(type>=0&&type<=32767);if(b!=fixture::bodyPointer())return false;Access::Set(l,true);return true;}
Registry ObserveRegistry(Context&,const Lease&)noexcept{return fixture::alive?Registry::Same:Registry::Retired;}
bool ReadyRigid(Context&,Lease& l)noexcept {Access::Pending(l,fixture::pending);return l.Held()&&fixture::alive&&!fixture::pending;}
bool Release(Context&,Lease& l)noexcept {++fixture::releases;Access::Set(l,false);return true;}
bool AbandonInvalidated(Context&,Lease& l)noexcept {Access::Set(l,false);return true;}
bool ReadRigid(Context&,Lease&,RigidSnapshot& out)noexcept{
    if(!fixture::alive||fixture::pending||!fixture::geometry)return false;
    out.position={float(fixture::body.p.x),float(fixture::body.p.y),float(fixture::body.p.z)};
    out.orientation={float(fixture::body.q.x),float(fixture::body.q.y),float(fixture::body.q.z),float(fixture::body.q.w)};
    out.mass=float(fixture::mass);out.scale=1;out.active=true;out.simulationClass=2;
    out.linearVelocity=fixture::linear;out.angularVelocity=fixture::angular;return true;}
bool SetVelocity(Context&,Lease&,V3 v,V3 w)noexcept {
    ++fixture::velocities;fixture::linear=v;fixture::angular=w;return true;}
bool ReadConstraint(Context&,Lease&,int,ConstraintSnapshot&)noexcept{return false;}
bool AcquireLivingH24(Context&,const LivingSourceH24&,LivingLeaseH24&) noexcept{return false;}
bool CurrentLivingH24(Context&,LivingLeaseH24&,const LivingSourceH24&) noexcept{return false;}
bool ReleaseLivingH24(Context&,LivingLeaseH24&) noexcept{return false;}
bool AbandonLivingH24(Context&,LivingLeaseH24&) noexcept{return false;}
ConstraintResult AddSelfIgnorePairH24(Context&,Lease&,LivingLeaseH24&) noexcept{return {};}
ConstraintResult RemoveSelfIgnorePairH24(Context&,Lease&,std::uint32_t) noexcept{return {};}
bool SelfIgnorePairPresentH24(Context&,Lease&,LivingLeaseH24&,std::uint32_t) noexcept{return false;}
ConstraintResult AddConstraint(Context&,Lease&,Lease&,const constraint_packet60::Packet&) noexcept{return {};}
ConstraintResult RemoveConstraint(Context&,Lease&,std::uint32_t) noexcept{return {};}
void* NativeHandle(Context&,Lease& l,bool allow)noexcept{
    return l.Held()&&fixture::alive&&(allow||!fixture::pending)?fixture::bodyPointer():nullptr;}
bool ObserveSuspended(Context&,int id,void* body,const void* entity)noexcept {
    return fixture::suspended&&CapturedItemSourceH5(id,body,entity);}
bool ObserveLivingExcluded(Context&,int id,void* body,const void* entity)noexcept {
    ++fixture::livingCalls;
    if(fixture::motionRevokes&&fixture::livingCalls>=fixture::revokeAt){fixture::predicate=false;fixture::motionRevokes=false;}
    return fixture::living&&CapturedPlayerSourceH5(id,body,entity);}
}
namespace kcd2vr::retail_build112 {bool Ready(std::uintptr_t) noexcept{return false;}}
namespace kcd2vr::native_weapon_body_t4 {namespace {
std::uint64_t HostNow()noexcept{return fixture::now;}
bool HostInstall()noexcept{return fixture::install;}
bool HostBind(phase::Context&,void*& m)noexcept{m=reinterpret_cast<void*>(0x20000);return true;}
void* HostCreate(phase::Context&,const Pose& p)noexcept{
    ++fixture::creates;fixture::alive=true;fixture::pending=true;fixture::body=p;
    if(fixture::createDisable){SetEnabled(false);Achieved out{};assert(Read(out)&&!out.safeNativeRestore&&!out.colliderRetired);}
    return fixture::bodyPointer();}
void* HostGeometry(phase::Context&,const policy::Shape&,void*)noexcept{return fixture::geometryPointer();}
bool HostAddGeometry(phase::Context&,void*,void*,const policy::Shape& s,bool c)noexcept{
    ++fixture::adds;if(c)++fixture::enables;fixture::geometry=true;fixture::collision=c;fixture::mass=s.mass;
    if(c&&fixture::postEffectFault){fault.store(true);return false;}
    return !(c&&fixture::postEffectRefuse);}
bool HostRemoveGeometry(phase::Context&,void*)noexcept{++fixture::removes;fixture::geometry=false;fixture::collision=false;return true;}
bool HostPart(phase::Context&,void*,void*,bool c,double m)noexcept{return fixture::geometry&&fixture::collision==c&&fixture::mass==m;}
bool HostConfigure(phase::Context&,void*)noexcept{const auto p=ContactFlagsH24();assert(p.type==15&&p.flags==0x80000000u&&p.flagsAND==0xFFFFFFFFu&&ContactFlagsCurrentH24(p.flagsOR));return true;}
bool HostFilters(phase::Context&,void*)noexcept{return fixture::filters;}
bool HostPairClassH5(phase::Context&,void*,unsigned ignore)noexcept{return ignore==4;}
bool HostUnregister(phase::Context&,void*,void*)noexcept{++fixture::unregisters;return true;}
int HostDestroy(phase::Context&,void*)noexcept{++fixture::destroys;fixture::alive=false;return 1;}
}}
namespace kcd2vr::native_weapon_clearance_t4 {namespace {
bool HostClearanceInstall()noexcept{return fixture::clearanceInstallOK;}
float HostClearance(native_physics_phase_t4::Context&,Capsule,void*)noexcept {
    ++fixture::queries;if(fixture::clearRevokes)fixture::predicate=false;return fixture::clearResult;}
}}
namespace fixture {
void Tick(){++serial;auto& c=p::Access::Ctx();client.step(c,nullptr);}
void Reset(){auto& c=p::Access::Ctx();p::Access::PhaseOf(p::Phase::WorldReset);client.reset(c,nullptr);alive=false;geometry=false;collision=false;}
void Fresh(w::Input& in){in.tick=now;in.sample++;w::Submit(in);}
void Advance(w::Input& in,unsigned ms=10){now+=ms;Fresh(in);Tick();}
void Cleanup(){w::SetEnabled(false);Tick();pending=false;Tick();Tick();assert(!alive);}
void Restart(){
    Cleanup();predicate=suspended=living=filters=true;clearResult=0;clearRevokes=motionRevokes=createDisable=false;
    postEffectRefuse=postEffectFault=false;pending=true;linear={};angular={};w::SetEnabled(true);
}
void Active(w::Input& in){Fresh(in);Tick();assert(alive&&pending&&!collision);pending=false;
    Advance(in);assert(geometry&&!collision);Advance(in);assert(collision);
    w::Achieved out{};assert(w::Read(out)&&out.poseValid&&out.state==w::State::Active&&!out.safeNativeRestore&&!out.colliderRetired);
    assert(pose_core::length(out.weaponWorld.p-in.weaponWorld.p)<1e-5);
}
}
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
static Matrix34V6550 sourceWorldH5{};
static bool GetEntityWorldTmV6552(void* p,std::uint32_t id,Matrix34V6550& tm){
 if(p!=entity.data()||id!=42)return false;tm=sourceWorldH5;return true;
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
