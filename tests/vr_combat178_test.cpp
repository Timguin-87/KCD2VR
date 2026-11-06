#include "vrik/weapon_sweep112.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace kcd2vr::physical;
static unsigned checks=0;
static void ck(bool ok,const char* label){++checks;if(!ok){std::fprintf(stderr,"FAIL %s\n",label);std::exit(1);}}
struct Replay {
    Input in{};State state{};std::array<Profile,2> profiles{};Output out{};double time=1;bool known=true,blocked=false,bodyContact=false;
    std::vector<Command> commands;
    Replay(){in.epoch=in.bodyEpoch=in.sample=1;in.tick=1000;in.timeNs=1000000000;
        in.allowed=in.tracked[0]=in.tracked[1]=in.intentTracked[0]=true;in.grip[0]=1;
        in.dynamics=false;in.contact=true;in.naturalCombat117=in.heldMotionAttacks=in.requirePrimaryGrip114=true;
        in.coherentMelee119=in.controllerAnchor119=true;
        in.shoulders[0]={0,0,1.3};in.shoulders[1]={-.2,0,1.3};in.reach[0]=1.3;
        in.hands[0].p={-.25,.2,1.2};in.intentHands[0]=in.hands[0];
        auto& p=profiles[0];p.key=p.generation=1;p.classId=4;p.primary=p.geometry=true;p.axis={0,1,0};p.tip={0,.6,0};
        sample();rest(.12);
    }
    void sample(){out=step(state,in,profiles,[&](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=known;c.fraction=blocked?0:1;if(blocked&&bodyContact){c.plane=true;c.fraction=.5;c.normal={0,-1,0};c.point={0,1,1.2};c.native172.entityId=4711;c.native172.body=kcd2vr::npc_contact172::Body::Articulated;}return c;});
        for(unsigned i=0;i<out.commandCount;++i)commands.push_back(out.commands[i]);}
    void move(Vec3 delta,double dt=.01,bool model=true,bool relative=true){time+=dt;in.timeNs=std::uint64_t(std::llround(time*1e9));in.tick=in.timeNs/1000000;++in.sample;
        if(model)in.hands[0].p=in.hands[0].p+delta;if(relative)in.intentHands[0].p=in.intentHands[0].p+delta;sample();}
    void rest(double seconds){for(int n=0;n<int(std::ceil(seconds/.01));++n)move({});}
    int count(CommandKind kind)const{int n=0;for(auto c:commands)n+=c.kind==kind;return n;}
    int hits()const{return count(CommandKind::Slash)+count(CommandKind::Thrust)+count(CommandKind::Pommel);}
};
// V66.178 F4 rung 2 VR combat core: strike quality + native rhythm gate.
static Output::Strike178 last{};
static void setup(Replay& r,bool busy,double minScale,std::uint64_t interval=550){r.in.physicalCombat177=true;r.in.constraintDrive129=true;r.in.dynamics=true;
    r.in.nativeStateKnown178=true;r.in.nativeAttackBusy178=busy;r.in.strikeMinScale178=minScale;r.in.strikeMinIntervalMs178=interval;}
