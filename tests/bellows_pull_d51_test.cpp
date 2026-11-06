// Actual full-alchemy input adapter and actual smith ForgePump. Native recipe,
// heat, animation callbacks and headset motion are not simulated by this test.
#define KCD2VR_INPUT67_NO_MAIN
#include "subsystems/alchemy/input67_test.cpp"
#include "../src/subsystems/smithing/source_grip_h42.h"
#include <cstdlib>
#include <cstring>
#include <string>
namespace bg51=kcd2vr::subsystems::smithing::bellows_geometry_d51;
namespace pump51=kcd2vr::subsystems::smithing::source_h42;
namespace cap51=kcd2vr::diagnostics::capture168;
static unsigned checks51=0;
static void expect51(bool condition,const char* message){
 ++checks51;if(!condition){std::fprintf(stderr,"D51 bellows FAIL: %s\n",message);std::exit(1);}
}
static void alchemy51(Fixture& q,bool working=false){
 // Admission's ordinary native request is distinct from a READY restart.
 // Let its finite 80 ms pulse finish before publishing the work owner.
 q.admit(18,1,false);q.advance(81);auto& t=q.in.nativeFrame.table;
 t.actionKnown=true;t.action=999;t.handTypesKnown68=true;t.handType68[1]=18;
 q.in.action.parent=999;q.in.work={1,999,555,3,q.in.now,44,s::Bellows,2,2,working,!working};
 auto& offer=q.in.nativeFrame.offers[18];offer.boundsValid=true;
 for(int j=0;j<3;++j){offer.bounds.local[j]=-.02f;offer.bounds.local[j+3]=.02f;offer.bounds.world[j*4+j]=1;}
 const auto out=q.step();expect51(out.gesture.enabled&&!out.use,"current left bellows contact establishes motion baseline");
}
static a::Output move51(Fixture& q,float height,unsigned ms=10){
 q.advance(ms);q.f.hand[1].position[1]=height;q.in.world[1].point[2]=height;q.in.palm[1].point.z=height;
 return q.step();
}
struct Forge51 {
 pump51::Input in{};Frame f{};pump51::ForgePump controller; pump51::PumpTraceD42 trace{};
 Forge51(){in.enabled=true;in.now=1000;in.instance=1;in.lifetime=2;in.modeEpoch=3;in.phase=8;
  in.frame.instance=1;in.frame.lifetime=2;in.frame.modeEpoch=3;in.frame.phase=8;in.frame.sequence=1;in.frame.tick=1000;in.frame.player=7;
  auto& src=in.frame.source[2];src.instance=1;src.player=7;src.entity=9;src.holder=10;src.phase=8;src.slot=2;src.side=0;
  src.geometryKnown=true;src.geometryEntity=9;src.geometryType=pump51::Geometry::ForgeHandle;src.slotWorld={0,0,0};
  in.palm[0].valid=true;in.palm[0].point={0,0,0};
  f.allowed=f.headValid=true;f.poseEpoch=4;f.sourceEpoch=5;f.sequence=1;f.timeNs=1000000000;f.tick=1000;
  f.hand[0].valid=true;f.hand[0].rotation[3]=1;
 }
 bool move(float grip,float height,float manual=0,unsigned ms=16){
  in.now+=ms;in.frame.tick=f.tick=in.now;++in.frame.sequence;++f.sequence;f.timeNs+=std::uint64_t(ms)*1000000;
  f.hand[0].grip=grip;f.hand[0].position[1]=height;in.palm[0].grip=grip;in.palm[0].point.z=height;
  return controller.step(in,f,manual,false,&trace);
 }
 void acquire(){expect51(!move(0,0),"smith source baseline has no RT");
  expect51(!move(.8f,0)&&trace.reason==pump51::PumpReasonD42::Acquired,"right Grip at exact handle admits pump without physical RT");}
};
int main(){
 {Fixture q;q.admit(18,1,false);const auto out=q.step();
  expect51(out.use&&out.control.type==18&&!out.gesture.enabled,"initial Grip still requests native LT admission without a downstroke");
  expect51(out.gesture.totalSeconds==0,"initial native admission is not claimed as gesture-earned pumping");}
 {Fixture q;alchemy51(q);auto out=move51(q,.04f);
  expect51(!out.use&&out.gesture.totalSeconds==0,"upward-only READY motion cannot request LT");
  out=move51(q,.08f);expect51(!out.use&&out.gesture.totalSeconds==0,"continued raising cannot request LT");}
 {Fixture q;alchemy51(q);auto out=move51(q,-.025f);
  expect51(out.use&&out.control.type==18&&out.gesture.status==g::Status::ReadyGesture,"2.5 cm downstroke requests type18 native LT");
  expect51(out.gesture.totalSeconds==0,"requesting native pump never fabricates work/heat");
  q.advance();q.grip(1,0);out=q.step();
  expect51(!out.use&&!out.gesture.enabled&&out.gesture.releaseRequested,"open hand revokes pending gesture pulse in same input sample");}
 {Fixture q;alchemy51(q);
  for(int i=0;i<12;++i){auto out=move51(q,-.01f);expect51(!out.use,"subthreshold downstroke is not banked across a return");
   out=move51(q,0);expect51(!out.use,"upstroke resets incomplete READY pull");}
  auto out=move51(q,-.03f);expect51(out.use,"deliberate full pull works after small oscillations");}
 {Fixture q;alchemy51(q);q.in.nativeBellowsD47=true;auto out=move51(q,-.04f);
  expect51(out.passenger&&!out.use&&out.gesture.status==g::Status::NativePassenger,"manual LT owns native bellows instead of competing with gesture");
  expect51(out.gesture.totalSeconds==0,"manual LT has no synthetic gesture clock credit");
  q.advance();q.grip(1,0);out=q.step();expect51(!out.use,"open hand cannot replay a gesture while manual native input owns action");}
 {Fixture q;alchemy51(q,true);auto out=move51(q,-.02f);const auto down=out.gesture.totalSeconds;
  expect51(down>0&&out.gesture.status==g::Status::Pumping,"running native pump accepts deliberate downward movement");
  out=move51(q,0);expect51(out.gesture.totalSeconds>down,"running native pump retains native return-stroke credit");
  const auto returned=out.gesture.totalSeconds;out=move51(q,0);expect51(out.gesture.totalSeconds==returned&&out.gesture.clockStop,"stationary contact stops native gesture clock");
  q.advance();q.grip(1,0);out=q.step();expect51(!out.gesture.enabled&&!out.use,"release ends running pump ownership promptly");}
 {Fixture q;alchemy51(q);q.f.head[1]=-.04f;auto out=move51(q,-.04f);
  expect51(!out.use&&out.gesture.totalSeconds==0,"whole body lowering is not a rope pull");}
 {Fixture q;alchemy51(q);q.in.nativeFrame.offers[18].valid=false;auto out=move51(q,-.04f);
  expect51(!out.use&&out.gesture.clockStop,"missing handle geometry cannot request LT");
  q.in.nativeFrame.offers[18].valid=true;out=move51(q,-.06f);
  expect51(!out.use,"geometry recovery takes a baseline instead of replaying lost motion");
  out=move51(q,-.09f);expect51(out.use,"fresh near-handle pull can resume after valid geometry returns");}
 {Fixture q;alchemy51(q);q.in.nativeFrame.table.handType68[1]=17;auto out=move51(q,-.04f);
  expect51(!out.use&&!out.gesture.enabled,"different native hand member cannot borrow bellows gesture");}
 {Forge51 q;q.acquire();expect51(!q.move(.8f,.04f),"smith upstroke never requests RT");
  expect51(q.move(.8f,0)&&q.trace.reason==pump51::PumpReasonD42::Pumping,"smith right hand downstroke requests native RT");
  expect51(!q.move(.8f,.04f),"return stops RT");expect51(q.move(.8f,0),"second full downstroke repeats RT");
  expect51(!q.move(0,0)&&!q.trace.acquired&&!q.trace.pressed,"smith opening right hand cancels same sample");}
 {Forge51 q;q.acquire();expect51(!q.move(.8f,-.04f,.8f)&&q.trace.reason==pump51::PumpReasonD42::ManualInput,"physical RT owns smith action before gesture");
  expect51(!q.trace.acquired&&!q.trace.pressed,"manual RT clears automatic pump lease");}
 {Forge51 q;q.acquire();q.in.frame.source[2].slotWorld.x=3.1;
  expect51(!q.move(.8f,-.04f)&&!q.trace.acquired,"3.1 m recorded handle mismatch remains rejected instead of enlarging reach");}
 {Forge51 q;q.acquire();q.in.frame.source[2].geometryKnown=false;
  expect51(!q.move(.8f,-.04f)&&!q.trace.acquired,"missing current source cancels smith pump");}
 {bg51::Gate gate;unsigned extraReads=0;
  const auto observe=[&](std::uint64_t tick){if(gate.begin(1,2,1,tick)){++extraReads;return true;}return false;};
  expect51(!observe(1000)&&extraReads==0,"F5 off skips all supplementary geometry reads");
  cap51::gate.select(cap51::Domain::Vrik,true,1000);
  expect51(!observe(1000)&&extraReads==0,"F7 alone does not enable interaction-native diagnostics");
  cap51::gate.select(cap51::Domain::Interaction,true,1000);
  expect51(observe(1000)&&extraReads==1,"finite F5 starts source decomposition");
  expect51(!observe(1499)&&extraReads==1,"supplementary reads are gated before half-second cadence");
  expect51(observe(1500)&&extraReads==2,"F5 heartbeat admits next bounded sample");
  expect51(!observe(181000)&&extraReads==2,"expired finite F5 cannot read or format extra geometry");
  cap51::gate.select(cap51::Domain::Vrik,false,181000);cap51::gate.select(cap51::Domain::Interaction,false,181000);
 }
 {bg51::Evidence e{};e.entity=e.character=e.skeleton=e.attachment=UINTPTR_MAX;e.stage=4;e.joint=21;
  e.entityKnown=e.modelKnown=e.jointKnown=e.relativeKnown=e.rootKnown=e.branchKnown=e.cachedKnown=true;
  for(auto values:{std::pair{e.entityWorld,12},std::pair{e.modelWorld,8},std::pair{e.jointModel,7},std::pair{e.relative,7},
                  std::pair{e.rootModel,7},std::pair{e.branchModel,7},std::pair{e.cachedModel,7},std::pair{e.pointModel,3},std::pair{e.pointWorld,3}})
   for(int i=0;i<values.second;++i)values.first[i]=-3.40282347e38f;
  char line[1900]{};auto n=bg51::formatTransform(line,sizeof(line),"ALCHEMY_LT",UINT64_MAX,UINT64_MAX,UINTPTR_MAX,UINT64_MAX,UINT32_MAX,e);
  expect51(n>0&&n<int(sizeof(line))&&std::strstr(line,"writes=NONE"),"full transform decomposition fits recorder without clipping evidence");
  n=bg51::formatRig(line,sizeof(line),"SMITHING_RT",UINT64_MAX,UINT64_MAX,UINT32_MAX,e);
  expect51(n>0&&n<int(sizeof(line))&&std::strstr(line,"cachedPoseUse=DIAGNOSTIC_COMPARISON_ONLY"),"cached attachment is explicitly comparison evidence only");
  n=bg51::formatOwner(line,sizeof(line),"ALCHEMY_LT",UINT64_MAX,UINT64_MAX,UINTPTR_MAX,UINT64_MAX,UINT32_MAX,e);
  expect51(n>0&&n<int(sizeof(line))&&std::strstr(line,"nativeAction="),"owner identity includes native parent source independently of target position");
 }
 std::printf("bellows_pull_d51: %u checks PASS; actual input/ForgePump, no native outcome or headset claim\n",checks51);
 return 0;
}
