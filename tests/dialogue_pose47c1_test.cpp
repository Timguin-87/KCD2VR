#include "dialogue_pose47c1.h"
#include "vrik/visual_selection59c7f.h"
#include "npc_target47c1.h"
#include <cassert>
#include <cstring>
#include <array>
#include <cstdint>
using namespace kcd2vr;
int main(){
 dialogue_pose47c1::Context c{};
 for(int census:{2,3,4,5})assert(visual_selection59c7f::Selected(true,census,0));
 assert(visual_selection59c7f::Selected(true,0,3));
 assert(!visual_selection59c7f::Selected(true,0,0));
 assert(!visual_selection59c7f::Selected(true,1,0));
 assert(!visual_selection59c7f::Selected(false,4,0));
 c.enabled=c.vrik=c.dialogue=true;c.cameraMode=2;
 // Ordinary immersive dialogue: UI is gameplay, even though native input is locked.
 assert(dialogue_pose47c1::Allow(c));
 for(auto o:{ui_menu_router::Owner::InventoryMap,ui_menu_router::Owner::Shop,
  ui_menu_router::Owner::Pause,ui_menu_router::Owner::LoadingScreen,
  ui_menu_router::Owner::GameOver,ui_menu_router::Owner::NativeContext}){
  c.ui.owner=o;assert(!dialogue_pose47c1::Allow(c));
 }c.ui.owner=ui_menu_router::Owner::None;
 for(int mode:{0,1}){c.cameraMode=mode;assert(!dialogue_pose47c1::Allow(c));}c.cameraMode=2;
 for(bool* p:{&c.enabled,&c.vrik,&c.dialogue}){*p=false;assert(!dialogue_pose47c1::Allow(c));*p=true;}
 for(bool* p:{&c.theater,&c.matrix,&c.vrMenu,&c.nativeReference,&c.activity}){*p=true;assert(!dialogue_pose47c1::Allow(c));*p=false;}
 c.ui.uncertain=1;assert(!dialogue_pose47c1::Allow(c));c.ui.uncertain=0;
 constexpr std::uintptr_t base=0x10000000;
 alignas(8) std::array<unsigned char,64> puppet{};std::uintptr_t local=base+0x4072C78,other=local;
 std::uintptr_t vf=base+npc_target47c1::AiPuppet;void* ent=&local;
 std::memcpy(puppet.data(),&vf,8);std::memcpy(puppet.data()+0x38,&ent,sizeof ent);
 bool fail=false;
 auto read=[&](void* p,auto& out){
  if(fail)return false;
  if(p==reinterpret_cast<void*>(vf+0x18)){std::uintptr_t g=base+0x602360;std::memcpy(&out,&g,sizeof out);return true;}
  if(p==puppet.data()||p==puppet.data()+0x38||p==&local||p==&other){std::memcpy(&out,p,sizeof out);return true;}
  return false;
 };
 void* target=nullptr;
 assert(npc_target47c1::Resolve(read,base,puppet.data(),&local,target)&&target==&local);
 assert(!npc_target47c1::Resolve(read,base,puppet.data(),&other,target));
 ent=&other;std::memcpy(puppet.data()+0x38,&ent,sizeof ent);
 assert(!npc_target47c1::Resolve(read,base,puppet.data(),&local,target)); // NPC / unproven twin never retargeted
 ent=&local;std::memcpy(puppet.data()+0x38,&ent,sizeof ent);
 vf=base+npc_target47c1::SOPuppet;std::memcpy(puppet.data(),&vf,8);
 assert(npc_target47c1::Resolve(read,base,puppet.data(),&local,target));
 vf=base+0x3a3bc90;std::memcpy(puppet.data(),&vf,8); // actor is not a puppet
 assert(!npc_target47c1::Resolve(read,base,puppet.data(),&local,target));
 assert(npc_target47c1::Resolve(read,base,&local,&local,target));
 fail=true;assert(!npc_target47c1::Resolve(read,base,&local,&local,target));
 return 0;
}
