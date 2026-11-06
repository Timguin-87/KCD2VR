#include "vrik/weapon_sweep112.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace kcd2vr::physical;
static unsigned checks=0;
static void ck(bool ok,const char* label){++checks;if(!ok){std::fprintf(stderr,"FAIL %s\n",label);std::exit(1);}}
struct Replay {
    Input in{};State state{};std::array<Profile,2> profiles{};Output out{};double time=1;bool known=true,blocked=false;
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
    void sample(){out=step(state,in,profiles,[&](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=known;c.fraction=blocked?0:1;return c;});
        for(unsigned i=0;i<out.commandCount;++i)commands.push_back(out.commands[i]);}
    void move(Vec3 delta,double dt=.01,bool model=true,bool relative=true){time+=dt;in.timeNs=std::uint64_t(std::llround(time*1e9));in.tick=in.timeNs/1000000;++in.sample;
        if(model)in.hands[0].p=in.hands[0].p+delta;if(relative)in.intentHands[0].p=in.intentHands[0].p+delta;sample();}
    void rest(double seconds){for(int n=0;n<int(std::ceil(seconds/.01));++n)move({});}
    int count(CommandKind kind)const{int n=0;for(auto c:commands)n+=c.kind==kind;return n;}
    int hits()const{return count(CommandKind::Slash)+count(CommandKind::Thrust)+count(CommandKind::Pommel);}
};
int main(){
    {Replay r;r.move({.025,0,0});for(int n=0;n<14;++n)r.move({0,.02,0});
     ck(r.count(CommandKind::Thrust)==1&&r.count(CommandKind::Slash)==0,"R39R1C1 small lateral onset cannot lock an otherwise deliberate stab into cut distance");}
    {Replay r;r.in.hands[0].p={-.65,.02,1.3};r.in.intentHands[0]=r.in.hands[0];r.state={};r.commands.clear();r.sample();r.rest(.12);
     for(int n=0;n<12;++n)r.move({0,.02,0});
     ck(r.count(CommandKind::Thrust)==1,"R39R1C1 an axial cross-body stab need not move radially away from the HMD");}

    {Replay r;const auto before=r.out.weapons[0];r.known=false;r.move({.025,0,0});
     ck(!r.out.contactKnown[0]&&m::length(r.out.weapons[0].p-before.p)<1e-9,"unknown trajectory cannot move past last checked pose");
     for(int n=0;n<23;++n)r.move({.025,0,0});
     ck(m::length(r.out.weapons[0].p-before.p)<1e-9,"repeated unknown frames cannot accumulate unchecked travel");
     ck(r.count(CommandKind::Slash)==1,"unknown presentation query does not masquerade as a real blocked native input");
     ck(r.out.impactSpeed[0]==0,"unknown path does not create impact haptics");}

    {Replay r;for(int i=0;i<6;++i)r.move({.025,0,0});r.rest(.1);ck(r.hits()==0,"fast 15 cm reposition cannot issue a cut");}
    {Replay r;for(int jab=0;jab<8;++jab){for(int n=0;n<3;++n)r.move({0,.02,0});for(int n=0;n<3;++n)r.move({0,-.02,0});}ck(r.hits()==0,"repeated short jabs cannot bank distance");}
    {Replay r;for(int n=0;n<60;++n)r.move({.01,0,0});r.rest(.1);ck(r.hits()==0,"slow 60 cm sweep cannot issue a cut");}
    {Replay r;for(int n=0;n<10;++n)r.move({.025,0,0});ck(r.hits()==0,"25 cm remains below cut commit (R45: 30 cm)");for(int n=0;n<13;++n)r.move({.025,0,0});
        ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Slash)==1,"one half-metre fast cut prepares and commits once");const auto n=r.commands.size();r.sample();ck(r.commands.size()==n,"duplicate eye cannot replay strike");}
    {Replay r;for(int n=0;n<7;++n)r.move({0,.02,0});r.rest(.1);ck(r.hits()==0,"14 cm poke cannot commit");}
    {Replay r;for(int n=0;n<11;++n)r.move({0,.02,0});ck(r.count(CommandKind::Thrust)==1,"deliberate 22 cm thrust commits once");}
    // Real jabs include a little wrist rotation: tip angular speed must not
    // reclassify coherent hand translation as a half-metre cut.
    {Replay r;for(int n=0;n<12;++n){
        r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,n%2?.05:-.05});
        r.move({0,.02,0});}
        ck(r.count(CommandKind::Thrust)==1&&r.count(CommandKind::Slash)==0,"V125 deliberate jab tolerates small blade-angle wobble");}
    {Replay r;for(int n=0;n<30;++n){
        r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,n%2?.05:-.05});
        r.move({0,.001,0});}
        ck(r.hits()==0,"V125 blade-angle wobble without deliberate hand travel cannot attack");}
    for(bool model:{false,true}){Replay r;for(int n=0;n<24;++n)r.move({.025,0,0},.01,model,!model);ck(r.hits()==0,"motion in only one reference frame cannot attack");}
    {Replay r;for(int n=0;n<9;++n)r.move({.025,0,0});r.known=false;r.move({.025,0,0});ck(r.count(CommandKind::Abort)==0,"V136 unknown world-contact query no longer cancels a prepared swing");}
    {Replay r;for(int n=0;n<9;++n)r.move({.025,0,0});r.in.grip[0]=0;r.move({.025,0,0});ck(r.count(CommandKind::Abort)==1&&r.hits()==0,"grip release cancels prepared swing (R45: before the 30 cm commit)");}
    {Replay r;r.blocked=true;for(int n=0;n<24;++n)r.move({.025,0,0});ck(r.hits()==0,"wall pressure cannot become repeated strikes");}
    {Replay r;for(int n=0;n<24;++n)r.move({.025,0,0});r.rest(.25);for(int n=0;n<24;++n)r.move({-.025,0,0});ck(r.count(CommandKind::Slash)==2,"separate opposite strokes rearm");}
    double earliest=1,latest=0;
    for(int hz:{45,50,60,72,90,120,144}){Replay r;double first=0,start=r.time;
        for(int n=0;n<int(.26*hz);++n){r.move({2.4/hz,0,0},1.0/hz);if(!first&&r.hits())first=r.time-start;}
        ck(r.count(CommandKind::Slash)==1,"deliberate cut works at every tested refresh rate");earliest=std::min(earliest,first);latest=std::max(latest,first);
    }ck(earliest>=.3/2.4-1e-9&&latest-earliest<.024,"distance qualification differs by at most one low-rate sample (R45: 30 cm)");
    for(int hz:{50,90}){Replay r;r.in.hands[0].p={0,.2,1.2};r.in.intentHands[0]=r.in.hands[0];r.state={};r.sample();r.rest(.12);
        for(int n=1;n<=int(.22*hz);++n){const double a=-4.5*n/hz;const auto q=m::from_rotation_vector({0,0,a});
            const auto pos=Vec3{.2*std::sin(-a),.2*std::cos(a),1.2};const auto delta=pos-r.in.hands[0].p;
            r.in.hands[0].q=r.in.intentHands[0].q=q;r.move(delta,1.0/hz);}
        ck(r.count(CommandKind::Slash)==1,"coordinated hand and blade arc qualifies a cut at 50 and 90 Hz");}
    // A moving cut may cross guard space; a weapon already held there remains a guard.
    {Replay r;r.in.hands[0].p={0,-.35,1.4};r.in.hands[0].q=m::from_rotation_vector({0,0,-3.14159265358979323846/2});r.in.intentHands[0]=r.in.hands[0];r.state={};r.commands.clear();r.sample();r.rest(.12);
        for(int n=0;n<24;++n)r.move({0,.025,0});ck(r.count(CommandKind::Slash)==1,"cut begun outside guard survives crossing guard region");}
    {Replay r;r.in.hands[0].p={0,.35,1.4};r.in.hands[0].q=m::from_rotation_vector({0,0,-3.14159265358979323846/2});r.in.intentHands[0]=r.in.hands[0];r.state={};r.sample();r.rest(.12);
        ck(r.out.guard117,"stationary horizontal weapon in front of body is guard");ck(r.hits()==0,"holding guard does not attack");}
    {Replay r;r.in.hands[0].p={-.3,.35,1.9};r.in.hands[0].q=m::from_rotation_vector({0,0,-3.14159265358979323846/2});
        r.in.intentHands[0]=r.in.hands[0];r.state={};r.commands.clear();r.sample();r.rest(.12);
        for(int n=0;n<24;++n)r.move({0,0,-.025});
        ck(r.count(CommandKind::Slash)==1,"V121 overhead preparation survives descending through face guard");
        for(auto c:r.commands)if(c.kind==CommandKind::Slash)ck(c.zone117==0,"V121 overhead cut requests native upper zone");}
    // The hand anchors free-space motion even when the body/model frame turns.
    {Replay r;r.in.dynamics=true;r.in.haveWorld=true;r.in.attacks=false;for(int n=0;n<20;++n){r.in.world.q=m::from_rotation_vector({0,0,n*.02});r.in.world.p={n*.01,0,0};
        r.in.hands[0].q=m::from_rotation_vector({.01*n,0,0});r.move({.006,0,0});
        ck(m::length(r.out.hands[0].p-r.in.hands[0].p)<1e-8&&m::angle(r.out.weapons[0].q,r.in.hands[0].q)<1e-6,"free-space anchor adds no artificial position/orientation lag");}}
    {Profile p{};p.side=0;p.tip={0,.8,0};unsigned rays=0;
        const auto hit=sweep_weapon112(p,Pose{},Pose{{},{.05,0,0}},false,[&](Vec3 a,Vec3,double){
            ++rays;Contact103 c{};c.fraction=0;c.plane=true;c.point=a;c.normal={-1,0,0};return c;});
        ck(hit.known&&hit.plane&&hit.fraction==0&&rays==1,"V125 already proven zero-fraction stop needs no redundant feature rays");}
    // A verified contact still constrains the anchored weapon and produces one rebound.
    {Replay r;r.profiles[0].nativeSeed=true;r.profiles[0].nativeWeapon={{},{3,3,3}};
        const auto last=r.out.weapons[0];r.time+=.08;r.in.tick+=80;r.in.timeNs+=80000000;++r.in.sample;r.in.hands[0].p.x+=.02;
        Pose sweptFrom{};r.out=step(r.state,r.in,r.profiles,[&](const Profile&,Pose a,Pose,bool){sweptFrom=a;return Contact103{};});
        ck(m::length(sweptFrom.p-last.p)<1e-9,"timing reset begins sweep at last allowed pose rather than authored native animation");
        ck(r.out.commandCount==0,"timing reset cannot commit the interrupted gesture");
        ++r.profiles[0].generation;++r.in.sample;r.in.tick+=10;r.in.timeNs+=10000000;
        r.out=step(r.state,r.in,r.profiles,[&](const Profile&,Pose a,Pose,bool){sweptFrom=a;return Contact103{};});
        ck(m::length(sweptFrom.p-r.profiles[0].nativeWeapon.p)<1e-9,"new item generation still starts from its native seed");
    }
    {Replay r;r.in.dynamics=true;r.in.attacks=false;unsigned contacts=0;bool wall=false;
        auto sample=[&](){r.out=step(r.state,r.in,r.profiles,[&](const Profile&,Pose a,Pose b,bool){Contact103 c{};c.known=true;
            if(wall&&b.p.x>a.p.x+1e-7){c.fraction=0;c.plane=true;c.normal={-1,0,0};c.point=a.p;++contacts;}return c;});};
        wall=true;r.in.hands[0].p.x+=.04;++r.in.sample;r.in.tick+=10;r.in.timeNs+=10000000;sample();
        ck(r.out.blocked[0]&&r.out.hands[0].p.x<r.in.hands[0].p.x-.02,"controller anchor cannot bypass wall constraint");
        ck(contacts>0&&r.state.weapon[0].recoilVelocity.x<0,"verified inward contact creates outward recoil");
    }
    std::printf("melee119=%u/%u cutCommitMs=%.2f..%.2f retail_execution=NONE\n",checks,checks,earliest*1000,latest*1000);
}
