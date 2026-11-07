// Actual successor adapter, exact native memory/status leaves doubled. This
// does not run a world query, load retail code or prove a headset impalement.
#define main H7FixtureUnusedMain
#include "corpse_contact_native_h7_test.cpp"
#undef main
#include "subsystems/gore/impale_constraint_h21.h"
#include "vrik/contact_authority176.h"
#include "core/collab_test_h21.h"
static kcd2vr::native_weapon_body_t4::Achieved achievedH21{};static bool achievedReadH21=true;static unsigned submittedH21=0;
namespace kcd2vr::native_weapon_body_t4 {bool Read(Achieved& out)noexcept{out=achievedH21;return achievedReadH21;}}
namespace kcd2vr::impale_constraint_h21 {
bool Submit(const Input& in)noexcept{assert(in.source.physicsId==-1&&in.source.part==379&&in.source.foreignType==2&&in.nativeDeadCurrent);++submittedH21;return true;}
void Refuse(const char*)noexcept{}
}
namespace npc172=kcd2vr::npc_contact172;
static std::atomic<void*> g_last_player_entity{nullptr};
static std::atomic<void*> g_henry_item_attach_manager{nullptr};static bool solverClearH18=true;
struct PhysFrame6698{void* player=nullptr;void* manager=nullptr;std::array<ph::Profile,2> profiles{};ph::Input input{};std::uint64_t request=0;};
static bool NativeHeldContactsCurrentT4(){return solverClearH18;}
static bool PhysVft6698(void* p,std::uintptr_t rva){std::uintptr_t vf=0;return p&&Read(p,vf)&&vf==base+rva;}
static bool NativeHandKey6697(void* manager,int side,std::uint64_t& key){return side>=0&&side<2&&Read(static_cast<unsigned char*>(manager)+0x100+side*8,key);}
namespace retail_build112 {static bool Ready(std::uintptr_t b)noexcept{return b==base;}}
namespace kcd2vr::frik_pose_probe {
#include <impale_successor_h18_native.inc>
}
static void successorSetup(){setup();f::g_corpse_contact_fault_h7.clear();g_last_player_entity=nullptr;
 put(proxy+0x38,body);put(body+0x28,std::int16_t(2));put(body+0x20,ent);
 put(base+0x3A48B30+0x50,base+0x5D6F44);
 put(base+0x5D6F44,std::array<unsigned char,13>{0x0F,0xBF,0x41,0x28,0x3B,0xC2,0x75,0x05,0x48,0x8B,0x41,0x20,0xC3});}
static ph::Contact103 contact(){ph::Contact103 c{};c.known=c.plane=true;c.fraction=.5;c.point={0,.7,0};
 auto& w=c.native172;w.mask=npc172::BodiesAndWorld;w.ownerRead=true;w.body=npc172::Body::Articulated;
 w.owner=ent;w.entityId=456;w.collider=body;w.part=379;return c;}
