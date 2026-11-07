// H1 qualification through the actual producer/recognizer. No native game call.
#define main existing_two_hand_intent63_main_h1
#include "two_hand_intent63_test.cpp"
#undef main
#include <memory>

static void resetCountsH1(Lever63& r){r.prepares=r.commits=0;}
static void primaryFrameH1(Lever63& r,unsigned n,double handStep=.005,unsigned samples=48){
 const double angle=-.96+.96*n/samples;
 r.i.hands[0].q=m::from_rotation_vector({0,0,angle});r.i.hands[0].p.y+=handStep;
 r.i.hands[1].p=r.i.hands[0].p+m::rotate(r.i.hands[0].q,r.p[0].grip[1].p);r.next();
}
static void primaryArcH1(Lever63& r,double handStep=.005,unsigned samples=48){
 for(unsigned n=1;n<=samples;++n)primaryFrameH1(r,n,handStep,samples);
}
static void primaryStabH1(Lever63& r){
 r.i.hands[0].q={};for(unsigned n=0;n<15;++n)r.next();resetCountsH1(r);
 for(unsigned n=0;n<18;++n){r.i.hands[0].p.y+=.02;r.next();}
}
struct IntentH1 {
 kcd2vr::melee118::State state{};kcd2vr::melee118::Input input{};kcd2vr::melee118::Policy policy{};
 unsigned prepares=0,commits=0,firstCommit=0;
 IntentH1(bool heavy,bool two,bool guard=true){state.ready=true;
  input.dt=.01;input.eligible=input.known=true;input.outward=.4;input.guardedOrigin59=guard;
  input.axis=input.modelAxis={0,0,1};input.forward=input.modelForward={0,1,0};
  policy.deliberate119=policy.handRearm178=policy.responsive44c2=policy.forwardOnly19=true;
  policy.purposefulCuts52=policy.deliberateCut42=true;policy.heavy60=heavy;policy.twoHand60=two;}
 void sample(unsigned n){const auto o=kcd2vr::melee118::update(state,input,policy);
  prepares+=o.prepare;commits+=o.commit;if(o.commit&&!firstCommit)firstCommit=n;}
};
int main(){
 // Real native-profile halberd latch: the support hand supplies .6m/s lever
 // motion in both families, below the ordinary .8m/s guarded-hand threshold.
 for(bool direct:{false,true}){auto r=std::make_unique<Lever63>();r->i.motionOwned59c5=direct;
  r->s.weapon[0].guardExitTick32=r->i.tick;r->arc(false,.96,48);
  ck(r->o.support[0]&&r->prepares==1&&r->commits==1,"slow deliberate two-hand halberd lever is one actual producer attempt");}
 {auto r=std::make_unique<Lever63>();r->p[0].classId=4;
  r->s.weapon[0].guardExitTick32=r->i.tick;r->arc(false,.96,48);
  ck(!r->o.guard117&&r->prepares==1&&r->commits==1,"H27 sword edge sweep after guard release keeps C54 admission without the later hand-speed veto");}
 {
  IntentH1 one(true,false);one.input.tip=one.input.modelTip={.04,.02,0};one.input.hand=one.input.modelHand={0,.005,0};
  for(unsigned n=1;n<=25;++n)one.sample(n);
  ck(one.commits==0,"one-hand heavy slow guarded motion keeps the old hand-speed floor");
  IntentH1 two(true,true);two.input=one.input;for(unsigned n=1;n<=25;++n)two.sample(n);
  ck(two.prepares==1&&two.commits==1,"two-hand heavy guard cut earns10cm translated hand proof and one release");
  IntentH1 flick(true,true);flick.input.tip=flick.input.modelTip={.04,.02,0};
  for(unsigned n=1;n<=25;++n)flick.sample(n);
  ck(flick.prepares==0&&flick.commits==0,"fast tip with fixed guarded hands is still only guard adjustment");
  IntentH1 backwards(true,true);backwards.input.axis=backwards.input.modelAxis={0,1,0};
  backwards.input.tip=backwards.input.modelTip=backwards.input.hand=backwards.input.modelHand={0,-.03,0};
  for(unsigned n=1;n<=25;++n)backwards.sample(n);
  ck(backwards.prepares==0&&backwards.commits==0,"heavy backward recovery cannot become an attack");
  IntentH1 stab(true,false);stab.input.axis=stab.input.modelAxis={0,1,0};
  stab.input.tip=stab.input.modelTip=stab.input.hand=stab.input.modelHand={0,.02,0};
  for(unsigned n=1;n<=18;++n)stab.sample(n);
  ck(stab.prepares==1&&stab.commits==1&&stab.firstCommit==10,"heavy forward stab earns the longer .195m free-space travel");
  IntentH1 stale(true,true);stale.input=two.input;stale.sample(1);stale.input.dt=.121;stale.sample(2);
  ck(stale.commits==0&&!stale.state.active,"stale heavy sample cannot carry a candidate through the gap");
 }
 // A new latch or a recovered retained latch must not bless old one-hand
 // movement retroactively. Continuous support is a candidate credential.
 for(bool regain:{false,true}){IntentH1 changing(true,regain);
  changing.input.tip=changing.input.modelTip={.04,.02,0};changing.input.hand=changing.input.modelHand={0,.005,0};
  for(unsigned n=1;n<=20;++n){if(regain&&n==10)changing.policy.twoHand60=false;
   if(regain&&n==11)changing.policy.twoHand60=true;changing.sample(n);}
  ck(changing.state.active&&!changing.state.prepared,"pre-latch slow guarded motion remains an unprepared candidate");
  changing.policy.twoHand60=true;for(unsigned n=21;n<=25;++n)changing.sample(n);
  ck(changing.prepares==0&&changing.commits==0&&!changing.state.twoHand60,"late or recovered support cannot spend old one-hand motion at the lower floor");
  changing.input.hand=changing.input.modelHand={0,.009,0};changing.sample(26);
  ck(changing.prepares==1&&changing.commits==1,"same candidate can still earn the ordinary one-hand hand-speed proof");
 }
 for(int transition=0;transition<3;++transition){auto r=std::make_unique<Lever63>();
  if(transition==0){r->i.grip[1]=0;for(unsigned n=0;n<15;++n)r->next();}
  resetCountsH1(*r);r->s.weapon[0].guardExitTick32=r->i.tick;bool activeBefore=false,stableAfter=false;
  for(unsigned n=1;n<=48;++n){
   if(transition==0&&n==20)r->i.grip[1]=1;
   if(transition==1&&n==5)r->i.reach[1]=.08;
   if(transition==1&&n==20)r->i.reach[1]=1.4;
   if(transition==2&&n==5)r->i.intentTracked[1]=false;
   if(transition==2&&n==20)r->i.intentTracked[1]=true;
   primaryFrameH1(*r,n);if(n==18)activeBefore=r->s.weapon[0].stroke118.active;
   if(n>=30)stableAfter=stableAfter||r->s.weapon[0].supportIntent63;
  }
  std::printf("H27 support transition=%d activeBefore=%d stableAfter=%d prepares=%u commits=%u\n",transition,activeBefore,stableAfter,r->prepares,r->commits);
  ck(stableAfter&&r->commits==1,"H27 actual postguard edge earns its single-hand C54 cut across support acquisition/recovery");
  ck(r->prepares==1&&r->commits==1,"H27 support transition cannot mint a second native attempt");
 }
 // Retail class flags are not replaced: axes/maces remain one-handed here.
 for(int cls:{0,1,3,4,5,7,16}){auto r=std::make_unique<Lever63>();r->i.grip[1]=0;
  r->p[0].classId=cls;r->p[0].twoHanded=cls==4||cls==7;++r->p[0].generation;
  for(unsigned n=0;n<15;++n)r->next();resetCountsH1(*r);r->s.weapon[0].guardExitTick32=r->i.tick;
  primaryArcH1(*r,.012);
  ck(cls==0?(r->prepares==0&&r->commits==0):(r->prepares==1&&r->commits==1),"actual class preserves its existing ordinary-melee eligibility for a deliberate cut");
  ck(!r->o.support[0],"recognizer policy does not invent an unsupported offhand grip");
  ck(r->s.weapon[0].stroke118.heavy60==(cls==7),"producer uses polearm recognition only for class7; axe/mace retain ordinary travel");
  primaryStabH1(*r);
  ck(cls==0?(r->prepares==0&&r->commits==0):(r->prepares==1&&r->commits==1),"actual class preserves deliberate forward-thrust recognition");
 }
 // Short checked actor contacts retain their existing native-input path; the
 // longer free-space heavy travel must not force a blade through the actor.
 for(bool direct:{false,true})for(bool known:{false,true}){IntentH1 shortStab(true,false);
  shortStab.policy.motionOwned59c5=direct;shortStab.input.axis=shortStab.input.modelAxis={0,1,0};
  shortStab.input.tip=shortStab.input.modelTip=shortStab.input.hand=shortStab.input.modelHand={0,.015,0};
  for(unsigned n=1;n<=3;++n){if(n==3){shortStab.input.blocked=true;shortStab.input.known=known;
    shortStab.input.contactEntity175=42;shortStab.input.contactFraction175=.5;}shortStab.sample(n);}
  ck(shortStab.commits==(known?1u:0u),"heavy short checked-contact thrust keeps admission and refuses unknown contact");}
 // Retained/dormant support is identity, not the new low-speed credential.
 for(int loss=0;loss<3;++loss){auto r=std::make_unique<Lever63>();
  if(loss==0)r->i.reach[1]=.08;else if(loss==1)r->i.intentTracked[1]=false;else r->i.grip[1]=0;
  r->next();resetCountsH1(*r);r->s.weapon[0].guardExitTick32=r->i.tick;primaryArcH1(*r);
  ck(!r->o.guard117&&r->prepares==1&&r->commits==1,"H27 postguard edge may earn its own C54 single-hand cut without borrowing dormant support");
  ck(!r->s.weapon[0].stroke118.twoHand60&&!r->s.weapon[0].guardStroke36.twoHand60,"producer requires stable usable support in both intent families");}
 {auto r=std::make_unique<Lever63>();r->s.weapon[0].guardExitTick32=r->i.tick;r->arc(false,.08,4);
  ck(r->prepares==0&&r->commits==0,"short guarded support wrist flick remains excluded");}
 {IntentH1 rest(true,true);rest.input.blocked=true;rest.input.contactEntity175=0;
  rest.input.tip=rest.input.modelTip={.04,.02,0};rest.input.hand=rest.input.modelHand={0,.005,0};
  for(unsigned n=1;n<=25;++n)rest.sample(n);
  ck(rest.prepares==0&&rest.commits==0,"resting world-contact pressure cannot create a new heavy gesture");}
 std::printf("heavy_recognition_h1=%u/%u native_execution=NONE\n",checks-failed,checks);return failed?1:0;
}
