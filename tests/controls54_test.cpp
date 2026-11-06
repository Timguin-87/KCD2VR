#include "vrik/roomscale58.h"
#include "vrik/gesture_talk59c5.h"
#include "combat_menu49.h"
#include "immersion_menu45.h"
#include "vr_menu.h"
#include <cassert>
#include <cstdio>
int main(){
 namespace rs=kcd2vr::roomscale58;rs::Config c{};assert(c.dodgeCooldownMs==2000);
 rs::State s{};
 auto sample=[&](unsigned long long t,double x,double y){rs::observeHmd(s,t,x,y);return rs::drive(s,t,x,y,false,true,true,c);};
 sample(1000,0,0);bool first=false;for(int i=1;i<=10;++i)first|=sample(1000+i*20,.012*i,0).dodge;
 assert(first&&s.dodges==1);
 // Return motion is ignored, even after cooldown; regain neutral to re-arm.
 for(int i=1;i<=120;++i)sample(1200+i*20,.12,0);
 for(int i=1;i<=10;++i)assert(!sample(3600+i*20,.12-.012*i,0).dodge);
 for(int i=1;i<=20;++i)assert(!sample(3800+i*20,0,0).dodge);
 bool second=false;for(int i=1;i<=10;++i)second|=sample(4200+i*20,-.012*i,0).dodge;
 assert(second&&s.dodges==2);
 rs::State slow{};for(int i=0;i<=80;++i){rs::observeHmd(slow,1000+i*20,i*.001,0);assert(!rs::drive(slow,1000+i*20,i*.001,0,false,true,true,c).dodge);}
 rs::State back{};bool backward=false;for(int i=0;i<=50;++i){rs::observeHmd(back,2000+i*20,0,-i*.0021);backward|=rs::drive(back,2000+i*20,0,-i*.0021,false,true,true,c).dodge;}assert(backward);
 namespace gt=kcd2vr::gesture_talk59c5;gt::State g{};gt::Config cfg{};cfg.holdPoint=true;cfg.manualPoint54=true;
 gt::Hand h[2]{};h[0].valid=h[0].empty=true;h[0].grip=.9;h[0].triggerTouch=false;h[0].forward=.45;h[0].up=-.1;
 for(int i=0;i<=40;++i)gt::update(g,1000+i*10,h,true,true,false,cfg);
 auto o=gt::update(g,5000,h,true,true,true,cfg);assert(o.openChat&&!o.option&&g.pointOwner==0&&g.pendingOption==0);
 struct Pad{bool rb=false,b=false,x=false;};Pad p{};gt::apply54(p,o);assert(p.rb&&!p.x&&!p.b);
 o=gt::update(g,5010,h,true,true,true,cfg,true);assert(o.openChat);
 assert(!gt::update(g,5020,h,true,true,false,cfg).openChat); // same pose cannot reopen after selection
 g={};for(int i=0;i<=40;++i)gt::update(g,6000+i*10,h,true,false,false,cfg);
 assert(!gt::update(g,6410,h,false,false,true,cfg).openChat); // pause/dialogue revokes immediately
 // Production policy: an empty off-hand point in combat taunts; it never
 // installs the persistent Quick Talk chooser. A held weapon never qualifies.
 cfg.manualPoint54=false;g={};
 for(int i=0;i<=31;++i)gt::update(g,7000+i*10,h,true,true,false,cfg);
 assert(g.pointOwner==-1&&g.pendingOption==3&&g.lastKind==gt::Kind::Taunt);
 g={};h[0].empty=false;
 for(int i=0;i<=40;++i)assert(!gt::update(g,8000+i*10,h,true,true,false,cfg).openChat);
 h[0].empty=true;g={};
 for(int i=0;i<=40;++i)gt::update(g,9000+i*10,h,true,false,false,cfg);
 assert(gt::update(g,10000,h,true,false,true,cfg).openChat&&g.pointOwner==0);
 g={};assert(!gt::battleCryContact54(g,11000,true,true,false,cfg));
 assert(!gt::battleCryContact54(g,11000,false,true,true,cfg));
 assert(gt::battleCryContact54(g,11000,true,true,true,cfg));
 auto cry=gt::update(g,11010,h,true,true,true,cfg);assert(cry.openChat&&cry.option==2&&g.pointOwner==-1);
 assert(!gt::battleCryContact54(g,11020,true,true,true,cfg)); // held contact cannot spam native voice input
 kcd2vr::VrMenuState settings{};settings.exp_weight_feel=1.7f;settings.exp_guard_auto_windup=true;settings.stat_sim26=3;settings.roomscale_mode=3;settings.vr_ui_center_offset_y=-153;
 const kcd2vr::VrMenuState defaults{};kcd2vr::combat_menu49::defaults54(settings);
 assert(settings.exp_weight_feel==defaults.exp_weight_feel&&!settings.exp_guard_auto_windup&&settings.stat_sim26==0);
 assert(settings.roomscale_mode==3&&settings.vr_ui_center_offset_y==-153);
 assert(kcd2vr::combat_menu49::clean()&&kcd2vr::immersion_menu45::clean());
 for(int mode=0;mode<4;++mode){const auto d=kcd2vr::combat_menu49::dodgeMode54(mode);assert(d>=0&&d<=3);assert(kcd2vr::combat_menu49::dodgeMode54(d)==mode);}
 std::puts("controls54: cooldown, return suppression, slow/forward filtering, held chooser, revocation and combat-only defaults PASS");
}

