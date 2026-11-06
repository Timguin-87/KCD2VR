// R94: the actual static_grab_r89.inl lifecycle and body/owner validation.
// tools/run_static_prop_r94.py substitutes only the fixed-address GetProxy
// invocation. All fields, guards, acquisition, servo, release and contact
// consumers below are the production implementation, with explicit leaf spies.
#include "vrik/static_grab_r89.h"
#include "core/mutual_admission_t25.h"
#include "core/impact_identity59c6.h"
#include "subsystems/physics/touch_react_service60.h"
#include "subsystems/physics/runtime/native_grab_producer60.h"
#include "diagnostics/capture168.h"
#include "vrik/lab_prop58.h"
#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <limits>
#ifndef _WIN32
#define __try try
#define __except(x) catch(...)
#define __fastcall
#define EXCEPTION_EXECUTE_HANDLER 1
#endif
namespace ph=kcd2vr::physical;namespace pm=kcd2vr::pose_core;namespace prop5916=kcd2vr::prop5916c1;
namespace tracking59c6 {inline std::atomic<std::uint64_t> epoch{12};}
static std::uint64_t clockR94=1000;
static std::uint64_t GetTickCount64(){return clockR94;}
static unsigned GetCurrentThreadId(){return 77;}
struct V3 {float x,y,z;};struct Q4 {float x=0,y=0,z=0,w=1;};
static float Len(V3 v){return std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z);}
static V3 Normalize(V3 v){const auto n=Len(v);return {v.x/n,v.y/n,v.z/n};}
static Q4 QFromBasis(V3,V3,V3){return {};}
struct Matrix34V6550 {float m[12]{1,0,0,0,0,1,0,0,0,0,1,0};};
static constexpr std::uintptr_t kCEntityVftRva=0x4072C78;
static std::uintptr_t WhBase(){return 0x100000000ull;}
struct CommittedFrame6695 {
 struct {std::uint64_t sample=1,tick=1000,actor=0xABC,epoch=9;} stamp;
 bool valid[2]{true,true},propRequestedValid5916c1[2]{true,true},mapFoldTrackedM39=true;
 ph::Pose propRequested5916c1[2]{};float mapGripM39[2]{.8f,.8f},pickupTriggerD80[2]{};
 std::uint64_t doorTrackingEpoch66=12,publicationR24=4;
};
static CommittedFrame6695 committedR94{};
struct PhysFrame6698 {ph::Input input{};ph::Output output{};ph::Profile profiles[2]{};
 std::uintptr_t characterT23=0;std::uint64_t trackingEpochT23=0,publicationT23=0;void* manager=nullptr;};
