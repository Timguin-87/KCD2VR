// Actual physical kernel: ordinary deliberate forward strokes out of held guard.
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
    pm::Quat model{},tracking{};bool known=true,blocked=false,actor=false;
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
        const double gripX=axis.x<-.9?.20:-.20;
        in.hands[0]={model,pm::rotate(model,{gripX,.35,1.45})};in.intentHands[0]={tracking,pm::rotate(tracking,{gripX,.35,1.45})};
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
            if(blocked){c.fraction=actor?.5:0;c.plane=true;c.normal=pm::rotate(model,{0,-1,0});c.point=from.p;
                if(actor){auto& n=c.native172;n.ownerRead=true;n.owner=1;n.collider=2;n.entityId=13;n.part=1;n.mask=kcd2vr::npc_contact172::BodiesAndWorld;n.body=kcd2vr::npc_contact172::Body::Living;}}
            return c;});
        for(unsigned i=0;i<out.commandCount;++i)commands.push_back(out.commands[i]);
    }
    unsigned count(ph::CommandKind k)const{unsigned n=0;for(auto c:commands)n+=c.kind==k;return n;}
    unsigned hits()const{return count(ph::CommandKind::Slash)+count(ph::CommandKind::Thrust)+count(ph::CommandKind::Pommel);}
    std::uint64_t first(ph::CommandKind k)const{for(auto c:commands)if(c.kind==k)return c.tick;return 0;}
};
int main(){
    {Rig36 r(false);r.in.intent59c16=true;r.in.attackAtPrepare178r42=true;
     for(unsigned i=0;i<14;++i)r.step({0,.025,0});
     std::printf("H29 intent Prepare=%llu Slash=%llu\n",(unsigned long long)r.first(ph::CommandKind::Prepare),(unsigned long long)r.first(ph::CommandKind::Slash));
     check36(r.count(ph::CommandKind::Prepare)==1&&r.count(ph::CommandKind::Slash)==1&&
         r.first(ph::CommandKind::Prepare)<r.first(ph::CommandKind::Slash),"H29 intent announces qualified preparation before commitment, even with legacy early-release option enabled");
     std::uint64_t gesture=0;ph::Vec3 origin{};
     for(const auto& c:r.commands)if(c.kind==ph::CommandKind::Prepare){gesture=c.gesture59c3;origin=c.origin167;}
     for(const auto& c:r.commands)if(c.kind==ph::CommandKind::Slash)check36(c.gesture59c3==gesture&&gesture&&pm::length(c.origin167-origin)<1e-9,"early preparation and commitment retain same gesture and origin");
    }
    {Rig36 r(false);r.in.intent59c16=true;
     for(unsigned i=0;i<10;++i)r.step({0,.025,0});
     for(unsigned i=0;i<4;++i)r.step({0,-.025,0});
     check36(r.count(ph::CommandKind::Prepare)==1&&r.count(ph::CommandKind::Abort)==1&&!r.hits(),"qualified but withdrawn intent cancels preparation without committing an attack");
    }
    {Rig36 r(false);r.in.intent59c16=true;
     for(unsigned i=0;i<7;++i)r.step({0,-.025,0});
     check36(!r.count(ph::CommandKind::Prepare)&&!r.hits(),"backward windup never becomes early native attack preparation");
    }
    {Rig36 r(false,{0,1,0});r.in.intent59c16=true;r.in.attackAtPrepare178r42=true;
     for(unsigned i=0;i<10;++i)r.step({0,.04,0});
     check36(r.count(ph::CommandKind::Prepare)==1&&r.count(ph::CommandKind::Thrust)==1&&
         r.first(ph::CommandKind::Prepare)<r.first(ph::CommandKind::Thrust),"forward stab announces qualified preparation before one committed thrust");
    }
    for(unsigned failure=0;failure<3;++failure){Rig36 r(false);r.in.intent59c16=true;
     for(unsigned i=0;i<10;++i)r.step({0,.025,0});
     check36(r.count(ph::CommandKind::Prepare)==1&&!r.hits(),"negative begins with one live qualified preparation");
     if(failure==0)r.in.allowed=false;
     if(failure==1)r.in.tracked[0]=false;
     if(failure==2)++r.profiles[0].generation;
     r.step({0,.025,0});
     check36(!r.hits()&&!r.state.weapon[0].prepared,"context, tracking or item replacement retires early preparation without attack");
    }
    for(bool direct:{false,true})for(ph::Vec3 axis:{ph::Vec3{1,0,0},ph::Vec3{-1,0,0},ph::Vec3{0,0,1},ph::Vec3{0,0,-1}}){
        Rig36 r(direct,axis);check36(r.out.guard117,"settles actual transverse/up/down held guard");const auto start=r.in.tick;
        r.step({0,.025,0});r.step({0,.025,0});
        check36(r.out.guard117&&!r.count(ph::CommandKind::Prepare),"first 5cm is still guard; no first-sample native offer");
        for(unsigned i=0;i<12;++i)r.step({0,.025,0});
        check36(r.count(ph::CommandKind::Prepare)==1&&r.count(ph::CommandKind::Slash)==1&&r.hits()==1,
            "2.5m/s complete cut from each starting axis prepares and commits once in HYBRID/DIRECT");
        check36(r.first(ph::CommandKind::Prepare)>=start+40&&r.first(ph::CommandKind::Prepare)<r.first(ph::CommandKind::Slash),
            "normal preparation travel precedes commitment without arbitrary fixed wait");
    }
    for(bool direct:{false,true})for(double yaw:{0.,1.5707963267948966,-2.2}){
        Rig36 r(direct,{1,0,0},yaw,yaw+.7);for(unsigned i=0;i<15;++i)r.step({.006,.025,-.005});
        check36(r.count(ph::CommandKind::Prepare)==1&&r.count(ph::CommandKind::Slash)==1,
            "common body yaw plus different tracking calibration preserves upper/diagonal forward cut");
    }
    for(bool direct:{false,true}){
        // Axis faces forward only by .5; old axial guard-exit demanded .6.
        Rig36 r(direct,{.866025403784,.5,0});check36(r.out.guard117,"diagonal stab starts in held guard");
        for(unsigned i=0;i<10;++i)r.step({.01732050807568,.010,0});
        check36(r.count(ph::CommandKind::Prepare)==1&&r.count(ph::CommandKind::Thrust)==1,
            "diagonal forward axial stab uses existing classifier preparation instead of stricter guard axis");
    }
    for(bool direct:{false,true})for(unsigned kind=0;kind<8;++kind){
        Rig36 r(direct);const ph::Vec3 delta=kind==0?ph::Vec3{.045,.010,0}:kind==1?ph::Vec3{0,.005,.045}:
            kind==2?ph::Vec3{0,-.025,0}:kind==3?ph::Vec3{0,.004,0}:ph::Vec3{0,.025,0};
        if(kind==6)r.blocked=true;if(kind==7)r.known=false;
        for(unsigned i=0;i<8;++i){r.step(delta,kind!=4,kind!=5);if(kind==2&&i==3)break;}
        check36(!r.count(ph::CommandKind::Prepare)&&!r.hits(),
            "lateral/upward parry, withdrawal, slow reach, single-frame-family motion and blocked/unknown do not hand off");
    }
    for(bool direct:{false,true}){
        Rig36 r(direct);for(unsigned i=0;i<4;++i)r.step({0,.025,0});
        for(unsigned i=0;i<3;++i)r.step({0,-.025,0});
        for(unsigned i=0;i<12;++i)r.step();
        check36(r.out.guard117&&!r.count(ph::CommandKind::Prepare)&&!r.hits(),"short guard recovery and reversal cannot bank travel");
        for(unsigned i=0;i<3;++i)r.step({0,.025,0});++r.profiles[0].generation;r.step();r.commands.clear();
        for(unsigned i=0;i<2;++i)r.step({0,.025,0});
        check36(!r.count(ph::CommandKind::Prepare),"weapon generation change retires shadow travel");
    }
    for(bool direct:{false,true}){
        Rig36 r(direct);for(unsigned i=0;i<4;++i)r.step({0,.025,0});
        r.blocked=r.actor=true;r.step({0,.025,0});
        check36(r.count(ph::CommandKind::Prepare)==1&&r.count(ph::CommandKind::Slash)==1,
            "checked first body impact may complete existing deliberate guard candidate before free-space prep travel");
        const auto before=r.hits();for(unsigned i=0;i<8;++i)r.step({0,.025,0});
        check36(r.hits()==before,"continued body pressure cannot generate another attack");
        Rig36 c(direct);for(unsigned i=0;i<3;++i)c.step({0,.025,0});
        c.in.attacks=false;c.step();c.in.attacks=true;c.commands.clear();
        for(unsigned i=0;i<2;++i)c.step({0,.025,0});
        check36(!c.count(ph::CommandKind::Prepare),"attack context loss retires prior shadow travel");
    }
    {Rig36 short42(false);short42.in.tick+=30;short42.in.timeNs+=30000000;short42.blocked=short42.actor=true;short42.step({0,.100,0});
     check36(short42.hits()==0,"C42 short guard contact cannot bypass hybrid deliberate-cut proof");}
    for(bool direct:{false,true}){
        Rig36 r(direct);r.in.tick+=70;r.in.timeNs+=70000000;r.blocked=r.actor=true;r.step({0,.220,0});
        check36(r.count(ph::CommandKind::Prepare)==1&&r.count(ph::CommandKind::Slash)==1,
            "C42 deliberate22cm80ms contact qualifies after checked swept fraction without replay");
        bool origin=false;for(const auto& c:r.commands)if(c.kind==ph::CommandKind::Prepare)
            origin=std::abs(c.origin167.x+.20)<1e-6&&std::abs(c.origin167.y-.35)<1e-6;
        check36(origin,"contact-first handoff retains physical pre-increment origin");
    }
    for(bool direct:{false,true})for(unsigned zone=0;zone<4;++zone){
        Rig36 r(direct);const ph::Vec3 starts[]{{-.2,.35,1.5},{-.2,.35,1.4},{.10,.35,1.4},{-.10,.35,1.4}};
        const pm::Quat turns[]{{0,-.7071067811865475,0,.7071067811865475},{0,.7071067811865475,0,.7071067811865475},{0,0,1,0},{0,0,0,1}};
        r.in.hands[0]={pm::multiply(r.model,turns[zone]),pm::rotate(r.model,starts[zone])};
        r.in.intentHands[0]={pm::multiply(r.tracking,turns[zone]),pm::rotate(r.tracking,starts[zone])};
        for(unsigned i=0;i<30;++i)r.step();r.commands.clear();
        check36(r.out.guard117,"real same-weapon wrist rotation and upper/lower/left/right hand position settles guard");
        for(unsigned i=0;i<15;++i)r.step({0,.025,0});
        check36(r.count(ph::CommandKind::Prepare)==1&&r.count(ph::CommandKind::Slash)==1,
            "same asset from four wrist rotations/starting positions emits one forward cut");
        const auto chest=(r.in.shoulders[0]+r.in.shoulders[1])*.5-ph::body_axes(r.in).up*.18;
        const auto expected=ph::body_vector117(r.in,pm::rotate(r.model,starts[zone])-chest);
        bool origin=false;for(const auto& c:r.commands)if(c.kind==ph::CommandKind::Prepare)origin=pm::length(c.origin167-expected)<1e-6;
        check36(origin,"native origin records the actual start before shadow travel, not the handoff position");
    }
    std::printf("guard_cut36=%u/%u native_calls=NONE forced_outcomes=NONE\n",checks36-failures36,checks36);return failures36?1:0;
}
