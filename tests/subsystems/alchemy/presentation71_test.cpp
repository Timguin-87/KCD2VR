#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
namespace b=kcd2vr::subsystems::alchemy::bridge67;
static kcd2vr::alchemy_tool18::Binding attached71(Fixture& q,int side,int type){
 q.tool(side,type);auto& t=q.in.tool[side];
 t.binding={1,2,888,std::uintptr_t(800+side),t.entity,side,type};t.binding.object=900+side;t.binding.lifetime=3;return t.binding;
}
static void publish71(Fixture& q,const a::Output& o){
 b::enabled.store(true);kcd2vr::minigame_hands45::modeEpoch67.store(q.in.modeEpoch);
 kcd2vr::tracking59c6::epoch.store(q.f.sourceEpoch);b::finish(q.in.nativeFrame.table);b::control.publish(o.control);
 for(int h=0;h<2;++h)b::tools[h].publish(q.in.tool[h]);
}
int main(){
 Fixture q;auto book=attached71(q,0,19);q.selected(19);q.in.nativeA=true;q.in.action.parent=400;
 auto o=q.step();publish71(q,o);
 check(o.passenger&&o.semiControl.manualUse&&!o.admitted&&!o.use);
 check(b::passenger(q.in.now)&&!b::visualPassenger71(q.in.now));
 check(b::presentationOwns71(book,q.in.now,q.f.poseEpoch));
 check(!b::controllerOwns(0,book.entity,q.in.now,book.instance,book.lifetime,q.f.poseEpoch));
 // Native page-turn parent changes do not revoke the current book presentation.
 q.advance();q.in.nativeA=false;q.in.action.parent=401;o=q.step();publish71(q,o);
 check(!o.passenger&&b::presentationOwns71(book,q.in.now,q.f.poseEpoch));
 q.advance();q.in.action.parent=0;o=q.step();publish71(q,o);
 check(b::presentationOwns71(book,q.in.now,q.f.poseEpoch));
 // Actual held mortar/pestle may be moved before the new work lease exists.
 auto mortar=attached71(q,1,2),pestle=attached71(q,0,3);o=q.step();publish71(q,o);
 check(!o.use&&o.gesture.totalSeconds==0&&!o.admitted);
 check(b::presentationOwns71(mortar,q.in.now,4)&&b::presentationOwns71(pestle,q.in.now,4));
 check(!b::presentationOwns71(mortar,q.in.now+81,4));
 auto wrong=mortar;wrong.object++;check(!b::presentationOwns71(wrong,q.in.now,4));
 wrong=mortar;wrong.attachment++;check(!b::presentationOwns71(wrong,q.in.now,4));
 wrong=mortar;wrong.player++;check(!b::presentationOwns71(wrong,q.in.now,4));
 wrong=mortar;wrong.lifetime++;check(!b::presentationOwns71(wrong,q.in.now,4));
 check(!b::presentationOwns71(mortar,q.in.now,5));
 kcd2vr::tracking59c6::epoch.store(6);check(!b::presentationOwns71(mortar,q.in.now,4));kcd2vr::tracking59c6::epoch.store(5);
 auto detached=q.in.tool[1];detached.joined=false;b::tools[1].publish(detached);check(!b::presentationOwns71(mortar,q.in.now,4));
 publish71(q,o);auto c=o.control;c.passenger=true;c.trackedPresentation71=false;b::control.publish(c);
 check(b::passenger(q.in.now)&&b::visualPassenger71(q.in.now)&&!b::presentationOwns71(mortar,q.in.now,4));
 // Explicit native recovery remains native even when A takes the early passenger return.
 q.state.mortarNativeRecovery70=true;q.in.nativeA=true;q.in.action.parent=700;o=q.step();publish71(q,o);
 check(o.passenger&&!o.control.trackedPresentation71&&b::visualPassenger71(q.in.now));
 q.advance();q.in.nativeA=false;o=q.step();publish71(q,o);
 check(o.passenger&&!o.control.trackedPresentation71&&b::visualPassenger71(q.in.now));
 // D39 ordinary Return keeps actual attached tools on tracked wrists in
 // saved Free/6DOF. Native/Semi and all native action/Detach semantics remain.
 for(int mode:{0,1,2,3}){
  kcd2vr::minigame_hands45::setMode(kcd2vr::minigame_hands45::Alchemy,mode);
  Fixture r;r.admit(0,0);const auto bowl=attached71(r,0,0);r.step();r.advance(100);r.step();r.grip(0,0);
  auto returned=r.step();publish71(r,returned);const bool free=mode>=2;
  check(returned.ordinaryReturnRequested&&returned.use&&returned.passenger&&!returned.control.held[0]);
  check(returned.control.trackedPresentation71==free&&b::visualPassenger71(r.in.now)==!free);
  check(b::presentationOwns71(bowl,r.in.now,r.f.poseEpoch)==free);
  r.advance();r.in.action.parent=777;returned=r.step();publish71(r,returned);
  check(returned.passenger&&!returned.ordinaryReturnObserved&&returned.control.trackedPresentation71==free);
  auto changed=r.in.tool[0];changed.binding.object++;b::tools[0].publish(changed);
  check(!b::presentationOwns71(bowl,r.in.now,r.f.poseEpoch));
  changed=r.in.tool[0];changed.joined=false;b::tools[0].publish(changed);
  check(!b::presentationOwns71(bowl,r.in.now,r.f.poseEpoch));
  r.advance(81);r.in.action.parent=0;r.in.tool[0].joined=false;returned=r.step();publish71(r,returned);
  check(returned.ordinaryReturnObserved&&!returned.admitted&&!returned.use&&!b::presentationOwns71(bowl,r.in.now,r.f.poseEpoch));
 }
 // D64: anchored retort operation stays native; FullVR wrists remain tracked.
 for(int mode:{0,1,2,3}){
  kcd2vr::minigame_hands45::setMode(kcd2vr::minigame_hands45::Alchemy,mode);
  Fixture r;r.in.action.parent=777;r.in.nativeFrame.table.actionKnown=true;
  r.in.nativeFrame.table.action=777;r.in.nativeFrame.table.nativeRetortD56=true;
  auto retort=r.step();publish71(r,retort);const bool full=mode>=2;
  check(retort.passenger&&retort.semiControl.manualUse&&!retort.use);
  check(retort.control.trackedPresentation71==full&&b::visualPassenger71(r.in.now)==!full);
  const auto fixture=attached71(r,0,4);publish71(r,retort);
  check(!b::presentationOwns71(fixture,r.in.now,r.f.poseEpoch));
 }
 // Lost input/menu/context never leaves an indefinite tracked presentation grant.
 q.in.qualified=false;o=q.step();publish71(q,o);check(!o.control.enabled&&!b::presentationOwns71(book,q.in.now,4));
 std::printf("presentation71: %u checks PASS (tracked native binding independent of work/Use, cleanup retained)\n",checks);
}
