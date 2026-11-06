#include "vrik/physical_weapon.h"
#include "vrik/equipment_gesture114.h"
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr::physical;
static unsigned checks=0;
static void ck(bool ok,const char* text){++checks;if(!ok){std::fprintf(stderr,"FAIL %s\n",text);std::exit(1);}}
static bool has(const Output& o,CommandKind k){for(unsigned n=0;n<o.commandCount;++n)if(o.commands[n].kind==k)return true;return false;}
int main(){
    Input i{};i.epoch=i.bodyEpoch=i.sample=1;i.tick=1000;
    i.allowed=i.tracked[0]=i.tracked[1]=i.intentTracked[0]=i.intentTracked[1]=true;
    i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};
    i.hands[1].p={0,.4,1.3};i.hands[0].p={.10,.25,1.3};
    i.requirePrimaryGrip114=i.bowTrigger=i.bowRetention=true;i.grip[1]=1;
    i.contact=i.dynamics=i.attacks=false;i.bowActions=true;
    Profile p{};p.key=p.generation=1;p.side=1;p.classId=9;p.primary=p.support=p.geometry=true;
    p.grip[0].p={0,-.15,0};p.axis={0,-1,0};p.tip={0,0,.6};
    State state{};std::array<Profile,2> profiles{Profile{},p};
    auto sample=[&](){++i.sample;i.tick+=10;return step(state,i,profiles,[](const Profile&,Pose,Pose,bool){return Contact103{};});};
    sample();i.trigger[0]=1;auto o=sample();
    ck(o.bowHeld&&has(o,CommandKind::BowBegin),"trigger acquires nock without shoulder ticket or right grip");
    ck(o.bowFraction==0,"initial lateral acquisition offset cannot draw bow");
    ck(m::length(o.hands[0].p-Vec3{0,.25,1.3})<1.e-8,"drawing wrist projects onto captured native string hand");
    i.hands[0].p={.35,-.05,1.3};
    for(int n=0;n<20;++n)o=sample();
    ck(o.bowHeld&&o.bowFraction>.5,"10 cm acquisition offset does not cause false 35 cm lateral cancel");
    i.trigger[0]=0;o=sample();ck(has(o,CommandKind::BowFire),"trigger release requests fire after physical draw");
    ck(!has(step(state,i,profiles,[](const Profile&,Pose,Pose,bool){return Contact103{};}),CommandKind::BowFire),"duplicate sample cannot fire twice");
    i.hands[0].p={.10,.25,1.3};sample();i.trigger[0]=1;o=sample();
    ck(o.bowHeld&&!o.bow116.needsQuiver,"unconfirmed shot request cannot force another shoulder trip");
    i.grip[1]=0;o=sample();ck(!o.bowHeld&&has(o,CommandKind::BowCancel)&&o.releaseReason121[1]==2,"primary release cancels and reports reason");
    // Face guard remains guard; preparation above the head is available to cuts.
    Profile sword{};sword.key=2;sword.side=0;sword.classId=4;sword.primary=sword.geometry=true;sword.tip={.6,0,0};
    i.grip[0]=1;Pose weapon{};weapon.p={-.3,.3,1.50};
    ck(natural_guard117(i,sword,weapon,false),"horizontal weapon in front of face enters guard");
    weapon.p.z=1.82;ck(!natural_guard117(i,sword,weapon,false)&&!natural_guard117(i,sword,weapon,true),"overhead preparation is outside both guard thresholds");
    // Mount acquisition is forgiving but still needs neutral, dwell and pull-out.
    namespace e=kcd2vr::equipment114;e::State s{};e::Input h{};
    h.key=h.generation=h.epoch=1;h.tick=1000;h.eligible=h.mountKnown=true;h.hand={.18,0,0};
    ck(e::update(s,h)==e::Request::None,"holster neutral starts without a request");
    h.grip=1;h.tick+=10;ck(e::update(s,h)==e::Request::None&&s.latched,"18 cm acquisition accepts retargeted hand");
    h.hand.x=.25;h.tick+=80;ck(e::update(s,h)==e::Request::None,"small motion inside exit region cannot draw");
    h.hand.x=.33;h.tick+=10;ck(e::update(s,h)==e::Request::Draw,"deliberate pull after dwell requests draw");
    h.tick+=10;ck(e::update(s,h)==e::Request::None&&s.pending,"returned request still waits for native attachment");
    h.drawn=true;h.tick+=10;ck(e::update(s,h)==e::Request::None&&!s.pending,"observed native hand membership acknowledges draw");
    std::printf("PASS %u regression121 checks\n",checks);
}
