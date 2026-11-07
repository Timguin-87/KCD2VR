// R59.16C10R16 (Claude): two-handed AXIS grip. The off hand latches where it closes on the handle/shaft (no native socket
// needed - the out-of-combat case). C19C3 keeps the chosen point while Grip is held, prioritizes native sword wrist frames,
// and releases only actual Grip/ownership loss. Same rig as support_constraint130_test.
#include "../src/vrik/physical_weapon.h"
#include <cstdio>
using namespace kcd2vr::physical; namespace m=kcd2vr::pose_core;
static unsigned checks=0,failures=0; static void check(bool b,const char*n){++checks;if(!b)++failures;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
static auto clear=[](const Profile&,Pose,Pose,bool){return 1.0;};
static Input input(bool axis){Input i{};i.allowed=true;i.epoch=i.bodyEpoch=1;i.sample=1;i.tick=1000;i.timeNs=1000000000ull;i.tracked[0]=i.tracked[1]=true;
    i.grip[0]=1;i.grip[1]=0;i.hands[0].p={.2,.25,1.3};i.hands[1].p={-.3,.1,1.0};i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};i.reach[0]=i.reach[1]=.8;
    i.dynamics=true;i.controllerAnchor119=true;i.constraintDrive129=true;i.supportConstraint130=true;i.supportMaxAngle=55*3.14159265358979323846/180.;
    i.supportAxis59c16=axis;return i;}
// Longsword with NO captured/synthesized off-hand socket (support=false): the case that never latched outside a fight.
static std::array<Profile,2> sword(int cls=4){std::array<Profile,2> p{};auto&a=p[0];a.key=1;a.generation=1;a.side=0;a.classId=cls;
    a.primary=a.geometry=a.twoHanded=true;a.support=false;a.weight=cls==7?9.6:2;a.extent=cls==7?2.0:1.15;a.tip={0,cls==7?1.8:.9,0};p[1].side=1;return p;}
