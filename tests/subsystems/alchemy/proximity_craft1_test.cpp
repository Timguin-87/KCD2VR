// Connected crafting policy: actual near selector -> input adapter -> current
// native-selection bridge -> final pad. Engine selection/attachment/clock leaves
// are explicit; no game DLL or fabricated recipe result is used.
#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
#include "../../../src/vrik/semi_workstations46.h"
#include <string>
// Diagnostic sink is an explicit host leaf; the actual public input adapter
// still formats only under its production recorder gates.
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace b1=kcd2vr::subsystems::alchemy::bridge67;
namespace sw1=kcd2vr::semi_workstations46;
static Frame liveFrame1{};
namespace kcd2vr::frik_pose_probe {
bool WorkstationFrame46(workstation_sample46::Frame& out) noexcept {out=liveFrame1;return true;}
}
static unsigned total1=0,bad1=0;
static void expect1(bool v,const char* message){++total1;if(!v){++bad1;std::fprintf(stderr,"FAIL %u: %s\n",total1,message);}}
static void policy1(Fixture& q){
#ifdef KCD2VR_CLOSE_PICKUP_D39
 kcd2vr::subsystems::alchemy::reach_craft1::setPickupReachD39(1);q.in.pickupReachD39=1;
#endif
#ifdef KCD2VR_CRAFT_PROXIMITY_V1
 q.in.proximityCraft1=true;
#endif
 q.in.context.tableGeneration=q.in.context.lifetime;
 for(auto& o:q.in.nativeFrame.offers)o={};
 for(auto& mb:b1::offers)mb.publish({});for(auto& mb:b1::tools)mb.publish({});
 b1::enabled=true;b1::table.publish({});b1::completed.publish({});b1::nomination.publish({});b1::control.publish({});
 kcd2vr::minigame_hands45::modeEpoch67=q.in.modeEpoch;kcd2vr::tracking59c6::epoch=q.f.sourceEpoch;
}
static void box1(Fixture& q,int slot,double centre){
 auto& o=q.in.nativeFrame.offers[slot];o={};o.ref={slot,unsigned(100+slot)};o.valid=o.boundsValid=true;o.tick=q.in.now;
 for(int a=0;a<3;++a){o.bounds.local[a]=-.005f;o.bounds.local[a+3]=.005f;o.bounds.world[a*4+a]=1.f;}
 o.bounds.world[3]=float(centre);o.point={999,999,999}; // this is deliberately NOT the distance source
}
static void palm1(Fixture& q,int side,double x){q.in.palm[side].point={x,0,0};q.in.world[side].point[0]=float(x);}
static void publish1(Fixture& q,const a::Output& out){
 liveFrame1=q.f;b1::table.publish(q.in.nativeFrame.table);b1::nomination.publish(out.nomination);b1::control.publish(out.control);
 for(unsigned i=0;i<n::Capacity;++i){const auto& o=q.in.nativeFrame.offers[i];
  b1::offers[i].publish(o.valid?b1::NativeOffer{q.in.context.instance,q.in.context.player,q.in.context.lifetime,o}:b1::NativeOffer{});}
 for(int h=0;h<2;++h)b1::tools[h].publish(q.in.tool[h]);
}
static int owner1(Fixture& q,const a::Output& out,int slot){
 publish1(q,out);bool nominated=false;int s=b1::selection(q.in.context.player,100u+unsigned(slot),q.in.now,nominated);
 return nominated?s:-1;
}
static unsigned hands1(int t){return (t>=13&&t<=16)||t==18?2u:t==1||(t>=7&&t<=12)?1u:3u;}
static double metres1(int t){return hands1(t)==3u?.10:.20;}
static void refresh1(Fixture& q,unsigned ms=10){q.advance(ms);}
static void roleMatrix1(){
 for(int type=0;type<21;++type)for(int side=0;side<2;++side){
  const bool eligible=(hands1(type)&(1u<<unsigned(side)))!=0;
  for(double gap:{.005,metres1(type)-.001,metres1(type)+.001}){
   Fixture q;policy1(q);box1(q,type,0);palm1(q,side,gap+.005);palm1(q,1-side,3);auto out=q.step();
   bool near=eligible&&gap<metres1(type);
   expect1((out.nomination.decision.source!=n::Source::Gaze)==near,"role-specific hover before Grip; 10/20 cm is surface distance");
   expect1(owner1(q,out,type)==(near?side:-1),"native selection bridge independently agrees with hover region");
   expect1(!out.use&&!out.admitted&&!out.control.held[side],"hover never consumes ingredients or invents an attachment");
   q.grip(side,1);out=q.step();expect1(!out.use,"no synthetic native-selected confirmation");
   refresh1(q);q.selected(type);out=q.step();
   expect1(out.admitted==near&&out.use==(near&&type!=4),"current selected nominee admits; native retort retains physical Hold A");
   expect1(!out.control.held[0]&&!out.control.held[1],"near plus Use cannot mint native tool ownership");
  }
 }
}
static void selectionCases1(){
 {Fixture q;policy1(q);box1(q,14,0);palm1(q,1,.155);palm1(q,0,3);q.grip(1,1);
  auto out=q.step();expect1(owner1(q,out,14)==1,"15cm liquid overrides another gaze target without looking");
  q.selected(6);refresh1(q);out=q.step();expect1(!out.use&&owner1(q,out,14)==1,"wrong final native selection does not trigger a different operation");
  q.selected(14);refresh1(q);out=q.step();expect1(out.use&&out.admitted,"same native target then admits");
  refresh1(q,81);out=q.step();expect1(!out.use,"one release-hint pulse, not continuous Use");}
 {Fixture q;policy1(q);box1(q,0,0);palm1(q,0,.085);palm1(q,1,.025);auto out=q.step();
  expect1(owner1(q,out,0)==1,"nearest eligible central hand wins");
  q.grip(0,1);out=q.step();expect1(owner1(q,out,0)==0,"deliberate Grip outranks idle opposite hover");}
 {Fixture q;policy1(q);box1(q,0,0);palm1(q,0,.075);palm1(q,1,.085);auto out=q.step();
  expect1(owner1(q,out,0)==0,"initial nearest central hover");
  refresh1(q);palm1(q,1,.065);out=q.step();expect1(owner1(q,out,0)==0,"one centimetre tie band does not flicker wrists");
  refresh1(q);palm1(q,1,.025);out=q.step();expect1(owner1(q,out,0)==1,"materially nearer hand can take unlatched hover");}
 {Fixture q;policy1(q);box1(q,14,0);palm1(q,1,.155);palm1(q,0,3);auto out=q.step();
  expect1(owner1(q,out,14)==1,"left near before leaving");
  refresh1(q);palm1(q,1,.206);out=q.step();expect1(out.nomination.decision.source==n::Source::Gaze&&owner1(q,out,14)==-1,"head returns outside20cm, not a larger hidden exit radius");
  refresh1(q);palm1(q,1,.155);out=q.step();expect1(owner1(q,out,14)==1,"reentry is current, no new mode/rung needed");}
 {Fixture q;policy1(q);box1(q,14,0);box1(q,16,1);palm1(q,1,.155);palm1(q,0,3);q.grip(1,1);q.step();
  refresh1(q);palm1(q,1,1.155);auto out=q.step();
  expect1(out.nomination.decision.ref.slot==16,"untransported close Grip may move to a different near target");
  expect1(!out.use,"retarget still requires actual native selected confirmation");}
 for(int fault=0;fault<9;++fault){Fixture q;policy1(q);box1(q,14,0);palm1(q,1,.155);palm1(q,0,3);q.grip(1,1);q.step();refresh1(q);q.selected(14);
  if(fault==0)q.in.qualified=false;if(fault==1)q.in.palm[1].valid=false;if(fault==2)q.in.palm[1].tick=q.in.now-81;
  if(fault==3)q.in.nativeFrame.offers[14].tick=q.in.now-81;if(fault==4)q.in.nativeFrame.table.tick=q.in.now-81;
  if(fault==5)q.in.nativeFrame.table.refs[14].entity++;if(fault==6)q.in.modeEpoch++;if(fault==7)q.in.cancel=true;
  if(fault==8)q.in.nativeFrame.offers[14].bounds.world[0]=0;
  auto out=q.step();expect1(!out.use&&!out.admitted,"stale/replaced/invalid/context loss cannot authorize a grab");}
 // An empty held source cannot monopolize a different hand's real nearby
 // acquisition. Native selection, table bounds, release quarantine and a real
 // new squeeze remain independent requirements; admitted owners stay unchanged.
 for(int type:{14,1})for(int fault=0;fault<8;++fault){Fixture q;policy1(q);const int eligible=type==1?0:1,empty=1-eligible;
  box1(q,type,0);palm1(q,empty,.02);palm1(q,eligible,3);q.grip(empty,1);auto out=q.step();
  expect1(out.nomination.decision.gripSelectionOwnedCraft1&&!n::valid(out.nomination.decision.ref)&&!out.use,
   "empty held hand keeps native head selection suppressed before an eligible opposite squeeze");
  refresh1(q);palm1(q,eligible,.025);q.grip(eligible,1);q.selected(type);
  if(fault==1)q.in.nativeSelectedKnown=false;
  if(fault==2)q.in.nativeFrame.offers[type].tick=q.in.now-81;
  if(fault==3)q.in.nativeFrame.offers[type].bounds.world[0]=0;
  if(fault==4)q.grip(eligible,.45f); // no preceding acquisition edge on this hand
  if(fault==5)q.state.selection.quarantined[eligible]=true;
  if(fault==6)q.in.palm[eligible].valid=false;
  if(fault==7)palm1(q,eligible,.3);
  out=q.step();expect1(out.admitted==(fault==0)&&out.use==(fault==0),
   "only a fresh eligible opposite squeeze with current bounds and native selection escapes empty-hand starvation");
  if(fault==0)expect1(q.state.admittedSide==eligible&&owner1(q,out,type)==eligible,"both closed hands select the actually eligible native source");
 }
 // Consumer sees newer geometry than the copied nomination: old gap is not authority.
 {Fixture q;policy1(q);box1(q,14,0);palm1(q,1,.155);palm1(q,0,3);auto out=q.step();publish1(q,out);
  auto moved=q.in.nativeFrame.offers[14];moved.bounds.world[3]=2;b1::offers[14].publish({1,2,3,moved});
  bool chosen=false;expect1(b1::selection(2,114,q.in.now,chosen)==-1&&!chosen,"current consumer bounds retire old nearby nomination");}
}
static void centralWork1(){
 for(int requested:{2,3})for(int side=0;side<2;++side){Fixture q;policy1(q);box1(q,requested,0);palm1(q,side,.055);palm1(q,1-side,3);
  q.grip(side,1);q.step();refresh1(q);q.selected(requested);auto out=q.step();expect1(out.admitted&&out.use,"either central hand can request native mortar/pestle pair");
  q.in.action.parent=999;q.in.work={1,999,555,3,q.in.now,44,s::Mortar,3,3,true,false};q.tool(0,3);q.tool(1,2);q.grip(0,1);q.grip(1,1);
  q.in.world[0].point[0]=.02f;q.in.world[1].point[0]=0;out=q.step();
  expect1(out.gesture.enabled&&out.control.held[0]&&out.control.held[1],"native left mortar/right pestle retained, not reattached or swapped");}
 // Carafe mechanics still operate at their existing recipient/tilt contract.
 {Fixture q;policy1(q);box1(q,14,0);box1(q,6,.15);palm1(q,1,.055);palm1(q,0,3);q.grip(1,1);q.step();refresh1(q);q.selected(14);q.step();q.resource();q.step();
  refresh1(q);q.f.hand[1].rotation[2]=q.f.hand[1].rotation[3]=.70710678f;auto out=q.step();
  expect1(out.gesture.totalSeconds>0&&out.gesture.status==g::Status::Pouring,"working sustained-tilt oil path remains connected");
  const auto credit=out.gesture.totalSeconds;out=q.step();expect1(out.gesture.totalSeconds==credit,"no second-eye work credit");
  refresh1(q);q.grip(1,0);out=q.step();expect1(out.gesture.releaseRequested&&!out.control.held[1],"resource release retains native return request");}
 // Bellows close enough for20cm selection must also be close enough for its work owner.
 {Fixture q;policy1(q);box1(q,18,0);palm1(q,1,.155);palm1(q,0,3);q.grip(1,1);q.step();refresh1(q);q.selected(18);auto out=q.step();
  expect1(out.admitted&&out.use,"left bellows15cm admits after native selection");
  auto& t=q.in.nativeFrame.table;t.actionKnown=true;t.action=999;t.handTypesKnown68=true;t.handType68[1]=18;
  q.in.action.parent=999;q.in.work={1,999,555,3,q.in.now,44,s::Bellows,3,3,true,false};q.step();
  refresh1(q);q.f.hand[1].position[1]-=.02f;q.in.world[1].point[2]-=.02f;q.in.palm[1].point.z=-.02;
  out=q.step();expect1(out.gesture.enabled&&out.gesture.totalSeconds>0,"same20cm region reaches actual bellows work, not a remaining10cm gate");
  refresh1(q);out=q.step();const auto credit=out.gesture.totalSeconds;refresh1(q);out=q.step();expect1(out.gesture.totalSeconds==credit,"stationary bellows hand earns no work");}
}
static void heldAndFallback1(){
 {Fixture q;policy1(q);box1(q,0,0);palm1(q,0,.055);palm1(q,1,3);q.grip(0,1);q.step();refresh1(q);q.selected(0);q.step();
  q.tool(0,0);auto out=q.step();expect1(out.ordinaryHeld&&out.control.held[0],"actual ordinary attachment grants hold");
  refresh1(q,81);palm1(q,0,2);out=q.step();
  expect1(out.ordinaryHeld&&out.control.held[0]&&out.nomination.decision.source==n::Source::Right&&!n::valid(out.nomination.decision.ref),"held empty source selection does not drop the real carried bowl or borrow a head target");
  refresh1(q);q.in.action.parent=0;out=q.step();expect1(out.ordinaryHeld&&out.admitted,"ending Take does not discard actual carried attachment");}
 {Fixture q;policy1(q);box1(q,2,0);box1(q,0,1);box1(q,6,1.5);palm1(q,1,.055);palm1(q,0,3);q.grip(1,1);q.step();refresh1(q);q.selected(2);q.step();
  q.tool(0,3);q.tool(1,2);q.grip(0,1);
  for(int h=0;h<2;++h){auto& t=q.in.tool[h];t.parent=999;t.binding={1,2,888,std::uintptr_t(800+h),t.entity,h,t.type};
   t.binding.object=900+h;t.binding.lifetime=3;t.binding.action=999;}
  q.in.action.parent=999;q.in.work={1,999,555,3,q.in.now,44,s::Mortar,3,3,true,false};q.step();
  refresh1(q);palm1(q,1,.5);palm1(q,0,.52);q.in.aimH21[1]={{1,0,0},q.in.now,4,5,6,1,true};
  auto out=q.step();expect1(q.state.mortarPair69&&out.control.held[0]&&out.control.held[1],"real pair remains owned away from a destination");
  expect1(out.nomination.decision.source==n::Source::Left&&!n::valid(out.nomination.decision.ref)&&!out.nomination.decision.aimedDestinationH21,"held empty mortar selection grants no distant ray target outside the10cm central zone");
  refresh1(q);palm1(q,1,.95);palm1(q,0,.97);q.selected(0);out=q.step();
  expect1(out.nomination.decision.source==n::Source::Left&&out.nomination.decision.ref.slot==0,"carrying left palm selects central destination when it enters10cm");
  expect1(out.control.held[0]&&out.control.held[1]&&!out.mortarStop69.serial,"mere destination hover does not synthesize a recipe transfer");}
}
static void actualPad1(){
 struct Pad{bool a=false,b=false,x=false;float lx=0,ly=0,rt=0,lt=0;};
 kcd2vr::subsystems::alchemy::reach_craft1::enabled=false; // Saved FREE VRIK must not depend on the trial switch.
 Fixture q;policy1(q);box1(q,14,0);palm1(q,1,.155);palm1(q,0,3);q.grip(1,1);
 sw1::State state;sw1::Context c{};c.enabled=c.qualified=c.alchemyTracked=c.gestureAlchemy67=true;c.alchemy=1;c.lifetime=3;
 auto step=[&](){c.world67=q.in.world;c.worldPlayer67[0]=c.worldPlayer67[1]=9;c.aimH21=q.in.aimH21;
  b1::completed.publish(q.in.nativeFrame);b1::table.publish(q.in.nativeFrame.table);s::work.publish({});
  kcd2vr::alchemy_input19c4::clear();
  if(q.in.nativeSelectedKnown){
   kcd2vr::alchemy_input19c4::observed[q.in.nativeSelected.type].publish(q.in.nativeSelected);
   kcd2vr::alchemy_input19c4::selected.publish({1,3,q.in.now,q.in.nativeSelected.entity,q.in.nativeSelected.type});}
  Pad p;state.apply(c,q.f,q.in.now,p);return p;};
 auto pad=step();expect1(!pad.a&&state.lastAlchemy67.nomination.decision.source==n::Source::Left,"actual public adapter enables20cm policy with recording-independent context");
 refresh1(q);q.selected(14);pad=step();expect1(pad.a&&state.lastAlchemy67.admitted,"actual public adapter publishes the native A request");
 refresh1(q,81);pad=step();expect1(!pad.a,"actual final pad does not repeat the release-hint request");
}
#ifdef KCD2VR_CLOSE_PICKUP_D39
static void closePickupD39(){
 namespace reach=kcd2vr::subsystems::alchemy::reach_craft1;
 for(int layout:{1,-1})for(int type=0;type<21;++type)for(int side=0;side<2;++side){
  const bool eligible=(hands1(type)&(1u<<unsigned(side)))!=0;
  for(double gap:{.005,.029,.031,.15}){
   Fixture q;policy1(q);q.in.pickupReachD39=0;reach::setPickupReachD39(0);
   const double centre=layout*2.;box1(q,type,centre);palm1(q,side,centre+layout*(gap+.005));palm1(q,1-side,8);
   auto out=q.step();const bool near=eligible&&gap<.03;
   expect1(owner1(q,out,type)==(near?side:-1),"close pickup uses current object bounds in standard and mirrored layouts");
   expect1(!out.use&&!out.admitted,"close bounds alone cannot acquire a native object");
   q.grip(side,1);q.step();refresh1(q);q.selected(type);out=q.step();
   const bool selectedNear=eligible&&gap<((type>=13&&type<=16)?metres1(type):.03);
   expect1(out.admitted==selectedNear&&out.use==(selectedNear&&type!=4),"D44 native-selected radii are preserved; native retort keeps physical Hold A");
  }
 }
 {Fixture q;policy1(q);box1(q,14,0);palm1(q,1,.155);palm1(q,0,3);q.grip(1,1);auto old=q.step();publish1(q,old);
  reach::setPickupReachD39(0);bool nominated=false;
  expect1(b1::selection(2,114,q.in.now,nominated)==1&&!nominated,"tightened live policy keeps a held hand's empty selection, not its stale 15cm nominee");
  q.in.pickupReachD39=0;refresh1(q);q.selected(14);auto out=q.step();
  expect1(!out.admitted&&!out.use,"live reach change cancels a pending wider pickup without emitting native Use");
  palm1(q,1,.025);refresh1(q);out=q.step();expect1(!out.admitted,"held grip remains quarantined through an unaccepted policy change");
  q.grip(1,0);q.step();q.grip(1,1);q.step();refresh1(q);q.selected(14);out=q.step();
  expect1(out.admitted&&out.use,"a fresh close grip after live policy change can acquire normally");}
 {Fixture q;policy1(q);box1(q,0,0);box1(q,6,1);palm1(q,0,.025);palm1(q,1,3);q.grip(0,1);q.step();refresh1(q);q.selected(0);q.step();
  q.tool(0,0);q.step();q.in.pickupReachD39=0;reach::setPickupReachD39(0);refresh1(q,81);palm1(q,0,.925);q.selected(6);auto out=q.step();
  expect1(out.admitted&&out.ordinaryHeld&&out.control.held[0],"changing source reach keeps an already attached bowl's ownership");
  expect1(!out.nomination.decision.sourceAcquisitionD39&&owner1(q,out,6)==0,"carried destination retains its existing 10cm radius under close source pickup");
  q.grip(0,0);out=q.step();expect1(out.ordinaryReturnRequested&&out.use,"one physical release at the retained destination requests native return");}
 {Fixture q;policy1(q);q.in.pickupReachD39=0;reach::setPickupReachD39(0);box1(q,18,0);palm1(q,1,.025);palm1(q,0,3);
  q.grip(1,1);q.step();refresh1(q);q.selected(18);q.step();
  auto& t=q.in.nativeFrame.table;t.actionKnown=true;t.action=999;t.handTypesKnown68=true;t.handType68[1]=18;
  q.in.action.parent=999;q.in.work={1,999,555,3,q.in.now,44,s::Bellows,3,3,true,false};q.step();
  refresh1(q);q.f.hand[1].position[1]-=.02f;q.in.world[1].point[2]-=.02f;q.in.palm[1].point.z=-.02;
  auto out=q.step();expect1(out.gesture.enabled&&out.gesture.totalSeconds>0,"close source pickup does not shrink or disable native bellows work contact");}
 // Execute the real copied-input/final-pad adapter at a formerly assisted gap.
 {struct Pad{bool a=false,b=false,x=false;float lx=0,ly=0,rt=0,lt=0;};Fixture q;policy1(q);reach::setPickupReachD39(0);
  box1(q,14,0);palm1(q,1,.155);palm1(q,0,3);q.grip(1,1);sw1::State state;sw1::Context c{};
  c.enabled=c.qualified=c.alchemyTracked=c.gestureAlchemy67=true;c.alchemy=1;c.lifetime=3;
  auto step=[&](){c.world67=q.in.world;c.worldPlayer67[0]=c.worldPlayer67[1]=9;
   b1::completed.publish(q.in.nativeFrame);b1::table.publish(q.in.nativeFrame.table);s::work.publish({});
   kcd2vr::alchemy_input19c4::clear();
   if(q.in.nativeSelectedKnown){kcd2vr::alchemy_input19c4::observed[q.in.nativeSelected.type].publish(q.in.nativeSelected);
    kcd2vr::alchemy_input19c4::selected.publish({1,3,q.in.now,q.in.nativeSelected.entity,q.in.nativeSelected.type});}
   Pad p;state.apply(c,q.f,q.in.now,p);return p;};
  auto p=step();refresh1(q);q.selected(14);p=step();
  expect1(p.a&&state.lastAlchemy67.admitted&&state.lastAlchemy67.nomination.decision.selectedPickupD44,"D44 public final pad accepts same native selected oil within20cm on actual left Grip");
  refresh1(q);palm1(q,1,.025);p=step();refresh1(q);q.selected(14);p=step();
  expect1(p.a&&state.lastAlchemy67.admitted,"same live close policy permits native-confirmed pickup after the actual hand approaches");}
 reach::setPickupReachD39(1);
}
#endif
int main(){
#ifdef KCD2VR_CLOSE_PICKUP_D39
 expect1(kcd2vr::subsystems::alchemy::reach_craft1::getPickupReachD39()==0,"new source defaults to close-bounds pickup");
#endif
 kcd2vr::subsystems::alchemy::reach_craft1::enabled=true;roleMatrix1();selectionCases1();centralWork1();heldAndFallback1();actualPad1();
#ifdef KCD2VR_CLOSE_PICKUP_D39
 closePickupD39();
#endif
 std::printf("CRAFT1 proximity: %u checks, %u failures; actual production adapters, modeled native selection/tool leaves\n",total1,bad1);
 return bad1?1:0;}
