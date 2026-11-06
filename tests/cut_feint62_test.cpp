#include "vrik/physical_weapon.h"
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <vector>
using namespace kcd2vr::physical;
static unsigned checks=0;
static void ck(bool ok,const char* label){++checks;if(!ok){std::fprintf(stderr,"FAIL %s\n",label);std::exit(1);}}
struct Replay {
    Input in{};State state{};std::array<Profile,2> profiles{};Output out{};double time=1;bool known=true,blocked=false;
    std::vector<Command> commands;
    Replay(){in.epoch=in.bodyEpoch=in.sample=1;in.tick=1000;in.timeNs=1000000000;
        in.allowed=in.tracked[0]=in.tracked[1]=true;in.grip[0]=1;
        in.dynamics=false;in.contact=true;in.naturalCombat117=in.heldMotionAttacks=in.requirePrimaryGrip114=true;
        in.shoulders[0]={.2,0,1.3};in.shoulders[1]={-.2,0,1.3};in.reach[0]=1.0;
        in.hands[0].p={.05,.2,1.2};in.intentHands[0]=in.hands[0];in.intentTracked[0]=true;
        auto& p=profiles[0];p.key=p.generation=1;p.classId=4;p.primary=p.geometry=true;p.axis={0,1,0};p.tip={0,.6,0};
        sample();rest(.12);
    }
    void sample(){out=step(state,in,profiles,[&](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=known;c.fraction=blocked?0:1;return c;});
        for(unsigned i=0;i<out.commandCount;++i)commands.push_back(out.commands[i]);}
    void move(Vec3 delta,double dt=.01,bool moveModel=true,bool moveRelative=true){
        time+=dt;in.timeNs=std::uint64_t(std::llround(time*1e9));in.tick=in.timeNs/1000000;++in.sample;
        if(moveModel)in.hands[0].p=in.hands[0].p+delta;
        if(moveRelative)in.intentHands[0].p=in.intentHands[0].p+delta;
        sample();
    }
    void rest(double seconds){for(int n=0;n<int(std::ceil(seconds/.01));++n)move({});}
    int count(CommandKind kind)const{int n=0;for(auto c:commands)n+=c.kind==kind;return n;}
    int hits()const{return count(CommandKind::Slash)+count(CommandKind::Thrust)+count(CommandKind::Pommel);}
};
int main(){
    const auto motionOwned=[](Replay& r){r.in.coherentMelee119=r.in.physicalCombat177=r.in.motionOwned59c5=true;
        r.in.windupHold57=false;r.in.reach[0]=1.9;};
    const auto forwardOnly=[](Replay& r){r.in.coherentMelee119=r.in.physicalCombat177=r.in.forwardOnly19=true;
        r.in.motionOwned59c5=false;r.in.thrustAsSwing55=true;r.in.windupHold57=false;r.in.reach[0]=1.9;};
    // C62 actual producer: deliberate translated cut-to-cut, not a second
    // preparation, new clock, or second attack. Exercise both motion owners.
    for(bool direct62:{false,true}){Replay r;forwardOnly(r);r.in.motionOwned59c5=direct62;
        for(int n=0;n<4;++n)r.move({-.03,0,0});
        ck(r.count(CommandKind::Prepare)==1&&r.hits()==0,"C62 initial cut has one uncommitted preparation");
        const auto gesture=r.state.weapon[0].gesture59c3,prepareTick=r.state.weapon[0].prepareTick;
        for(int n=0;n<4;++n)r.move({0,0,-.03});
        ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Zone57)==1&&r.hits()==0&&
           r.state.weapon[0].prepareTick==prepareTick&&r.state.weapon[0].gesture59c3==gesture&&
           r.state.weapon[0].stroke118.age>.079&&r.state.weapon[0].stroke118.age<.081,
           "C62 fresh overhead segment refines same prepared cut and keeps original lifetime");
        for(const auto& c:r.commands)if(c.kind==CommandKind::Zone57)
            ck(c.cutFeint62&&c.zone117==0&&c.stroke117==CommandKind::Slash&&c.gesture59c3==gesture&&!c.held57,
               "C62 purposefully translated new zone is tagged as one zone-only cut request");
        for(int n=0;n<8;++n)r.move({0,0,-.03});
        ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Zone57)==1&&r.count(CommandKind::Slash)==1&&
           r.count(CommandKind::Abort)==0,"C62 refined cut earns new full travel before exactly one release");
        for(const auto& c:r.commands)if(c.kind==CommandKind::Slash)
            ck(c.cutFeint62&&c.zone117==0&&c.gesture59c3==gesture,"C62 release keeps original attempt and refined requested zone; native ACK belongs to consumer");
    }
    for(int changedFamily62=0;changedFamily62<2;++changedFamily62){Replay r;forwardOnly(r);
        for(int n=0;n<4;++n)r.move({-.03,0,0});
        for(int n=0;n<5;++n)r.move({0,0,-.03},.01,changedFamily62==0,changedFamily62==1);
        ck(r.count(CommandKind::Zone57)==0,"C62 direction change in only one family cannot advertise a cut feint");}
    {Replay r;forwardOnly(r);const auto transform=m::from_rotation_vector({0,0,1.5707963267948966});
        r.in.intentHands[0].q=transform;r.in.intentHands[0].p=m::rotate(transform,r.in.hands[0].p)+Vec3{2,-3,.4};
        r.rest(.12);r.commands.clear();
        for(int n=0;n<5;++n){const Vec3 d{-.03,0,0};r.in.intentHands[0].p=r.in.intentHands[0].p+m::rotate(transform,d);r.move(d,.01,true,false);}
        for(int n=0;n<4;++n){const Vec3 d{0,0,-.03};r.in.intentHands[0].p=r.in.intentHands[0].p+m::rotate(transform,d);r.move(d,.01,true,false);}
        ck(r.count(CommandKind::Zone57)==1&&r.count(CommandKind::Prepare)==1&&r.hits()==0,
           "C62 paired direction proof works in independently rotated/translated tracking coordinates");}
    {Replay r;forwardOnly(r);for(int n=0;n<4;++n)r.move({-.03,0,0});
        for(int n=0;n<7;++n){const double a=(n+1)*.14;r.move({-.03*std::cos(a),0,-.03*std::sin(a)});}
        ck(r.count(CommandKind::Zone57)==0,"C62 ordinary smoothly curved cut has no extra zone event");}
    {Replay r;forwardOnly(r);for(int n=0;n<4;++n)r.move({-.03,0,0});
        for(int n=1;n<=4;++n){r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({-.15*n,0,0});r.move({});}
        ck(r.count(CommandKind::Zone57)==0,"C62 fast tip-only wrist arc without translated hands cannot feint");}
    {Replay r;forwardOnly(r);for(int n=0;n<4;++n)r.move({-.03,0,0});
        for(int n=0;n<4;++n)r.move({0,0,-.03});
        for(int n=0;n<4;++n)r.move({.03,0,0});
        ck(r.count(CommandKind::Zone57)==1&&r.hits()==0&&r.count(CommandKind::Abort)==1,
           "C62 a second abrupt turn cannot renew the original attempt or create another refinement");}
    {Replay r;forwardOnly(r);for(int n=0;n<12;++n)r.move({-.03,0,0});
        const auto hits=r.hits();for(int n=0;n<4;++n)r.move({0,0,-.03});
        ck(hits==1&&r.hits()==1&&r.count(CommandKind::Zone57)==0,"C62 committed attack cannot be reclassified by a later turn");}
    {namespace mm=kcd2vr::melee118;mm::State s{};s.ready=true;mm::Input in{};
        in.dt=.01;in.eligible=in.known=true;in.tip=in.modelTip=in.hand=in.modelHand={-.03,0,0};
        in.axis=in.modelAxis={0,1,0};in.forward=in.modelForward={0,1,0};
        mm::Policy p{true,true,true,false,false,true};p.deliberateCut42=p.purposefulCuts52=p.cutFeint62=true;
        for(int n=0;n<4;++n)mm::update(s,in,p);
        in.tip=in.modelTip=in.hand=in.modelHand={0,0,-.03};
        for(int n=0;n<4;++n)mm::update(s,in,p);
        ck(s.cutSegment62&&s.age>.079,"C62 direct kernel retains original age across segment refinement");
        in.dt=.12;in.tip=in.modelTip=in.hand=in.modelHand={0,0,-.003};
        auto o=mm::update(s,in,p);ck(!o.refineCut62&&!o.commit,"C62 slow paused remainder cannot borrow old cut commitment distance");
    }
    std::printf("C62 cut feint production checks: %u PASS\n",checks);
}
