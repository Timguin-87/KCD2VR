#include "vrik/physical_weapon.h"
#include <cstdio>
#include <vector>
#include <limits>
using namespace kcd2vr::physical;
static unsigned checks=0,failures=0;
static void ck(bool ok,const char* name){++checks;if(!ok)++failures;std::printf("%s %s\n",ok?"PASS":"FAIL",name);}
static int count(const Output& o,CommandKind k){int n=0;for(unsigned j=0;j<o.commandCount;++j)n+=o.commands[j].kind==k;return n;}
struct BowReplay {
    Input in{};std::array<Profile,2> profiles{};State state{};Output out{};
    int begins=0,fires=0,cancels=0;
    Vec3 nock{0,.35,1.3};bool geometry=true,validRight=true,validLeft=true;
    BowReplay(){in.epoch=in.bodyEpoch=1;in.allowed=true;in.bowRetention=true;
        in.grip[1]=1;in.requirePrimaryGrip114=true;in.reach[0]=in.reach[1]=1;
        in.shoulders[0]={.2,0,1.3};in.shoulders[1]={-.2,0,1.3};
        in.hands[1].p={0,.4,1.3};in.hands[0].p=nock;
        auto& p=profiles[1];p.key=p.generation=7;p.side=1;p.classId=9;p.primary=p.support=true;
        p.grip[0].p={0,-.05,0};p.axis={0,-1,0};
    }
    void next(){++in.sample;in.tick+=10;in.timeNs=in.tick*1000000;
        profiles[1].support=geometry;in.bowActions=true;
        // Actual coherent_pose order: selector before output tracking flags.
        in.tracked[0]=in.tracked[1]=false;
        select_bow_policy175(in,profiles[1],true,validRight,validLeft);
        in.tracked[0]=in.intentTracked[0]=validRight;in.tracked[1]=in.intentTracked[1]=validLeft;
        resolve_bow_mode146(in,profiles[1],&state);
        out=step(state,in,profiles,[](const Profile&,Pose,Pose,bool){return Contact103{};});
        begins+=count(out,CommandKind::BowBegin);fires+=count(out,CommandKind::BowFire);cancels+=count(out,CommandKind::BowCancel);
    }
    void pull(){for(int j=1;j<=16;++j){in.hands[0].p=nock+Vec3{.012*j,-.012*j,.003*j};next();}}
    void shot(){in.hands[0].p=nock;in.trigger[0]=0;next();in.trigger[0]=1;next();pull();in.trigger[0]=0;next();}
};
struct MeleeReplay {
    Input in{};std::array<Profile,2> profiles{};State state{};Output out{};
    bool blocked=false,actor=false,unknown=false,opponent=true,owner=true,part=true;Vec3 normal{-1,0,0};std::vector<Command> commands;
    MeleeReplay(){in.epoch=in.bodyEpoch=1;in.tick=1000;in.timeNs=1000000000;
        in.allowed=in.tracked[0]=in.tracked[1]=in.intentTracked[0]=true;
        in.grip[0]=1;in.requirePrimaryGrip114=in.heldMotionAttacks=in.coherentMelee119=in.controllerAnchor119=true;
        in.dynamics=false;in.reach[0]=1.3;in.shoulders[0]={0,0,1.3};in.shoulders[1]={-.2,0,1.3};
        in.hands[0].p={-.25,.2,1.2};in.intentHands[0]=in.hands[0];
        auto& p=profiles[0];p.key=p.generation=1;p.classId=4;p.primary=p.geometry=true;p.axis={0,1,0};p.tip={0,.6,0};
        for(int j=0;j<13;++j)move({});}
    void move(Vec3 delta,bool relative=true){++in.sample;in.tick+=10;in.timeNs+=10000000;
        in.hands[0].p=in.hands[0].p+delta;if(relative)in.intentHands[0].p=in.intentHands[0].p+delta;
        out=step(state,in,profiles,[&](const Profile&,Pose from,Pose,bool){Contact103 c{};c.known=!unknown;
            if(blocked){c.fraction=0;c.plane=true;c.normal=normal;c.point=from.p;
                if(actor){c.currentOpponent175=opponent;c.native172.mask=kcd2vr::npc_contact172::BodiesAndWorld;
                    c.native172.ownerRead=owner;c.native172.owner=owner?23:0;c.native172.collider=24;c.native172.entityId=42;
                    c.native172.part=part?2:-1;c.native172.body=kcd2vr::npc_contact172::Body::Articulated;}}
            return c;});
        for(unsigned j=0;j<out.commandCount;++j)commands.push_back(out.commands[j]);}
    int count(CommandKind kind)const{int n=0;for(auto c:commands)n+=c.kind==kind;return n;}
};
int main(){
    for(bool slide:{false,true})for(bool backout:{false,true})for(bool secondKnown:{false,true}){
        MeleeReplay r;r.in.wallSlide58=slide;r.in.attacks=false;
        const auto start=r.out.weapons[0];r.state.weapon[0].wallStuckSince58=r.in.tick-100;
        r.in.hands[0].p=start.p+Vec3{.06,0,.03};
        ++r.in.sample;r.in.tick+=10;r.in.timeNs+=10000000;unsigned queries=0;
        auto query=[&](const Profile&,Pose from,Pose,bool){
            Contact103 c{};++queries;
            if(queries==1){c.fraction=0;c.plane=true;c.normal=backout?Vec3{1,0,0}:Vec3{-1,0,0};c.point=from.p;}
            else if(!secondKnown){c.known=false;c.fraction=0;}
            return c;
        };
        r.out=step(r.state,r.in,r.profiles,query);
        ck(queries==unsigned(backout||slide?2:1),"optional slide changes only tangent follow-up, never checked outward recovery");
        const auto delta=r.out.weapons[0].p-start.p;
        if(secondKnown&&(slide||backout))
            ck(backout?delta.x>.059&&std::abs(delta.z)<1e-8:delta.z>.029&&std::abs(delta.x)<1e-8,
               "wall recovery moves solely along the independently checked tangent or outward path");
        else ck(m::length(delta)<1e-8,"slide disabled or recovery UNKNOWN never advances an unchecked path");
        ck(r.out.commandCount==0&&r.out.impactSpeed[0]==0,"checked escape/slide never fabricates an attack or inward impact");
    }
    // C58R1: exercise the real weapon kernel with the runtime friction values,
    // not only the zero-friction geometry contract. Every tangent still gets
    // its own second sweep; an unknown/blocked corner never becomes permission.
    ck(Input{}.wallFriction57==0,"C58R1 pure Input default preserves zero-friction geometry");
    for(double friction:{0.,.35,.95,-1.,2.,std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::infinity()}){
        const double bounded=std::isfinite(friction)?std::clamp(friction,0.,.95):0.;
        for(int route=0;route<10;++route){
            MeleeReplay r;r.in.wallSlide58=route!=8;r.in.wallFriction57=friction;r.in.attacks=false;
            const auto start=r.out.weapons[0];r.state.weapon[0].wallStuckSince58=r.in.tick-100;
            r.in.hands[0].p=start.p+Vec3{.06,0,.03};
            ++r.in.sample;r.in.tick+=10;r.in.timeNs+=10000000;
            unsigned queries=0;bool finiteQueries=true;const bool backout=route>=5&&route<=7;
            auto query=[&](const Profile&,Pose from,Pose to,bool){
                ++queries;finiteQueries=finiteQueries&&finite(from)&&finite(to);Contact103 c{};
                if(queries==1){c.fraction=0;c.plane=true;c.normal=backout?Vec3{1,0,0}:Vec3{-1,0,0};c.point=from.p;
                    if(route==9)c.known=false;}
                else if(route==1||route==6){c.known=false;c.fraction=0;}
                else if(route==2||route==7){c.fraction=0;c.plane=true;c.normal={0,0,-1};c.point=from.p;}
                else if(route==3||route==4){c.fraction=.4;c.plane=route==3;c.normal={0,0,-1};c.point=from.p;c.limited115=route==4;}
                return c;
            };
            r.out=step(r.state,r.in,r.profiles,query);const auto delta=r.out.weapons[0].p-start.p;
            const bool noMove=route==1||route==2||route==6||route==7||route==8||route==9;
            const double expected=noMove?0.:backout?.06:.03*(1.-bounded)*(route==3||route==4?.4:1.);
            ck(finiteQueries&&finite(r.out.weapons[0]),"C58R1 invalid friction never reaches a query or output as NaN/Inf");
            ck(backout?std::abs(delta.x-expected)<1e-9&&std::abs(delta.z)<1e-9:
                       std::abs(delta.z-expected)<1e-9&&std::abs(delta.x)<1e-9,
               "C58R1 .35/.95 attenuation preserves hard stop, exact checked corner prefix and full outward backout");
            ck(queries==unsigned(route==8||route==9?1:2),"C58R1 friction adds no queries and cannot turn slide on or bypass unknown first contact");
            const bool scraped=!backout&&!noMove&&route!=4&&expected>.0005;
            ck(r.out.wallScraped58[0]==scraped,"C58R1 scrape evidence requires actual checked non-backout tangent movement");
            ck(r.out.commandCount==0&&r.out.impactSpeed[0]==0,"C58R1 friction and scrape evidence invent neither attacks nor impact energy");
            const auto repeat=step(r.state,r.in,r.profiles,query);
            ck(!repeat.wallScraped58[0]&&queries==unsigned(route==8||route==9?1:2),"C58R1 repeated sample cannot re-emit scrape evidence or query again");
        }
        // Two actual planes form a corner. Repeated pressure against both
        // cannot tunnel, accumulate a skin offset or release the checked seed.
        MeleeReplay r;r.in.wallSlide58=true;r.in.wallFriction57=friction;r.in.attacks=false;
        const auto start=r.out.weapons[0];const double stopZ=start.p.z+.045;bool safe=true;double worstStep=0;
        for(int sample=1;sample<=120;++sample){
            const auto before=r.out.weapons[0].p;r.in.hands[0].p=start.p+Vec3{.06,0,.001*sample};
            ++r.in.sample;r.in.tick+=10;r.in.timeNs+=10000000;unsigned queries=0;
            r.out=step(r.state,r.in,r.profiles,[&](const Profile&,Pose from,Pose to,bool){
                ++queries;Contact103 c{};
                if(queries==1){c.fraction=0;c.plane=true;c.normal={-1,0,0};c.point=from.p;}
                else if(to.p.z>stopZ&&to.p.z>from.p.z){c.fraction=std::clamp((stopZ-from.p.z)/(to.p.z-from.p.z),0.,1.);
                    c.plane=true;c.normal={0,0,-1};c.point={from.p.x,from.p.y,stopZ};}
                return c;
            });
            safe=safe&&finite(r.out.weapons[0])&&std::abs(r.out.weapons[0].p.x-start.p.x)<1e-9&&
                r.out.weapons[0].p.z<=stopZ+1e-9&&queries<=2&&r.out.commandCount==0;
            worstStep=std::max(worstStep,m::length(r.out.weapons[0].p-before));
        }
        ck(safe&&worstStep<.021,"C58R1 120-sample two-wall corner remains checked and bounded under runtime friction");
    }
    // Retained physical pose behind Henry after root movement. Direct rotation
    // clips a surface; translating at the old orientation is a distinct path.
    for(int mode=0;mode<5;++mode){
        MeleeReplay r;r.in.contact=r.in.physicalCombat177=true;r.in.reach[0]=.55;
        r.in.hands[0].q=m::from_rotation_vector({0,0,.7});
        auto& retained=r.state.weapon[0];retained.weapon.p={-1.1,.2,1.2};
        retained.acquiring124=true;const auto seed=retained.weapon;
        ++r.in.sample;r.in.tick+=10;r.in.timeNs+=10000000;
        unsigned queries=0;
        auto query=[&](const Profile&,Pose from,Pose to,bool){
            ++queries;Contact103 c{};
            if(m::angle(from.q,to.q)>.01){c.fraction=0;c.plane=true;
                c.normal=mode==1?Vec3{0,1,0}:Vec3{1,0,0};c.point=from.p;}
            else if(mode==2){c.known=false;c.fraction=0;}
            else if(mode==3){c.fraction=0;c.plane=true;c.normal={-1,0,0};c.point=from.p;}
            else if(mode==4){c.fraction=.25;c.limited115=true;}
            return c;
        };
        r.out=step(r.state,r.in,r.profiles,query);
        ck(queries==2,"C10R2 acquisition makes at most one additional checked recovery sweep");
        if(mode<=1){
            ck(r.out.owned[0]&&!r.out.handlingOnly55c1[0]&&r.out.releaseReason121[0]==0&&
               m::length(r.out.hands[0].p-r.in.shoulders[0])<=r.in.reach[0]+.001,
               "C10R2 checked outward/tangent acquisition returns to reachable collision ownership");
            ck(m::angle(r.out.weapons[0].q,seed.q)<1e-8&&r.out.commandCount==0&&r.out.impactSpeed[0]==0,
               "C10R2 acquisition keeps the checked orientation and never invents a strike or impact");
            ++r.in.sample;r.in.tick+=10;r.in.timeNs+=10000000;queries=0;
            r.out=step(r.state,r.in,r.profiles,[](const Profile&,Pose,Pose,bool){return Contact103{};});
            ck(r.out.owned[0]&&!r.out.handlingOnly55c1[0],"C10R2 recovered weapon continues normally on the next fresh sample");
        }else if(mode==4){
            const auto expected=seed.p+(r.in.hands[0].p-seed.p)*.25;
            ck(m::length(r.state.weapon[0].weapon.p-expected)<1e-8&&r.out.handlingOnly55c1[0]&&
               r.out.sweepDeferred115[0]&&!r.out.contactKnown[0],
               "C10R2 partial recovery retains only checked prefix and stays handling-only while unreachable");
        }else{
            ck(m::length(r.state.weapon[0].weapon.p-seed.p)<1e-8&&r.out.handlingOnly55c1[0]&&
               !attack_authority55c1(r.in,r.out,0),
               "C10R2 UNKNOWN or opposing obstacle never promotes the unswept controller display pose");
        }
    }
    {MeleeReplay r;r.in.contact=true;r.blocked=true;r.normal={1,0,0};
        for(int j=0;j<12;++j)r.move({.005,0,0});const auto start=r.out.weapons[0];
        r.unknown=true;r.in.contactSuspended59c1=true;r.move({.005,0,0});
        ck(m::length(r.state.weapon[0].weapon.p-start.p)<1e-8&&!r.out.wallBackout58[0]&&r.out.handlingOnly55c1[0]&&!attack_authority55c1(r.in,r.out,0),
           "R59C1 unknown coverage preserves checked seed; display recovery never grants attack authority");}
    {MeleeReplay r;r.in.contact=true;r.blocked=true;r.normal={1,0,0};
        for(int j=0;j<12;++j)r.move({.005,0,0});const auto start=r.out.weapons[0];
        r.normal={-1,0,0};r.move({.005,0,0}); // a real second obstacle faces the outward motion
        ck(m::length(r.out.weapons[0].p-start.p)<1e-8&&!r.out.wallBackout58[0],
           "R58C1 outward wall recovery cannot bypass an opposing second obstacle");}
    {MeleeReplay r;r.in.contact=true;r.blocked=true;r.normal={1,0,0};
        for(int j=0;j<5;++j)r.move({.005,0,0});const auto start=r.out.weapons[0]; // 50 ms stuck: not yet
        ck(m::length(r.out.weapons[0].p-start.p)<1e-8,"R59 no back-out before 60 ms stuck");
        for(int j=0;j<3;++j)r.move({.005,0,0}); // the outward sweep is stopped at its start by the surface we are inside (same normal)
        const auto moved=r.out.weapons[0].p-start.p;
        ck(m::length(moved)<1e-9&&!r.out.wallBackout58[0],
           "R59C1 same-facing obstacle normal cannot authorize an unswept escape through another surface");}
    {MeleeReplay r;r.in.physicalCombat177=true;r.move({});
        r.in.kickRequest59=r.in.tick;r.move({});
        const auto afterRequest=r.commands.size();
        ck(afterRequest>=1&&r.commands.back().kind==CommandKind::Prepare&&r.commands.back().zone117==3&&r.commands.back().kick59&&r.commands.back().stroke117==CommandKind::Slash,
           "R59 R3 kick: the request becomes a prepare at zone 3 (companion kick row), marked kick59");
        r.move({});
        ck(r.commands.size()==afterRequest+1&&r.commands.back().kind==CommandKind::Slash&&r.commands.back().zone117==3&&r.commands.back().kick59,
           "R59 R3 kick: the release follows one frame later at the same zone");
        r.move({});r.move({});
        ck(r.commands.size()==afterRequest+1,"R59 R3 kick: one request, one prepare, one release, nothing repeated");
        r.in.kickRequest59=r.in.tick;r.move({});r.move({});
        ck(r.commands.size()==afterRequest+3,"R59 R3 kick: a new request tick is a new kick");}
    {MeleeReplay r;r.in.physicalCombat177=true;r.move({});
        r.in.nativeAttackBusy178=true;r.in.kickRequest59=r.in.tick;r.move({});r.move({});
        const auto busy=r.commands.size();
        ck(busy==0||r.commands.back().kind!=CommandKind::Prepare,"R59.16C2 R3 kick: a busy native attack defers the kick (no prepare yet)");
        r.in.nativeAttackBusy178=false;r.move({});
        ck(r.commands.size()>busy&&r.commands.back().kind==CommandKind::Prepare&&r.commands.back().kick59,
           "R59.16C2 R3 kick: the deferred request fires once the gate clears, instead of being silently consumed");}
    {MeleeReplay r;r.in.physicalCombat177=true;r.move({});
        r.in.nativeAttackBusy178=true;r.in.kickRequest59=r.in.tick;
        for(int j=0;j<80;++j)r.move({});
        r.in.nativeAttackBusy178=false;r.move({});r.move({});
        bool prepared=false;for(const auto& c:r.commands)prepared|=c.kick59;
        ck(!prepared&&r.out.kickDrop59c2==3&&r.out.kickDropTick59c2,"R59.16C2 R3 kick: an expired deferral reports RHYTHM and never fires late");}
    {MeleeReplay r;r.in.physicalCombat177=true;r.move({});
        r.in.kickRequest59=r.in.tick>2000?r.in.tick-2000:1;r.move({});
        bool prepared=false;for(const auto& c:r.commands)prepared|=c.kick59;
        ck(!prepared&&r.out.kickDrop59c2==5,"R59.16C2 R3 kick: a press older than 1 s (e.g. made during a clinch) is reported STALE, not replayed");}
    {MeleeReplay r;r.in.haveWorld=r.in.impaleEnabled178r15=true;r.in.worldScale=1;r.move({});
        auto& q=r.in.receipt58c1;q.serial=10;q.target=42;q.key=q.generation=q.epoch=q.request=1;
        q.attempt=1;q.targetIdentity=0x1234;q.collider=0x5678;q.rawPart=0;q.continuation=true;
        q.tick=r.in.tick+10;q.impactTick=r.in.tick;q.kind=1;q.seen=q.accepted=q.fields=q.applied=q.bodyContact=true;
        q.blocked=q.perfect=q.parry=0;q.death=0;q.damage=5;
        const auto tip=point(r.out.weapons[0],r.profiles[0].tip);q.point[0]=float(tip.x);q.point[1]=float(tip.y);q.point[2]=float(tip.z);
        r.move({});ck(!r.out.impaleLatchRequest,"R58C1 accepted nonlethal thrust cannot pin a living opponent");
        q.death=1;q.tick=r.in.tick+10;r.move({});
        ck(r.out.impaleLatchRequest&&r.out.impaleSerial==10,"R58C1 matching lethal native thrust receipt requests one display latch");
        r.in.impalePinValid178r15=true;r.in.impalePinSerial178r19=10;r.in.impalePinWorld178r15=tip;r.move({});
        ck(r.out.impaleActive&&r.out.displayLatched[0],"R58C1 fresh matching clear pin becomes presentation only");
        ck(m::length(r.out.weapons[0].p-r.state.weapon[0].weapon.p)<1e-8,"R58C1 display latch leaves retained physical weapon unchanged");
        r.in.impalePinWorld178r15=tip+Vec3{0,.4,0};r.move({});
        ck(r.out.impaleReleased&&!r.out.displayLatched[0],"R58C1 excess corpse displacement releases instead of dragging the hand");
        r.in.impalePinWorld178r15=tip;q.tick=r.in.tick+10;r.move({});
        ck(!r.out.impaleLatchRequest,"R58C1 released receipt cannot relatch");}
    {BowReplay r;r.next();ck(physical_bow175(r.in)&&r.in.bowGeometryReady175&&!r.in.nativeBowTrigger126,
        "real caller selects physical string before tracked output initialization");
     r.in.trigger[0]=1;for(int j=0;j<100;++j)r.next();r.in.trigger[0]=0;r.next();
     ck(r.begins==1&&r.fires==0&&r.cancels==1,"stationary Trigger hold/release never requests physical shot");}
    {BowReplay r;r.geometry=false;r.next();r.in.trigger[0]=1;for(int j=0;j<40;++j)r.next();
     ck(physical_bow175(r.in)&&!r.in.nativeBowTrigger126&&r.begins==0,"missing geometry stays physical not-ready without native RT fallback");
     r.geometry=true;for(int j=0;j<20;++j)r.next();ck(r.begins==0,"geometry arriving during held Trigger cannot latch");
     r.in.trigger[0]=0;r.next();r.in.trigger[0]=1;r.next();r.pull();r.in.trigger[0]=0;r.next();
     ck(r.fires==1,"geometry recovery plus neutral Trigger permits intentional physical shot");}
    for(bool gripHeld:{false,true}){BowReplay r;r.in.trigger[0]=1;r.in.grip[0]=gripHeld?1:0;
        for(int j=0;j<20;++j)r.next();r.in.grip[0]=0;for(int j=0;j<20;++j)r.next();
        ck(r.begins==0&&r.fires==0,"held Trigger on entry and Grip release cannot rearm string");}
    {BowReplay r;r.shot();r.shot();ck(r.begins==2&&r.fires==2&&r.cancels==0,"two automatic-load physical distance pulls request one shot per deliberate release");
     const auto sample=r.in.sample,tick=r.in.tick,stamp=r.in.timeNs;const auto old=r.out;
     auto duplicate=step(r.state,r.in,r.profiles,[](const Profile&,Pose,Pose,bool){return Contact103{};});
     ck(duplicate.commandCount==0&&r.state.sample==sample&&r.in.tick==tick&&r.in.timeNs==stamp,"duplicate-eye consumer cannot replay shot or refresh original sample timestamp");}
    {BowReplay r;r.next();r.in.trigger[0]=1;r.next();r.pull();r.geometry=false;r.in.trigger[0]=0;r.next();
     ck(r.fires==0&&r.cancels==1,"geometry loss at release cancels an already drawn bow");}
    {BowReplay r;r.next();r.in.trigger[0]=1;r.next();r.pull();r.validRight=false;r.in.trigger[0]=0;r.next();
     ck(r.fires==0&&r.cancels==1,"tracking loss at release cancels rather than firing");}
    {BowReplay r;r.next();r.in.trigger[0]=1;r.next();r.pull();r.in.grip[1]=0;r.in.trigger[0]=0;r.next();
     ck(r.fires==0&&r.cancels==1,"primary bow Grip loss cancels and cannot become release-to-fire");}
    {BowReplay r;r.next();ck(bow_uses_trigger175(r.in)&&bow_requires_charge_ack175(r.in),"automatic physical string owns Trigger and requires native charge acknowledgement");
     Input native{};select_bow_policy175(native,r.profiles[1],false,true,true);ck(!physical_bow175(native)&&native.nativeBowTrigger126&&!native.experiments169,"explicit native mode retains native draw policy");
     Profile crossbow=r.profiles[1];crossbow.classId=10;select_bow_policy175(native,crossbow,true,true,true);ck(!physical_bow175(native)&&!native.bowStringTrigger160,"crossbow is not silently assigned physical bow string");}
    {MeleeReplay r;for(int j=0;j<24;++j)r.move({.025,0,0});ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Slash)==1,"clear deliberate cut retains original prepare and commit");}
    {MeleeReplay r;for(int j=0;j<12;++j)r.move({.025,0,0});const auto stop=r.out.weapons[0];r.blocked=r.actor=true;r.move({.025,0,0});
     ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Slash)==1&&r.count(CommandKind::Abort)==0,"first qualified actor impact admits already prepared pre-contact stroke");
     ck(r.out.blocked[0]&&m::length(r.out.weapons[0].p-stop.p)<1e-9,"contact admission leaves visible blade at its same collision stop");
     for(int j=0;j<100;++j)r.move({j<10?.002:0,0,0});ck(r.count(CommandKind::Slash)==1,"actor resting pressure never repeats the committed stroke");}
    {MeleeReplay r;for(int j=0;j<8;++j)r.move({.025,0,0});r.blocked=true;r.move({.025,0,0}); // R45: 20 cm (commit is now 30 cm)
     ck(r.count(CommandKind::Slash)==1&&r.count(CommandKind::Abort)==0,"R54: a prepared stroke stopped by the world still releases the native attack (it hits nothing)");}
    {MeleeReplay r;r.blocked=r.actor=true;for(int j=0;j<30;++j)r.move({.025,0,0});ck(r.count(CommandKind::Prepare)==0&&r.count(CommandKind::Slash)==0,"actor pressure without a prior free stroke cannot start an attack");}
    {MeleeReplay r;for(int j=0;j<5;++j)r.move({.025,0,0});r.blocked=r.actor=true;r.move({.005,0,0}); // R45: a prepared stroke that only touches (0.5 m/s) is cancelled; 15 cm at 2.5 m/s is a close strike
     ck(r.count(CommandKind::Slash)==1&&r.count(CommandKind::Abort)==0,"R54: a prepared stroke that reaches the actor commits even when the last sample is slow (energy is judged by the producer)");} // was: insufficient pre-contact intent is canceled instead of banking post-contact pressure");}
    {MeleeReplay r;for(int j=0;j<12;++j)r.move({.025,0,0},false);r.blocked=r.actor=true;r.move({.025,0,0},false);
     ck(r.count(CommandKind::Slash)==0,"locomotion alone cannot qualify actor-contact intent");}
    {Contact103 c{};c.fraction=0;c.plane=true;c.native172.mask=kcd2vr::npc_contact172::BodiesAndWorld;
     c.native172.body=kcd2vr::npc_contact172::Body::Living;c.native172.entityId=1;
     ck(!actor_contact175(c),"body type without live owner/collider/part proof cannot qualify actor intent");}
    {MeleeReplay r;r.in.physicalCombat177=r.in.dynamics=r.in.constraintDrive129=true;r.opponent=false;
     for(int j=0;j<12;++j)r.move({.025,0,0});const auto stop=r.out.weapons[0];r.blocked=r.actor=true;r.move({.025,0,0});
     ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Slash)==1&&r.count(CommandKind::Abort)==0,
        "F4 first fully qualified NPC contact requests prepared cut even before native opponent selection");
     ck(r.out.blocked[0]&&m::length(r.out.weapons[0].p-stop.p)<1e-9,"non-opponent admission does not move the stopped blade");
     for(int j=0;j<100;++j)r.move({.004,0,0});ck(r.count(CommandKind::Slash)==1,"non-opponent resting pressure cannot buy another hit");}
    {MeleeReplay r;r.in.physicalCombat177=r.in.dynamics=r.in.constraintDrive129=true;r.opponent=false;r.normal={0,-1,0};
     for(int j=0;j<7;++j)r.move({0,.02,0});r.blocked=r.actor=true;r.move({0,.02,0});
     ck(r.count(CommandKind::Thrust)==1&&r.count(CommandKind::Slash)==0&&r.count(CommandKind::Abort)==0,
        "F4 axial approach can request a stab on first NPC contact before the free-space threshold");
     ck(r.out.strike178.accepted&&r.out.strike178.scale>=r.in.strikeMinScale178,
        "contact stab retains the ordinary useful-energy gate and pre-impact velocity");}
    for(int missing=0;missing<3;++missing){MeleeReplay r;r.in.physicalCombat177=r.in.dynamics=r.in.constraintDrive129=true;r.opponent=false;
     r.owner=missing!=0;r.part=missing!=1;
     for(int j=0;j<8;++j)r.move({.025,0,0});r.blocked=true;r.actor=missing!=2;r.move({.025,0,0}); // R45: below the 30 cm commit
     ck(r.count(CommandKind::Slash)==1&&r.count(CommandKind::Abort)==0,"R54: a prepared stroke commits on any obstruction; owner/part proofs belong to the hit chain");}
    {MeleeReplay r;r.in.physicalCombat177=r.in.dynamics=r.in.constraintDrive129=true;r.opponent=false;r.blocked=r.actor=true;
     for(int j=0;j<60;++j)r.move({.025,0,0});ck(r.count(CommandKind::Prepare)==0&&r.count(CommandKind::Slash)==0,
        "non-opponent pressure without free approach cannot start an F4 attack");}
    {MeleeReplay r;r.opponent=false;for(int j=0;j<8;++j)r.move({.025,0,0});r.blocked=r.actor=true;r.move({.025,0,0}); // R45: below the 30 cm commit
     ck(r.count(CommandKind::Slash)==1&&r.count(CommandKind::Abort)==0,"R54: the collision never cancels a prepared stroke (non-F4 path too)");} // was: non-F4 path preserves existing current-opponent contact requirement
    {kcd2vr::windup57c1::Arm arm;std::uint64_t tick=100;
     auto feed=[&](Vec3 a,Vec3 b,bool eligible=true){tick+=10;return arm.update({tick,.01,{.25,0,.25},a,b,eligible});};
     int prepares=0;for(int j=0;j<200;++j)prepares+=feed({},{});
     ck(prepares==0,"R57C1 static shoulder pose never invents a wind-up");
     for(int j=0;j<14;++j)prepares+=feed({0,-.01,0},{0,-.01,0});
     for(int j=0;j<18;++j)prepares+=feed({},{});
     ck(prepares==1,"R57C1 deliberate 14cm backward movement then 150ms settle arms exactly once");
     for(int j=0;j<400;++j)prepares+=feed({},{});
     ck(prepares==1,"R57C1 timeout/continued resting cannot auto-rearm without another wind-up");
     for(int variant=0;variant<4;++variant){arm.reset();int n=0;
      for(int j=0;j<20;++j)n+=feed(variant==0?Vec3{}:Vec3{0,-.01,0},variant==1?Vec3{}:Vec3{0,-.01,0},variant!=2);
      if(variant==3){tick+=100;feed({},{});}
      for(int j=0;j<20;++j)n+=feed({},{});
      ck(n==0,"R57C1 head-only/model-only/ineligible/stale winding cannot prepare");}
     arm.reset();int shaken=0;
     for(int j=0;j<25;++j){shaken+=feed({0,-.01,0},{0,-.01,0});shaken+=feed({0,.01,0},{0,.01,0});}
     for(int j=0;j<20;++j)shaken+=feed({},{});
     ck(shaken==0,"R57C1 repeated one-centimeter reversals cannot accumulate a twelve-centimeter wind-up");}
    {MeleeReplay r;r.in.windupHold57=r.in.thrustAsSwing55=r.in.physicalCombat177=true;
     r.in.strikeMinScale178=0;r.in.hands[0].p={.3,.10,1.25};r.in.intentHands[0]=r.in.hands[0];
     for(int j=0;j<20;++j)r.move({});r.commands.clear();
     for(int j=0;j<16;++j)r.move({0,-.01,0});for(int j=0;j<20;++j)r.move({});
     ck(r.count(CommandKind::Prepare)==1&&r.state.weapon[0].cocked57.active,"R57C1 production physical solver advertises one deliberately held preparation");
     for(int j=0;j<100;++j)r.move({});
     ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Abort)==0,"R57C1 held wind-up survives a one-second native charge interval without repeated input");
     for(int j=0;j<60;++j)r.move({-.01,0,0});for(int j=0;j<34;++j)r.move({});
     ck(r.count(CommandKind::Zone57)==1&&r.count(CommandKind::Slash)==0&&r.count(CommandKind::Abort)==0,
        "R57C1 rear cross-body feint advertises a new held zone without an accidental slash or cancellation");
     for(int j=0;j<20;++j)r.move({0,.025,0});
     bool heldThrust=false;for(const auto& c:r.commands)heldThrust|=c.kind==CommandKind::Thrust&&c.held57;
     ck(heldThrust&&r.count(CommandKind::Prepare)==1,"R57C1 actual thrust keeps held-prepare provenance without a second RT-down");}
    std::printf("physical_contract175=%u/%u native_execution=NONE\n",checks-failures,checks);return failures?1:0;
}
