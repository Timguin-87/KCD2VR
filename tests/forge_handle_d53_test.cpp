// D52 19:50:53.218 physical rig/palm evidence replay through the actual ForgePump.
// Native pose reading is not executed here; retail/rig qualification remains in native_sources_h42.inl.
#define main priorBellows51Main
#include "bellows_pull_d51_test.cpp"
#undef main
int main(){
 namespace geo=kcd2vr::subsystems::alchemy::geometry68;
 geo::Bounds transform{};
 const float matrix[]{.32606304f,.94534808f,0.f,3185.5271f,-.94534808f,.32606304f,0.f,502.19312f,0.f,0.f,1.f,41.313629f};
 for(int i=0;i<12;++i)transform.world[i]=matrix[i];
 const auto physical=geo::transform(transform,{.6339606,-2.0057979,1.3763232});
 const geo::Point oldSocket{3184.2566,504.06131,42.878574};
 const geo::Point palm{3183.9729,500.90878,42.710503};
 for(bool corrected:{false,true}){
  Forge51 q;q.in.frame.source[2].slotWorld=corrected?physical:oldSocket;q.in.palm[0].point=palm;
  const auto step=[&](float grip,float y,float manual=0.f){
   q.in.now+=16;q.in.frame.tick=q.f.tick=q.in.now;++q.in.frame.sequence;++q.f.sequence;q.f.timeNs+=16000000;
   q.f.hand[0].grip=q.in.palm[0].grip=grip;q.f.hand[0].position[1]=y;q.in.palm[0].point.z=palm.z+y;
   return q.controller.step(q.in,q.f,manual,false,&q.trace);
  };
  expect51(!step(0,0),"recorded source starts a baseline, never a generated RT");
  expect51(!step(.8f,0),"acquisition itself does not count as pumping");
  std::printf("target=%s gap=%.6f acquired=%d\n",corrected?"evaluated_joint":"authored_socket",q.trace.gap,q.trace.acquired);
  expect51(q.trace.acquired==corrected,"physical target accepts recorded hand; old three-metre socket fails");
  expect51(step(.8f,-.04f)==corrected,"deliberate four-centimetre pull requests RT only after true local acquisition");
  expect51(!step(0,-.04f)&&!q.trace.acquired,"release cancels acquisition and native RT request immediately");
 }
 bg51::Evidence e{};e.physicalHandleJointD53=true;char text[1900]{};
 bg51::formatTransform(text,sizeof(text),"SMITHING_RT",1,1,1,1,1,e);
 expect51(std::strstr(text,"target=EVALUATED_HANDLE_JOINT")!=nullptr,"F5 distinguishes physical target from authored socket comparison");
 std::printf("forge_handle_d53: %u checks PASS; recorded geometry plus actual ForgePump, native outcomes unobserved\n",checks51);
}
