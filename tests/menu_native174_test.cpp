#include "native_vr_pad_premap.h"
#include <windows.h>
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
int main(){
 ControllerState l{},r{};l.active=r.active=true;l.thumb_x=.83f;l.thumb_y=-.87f;r.thumb_x=-.72f;r.thumb_y=.92f;
 native_vr_pad_premap::Context c{};c.theater=true;c.apse_ui=true;
 auto p=native_vr_pad_premap::Build(l,r,c);
 assert(p.lx>0&&p.ly<0&&p.rx<0&&p.ry>0);
 assert(!p.lb&&!p.rb&&!p.dpad_up&&!p.dpad_left);
 r.quick_access_touch=true; p=native_vr_pad_premap::Build(l,r,c);
 assert(p.dpad_right&&p.dpad_down&&p.lx==0&&p.ly==0&&p.rx<0&&p.ry>0);
 r.quick_access_touch=false;c.theater=false;c.apse_ui=false;c.dialogue_navigation=true;
 p=native_vr_pad_premap::Build(l,r,c);assert(p.rx<0&&p.ry>0&&p.ly<0&&!p.r3);
 c.dialogue_navigation=false;c.system_menu=true;
 p=native_vr_pad_premap::Build(l,r,c);assert(p.rx<0&&p.ry>0);
 c.system_menu=false;c.semantic_minigame=true;
 p=native_vr_pad_premap::Build(l,r,c);assert(p.rx<0&&p.ry>0);
 // V66.178R6: Quest Left Menu inside the APSE hub / Shop / Crafting is the native close press (B),
 // never xi_back (KCD2 binds xi_back there to help_dialog/open_help_screen = the "zoom" overlay).
 {ControllerState l2{},r2{};l2.active=r2.active=true;native_vr_pad_premap::Context hub{};hub.theater=true;hub.apse_ui=true;hub.apse_hub=true;hub.back_context=true;
  native_vr_pad_premap::Reset();l2.menu_down=true;
  for(int i=0;i<10;++i){hostTickCount64+=16;auto q=native_vr_pad_premap::Build(l2,r2,hub);assert(q.b&&!q.back&&!q.rb);}
  l2.menu_down=false;auto q=native_vr_pad_premap::Build(l2,r2,hub);assert(!q.b&&!q.back);
  // a Left Menu press that started in gameplay stays quarantined in the hub, like a held B
  native_vr_pad_premap::Reset();native_vr_pad_premap::Context play{};l2.menu_down=true;
  q=native_vr_pad_premap::Build(l2,r2,play);assert(!q.b&&!q.back);
  q=native_vr_pad_premap::Build(l2,r2,hub);assert(!q.b&&!q.back);
  // gameplay B is untouched: Quick Talk shoulder, no close
  native_vr_pad_premap::Reset();l2.menu_down=false;r2.b_down=true;q=native_vr_pad_premap::Build(l2,r2,play);assert(!q.rb&&!q.b); // C45 default: B is not Quick Speak
  native_vr_pad_premap::SetBQuickTalk45(true);native_vr_pad_premap::Reset();q=native_vr_pad_premap::Build(l2,r2,play);assert(q.rb&&!q.b);native_vr_pad_premap::SetBQuickTalk45(false);}
 std::puts("PASS native XI both sticks across inventory, dialogue, system menu and minigame; modifier D-pad; R6 Left Menu = hub close");
}
