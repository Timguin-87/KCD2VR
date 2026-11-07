// R59.16C10R16 (Claude): F4 press 1 VR INTENT producer rules. The native attack is announced once, at the VR commit
// (prepare + release in the same producer frame; the native consumer defers the release by one native frame), an
// unfinished candidate is never announced, and the rhythm gate does not drop strokes. Same replay rig as r44_intent_test.
#include "vrik/weapon_sweep112.h"
#include <cstdio>
#include <vector>
using namespace kcd2vr::physical;
static int failures=0;
static void ck(bool value,const char* label){std::printf("%s %s\n",value?"PASS":"FAIL",label);failures+=!value;}
struct Replay16 {
    Input in{};State state{};std::array<Profile,2> profiles{};Output out{};std::vector<Command> commands;
    explicit Replay16(bool intent){
        in.epoch=in.bodyEpoch=in.sample=1;in.tick=1000;in.timeNs=1000000000;
        in.allowed=in.tracked[0]=in.tracked[1]=in.intentTracked[0]=true;in.grip[0]=1;
        in.contact=in.dynamics=in.physicalCombat177=in.constraintDrive129=true;
        in.naturalCombat117=in.coherentMelee119=in.heldMotionAttacks=in.requirePrimaryGrip114=in.controllerAnchor119=true;
        in.attackAtPrepare178r42=in.contactStartsAttack178r41=true;in.weaponAuthorityScale178r44=.6;
        in.nativeStateKnown178=true;in.strikeMinScale178=.35;in.strikeMinIntervalMs178=550;in.intent59c16=intent;
        in.shoulders[0]={0,0,1.3};in.shoulders[1]={-.2,0,1.3};in.reach[0]=1.5;
        in.hands[0].p={-.25,.2,1.2};in.intentHands[0]=in.hands[0];
        auto& p=profiles[0];p.key=p.generation=1;p.classId=4;p.primary=p.geometry=true;
        p.weight=3.4;p.axis={0,1,0};p.tip={0,.6,0};sample();
        for(int n=0;n<15;++n)move({});commands.clear();
    }
    void sample(){
        out=step(state,in,profiles,[&](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=true;c.fraction=1;return c;});
        for(unsigned i=0;i<out.commandCount;++i)commands.push_back(out.commands[i]);
    }
    void move(Vec3 delta){in.tick+=10;in.timeNs=in.tick*1000000;++in.sample;
        in.hands[0].p=in.hands[0].p+delta;in.intentHands[0].p=in.intentHands[0].p+delta;sample();}
    int count(CommandKind k)const {int n=0;for(const auto& c:commands)n+=c.kind==k;return n;}
    int hits()const {return count(CommandKind::Slash)+count(CommandKind::Thrust)+count(CommandKind::Pommel);}
    std::uint64_t first(CommandKind k)const {for(const auto& c:commands)if(c.kind==k)return c.sample;return 0;}
    std::uint64_t firstHit()const {for(const auto& c:commands)if(c.kind==CommandKind::Slash||c.kind==CommandKind::Thrust||c.kind==CommandKind::Pommel)return c.sample;return 0;}
};
int main(){
    for(bool stab:{true,false}){
        Replay16 r(true);r.in.strikeMinIntervalMs178=0;
        for(int n=0;n<12;++n)r.move(stab?Vec3{0,.03,0}:Vec3{.12,0,0});
        ck(r.hits()==1&&r.count(CommandKind::Prepare)==1,stab?"intent thrust: one prepare, one release":"intent cut: one prepare, one release");
        ck(r.first(CommandKind::Prepare)==r.firstHit(),stab?"intent thrust: prepare is announced in the commit frame":"intent cut: prepare is announced in the commit frame");
        std::uint64_t g=0;bool same=true;for(const auto& c:r.commands){if(c.kind==CommandKind::Prepare)g=c.gesture59c3;else if(c.kind!=CommandKind::Abort)same=same&&g&&c.gesture59c3==g;}
        ck(same&&r.count(CommandKind::Abort)==0,"intent prepare and release share one gesture and nothing is cancelled");
        int zp=-9,zr=-8;for(const auto& c:r.commands){if(c.kind==CommandKind::Prepare)zp=c.zone117;else if(c.kind!=CommandKind::Abort)zr=c.zone117;}
        ck(zp==zr,"intent prepare and release announce the same zone");
    }
    {Replay16 r(false);r.in.strikeMinIntervalMs178=0;for(int n=0;n<12;++n)r.move({0,.03,0});
        ck(r.hits()==1&&r.first(CommandKind::Prepare)&&r.first(CommandKind::Prepare)<r.firstHit(),"comparison (SYNCHRONIZED): prepare precedes the commit frame");}
    {Replay16 r(true);for(int n=0;n<3;++n)r.move({0,.02,0});for(int n=0;n<20;++n)r.move({});
        ck(r.count(CommandKind::Prepare)==0&&r.hits()==0&&r.count(CommandKind::Abort)==0,"intent: an unfinished 6 cm move announces nothing and cancels nothing");}
    {Replay16 r(true);r.in.nativeAttackBusy178=true;for(int n=0;n<12;++n)r.move({0,.03,0});
        ck(r.hits()==1,"intent: a busy native action does not drop the stroke (no rhythm gate)");}
    {Replay16 r(true);r.in.strikeMinIntervalMs178=3000;for(int n=0;n<12;++n)r.move({0,.03,0});
        for(int n=0;n<30;++n)r.move({0,-.004,0});for(int n=0;n<30;++n)r.move({});
        for(int n=0;n<12;++n)r.move({0,.03,0});
        ck(r.hits()==2,"intent: the minimum-interval slider does not drop a second real stroke");}
    {Replay16 r(false);r.in.nativeAttackBusy178=true;for(int n=0;n<10;++n)r.move({0,.03,0});
        ck(r.hits()==0,"comparison: the other modes keep the R44 busy-native admission");}
    std::printf("R16 intent failures=%d\n",failures);return failures?1:0;
}
