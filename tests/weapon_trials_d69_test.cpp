// Reuse the actual production-kernel fixture, not copied motor logic.
#define main held_d67_reference_main
#include "held_motion_d67_test.cpp"
#undef main
namespace trials=kcd2vr::weapon_trials_d69;
int main(){
 ck(!trials::sameFrame()&&!trials::rigidGrip(),"all weapon experiments default OFF");
 const int old=held_d67_reference_main();
 ck(old==0,"OFF retains every D67 accepted grip/contact/release regression");
 // Deliberately disable the existing anchor option: the experiment must have
 // its own clear-space translation policy while retaining the same angular law.
 cfg::setGripAnchored(false);trials::setRigidGrip(true);
 for(int cls:{4,7})for(bool physical:{false,true}){Fixture f(cls);f.in.physicalCombat177=physical;
  for(int i=0;i<15;++i){f.in.hands[0].p.y+=.008;f.in.hands[0].q=m::from_rotation_vector({0,0,.01*i});f.next();}
  ck(f.out.active[0]&&m::length(f.out.hands[0].p-f.in.hands[0].p)<1e-8,"ON rigid primary position follows CURRENT controller");
  ck(m::angle(f.out.hands[0].q,f.in.hands[0].q)>1e-5,"ON retains angular tip resistance");
  ck(m::length(compose(f.out.weapons[0],f.p[0].grip[0]).p-f.out.hands[0].p)<1e-9,"ON weapon and hand share non-origin primary pivot");
  const auto previous=f.out;auto repeat=step(f.state,f.in,f.p,[](const Profile&,Pose,Pose,bool){return Contact103{};});
  ck(m::length(previous.weapons[0].p-repeat.weapons[0].p)<1e-12&&m::angle(previous.weapons[0].q,repeat.weapons[0].q)<1e-7,"duplicate source does not integrate trial again");
 }
 for(int mode:{0,1}){Fixture f;f.in.contact=true;f.wall=mode==0;f.unknown=mode==1;f.in.hands[0].p.y=.55;f.next();
  ck(f.out.blocked[0]&&f.out.hands[0].p.y<=.4000001,"ON wall/unknown collision still constrains rigid candidate");
  for(int i=0;i<10;++i)f.next();ck(f.out.hands[0].p.y<=.4000001,"ON blocked/rejoin motor never bypasses prior check");
  trials::setRigidGrip(false);f.next();ck(f.out.hands[0].p.y<=.4000001,"OFF transition preserves checked contact owner");trials::setRigidGrip(true);
 }
 for(double deg:{63.115,54.833})release(true,deg,false,true);
 {Fixture f;f.in.hands[0].q=m::from_rotation_vector({0,0,.6});f.next();
  trials::setRigidGrip(false);const auto before=f.out.weapons[0];f.next();
  ck(f.out.active[0]&&!f.state.weapon[0].rigidGripD69&&finite(f.out.weapons[0]),"OFF retires trial motor momentum without losing item");
  ck(m::angle(before.q,f.out.weapons[0].q)<.7&&f.out.commandCount==0,"OFF handover does not synthesize attack or native pose");
 }
 {namespace d=kcd2vr::dynamics177;const auto b=d::body(7,9.6,1.8);const auto a=d::authority(10,10,1,false,false);
  Vec3 pivot{},velocity{},w1{},w2{};Quat q1{},q2{};bool exact=true;
  for(int i=0;i<100;++i){const auto target=m::from_rotation_vector({.01*i,.002*i,.003*i});
   d::advance(b,a,{0,1,0},{0,0,-9.81},{.1*i,0,0},target,{10,0,0},{2,0,0},.004,pivot,q1,velocity,w1);
   d::advanceAngularD69(b,a,{0,1,0},{0,0,-9.81},target,{2,0,0},.004,q2,w2);
   exact=exact&&q1.x==q2.x&&q1.y==q2.y&&q1.z==q2.z&&q1.w==q2.w&&w1.x==w2.x&&w1.y==w2.y&&w1.z==w2.z;
  }ck(exact,"angular-only helper is exactly the accepted angular dynamics arithmetic");
 }
 // Actual-kernel prediction policy transition: injected offset is not a punch/cut.
 for(bool prepared:{false,true}){trials::setRigidGrip(false);cfg::setGripAnchored(true);Fixture f;
  f.in.attacks=f.in.heldMotionAttacks=f.in.coherentMelee119=f.in.naturalCombat117=f.in.forwardOnly19=true;
  f.in.intentTracked[0]=f.in.intentTracked[1]=true;
  for(int i=0;i<8;++i)f.next();
  auto& st=f.state.weapon[0];st.prepared=prepared;st.waitingPrepare44c1=prepared;
  st.linearVelocity={6,0,0};st.angularVelocity={0,0,20};
  st.stroke118.active=true;st.stroke118.kind=kcd2vr::melee118::Kind::Thrust;
  const auto heldKey=st.key;f.in.handPredictionPolicyD69=25;f.in.hands[0].p.y+=.04;f.next();
  ck(f.out.active[0]&&st.key==heldKey&&st.handPredictionPolicyD69==25,"prediction policy preserves current physical item owner");
  ck(f.out.commandCount==0&&st.prepared==prepared,"policy-only offset neither commits nor aborts native preparation");
  ck(prepared||!st.stroke118.active,"policy change retires unentered local nomination");
  ck(m::length(st.linearVelocity)<1e-7&&m::length(st.angularVelocity)<1e-7,"policy change cannot convert target offset into motor feed-forward");
  f.in.handPredictionPolicyD69=0;f.in.hands[0].p.y-=.04;f.next();
  ck(f.out.commandCount==0&&st.prepared==prepared,"prediction OFF handover also retains native preparation without fake attack");
 }
 std::printf("D69 weapon trials %d/%d\n",count-failed,count);return failed?1:0;
}
