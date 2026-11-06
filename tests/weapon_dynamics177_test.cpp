#include "vrik/physical_weapon.h"
#include <cstdio>
#include <chrono>
using namespace kcd2vr::physical;
namespace d=kcd2vr::dynamics177;
static int checks=0,failed=0;
static void ck(bool b,const char* name){++checks;failed+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",name);}
struct Replay {
    Input in{};State state{};std::array<Profile,2> profiles{};Output out{};
    bool wall=false,unknown=false;
    Replay(){in.allowed=true;in.epoch=in.bodyEpoch=1;in.tick=1000;in.sample=1;
        in.physicalCombat177=in.controllerAnchor119=in.constraintDrive129=true;
        in.tracked[0]=in.tracked[1]=true;in.grip[0]=1;in.attacks=false;
        in.hands[0].p={.2,.3,1.3};in.shoulders[0]={.2,0,1.3};in.reach[0]=1.5;
        auto& p=profiles[0];p.key=p.generation=1;p.classId=4;p.primary=p.geometry=true;
        p.weight=3.4;p.extent=.9;p.tip={0,.9,0};profiles[1].side=1;next();}
    void next(unsigned ms=10){in.tick+=ms;++in.sample;in.timeNs=in.tick*1000000;
        out=step(state,in,profiles,[&](const Profile&,Pose a,Pose b,bool){
            Contact103 c{};if(unknown){c.known=false;c.fraction=0;return c;}
            if(wall&&b.p.y>.4){c.fraction=std::clamp((.4-a.p.y)/std::max(1e-9,b.p.y-a.p.y),0.,1.);
                c.plane=true;c.point={.2,.4,1.3};c.normal={0,-1,0};c.feature={0,0,0};}
            return c;
        });}
};
// Exercise the actual displayed/checked weapon, not just the PD force caps.
struct HandlingReplay : Replay {
    HandlingReplay(double stamina,bool support,int cls,unsigned ms=10){
        in.weaponAuthorityScale178r44=.6;in.attributes={20,24,stamina,true};
        in.supportConstraint130=true;in.grip[1]=support?1:0;
        in.shoulders[1]={-.2,0,1.3};in.reach[1]=1.5;
        auto& p=profiles[0];p.classId=cls;p.support=p.twoHanded=true;
        p.weight=cls==7?9.6:3.4;p.extent=cls==7?1.8:.9;p.tip={0,p.extent,0};
        p.grip[1].p={-.12,0,0};state={};
        for(unsigned n=0;n<1000/ms;++n)next(ms);
        ck(out.active[0]&&out.support[0]==support,"handling replay has exact primary/support ownership");
    }
    void next(unsigned ms=10){in.hands[1]=compose(in.hands[0],profiles[0].grip[1]);Replay::next(ms);}
};
static void stats59c17_cases(int& total,int& failed){
    namespace d=kcd2vr::dynamics177;auto ck=[&](bool b,const char* s){++total;failed+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",s);};
    auto near=[](double a,double b){return std::fabs(a-b)<1e-6;};
    auto s=d::stats59c17(20,24,10,20,1.,true);
    ck(s.active&&near(s.S,1.3)&&near(s.A,1.1),"R17 Henry 20/24 with a 10/20 weapon: S 1.30 (strong), A 1.10");
    s=d::stats59c17(20,20,30,30,1.,true);ck(near(s.S,1./3.)&&near(s.A,.5),"C26c a weapon 10 above Henry's stats: S 1/3 (3x heavier one-handed), A 0.50 (slow)");
    ck(!d::stats59c17(20,24,10,20,0.,true).active&&!d::stats59c17(20,24,10,20,1.,false).active,"R17 effect 0 or no live stats = R16 handling");
    s=d::stats59c17(20,24,-1,-1,1.,true);ck(s.active&&near(s.S,1.)&&near(s.A,1.),"R17 unknown requirement = neutral");
    { // C26c: two hands bring the under-requirement weapon to ~1.8x; weapon skill scales drive and speed; simulation presets
      const auto low=d::stats59c17(20,20,30,30,1.,true);
      const auto one=d::statAuthority59c17(d::authority(10,10,1,false,false),low),two=d::statAuthority59c17(d::authority(10,10,1,false,true),low);
      const auto ref=d::authority(10,10,1,false,false);
      ck(near(ref.force/one.force,3.)&&std::fabs(ref.force/two.force-1.765)<.01,"C26c 10 below requirement: 3x heavier one-handed, ~1.8x with two hands");
      ck(near(d::skillFactor26(15),1.)&&near(d::skillFactor26(0),.7)&&near(d::skillFactor26(30),1.3)&&near(d::skillFactor26(-1),1.),"C26c skill 0/15/30 = 0.7/1/1.3, unread = neutral");
      ck(d::skillSlot26(4)==0&&d::skillSlot26(5)==1&&d::skillSlot26(7)==2&&d::skillSlot26(0)==3&&d::skillSlot26(12)==4&&d::skillSlot26(8)==-1,"C26c weapon class -> skill (weapon_class.xml)");
      const auto sk=d::rpg26(d::stats59c17(20,20,20,20,1.,true),0);ck(near(sk.S,.7)&&near(sk.A,.7),"C26c skill 0 at requirement: 1.43x heavier and slower");
      ck(!d::rpg26(d::stats59c17(20,20,20,20,0.,true),0).active,"C26c effect 0 keeps R16 handling");
      d::Sim26 sm{};ck(!d::sim26(0,sm)&&d::sim26(1,sm)&&sm.skill==2&&d::sim26(3,sm)&&near(sm.strength,25),"C26c simulation presets");
    }
    s=d::stats59c17(20,24,10,20,1.,true);const auto a=d::statAuthority59c17({100,10},s);
    ck(near(a.force,130)&&near(a.torque,10*1.3*std::sqrt(1.1)),"R17 strength drives force, strength x sqrt(agility) the torque");
    const auto c=d::statCompliance59c17({.10,.20},d::stats59c17(20,20,30,30,1.,true));
    ck(near(c.metres,.10*1.6)&&near(c.radians,.20*1.6),"R17 an over-heavy weapon trails up to x1.6 of the envelope");
    // advance(): agility 1 is the R16 integration exactly
    const auto b=d::body(4,3.4,1.0);d::Vec3 p1{},v1{},w1{},p2{},v2{},w2{};d::Quat q1{},q2{};
    for(int i=0;i<50;++i){d::advance(b,{180,35},{0,1,0},{0,0,-9.81},{.2,0,0},{},{},{},.004,p1,q1,v1,w1);
        d::advance(b,{180,35},{0,1,0},{0,0,-9.81},{.2,0,0},{},{},{},.004,p2,q2,v2,w2,1.);}
    ck(near(p1.x,p2.x)&&near(q1.w,q2.w),"R17 advance() with agility 1 is the R16 drive");
    d::Vec3 p3{},v3{},w3{};d::Quat q3{};for(int i=0;i<20;++i)d::advance(b,{180,35},{0,1,0},{0,0,-9.81},{.2,0,0},{},{},{},.004,p3,q3,v3,w3,1.25);
    d::Vec3 p4{},v4{},w4{};d::Quat q4{};for(int i=0;i<20;++i)d::advance(b,{180,35},{0,1,0},{0,0,-9.81},{.2,0,0},{},{},{},.004,p4,q4,v4,w4,.6);
    ck(p3.x>p4.x,"R17 an agile Henry's weapon follows the hand faster than a slow one");
}
int main(){
 // Historical-axis fixture; the accepted 0.2 bias has its own two_hand_bias_d65 suite.
 kcd2vr::two_hand_d65::enabled=false;

    // D67 explicit D66 linear-compliance reference. Default anchored grip and
    // retained angular resistance are independently tested by held_motion_d67.
    kcd2vr::held_motion_d67::setGripAnchored(false);
    kcd2vr::held_motion_d67::setSupportRelease(false);
    { // H2: reference continuity, geometry sensitivity and excluded item classes.
        const auto ref=d::handlingLoadsH2(4,3.4,.9);
        const auto shortBlade=d::handlingLoadsH2(1,3.4,.5);
        const auto longBlade=d::handlingLoadsH2(4,3.4,1.4);
        const auto axeLoad=d::handlingLoadsH2(3,6.5,.9);
        const auto poleLoad=d::handlingLoadsH2(7,9.6,2.0);
        ck(std::abs(ref.linear-3.4)<1e-12&&std::abs(ref.angular-3.4)<1e-12,
            "H2 reference longsword keeps both original drive loads");
        ck(shortBlade.linear<ref.linear&&shortBlade.angular<ref.angular,
            "H2 short sword follows more readily than reference longsword");
        ck(std::abs(longBlade.linear-ref.linear)<1e-12&&longBlade.angular>ref.angular,
            "H2 longer equal-mass sword resists rotation without fake extra mass");
        ck(axeLoad.angular>ref.angular&&poleLoad.angular>=axeLoad.angular,
            "H2 head-heavy axe and long polearm retain higher rotational load");
        bool excluded=true;
        for(int cls:{0,6,8,9,10,11,12,13,14,15,17}){
            const auto v=d::handlingLoadsH2(cls,2.7,1.2);
            excluded=excluded&&v.linear==2.7&&v.angular==2.7;
        }
        ck(excluded,"H2 bow, torch, shield, fists and ranged tuning unchanged");
        const auto missing=d::handlingLoadsH2(4,3.4,0);
        const auto invalid=d::handlingLoadsH2(7,std::numeric_limits<double>::quiet_NaN(),2.0);
        ck(missing.linear==3.4&&missing.angular==3.4&&std::isfinite(invalid.linear)&&
            std::isfinite(invalid.angular)&&poleLoad.angular<=10.2+1e-12,
            "H2 missing reach preserves legacy; invalid weight and long poles remain bounded");
    }
    {
        // Exercise the production comparison drive, including its pose bounds.
        Replay shortReach,longReach;
        for(auto* r:{&shortReach,&longReach})r->in.physicalCombat177=false;
        shortReach.profiles[0].tip={0,.5,0};shortReach.profiles[0].extent=.5;
        longReach.profiles[0].tip={0,1.8,0};longReach.profiles[0].extent=1.8;
        for(auto* r:{&shortReach,&longReach}){
            r->in.hands[0].q=m::from_rotation_vector({.06,0,0});r->next();
        }
        const auto error=[](const Replay& r){return m::length(m::rotation_vector(
            m::multiply(r.in.hands[0].q,m::conjugate(r.out.weapons[0].q))));};
        ck(shortReach.out.active[0]&&longReach.out.active[0]&&
            error(longReach)>error(shortReach)+1e-6,
            "H2 production comparison drive resists abrupt rotation more with longer reach");
    }
    const auto sword=d::body(4,3.4,.9),longSword=d::body(4,3.4,1.4),axe=d::body(3,6.5,.9);
    ck(sword.mass==1.4&&sword.mass!=3.4,"inventory weight is not mislabeled kilograms");
    ck(d::body(16,3,.7).mass<1.5,"retail hunting sword class16 does not inherit polearm mass");
    ck(longSword.gripInertia>sword.gripInertia&&axe.gripInertia>sword.gripInertia,"length and forward balance increase pitch/yaw inertia");
    const auto one=d::authority(10,10,1,true,false),two=d::authority(10,10,1,true,true);
    const auto heavy=d::scaledAuthority44c1(one,.6),invalid=d::scaledAuthority44c1(one,NAN);
    ck(heavy.force<one.force&&heavy.torque<one.torque,"weight feel reduces both authority caps without changing mass");
    ck(invalid.force==one.force&&invalid.torque==one.torque,"invalid weight feel cannot poison the physical pose with NaN");
    const auto exhausted=d::fatiguedAuthority59c10r2(one,.6,5);
    ck(std::abs(exhausted.force-one.force*.12)<1e-10&&std::abs(exhausted.torque-one.torque*.12)<1e-10,
       "fatigue divides the clamped user authority; .6 / 5 stays .12, not .2");
    ck(d::fatiguedAuthority59c10r2(one,NAN,NAN).force==one.force,
       "nonfinite fatigue/setting use neutral authority");
    ck(two.force>one.force&&two.torque>one.torque,"latched support increases force and torque authority");
    ck(d::authority(10,10,0,true,false).torque<one.torque,"existing live stamina reduces drive authority without writing stamina");
    { // Actual motor, not only the pre-motor Authority struct: gravity cannot erase fatigue.
     const auto pole=d::body(7,9.6,2.0);Vec3 p1{},v1{},w1{},p2{},v2{},w2{};Quat q1{},q2{};
     const auto full=d::fatiguedAuthority59c10r2(one,.6,1),empty=d::fatiguedAuthority59c10r2(one,.6,5);
     const auto target=m::from_rotation_vector({0,0,.6});
     d::advance(pole,full,{0,1,0},{0,0,-9.81},{},target,{},{},.004,p1,q1,v1,w1);
     d::advance(pole,empty,{0,1,0},{0,0,-9.81},{},target,{},{},.004,p2,q2,v2,w2);
     ck(m::length(w1)>0&&m::length(w2)<m::length(w1)*.21,"long-pole motor retains fivefold exhausted steering reduction despite gravity support");
     Vec3 stationary{},velocity{},omega{};Quat held{};
     for(int i=0;i<250;++i)d::advance(pole,empty,{0,1,0},{0,0,-9.81},{},{},{},{},.004,stationary,held,velocity,omega);
     ck(m::length(stationary)<1e-9&&m::angle(held,Quat{})<1e-9,"reserved static support keeps an exhausted pole guard stationary");
    }
    const auto hit=d::impact(sword,{0,1,0},{0,.9,0},{0,0,1},{0,0,-3});
    const auto jab=d::impact(sword,{0,1,0},{0,.9,0},{0,0,1},{0,0,-.3});
    ck(hit.energy>jab.energy*99&&hit.energy<jab.energy*101,"normal kinetic-energy estimate scales with speed squared");
    const auto grip=d::impact(sword,{0,1,0},{0,0,0},{0,0,1},{0,0,-3});
    ck(std::isfinite(grip.energy)&&grip.effectiveMass>0&&grip.effectiveMass<=sword.mass,"contact at grip has no I/r-squared singularity");
    ck(d::impact(sword,{0,1,0},{0,.9,0},{0,0,1},{3,0,0}).energy==0,"tangential scrape cannot become a normal impact");
    ck(d::impact(sword,{0,1,0},{0,.9,0},{0,0,1},{}).impulse==0,"resting blade cannot pump recoil");
    for(unsigned ms:{8u,11u,33u}){
        Replay r;double maxError=0;
        for(int n=0;n<120;++n){r.in.hands[0].p.x+=.04*ms/1000.;r.next(ms);
            maxError=std::max(maxError,m::length(r.out.hands[0].p-r.in.hands[0].p));}
        std::printf("slow %u ms: %.6f m\n",ms,maxError);
        ck(maxError<.008&&r.out.active[0],"slow motion remains within 8mm at 30-125Hz including gravity");
    }
    {Replay r;for(int n=0;n<60;++n)r.next();
     // R59.16C10R14: the arm carries the weapon's weight first (static support reserved), so a held guard no longer sags;
     // the bound is what matters - never more than 8 mm below the hand, never above it.
     ck(r.out.hands[0].p.z<=r.in.hands[0].p.z+1e-9&&r.in.hands[0].p.z-r.out.hands[0].p.z<.008,"held horizontal weapon has bounded gravity compliance");}
    {Replay r;double maximum=0;for(int n=0;n<20;++n){r.in.hands[0].p.x+=.02;r.next();maximum=std::max(maximum,m::length(r.out.hands[0].p-r.in.hands[0].p));}
     ck(maximum<=.055001,"abrupt free-space motion retains existing 55mm separation bound");
     for(int n=0;n<40;++n)r.next();ck(m::length(r.out.hands[0].p-r.in.hands[0].p)<.008,"drive settles after the hand stops");}
    {Replay r;r.wall=true;for(int n=0;n<25;++n){r.in.hands[0].p.y+=.008;r.next();}
     ck(r.out.weapons[0].p.y<=.400001&&r.out.blocked[0],"inertial target cannot push final checked weapon through a plane");
     for(int n=0;n<40;++n)r.next();ck(r.out.impact177[0].energy==0,"continued wall pressure adds no new impact event");}
    {Replay r;const auto before=r.out.weapons[0];r.unknown=true;r.in.hands[0].p.y+=.15;r.next();
     ck(m::length(r.out.weapons[0].p-before.p)<1e-9&&!r.out.contactKnown[0]&&r.out.impactSpeed[0]==0,"UNKNOWN freezes checked motion without invented impact");}
    {Replay r;r.in.physicalCombat177=false;r.profiles[0].classId=9;auto copy=r;r.next();copy.in.physicalCombat177=true;copy.next();
     ck(m::length(r.out.weapons[0].p-copy.out.weapons[0].p)<1e-12,"ranged weapon does not acquire the experimental melee drive");}
    {Replay a,b;a.in.haveWorld=b.in.haveWorld=true;a.in.worldScale=1;b.in.worldScale=.8;
     // Same physical weapon/targets expressed at different native model scales.
     b.in.hands[0].p=b.in.hands[0].p*1.25;b.in.shoulders[0]=b.in.shoulders[0]*1.25;b.in.reach[0]*=1.25;
     b.profiles[0].tip=b.profiles[0].tip*1.25;b.profiles[0].extent*=1.25;
     a.state={};b.state={};for(int n=0;n<150;++n){a.next();b.next();}
     ck(m::length(a.out.hands[0].p-b.out.hands[0].p*.8)<.0001,"gravity and drive use metres across native model scale");}
    for(int cls:{4,7})for(bool support:{false,true})for(unsigned ms:{8u,11u,16u})for(bool cut:{false,true}){
        HandlingReplay rested(1,support,cls,ms),tired(0,support,cls,ms);
        const auto start=rested.in.hands[0];double earlyRest=0,earlyTired=0,maxLag=0;
        double projectedError=0;unsigned projectedFrames=0;
        // R59.16C10R14: the production envelope is the C7B/C10R2 compliance widened by the class handling (halberd one-handed x2.5, two hands x1.6).
        const auto envelope=d::classCompliance59c14(d::compliance59c10r2(5,support),d::handling59c14(cls,support,tired.in.poleOneHand59c14));
        for(unsigned tick=ms;tick<=200;tick+=ms){
            const auto before=tired.out.weapons[0];const double t=double(tick)/200.,s=3*t*t-2*t*t*t;
            if(cut)rested.in.hands[0].q=tired.in.hands[0].q=m::from_rotation_vector({0,0,1.4*s});
            else rested.in.hands[0].p.y=tired.in.hands[0].p.y=start.p.y+.45*s;
            rested.next(ms);tired.next(ms);
            const double lag=cut?m::angle(tired.out.weapons[0].q,tired.in.hands[0].q):m::length(tired.out.hands[0].p-tired.in.hands[0].p);
            maxLag=std::max(maxLag,lag);
            if(tick<=80){earlyRest=cut?m::angle(rested.out.weapons[0].q,start.q):rested.out.weapons[0].p.y-start.p.y;
                earlyTired=cut?m::angle(tired.out.weapons[0].q,start.q):tired.out.weapons[0].p.y-start.p.y;}
            if(std::abs(lag-(cut?envelope.radians:envelope.metres))<1e-6){
                const auto actual=cut?m::rotation_vector(m::multiply(tired.out.weapons[0].q,m::conjugate(before.q)))*(1000./ms):
                    (point(tired.out.weapons[0],tired.profiles[0].grip[0].p)-point(before,tired.profiles[0].grip[0].p))*(1000./ms);
                const auto stored=cut?tired.state.weapon[0].angularVelocity:tired.state.weapon[0].linearVelocity;
                projectedError=std::max(projectedError,m::length(actual-stored));++projectedFrames;
            }
        }
        std::printf("FINAL_HANDLING class=%d support=%d dt=%u motion=%s earlyRatio=%.3f maxLag=%.5f projected=%u velocityError=%.6f\n",
            cls,support,ms,cut?"cut_rad":"thrust_m",earlyTired/earlyRest,maxLag,projectedFrames,projectedError);
        ck(earlyRest>0&&earlyTired<earlyRest*.85,"exhaustion measurably slows final initial acceleration, 60-125Hz");
        ck(maxLag<=(cut?envelope.radians:envelope.metres)+1e-6,"displayed lag stays within finite fatigue envelope");
        ck(projectedFrames>0&&projectedError<1e-5,"projection carries actual displayed velocity into the next step");
        // Reverse, stop, release and reacquire: no continuing ballistic blade or frozen pose.
        bool reversalFinite=true;
        for(unsigned tick=ms;tick<=200;tick+=ms){const double t=double(tick)/200.,s=3*t*t-2*t*t*t;
            tired.in.hands[0].q=cut?m::from_rotation_vector({0,0,1.4*(1-s)}):start.q;
            tired.in.hands[0].p=start.p;if(!cut)tired.in.hands[0].p.y+=.45*(1-s);tired.next(ms);
            reversalFinite&=finite(tired.out.weapons[0])&&tired.out.active[0];}
        ck(reversalFinite,"reversal retains finite owned checked weapon");
        // An exhausted one-hand pole has I=4.3092 and torque=1.68: alpha=.3899.
        // Even ideal rest-to-rest recovery from .7 rad takes 2*sqrt(.7/alpha)=2.68 s.
        // The old fixed 1 s silently required a gravity-floor steering boost.
        // Keep the same position/angle limits, with a physical recovery deadline
        // (50% damping margin + .25 s), and require angular motion to settle too.
        const auto recoveryBody=d::body(cls,tired.profiles[0].weight,tired.profiles[0].extent);
        const auto recoveryAuthority=d::classAuthority59c14(d::fatiguedAuthority59c10r2(
            d::authority(10,10,1,false,support),tired.in.weaponAuthorityScale178r44,5),
            d::handling59c14(cls,support,tired.in.poleOneHand59c14));
        const double alpha=recoveryAuthority.torque/recoveryBody.gripInertia;
        const unsigned settleMs=cut?std::max(1000u,unsigned(1000*(3*std::sqrt(envelope.radians/alpha)+.25))):1000u;
        tired.in.hands[0]=start;bool boundedSettle=true;
        for(unsigned tick=0;tick<settleMs;tick+=ms){tired.next(ms);boundedSettle&=finite(tired.out.weapons[0])&&
            m::angle(tired.out.weapons[0].q,start.q)<=envelope.radians+1e-6;}
        std::printf("FINAL_SETTLED drift=%.6f angle=%.6f omega=%.6f\n",m::length(tired.out.hands[0].p-start.p),
            m::angle(tired.out.weapons[0].q,start.q),m::length(tired.state.weapon[0].angularVelocity));
        ck(boundedSettle&&m::length(tired.out.hands[0].p-start.p)<.01&&m::angle(tired.out.weapons[0].q,start.q)<.035&&
           m::length(tired.state.weapon[0].angularVelocity)<.05,
           "exhausted sword/pole settles within authority-derived recovery budget without growing oscillation");
        tired.in.tracked[0]=false;tired.next(ms);
        ck(!tired.out.active[0]&&!tired.out.owned[0]&&tired.out.impactSpeed[0]==0,"tracking loss releases immediately without impulse or grace extension");
        tired.in.tracked[0]=true;tired.next(ms);
        ck(tired.out.active[0]&&tired.out.commandCount==0&&tired.out.impactSpeed[0]==0,
           "fresh reacquisition owns handling without emitting an attack or impact");
    }
    // H21 causal baseline replay: the same final production poses are also
    // measured against frozen e7f7841. Ordinary axes/maces must be unchanged;
    // supported class7 has modestly less acceleration, with the same envelope.
    for(int cls:{3,5,7})for(bool support:{false,true})for(unsigned ms:{8u,10u,16u}){
        HandlingReplay r(1,support,cls,ms);const auto start=r.in.hands[0];
        double early=0,maxLag=0,first=0;
        for(unsigned tick=ms;tick<=80;tick+=ms){
            r.in.hands[0].q=m::from_rotation_vector({0,0,.8*double(tick)/80.});r.next(ms);
            early=m::angle(r.out.weapons[0].q,start.q);if(tick==ms)first=early;
            maxLag=std::max(maxLag,m::angle(r.out.weapons[0].q,r.in.hands[0].q));
        }
        const auto limit=d::classCompliance59c14(d::compliance59c10r2(1,support),d::handling59c14(cls,support,r.in.poleOneHand59c14));
        ck(early>0&&finite(r.out.weapons[0])&&maxLag<=limit.radians+1e-6,"H21 final weapon acceleration remains finite with the existing pose bound");
        std::printf("H21_HEFT class=%d support=%d dt=%u firstRad=%.12f earlyRad=%.12f maxLag=%.12f bound=%.12f\n",cls,support,ms,first,early,maxLag,limit.radians);
    }
    for(bool experimental:{false,true}){
        Replay r;r.in.physicalCombat177=experimental;const auto start=std::chrono::steady_clock::now();
        for(int n=0;n<100000;++n){r.in.hands[0].p.x=.2+.08*std::sin(n*.02);r.next();}
        const auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()-start).count();
        std::printf("HOST_KERNEL_BENCH rung2=%d steps=100000 meanUs=%.4f nativeQueries=MOCK_CLEAR fps=NOT_INFERRED\n",experimental,double(ns)/100000000.);
        ck(r.out.active[0]&&finite(r.out.weapons[0]),"long repeated slow-motion sequence remains finite and owned");
    }
    stats59c17_cases(checks,failed); // R59.16C10R17
    std::printf("%d checks, %d failures\n",checks,failed);return failed?1:0;
}