static void thrust(Replay& r,double step=.02){for(int n=0;n<11;++n){r.move({0,step,0});if(r.out.strike178.serial)last=r.out.strike178;}r.rest(.05);}
int main(){
    {Replay r;setup(r,false,0);thrust(r);
     ck(r.hits()==1&&last.accepted&&last.tipSpeed>0&&last.scale>0,"rung 2: a deliberate thrust commits once and reports a measured strike");
     std::printf("measured thrust: tip=%.2f m/s E=%.2f J ref=%.2f J scale=%.2f\n",last.tipSpeed,last.energy,last.reference,last.scale);}
    {Replay r;setup(r,true,0);thrust(r);ck(r.hits()==0,"rung 2: native attack still requested/prepared -> no new native attack (rhythm)");}
    {Replay r;setup(r,false,0);r.in.nativeStateKnown178=false;r.in.nativeAttackBusy178=false;thrust(r);
     ck(r.hits()==1,"V178R3 rung 2: unknown native state no longer blocks (min interval still applies)");}
    {Replay r;setup(r,false,1.0);last={};thrust(r);
     // R59.13 (user): a light stroke is still a telegraphed attack (the NPC can block it);
     // only its strength is small. reason=3 marks WEAK, the commit still happens.
     ck(r.hits()==1&&last.accepted&&last.reason==3&&last.scale<1.0,"R59.13 rung 2: a light stroke still commits as an attack, reported WEAK");}
    {Replay r;setup(r,false,0,550);thrust(r);r.move({0,-.2,0});r.rest(.30);thrust(r);
     ck(r.hits()==1,"rung 2: second strike inside the rhythm interval does not queue a native attack");}
    {Replay r;setup(r,false,0,550);thrust(r);r.move({0,-.2,0});r.rest(.80);thrust(r);
     ck(r.hits()==2,"rung 2: second strike after the rhythm interval commits");}
    // V66.178R1: wrist keeps rotating (tip ~1.2 m/s) while the hand is calm -> still re-arms in rung 2.
    {Replay r;setup(r,false,0,550);thrust(r);r.move({0,-.2,0});
     for(int n=0;n<70;++n){const double a=.35*std::sin(n*.08);r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,a});r.move({});}
     r.in.hands[0].q=r.in.intentHands[0].q=Quat{};r.move({});thrust(r);
     ck(r.hits()==2,"rung 2: a rotating wrist with a calm hand re-arms (hand-based re-arm)");}
    {Replay r;thrust(r);r.move({0,-.2,0});
     for(int n=0;n<70;++n){const double a=.35*std::sin(n*.08);r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,a});r.move({});}
     r.in.hands[0].q=r.in.intentHands[0].q=Quat{};r.move({});thrust(r);
     std::printf("rung 1 same motion: hits=%d (old strict re-arm)\n",r.hits());}
    // V66.178R3: touching an enemy body (not yet the native opponent) must not cancel the stroke in rung 2.
    {Replay r;setup(r,false,0);for(int n=0;n<6;++n)r.move({0,.02,0});r.blocked=r.bodyContact=true;for(int n=0;n<5;++n)r.move({0,.02,0});r.rest(.05);
     ck(r.hits()==1,"V178R3 rung 2: enemy body contact keeps the stroke -> native attack commits (starts the fight)");}
    {Replay r;for(int n=0;n<6;++n)r.move({0,.02,0});r.blocked=r.bodyContact=true;for(int n=0;n<5;++n)r.move({0,.02,0});r.rest(.05);
     std::printf("rung 1 same body contact: hits=%d (old abort)\n",r.hits());}
    // V66.178R4 review case: a slow 1.3 m/s thrust must not be promoted by wrist roll about the blade axis.
    {double sc[2]{};int hits[2]{};for(int roll=0;roll<2;++roll){Replay r;setup(r,false,.0);last={};
        for(int n=0;n<17;++n){if(roll){const double a=.10*(n+1);r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,a,0});}
            r.move({0,.013,0});if(r.out.strike178.serial)last=r.out.strike178;}r.rest(.05);sc[roll]=last.scale;hits[roll]=r.hits();}
     std::printf("slow thrust scale: no roll=%.3f with 10 rad/s roll=%.3f (hits %d/%d)\n",sc[0],sc[1],hits[0],hits[1]);
     ck(sc[0]>0&&std::abs(sc[1]-sc[0])<.05,"V178R4: wrist roll about the blade axis adds no attack energy");
     ck(sc[0]<.70&&sc[1]<.70,"V178R4/R59.13: a 1.3 m/s thrust stays a WEAK stroke (small energy), with or without roll");}
    // V66.178R5: the R3 body-contact case with the DEFAULT 0.35 threshold. Contact resolution zeroes the
    // body velocity before the strike is measured; the approach speed must be scored, not the stopped blade.
    {Replay r;setup(r,false,.35);last={};for(int n=0;n<6;++n)r.move({0,.02,0});r.blocked=r.bodyContact=true;
     for(int n=0;n<5;++n){r.move({0,.02,0});if(r.out.strike178.serial)last=r.out.strike178;}r.rest(.05);
     std::printf("body-contact thrust at default threshold: useful=%.2f m/s scale=%.2f accepted=%d\n",last.tipSpeed,last.scale,last.accepted);
     ck(r.hits()==1&&last.accepted&&last.scale>=.35,"V178R5 rung 2: a 2 m/s thrust that touches the enemy is still scored as an attack at the default threshold");}
    // V66.178R6: the 2026-10-08 headset run stepped at 18-20 fps (50-55 ms). The old 50 ms sample
    // guard reset the stroke machine every other sample (81x UNAVAILABLE) and nothing could commit.
    {Replay r;setup(r,false,.35);last={};for(int n=0;n<5;++n){r.move({0,.11,0},.055);if(r.out.strike178.serial)last=r.out.strike178;}
     for(int n=0;n<6;++n)r.move({},.055);
     std::printf("18 fps thrust (55 ms samples): hits=%d scale=%.2f\n",r.hits(),last.scale);
     ck(r.hits()==1&&last.accepted,"V178R6 rung 2: a 2 m/s thrust sampled at 55 ms still prepares, commits and is scored");}
    {Replay r;setup(r,false,.35);last={};for(int n=0;n<5;++n){r.move({0,.11,0},.055);if(r.out.strike178.serial)last=r.out.strike178;}
     r.move({0,.11,0},.300);for(int n=0;n<6;++n)r.move({},.055);
     ck(r.count(CommandKind::Abort)<=1,"V178R6: a 300 ms hitch ends only the current candidate (no stuck state)");}
    // V66.178R18/R19 impale latch handshake (review F01/F02): REQUESTED -> first pin -> ACTIVE, display-only.
    {Replay r;setup(r,false,.35);r.in.impaleEnabled178r15=true;r.in.haveWorld=true;r.in.worldScale=1;r.in.world=Pose{};r.rest(.15);
     for(int n=0;n<6;++n)r.move({0,.04,0});r.blocked=r.bodyContact=true;
     std::uint64_t serial=0;Vec3 tip{};int requestFrames=0;
     for(int n=0;n<5;++n){r.move({0,.04,0});if(r.out.impaleLatchRequest){serial=r.out.impaleSerial;tip=r.out.impaleTipWorld;++requestFrames;}}
     std::printf("latch: strike scale=%.2f accepted=%d request=%d\n",r.out.strike178.scale,r.out.strike178.accepted,requestFrames);
     ck(serial!=0&&!r.out.impaleActive&&!r.out.impaleReleased,"latch requested after the accepted body-stopped thrust; not active, not released without a pin");
     for(int n=0;n<5;++n){r.move({});if(r.out.impaleLatchRequest)++requestFrames;}
     ck(requestFrames>=6&&!r.out.impaleReleased,"the request is re-published while the first pin is awaited (no release inside the 250 ms window)");
     r.in.impalePinValid178r15=true;r.in.impalePinWorld178r15=tip;r.in.impalePinSerial178r19=serial+1;r.move({});
     ck(!r.out.impaleActive,"a pin with another serial is not this latch's pin");
     r.in.impalePinSerial178r19=serial;r.move({});
     ck(r.out.impaleActive&&r.out.displayLatched[0],"serial-matched pin: ACTIVE, display packet latched");
     const auto physW=r.out.weapons[0],shownW=r.out.displayWeapons[0];
     r.blocked=r.bodyContact=false; // pulling away from the body is not blocked
     for(int n=0;n<4;++n)r.move({0,-.03,0}); // the hand overshot the stopped blade; 12 cm back is not yet 14 cm behind the pinned grip
     ck(r.out.impaleActive&&r.out.impalePull<.14&&m::length(r.out.weapons[0].p-r.out.displayWeapons[0].p)>.01,"pulling 12 cm: still pinned; display stays on the pin while the physical weapon follows the hand (display-only)");
     ck(m::length(r.out.displayWeapons[0].p-shownW.p)<.02,"the displayed weapon did not move with the hand");
     (void)physW;
     bool released=false;for(int n=0;n<12&&!released;++n){r.move({0,-.03,0});released=r.out.impaleReleased;}
     ck(released,"pulling the hand 14 cm behind the pinned grip releases");r.move({});
     ck(!r.out.displayLatched[0]&&m::length(r.out.weapons[0].p-r.out.displayWeapons[0].p)<1e-9,"after release display == physical again");}
    {Replay r;thrust(r);ck(r.hits()==1,"rung 1 (physicalCombat177 off) unchanged");}
    std::printf("vr_combat178 %u checks, 0 failures\n",checks);return 0;}
