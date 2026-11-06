#include <windows.h>
#include "../src/native_vr_pad_premap.cpp"
#ifndef QUICK_TALK_HEADER_H4
#define QUICK_TALK_HEADER_H4 "../src/vrik/gesture_talk59c5.h"
#endif
#include QUICK_TALK_HEADER_H4
#include "../src/vrik/quick_talk_owner_h4.h"
#include <cstdio>
using namespace kcd2vr;namespace gt=gesture_talk59c5;namespace q=quick_talk_owner_h4;
static unsigned total=0,fail=0;static void ck(bool b,const char*s){++total;if(!b){++fail;std::printf("FAIL %s\n",s);}}
template<class T>static unsigned claim(const T&o){if constexpr(requires{o.claimMaskH4;})return o.claimMaskH4;else return 0;}
static gt::Hand point(){gt::Hand h{};h.valid=h.empty=true;h.grip=.8;h.triggerTouch=false;h.forward=.45;h.up=-.2;return h;}
int main(){
 ControllerState l{},r{};l.active=r.active=true;r.quick_access_touch=true;l.thumb_x=-1;native_vr_pad_premap::Context ctx{};
 gt::Config c{};c.holdPoint=true;
 for(unsigned side=0;side<2;++side){gt::State s{};gt::Hand h[2]{};h[side]=point();hostTickCount64=1000;
  auto pending=gt::update(s,1000,h,true,false,false,c);auto pad=native_vr_pad_premap::Build(l,r,ctx);ck(pad.dpad_left,"ordinary physical quick-weapon chord remains available without gesture arbitration");gt::apply54(pad,pending);ck(pad.dpad_left&&claim(pending)==0,"legacy delayed classifier cannot claim a hand before admitting the opener");
  auto opened=gt::update(s,1300,h,true,false,false,c);ck(opened.openChat&&s.pointOwner==int(side),"qualified empty-hand point requests native chooser");gt::apply54(pad,opened);ck(!pad.dpad_left&&!pad.dpad_right&&claim(opened)==(1u<<side),"admitted opener arbitrates same-frame legacy weapon chord");
  h[side].forward=.31;h[side].grip=.5;h[side].triggerTouch=false;auto retained=gt::update(s,1330,h,true,false,true,c);ck(retained.openChat&&s.pointOwner==int(side),"actual native-open chooser survives bounded geometry/Grip jitter while index remains extended");
  auto selected=gt::update(s,1350,h,true,false,true,c,true);ck(selected.openChat&&s.pointOwner==int(side),"choice retains same native chooser owner for input delivery");h[side].forward=.1;h[side].grip=.1;ck(gt::update(s,1370,h,true,false,true,c).openChat,"choice delivery survives immediate hand lowering");auto after=gt::update(s,1390,h,true,false,false,c);ck(!after.openChat,"selection cannot replay the old opener into gameplay");
  gt::State lost{};h[side]=point();gt::update(lost,2000,h,true,false,false,c);gt::update(lost,2300,h,true,false,false,c);h[side].valid=false;ck(!gt::update(lost,2310,h,true,false,true,c).openChat,"real tracking loss cancels chooser immediately, no held-pose timeout");
  gt::State release{};h[side]=point();gt::update(release,3000,h,true,false,false,c);gt::update(release,3300,h,true,false,false,c);h[side].grip=.1;ck(!gt::update(release,3310,h,true,false,true,c).openChat,"deliberate Grip release ends chooser");
  gt::State refused{};h[side]=point();gt::update(refused,4000,h,true,false,false,c);gt::update(refused,4300,h,true,false,false,c);ck(!gt::update(refused,5101,h,true,false,false,c).openChat,"native opener absent/refused is bounded to original800ms request, no indefinite RB");
  gt::State menu{};gt::update(menu,6000,h,true,false,false,c);gt::update(menu,6300,h,true,false,false,c);ck(!gt::update(menu,6310,h,false,false,true,c).openChat,"menu/context ownership revokes opener");
  gt::State occupied{};h[side].empty=false;gt::update(occupied,7000,h,true,false,false,c);ck(!gt::update(occupied,7400,h,true,false,false,c).openChat,"equipped weapon/support hand cannot activate point talk");
  for(bool raise:{false,true}){gt::State combat{};h[side]=point();if(raise){h[side].forward=.1;h[side].up=.3;h[side].triggerTouch=true;}gt::update(combat,8000,h,true,true,false,c);const auto onset=raise?8400:8300;gt::update(combat,onset,h,true,true,false,c);h[side].valid=false;auto cancelled=gt::update(combat,onset+10,h,true,true,false,c);auto raw=native_vr_pad_premap::State{};gt::apply54(raw,cancelled);ck(!cancelled.openChat&&!raw.rb&&!raw.x&&!raw.b,"Taunt/Raise owner loss during opener pulse emits neither RB nor blind option");}
 }
 q::Stamp st{};st.tick=8000;st.body=4;st.request=5;st.player=6;st.mask=1;q::mailbox.publish(st);q::Stamp copy{};q::mailbox.read(copy);ck(q::current(copy,8080,4,5,6)==1&&!q::current(copy,8081,4,5,6),"input claim has original80ms bound");
 native_vr_pad_premap::Reset();ctx.quick_talk_claim_h4=true;auto reserved=native_vr_pad_premap::Build(l,r,ctx);ctx.quick_talk_claim_h4=false;auto closed=native_vr_pad_premap::Build(l,r,ctx);ck(!reserved.dpad_left&&!closed.dpad_left,"held legacy weapon chord stays quarantined across gesture cancellation/menu close");l.thumb_x=0;native_vr_pad_premap::Build(l,r,ctx);l.thumb_x=-1;ck(native_vr_pad_premap::Build(l,r,ctx).dpad_left,"neutral then deliberate fresh legacy weapon chord remains available");
 // The mailbox arbitrates BEFORE equipment enqueue. Actual queued equipment
 // ownership is tested in holster_completion_h36_bridge, not by a second policy.
 ck(!q::current(copy,8001,9,5,6)&&!q::current(copy,8001,4,9,6)&&!q::current(copy,8001,4,5,9),"pre-admission claim cannot cross original body/request/player");
 ck(!q::current(copy,7999,4,5,6),"future pre-admission stamp cannot own current input");
 st.tick=8020;st.mask=0;q::mailbox.publish(st);q::mailbox.read(copy);
 ck(!q::current(copy,8030,4,5,6),"closed input claim has no retained cancellation tombstone");
 // Preserve the later combat utility rule: partial Trigger squeeze is not a raise gesture.
 for(float triggerH4:{.10f,.30f,.60f}){gt::State partial{};gt::Hand handsH4[2]{};
  handsH4[0]=point();handsH4[0].forward=.1;handsH4[0].up=.3;handsH4[0].trigger=triggerH4;handsH4[0].triggerTouch=true;
  gt::update(partial,10000,handsH4,true,true,false,c);const auto outH4=gt::update(partial,10500,handsH4,true,true,false,c);
  ck(!outH4.openChat&&!outH4.option&&partial.fired==0,"partial trigger cannot broaden raise/battlecry utility during combat");}
 std::printf("quick_talk_arbitration_h4=%u/%u nativeExecution=PAD_ONLY\n",total-fail,total);return fail?1:0;
}
