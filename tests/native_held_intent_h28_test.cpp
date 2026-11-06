#include "../src/vrik/native_held_pose_t4.h"
#include <cstdio>
#include <vector>
using namespace kcd2vr;namespace p=physical;namespace b=native_weapon_body_t4;
static unsigned checks=0,failures=0;void ck(bool x,const char* msg){++checks;if(!x){++failures;std::printf("FAIL %s\n",msg);}}
struct Rig {
 b::Input source{};b::Achieved a{};b::ContactOwnerH24 live{};p::Input in{};p::Output out{};p::State state{};std::array<p::Profile,2> profiles{};std::vector<p::Command> commands;
 Rig(){source.owner={77,2,3,4,5,6,7,8,9,10,11,0};source.colliderLease=12;source.trackingEpoch=13;source.cancelGeneration=14;
 a.owner=source.owner;a.colliderLease=12;a.trackingEpoch=13;a.cancelGeneration=14;a.worldGeneration=15;a.bodyGeneration=16;a.bodyId=17;a.state=b::State::Active;a.poseValid=true;a.colliderRetired=false;
 a.sample=20;a.sourceTick=a.observedTick=1000;a.weaponWorld.p={.5,.3,1.1};a.intentFrameH28.valid=true;
 live.owner=a.owner;live.colliderLease=12;live.trackingEpoch=13;live.cancelGeneration=14;live.worldGeneration=15;live.bodyGeneration=16;live.bodyId=17;live.body=100;live.foreign=101;live.foreignType=0x5634;live.active=true;
 in.haveWorld=in.allowed=in.tracked[0]=in.intentTracked[0]=true;in.sample=20;in.tick=1000;in.epoch=3;in.bodyEpoch=4;in.reach[0]=3;in.worldScale=1;in.positionGuard176=in.naturalCombat117=in.coherentMelee119=in.physicalCombat177=in.forwardOnly19=true;in.grip[0]=1;in.strikeMinScale178=.05;
 in.shoulders[0]={.15,0,1.3};in.shoulders[1]={-.15,0,1.3};
 auto& w=profiles[0];w.key=77;w.generation=2;w.entity=11;w.side=0;w.primary=w.geometry=true;w.classId=4;w.weight=1.6;w.tip={1,0,0};w.axis={1,0,0};w.guardFaceReadyH24=true;w.guardFaceNormalH24={0,0,1};
 for(int i=0;i<10;++i)step();}
 void step(bool advance=true){++in.sample;in.tick+=10;in.attacks=true;
 if(advance){++a.sample;a.sourceTick=a.observedTick=in.tick;}else a.observedTick=in.tick;
 live.sourceSample=a.sample;live.sourceTick=a.sourceTick;live.observedTick=a.observedTick;
 in.hands[0]=in.intentHands[0]={a.weaponWorld.q,a.weaponWorld.p};
 ck(native_held_pose_t4::apply(source,a,in.tick,in,profiles,out,state,&live,1),"actual apply");
 for(unsigned n=0;n<out.commandCount;++n)commands.push_back(out.commands[n]);}
 unsigned count(p::CommandKind k){unsigned n=0;for(const auto& c:commands)n+=c.kind==k;return n;}
};
int main(){
 {const auto replay=[](float intent,float speed,double angular){
    auto tune=melee_tuning_c54::read(4);tune.intentH29=intent;tune.sensitivityH29=speed;melee_tuning_c54::set(4,tune);
    Rig r;for(unsigned i=1;i<=30;++i){double angle=angular*.01*i;r.a.weaponWorld.q={0,0,std::sin(angle*.5),std::cos(angle*.5)};r.a.angular={0,0,angular};r.step();}
    std::uint64_t commit=0;for(const auto& c:r.commands)if(c.kind==p::CommandKind::Slash&&!commit)commit=c.tick;
    return commit;
  };
  const auto slowIntent=replay(.5f,1.f,10),quickIntent=replay(2.f,1.f,10);
  std::printf("intent slow=%llu quick=%llu\n",(unsigned long long)slowIntent,(unsigned long long)quickIntent);
  ck(slowIntent&&quickIntent&&quickIntent<slowIntent,"intent slider changes achieved-motion qualification time at identical physical speed");
  ck(!replay(1.f,.5f,3)&&replay(1.f,2.f,3),"speed slider admits an achieved stroke formerly below speed, independently of intent");
  melee_tuning_c54::set(4,{});
 }

 {Rig r;for(unsigned i=1;i<=15;++i){double angle=.1*i;r.a.weaponWorld.q={0,0,std::sin(angle*.5),std::cos(angle*.5)};r.a.angular={0,0,10};r.step();}
 std::printf("prepare=%u cut=%u thrust=%u reason=%s\n",r.count(p::CommandKind::Prepare),r.count(p::CommandKind::Slash),r.count(p::CommandKind::Thrust),melee118::name(r.out.melee118.reason));
 ck(r.count(p::CommandKind::Prepare)==1&&r.count(p::CommandKind::Slash)==1,"one actual achieved wrist edge prepares/releases");
 std::uint64_t gesture=0;for(const auto& c:r.commands){if(!gesture)gesture=c.gesture59c3;ck(c.gesture59c3==gesture&&gesture&&c.solverH28.body==100&&c.solverH28.sourceSample<=c.sample,"same original gesture with solver receipt");}
 auto before=r.commands.size();for(unsigned n=0;n<4;++n)r.step(false);ck(r.commands.size()==before,"duplicate achieved source cannot replay input");}
 {Rig r;for(int n=0;n<20;++n){r.a.intentFrameH28.trackingFromModel.p.y=.08*n;r.step();}ck(r.commands.empty(),"hand/relative target movement with duplicate achieved source makes no attack");}
 {Rig r;r.a.bodyGeneration++;for(unsigned n=0;n<4;++n)r.step();ck(r.commands.empty(),"incarnation transition cannot inherit attack");}
 {Rig r;for(unsigned n=0;n<20;++n){r.a.weaponWorld.p.y+=.04;r.a.intentFrameH28.trackingFromModel.p.y-=.04;r.step();}
 ck(r.commands.empty(),"common world/body travel absent from original tracking frame cannot supply strike");}
 {Rig r;r.a.intentFrameH28.scale=0;r.step();ck(!p::attack_authority55c1(r.in,r.out,0)&&r.commands.empty(),"invalid original source frame removes typed attack authority");}
 {melee118::State s{};s.ready=true;melee118::Input in{};in.eligible=true;in.dt=.01;
  in.axis=in.modelAxis=in.forward=in.modelForward={0,1,0};in.hand=in.modelHand={0,-.05,0};in.tip=in.modelTip={0,-.05,0};
  melee118::Policy policy{};policy.deliberate119=policy.forwardOnly19=policy.purposefulCuts52=true;
  policy.cutSensitivityH29=policy.stabSensitivityH29=policy.intentSensitivityH29=2;
  bool attack=false;for(unsigned i=0;i<20;++i){auto r=melee118::update(s,in,policy);attack|=r.prepare||r.commit;}
  ck(!attack,"maximum intent and speed sensitivity never promote backward windup into a native attack");
 }
 std::printf("achieved_intent_h28 %u checks %u failures; native acceptance unclaimed\n",checks,failures);return failures?1:0;
}
