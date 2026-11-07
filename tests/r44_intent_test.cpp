#include "vrik/weapon_sweep112.h"
#include <cstdio>
#include <vector>
using namespace kcd2vr::physical;
static int failures=0;
static void ck(bool value,const char* label){std::printf("%s %s\n",value?"PASS":"FAIL",label);failures+=!value;}
struct Replay44 {
    Input in{};State state{};std::array<Profile,2> profiles{};Output out{};
    bool blocked=false;Vec3 normal{0,-1,0};std::vector<Command> commands;
    Replay44(){
        in.epoch=in.bodyEpoch=in.sample=1;in.tick=1000;in.timeNs=1000000000;
        in.allowed=in.tracked[0]=in.tracked[1]=in.intentTracked[0]=true;in.grip[0]=1;
        in.contact=in.dynamics=in.physicalCombat177=in.constraintDrive129=true;
        in.naturalCombat117=in.coherentMelee119=in.heldMotionAttacks=in.requirePrimaryGrip114=in.controllerAnchor119=true;
        in.attackAtPrepare178r42=in.contactStartsAttack178r41=true;in.weaponAuthorityScale178r44=.6;
        in.nativeStateKnown178=true;in.strikeMinScale178=.35;in.strikeMinIntervalMs178=550;
        in.shoulders[0]={0,0,1.3};in.shoulders[1]={-.2,0,1.3};in.reach[0]=1.5;
        in.hands[0].p={-.25,.2,1.2};in.intentHands[0]=in.hands[0];
        auto& p=profiles[0];p.key=p.generation=1;p.classId=4;p.primary=p.geometry=true;
        p.weight=3.4;p.axis={0,1,0};p.tip={0,.6,0};sample();
        for(int n=0;n<15;++n)move({});commands.clear();
    }
    void sample(){
        out=step(state,in,profiles,[&](const Profile&,Pose,Pose,bool){
            Contact103 c{};c.known=true;c.fraction=1;
            if(blocked){c.fraction=.5;c.plane=true;c.normal=normal;c.point={0,1,1.2};
                c.native172.mask=kcd2vr::npc_contact172::BodiesAndWorld;c.native172.ownerRead=true;c.native172.owner=23;c.native172.collider=24;c.native172.part=2;
                c.native172.entityId=4711;c.native172.body=kcd2vr::npc_contact172::Body::Articulated;}
            return c;});
        for(unsigned i=0;i<out.commandCount;++i)commands.push_back(out.commands[i]);
    }
    void move(Vec3 delta,bool relative=true){in.tick+=10;in.timeNs=in.tick*1000000;++in.sample;
        in.hands[0].p=in.hands[0].p+delta;if(relative)in.intentHands[0].p=in.intentHands[0].p+delta;sample();}
    int hits()const {int n=0;for(const auto& c:commands)n+=c.kind==CommandKind::Slash||c.kind==CommandKind::Thrust||c.kind==CommandKind::Pommel;return n;}
};
int main(){
    {Replay44 r;r.in.motionOwned59c5=true;r.in.strikeMinIntervalMs178=0;
        r.blocked=true;r.in.tick+=40;r.in.timeNs=r.in.tick*1000000;++r.in.sample;
        r.in.hands[0].p=r.in.hands[0].p+Vec3{0,.16,0};r.in.intentHands[0].p=r.in.intentHands[0].p+Vec3{0,.16,0};r.sample();
        std::uint64_t id=0;int prepares=0,hits=0;
        for(const auto& c:r.commands){if(c.kind==CommandKind::Prepare){id=c.gesture59c3;++prepares;}
            if(c.kind==CommandKind::Thrust){++hits;ck(id&&c.gesture59c3==id,"C5 first-contact prepare/commit own a fresh shared gesture");}}
        ck(prepares==1&&hits==1,"C5 first-sample physical thrust reaches producer once");
        ck(r.out.impactRecord178r30[0].valid&&r.out.impactRecord178r30[0].gesture59c3==id,"C5 immutable impact belongs to that first-sample thrust");
    }

    for(bool stab:{false,true}){
        Replay44 r;r.in.strikeMinIntervalMs178=0;
        for(int n=0;n<8;++n)r.move(stab?Vec3{0,.04,0}:Vec3{.12,0,0});
        std::uint64_t gesture=0;bool prepared=false,released=false;
        for(const auto& c:r.commands){
            if(c.kind==CommandKind::Prepare){gesture=c.gesture59c3;prepared=true;}
            if(c.kind==CommandKind::Slash||c.kind==CommandKind::Thrust){released=true;ck(gesture&&c.gesture59c3==gesture,"prepare and deliberate release retain the same gesture identity");}
        }
        ck(prepared&&released,"both cuts and thrusts expose prepare/release identity");
        r.normal=stab?Vec3{0,-1,0}:Vec3{-1,0,0};r.blocked=true;r.move(stab?Vec3{0,.04,0}:Vec3{.12,0,0});
        const auto& impact=r.out.impactRecord178r30[0];
        ck(impact.valid&&impact.gesture59c3==gesture,"a stopped blade keeps the deliberate gesture on its contact");
    }
    {Replay44 r;for(int n=0;n<3;++n)r.move({0,.02,0});
        ck(r.hits()==0,"6cm native preparation alone must not release an attack");}
    {Replay44 r;r.in.strikeMinScale178=1.;for(int n=0;n<12;++n)r.move({0,.02,0});
        ck(r.hits()==1,"deliberate coherent thrust is not discarded by a second simulated-energy threshold");}
    {Replay44 r;r.in.strikeMinScale178=0.;for(int n=0;n<100;++n)r.move({0,.0005,0});
        ck(r.hits()==0,"lowering energy threshold still cannot turn slow drift into a deliberate attack");}
    {Replay44 r;for(int n=0;n<12;++n)r.move({0,.04,0});
        double speed=0;for(const auto& c:r.commands)if(c.kind==CommandKind::Thrust)speed=c.timingSpeed178r22c2;
        ck(r.hits()==1&&speed>1.,"deliberate fast thrust releases once with measured speed for native timing");}
    {Replay44 r;for(int n=0;n<3;++n)r.move({0,.04,0},false);r.blocked=true;r.move({0,.04,0},false);
        ck(r.out.blocked[0]&&r.hits()==0,"body contact during common/model-only motion stops the blade without inventing an attack");}
    {Replay44 r;r.in.nativeAttackBusy178=true;for(int n=0;n<10;++n)r.move({0,.03,0});
        ck(r.hits()==0,"busy native action retains admission instead of queuing a second attack");}
    {Replay44 r;r.in.nativeAttackBusy178=true;for(int n=0;n<3;++n)r.move({0,.03,0});
        r.in.nativeAttackBusy178=false;for(int n=0;n<9;++n)r.move({0,.03,0});
        ck(r.hits()==1,"temporary preparation veto cannot discard the rest of the same deliberate gesture");}
    {Replay44 r;r.in.physicalCombat177=false;r.in.attackAtPrepare178r42=false;for(int n=0;n<12;++n)r.move({0,.03,0});
        double speed=0;for(const auto& c:r.commands)if(c.kind==CommandKind::Thrust)speed=c.timingSpeed178r22c2;
        ck(r.hits()==1&&speed>1.,"rung 1 carries deliberate thrust speed instead of a permanent zero");}
    for(bool rung2:{false,true}){
        Replay44 r;r.in.physicalCombat177=rung2;r.in.positionGuard176=true;r.in.strikeMinIntervalMs178=0;
        // Forward-facing weapon is outside guard; identical motion in both frames.
        // A complete fast cut finishes at 50 ms, before the old fixed age gate.
        for(int n=0;n<5;++n)r.move({.12,0,0});
        ck(r.hits()==1,rung2?"F4 R2 fast 60cm cut releases before the gesture has finished":"F4 R1 fast 60cm cut releases before the gesture has finished");
    }
    {Replay44 r;r.in.physicalCombat177=false;r.in.positionGuard176=true;
        for(int n=0;n<12;++n)r.move({0,.03,0});
        r.commands.clear();
        // Retraction is hand recovery, with a still-moving tip. Old R1's
        // strict tip-quiet gate cannot rearm this ordinary preparation.
        for(int n=0;n<28;++n){
            r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,n%2?.01:-.01});
            r.move({0,-.0025,0});
        }
        for(int n=0;n<12;++n)r.move({0,.03,0});
        ck(r.hits()==1,"F4 R1 rearms after quiet hand recovery without requiring a frozen weapon tip");
    }
    { // Actual recognizer: same fixed-wrist arc as supplied case, first body contact at 40 ms.
        namespace ms=kcd2vr::melee118;ms::State st{};st.ready=true;bool hit=false,aborted=false;int prepared=0;
        for(int f=1;f<=4;++f){ms::Input in{};in.eligible=in.known=true;in.dt=.010;
            const double a0=(f-1)*1.57079632679/15,a1=f*1.57079632679/15;
            in.tip=in.modelTip={.6*(std::cos(a1)-std::cos(a0)),.6*(std::sin(a1)-std::sin(a0)),0};
            in.axis=in.modelAxis={std::cos(a1),std::sin(a1),0};
            if(f==4){in.blocked=true;in.contactEntity175=456;in.contactFraction175=1.;}
            const auto v=ms::update(st,in,{true,true});prepared+=v.prepare;hit|=v.commit;aborted|=v.abort;
        }
        ck(prepared==1,"REVIEW R46 wrist contact fixture prepared by 30 ms");
        ck(hit&&!aborted,"REVIEW R46 deliberate 25cm wrist arc at first NPC contact commits rather than aborts");
    }
    for(bool startInGuard:{false,true}){
        Replay44 r;r.in.physicalCombat177=false;r.in.dynamics=false;r.in.positionGuard176=true;
        r.in.guardEnterMaxSpeed178r29=2.5;r.in.guardExitSpeed178r45=3.5;r.in.guardEnterDwellMs178r29=0;
        r.in.hands[0].p=r.in.intentHands[0].p={startInGuard?-.25:.65,.2,1.2};
        r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,-1.57079632679});
        r.state={};r.sample();for(int n=0;n<20;++n)r.move({});r.commands.clear();
        ck(r.out.guard117==startInGuard,"REVIEW R46 wrist producer fixture has requested initial guard state");
        int guardFrames=0;
        for(int n=1;n<=15;++n){
            r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,-1.57079632679+n*1.57079632679/15});
            r.move({});guardFrames+=r.out.guard117;
        }
        std::printf("REVIEW wrist producer inside=%d hits=%d guardFrames=%d\n",startInGuard,r.hits(),guardFrames);
        ck(r.hits()==1,startInGuard?"REVIEW R46 6.3m/s wrist cut beginning in guard must reach attack producer":"REVIEW R46 control wrist cut outside guard reaches attack producer");
    }
    std::printf("R44 intent failures=%d; native calls=NONE\n",failures);return failures?1:0;
}
