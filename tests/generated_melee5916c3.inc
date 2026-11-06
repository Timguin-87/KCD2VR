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
