// D34 FE VrFeintHybrid: pure decision (pose ask + game-thread gate).
#include "vrik/feint_hybrid_fe.h"
#include "vrik/physical_weapon.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
using namespace kcd2vr::physical;
namespace fe=kcd2vr::feint_fe;
static unsigned checks=0;
static void ck(bool ok,const char* label){++checks;if(!ok){std::fprintf(stderr,"FAIL %s\n",label);std::exit(1);}}
// Pose producer replay (same harness as cut_feint62_test).
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
static fe::Live live(std::uint64_t g,std::uint64_t tick,int zone,int angle,bool nearB=false){return {true,nearB,g,tick,zone,angle};}
int main(){
    int z=-1;
    // Command default: no FE request unless the producer sets it.
    {kcd2vr::physical::Command c{};ck(!c.feintFE,"command default not FE");}
    // Disabled / not committed / not a cut never ask and reset the track.
    {fe::Track t{};ck(fe::consider(t,false,7,true,1000,1,live(7,1001,2,0),1001,z)==fe::Why::Disabled&&z==-1,"disabled");
     ck(fe::consider(t,true,7,true,0,1,live(7,1001,2,0),1001,z)==fe::Why::NotCommitted,"no commit");
     ck(fe::consider(t,true,7,false,1000,1,live(7,1001,2,0),1001,z)==fe::Why::NotCommitted,"thrust/not cut");}
    // Announced upper_left (1) at commit; blade settles upper_right (2, angle 0 = 45 deg from both edges): ask on 2nd sample.
    {fe::Track t{};
     ck(fe::consider(t,true,7,true,1000,1,live(7,1010,2,0),1010,z)==fe::Why::Settling,"first new sample settles");
     ck(fe::consider(t,true,7,true,1000,1,live(7,1010,2,0),1012,z)==fe::Why::NoSample,"same pose sample ignored");
     ck(fe::consider(t,true,7,true,1000,1,live(7,1021,2,0),1021,z)==fe::Why::Ask&&z==2,"stable new sector asks");
     // the asked zone becomes the announced one: no repeat ask for the same zone
     ck(fe::consider(t,true,7,true,1000,1,live(7,1032,2,0),1032,z)==fe::Why::SameZone,"no repeat for same zone");
     // a second, different zone may ask once more (game thread still spends only one native feint)
     (void)fe::consider(t,true,7,true,1000,1,live(7,1043,0,90),1043,z);
     ck(fe::consider(t,true,7,true,1000,1,live(7,1054,0,90),1054,z)==fe::Why::Ask&&z==0,"second ask other zone");
     ck(fe::consider(t,true,7,true,1000,1,live(7,1065,1,180),1065,z)==fe::Why::Spent,"budget per stroke");
     // a new gesture resets
     ck(fe::consider(t,true,8,true,2000,1,live(8,2001,2,0),2001,z)==fe::Why::Settling,"new gesture resets");}
    // The live sector equal to the announced zone never asks.
    {fe::Track t{};for(std::uint64_t k=0;k<5;++k)ck(fe::consider(t,true,7,true,1000,2,live(7,1010+k*11,2,0),1010+k*11,z)!=fe::Why::Ask,"same zone");}
    // Clear margin: within 15 deg of a boundary (C39 near flag is only 8) resets the candidate; C39 near flag refuses.
    {fe::Track t{};
     ck(fe::consider(t,true,7,true,1000,1,live(7,1010,2,35),1010,z)==fe::Why::Unclear,"10 deg from 45 boundary unclear");
     ck(fe::consider(t,true,7,true,1000,1,live(7,1021,2,0),1021,z)==fe::Why::Settling,"unclear sample broke stability");
     ck(fe::consider(t,true,7,true,1000,1,live(7,1032,2,0,true),1032,z)==fe::Why::Unclear,"C39 near boundary");
     ck(fe::consider(t,true,7,true,1000,1,live(7,1043,2,0),1043,z)==fe::Why::Settling,"near broke stability");
     ck(fe::consider(t,true,7,true,1000,1,live(7,1054,2,0),1054,z)==fe::Why::Ask,"then asks");}
    // An upward cut that C39 remapped to its lateral side (angle in the low table) keeps C39's own confidence.
    ck(fe::clear(false,2,270)&&!fe::clear(true,2,270),"remapped upward cut");
    ck(!fe::clear(false,5,270)&&!fe::cutZone(5),"lower zone is not a feint target");
    // Another gesture's mailbox sample and a late sample never ask.
    {fe::Track t{};ck(fe::consider(t,true,7,true,1000,1,live(9,1010,2,0),1010,z)==fe::Why::NoSample,"foreign gesture sample");
     ck(fe::consider(t,true,7,true,1000,1,live(7,1500,2,0),1000+fe::kWindowMs+1,z)==fe::Why::Late,"after window");}
    // Game-thread gate.
    fe::GameState g{};g.on=g.hybrid=g.owner=g.opponent=g.readable=true;g.sinceCommitMs=120;g.henryPhase=4;
    g.npcPhase=2;g.npc5D0=-1;g.oldZone=1;g.target=2;
    ck(fe::gate(g)==fe::Gate::Pass&&std::strcmp(fe::name(fe::Gate::Pass),"NATIVE_936A28")==0,"pass");
    {auto b=g;b.on=false;ck(fe::gate(b)==fe::Gate::Disabled,"gate disabled");}
    {auto b=g;b.hybrid=false;ck(fe::gate(b)==fe::Gate::NotHybrid,"DIRECT/INTENT untouched");}
    {auto b=g;b.owner=false;ck(fe::gate(b)==fe::Gate::Owner,"owner");}
    {auto b=g;b.target=5;ck(fe::gate(b)==fe::Gate::Owner,"target not cut zone");}
    {auto b=g;b.used=true;ck(fe::gate(b)==fe::Gate::Spent,"one native feint per stroke");}
    {auto b=g;b.sinceCommitMs=fe::kWindowMs+1;ck(fe::gate(b)==fe::Gate::Late,"late");}
    {auto b=g;b.henryPhase=8;ck(fe::gate(b)==fe::Gate::HenryNotWindup,"strike phase: native would not notify");}
    {auto b=g;b.opponent=false;ck(fe::gate(b)==fe::Gate::NoOpponent,"no opponent");}
    {auto b=g;b.npcPhase=-1;ck(fe::gate(b)==fe::Gate::NoOpponent,"opponent phase unread");}
    {auto b=g;b.npcPhase=256;b.npc5D0=1;ck(fe::gate(b)==fe::Gate::NpcBlockStarted,"NPC block started (256, row taken)");}
    {auto b=g;b.npcPhase=128;b.npc5D0=0;ck(fe::gate(b)==fe::Gate::NpcBlockStarted,"NPC block started (128, row 0)");}
    {auto b=g;b.npcPhase=256;b.npc5D0=-1;ck(fe::gate(b)==fe::Gate::Pass,"guard phase without row = not started");}
    {auto b=g;b.npcPhase=2;b.npc5D0=1;ck(fe::gate(b)==fe::Gate::Pass,"stale row outside block phase");}
    {auto b=g;b.oldZone=2;ck(fe::gate(b)==fe::Gate::NoZoneChange,"no zone change");}
    {auto b=g;b.npcPhase=256;b.npc5D0=2;b.oldZone=2;ck(fe::gate(b)==fe::Gate::NpcBlockStarted,"NPC gate before zone check");}
    {auto b=g;b.comboSlot=true;ck(fe::gate(b)==fe::Gate::ComboSlot,"combo slot kept");}
    {auto b=g;b.readable=false;ck(fe::gate(b)==fe::Gate::Unreadable,"unreadable");}
    // Pose producer: HYBRID (forwardOnly, not motion-owned) committed cut from the right, then the blade turns overhead.
    const auto hybrid=[](Replay& r,bool direct){r.in.coherentMelee119=r.in.physicalCombat177=r.in.forwardOnly19=true;
        r.in.motionOwned59c5=direct;r.in.thrustAsSwing55=true;r.in.windupHold57=false;r.in.reach[0]=1.9;r.in.dynamics=true;};
    const auto feCount=[](const Replay& r,int& zone,std::uint64_t& gesture){int n=0;for(const auto& c:r.commands)if(c.feintFE){++n;zone=c.zone117;gesture=c.gesture59c3;
        ck(c.kind==CommandKind::Zone57&&c.stroke117==CommandKind::Slash&&!c.cutFeint62&&!c.held57,"FE request shape");}return n;};
    const auto slashBeforeFE=[](const Replay& r){bool slash=false;for(const auto& c:r.commands){if(c.kind==CommandKind::Slash)slash=true;if(c.feintFE)return slash;}return false;};
    {fe::enabled=true;Replay r;hybrid(r,false);for(int n=0;n<12;++n)r.move({-.03,0,0});
     ck(r.count(CommandKind::Slash)==1,"HYBRID cut committed");const auto g=r.state.weapon[0].gesture59c3;
     for(int n=0;n<10;++n)r.move({0,0,-.03});
     int zf=-1;std::uint64_t gf=0;ck(feCount(r,zf,gf)==1&&zf==0&&gf==g&&slashBeforeFE(r),"post-commit turn asks ONE native feint to head, same gesture");
     ck(r.count(CommandKind::Slash)==1&&r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Abort)==0,"no second attack, no abort");}
    {fe::enabled=false;Replay r;hybrid(r,false);for(int n=0;n<12;++n)r.move({-.03,0,0});for(int n=0;n<10;++n)r.move({0,0,-.03});
     int zf=-1;std::uint64_t gf=0;ck(feCount(r,zf,gf)==0,"VrFeintHybrid=false: no request");fe::enabled=true;}
    {Replay r;hybrid(r,true);for(int n=0;n<12;++n)r.move({-.03,0,0});for(int n=0;n<10;++n)r.move({0,0,-.03});
     int zf=-1;std::uint64_t gf=0;ck(feCount(r,zf,gf)==0,"DIRECT motion owner: FE never asks (C39/C62 own it)");}
    {Replay r;hybrid(r,false);for(int n=0;n<24;++n)r.move({-.03,0,0});
     int zf=-1;std::uint64_t gf=0;ck(feCount(r,zf,gf)==0,"straight committed cut: no feint");}
    {Replay r;hybrid(r,false);for(int n=0;n<12;++n)r.move({-.03,0,0});r.rest(.5);for(int n=0;n<10;++n)r.move({0,0,-.03});
     int zf=-1;std::uint64_t gf=0;ck(feCount(r,zf,gf)==0,"turn after the FE window: no feint");}
    std::printf("feint_hybrid_fe_test %u/%u PASS\n",checks,checks);
    return 0;
}
