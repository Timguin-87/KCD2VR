#include "native_vr_pad_premap.h"
#include <cassert>
#include <cstdio>
#include <windows.h>
#include <limits>
using namespace kcd2vr;
int main(){
 ControllerState l{},r{};l.active=r.active=true;l.trigger=.8f;r.trigger=.7f;l.grip=r.grip=.9f;
 l.x_down=l.y_down=r.a_down=r.b_down=true;l.thumb_x=.6f;l.thumb_y=.8f;r.thumb_x=.7f;r.thumb_y=-.7f;
 native_vr_pad_premap::Context c{};c.semantic_minigame=true;c.back_context=true;
 native_vr_pad_premap::Reset();auto p=native_vr_pad_premap::Build(l,r,c);
 assert(p.lt==.8f&&p.rt==.7f&&p.lb&&p.rb&&p.a&&p.b&&p.x&&p.y);
 assert(p.lx>0&&p.ly>0&&p.rx>0&&p.ry<0);
 r.quick_access_touch=true;l.thumb_x=0;l.thumb_y=-1;p=native_vr_pad_premap::Build(l,r,c);
 assert(p.dpad_down&&p.lx==0&&p.ly==0&&p.lt==.8f);
 l.thumb_y=1;p=native_vr_pad_premap::Build(l,r,c);assert(p.dpad_up);
 l.thumb_y=0;l.thumb_x=-1;p=native_vr_pad_premap::Build(l,r,c);assert(p.dpad_left&&!p.dpad_right&&p.lx==0);
 l.thumb_x=1;p=native_vr_pad_premap::Build(l,r,c);assert(p.dpad_right&&!p.dpad_left);
 l.active=false;p=native_vr_pad_premap::Build(l,r,c);assert(!p.lb&&p.rb);l.active=true;
 c.semantic_minigame=false;c.theater=true;p=native_vr_pad_premap::Build(l,r,c);assert(p.lb&&p.rb);
 c.pointer_hand=0;p=native_vr_pad_premap::Build(l,r,c);assert(p.lt==0&&!p.lb&&p.rb);
 c.vr_menu=true;p=native_vr_pad_premap::Build(l,r,c);assert(p.lt==0&&p.rt==0&&!p.a&&!p.lb);
 c={};r.quick_access_touch=false;r.grip=1;r.thumb_x=.7f;r.thumb_y=0;
 c.grip_weapon_comparison114=false;p=native_vr_pad_premap::Build(l,r,c);assert(p.rx==0);
 c.grip_weapon_comparison114=true;p=native_vr_pad_premap::Build(l,r,c);assert(p.rx>0); // VRIK Grip must not consume turning
 // V168 execute the real premapper through both held-X context transitions.
 native_vr_pad_premap::Reset();l={};r={};l.active=r.active=true;c={};hostTickCount64=20000;
 l.x_down=true;p=native_vr_pad_premap::Build(l,r,c);assert(!p.dpad_down&&!p.dpad_up);
 hostTickCount64+=100;c.semantic_minigame=true;p=native_vr_pad_premap::Build(l,r,c);assert(p.x);
 hostTickCount64+=1000;c={};p=native_vr_pad_premap::Build(l,r,c);assert(!p.dpad_down&&!p.dpad_up);
 l.x_down=false;p=native_vr_pad_premap::Build(l,r,c);assert(!p.dpad_down&&!p.dpad_up);
 ++hostTickCount64;l.x_down=true;native_vr_pad_premap::Build(l,r,c);
 hostTickCount64+=40;l.x_down=false;p=native_vr_pad_premap::Build(l,r,c);assert(p.x&&!p.dpad_down&&!p.dpad_up); // R58: a short X is Quick Speak (companion pak)
 // R58: X held 0.5 s = torch; 1.5 s = visor; the legacy mapping (short = torch) stays behind VrXQuickTalk=false.
 ++hostTickCount64;l.x_down=true;native_vr_pad_premap::Build(l,r,c);hostTickCount64+=600;p=native_vr_pad_premap::Build(l,r,c);assert(p.torchRequestTick62==hostTickCount64&&!p.dpad_down&&!p.dpad_up);
 hostTickCount64+=10;l.x_down=false;p=native_vr_pad_premap::Build(l,r,c);assert(!p.torchRequestTick62&&!p.dpad_down&&!p.dpad_up&&!p.x);
 hostTickCount64+=200;l.quick_access_touch=true;l.x_down=true;native_vr_pad_premap::Build(l,r,c);hostTickCount64+=1600;p=native_vr_pad_premap::Build(l,r,c);assert(p.dpad_up&&!p.dpad_down);
 hostTickCount64+=200;l.x_down=false;p=native_vr_pad_premap::Build(l,r,c);assert(!p.dpad_down&&!p.dpad_up&&!p.x);
 l.quick_access_touch=false;native_vr_pad_premap::SetXQuickTalk58(false);++hostTickCount64;l.x_down=true;native_vr_pad_premap::Build(l,r,c);
 hostTickCount64+=40;l.x_down=false;p=native_vr_pad_premap::Build(l,r,c);assert(p.torchRequestTick62==hostTickCount64&&!p.dpad_down&&!p.dpad_up&&!p.x);native_vr_pad_premap::SetXQuickTalk58(true);
 // A pending gameplay pulse must not leak when a menu closes.
 c.theater=true;native_vr_pad_premap::Build(l,r,c);c={};p=native_vr_pad_premap::Build(l,r,c);assert(!p.dpad_down&&!p.dpad_up);
 ++hostTickCount64;l.x_down=true;native_vr_pad_premap::Build(l,r,c);
 r.quick_access_touch=true;native_vr_pad_premap::Build(l,r,c);r.quick_access_touch=false;
 hostTickCount64+=1000;p=native_vr_pad_premap::Build(l,r,c);assert(!p.dpad_down&&!p.dpad_up);
 l.x_down=false;p=native_vr_pad_premap::Build(l,r,c);assert(!p.dpad_down&&!p.dpad_up);
 // Actual C62 button producer: no calibration dependency, one edge, no XI duplicate.
 native_vr_pad_premap::Reset();l={};r={};l.active=r.active=true;c={};hostTickCount64=40000;
 l.x_down=true;p=native_vr_pad_premap::Build(l,r,c);assert(p.xUtilityPress62==1&&!p.torchRequestTick62);
 hostTickCount64+=499;p=native_vr_pad_premap::Build(l,r,c);assert(!p.torchRequestTick62);
 ++hostTickCount64;p=native_vr_pad_premap::Build(l,r,c);assert(p.torchRequestTick62==40500&&!p.dpad_down);
 hostTickCount64+=1000;p=native_vr_pad_premap::Build(l,r,c);assert(!p.torchRequestTick62&&!p.dpad_down&&!p.xUtilityPress62);
 l.x_down=false;p=native_vr_pad_premap::Build(l,r,c);assert(!p.torchRequestTick62&&!p.x);
 native_vr_pad_premap::Reset();r.quick_access_touch=true;l.x_down=true;p=native_vr_pad_premap::Build(l,r,c);assert(p.xUtilityPress62==3);
 hostTickCount64+=1000;p=native_vr_pad_premap::Build(l,r,c);assert(!p.torchRequestTick62&&!p.dpad_down);
 r.quick_access_touch=false;p=native_vr_pad_premap::Build(l,r,c);hostTickCount64+=1000;p=native_vr_pad_premap::Build(l,r,c);assert(!p.torchRequestTick62); // Wait cannot become torch while held
 l.x_down=false;native_vr_pad_premap::Build(l,r,c);l.quick_access_touch=true;l.x_down=true;p=native_vr_pad_premap::Build(l,r,c);assert(p.xUtilityPress62==2);
 hostTickCount64+=500;p=native_vr_pad_premap::Build(l,r,c);assert(p.dpad_up&&!p.torchRequestTick62&&!p.dpad_down);
 native_vr_pad_premap::Reset();l.quick_access_touch=false;l.active=false;p=native_vr_pad_premap::Build(l,r,c);assert(p.xUtilityPress62==6);
 hostTickCount64+=500;p=native_vr_pad_premap::Build(l,r,c);assert(!p.torchRequestTick62);l.active=true;
 native_vr_pad_premap::Reset();c.semantic_minigame=true;p=native_vr_pad_premap::Build(l,r,c);assert(p.xUtilityPress62==4&&p.x&&!p.torchRequestTick62);
 // Focus loss before the 500ms threshold cannot revive a held button on return.
 native_vr_pad_premap::Reset();c={};l={};r={};l.active=r.active=true;hostTickCount64=47000;
 l.x_down=true;native_vr_pad_premap::Build(l,r,c);hostTickCount64+=200;l.active=false;native_vr_pad_premap::Build(l,r,c);
 hostTickCount64+=300;l.active=true;p=native_vr_pad_premap::Build(l,r,c);assert(!p.torchRequestTick62);
 hostTickCount64+=800;p=native_vr_pad_premap::Build(l,r,c);assert(!p.torchRequestTick62&&!p.dpad_up&&!p.x);
 l.x_down=false;native_vr_pad_premap::Build(l,r,c);l.x_down=true;native_vr_pad_premap::Build(l,r,c);hostTickCount64+=500;p=native_vr_pad_premap::Build(l,r,c);assert(p.torchRequestTick62==hostTickCount64);
 native_vr_pad_premap::Reset();l.active=false;native_vr_pad_premap::Build(l,r,c);hostTickCount64+=200;l.active=true;native_vr_pad_premap::Build(l,r,c);hostTickCount64+=500;p=native_vr_pad_premap::Build(l,r,c);assert(!p.torchRequestTick62);
 // Native minigame/menu values remain available; laser ownership only masks its hand.
 c.semantic_minigame=true;l.x_down=true;l.trigger=2;r.trigger=std::numeric_limits<float>::quiet_NaN();
 p=native_vr_pad_premap::Build(l,r,c);assert(p.x&&p.lt==1&&p.rt==0);
 c.theater=true;c.pointer_hand=1;r.trigger=.8;l.trigger=.6;l.grip=r.grip=.9;
 p=native_vr_pad_premap::Build(l,r,c);assert(p.x&&p.lt==.6f&&p.rt==0&&!p.rb);
 // R59.16C10: B transaction receipt edges and the real stage epoch rearm (one owned hold traverses a stage change once).
 native_vr_pad_premap::Reset();l={};r={};l.active=r.active=true;c={};hostTickCount64=50000;
 native_vr_pad_premap::SetBQuickTalk45(true); // C45: legacy B = Quick Speak (RB) only with VrBQuickTalk=1
 r.b_down=true;p=native_vr_pad_premap::Build(l,r,c);assert(p.backEvent59c10==1&&!p.backOwned59c10&&p.rb&&!p.b); // gameplay-born press = quick-talk RB, not owned
 hostTickCount64+=20;p=native_vr_pad_premap::Build(l,r,c);assert(p.backEvent59c10==0);
 r.b_down=false;p=native_vr_pad_premap::Build(l,r,c);assert(p.backEvent59c10==2&&!p.backOwned59c10);
 c.semantic_minigame=true;c.back_context=true;c.back_epoch=7;hostTickCount64+=20;
 r.b_down=true;p=native_vr_pad_premap::Build(l,r,c);assert(p.backEvent59c10==1&&p.backOwned59c10&&p.b&&!p.rb); // owned press inside a minigame
 hostTickCount64+=100;p=native_vr_pad_premap::Build(l,r,c);assert(p.b&&p.backEvent59c10==0);
 c.back_epoch=8;hostTickCount64+=100;p=native_vr_pad_premap::Build(l,r,c);assert(p.b&&p.backEvent59c10==0); // epoch change: pending until 350 ms of hold
 hostTickCount64+=200;p=native_vr_pad_premap::Build(l,r,c);assert(!p.b&&p.backEvent59c10==3&&p.backOwned59c10); // one 50 ms release pulse, reported once
 hostTickCount64+=10;p=native_vr_pad_premap::Build(l,r,c);assert(!p.b&&p.backEvent59c10==0); // held release interval is not another event
 hostTickCount64+=60;p=native_vr_pad_premap::Build(l,r,c);assert(p.b&&p.backEvent59c10==0); // new press delivered on the same physical hold
 r.b_down=false;p=native_vr_pad_premap::Build(l,r,c);assert(p.backEvent59c10==2&&p.backOwned59c10);
 l.menu_down=true;c.apse_ui=true;p=native_vr_pad_premap::Build(l,r,c);assert(p.backEvent59c10==4&&p.backOwned59c10);l.menu_down=false;
 // H19: real native chat witness, independently of the retired back-map probe.
 native_vr_pad_premap::Reset();c={};l={};r={};l.active=r.active=true;hostTickCount64=60000;
 c.quick_talk_open54=true;r.b_down=true;l.x_down=true;r.a_down=true;l.y_down=true;
 r.thumb_y=1;r.grip=1;r.quick_access_touch=true;
 p=native_vr_pad_premap::Build(l,r,c);assert(p.a&&p.b&&p.x&&p.y&&!p.rb&&!p.r3&&!p.dpad_up&&!p.lt&&!p.rt);
 hostTickCount64+=600;p=native_vr_pad_premap::Build(l,r,c);assert(p.b&&p.x&&!p.torchRequestTick62&&!p.dpad_up);
 c.quick_talk_open54=false;r.quick_access_touch=false;r.thumb_y=0;hostTickCount64+=5;
 p=native_vr_pad_premap::Build(l,r,c);assert(!p.rb&&!p.b&&!p.x&&!p.torchRequestTick62);
 hostTickCount64+=600;p=native_vr_pad_premap::Build(l,r,c);assert(!p.x&&!p.torchRequestTick62);
 l.x_down=false;r.b_down=false;native_vr_pad_premap::Build(l,r,c);
 l.x_down=true;native_vr_pad_premap::Build(l,r,c);hostTickCount64+=40;l.x_down=false;
 p=native_vr_pad_premap::Build(l,r,c);assert(p.x); // deliberate new short X still opens chat
 // A gameplay-born B cannot become a chooser selection while held across opening.
 native_vr_pad_premap::Reset();c={};r.b_down=true;native_vr_pad_premap::Build(l,r,c);
 c.quick_talk_open54=true;p=native_vr_pad_premap::Build(l,r,c);assert(!p.b);
 r.b_down=false;native_vr_pad_premap::Build(l,r,c);r.b_down=true;p=native_vr_pad_premap::Build(l,r,c);assert(p.b);
 std::puts("PASS authored native controls and native-chat A/B/X/Y, no chat-to-torch/visor/gameplay B leak");
}