static kcd2vr::core::SnapshotMailbox<PhysFrame6698> g_phys_frame6698;
static std::atomic<unsigned> g_stage_thread596{77};
static std::atomic<std::uint64_t> g_stage_tick596{1000};
static std::atomic<bool> g_committed_pose_valid6689{true},g_prop_fault5916c1{false};
static bool g_prop_replay596=false;
static std::atomic<void*> g_henry_item_attach_manager{reinterpret_cast<void*>(0xDDD)};
static std::uint64_t weaponKeysR94[2]{100,101};
static bool NativeHandKey6697(void*,int side,std::uint64_t& key){key=weaponKeysR94[side];return true;}
static bool PhysEmpty6698(std::uint64_t key){return !key;}
static std::uint64_t ReadCompletedPublicationT23(){return committedR94.publicationR24;}
static std::atomic<std::uint64_t> g_pose_epoch6689{9},g_contact_control_epoch6695{5};
static std::atomic<void*> g_committed_character6689{reinterpret_cast<void*>(0xABC)},g_last_player_entity{nullptr};
static int velocityWrites=0,luaCalls=0,frameReads=0;
static bool luaPass=true,promotionConfirmed=true,contextOkay=true,handFree[2]{true,true},tracked[2]{true,true},pinsOkay=true,replaceOnVelocity=false;
static bool pickupOutstanding=false;static std::vector<std::string> rows,chunks;
namespace diagnostic_trace {void Write(const char* row){rows.emplace_back(row);}}
namespace kcd2vr::visible_failure {inline bool safe(const char*){return true;}}
namespace action_time_owner59c11 {inline bool NativePickupOutstandingD72(){return pickupOutstanding;}}
namespace kcd2vr::physics::grab_producer60 {
inline Status testStatusR94[2]{};
bool ReadStatus(int side,Status& s) noexcept {if(side<0||side>1)return false;s=testStatusR94[side];return true;}
}
struct FakeEntityR94 {
 alignas(16) unsigned char entity[0xC0]{},cls[0x30]{},proxy[0x50]{},body[2][0x70]{};
 unsigned index=0;std::uint32_t id=7;const char* className="BasicEntity";bool present=true;
 Matrix34V6550 tm{};float bounds[6]{-.05f,-.05f,-.05f,.05f,.05f,.05f};
 prop5916::Dynamics dynamics{};
};
static FakeEntityR94 fixtures[2];
template<class T>static void put(void* p,std::size_t n,const T& v){std::memcpy(static_cast<unsigned char*>(p)+n,&v,sizeof(v));}
template<class T>static bool Read(const void* p,T& value){if(!p)return false;std::memcpy(&value,p,sizeof(value));return true;}
static FakeEntityR94* fixture(void* entity){for(auto& f:fixtures)if(f.entity==entity)return &f;return nullptr;}
static void bodySetup(FakeEntityR94& f,bool rigid){
 auto* b=f.body[f.index];put(b,0,WhBase()+(rigid?0x3A48DC0:0x3A578C0));
 put(b,0x20,static_cast<void*>(f.entity));put(b,0x28,short(2));put(f.proxy,0x38,static_cast<void*>(b));
 f.dynamics.mass=rigid?.35:0;f.dynamics.centre={};
}
static void* EntityById6694(std::uint32_t id){for(auto& f:fixtures)if(f.id==id&&f.present)return f.entity;return nullptr;}
static bool PhysVft6698(void* p,std::uintptr_t rva){std::uintptr_t v=0;return Read(p,v)&&v==WhBase()+rva;}
struct StageLeafPin596 {std::uintptr_t rva;unsigned char bytes[8];};
template<std::size_t N>static bool StageLeafPins596(const StageLeafPin596(&)[N]){return pinsOkay;}
static void* TestGetProxyR94(void* e,int which){auto* f=fixture(e);return f&&which==1?f->proxy:nullptr;}
static bool SmallPickupBodyR24(void* e,void*& body){auto* f=fixture(e);body=f?f->body[f->index]:nullptr;return f;}
static bool PropPhysicsPins5916C1(){return pinsOkay;}
namespace retail_build112 {static bool Ready(std::uintptr_t){return pinsOkay;}}
static bool PropDynamics5916C1(void* b,prop5916::Dynamics& d){for(auto& f:fixtures)if(f.body[f.index]==b){d=f.dynamics;return d.valid();}return false;}
static bool PropSetVelocity5916C1(void* b,pm::Vec3 v,pm::Vec3 w={}){
 ++velocityWrites;for(auto& f:fixtures)if(f.body[f.index]==b){f.dynamics.linear=v;f.dynamics.angular=w;
  if(replaceOnVelocity){f.index=1-f.index;bodySetup(f,true);}return true;}return false;
}
static bool PropHandFree596(int side){return side>=0&&side<2&&handFree[side];}
static bool HeldInteractionVisual43(){return false;}
struct PropLock596 {bool owned=true;};
static struct {std::uint32_t id=0;int side=0;void* body=nullptr;} g_prop596;
static bool EntityNameV178r19(void*,std::uint32_t,char* out,std::size_t n){std::snprintf(out,n,"tankard");return true;}
static bool GetEntityWorldTmV6552(void* e,std::uint32_t,Matrix34V6550& tm){auto* f=fixture(e);if(!f)return false;tm=f->tm;return true;}
static bool SmallPickupBoundsR24(void* e,void*& render,float (&bounds)[6]){auto* f=fixture(e);if(!f)return false;render=e;std::memcpy(bounds,f->bounds,sizeof(bounds));return true;}
static ph::Pose PropRequestedWorld5916C1(const CommittedFrame6695& f,int side){return f.propRequested5916c1[side];}
static bool PropInstantContextD78(std::uint64_t){return contextOkay;}
static bool TrackingHandUsable111(int side,std::uint64_t=0){return tracked[side];}
static bool TrackingHeadUsable111(std::uint64_t){return contextOkay;}
static bool ReadCommittedPacket6695(CommittedFrame6695& f){++frameReads;f=committedR94;return true;}
static bool fakeLua(const char* chunk) noexcept {
 ++luaCalls;chunks.emplace_back(chunk);if(!luaPass)return false;
 if(std::strstr(chunk,"PrepareSceneryR94(")&&std::strstr(chunk,",true)")){
  unsigned id=0;std::sscanf(std::strstr(chunk,"PrepareSceneryR94("),"PrepareSceneryR94(%u",&id);
  for(auto& f:fixtures)if(f.id==id&&promotionConfirmed){f.index=1-f.index;bodySetup(f,true);}
 }
 if(std::strstr(chunk,"TryPrepareContactR24(")&&promotionConfirmed){
  unsigned id=0;std::sscanf(std::strstr(chunk,"TryPrepareContactR24("),"TryPrepareContactR24(%u",&id);
  for(auto& f:fixtures)if(f.id==id){f.index=1-f.index;bodySetup(f,true);}
 }
 return true;
}
namespace gore32n {inline std::atomic<bool(*)(const char*) noexcept> g_lua_exec43{&fakeLua};}
#ifndef KCD2VR_STATIC_R94_ADAPTER
#error Build through tools/run_static_prop_r94.py to substitute the one native GetProxy call
#endif
using SmallContactEvidenceR24=kcd2vr::touch_react_service60::Evidence;
static bool ExecuteSmallContactR24(const SmallContactEvidenceR24&,const char*) noexcept;
#include KCD2VR_STATIC_R94_CONTACT_ADAPTER
static bool ExecuteSmallContactR24(const SmallContactEvidenceR24& e,const char* lua) noexcept {
 CommittedFrame6695 f{};return SmallWeaponSourceR94(e,f)&&fakeLua(lua);
}
#include KCD2VR_STATIC_R94_ADAPTER

