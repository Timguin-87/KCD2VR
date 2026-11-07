#include "ui_hud_experiment36c5.h"
#include "subsystems/hud_ui_controls/prompt_policy_d80.h"
#include <cstdio>
#include <limits>

using namespace kcd2vr;
static int checks=0,failures=0;
static void check(bool ok,const char* why){++checks;if(!ok){++failures;std::printf("FAIL %s\n",why);}}
static ui_contextual_hud162r1::Input front(){
    ui_contextual_hud162r1::Input in{};in.enabled=in.gameplay=in.headValid=true;in.now=100;
    const float s=std::sqrt(.5f);
    in.palm[0]={{-.10f,0,-.45f},{0,-s,0,s},true};
    in.palm[1]={{.10f,0,-.45f},{0,s,0,s},true};return in;
}
static bool open(float g,float t,bool b,bool gv,bool tv,bool touch,bool touchValid,bool held){
#ifdef LEGACY_OPEN
    (void)gv;(void)tv;(void)touch;(void)touchValid;(void)held;return ui_palm171::Open(g,t,b,false);
#else
    return ui_palm171::OpenCurrent(g,t,b,gv,tv,touch,touchValid,held);
#endif
}
int main(){
    using ui_contextual_hud162r1::Looking;
    auto in=front();
    check(Looking(in,0,false),"left palm faces viewer");check(Looking(in,1,false),"right palm faces viewer");
    for(unsigned h=0;h<2;++h){auto back=in;back.palm[h].orientation.y*=-1;
        check(!Looking(back,h,false),"backhand cannot start reveal");check(!Looking(back,h,true),"backhand cannot retain reveal");
        auto edge=in;edge.palm[h].orientation={0,0,0,1};check(!Looking(edge,h,false),"edge-on hand");
        auto far=in;far.palm[h].position.z=-1.2f;check(!Looking(far,h,false),"too far");
        auto side=in;side.palm[h].position.x=h?.8f:-.8f;check(!Looking(side,h,false),"outside head gaze");
    }
    check(open(0,0,false,true,true,false,true,false),"known open index");
    check(!open(0,0,false,true,true,true,true,false),"capacitive index resting on trigger");
    check(open(0,0,false,true,true,true,false,false),"controller without capacitive binding");
    check(!open(0,0,false,false,true,false,true,false),"missing current grip action");
    check(!open(0,0,false,true,false,false,true,false),"missing current trigger action");
    check(!open(0,0,false,true,true,false,true,true),"known held physical owner");
    check(!open(.8f,0,false,true,true,false,true,false),"gripped hand");
    check(!open(0,.8f,false,true,true,false,true,false),"index flexed");
    check(!open(0,0,true,true,true,false,true,false),"button pressed");
    check(!open(std::numeric_limits<float>::quiet_NaN(),0,false,true,true,false,true,false),"invalid analog input");
    ui_hud_experiment36c5::PalmReveal reveal;
    const std::array<bool,2> both{true,true};
    auto v=reveal.Update(in,both);check(!v.left&&!v.right,"one-second dwell retained");
    in.now=1099;v=reveal.Update(in,both);check(!v.left&&!v.right,"not before dwell");
    in.now=1100;v=reveal.Update(in,both);check(v.left&&v.right,"after dwell");
    in.now=4099;v=reveal.Update(in,both);check(v.left&&v.right,"visible just before 3s cap");
    in.now=4100;v=reveal.Update(in,both);check(!v.left&&!v.right,"three-second cap");
    in.now=7000;v=reveal.Update(in,both);check(!v.left&&!v.right,"continuous glance never refreshes timer");
    in.now=7001;v=reveal.Update(in,{false,false});check(!v.left&&!v.right,"closed after expiry");
    in.now=7002;v=reveal.Update(in,both);check(!v.left&&!v.right,"new glance rearms dwell");
    in.now=8002;v=reveal.Update(in,both);check(v.left&&v.right,"new deliberate glance can reveal");
    in.now=8003;v=reveal.Update(in,{false,true});check(!v.left&&v.right,"closing hides immediately and independently");
    in.now=8004;in.palm[1].orientation.y*=-1;v=reveal.Update(in,both);check(!v.right,"turning backhand hides immediately");
    in=front();in.now=9000;reveal={};(void)reveal.Update(in,both);in.now=10000;(void)reveal.Update(in,both);
    in.now=10001;in.headOrientation={0,1,0,0};v=reveal.Update(in,both);check(!v.left&&!v.right,"look away hides immediately");
    in=front();in.now=11000;reveal={};(void)reveal.Update(in,both);in.now=12000;(void)reveal.Update(in,both);
    in.now=12001;in.palm[0].valid=false;v=reveal.Update(in,both);check(!v.left&&v.right,"per-hand tracking loss");
    in.now=11999;v=reveal.Update(in,both);check(!v.left&&!v.right,"clock rollback clears reveal");
    in=front();reveal={};(void)reveal.Update(in,both);in.now=1100;(void)reveal.Update(in,both);
    in.now=1101;in.enabled=false;v=reveal.Update(in,both);check(!v.active&&!v.left&&!v.right,"off clears state");
    check(ui_contextual_hud162r1::CompassGroupD84==ui_contextual_hud162r1::Left,"left compass preserved");
    check(ui_contextual_hud162r1::VitalsGroupD84==ui_contextual_hud162r1::Right,"right health preserved");
#ifndef LEGACY_OPEN
    check(std::fabs(prompt_policy_d80::minigameScale.load()-.90f)<.0001f,"crafting icons twice tested .45 scale");
#endif
    std::printf("palm_hud_d85r5r1: %d/%d passed\n",checks-failures,checks);return failures?1:0;
}
