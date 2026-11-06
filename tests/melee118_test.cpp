#include "vrik/physical_weapon.h"
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <memory>
#include <vector>
using namespace kcd2vr::physical;
static unsigned checks=0;
static void ck(bool ok,const char* label){++checks;if(!ok){std::fprintf(stderr,"FAIL %s\n",label);std::exit(1);}}
struct Replay {
    // Debug retains all replay locals in main's stack frame. Keep the large
    // production state on the heap; this changes no production stack setting.
    Input in{};std::unique_ptr<State> storage=std::make_unique<State>();State& state=*storage;
    std::array<Profile,2> profiles{};Output out{};double time=1;bool known=true,blocked=false;
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

// This fallback lets the identical checks exercise the literal older classifier:
// it has no control field, so only opt-in expectations fail instead of compilation.
template<class P> static auto axisControlH41(P& p,double v,int)->decltype(p.bladeAxisCosineH41=v,void()){p.bladeAxisCosineH41=v;}
template<class P> static void axisControlH41(P&,double,long){}
static void bladeAxisControlsH41(){
    namespace me=kcd2vr::melee118;
    const auto sample=[](double cosine,double modelCosine){
        me::Input x{};x.dt=.01;x.eligible=x.known=true;x.outward=1;
        x.axis=x.modelAxis={0,1,0};
        x.hand={.03*std::sqrt(1-cosine*cosine),.03*cosine,0};
        x.modelHand={.03*std::sqrt(1-modelCosine*modelCosine),.03*modelCosine,0};
        x.tip=x.hand;x.modelTip=x.modelHand;return x;};
    const auto classify=[&](double value,double world,double model){
        me::Policy p{};axisControlH41(p,value,0);me::State s{};s.ready=true;
        const auto result=me::update(s,sample(world,model),p);return result;};
    ck(classify(.80,.75,.75).observation.kind==me::Kind::Cut,"H41 default .80 retains .75-axis cut");
    ck(classify(.72,.75,.75).observation.kind==me::Kind::Thrust,"H41 explicit .72 recognizes .75-axis thrust");
    for(double setting:{.60,.72,.80,.95}){
        ck(classify(setting,0,0).observation.kind==me::Kind::Cut,"H41 sideways motion remains cut at every setting");
        ck(classify(setting,.99,.99).observation.kind==me::Kind::Thrust,"H41 aligned forward motion remains thrust");
        ck(classify(setting,-.99,-.99).observation.kind==me::Kind::Pommel,"H41 reverse axial classification remains pommel, never thrust");
        ck(classify(setting,.99,-.99).observation.kind==me::Kind::Cut,"H41 opposite frame signs cannot qualify axial motion");
    }
    ck(classify(.72,.75,.65).observation.kind==me::Kind::Cut&&classify(.72,.65,.75).observation.kind==me::Kind::Cut,
        "H41 both independent frames must meet the selected alignment");
    ck(classify(.72,-.75,-.75).observation.kind==me::Kind::Pommel&&classify(.80,-.75,-.75).observation.kind==me::Kind::Cut,
        "H41 opt-in widening also applies to reverse pommel classification");
    for(double invalid:{.59,.96,-1.,std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN()})
        ck(classify(invalid,.75,.75).observation.kind==me::Kind::Cut,"H41 invalid setting falls back to .80 rather than widening");
    for(bool model:{false,true}){me::Policy p{};axisControlH41(p,.72,0);me::State s{};s.ready=true;auto x=sample(.75,.75);
        (model?x.modelAxis:x.axis)={};const auto o=me::update(s,x,p);
        ck(o.observation.reason==me::Reason::Unavailable&&!s.active&&!o.commit,"H41 missing axis keeps original unavailable behavior");}
    for(double setting:{.72,.80}){me::Policy p{};axisControlH41(p,setting,0);
        p.deliberate119=p.forwardOnly19=p.motionOwned59c5=p.pommelForward69=true;
        me::State s{};s.ready=true;auto x=sample(-.75,-.75);x.forward=x.modelForward={0,1,0};
        const auto o=me::update(s,x,p);
        ck(o.observation.reason==me::Reason::Withdrawn&&o.observation.windup&&!s.active&&!o.prepare&&!o.commit,
            "H41 true backward windup remains withdrawn at default and wider alignment");
        x=sample(.75,.75);x.axis=x.modelAxis={0,-1,0};x.forward=x.modelForward={0,1,0};s={};s.ready=true;
        const auto endFirst=me::update(s,x,p);
        ck(!endFirst.prepare&&!endFirst.commit,"H41 wider reverse classification does not relax existing .80 forward-pommel safeguard");
    }
    const auto refine=[&](double setting,double age,bool disagree){me::Policy p{};p.deliberate119=true;axisControlH41(p,setting,0);
        me::State s{};s.ready=true;auto first=sample(.60,.60);me::update(s,first,p);
        ck(s.active&&s.kind==me::Kind::Cut,"H41 refinement begins with an actual cut candidate");
        s.age=age;auto next=sample(.90,disagree?.65:.90);me::update(s,next,p);return s.kind;};
    ck(refine(.80,.060,false)==me::Kind::Cut,"H41 default cumulative alignment retains the original cut");
    ck(refine(.72,.060,false)==me::Kind::Thrust,"H41 selected alignment also controls cumulative refinement at 60 ms");
    ck(refine(.72,.061,false)==me::Kind::Cut,"H41 alignment control never extends the existing 60 ms refinement");
    ck(refine(.72,.060,true)==me::Kind::Cut,"H41 refinement requires both motion families at selected alignment");
}

int main(){
    for(unsigned scenario=0;scenario<3;++scenario){ // Geometric wrist cuts with backward cocking between.
        namespace me=kcd2vr::melee118;me::State state;state.ready=true;me::Policy policy;
        policy.deliberate119=policy.handRearm178=policy.responsive44c2=policy.forwardOnly19=true;
        policy.purposefulCuts52=policy.deliberateCut42=policy.c54PostGuardH27=true;
        me::Input in;in.dt=.01;in.eligible=in.known=true;in.outward=.4;
        in.forward=in.modelForward={0,1,0};me::Vec3 prior{0,0,.6};bool recoveryPhase=false;
        auto arc=[&](double angle,double handStep){
            const me::Vec3 tip{0,.6*std::sin(angle),.6*std::cos(angle)};
            in.axis=in.modelAxis={0,std::sin(angle),std::cos(angle)};
            in.hand=in.modelHand={0,handStep,0};in.tip=in.modelTip=tip-prior+in.hand;prior=tip;
            if(recoveryPhase&&scenario==1)in.modelTip=in.modelTip*-1.; // families disagree
            in.blocked=recoveryPhase&&scenario==2; // a stopped blade is not free recovery
            return me::update(state,in,policy).commit;
        };
        unsigned first=0,windup=0,second=0;
        for(unsigned n=1;n<=24;++n)first+=arc(n*.05,.0001);
        recoveryPhase=true;
        for(unsigned n=1;n<=24;++n)windup+=arc((24-n)*.05,0);
        recoveryPhase=false;
        for(unsigned n=1;n<=24;++n)second+=arc(n*.05,.0001);
        ck(first==1&&windup==0&&second==(scenario==0?1u:0u),
            "wrist rearm requires paired unobstructed backward tip recovery and never fires on windup");
    }
    const auto motionOwned=[](Replay& r){r.in.coherentMelee119=r.in.physicalCombat177=r.in.motionOwned59c5=true;
        r.in.windupHold57=false;r.in.reach[0]=1.9;};
    const auto forwardOnly=[](Replay& r){r.in.coherentMelee119=r.in.physicalCombat177=r.in.forwardOnly19=true;
        r.in.motionOwned59c5=false;r.in.thrustAsSwing55=true;r.in.windupHold57=false;r.in.reach[0]=1.9;};
    {Replay r;forwardOnly(r);r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,3.141592653589793});
        r.rest(.12);r.commands.clear();
        for(int n=1;n<=10;++n){r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,3.141592653589793-.1*n});r.move({0,.015,0});}
        ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Slash)==1&&r.count(CommandKind::Pommel)==0,
           "forward cocked blade unwinding through a real wrist cut is not backward recovery or an automatic pommel");}
    {Replay r;forwardOnly(r);r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,3.141592653589793});
        r.rest(.12);r.commands.clear();for(int n=0;n<18;++n)r.move({0,.02,0});
        ck(r.count(CommandKind::Prepare)==0&&r.hits()==0,"backward-facing end-first translation without edge rotation is not converted into a cut");}
    // C71: the same end-first forward push IS a pommel strike when the pommel policy is on; retreat and edge-rotation cuts stay as before.
    {Replay r;forwardOnly(r);r.in.pommelStrike69=true;r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,3.141592653589793});
        r.rest(.12);r.commands.clear();for(int n=0;n<18;++n)r.move({0,.02,0});
        ck(r.count(CommandKind::Pommel)==1&&r.count(CommandKind::Slash)==0&&r.count(CommandKind::Thrust)==0,"C71 forward end-first push is exactly one pommel when VrPommelStrike is on");}
    {Replay r;forwardOnly(r);r.in.pommelStrike69=true;r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,3.141592653589793});
        r.rest(.12);r.commands.clear();for(int n=0;n<18;++n)r.move({0,-.02,0});
        ck(r.hits()==0,"C71 end-first RETREAT is never a pommel");}
    {Replay r;forwardOnly(r);r.in.pommelStrike69=true;r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,3.141592653589793});
        r.rest(.12);r.commands.clear();
        for(int n=1;n<=10;++n){r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,3.141592653589793-.1*n});r.move({0,.015,0});}
        ck(r.count(CommandKind::Pommel)==0&&r.count(CommandKind::Slash)==1,"C71 cocked blade unwinding through a wrist cut stays a cut with pommel policy on");}
    // Counterexamples for the narrow cocked-cut conversion: a forward
    // pommel push plus blade-axis roll has no transverse edge velocity.
    {Replay r;forwardOnly(r);const auto back=m::from_rotation_vector({0,0,3.141592653589793});
        r.in.hands[0].q=r.in.intentHands[0].q=back;r.rest(.12);r.commands.clear();
        for(int n=1;n<=18;++n){r.in.hands[0].q=r.in.intentHands[0].q=m::multiply(back,m::from_rotation_vector({0,.2*n,0}));r.move({0,.02,0});}
        ck(r.count(CommandKind::Prepare)==0&&r.hits()==0,"end-first push plus pure blade-axis roll cannot become a cocked edge cut");}
    for(bool modelRotates:{false,true}){Replay r;forwardOnly(r);const auto back=m::from_rotation_vector({0,0,3.141592653589793});
        r.in.hands[0].q=r.in.intentHands[0].q=back;r.rest(.12);r.commands.clear();
        for(int n=1;n<=5;++n){const auto q=m::from_rotation_vector({0,0,3.141592653589793-.1*n});
            if(modelRotates)r.in.hands[0].q=q;else r.in.intentHands[0].q=q;r.move({0,.015,0});}
        ck(r.count(CommandKind::Prepare)==0&&r.hits()==0,"cocked edge rotation in only one independent motion family is not an attack");}
    {Replay r;forwardOnly(r);const auto back=m::from_rotation_vector({0,0,3.141592653589793});
        r.in.hands[0].q=r.in.intentHands[0].q=back;r.rest(.12);r.commands.clear();
        for(int n=1;n<=18;++n){r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,3.141592653589793-.005*n});r.move({0,.02,0});}
        ck(r.count(CommandKind::Prepare)==0&&r.hits()==0,"forward end-first push with slow transverse drift lacks cutting edge velocity");}
    {Replay r;forwardOnly(r);const auto back=m::from_rotation_vector({0,0,3.141592653589793});
        r.in.hands[0].q=r.in.intentHands[0].q=back;r.rest(.12);r.commands.clear();
        for(int n=1;n<=10;++n){r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,3.141592653589793-.1*n});r.move({0,-.015,0});}
        ck(r.count(CommandKind::Prepare)==0&&r.hits()==0,"cocked edge rotation with retreating hands remains recovery");}
    {Replay r;forwardOnly(r);r.move({.03,0,0});
        ck(r.count(CommandKind::Prepare)==0&&r.hits()==0,"C42 hybrid short adjustment does not announce an attack");
        r.rest(.20);ck(r.hits()==0&&r.count(CommandKind::Abort)==0,"unannounced short adjustment needs no native abort");}
    {Replay r;forwardOnly(r);r.move({.025,0,0});for(int n=0;n<14;++n)r.move({0,.02,0});
        ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Zone57)==0&&r.count(CommandKind::Thrust)==1&&r.count(CommandKind::Slash)==0,
           "C42 unannounced adjustment refines to one genuine thrust without a phantom cut zone");
        const auto id=r.commands.front().gesture59c3;
        for(const auto& c:r.commands)if(c.kind==CommandKind::Prepare||c.kind==CommandKind::Zone57||c.kind==CommandKind::Thrust)
            ck(c.gesture59c3==id,"SYNC refinement keeps the same gesture identity");}
    for(Vec3 direction:{Vec3{.03,.005,0},Vec3{-.03,0,0},Vec3{0,.008,-.03}}){
        Replay r;forwardOnly(r);for(int n=0;n<24;++n)r.move(direction);
        ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Slash)==1,"forward-only: forward/lateral/overhead cut preserves one prepare and one native release");
    }
    {Replay r;forwardOnly(r);for(int n=0;n<18;++n)r.move({0,.02,0});
        ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Thrust)==1,"forward-only: intentional forward stab retains native input classification");}
    for(int frame=0;frame<3;++frame){Replay r;forwardOnly(r);
        for(int n=0;n<18;++n)r.move({0,-.02,0},.01,frame!=1,frame!=0);
        ck(r.count(CommandKind::Prepare)==0&&r.hits()==0,"forward-only: rearward recovery and frame disagreement do not create automatic pommel attack");}
    {Replay r;forwardOnly(r);r.profiles[0].axis={0,-1,0};r.profiles[0].tip={0,-.6,0};r.rest(.12);r.commands.clear();
        for(int n=0;n<18;++n)r.move({0,-.02,0});
        ck(r.count(CommandKind::Prepare)==0&&r.hits()==0,"forward-only: backward-facing axial movement cannot masquerade as forward stab");}
    for(bool model:{false,true}){Replay r;forwardOnly(r);for(int n=0;n<20;++n)r.move({.03,0,0},.01,model,!model);
        ck(r.count(CommandKind::Prepare)==0&&r.hits()==0,"forward-only: body/relative motion disagreement is not an attack");}
    for(bool backward:{false,true}){Replay r;forwardOnly(r);
        const auto trackingFromModel=kcd2vr::pose_core::from_rotation_vector({0,0,1.5707963267948966});
        r.in.intentHands[0].q=trackingFromModel;
        r.in.intentHands[0].p=kcd2vr::pose_core::rotate(trackingFromModel,r.in.hands[0].p)+Vec3{2,-3,.4};
        r.rest(.12);r.commands.clear();
        const Vec3 modelDelta{0,backward?-.02:.02,0};
        for(int n=0;n<18;++n){r.in.intentHands[0].p=r.in.intentHands[0].p+kcd2vr::pose_core::rotate(trackingFromModel,modelDelta);r.move(modelDelta,.01,true,false);}
        ck(backward?(r.count(CommandKind::Prepare)==0&&r.hits()==0):(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Thrust)==1),
            backward?"90 degree rotated/translated tracking frame: backward winding is not an attack":"90 degree rotated/translated tracking frame: true forward thrust is accepted");
    }
    {Replay r;forwardOnly(r);for(int n=0;n<4;++n)r.move({.03,0,0});r.in.tracked[0]=false;r.move({.03,0,0});
        for(const auto& c:r.commands)if(c.kind==CommandKind::Abort)ck(!c.recognizerAbort18,"tracking-loss Abort remains hard even in direct/synchronized forward policy");}
    {
        Input in{};in.shoulders[0]={.2,0,1.3};in.shoulders[1]={-.2,0,1.3};
        ck(!windup_cue_motion59c10(in,{0,-.02,0},{},{0,-.03,0},{},.01),"C10 backswing cue rejects model-only locomotion");
        ck(!windup_cue_motion59c10(in,{}, {0,-.02,0},{},{0,-.03,0},.01),"C10 backswing cue rejects relative-only disagreement");
        ck(!windup_cue_motion59c10(in,{0,-.005,0},{0,-.005,0},{0,-.005,0},{0,-.005,0},.01),"C10 slow rearward reposition does not become a threat");
        ck(windup_cue_motion59c10(in,{0,-.02,0},{0,-.02,0},{0,-.03,0},{0,-.03,0},.01),"C10 coherent deliberate rearward cue qualifies both motion frames");
        // Body turned 90 degrees: use shoulder-derived body vectors, not a guessed fixed forward axis.
        in.shoulders[0]={0,.2,1.3};in.shoulders[1]={0,-.2,1.3};
        ck(windup_cue_motion59c10(in,{.02,0,0},{.02,0,0},{.03,0,0},{.03,0,0},.01),"C10 backswing qualification follows Henry's body basis");
    }
    for(int frames=0;frames<3;++frames){Replay r;motionOwned(r);r.in.thrustAsSwing55=true;
        r.profiles[0].axis={0,0,1};r.profiles[0].tip={0,0,.6};r.rest(.12);r.commands.clear();
        r.move({0,-.02,0},.01,frames!=1,frames!=0);r.rest(.62);
        ck(r.count(CommandKind::Prepare)==0&&r.count(CommandKind::Abort)==0&&r.hits()==0&&!r.state.weapon[0].cocked57.active,
           "C28 DIRECT rearward-only cues do not consume the finite forward threat clock in either motion family");
    }
    {Replay r;motionOwned(r);r.in.telegraphWindup59c10=false;r.move({.03,0,0});
        ck(r.count(CommandKind::Prepare)==1&&r.hits()==0,"C10 forward onset remains independent of the optional backswing cue");}
    {Replay r;motionOwned(r);r.move({.03,0,0});
        ck(r.count(CommandKind::Prepare)==1&&r.hits()==0,"C10 onset advertises first deliberate forward sample without committing it");
        r.sample();ck(r.count(CommandKind::Prepare)==1,"C10 duplicate pose cannot duplicate onset intent");
        for(int n=0;n<20;++n)r.move({.03,0,0});
        ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Slash)==1&&r.count(CommandKind::Abort)==0,
           "C10 a complete forward cut has one preparation and one commitment");}
    {Replay r;motionOwned(r);r.move({.03,0,0});r.move({.03,0,0});r.rest(.20);
        ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Abort)==1&&r.hits()==0,
           "C10 stopped short onset is cancelled once and cannot become a hit");}
    {Replay r;motionOwned(r);r.move({.025,0,0});for(int n=0;n<14;++n)r.move({0,.02,0});
        ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Zone57)==1&&r.count(CommandKind::Thrust)==1&&r.count(CommandKind::Slash)==0,
           "C10 early lateral onset refines to a thrust without duplicate prepare or wrong cut commitment");
        const auto identity=r.commands.front().gesture59c3;
        for(auto c:r.commands)if(c.kind==CommandKind::Prepare||c.kind==CommandKind::Zone57||c.kind==CommandKind::Thrust)
            ck(c.gesture59c3==identity,"C10 onset refinement retains the original attempt identity");
        for(auto c:r.commands)if(c.kind==CommandKind::Zone57)
            ck(c.stroke117==CommandKind::Thrust&&c.zone117==5,"C10 same-attempt rezone carries corrected technique before contact");}
    {Replay r;motionOwned(r);r.move({.03,0,0});r.blocked=true;r.move({.03,0,0});r.rest(.1);
        ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Abort)==1&&r.hits()==0,
           "C10 world obstruction retires existing onset rather than leaking prepared state");}
    {Replay r;motionOwned(r);for(int n=0;n<10;++n)r.move({.03,0,0},.01,true,false);
        ck(r.count(CommandKind::Prepare)==0&&r.hits()==0,"C10 body movement without tracked-relative stroke never telegraphs");}
    {namespace mm=kcd2vr::melee118;mm::State s{};s.ready=true;mm::Input in{};
        in.dt=.01;in.eligible=in.known=true;in.tip=in.modelTip={0,-.03,0};in.hand=in.modelHand={0,-.01,0};
        in.axis=in.modelAxis={1,0,0};in.modelForward={0,1,0};
        auto out=mm::update(s,in,{true,true,true,true,true});
        ck(!out.prepare&&!out.commit&&!s.active&&out.observation.windup,"C10 backward cue remains excluded from forward onset and commitment");
        s={};s.ready=true;out=mm::update(s,in,{true,true,true,false,true});
        ck(!out.observation.windup&&!out.prepare&&!out.commit,"C10 native comparison does not gain a backward preparation cue");
        s={};s.ready=true;in.tip=in.modelTip={.03,0,0};in.hand=in.modelHand={.03,0,0};in.axis=in.modelAxis={0,1,0};
        out=mm::update(s,in,{true,true,true,false,false});
        ck(!out.prepare&&!out.commit&&s.active,"C10 onset policy cannot alter native comparison timing");}
    {Replay r;for(int jab=0;jab<8;++jab){for(int i=0;i<2;++i)r.move({0,.015,0});for(int i=0;i<2;++i)r.move({0,-.015,0});}
        ck(r.hits()==0,"repeated 3 cm push-pull jabs cannot accumulate a thrust");ck(r.count(CommandKind::Abort)>0,"withdrawal balances the prepared input");}
    {Replay r;for(int n=0;n<40;++n)r.move({0,.006,0});r.rest(.1);ck(r.hits()==0,"0.6 m/s reaching never spends a native thrust");}
    {Replay r;for(int n=0;n<12;++n)r.move({.006,0,0});r.rest(.1);ck(r.hits()==0,"slow lateral reposition is not a cut");}
    {Replay r;for(int n=0;n<5;++n)r.move({0,.02,0});r.rest(.03);ck(r.count(CommandKind::Thrust)==1,"short deliberate 10 cm jab commits near its speed peak");}
    {Replay r;for(int n=0;n<3;++n)r.move({0,.02,0});r.rest(.1);ck(r.hits()==0,"fast 6 cm poke stays below coherent thrust travel threshold");}
    {Replay r;for(int n=0;n<24;++n)r.move({.02,0,0});ck(r.count(CommandKind::Slash)==1,"one prolonged cut creates exactly one native request");
        const auto before=r.commands.size();r.sample();ck(r.commands.size()==before,"duplicate eye/sample cannot replay a command");}
    {Replay r;for(int n=0;n<8;++n)r.move({0,.02,0});r.rest(.01);
        for(int n=0;n<25;++n)r.move({0,0,0});for(int n=0;n<8;++n)r.move({0,.02,0});
        ck(r.count(CommandKind::Thrust)==2,"two separate deliberate jabs rearm after a quiet interval");}
    {Replay r;for(int n=0;n<12;++n)r.move({.018,0,0});r.rest(.13);
        for(int n=0;n<20;++n)r.move({-.018,0,0});ck(r.count(CommandKind::Slash)==2,"deliberate opposite cut can rearm without freezing the hand");}
    {Replay r;for(int n=0;n<20;++n)r.move({.02,0,0},.01,true,false);ck(r.hits()==0,"common body movement without relative hand motion cannot attack");}
    {Replay r;for(int n=0;n<20;++n)r.move({.02,0,0},.01,false,true);ck(r.hits()==0,"head-relative movement alone cannot attack");}
    {Replay r;for(int n=0;n<20;++n){const double a=(n+1)*.04;r.in.hands[0].q=r.in.intentHands[0].q=m::from_rotation_vector({0,0,a});r.move({});}
        ck(r.hits()==0,"pure wrist rotation without deliberate hand motion cannot attack");}
    {Replay r;r.move({0,.02,0});r.move({0,.02,0});r.known=false;r.move({0,.02,0});r.known=true;
        for(int n=0;n<8;++n)r.move({0,.02,0});ck(r.hits()==1,"V146R1 one unknown collision frame inside a clear swing neither cancels it nor duplicates it");
        ck(r.count(CommandKind::Abort)==0&&r.count(CommandKind::Prepare)==1,"V146R1 the pending prepare survives one unknown frame (no abort/re-prepare)");}
    {Replay r;r.move({0,.02,0});r.move({0,.02,0});r.in.grip[0]=0;r.move({0,.02,0});
        ck(r.hits()==0&&r.count(CommandKind::Abort)==1,"grip release aborts without a final swing");}
    {Replay r;r.move({0,.02,0});r.move({0,.02,0});r.move({0,.04,0},.08);
        ck(r.hits()==0,"80 ms pose gap never makes a fabricated high-speed sample");}
    {Replay r;r.move({0,.02,0});r.move({0,.02,0});++r.profiles[0].generation;
        for(int n=0;n<8;++n)r.move({0,.02,0});ck(r.hits()==0,"replaced weapon cannot inherit a prepared gesture");}
    {Replay r;r.blocked=true;for(int n=0;n<12;++n)r.move({0,.02,0});ck(r.hits()==0,"continued pressure against a blocked weapon is not a new strike");}
    {Replay r;r.in.tracked[0]=false;r.move({0,.02,0});r.in.tracked[0]=true;for(int n=0;n<8;++n)r.move({0,.02,0});
        ck(r.hits()==0,"tracking reacquisition requires rearming rather than replaying motion");}
    double firstMin=10,firstMax=0;
    for(int hz:{60,72,90,120,144}){
        Replay r;double first=0;const double start=r.time;
        for(int n=0;n<hz/5;++n){r.move({0,1.8/hz,0},1.0/hz);if(!first&&r.count(CommandKind::Thrust))first=r.time-start;}
        ck(r.count(CommandKind::Prepare)==1&&r.count(CommandKind::Thrust)==1,"one coherent thrust at every tested refresh rate");
        for(auto c:r.commands)if(c.kind==CommandKind::Prepare||c.kind==CommandKind::Thrust)ck(c.zone117==5&&c.stroke117==CommandKind::Thrust,"type and zone survive preparation to commit");
        firstMin=std::min(firstMin,first);firstMax=std::max(firstMax,first);
    }
    ck(firstMin>=.060&&firstMax-firstMin<.018,"commit timing varies by at most one low-rate sample, not a fixed frame count");
    {Replay r;r.in.thrustAsSwing55=true;for(int n=0;n<8;++n)r.move({0,.02,0});
        bool any=false;for(auto c:r.commands)if(c.kind==CommandKind::Prepare||c.kind==CommandKind::Thrust){any=true;
            ck((c.zone117==0||c.zone117==1||c.zone117==2)&&c.stroke117==CommandKind::Thrust,"R55 thrust-as-swing keeps the thrust classification and asks for the ordinary zone of its origin");}
        ck(any&&r.count(CommandKind::Thrust)==1,"R55 thrust-as-swing still commits one thrust");}
    bladeAxisControlsH41();
    std::printf("melee118=%u/%u commitRangeMs=%.2f..%.2f retail_execution=NONE\n",checks,checks,firstMin*1000,firstMax*1000);
}
