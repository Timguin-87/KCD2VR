// C44 IMPALE HANG WEIGHT, weapon side (physical_weapon.h): while a dead body hangs on the blade (hang pin, VrImpaleHang) the
// weapon TARGET sags by VrImpaleHangMassKg - the grip ends up below the hand and the tip droops - ramped in over ~300 ms and
// back out after the release. Nothing native: the sim's own target only. The STUCK pin (mode 1) never sags (no living load).
#include "vrik/weapon_sweep112.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr::physical;
static unsigned checks=0;
static void ck(bool ok,const char* label){++checks;if(!ok){std::printf("FAIL %s\n",label);std::exit(1);}std::printf("PASS %s\n",label);}
struct Replay {
    Input in{};State state{};std::array<Profile,2> profiles{};Output out{};double time=1;
    Replay(){in.epoch=in.bodyEpoch=in.sample=1;in.tick=1000;in.timeNs=1000000000;
        in.allowed=in.tracked[0]=in.tracked[1]=in.intentTracked[0]=true;in.grip[0]=1;
        in.dynamics=true;in.contact=true;in.naturalCombat117=in.heldMotionAttacks=in.requirePrimaryGrip114=true;
        in.coherentMelee119=in.controllerAnchor119=in.constraintDrive129=in.physicalCombat177=true;
        in.shoulders[0]={0,0,1.3};in.shoulders[1]={-.2,0,1.3};in.reach[0]=1.3;
        in.hands[0].p={-.25,.2,1.2};in.intentHands[0]=in.hands[0];
        in.impaleEnabled178r15=in.impaleHang32=true;in.haveWorld=true;in.worldScale=1;in.world=Pose{};
        auto& p=profiles[0];p.key=p.generation=1;p.classId=4;p.primary=p.geometry=true;p.axis={0,1,0};p.tip={0,.6,0};p.weight=1.5f;
        sample();rest(1.0);
    }
    void sample(){out=step(state,in,profiles,[&](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=true;c.fraction=1;return c;});}
    void move(Vec3 delta,double dt=.011){time+=dt;in.timeNs=std::uint64_t(std::llround(time*1e9));in.tick=in.timeNs/1000000;++in.sample;
        in.hands[0].p=in.hands[0].p+delta;in.intentHands[0].p=in.intentHands[0].p+delta;sample();}
    void rest(double seconds){for(int n=0;n<int(std::ceil(seconds/.011));++n)move({});}
    Vec3 tip() const {return point(out.weapons[0],profiles[0].tip);}
    Vec3 grip() const {return point(out.weapons[0],profiles[0].grip[0].p);}
    kcd2vr::contact58c1::Receipt receipt(std::uint64_t serial) const {
        kcd2vr::contact58c1::Receipt r{};r.serial=serial;r.attempt=77;r.target=4711;r.targetIdentity=0x1000;r.collider=0x2000;r.rawPart=457;r.continuation=true;
        r.seen=r.accepted=r.applied=r.fields=r.bodyContact=true;r.kind=1;r.feature=int(kcd2vr::bodycombo59c7g::Feature::Tip);r.weaponClass=4;r.blocked=r.perfect=r.parry=0;r.death=1;r.damage=20;r.energy=.9f;
        r.key=1;r.generation=1;r.epoch=in.bodyEpoch;r.request=in.epoch;r.tick=in.tick;r.impactTick=in.tick;const auto t=tip();r.point[0]=float(t.x);r.point[1]=float(t.y);r.point[2]=float(t.z);return r;}
};
int main(){
    Replay r;
    const double restGripDz=r.grip().z-r.in.hands[0].p.z,restTipDz=r.tip().z-r.grip().z;
    std::printf("rest: grip-hand dz %.4f tip-grip dz %.4f\n",restGripDz,restTipDz);
    ck(std::fabs(restGripDz)<.02,"at rest the sim grip sits at the hand");
    kcd2vr::gore32::hangMassKg.store(40.f);
    r.in.receipt58c1=r.receipt(12);r.move({});
    ck(r.out.impaleLatchRequest&&r.out.impaleSerial==12,"killing thrust (energy 0.9): latch requested");
    const Vec3 wound=r.tip();
    r.in.impalePinValid178r15=true;r.in.impalePinSerial178r19=12;r.in.impalePinWorld178r15=wound;r.in.impalePinMode40=0;
    r.move({});ck(r.out.impaleActive&&!r.out.displayLatched[0],"hang pin: active, display not pinned (the corpse follows the blade on the game thread)");
    r.rest(1.5);
    const double hangGripDz=r.grip().z-r.in.hands[0].p.z,hangTipDz=r.tip().z-r.grip().z;
    std::printf("hang 40 kg: grip-hand dz %.4f tip-grip dz %.4f\n",hangGripDz,hangTipDz);
    ck(hangGripDz<restGripDz-.04&&hangGripDz>restGripDz-.09,"40 kg: the grip sags about 6 cm below the hand");
    ck(hangTipDz<restTipDz-.05,"40 kg: the tip droops (hold him up by tilting the hand up)");
    ck(std::fabs(r.in.hands[0].p.z-1.2)<1e-9,"the hand itself (controller) is never moved");
    kcd2vr::gore32::hangMassKg.store(80.f);r.rest(1.5);
    const double heavyGripDz=r.grip().z-r.in.hands[0].p.z;
    std::printf("hang 80 kg: grip-hand dz %.4f\n",heavyGripDz);
    ck(heavyGripDz<hangGripDz-.03,"80 kg sags more than 40 kg");
    // release: pin gone -> the game thread released him; the sag ramps back out
    r.in.impalePinValid178r15=false;r.move({});ck(r.out.impaleReleased,"pin missing: released");
    r.rest(1.5);
    const double afterDz=r.grip().z-r.in.hands[0].p.z;
    std::printf("after release: grip-hand dz %.4f\n",afterDz);
    ck(std::fabs(afterDz-restGripDz)<.02,"after the release the grip is back at the hand");
    // STUCK pin (mode 1, C40 display only) never loads or sags the weapon
    {Replay q;kcd2vr::gore32::hangMassKg.store(80.f);q.in.impale40=true;q.in.receipt58c1=q.receipt(13);q.move({});
     q.in.impalePinValid178r15=true;q.in.impalePinSerial178r19=13;q.in.impalePinWorld178r15=q.tip();q.in.impalePinMode40=1;q.in.impalePinDepth40=.2;q.move({});
     ck(q.out.impaleActive,"STUCK pin active");q.rest(1.5);
     ck(std::fabs(q.grip().z-q.in.hands[0].p.z-restGripDz)<.02,"STUCK (mode 1): no sag on the weapon");}
    std::printf("impale_hang44 %u checks, 0 failures\n",checks);return 0;
}
