#include "vrik/guard_parry176.h"
#include "vrik/physical_guard112.h"
#include "diagnostics/parry_trace176.h"
#include <cstdio>
#include <limits>
namespace gp=kcd2vr::guard_parry176;
namespace gm=kcd2vr::guard_motion168;
namespace pt=kcd2vr::diagnostics::parry176;
static int total=0,failed=0;
static void ck(bool b,const char* s){++total;failed+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",s);}
int main(){
 using V=gp::V;const kcd2vr::pose_core::Quat identity{};const V right{1,0,0};
 namespace sb=kcd2vr::shield_block_r120;
 sb::enabled=false; // R120: the shield checks below are the legacy (switch OFF) rule; the R120 box is checked at the end
 ck(gp::defensiveSector(0)==0&&gp::defensiveSector(1)==2&&gp::defensiveSector(2)==1&&gp::defensiveSector(5)==5,"retail symmetric sectors preserve head/lower and mirror left/right");
 for(int z:{0,1,2,5}){V toward=z==0?V{0,.09,0}:z==5?V{0,-.09,0}:z==1?V{.09,0,0}:V{-.09,0,0};
  ck(gp::towardSector({toward,toward,true},z,identity,right),"raw and relative movement toward native sector qualifies");
  ck(!gp::towardSector({toward*-1,toward*-1,true},z,identity,right),"opposite movement does not qualify");}
 ck(!gp::towardSector({{.09,0,0},{},true},1,identity,right),"common hand/head translation does not qualify");
 ck(!gp::towardSector({{.09,0,0},{.09,0,0},true},3,identity,right),"disabled native sector rejected");
 ck(!gp::towardSector({{.09,0,0},{.09,0,0},true},1,{0,0,0,0},right),"invalid calibration rejected");
 ck(!gp::towardSector({{.09,0,0},{.09,0,0},true},1,identity,{}),"invalid body axes rejected");
 ck(!gp::towardSector({{.09,0,0},{std::numeric_limits<double>::quiet_NaN(),0,0},true},1,identity,right),"nonfinite original tracking rejected");
 gp::State state;gm::Sample s{100,1,1,77,{},{},true};
 auto step=[&](V hand,V head=V{}){s.tick+=10;++s.sequence;s.hand=hand;s.head=head;return state.update(s);};
 for(int i=0;i<15;++i)ck(!step({}).valid,"settle never emits a block edge");
 ck(state.motion.armed,"120ms quiet rearm completes");
 step({.03,0,0});step({.06,0,0});auto move=step({.09,0,0});
 ck(move.valid&&gp::towardSector(move,1,identity,right),"deliberate 9cm motion emits one sector-qualified candidate");
 ck(!state.update(s).valid,"repeated tracking sequence cannot repeat an edge");
 for(int i=0;i<10;++i)step({.09,0,0});ck(!state.motion.armed,"cooldown cannot be bypassed by short settle");
 for(int i=0;i<25;++i)step({.09,0,0});ck(state.motion.armed,"quiet rearm after cooldown");
 // R59.4 trailing-window displacement: a guard hand that never settles still qualifies.
 {gp::State w;gm::Sample ws{500,1,1,77,{},{},true};
  auto wstep=[&](V hand,V head=V{}){ws.tick+=10;++ws.sequence;ws.hand=hand;ws.head=head;w.update(ws);return w.recent();};
  ck(!wstep({}).valid,"single sample has no window displacement");
  for(int i=1;i<=8;++i)wstep({.02*i,0,0});
  auto r=w.recent();
  ck(r.valid&&!w.motion.armed&&gp::towardSector(r,1,identity,right),"continuous 16cm/80ms travel qualifies without a settle");
  const auto count=w.window.count;w.update(ws);w.update(ws);
  ck(w.window.count==count&&w.recent().valid,"duplicate native visits to one XR publication preserve trailing beat history");
  {auto changed=ws;changed.hand.x+=.01;gp::Window corrupted=w.window;corrupted.push(changed);
   ck(!corrupted.recent().valid,"changed coordinates under a reused XR identity invalidate beat history");}
  for(int i=0;i<30;++i)wstep({.16,0,0});
  ck(!gp::towardSector(w.recent(),1,identity,right),"a hand that stopped 160ms ago has no displacement");
  for(int i=1;i<=8;++i)wstep({.16,0,0},{.0,0,.02*i});
  ck(!gp::towardSector(w.recent(),0,identity,right),"head-only travel cannot qualify a window candidate");
  for(int i=1;i<=8;++i)wstep({.16+.02*i,0,0},{.0,0,.16+.02*i});
  ck(!gp::towardSector(w.recent(),1,identity,right),"common hand/head translation cannot qualify a window candidate");
  ws.tick+=200;++ws.sequence;w.update(ws);ck(!w.recent().valid,"tracking gap resets the window");
  for(int i=1;i<=8;++i)wstep({.16+.02*i+.16,0,0});ws.eligible=false;w.update(ws);ck(!w.recent().valid,"tracking loss clears the window");
  ws.eligible=true;for(int i=1;i<=8;++i)wstep({.02*i,0,0});++ws.key;w.update(ws);ck(!w.recent().valid,"weapon transition clears the window");
  for(int i=1;i<=3;++i)wstep({3.*i,0,0});ck(!w.recent().valid,"tracking jump above 8 m/s is not a gesture");}
 s.eligible=false;ck(!state.update(s).valid&&!state.motion.armed,"tracking loss revokes armed candidate");
 s.eligible=true;step({.18,0,0});ck(!state.motion.armed,"tracking recovery needs new settle");
 for(int i=0;i<15;++i)step({.18,0,0});++s.epoch;ck(!state.update(s).valid&&!state.motion.armed,"epoch transition cannot inherit armed gesture");
 for(int i=0;i<15;++i)step({.18,0,0});++s.key;ck(!state.update(s).valid&&!state.motion.armed,"weapon transition cannot inherit armed gesture");
 // Exact shared F4 geometry and producer arbitration, not a mirrored mock.
 gp::Position horizontal{{-.3,.4,.2},{.3,.4,.2},{},{1,0,0}};
 ck(gp::position(horizontal,false),"centered transverse weapon enters front guard");
 auto upright=horizontal;upright.grip={0,.4,-.1};upright.tip={0,.4,.5};
 ck(gp::position(upright,false),"upright blade in front of face qualifies");
 auto shifted=[&](double cm){auto p=horizontal;p.grip.x+=cm*.01;p.tip.x+=cm*.01;return p;};
 auto mirrored=shifted(55);mirrored.grip.x=-mirrored.grip.x;mirrored.tip.x=-mirrored.tip.x;
 ck(!gp::position(shifted(55),false)&&!gp::position(mirrored,false),"H24 tip-only overlap from outside torso centre is not a guard");
 ck(gp::position(shifted(45),false)&&!gp::position(shifted(55),false)&&!gp::position(shifted(-55),false),"H24 frontal central blade coverage requires centre within 48 cm");
 ck(gp::position(shifted(54),true)&&!gp::position(shifted(56),true),"H24 held centre hysteresis ends beyond 55 cm");
 gp::Position shield{{-.42,.35,.10},{},{},{1,0,0},true};
 ck(gp::position(shield,false),"shield grip to the left of the torso qualifies without pretending its centre is the blade");
 shield.grip={-.65,.35,.10};
 ck(!gp::position(shield,false)&&!gp::position(shield,true),"D56 body-distance limit also constrains shield hold band");
 kcd2vr::melee_tuning_c54::guardBodyDistanceD56=.90f;
 ck(!gp::position(shield,false)&&gp::position(shield,true),"explicit longer distance preserves original shield lateral hold hysteresis");
 kcd2vr::melee_tuning_c54::guardBodyDistanceD56=.65f;
 shield.grip.y=-.1;ck(!gp::position(shield,true),"shield behind chest cannot defend");
 gp::Position longBlade{{-.55,.09,-.29},{2.0,.09,.9},{},{1,0,0}};
 ck(!gp::position(longBlade,false),"H24 tip-only shallow overlap of a side-held long blade cannot block");
 auto pointed=horizontal;pointed.grip={0,.2,.2};pointed.tip={0,.8,.2};
 ck(!gp::position(pointed,false),"pointing blade along thrust direction remains attack-capable");
 auto behind=horizontal;behind.grip.y=behind.tip.y=-.3;ck(!gp::position(behind,true),"behind-body weapon releases guard");
 auto low=horizontal;low.grip.z=low.tip.z=-.6;ck(!gp::position(low,true),"lowered weapon releases guard (R45: -50 cm held)");
 auto turned=horizontal;turned.bodyRight={0,1,0};turned.grip={-.4,-.3,.2};turned.tip={-.4,.3,.2};
 ck(gp::position(turned,false),"guard cone follows body axes instead of fixed world axes");
 {gp::Entry e{};ck(gp::enter(e,true,true,1000,5.0,2.5,0,3.5)==false&&gp::enter(e,true,true,1010,1.0,2.5,0,3.5),"R45: a held guard releases above the exit speed and holds below it");
  ck(gp::enter(e,true,true,1020,5.0,2.5,0,0),"R45: exit speed 0 keeps the position-only hold");
  ck(!gp::enter(e,true,false,1030,3.0,2.5,0,3.5)&&gp::enter(e,true,false,1040,2.0,2.5,0,3.5),"R45: entry still needs a slow blade; no dwell by default");}
 // R59.16C10R17 strict guard entry, forward-aware exit and the against-the-attack parry motion.
 {auto cocked=horizontal;cocked.grip={.15,.05,.35};cocked.tip={.15,-.55,.55};
  ck(!gp::position(cocked,false,true),"R17 strict: a sword cocked over the shoulder (tip behind) is not a guard");
  ck(gp::position(horizontal,false,true)&&gp::position(upright,false,true),"R17 strict: the ordinary frontal and upright guards still qualify");
  auto hugging=horizontal;hugging.grip.y=hugging.tip.y=.05;
  ck(!gp::position(hugging,false)&&!gp::position(hugging,false,true)&&!gp::position(hugging,true,false),"H24 frontal distance is mandatory even with stricter motion entry disabled");
  auto heldClose=horizontal;heldClose.grip.y=heldClose.tip.y=.08;
  ck(!gp::position(heldClose,false,true)&&gp::position(heldClose,true,true),"R17 strict: a held guard keeps a looser 6 cm hysteresis");
  gp::Position shieldStrict{{-.42,.35,.10},{},{},{1,0,0},true};ck(gp::position(shieldStrict,false,true),"R17 strict never changes shields");}
 {gp::Entry e{};(void)gp::enter(e,true,false,1000,1.,2.5,0,3.5);
  ck(gp::enter(e,true,true,1010,4.5,2.5,0,3.5,1.0),"R17 a fast sideways/upward parry from the guard keeps the block");
  ck(!gp::enter(e,true,true,1020,2.,2.5,0,3.5,4.0),"R17 the tip moving at the enemy faster than the exit speed leaves the guard");
  ck(gp::enter(e,true,true,1030,7.5,2.5,0,3.5,0.),"H5 fast lateral parry retains guard without forward approach");
  ck(!gp::enter(e,true,true,1040,7.5,2.5,0,3.5,4.),"H5 actual forward approach still exits into attack");}
 {auto mv=[](V v){return gp::Movement{v,v,true};};
  auto a=gp::againstAttack(mv({.04,0,0}),2,identity,right);
  ck(a.ok&&a.lateral==1,"R17 attack on Henry's right: 4 cm to the right qualifies (toward)");
  a=gp::againstAttack(mv({.04,0,0}),1,identity,right);
  ck(a.ok&&a.lateral==-1,"R17 either lateral sign qualifies until a headset log proves the frame (receipt says mirrored)");
  ck(gp::againstAttack(mv({0,.04,0}),0,identity,right).ok,"R17 overhead attack: raising the guard qualifies");
  ck(gp::againstAttack(mv({0,0,-.04}),5,identity,right).ok,"R17 thrust: beating forward qualifies");
  ck(!gp::againstAttack(mv({0,0,.05}),2,identity,right).ok&&!gp::againstAttack(mv({0,0,.05}),0,identity,right).ok,"R17 pulling the guard back is never a parry");
  ck(!gp::againstAttack(mv({0,-.05,0}),0,identity,right).ok,"R17 dropping the guard under an overhead blow is not a parry");
  ck(!gp::againstAttack(mv({.02,0,0}),2,identity,right).ok,"R17 a 2 cm wobble is not a parry");
  ck(!gp::againstAttack(gp::Movement{{.04,0,0},{},true},2,identity,right).ok,"R17 only HMD-relative travel counts (walking/turning with the guard is not a parry)");
  ck(!gp::againstAttack(gp::Movement{{},{.05,0,0},true},2,identity,right).ok,"head-only movement cannot invent an against-weapon beat");
  for(int z:{0,1,2,5})ck(gp::againstAttack(mv({0,0,-.05}),z,identity,right,.04,.50).ok,"forward beat qualifies against each known incoming sector");
  ck(!gp::againstAttack(mv({.04,0,0}),3,identity,right).ok&&!gp::againstAttack(mv({.04,0,0}),2,{0,0,0,0},right).ok,"R17 disabled sector / invalid calibration rejected");}
 {gp::State w;gm::Sample sample{1000,1,1,77,{},{},true};
  const auto next=[&](V p){sample.tick+=10;++sample.sequence;sample.hand=p;w.update(sample);};
  for(int i=0;i<11;++i)next({0,0,.01*i}); // backward recovery first
  for(int i=1;i<=5;++i)next({0,0,.10-.012*i}); // then a 6cm forward beat
  ck(!gp::againstAttack(w.recent(),1,identity,right,.04,.50).ok&&gp::againstRecent(w,1,identity,right,.04,.50).ok,
     "recent forward beat survives earlier opposite recovery in the full trailing window");
 }
 namespace ph=kcd2vr::physical;
 ph::Input in{};in.allowed=true;in.tracked[0]=in.tracked[1]=true;in.epoch=in.bodyEpoch=1;in.sample=1;in.tick=1000;
 in.shoulders[0]={.15,0,1.3};in.shoulders[1]={-.15,0,1.3};in.grip[0]=1;in.naturalCombat117=in.coherentMelee119=in.positionGuard176=true;
 in.dynamics=false;in.contact=false;in.controllerAnchor119=true;in.heldMotionAttacks=true;
 in.hands[0].p={-.3,.4,1.5};in.intentHands[0]=in.hands[0];in.intentTracked[0]=true;
 std::array<ph::Profile,2> profiles{};auto& profile=profiles[0];profile.key=77;profile.generation=1;profile.side=0;profile.classId=4;profile.primary=profile.geometry=true;profile.tip={.6,0,0};profile.axis={1,0,0};profile.guardFaceReadyH24=true;profile.guardFaceNormalH24={0,1,0};
 ph::State producer{};auto clear=[](const ph::Profile&,ph::Pose,ph::Pose,bool){return 1.;};
 auto output=ph::step(producer,in,profiles,clear);
 ck(output.guard117&&output.owned[0],"actual producer uses positional guard on first owned sample");
 bool attack=false;for(int n=0;n<3;++n){++in.sample;in.tick+=10;in.hands[0].p.x+=.03;in.intentHands[0]=in.hands[0];output=ph::step(producer,in,profiles,clear);
  for(unsigned j=0;j<output.commandCount;++j)attack|=output.commands[j].kind==ph::CommandKind::Prepare||output.commands[j].kind==ph::CommandKind::Slash||output.commands[j].kind==ph::CommandKind::Thrust;}
 ck(output.guard117&&!output.strokeActive119&&!attack,"guard counter-motion never publishes an attack first in actual producer");
 {auto sin=in;sin.guardStrict59c17=true;sin.attacks=false;ph::State sp{}; // R59.16C10R17 strict entry through the actual producer
  sin.hands[0].p={-.3,0,1.5};sin.intentHands[0]=sin.hands[0];++sin.sample;sin.tick+=10;auto so=ph::step(sp,sin,profiles,clear);
  ck(!so.guard117,"R17 strict producer: a blade at the chest (not held out in front) does not enter guard");
  bool entered=false;for(int n=0;n<20;++n){++sin.sample;sin.tick+=10;sin.hands[0].p.y+=.02;sin.intentHands[0]=sin.hands[0];so=ph::step(sp,sin,profiles,clear);entered|=so.guard117;}
  ck(!entered&&so.guardForwardVetoes59c17>0,"R17 strict producer: the guard hand moving at the enemy (2 m/s forward) never enters guard");
  for(int n=0;n<12;++n){++sin.sample;sin.tick+=10;so=ph::step(sp,sin,profiles,clear);}
  ck(so.guard117,"R17 strict producer: the same blade held still in front enters guard");
  auto rin=sin;rin.guardStrict59c17=false;ph::State rp{};rin.hands[0].p={-.3,0,1.5};rin.intentHands[0]=rin.hands[0];entered=false;
  for(int n=0;n<20;++n){++rin.sample;rin.tick+=10;rin.hands[0].p.y+=.02;rin.intentHands[0]=rin.hands[0];entered|=ph::step(rp,rin,profiles,clear).guard117;}
  ck(entered,"R17 strict off: the R45 box guard is unchanged");}
 // C19C2: physically identical motion must keep its guard/attack meaning when
 // the tracking-relative family has a different fixed calibration orientation.
 for(bool strictEntry:{false,true})for(double yaw:{0.,1.5707963267948966,-1.5707963267948966,3.141592653589793}){
  for(int motion=0;motion<5;++motion){
   auto qi=in;qi.guardStrict59c17=strictEntry;qi.guardExitSpeed178r45=3.5;
   qi.guardEnterMaxSpeed178r29=strictEntry?1.5:2.5;qi.guardEnterDwellMs178r29=strictEntry?60:0;qi.attacks=true;
   qi.hands[0].p={-.3,.4,1.5};qi.hands[0].q={};qi.intentTracked[0]=true;
   const auto rotation=ph::m::from_rotation_vector({0,0,yaw});
   qi.intentHands[0]={rotation,ph::m::rotate(rotation,qi.hands[0].p)};
   ph::State qs{};ph::Output qo{};
   const auto tick=[&](ph::Vec3 modelDelta,ph::Vec3 relativeDelta){
    ++qi.sample;qi.tick+=10;qi.hands[0].p=qi.hands[0].p+modelDelta;
    qi.intentHands[0].p=qi.intentHands[0].p+ph::m::rotate(rotation,relativeDelta);
    qo=ph::step(qs,qi,profiles,clear);};
   for(int n=0;n<20;++n)tick({},{});
   ck(qo.guard117,"C19C2 identical calibrated forward guard settles in both coordinate families");
   bool leftGuard=false,attack=false;
   for(int n=0;n<3;++n){
    const ph::Vec3 delta=motion==1?ph::Vec3{.04,0,0}:motion==2?ph::Vec3{0,0,.04}:ph::Vec3{0,.04,0};
    tick(motion==4?ph::Vec3{}:delta,motion==3?ph::Vec3{}:delta);
    leftGuard|=!qo.guard117;attack|=qo.strokeActive119;
    for(unsigned j=0;j<qo.commandCount;++j)attack|=qo.commands[j].kind==ph::CommandKind::Prepare||
     qo.commands[j].kind==ph::CommandKind::Slash||qo.commands[j].kind==ph::CommandKind::Thrust;
   }
   if(motion==0)ck(leftGuard&&attack,"C19C2 coherent forward cut leaves guard equally after nonidentity calibration");
   else if(motion<3)ck(!leftGuard&&!attack,"C19C2 lateral/upward counter-parry remains held after calibration");
   else ck(!leftGuard&&!attack,"C19C2 common body or head-only motion cannot leave guard or announce an attack");
  }
 }
 auto nativeInput=in;nativeInput.positionGuard176=false;ph::Pose sideways{{},{.44,.4,1.5}}; // Blade is beyond the F4 torso entry width (R45: 42 cm).
 auto shorter=profile;shorter.tip={.2,0,0};
 ck(ph::natural_guard117(nativeInput,shorter,sideways,false)&&!ph::natural_guard117(in,shorter,sideways,false),"F12 retains its broader historical guard while F4 uses frontal torso coverage");
 ph::Pose vertical{{0,-std::sqrt(.5),0,std::sqrt(.5)},{0,.4,1.3}};
 ck(ph::natural_guard117(in,profile,vertical,false)&&!ph::natural_guard117(nativeInput,profile,vertical,false),"upright F4 guard does not alter baseline orientation policy");
 for(int klass:{8,17}){profiles={};auto& shield=profiles[1];shield.key=88;shield.generation=1;shield.side=1;shield.classId=klass;shield.primary=shield.geometry=true;shield.tip={0,1,0};
  in.hands[1].p={0,.4,1.4};in.grip[1]=1;in.positionGuard176=true;++in.sample;in.tick+=10;producer={};output=ph::step(producer,in,profiles,clear);
  ck(output.owned[1]&&output.guard117,"shield profile publishes raised-grip defense without inferred face orientation");
  ck(!ph::natural_guard117(nativeInput,shield,output.weapons[1],false),"new shield positional policy does not change F12 baseline");
 }
 pt::Clear();pt::Edge edge{},read{};auto generation=pt::publication.load();
 ck(!pt::Publish(edge,generation),"incomplete native edge never publishes");
 edge.tick=1;edge.epoch=4;edge.actor=1;edge.state=2;edge.entity=3;edge.henryId=123;edge.pressReturned=true;edge.cause=pt::Cause::Motion;edge.releaseReturned=true;
 ck(pt::Publish(edge,generation,&read)&&read.sequence&&pt::Read(edge)&&edge.cause==pt::Cause::Motion,"completed native call publishes copied edge identity");
 pt::Clear();ck(!pt::Read(read),"mode transition invalidates completed edge");
 ck(!pt::Publish(edge,generation),"old in-flight native call cannot republish after mode clear");
 ck(pt::Publish(edge,pt::publication.load())&&pt::Read(read)&&read.sequence>edge.sequence,"new generation accepts only a new completed edge");
 {gp::Position face{{-.3,.3,1.7},{.3,.3,1.7},{0,0,1.4},{1,0,0},false};face.head={0,0,1.7};face.faceKnown=true;
 ck(gp::position(face,false),"actual head-relative transverse blade covers face plane");
 auto lowFace=face;lowFace.grip.z=lowFace.tip.z=1.35;ck(!gp::position(lowFace,true),"held chest-height blade cannot remain face guard");
 auto point=face;point.tip={-.3,.9,1.7};ck(!gp::position(point,true),"forward pointed blade leaves the face-parallel block plane");
 }
 // D61 pending next-build cases: no native outcome or input mapping changes.
 {namespace tune=kcd2vr::melee_tuning_c54;
  gp::Position face{{-.3,.3,1.7},{.3,.3,1.7},{0,0,1.4},{1,0,0},false};face.head={0,0,1.7};face.faceKnown=true;
  auto legacy=face;legacy.faceKnown=false;
  const bool legacyBefore=gp::position(legacy,false);
  tune::guardBoxDepthD61=.50f;ck(gp::position(face,false),"D61 default depth retains accepted face coverage");
  tune::guardBoxDepthD61=.10f;ck(!gp::position(face,false)&&!gp::position(face,true),"D61 reduced depth excludes blade beyond box front");
  auto nearFace=face;nearFace.grip.y=nearFace.tip.y=.15;ck(gp::position(nearFace,false),"D61 reduced depth retains nearer valid guard");
  ck(gp::position(legacy,false)==legacyBefore,"D61 does not change missing-head fallback geometry");
  auto shield=face;shield.shield=true;
  ck(gp::position(shield,false),"D72 shield retains its torso posture when head is known and sword face depth is narrow");
  auto shieldUnknownHead=shield;shieldUnknownHead.faceKnown=false;
  ck(gp::position(shield,false)==gp::position(shieldUnknownHead,false),"D72 same shield posture has one rule across head availability");
  auto shieldTorso=shield;shieldTorso.grip={-.42,.25,1.35};shieldTorso.tip={-.42,.25,1.40};
  ck(gp::position(shieldTorso,false),"D72 real torso shield need not intersect the sword face-height box");
  auto shieldLowered=shieldTorso;shieldLowered.grip.z=.8;
  ck(!gp::position(shieldLowered,false),"D72 lowered shield below its torso region remains outside guard");
  auto shieldBehind=shieldTorso;shieldBehind.grip.y=-.1;
  ck(!gp::position(shieldBehind,true),"D72 known-head shield behind the torso remains outside guard");
  tune::guardBoxDepthD61=.80f;ck(gp::position(face,false)&&gp::position(shield,false),"D61 sword depth remains adjustable independently of D72 shield posture");
  tune::guardBodyDistanceD56=.35f;ck(!gp::position(face,false),"D61 depth cannot bypass separate chest-distance cap");
  tune::guardBodyDistanceD56=.65f;tune::guardBoxDepthD61=std::numeric_limits<float>::quiet_NaN();
  ck(gp::position(face,false),"D61 invalid depth falls back to unchanged default");
  tune::guardBoxDepthD61=.50f;
 }
 {// R120 shield block box (VrShieldBlockBoxR120 ON, defaults): chest = shoulder centre at the origin, y forward, z up.
  sb::enabled=true;
  const auto at=[&](V g){return gp::Position{g,{},{},right,true};};
  ck(gp::position(at({-.05,.35,-.05}),false),"R120 raised shield in front of the chest blocks");
  ck(!gp::position(at({-.20,.25,-.42}),false)&&!gp::position(at({-.20,.25,-.42}),true),"R120 shield lowered to the belly is NOT a block (old rule said block)");
  ck(!gp::position(at({-.25,.05,-.55}),false),"R120 shield hanging at the side is not a block");
  ck(!gp::position(at({-.05,.10,-.05}),false),"R120 shield closer than the box front is not a block");
  ck(!gp::position(at({-.05,.35,-.34}),false)&&gp::position(at({-.05,.35,-.34}),true),"R120 4 cm hysteresis keeps a held block, does not start one");
  ck(!gp::position(at({-.45,.35,-.05}),false),"R120 shield far to the left side is outside the box");
  auto headKnown=at({-.05,.35,-.05});headKnown.faceKnown=true;headKnown.head={0,0,.25};
  ck(gp::position(headKnown,false),"R120 head tracking does not move the shield box");
  sb::set(sb::Height,-.30f);ck(gp::position(at({-.20,.25,-.42}),false),"R120 lower box height slider makes a belly shield block");
  sb::set(sb::Height,sb::kSpecs[sb::Height].def);
  sb::set(sb::Forward,.40f);ck(!gp::position(at({-.05,.35,-.05}),false),"R120 forward slider moves the box away from the chest");
  sb::set(sb::Forward,sb::kSpecs[sb::Forward].def);
  sb::values[sb::HalfWidth]=std::numeric_limits<float>::quiet_NaN();ck(sb::get(sb::HalfWidth)==sb::kSpecs[sb::HalfWidth].def,"R120 invalid saved value falls back to default");
  sb::set(sb::HalfWidth,9.f);ck(sb::get(sb::HalfWidth)<=sb::kSpecs[sb::HalfWidth].hi+1e-4f,"R120 slider value is clamped");
  sb::set(sb::HalfWidth,sb::kSpecs[sb::HalfWidth].def);
  ck(gp::position(at({-.05,.35,-.05}),false)&&!gp::position(at({-.05,.70,-.05}),false),"R120 chest-distance limit still applies");
  sb::enabled=false;ck(gp::position(at({-.20,.25,-.42}),false),"R120 switch OFF restores the old wide shield rule");
  sb::enabled=true;
  ck(sb::dwellMs()==100&&sb::afterAttackMs()==350,"R120 default dwell / after-attack");
 }
 std::printf("guard_parry176=%d/%d retail_execution=NONE\n",total-failed,total);return failed?1:0;
}
