// C59 headset regression: actual physical kernel, not a duplicate gesture model.
#include "vrik/physical_weapon.h"
#include "vrik/user_modes175.h"
#include <cstdio>
#include <vector>
namespace ph=kcd2vr::physical;
namespace pm=kcd2vr::pose_core;
static unsigned checks36=0,failures36=0;
static void check36(bool yes,const char* text){++checks36;failures36+=!yes;std::printf("%s %s\n",yes?"PASS":"FAIL",text);}
struct Rig36 {
    ph::Input in{};ph::State state{};std::array<ph::Profile,2> profiles{};ph::Output out{};std::vector<ph::Command> commands;
    pm::Quat model{},tracking{};bool known=true,blocked=false,actor=false;ph::Vec3 normal{0,-1,0};
    Rig36(bool direct,ph::Vec3 axis={1,0,0},double yaw=0,double trackingYaw=0){
        model={0,0,std::sin(yaw*.5),std::cos(yaw*.5)};tracking={0,0,std::sin(trackingYaw*.5),std::cos(trackingYaw*.5)};
        in.allowed=in.tracked[0]=in.tracked[1]=in.intentTracked[0]=true;in.epoch=in.bodyEpoch=1;in.tick=1000;in.timeNs=1000000000;in.sample=1;
        in.requirePrimaryGrip114=in.heldMotionAttacks=in.controllerAnchor119=true;in.grip[0]=1;in.reach[0]=in.reach[1]=1.9;
        in.naturalCombat117=in.coherentMelee119=in.positionGuard176=in.attacks=true;
        const auto mode36=kcd2vr::user175::configuration(direct?kcd2vr::user175::Mode::CombatContactLab:kcd2vr::user175::Mode::CombatSynchronized);
        in.physicalCombat177=mode36.physical177;ph::select_forward_motion36(in,direct,!direct);
        in.dynamics=false;in.contact=true;in.strikeMinScaleGate59c14=false;
        in.windupHold57=false;in.guardEnterMaxSpeed178r29=2.5;in.guardEnterDwellMs178r29=0;in.guardExitSpeed178r45=3.5;
        in.shoulders[0]=pm::rotate(model,{.15,0,1.3});in.shoulders[1]=pm::rotate(model,{-.15,0,1.3});
        in.hands[0]={model,pm::rotate(model,{-.20,.35,1.45})};in.intentHands[0]={tracking,pm::rotate(tracking,{-.20,.35,1.45})};
        auto& p=profiles[0];p.key=77;p.generation=1;p.side=0;p.classId=4;p.primary=p.geometry=true;p.axis=axis;p.tip=axis*.6;
        // Explicit blade broad face in this synthetic profile; normal is perpendicular to blade.
        p.guardFaceReadyH24=true;p.guardFaceNormalH24={-axis.y,axis.x==0&&axis.y==0?1.:axis.x,0};
        for(unsigned i=0;i<30;++i)step();commands.clear();
    }
    void step(ph::Vec3 bodyDelta={},bool moveModel=true,bool moveTracking=true){
        ++in.sample;in.tick+=10;in.timeNs+=10000000;
        if(moveModel)in.hands[0].p=in.hands[0].p+pm::rotate(model,bodyDelta);
        if(moveTracking)in.intentHands[0].p=in.intentHands[0].p+pm::rotate(tracking,bodyDelta);
        out=ph::step(state,in,profiles,[&](const ph::Profile&,ph::Pose from,ph::Pose,bool){ph::Contact103 c{};c.known=known;
            if(blocked){c.fraction=actor?.5:0;c.plane=true;c.normal=pm::rotate(model,normal);c.point=from.p;
                if(actor){auto& n=c.native172;n.ownerRead=true;n.owner=1;n.collider=2;n.entityId=13;n.part=1;n.mask=kcd2vr::npc_contact172::BodiesAndWorld;n.body=kcd2vr::npc_contact172::Body::Living;}}
            return c;});
        for(unsigned i=0;i<out.commandCount;++i)commands.push_back(out.commands[i]);
    }
    unsigned count(ph::CommandKind k)const{unsigned n=0;for(auto c:commands)n+=c.kind==k;return n;}
    unsigned hits()const{return count(ph::CommandKind::Slash)+count(ph::CommandKind::Thrust)+count(ph::CommandKind::Pommel);}
    std::uint64_t first(ph::CommandKind k)const{for(auto c:commands)if(c.kind==k)return c.tick;return 0;}
};
int main(){
    // Headset 21:11:45-48: tip3.5-12.7m/s while hand .13-.55m/s,
    // committed in34-66ms. Same small fixed-hand arc is a guard adjustment.
    for(bool direct:{false,true})for(double handSpeed:{0.,.20,.50}){
        Rig36 r(direct);check36(r.out.guard117,"C59 recorded-wrist replay starts in guard");
        for(int n=1;n<=10;++n){
            r.in.hands[0].q=pm::multiply(r.model,pm::from_rotation_vector({0,0,n*.10}));
            r.in.intentHands[0].q=pm::multiply(r.tracking,pm::from_rotation_vector({0,0,n*.10}));
            r.step({0,handSpeed*.010,0});
        }
        std::printf("wrist guard direct=%d handSpeed=%.2f prepares=%u hits=%u\n",direct,handSpeed,r.count(ph::CommandKind::Prepare),r.hits());
        check36(!r.out.guard117&&r.count(ph::CommandKind::Prepare)==1&&r.hits()==1,"H27 C54 edge sweep qualifies once AFTER current spatial guard releases, without extra hand translation");
    }
    for(bool direct:{false,true}){
        Rig36 r(direct);r.in.hands[0].p=r.in.intentHands[0].p={.70,.35,1.45};r.state={};
        for(int n=0;n<30;++n)r.step();r.commands.clear();
        for(int n=0;n<30;++n)r.step({0,.023,0});
        check36(!r.hits()&&!r.count(ph::CommandKind::Prepare),"C59 both live modes reject the2.3m/s slow cut rather than banking its path");
    }
    for(bool direct:{false,true}){
        Rig36 r(direct);
        for(int n=1;n<=10;++n){
            r.in.hands[0].q=r.in.intentHands[0].q=pm::from_rotation_vector({0,0,n*.10});
            r.in.intentHands[0].p=r.in.intentHands[0].p+ph::Vec3{0,.002,0};
            r.step({0,.020,0},true,false);
        }
        check36(!r.out.guard117&&r.hits()==1&&r.count(ph::CommandKind::Prepare)==1,"H27 both rotating-edge families qualify after guard release despite unequal hand translation");
    }
    for(bool direct:{false,true}){
        Rig36 r(direct);
        for(int n=1;n<=20;++n){
            r.in.hands[0].q=r.in.intentHands[0].q=pm::from_rotation_vector({0,0,n*.06});
            r.step({0,.005,0});
        }
        std::printf("long wrist guard direct=%d handSpeed=.50 prepares=%u hits=%u\n",direct,r.count(ph::CommandKind::Prepare),r.hits());
        check36(!r.out.guard117&&r.hits()==1&&r.count(ph::CommandKind::Prepare)==1,"H27 C54 long deliberate edge arc qualifies once after guard release despite slow hand drift");
    }
    for(bool direct:{false,true}){
        Rig36 r(direct);
        for(int n=1;n<=10;++n){
            r.in.hands[0].q=r.in.intentHands[0].q=pm::from_rotation_vector({0,0,n*.10});
            r.step({0,.010,0});
        }
        check36(r.count(ph::CommandKind::Prepare)==1&&r.hits()==1,"C59 deliberate arm-assisted guarded cut retains one preparation and commitment");
    }
    // Explicitly preserve purposeful wrist cuts away from a guarded origin.
    for(bool direct:{false,true}){
        Rig36 r(direct);r.in.hands[0].p=r.in.intentHands[0].p={.70,.35,1.45};r.state={};
        for(int n=0;n<30;++n)r.step();r.commands.clear();
        check36(!r.out.guard117,"C59 free wrist control is outside guard");
        for(int n=1;n<=10;++n){r.in.hands[0].q=r.in.intentHands[0].q=pm::from_rotation_vector({0,0,n*.10});r.step();}
        check36(r.hits()==1,"C59 deliberate wrist cut outside guard remains available");
    }
    // Side cuts reaching an actual body: geometry's first approached actor
    // contact, both independent hand trajectories, same fractional increment.
    for(bool direct:{false,true})for(double sign:{-1.,1.}){
        Rig36 r(direct,{0,0,1});r.normal={-sign,0,0};r.commands.clear();
        for(unsigned n=0;n<4;++n)r.step({sign*.025,.002,0});
        r.blocked=r.actor=true;r.step({sign*.025,.002,0});
        std::printf("side contact direct=%d sign=%g prepares=%u hits=%u\n",direct,sign,r.count(ph::CommandKind::Prepare),r.hits());
        check36(r.count(ph::CommandKind::Prepare)==1&&r.hits()==1,"C59 either-side deliberate first body contact can leave guard");
        const auto count=r.hits();for(int n=0;n<10;++n)r.step({sign*.025,.002,0});
        check36(r.hits()==count,"C59 continued lateral body pressure cannot replay the attack");
    }
    for(bool direct:{false,true})for(unsigned blocked=0;blocked<3;++blocked){
        Rig36 r(direct,{0,0,1});for(unsigned n=0;n<4;++n)r.step({-.025,.002,0});
        r.blocked=blocked==1;r.known=blocked!=2;r.step({-.025,.002,0});
        check36(!r.hits()&&!r.count(ph::CommandKind::Prepare),"C59 lateral free parry/world/unknown is not a body-contact strike");
    }
    // Thrust has its own translation thresholds, not the guard-cut7cm proof.
    for(bool direct:{false,true}){
        Rig36 r(direct,{0,1,0});for(int n=0;n<12;++n)r.step({0,.015,0});
        check36(r.count(ph::CommandKind::Thrust)==1,"C59 axial1.5m/s thrust retains its lower speed policy");
    }
    for(bool direct:{false,true})for(double sign:{-1.,1.}){
        const ph::Vec3 axis{sign*.5,.866025403784,0};Rig36 r(direct,axis);
        check36(!r.out.guard117,"H24 steep diagonal tip-forward posture is ready to thrust, not broad-face block");
        for(int n=0;n<10;++n)r.step(axis*.020);
        check36(r.count(ph::CommandKind::Thrust)==1&&r.count(ph::CommandKind::Slash)==0,
            "C59 genuine tip-forward counterstab keeps THRUST from either physical side");
    }
    std::printf("guard_origin59=%u/%u native_calls=NONE forced_outcomes=NONE\n",checks36-failures36,checks36);return failures36?1:0;
}
