// Real input/near arbitration and native-selection source bridge. Native final
// selection and actual tool attachment are explicit leaves, never inferred Use.
#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
#include "../../../src/vrik/semi_workstations46.h"
#include "../../../src/vrik/workstation178r20c1.h"
#include "generated_acquisition_public_h5.inc"
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace b=kcd2vr::subsystems::alchemy::bridge67;
static Frame bridgeFrameH5{};
namespace kcd2vr::frik_pose_probe {
bool WorkstationFrame46(workstation_sample46::Frame& out) noexcept {out=bridgeFrameH5;return true;}
}
static unsigned checksH5=0,failuresH5=0;
static void ckH5(bool value,const char* reason){++checksH5;if(!value){++failuresH5;std::fprintf(stderr,"FAIL %s\n",reason);}}
static void pointH5(Fixture& q,int side,double x){q.in.palm[side].point={x,0,0};q.in.world[side].point[0]=float(x);}
static void offerH5(Fixture& q,int type,double x){auto& o=q.in.nativeFrame.offers[type];o={{type,unsigned(100+type)},{99,99,99},q.in.now,true};o.boundsValid=true;
 for(int i=0;i<3;++i){o.bounds.local[i]=-.005f;o.bounds.local[i+3]=.005f;o.bounds.world[i*4+i]=1;}
 o.bounds.world[3]=float(x);
}
static void beginH5(Fixture& q){q.in.context.tableGeneration=q.in.context.lifetime;
 for(auto& o:q.in.nativeFrame.offers)o={};
 for(auto& slot:b::offers)slot.publish({});
 for(auto& slot:b::tools)slot.publish({});
 b::table.publish({});b::nomination.publish({});b::control.publish({});b::enabled=true;
 kcd2vr::minigame_hands45::modeEpoch67=q.in.modeEpoch;kcd2vr::tracking59c6::epoch=q.f.sourceEpoch;
}
static int nativeSourceH5(Fixture& q,const a::Output& out,int type){
 bridgeFrameH5=q.f;b::table.publish(q.in.nativeFrame.table);b::nomination.publish(out.nomination);b::control.publish(out.control);
 for(unsigned i=0;i<n::Capacity;++i){const auto& o=q.in.nativeFrame.offers[i];
  b::offers[i].publish(o.valid?b::NativeOffer{q.in.context.instance,q.in.context.player,q.in.context.lifetime,o}:b::NativeOffer{});}
 for(int h=0;h<2;++h)b::tools[h].publish(q.in.tool[h]);
 bool nominated=false;const auto side=b::selection(q.in.context.player,unsigned(100+type),q.in.now,nominated);
 return nominated?side:-1;
}
static bool existingPolicyH5(int type,int side){return type>=13&&type<=16?side==1:(type==2||type==18)?side==1:type==3?side==0:side>=0;}
static void seedOldSourceH5(Fixture& q,int side,bool latch){
 n::Completed completed{};completed.context=q.in.context;completed.tick=q.in.nativeFrame.table.tick;completed.sequence=q.in.nativeFrame.table.sequence;
 completed.table=q.in.nativeFrame.table.refs;completed.offers=q.in.nativeFrame.offers;completed.count=n::Capacity;completed.complete=true;
 n::Input input{};input.context=q.in.context;input.now=q.in.now;input.palms=q.in.palm;
 q.grip(side,latch?1.f:0.f);input.palms=q.in.palm;
 const auto original=q.state.selection.step(completed,input); // actual defaultALL generic policy
 ckH5(original.source==(side?n::Source::Left:n::Source::Right)&&original.gripping==latch,"seed existing incompatible source through original generic near selector");
}
int main(){kcd2vr::subsystems::alchemy::reach_craft1::enabled=true;
 for(int type=0;type<int(n::Capacity);++type)for(int side=0;side<2;++side){
  Fixture q;beginH5(q);offerH5(q,type,0);pointH5(q,side,.02);pointH5(q,1-side,1);q.grip(side,1);q.step();q.advance();q.selected(type);
  const auto result=q.step();ckH5(result.admitted==existingPolicyH5(type,side),"actual admission equals the original native hand policy for every slot/hand");
 }
 // Every exact existing hand-restricted acquisition policy, both directions.
 for(int type:{2,3,13,14,15,16,18}){
  Fixture q;beginH5(q);offerH5(q,type,0);const int valid=type==3?0:1,wrong=1-valid;
  pointH5(q,wrong,.02);pointH5(q,valid,1);q.grip(wrong,1);
  auto out=q.step();
  ckH5(!out.admitted&&!out.use,"wrong-hand proximity never emits native action");
  ckH5(out.nomination.decision.source==n::Source::Gaze&&!out.nomination.decision.gripping,"inadmissible source cannot take immutable acquisition latch");
  ckH5(nativeSourceH5(q,out,type)==-1,"actual native-selection bridge remains head-owned for inadmissible source");
  q.advance();pointH5(q,valid,.06);q.grip(valid,1);q.selected(type);out=q.step();
  const auto expected=valid?n::Source::Left:n::Source::Right;
  ckH5(out.nomination.decision.source==expected&&out.nomination.decision.gripping,"valid opposite deliberate Grip wins while wrong Grip remains closed");
  ckH5(out.admitted&&out.use&&q.state.admittedSide==valid,"unchanged actual native-selected witness permits one valid acquisition");
  ckH5(!out.control.held[0]&&!out.control.held[1],"native selection is not invented attachment ownership");
  ckH5(nativeSourceH5(q,out,type)==valid,"actual native-selection bridge follows the valid source");
  q.advance(81);out=q.step();ckH5(out.admitted&&!out.use,"release-hint native input expires without second press");
 }
 // An already retained incompatible hover/latch must be discarded before the
 // early latched return, without quarantining the valid other closed hand.
 for(bool latch:{false,true}){Fixture q;beginH5(q);offerH5(q,14,0);pointH5(q,0,.01);pointH5(q,1,1);seedOldSourceH5(q,0,latch);
  q.advance();q.grip(0,1);pointH5(q,1,.06);q.grip(1,1);q.selected(14);auto result=q.step();
  ckH5(result.admitted&&q.state.admittedSide==1&&!q.state.selection.quarantined[1],"old incompatible hover/latch cannot quarantine or block the eligible closed hand");
  ckH5(nativeSourceH5(q,result,14)==1,"actual native bridge replaces incompatible pending source");
 }
 // A real admitted valid latch cannot be redirected by a different closer Grip.
 for(int side=0;side<2;++side){Fixture q;beginH5(q);const int type=side?14:3;
  q.admit(type,side);offerH5(q,type,0);q.advance();offerH5(q,4,1);pointH5(q,1-side,1);q.grip(1-side,1);
  auto out=q.step();ckH5(out.admitted&&q.state.admittedSide==side&&q.state.admittedRef.slot==type,"already admitted valid owner stays stable");
  ckH5(out.nomination.decision.ref.slot==type,"other hand cannot steal accepted latch");
 }
 // Ineligible Grip cannot override even an idle valid opposite near hover.
 Fixture hover;beginH5(hover);offerH5(hover,14,0);pointH5(hover,0,.01);pointH5(hover,1,.06);hover.grip(0,1);
 auto out=hover.step();ckH5(out.nomination.decision.source==n::Source::Left&&!out.nomination.decision.gripping&&!out.use,"valid idle palm retains near selection over inadmissible Grip");
 ckH5(nativeSourceH5(hover,out,14)==1,"native bridge receives valid idle source, not forbidden closer source");
 // Simultaneous closed hands use the eligible source; release does not invent
 // another action, and leaving existing near bounds restores native gaze.
 Fixture both;beginH5(both);offerH5(both,14,0);pointH5(both,0,.01);pointH5(both,1,.02);both.grip(0,1);both.grip(1,1);
 out=both.step();ckH5(out.nomination.decision.source==n::Source::Left&&out.nomination.decision.gripping&&!out.use,"both Grip uses eligible hand and waits native selection");
 both.advance();both.grip(0,0);both.grip(1,0);out=both.step();ckH5(!out.use&&!out.nomination.decision.gripping,"release creates no action");
 both.advance();pointH5(both,0,1);pointH5(both,1,2);out=both.step();ckH5(nativeSourceH5(both,out,14)==-1,"no near source restores native gaze");
 // Current final selection, topology, context and tracked hands remain gates.
 for(int fault=0;fault<8;++fault){Fixture q;beginH5(q);offerH5(q,14,0);pointH5(q,0,1);pointH5(q,1,.03);q.grip(1,1);q.step();q.advance();q.selected(14);
  if(fault==0)q.in.nativeSelectedKnown=false;
  if(fault==1)++q.in.nativeSelected.entity;
  if(fault==2)++q.in.nativeFrame.table.refs[14].entity;
  if(fault==3)q.in.qualified=false;
  if(fault==4)q.in.palm[1].valid=false;
  if(fault==5)q.f.headValid=false;
  if(fault==6)++q.in.modeEpoch;
  if(fault==7)q.in.nativeFrame.table.tick=q.in.now-81;
  out=q.step();ckH5(!out.admitted&&!out.use&&!out.control.held[0]&&!out.control.held[1],"original selection/source/context/tracking refusal preserved");
 }
 // Physical A remains the explicit native passenger path and revokes nomination.
 Fixture passenger;beginH5(passenger);offerH5(passenger,14,0);pointH5(passenger,1,.02);passenger.grip(1,1);passenger.step();
 passenger.in.nativeA=true;out=passenger.step();ckH5(out.passenger&&!out.admitted&&!out.use,"A passenger unchanged");
 ckH5(nativeSourceH5(passenger,out,14)==-1,"A passenger restores native selection instead of stale hand nomination");
 // Admitted ordinary-return destination nomination is deliberately NOT pickup
 // acquisition: original generic return source is preserved for either hand.
 for(int side=0;side<2;++side){Fixture q;beginH5(q);q.admit(5,side);q.tool(side,5);q.step();q.advance(81);q.step();q.advance();
  pointH5(q,side,1);pointH5(q,1-side,2);offerH5(q,14,1.02);q.selected(14);out=q.step();
  ckH5(out.ordinaryHeld&&out.nomination.decision.ref.slot==14&&!out.nomination.decision.gripping,"already carried ordinary tool retains destination nomination regardless pickup handedness");
  ckH5(nativeSourceH5(q,out,14)==side,"actual source bridge preserves original carrying-hand destination source");
  q.advance();q.grip(side,0);out=q.step();ckH5(out.ordinaryReturnRequested&&out.use&&!out.control.held[side],"original native-selected return request remains once-only");
 }
 // Existing mortar/pestle carrying pair still nominates pot6 with the LEFT
 // carrying palm, not the closer idle RIGHT source. No native effect is inferred.
 Fixture mortar;beginH5(mortar);mortar.admit(2,1);mortar.grip(0,1);mortar.tool(0,3);mortar.tool(1,2);
 for(int h=0;h<2;++h){auto& t=mortar.in.tool[h];t.parent=999;t.binding={1,2,888,std::uintptr_t(800+h),t.entity,h,t.type};t.binding.object=900+h;t.binding.lifetime=3;t.binding.action=999;}
 mortar.in.action.parent=999;mortar.in.work={1,999,555,3,mortar.in.now,44,s::Mortar,3,3,true,false};mortar.step();mortar.advance(81);mortar.step();mortar.advance();
 pointH5(mortar,1,.5);pointH5(mortar,0,2);offerH5(mortar,6,.52);mortar.selected(6);out=mortar.step();
 ckH5(out.admitted&&out.control.held[0]&&out.control.held[1],"real mortar pair remains held");
 ckH5(out.nomination.decision.source==n::Source::Left&&out.nomination.decision.ref.slot==6,"mortar pot destination source unchanged");
 ckH5(nativeSourceH5(mortar,out,6)==1,"actual native bridge preserves mortar pot destination");
 // Actual public AlchemyMode2, no F5 experiment/private enable flag: execute
 // exact current Mod mode/head gate and Context statements, then real final
 // semi_workstations46::State::apply, its mailboxes and native selection bridge.
 struct PadH5 {bool a=false,b=false,x=false,lb=false,rb=false;float lx=0,ly=0,rt=0,lt=0;};
 // CRAFT1: public source roles are now bilateral for the central pestle.
 // Use the still-right-only hourglass as this wrong-hand negative control;
 // proximity_craft1 tests every slot/hand, including bilateral central entry.
 // Exercise both the close source policy and the explicit assisted option.
 // Empty held hands still own an empty selection, but may not starve a fresh
 // eligible opposite Grip before any native operation has been admitted.
#ifdef KCD2VR_CLOSE_PICKUP_D39
 for(int pickupMode:{0,1})
#else
 for(int pickupMode:{1})
#endif
 for(int type:{14,1}){Fixture q;beginH5(q);offerH5(q,type,0);const int valid=type==1?0:1,wrong=1-valid;
#ifdef KCD2VR_CLOSE_PICKUP_D39
  kcd2vr::subsystems::alchemy::reach_craft1::setPickupReachD39(pickupMode);
#endif
  pointH5(q,wrong,.02);pointH5(q,valid,1);q.grip(wrong,1);
  kcd2vr::combat_rules178r10::workstationEnabled=true;kcd2vr::combat_rules178r10::f5Rung=0;
  kcd2vr::workstation178r20c1::setExperiment36(false);
  kcd2vr::minigame_hands45::setMode(kcd2vr::minigame_hands45::Alchemy,kcd2vr::minigame_hands45::Gesture);
  kcd2vr::minigame_hands45::publish(kcd2vr::minigame_hands45::Alchemy,q.in.now);
  q.in.modeEpoch=kcd2vr::minigame_hands45::modeEpoch67.load();q.in.context.generation=q.in.modeEpoch;
  for(auto& palm:q.in.palm)palm.generation=q.in.modeEpoch;
  kcd2vr::alchemy_input19c4::clear();kcd2vr::alchemy_semi46::work.publish({});kcd2vr::alchemy_semi46::producer.reset();
  kcd2vr::semi_workstations46::State app;
  const auto apply=[&](PadH5& pad){
   kcd2vr::minigame_hands45::publish(kcd2vr::minigame_hands45::Alchemy,q.in.now);
   b::enabled=false; // public producer must derive admission, never inherit fixture enable
   const auto gates=PublicAlchemySelectionH5(true);
   ckH5(gates.gesture&&gates.head&&!gates.legacyPointer&&b::enabled.load(),"public mode2 automatically enables only current gesture producer and keeps head fallback");
   ckH5(kcd2vr::combat_rules178r10::f5Rung.load()==0&&!kcd2vr::workstation178r20c1::experimentArmed36.load(),"actual public producer runs with F5 off and experiment unarmed");
   b::completed.publish(q.in.nativeFrame);b::table.publish(q.in.nativeFrame.table);bridgeFrameH5=q.f;
   for(unsigned i=0;i<n::Capacity;++i){const auto& o=q.in.nativeFrame.offers[i];b::offers[i].publish(o.valid?b::NativeOffer{1,2,3,o}:b::NativeOffer{});}
   kcd2vr::semi_workstations46::Context c{};c.alchemy=1;c.lifetime=3;c.qualified=c.alchemyTracked=true;
   c.world67=q.in.world;for(auto& player:c.worldPlayer67)player=9;
   PublicAlchemyContextH5(c,q.in.now);app.apply(c,q.f,q.in.now,pad);
  };
  PadH5 pad;apply(pad);bool nominated=false;
  ckH5(!pad.a&&!app.lastAlchemy67.admitted&&b::selection(2,unsigned(100+type),q.in.now,nominated)==wrong&&!nominated&&
   !n::valid(app.lastAlchemy67.nomination.decision.ref),"actual public held wrong hand keeps an empty selection without gaze pickup or A");
  q.advance();pointH5(q,valid,pickupMode==0?.02:.155);q.grip(valid,1);q.selected(type);
  kcd2vr::alchemy_input19c4::observed[type].publish(q.in.nativeSelected);
  kcd2vr::alchemy_input19c4::selected.publish({1,3,q.in.now,unsigned(100+type)});
  pad={};apply(pad);nominated=false;
  ckH5(pad.a&&app.lastAlchemy67.admitted&&app.alchemy67.admittedSide==valid,"actual public mode2 apply emits native input only after matching native-selected witness for eligible opposite hand");
  ckH5(b::selection(2,unsigned(100+type),q.in.now,nominated)==valid&&nominated,"actual public caller nomination reaches actual native selection source filter");
 }
 for(int mode:{kcd2vr::minigame_hands45::Native,kcd2vr::minigame_hands45::SemiPassenger}){
  Fixture q;beginH5(q);kcd2vr::combat_rules178r10::f5Rung=0;kcd2vr::workstation178r20c1::setExperiment36(false);
  kcd2vr::minigame_hands45::setMode(kcd2vr::minigame_hands45::Alchemy,mode);
  kcd2vr::minigame_hands45::publish(kcd2vr::minigame_hands45::Alchemy,q.in.now);
  const auto gates=PublicAlchemySelectionH5(true);
  ckH5(!gates.gesture&&gates.head&&!gates.legacyPointer&&!b::enabled.load(),"public Native/Semi keeps head selection, no hidden gesture nomination");
  kcd2vr::semi_workstations46::Context c{};c.alchemy=1;c.lifetime=3;c.qualified=c.alchemyTracked=true;PublicAlchemyContextH5(c,q.in.now);
  PadH5 pad;pad.a=true;kcd2vr::semi_workstations46::State app;app.apply(c,q.f,q.in.now,pad);
  ckH5(pad.a&&!app.lastAlchemy67.admitted&&!app.lastAlchemy67.use,"public Native/Semi manual native A survives unchanged");
 }
 std::printf("ALCHEMY_ACQUISITION_H5 checks=%u failures=%u\n",checksH5,failuresH5);return failuresH5?1:0;
}