namespace n=static_grab_r89n;namespace G=kcd2vr::static_grab_r89;namespace ma=kcd2vr::mutual_admission_t25;
static unsigned checks=0,failed=0;
static void check(bool ok,const char* what){++checks;if(!ok){++failed;std::printf("FAIL %s\n",what);}}
static void seed(bool rigid=false){
 n::g_held[0]={};n::g_held[1]={};n::g_entities={};n::g_contactBudgetR94={};
 for(int side=0;side<2;++side){
  ma::pickupClaimsD72[side]=0;ma::doorClaimsH41[side]=0;ma::active[side]=0;
  G::heldEntityR94[side]=0;G::pickupUseTick[side]=0;G::clearPresentationR94(side);
  n::g_edge[side]={};n::g_touch[side]={};n::g_contactMotionR94[side]={};n::g_impactSeenR94[side]={};
  kcd2vr::physics::grab_producer60::testStatusR94[side]={};handFree[side]=tracked[side]=true;
  auto& f=fixtures[side];f={};f.id=7+side;put(f.entity,0,WhBase()+kCEntityVftRva);
  put(f.entity,0x20,static_cast<void*>(f.cls));put(f.cls,0,WhBase()+0x3B282A0);put(f.cls,0x10,f.className);
  put(f.proxy,0,WhBase()+0x3AC3478);bodySetup(f,rigid);
 }
 committedR94=CommittedFrame6695{};clockR94=1000;g_stage_thread596=77;g_pose_epoch6689=9;g_contact_control_epoch6695=5;
 tracking59c6::epoch=12;g_prop596={};g_stage_tick596=1000;g_prop_fault5916c1=false;g_prop_replay596=false;g_committed_pose_valid6689=true;weaponKeysR94[0]=100;weaponKeysR94[1]=101;luaPass=promotionConfirmed=contextOkay=pinsOkay=true;
 pickupOutstanding=replaceOnVelocity=false;luaCalls=velocityWrites=frameReads=0;chunks.clear();rows.clear();G::enabled=true;
 kcd2vr::collab_h21::enabled=false;gore32n::g_lua_exec43=&fakeLua;
}
static void touch(int side,int which=0){const auto& f=fixtures[which];n::g_touch[side]={f.id,reinterpret_cast<std::uintptr_t>(f.entity),
 reinterpret_cast<std::uintptr_t>(f.body[f.index]),0xABC,clockR94,0,9,5,true};}