int main(){using pa=kcd2vr::partanchor59c7g::Identity;
 const pa old{1,2,3,456,ent,oldBody,390};pa fresh{};ph::Pose pose{};double scale=0;const char* why="";
 successorSetup();assert(f::ReadImpaleSuccessorH18(old,contact(),true,fresh,pose,scale,why));
 assert(fresh.body==body&&fresh.part==379&&fresh.serial==old.serial&&fresh.epoch==old.epoch&&queries==1&&oldReads==0);
 // Follow-up also revalidates exact CURRENT root/main, native kind2 and raw part.
 assert(f::ReadImpaleCurrentCorpseH18(fresh,pose,scale,why)&&queries==2);
 successorSetup();assert(!f::ReadImpaleSuccessorH18(old,contact(),false,fresh,pose,scale,why)&&queries==0);
 for(int x=0;x<8;++x){successorSetup();auto c=contact();
  if(x==0)c.native172.owner=ent+1;if(x==1)c.native172.entityId++;
  if(x==2)c.native172.collider=oldBody;if(x==3)c.native172.part=-1;
  if(x==4)c.native172.body=npc172::Body::Living;if(x==5)c.known=false;
  if(x==6)c.limited115=true;if(x==7)c.native172.mask=npc172::StaticTerrain;
  assert(!f::ReadImpaleSuccessorH18(old,c,true,fresh,pose,scale,why)&&queries==0&&oldReads==0);
 }
 for(int x=0;x<7;++x){successorSetup();
  if(x==0)put(body+0x28,std::int16_t(1));if(x==1)put(body+0x20,ent+1);
  if(x==2)put(ci+0x7B4,std::uint8_t(0));if(x==3)put(proxy+0x38,rootBody);
  if(x==4)put(ci+0x7A0,rootBody);if(x==5)put(base+0x5D6F44,std::uint8_t(0));
  if(x==6)put(base+0x3A48B30+0x50,base+0x5D6F45);
  assert(!f::ReadImpaleSuccessorH18(old,contact(),true,fresh,pose,scale,why)&&queries==0&&oldReads==0);
 }
 for(int x:{1,2,3,5}){successorSetup();mode=x;
  assert(!f::ReadImpaleSuccessorH18(old,contact(),true,fresh,pose,scale,why)&&queries==1&&oldReads==0&&!f::GeometryCorpseContactFaultH7());}
 successorSetup();mode=4;assert(!f::ReadImpaleSuccessorH18(old,contact(),true,fresh,pose,scale,why)&&f::GeometryCorpseContactFaultH7());
 const auto before=queries;assert(!f::ReadImpaleSuccessorH18(old,contact(),true,fresh,pose,scale,why)&&queries==before);
 // Actual held-weapon check: same pinned native manager/key relationship as
 // ordinary input. Old profiles cannot outlive a sheath, swap or foreign owner.
 for(int n=0;n<8;++n){successorSetup();constexpr std::uintptr_t pl=0xB000,mg=0xA000,henry=0xC000;
  PhysFrame6698 frame{};frame.player=reinterpret_cast<void*>(pl);frame.manager=reinterpret_cast<void*>(mg);frame.profiles[0].key=81;
  g_last_player_entity=reinterpret_cast<void*>(henry);g_henry_item_attach_manager=reinterpret_cast<void*>(mg);solverClearH18=true;
  put(pl,base+0x3AA4B98);put(pl+0x38,henry);put(henry,base+0x4072C78);put(pl+0x9D0,mg);
  put(mg,base+0x3EC9958);put(mg+0x38,pl);put(mg+0x100,std::uint64_t(81));
  if(n==1)put(mg+0x100,std::uint64_t(0));if(n==2)put(mg+0x100,std::uint64_t(82));
  if(n==3)put(mg+0x38,pl+1);if(n==4)g_henry_item_attach_manager=nullptr;
  if(n==5)put(pl+0x9D0,mg+1);if(n==6)g_last_player_entity=nullptr;if(n==7)solverClearH18=false;
  assert(f::NativeImpaleWeaponCurrentH18(frame,0)==(n==0));
  if(n==0){
   using namespace kcd2vr::native_weapon_body_t4;
   frame.request=3;frame.input.bodyEpoch=2;frame.input.tick=1000;frame.profiles[0].generation=8;
   achievedH21={};achievedH21.poseValid=true;achievedH21.state=State::Active;
   achievedH21.owner.key=81;achievedH21.owner.generation=8;achievedH21.owner.request=3;achievedH21.owner.epoch=2;
   achievedH21.bodyGeneration=9;achievedH21.sourceTick=1000;
   const pa current{1,2,3,456,ent,body,379};const ph::Vec3 grip{},tip{0,1,0},wound{0,.7,0};
   assert(!f::StageImpaleConstraintH21(frame,current,0,grip,tip,wound,1000)&&submittedH21==0);
   kcd2vr::collab_h21::setPhysicsMode(3);
   assert(f::StageImpaleConstraintH21(frame,current,0,grip,tip,wound,1000)&&submittedH21==1);
   achievedH21.owner.key=82;
   assert(!f::StageImpaleConstraintH21(frame,current,0,grip,tip,wound,1000)&&submittedH21==1);
   achievedH21.owner.key=81;achievedH21.sourceTick=900;
   assert(!f::StageImpaleConstraintH21(frame,current,0,grip,tip,wound,1000)&&submittedH21==1);
   achievedH21.sourceTick=1000;put(body+0x28,std::int16_t(1));
   assert(!f::StageImpaleConstraintH21(frame,current,0,grip,tip,wound,1000)&&submittedH21==1);
  }
 }
 std::puts("PASS H18 actual successor adapter: NEW raw part only, native kind2/current root+main/death/relinquish, before/after identity, exact foreign/status pins and exception quarantine");
}
