#include "../src/ui_scene_presentation.h"
#include <cassert>
using namespace kcd2vr;
int main(){
 using namespace ui_native_state;using namespace ui_menu_router;
 Router r;Snapshot s{};s.known=Bit(Menu)|Bit(Hud);s.visible=Bit(Hud);r.Resolve(s);
 r.Request(Owner::InventoryMap);r.Resolve(s);
 assert(!ui_scene_presentation::Resolve(r.Read(),false).theater);
 s.known|=Bit(Inventory);s.visible|=Bit(Inventory);r.Resolve(s);
 assert(ui_scene_presentation::Resolve(r.Read(),false).theater);
 s.visible&=~Bit(Inventory);r.Resolve(s);
 assert(!ui_scene_presentation::Resolve(r.Read(),false).theater);
 for(auto k:{SkipTime,LockPicking,Pickpocketing,ItemSelection,ItemTransfer,CraftList,Modal}){
   s.known|=Bit(k);s.visible|=Bit(k);r.Resolve(s);
   assert(!ui_scene_presentation::Resolve(r.Read(),false).theater);
   assert(!ui_scene_presentation::Resolve(r.Read(),true).theater);
   s.visible&=~Bit(k);r.Resolve(s);
 }
 s.known|=Bit(Shop);s.visible|=Bit(Shop);r.Resolve(s);
 assert(ui_scene_presentation::Resolve(r.Read(),false).theater);
 s.visible&=~Bit(Shop);r.Resolve(s);assert(!ui_scene_presentation::Resolve(r.Read(),false).theater);
 assert(ui_scene_presentation::Resolve(r.Read(),true).theater); // actual cutscene
 for(bool cinematic:{false,true}){
   assert(ui_scene_presentation::Resolve(r.Read(),ui_scene_presentation::AuthoredScene(cinematic,true,0)).theater);
   assert(!ui_scene_presentation::Resolve(r.Read(),ui_scene_presentation::AuthoredScene(cinematic,true,1)).theater);
   assert(!ui_scene_presentation::Resolve(r.Read(),ui_scene_presentation::AuthoredScene(cinematic,true,2)).theater);
 }
 s.visible|=Bit(Menu);r.Resolve(s);
 assert(ui_scene_presentation::Resolve(r.Read(),true).pause_overlay);
 assert(!ui_scene_presentation::Resolve(r.Read(),true).theater);
}