static void advance(std::uint64_t ms,double x=0){clockR94+=ms;g_stage_tick596=clockR94;committedR94.stamp.tick=clockR94;++committedR94.stamp.sample;committedR94.propRequested5916c1[0].p.x=x;}
static PhysFrame6698 weaponFrame(){
 PhysFrame6698 pf{};pf.input.allowed=pf.input.tracked[0]=pf.output.active[0]=true;
 pf.input.epoch=5;pf.input.bodyEpoch=9;pf.input.sample=committedR94.stamp.sample;pf.input.tick=clockR94;
 pf.characterT23=0xABC;pf.trackingEpochT23=12;pf.publicationT23=committedR94.publicationR24;pf.manager=g_henry_item_attach_manager.load();
 auto& p=pf.profiles[0];p.key=100;p.generation=200;
 auto& r=pf.output.impactRecord178r30[0];r.valid=true;r.entityId=7;r.collider=reinterpret_cast<std::uintptr_t>(fixtures[0].body[0]);
 r.key=p.key;r.generation=p.generation;r.epoch=9;r.request=5;r.sample=pf.input.sample;r.tick=clockR94;r.speed=1;
 r.body=int(kcd2vr::npc_contact172::Body::Physical);r.normal={-1,0,0};r.gesture59c3=12;r.part=0;return pf;
}
int main(){
 seed();touch(0);n::TryGrab(0,clockR94,committedR94);
 check(n::g_held[0].id==7&&G::ownsEntityR94(7)&&ma::pickupClaimsD72[0].load()!=0,"static entity promotes and claims only after native rigid readback");
 check(luaCalls==1&&velocityWrites==0&&fixtures[0].index==1,"acquire invokes preparation, never a transform/velocity placement");
 check(chunks[0].find("EnablePhysics")==std::string::npos,"acquire never disables native physics");
 touch(1);n::TryGrab(1,clockR94,committedR94);check(!n::g_held[1].id&&luaCalls==1,"second hand cannot acquire same entity");
 advance(10,.02);n::Carry(0,clockR94,&committedR94,.8f);
 check(velocityWrites==1&&fixtures[0].dynamics.linear.x>0&&fixtures[0].tm.m[3]==0,"raw controller target drives bounded velocity while achieved pose remains physical");
 n::Carry(0,clockR94,&committedR94,.8f);check(velocityWrites==1&&n::g_held[0].id,"duplicate sample preserves hold without repeated impulse");
 ++committedR94.stamp.sample;n::Carry(0,clockR94,&committedR94,.8f);check(velocityWrites==1&&n::g_held[0].id,"distinct sample at equal clock tick preserves hold without action");
 fixtures[0].tm.m[3]=.01f;fixtures[0].dynamics.angular={0,0,.7};advance(10,.03);n::Carry(0,clockR94,&committedR94,.8f);
 ph::Pose presentation{};check(G::selectPresentationR94(0xABC,9,clockR94,0,n::g_held[0].claim,presentation)&&std::fabs(presentation.p.x-.01)<1e-6,"presentation reports actual body instead of requested target");
 const auto before=fixtures[0].dynamics;const auto calls=velocityWrites;n::Carry(0,clockR94,&committedR94,0);
 check(!n::g_held[0].id&&!G::ownsEntityR94(7)&&!ma::pickupClaimsD72[0].load(),"release retires motor and matching claim");
 check(velocityWrites==calls&&luaCalls==1&&fixtures[0].dynamics.linear.x==before.linear.x&&fixtures[0].dynamics.angular.z==before.angular.z,"release preserves native linear and angular momentum without cleanup writes");
 check(!G::selectPresentationR94(0xABC,9,clockR94,0,123,presentation),"released presentation cannot revive an old grip");
 seed();promotionConfirmed=false;touch(0);n::TryGrab(0,clockR94,committedR94);
 check(!n::g_held[0].id&&!G::ownsEntityR94(7)&&!ma::pickupClaimsD72[0].load()&&!velocityWrites,"script completion without real rigid readback never starts hold");
 seed();luaPass=false;touch(0);n::TryGrab(0,clockR94,committedR94);check(!n::g_held[0].id&&!ma::pickupClaimsD72[0].load()&&luaCalls==1,"script refusal leaves no disabled-physics or restore debt");
 seed(true);touch(0);n::TryGrab(0,clockR94,committedR94);fixtures[0].index=1;bodySetup(fixtures[0],true);advance(10,.03);n::Carry(0,clockR94,&committedR94,.8f);
 check(!n::g_held[0].id&&!velocityWrites,"body replacement ends owner without action on old or new body");
 seed(true);touch(0);n::TryGrab(0,clockR94,committedR94);clockR94+=81;n::Carry(0,clockR94,&committedR94,.8f);
 check(!n::g_held[0].id&&!velocityWrites,"stale source cannot refresh a hold");
 seed(true);touch(0);n::TryGrab(0,clockR94,committedR94);handFree[0]=false;advance(10);n::Carry(0,clockR94,&committedR94,.8f);
 check(!n::g_held[0].id&&!velocityWrites,"native item taking hand ends scenery motor");
 seed();put(fixtures[0].proxy,0x28,std::uint32_t(0x40));touch(0);n::TryGrab(0,clockR94,committedR94);check(!luaCalls&&!n::g_held[0].id,"suspended native attached body is refused before script");
 seed();put(fixtures[0].body[0],0x20,static_cast<void*>(fixtures[1].entity));touch(0);n::TryGrab(0,clockR94,committedR94);check(!luaCalls,"foreign-owner mismatch cannot promote someone else's physics");
 seed();fixtures[0].bounds[3]=1.f;touch(0);n::TryGrab(0,clockR94,committedR94);check(!luaCalls,"oversize static furniture is never physicalized");
 seed();fixtures[0].className="PickableItem";put(fixtures[0].cls,0x10,fixtures[0].className);touch(0);n::TryGrab(0,clockR94,committedR94);check(!luaCalls,"inventory item stays on native pickup route");
 seed();fixtures[0].className="RigidBodyLight";put(fixtures[0].cls,0x10,fixtures[0].className);touch(0);n::TryGrab(0,clockR94,committedR94);check(n::g_held[0].id==7,"exact RigidBodyLight is admitted through actual acquisition");
 seed();kcd2vr::physics::grab_producer60::testStatusR94[1].state=kcd2vr::physics::grab_producer60::State::Held;kcd2vr::physics::grab_producer60::testStatusR94[1].owner.entity=7;touch(0);n::TryGrab(0,clockR94,committedR94);check(!luaCalls,"other hand constraint owner prevents second body motor");
 seed();touch(0);touch(1,1);n::TryGrab(0,clockR94,committedR94);n::TryGrab(1,clockR94,committedR94);check(n::g_held[0].id==7&&n::g_held[1].id==8,"two distinct props retain independent hands");
 n::Release(0,clockR94,"TEST");check(n::g_held[1].id==8&&ma::pickupClaimsD72[1].load(),"one release preserves other hand's owner");
 seed();G::enabled=false;n::Tick(clockR94);check(!frameReads&&!luaCalls&&!velocityWrites,"explicit feature OFF does no frame or physics polling");
 seed(true);touch(0);n::TryGrab(0,clockR94,committedR94);G::enabled=false;n::Tick(clockR94);check(!n::g_held[0].id&&!velocityWrites&&luaCalls==1,"OFF retires held motor without disabling or replacing physics");
 seed(true);n::React(7,reinterpret_cast<std::uintptr_t>(fixtures[0].entity),reinterpret_cast<std::uintptr_t>(fixtures[0].body[0]),0,{0,0,0},{-1,0,0},1,clockR94,committedR94);
 check(luaCalls==2&&chunks[1].find("PushPropR94")!=std::string::npos,"current free rigid prop receives bounded native point impulse");
 n::React(7,reinterpret_cast<std::uintptr_t>(fixtures[0].entity),reinterpret_cast<std::uintptr_t>(fixtures[0].body[0]),0,{0,0,0},{-1,0,0},1,clockR94,committedR94);check(luaCalls==2,"same contact cannot pump energy on repeated drain");
 seed(true);touch(0);n::TryGrab(0,clockR94,committedR94);n::React(7,reinterpret_cast<std::uintptr_t>(fixtures[0].entity),reinterpret_cast<std::uintptr_t>(fixtures[0].body[0]),1,{0,0,0},{-1,0,0},1,clockR94,committedR94);check(luaCalls==1,"held entity never receives independent push writer");
 seed();auto pf=weaponFrame();g_phys_frame6698.publish(pf);SmallContactEvidenceR24 ev{};CommittedFrame6695 cf{};
 check(SmallWeaponEvidenceR94(pf,0,ev)&&ev.sample.source==kcd2vr::passive169r1::Source::WeaponWorld&&SmallWeaponSourceR94(ev,cf),"weapon preparation retains exact WeaponWorld evidence");
 weaponKeysR94[0]=999;check(!SmallWeaponSourceR94(ev,cf),"current native weapon replacement rejects old impact");weaponKeysR94[0]=100;
 g_contact_control_epoch6695=6;check(!SmallWeaponSourceR94(ev,cf),"contact request reset rejects old impact");g_contact_control_epoch6695=5;
 auto changed=pf;changed.profiles[0].generation++;g_phys_frame6698.publish(changed);check(!SmallWeaponSourceR94(ev,cf),"changed profile generation invalidates synchronous preparation");g_phys_frame6698.publish(pf);
 G::enabled=false;check(!SmallWeaponSourceR94(ev,cf),"explicit OFF excludes new weapon preparation even with a record");G::enabled=true;
 void* prepared=nullptr;check(PrepareWeaponContactR94(pf,0,prepared)&&prepared==fixtures[0].body[1],"first static weapon contact can adopt confirmed new rigid body");
 seed();pf=weaponFrame();g_phys_frame6698.publish(pf);promotionConfirmed=false;check(!PrepareWeaponContactR94(pf,0,prepared)&&!prepared,"weapon script completion without rigid readback is refused");
 seed();pf=weaponFrame();fixtures[0].className="PickableItem";put(fixtures[0].cls,0x10,fixtures[0].className);g_phys_frame6698.publish(pf);
 n::WeaponContacts(clockR94,committedR94);check(luaCalls==2&&fixtures[0].index==1&&chunks[1].find("PushPropR94")!=std::string::npos,"first static Pickable weapon impact prepares then pushes without native pickup");
 n::WeaponContacts(clockR94,committedR94);check(luaCalls==2,"weapon impact cannot be replayed by another consumer drain");
 std::printf("static_prop_native_r94 %u/%u passed (native services substituted; no retail execution)\n",checks-failed,checks);return failed?1:0;
}