static void adv(Input&i,unsigned ms=10){++i.sample;i.tick+=ms;i.timeNs+=std::uint64_t(ms)*1000000ull;}
int main(){
 // Historical-axis fixture; the accepted 0.2 bias has its own two_hand_bias_d65 suite.
 kcd2vr::two_hand_d65::enabled=false;

    for(int cls:{4,7}){
        auto p=sword(cls);auto i=input(true);State s{};auto o=step(s,i,p,clear);
        // Close the left hand 15 cm below the right hand on the handle, 3 cm off the axis.
        i.hands[1].p=point(o.weapons[0],Vec3{.03,-.15,0});i.grip[1]=1;
        for(int n=0;n<8;++n){adv(i);o=step(s,i,p,clear);}
        check(o.support[0]&&o.owned[1]&&s.weapon[0].supportAxis59c16,cls==7?"polearm: off hand latches where it closes (no socket)":"longsword: off hand latches where it closes (no socket)");
        // Move the controller down the handle: the chosen grasp must not slide.
        for(int n=0;n<10;++n){i.hands[1].p=point(o.weapons[0],Vec3{0,-.15-.007*(n+1),0});adv(i);o=step(s,i,p,clear);}
        const double along=s.weapon[0].supportGrip169r2.p.y;
        check(o.support[0]&&std::abs(along+.15)<.001,"C19C3 the selected local shaft grasp stays fixed while Grip is held");
        // Both hands own the axis: put the left hand 80 degrees away from where the right wrist points the blade.
        const Vec3 primary=i.hands[0].p;
        i.hands[1].p=primary+m::rotate(m::from_rotation_vector({0,0,1.40}),Vec3{0,-.20,0});
        for(int n=0;n<30;++n){adv(i);o=step(s,i,p,clear);}
        const auto axisW=m::unit(m::rotate(o.weapons[0].q,Vec3{0,1,0}));const auto want=m::unit(primary-i.hands[1].p);
        const double err=axisW&&want?std::acos(std::clamp(m::dot(*axisW,*want),-1.0,1.0))*57.2958:180.0;
        std::printf("axis error %.1f deg (80 deg off the primary wrist)\n",err);
        check(o.support[0]&&err<12.0,"the weapon axis points from the off hand through the primary hand beyond the old 55-degree cap");
        // Pull the left hand 30 cm off the shaft: Grip is still held, no geometric unlatch.
        i.hands[1].p=i.hands[1].p+Vec3{0,0,-.30};adv(i);o=step(s,i,p,clear);adv(i);o=step(s,i,p,clear);
        check(s.weapon[0].support&&o.owned[0],"C19C3 ordinary separation cannot erase a valid held grasp");
        // Open, close again on the shaft: latches again.
        i.grip[1]=0;adv(i);o=step(s,i,p,clear);i.hands[1].p=point(o.weapons[0],Vec3{0,-.12,0});i.grip[1]=1;
        for(int n=0;n<8;++n){adv(i);o=step(s,i,p,clear);}
        check(o.support[0],"reopen + close on the handle latches again");
        adv(i);i.grip[1]=0;o=step(s,i,p,clear);check(!o.support[0]&&o.owned[0],"opening the off-hand grip releases it");
    }
    {auto p=sword();auto i=input(false);State s{};auto o=step(s,i,p,clear);
        i.hands[1].p=point(o.weapons[0],Vec3{0,-.15,0});i.grip[1]=1;for(int n=0;n<8;++n){adv(i);o=step(s,i,p,clear);}
        check(!o.support[0],"comparison: with VrTwoHandAxisGrip off a socket-less weapon cannot be held two-handed");}
    {auto p=sword();auto i=input(true);State s{};auto o=step(s,i,p,clear);
        i.hands[1].p=point(o.weapons[0],Vec3{.16,-.15,0});i.grip[1]=1;for(int n=0;n<8;++n){adv(i);o=step(s,i,p,clear);}
        check(!o.support[0],"a squeeze 16 cm beside the handle is not a grip");}
    {auto p=sword();p[0].twoHanded=false;auto i=input(true);State s{};auto o=step(s,i,p,clear);
        i.hands[1].p=point(o.weapons[0],Vec3{0,-.15,0});i.grip[1]=1;for(int n=0;n<8;++n){adv(i);o=step(s,i,p,clear);}
        check(!o.support[0],"a one-handed weapon is not held two-handed");}
    { // Actual solver: native sword wrist frame beats axis even with the default option on.
        auto p=sword();p[0].support=true;p[0].grip[1].p={-.024,-.194,.036};
        p[0].grip[1].q=m::from_rotation_vector({.3,.4,.1});
        auto i=input(true);i.intentTracked[0]=i.intentTracked[1]=true;State s{};auto o=step(s,i,p,clear);
        const auto native=p[0].grip[1];i.hands[1]=compose(o.weapons[0],native);i.grip[1]=1;
        for(int n=0;n<8;++n){adv(i);o=step(s,i,p,clear);}
        check(o.support[0]&&!s.weapon[0].supportAxis59c16&&
              m::length(s.weapon[0].supportGrip169r2.p-native.p)<1e-9&&m::angle(s.weapon[0].supportGrip169r2.q,native.q)<1e-8,
              "C19C3 actual qualified native sword wrist position AND rotation outrank generic axis");
        // Separation/twist: controller still supplies steering, not a replacement local wrist writer.
        i.hands[1].p=i.hands[1].p+Vec3{-.28,0,.1};i.hands[1].q=m::from_rotation_vector({0,0,2.9});
        for(int n=0;n<25;++n){adv(i);o=step(s,i,p,clear);}
        check(s.weapon[0].support&&m::length(s.weapon[0].supportGrip169r2.p-native.p)<1e-9&&
              m::angle(s.weapon[0].supportGrip169r2.q,native.q)<1e-8,"C19C3 held ordinary motion cannot overwrite or unlatch the selected native wrist");
        i.reach[1]=.08;adv(i);auto oneHand=s;oneHand.weapon[0].support=false;
        oneHand.weapon[0].supportGripReady169r2=false;oneHand.weapon[0].supportRearm=true;
        // This reference is already one-handed, not a synthetic support-release
        // edge carrying the old constrained basis into the D67 handover.
        oneHand.weapon[0].supportCorrection={};
        const auto oneHandOut=step(oneHand,i,p,clear);o=step(s,i,p,clear);
        check(m::length(o.weapons[0].p-oneHandOut.weapons[0].p)<1e-9&&m::angle(o.weapons[0].q,oneHandOut.weapons[0].q)<1e-8,
              "C19C3 dormant unreachable grasp grants no extra two-hand force or steering");
        check(s.weapon[0].support&&!o.owned[1]&&!o.support[0]&&m::length(o.hands[1].p-i.hands[1].p)<1e-9,
              "C19C3 unreachable support retains identity only and current controller hand, no stretch/hide");
        i.reach[1]=.8;i.hands[1]=compose(o.weapons[0],native);adv(i);o=step(s,i,p,clear);
        check(o.support[0]&&m::length(s.weapon[0].supportGrip169r2.p-native.p)<1e-9,"C19C3 anatomical reentry resumes same grasp without open/regrab");
        // A newly learned support does not move an already chosen local grasp.
        p[0].grip[1].p.y-=.02;adv(i);o=step(s,i,p,clear);
        check(s.weapon[0].support&&m::length(s.weapon[0].supportGrip169r2.p-native.p)<1e-9,"C19C3 new support profile waits until next grip instead of shifting held hand");
        // A different primary attachment is a different binding and must not inherit this grasp.
        p[0].grip[0].p.x+=.02;adv(i);o=step(s,i,p,clear);
        check(!s.weapon[0].support&&s.weapon[0].supportRearm,"C19C3 changed primary socket invalidates support binding");
        i.grip[1]=0;adv(i);o=step(s,i,p,clear);i.hands[1]=compose(o.weapons[0],p[0].grip[1]);i.grip[1]=1;
        for(int n=0;n<8;++n){adv(i);o=step(s,i,p,clear);}check(o.support[0],"C19C3 neutral regrip adopts current qualified socket");
        i.tracked[1]=false;adv(i);o=step(s,i,p,clear);
        check(!s.weapon[0].support&&!o.owned[1],"C19C3 true support tracking loss releases immediately");
        i.tracked[1]=true;for(int n=0;n<20;++n){adv(i);o=step(s,i,p,clear);}
        check(!o.support[0],"C19C3 tracking recovery while held does not silently reacquire");
        i.grip[1]=0;adv(i);o=step(s,i,p,clear);i.hands[1]=compose(o.weapons[0],p[0].grip[1]);i.grip[1]=1;
        for(int n=0;n<8;++n){adv(i);o=step(s,i,p,clear);}check(o.support[0],"C19C3 tracking loss requires a fresh deliberate grip");
        adv(i,81);o=step(s,i,p,clear);check(!s.weapon[0].support,"C19C3 no grasp retention through samples older than 80ms");
        for(int n=0;n<20;++n){adv(i);o=step(s,i,p,clear);}check(!o.support[0],"C19C3 stale-sample recovery requires neutral Grip");
        i.allowed=false;adv(i);o=step(s,i,p,clear);check(!o.owned[0]&&!o.owned[1]&&!s.weapon[0].support,"C19C3 menu/protected context clears both weapon and support ownership");
    }
    { // Actual C39 centre-line branch (the old test's absent bounds never exercised sliding).
        auto p=sword(7);p[0].diagnosticBounds59c6=true;p[0].diagnosticBoundsMin59c6={-.03,-.70,-.03};p[0].diagnosticBoundsMax59c6={.03,1.80,.03};
        auto i=input(true);State s{};auto o=step(s,i,p,clear);
        i.hands[1].p=point(o.weapons[0],Vec3{.06,-.25,0});i.grip[1]=1;
        for(int n=0;n<8;++n){adv(i);o=step(s,i,p,clear);}
        check(s.weapon[0].support&&shaft_line39(p[0]).valid,"measured polearm centreline acquired");
        const auto fixed=s.weapon[0].supportGrip169r2;
        for(int n=0;n<10;++n){i.hands[1].p=i.hands[1].p+Vec3{0,-.012,0};adv(i);o=step(s,i,p,clear);}
        check(s.weapon[0].support&&m::length(s.weapon[0].supportGrip169r2.p-fixed.p)<1e-9,
              "C42 measured polearm grasp station stays fixed while Grip held");
    }
    std::printf("%u checks, %u failures\n",checks,failures);return failures?1:0;}
