// C40 IMPALE BY ARMOUR, weapon side (physical_weapon.h): a NON-lethal native thrust receipt latches only with
// impale40 on; a STUCK pin keeps the DISPLAY blade on the wound line and caps how deep it is shown; no living-victim
// load on the weapon; a missing pin releases with the weapon-side code.
#include "vrik/weapon_sweep112.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr::physical;
static unsigned checks=0;
static void ck(bool ok,const char* label){++checks;if(!ok){std::printf("FAIL %s\n",label);std::exit(1);}std::printf("PASS %s\n",label);}
struct Replay {
    Input in{};State state{};std::array<Profile,2> profiles{};Output out{};double time=1;bool blocked=false;
    Replay(){in.epoch=in.bodyEpoch=in.sample=1;in.tick=1000;in.timeNs=1000000000;
        in.allowed=in.tracked[0]=in.tracked[1]=in.intentTracked[0]=true;in.grip[0]=1;
        in.dynamics=false;in.contact=true;in.naturalCombat117=in.heldMotionAttacks=in.requirePrimaryGrip114=true;
        in.coherentMelee119=in.controllerAnchor119=true;
        in.shoulders[0]={0,0,1.3};in.shoulders[1]={-.2,0,1.3};in.reach[0]=1.3;
        in.hands[0].p={-.25,.2,1.2};in.intentHands[0]=in.hands[0];
        in.impaleEnabled178r15=true;in.haveWorld=true;in.worldScale=1;in.world=Pose{};
        auto& p=profiles[0];p.key=p.generation=1;p.classId=4;p.primary=p.geometry=true;p.axis={0,1,0};p.tip={0,.6,0};
        sample();rest(.12);
    }
    void sample(){out=step(state,in,profiles,[&](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=true;c.fraction=blocked?.5:1;
        if(blocked){c.plane=true;c.normal={0,-1,0};c.point={0,1,1.2};c.native172.entityId=4711;c.native172.body=kcd2vr::npc_contact172::Body::Articulated;}return c;});}
    void move(Vec3 delta,double dt=.01){time+=dt;in.timeNs=std::uint64_t(std::llround(time*1e9));in.tick=in.timeNs/1000000;++in.sample;
        in.hands[0].p=in.hands[0].p+delta;in.intentHands[0].p=in.intentHands[0].p+delta;sample();}
    void rest(double seconds){for(int n=0;n<int(std::ceil(seconds/.01));++n)move({});}
    Vec3 tip() const {return point(out.weapons[0],profiles[0].tip);}
    Vec3 shownTip() const {return point(out.displayWeapons[0],profiles[0].tip);}
    kcd2vr::contact58c1::Receipt receipt(std::uint64_t serial,int death) const {
        kcd2vr::contact58c1::Receipt r{};r.serial=serial;r.attempt=77;r.target=4711;r.targetIdentity=0x1000;r.collider=0x2000;r.rawPart=457;r.continuation=true;
        r.seen=r.accepted=r.applied=r.fields=r.bodyContact=true;r.kind=1;r.feature=int(kcd2vr::bodycombo59c7g::Feature::Tip);r.weaponClass=4;r.blocked=r.perfect=r.parry=0;r.death=death;r.damage=20;
        r.key=1;r.generation=1;r.epoch=in.bodyEpoch;r.request=in.epoch;r.tick=in.tick;r.impactTick=in.tick;const auto t=tip();r.point[0]=float(t.x);r.point[1]=float(t.y);r.point[2]=float(t.z);return r;}
};
int main(){
    // the old rule: a non-lethal thrust never latches
    {Replay r;r.in.receipt58c1=r.receipt(11,0);r.move({});ck(!r.out.impaleLatchRequest,"impale40 off: non-lethal thrust receipt does not latch (C32 rule)");}
    // C40 explicitly selected: its existing native-result contract can now
    // request the separately validated evaluated-joint display attachment.
    {Replay r;r.in.impale40=true;r.in.receipt58c1=r.receipt(11,0);r.move({});ck(r.out.impaleLatchRequest,"C40 on: eligible non-lethal thrust requests its display anchor");}
    // C44 VrImpaleKillEnergy: a killing thrust below the energy threshold stays a plain native kill
    {Replay r;auto x=r.receipt(14,1);x.energy=.2f;r.in.receipt58c1=x;r.move({});ck(!r.out.impaleLatchRequest,"C44 killing thrust at energy 0.2 < 0.3: no latch");
     x=r.receipt(15,1);x.energy=.3f;r.in.receipt58c1=x;r.move({});ck(r.out.impaleLatchRequest&&r.out.impaleSerial==15,"C44 killing thrust at energy 0.3: latch");}
    Replay r;r.in.impale40=true;
    r.in.receipt58c1=r.receipt(12,1);r.move({});
    ck(r.out.impaleLatchRequest&&r.out.impaleSerial==12,"impale40 on: a lethal undefended thrust requests a latch (depth from energy x armour on the game thread)");
    {auto x=r.receipt(13,0);x.blocked=1;Replay b;b.in.impale40=true;b.in.receipt58c1=x;b.move({});ck(!b.out.impaleLatchRequest,"a native block never latches");}
    const Vec3 wound=r.tip();
    // first pin from the game thread: STUCK, 20 cm cap
    r.in.impalePinValid178r15=true;r.in.impalePinSerial178r19=12;r.in.impalePinWorld178r15=wound;r.in.impalePinMode40=1;r.in.impalePinDepth40=.20;
    r.move({});ck(r.out.impaleActive&&r.out.displayLatched[0],"STUCK pin: active, display latched");
    // push the hand 35 cm further through him (our blade skips him): shown penetration is capped at 20 cm
    for(int n=0;n<35;++n)r.move({0,.01,0});
    const Vec3 axis{0,1,0};
    const double shownPen=m::dot(r.shownTip()-wound,axis),physPen=m::dot(r.tip()-wound,axis);
    std::printf("physical pen %.3f shown pen %.3f\n",physPen,shownPen);
    ck(r.out.impaleActive&&physPen>.22&&shownPen<=.2001&&shownPen>.19,"deeper than the cap: the display stops at 20 cm, physics untouched");
    // he steps 6 cm sideways: the shown blade stays on the wound line
    r.in.impalePinWorld178r15=wound+Vec3{.06,0,0};r.move({});
    {const Vec3 d=r.shownTip()-r.in.impalePinWorld178r15;const double lat=m::length(d-axis*m::dot(d,axis));
     ck(r.out.impaleActive&&lat<1e-6,"lateral: the displayed blade passes through the moved wound");}
    // no extra weight on the weapon from a living victim: the hang load needs pin mode != 1 (checked by source); pin lost:
    r.in.impalePinValid178r15=false;r.move({});
    ck(r.out.impaleReleased&&r.out.impaleRelease40==7&&!r.out.displayLatched[0],"pin missing: released (weapon side code 7), display == physical");
    std::printf("impale40_weapon %u checks, 0 failures\n",checks);return 0;
}
