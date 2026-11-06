#pragma once
// Pure G1 host adapter. Native selection/hold hints, native work identity and
// actual attachment joins are INPUT evidence. This file reads no mailbox and
// calls no native action. The host publishes Output only after its own context
// recheck; use is an input request, never a claim of native completion.
#include "bridge67.h"
#include "instant_native_d72.h"
#include "mortar_handoff69.h"
#include "mortar_pour70.h"
#include "mortar_contact_repair.h"
#include "carafe_pour_d85.h"
#include "../../vrik/alchemy_input19c4.h"
#include "../../vrik/alchemy_semi46.h"
#include "../../vrik/workstation_sample46.h"

namespace kcd2vr::subsystems::alchemy::input67 {
namespace prox67=near67;
namespace bridge=bridge67;
namespace gesture=gesture67;
namespace semi=::kcd2vr::alchemy_semi46;
namespace native=::kcd2vr::alchemy_input19c4;
struct NativeAction {
 std::uintptr_t instance=0,parent=0;std::uint64_t lifetime=0,tick=0;bool valid=false,placeMortar69=false;
}; // a valid current zero parent proves end; unknown is never an end
struct Input {
 bool enabled=false,qualified=false,nativeA=false,cancel=false,nativeReturnAuthorized=false,nativeSelectedKnown=false;
 bool nativeBellowsD47=false; // physical LT/alch_use_bag, never a generated pulse
 bool proximityCraft1=false; // enabled by the live Free-Alchemy adapter only
 bool handsOnlyD53=false; // current SixDofVrik: hands select within 20cm, including an empty open palm
 bool palmRayD70=false; // host enables the current mode3 directional selection policy
 bool gestureTransferD59=false; // opt-in: retain one local herb/plate intent through native pickup/selection handoff
 int pickupReachD39=int(reach_craft1::PickupReachD39::Assisted); // explicit copied source policy
 std::uint64_t now=0,modeEpoch=0,currentNativeSequence=0,nativeSelectionTick=0;
 prox67::Context context{};bridge::NativeFrame nativeFrame{};
 std::array<prox67::Palm,2> palm{};std::array<prox67::AimH21,2> aimH21{};std::array<gesture::WorldPalm,2> world{};
 std::array<bridge::NativeTool,2> tool{};
 bridge::PestlePresentationD47 pestlePresentationD47{};
 mortar69::Result mortarResult69{};bool controllerMortar70=false;
 native::Observation nativeSelected{};NativeAction action{};semi::WorkWitness work{};
 // R8b: native selected TYPE when the completed frame selected no entity (alchemy_input19c4::selectedTypeOnly), 21 = none.
 // only type 18 (the mounted bellows: C37 selectedType=18 selectedEntity=0) is mapped to its table member.
 unsigned nativeSelectedTypeOnly=21;std::uint64_t nativeTypeOnlyTick=0;
};
struct Output {
 bridge::Nomination nomination{};bridge::Control control{};semi::Control semiControl{};
 gesture::Output gesture{};bool use=false,passenger=false,admitted=false;
 bool ordinaryHeld=false,ordinaryReturnRequested=false,ordinaryReturnObserved=false;
 bool logicalIngredientHeldD47=false; // native hand membership, never a physical attachment
 // F5 copied-state receipt: 0 off/idle, 1 attachment pending, 2 native pickup
 // busy, 3 selected recipient pending, 4 one native request, 5 cancelled.
 unsigned transferStateD59=0;
 bool unacquiredRetiredD61=false; // released request; current native idle/empty evidence, not a completion
 mortar69::Request mortarStop69{};bool mortarPlaced69=false;
};
inline bool actionCurrent(const Input& in) noexcept {
 return in.action.valid&&in.action.instance==in.context.instance&&in.action.lifetime==in.context.lifetime&&prox67::fresh(in.action.tick,in.now);
}
inline bool workCurrent(const Input& in) noexcept {
 return in.work.instance==in.context.instance&&in.work.lifetime==in.context.lifetime&&in.work.parent&&prox67::fresh(in.work.tick,in.now);
}
// The existing native acquisition hand policy, shared before nomination and at
// final admission. It never restricts an admitted tool's destination semantics.
inline constexpr bool sourceHandAllowedH5(int type,int side) noexcept {
 return (type>=13&&type<=16)?side==1:(type==2||type==18)?side==1:type==3?side==0:side>=0;
}
inline constexpr std::uint32_t sourceMaskH5(int side) noexcept {
 std::uint32_t mask=0;for(unsigned type=0;type<prox67::Capacity;++type)if(sourceHandAllowedH5(int(type),side))mask|=1u<<type;return mask;
}
struct State {
 prox67::State selection{};gesture::Controller work{};prox67::Context context{};
 std::array<prox67::Ref,prox67::Capacity> admittedTable{};
 prox67::Ref admittedRef{};int admittedSide=-1,handSelectionSide=-1;std::uint64_t admittedTick=0,useUntil=0,gripTick=0;
 std::uint64_t unacquiredReleaseTickD61=0;
 bool admitted=false,holdHint=false,physicalA=false,pendingA=false,releasedAdmission=false;std::uintptr_t passengerParent=0;
 bool admittedViaGazeD44=false;int gazeSideD44=-1;std::uint64_t gazeYieldTickD44=0;
 int pickupReachD39=int(reach_craft1::PickupReachD39::Assisted);bool pickupPolicySeenD39=false;
 // A release during delayed attachment may be reclaimed without another A.
 // A replaced child/attachment or observed effect/return closes only this resume.
 bool resourceResumeClosedR17=false;std::uintptr_t resourceChildR17=0;std::uint32_t resourceTokenR17=0;
 alchemy_tool18::Binding resourceBindingR17{};
 std::uint64_t passengerTick=0;std::uintptr_t operationParent=0,admissionBaseline=0;
 float referenceRotation[4]{0,0,0,1};bool referenceRotationValid=false;
 bool plateAdmissionR93=false;alchemy_tool18::Binding plateBindingR93{};
 bool ordinaryCarried=false,ordinaryWasHeld=false,ordinaryReleasePending=false,ordinaryQueued=false,ordinaryReturning=false,previousUse=false;
 bool ordinaryTiltIntentD56=false,ordinaryTiltUsedD56=false,mortarTiltToDishD56=false;
 bool ordinaryTransferD59=false,ordinaryTransferTiltD59=false;
 std::uint64_t ordinaryIntentTickD59=0;
 prox67::Ref ordinaryDestination{};std::uint64_t ordinaryReturnTick=0,ordinaryUseUntil=0;
 std::uintptr_t ordinaryBaseline=0,ordinaryReturnParent=0;
 bool mortarPair69=false,mortarLeft69=false,mortarRightGripD39=false,mortarReleasePending69=false,mortarReleaseIntent70=false,
      mortarStopping69=false,mortarReturning69=false,mortarAttempted69=false;
 bool mortarRetryNoEntry72=false,mortarRetryOpen72=false,mortarContentsTransferredD75=false;
 std::uint64_t mortarSerial69=0,mortarUseUntil69=0,mortarReturnTick69=0;
 std::uintptr_t mortarReturnParent69=0,mortarParent69=0,mortarChild69=0;
 mortar69::Request mortarRequest69{};alchemy_tool18::Binding mortarLeftBinding69{},mortarRightBinding69{};prox67::Ref mortarDestination69{};
 prox67::Ref lastAimDestinationH21{};
 float mortarNeutral69[4]{0,0,0,1};pour70::Controller mortarPour70;pour70::Home mortarHome70;bool mortarNativeRecovery70=false;
 geometry68::Bounds carafeHomeD85{};bool carafeHomeValidD85=false; // D85 craft: the carafe's own native bounds at admission
 void clearOrdinary() noexcept {
  plateAdmissionR93=false;plateBindingR93={};
  ordinaryCarried=ordinaryWasHeld=ordinaryReleasePending=ordinaryQueued=ordinaryReturning=false;ordinaryDestination={};
  ordinaryReturnTick=ordinaryUseUntil=0;ordinaryBaseline=ordinaryReturnParent=0;ordinaryTiltIntentD56=ordinaryTiltUsedD56=false;
  ordinaryTransferD59=ordinaryTransferTiltD59=false;ordinaryIntentTickD59=0;
 }
 void retire(const prox67::Input& input) noexcept {
  selection.retire(input);work.reset();context={};admittedRef={};admittedSide=handSelectionSide=-1;
  admittedTick=useUntil=gripTick=unacquiredReleaseTickD61=0;admitted=false;admittedTable={};holdHint=false;pendingA=false;releasedAdmission=false;passengerParent=0;passengerTick=0;operationParent=0;
  admittedViaGazeD44=false;gazeSideD44=-1;gazeYieldTickD44=0;
   admissionBaseline=0;clearOrdinary();referenceRotationValid=false;previousUse=false;
   resourceResumeClosedR17=false;resourceChildR17=0;resourceTokenR17=0;resourceBindingR17={};
  mortarPair69=mortarLeft69=mortarRightGripD39=mortarReleasePending69=mortarReleaseIntent70=mortarTiltToDishD56=mortarStopping69=mortarReturning69=mortarAttempted69=false;
  mortarRetryNoEntry72=mortarRetryOpen72=mortarContentsTransferredD75=false;
  mortarSerial69=mortarUseUntil69=mortarReturnTick69=0;mortarReturnParent69=mortarParent69=mortarChild69=0;
  mortarRequest69={};mortarDestination69={};lastAimDestinationH21={};mortarPour70={};mortarHome70={};mortarNativeRecovery70=false;
  carafeHomeD85={};carafeHomeValidD85=false;
 }
 template<class Frame> Output step(const Input& in,const Frame& f) noexcept {
  Output out{};const bool priorUse=previousUse;
  struct UseEdge {bool& prior;bool& current;~UseEdge(){prior=current;}} useEdge{previousUse,out.use};
  prox67::Input ni{};ni.context=in.context;ni.now=in.now;ni.currentFrameSequence=in.currentNativeSequence;ni.palms=in.palm;
  ni.proximityCraft1=in.proximityCraft1;ni.selectionFrozenCraft1=admitted;
  ni.handsOnlyD53=in.proximityCraft1&&in.handsOnlyD53;
  ni.pickupReachD39=reach_craft1::clampPickupReachD39(in.pickupReachD39);
  bool frame=in.enabled&&in.qualified&&!in.cancel&&prox67::valid(in.context)&&in.modeEpoch&&in.modeEpoch==in.context.generation&&
   f.allowed&&f.headValid&&f.sequence&&f.timeNs&&prox67::fresh(f.tick,in.now)&&f.poseEpoch==in.context.poseEpoch&&f.sourceEpoch==in.context.sourceEpoch;
  for(float v:f.head)frame=frame&&std::isfinite(v);
  for(int h=0;h<2;++h)if(f.hand[h].valid)frame=frame&&::kcd2vr::workstation_sample46::finite(f.hand[h]);
  const bool physicalUse=in.nativeA||in.nativeBellowsD47;
  const bool newA=physicalUse&&!physicalA;physicalA=physicalUse;
  if(!frame){retire(ni);return out;}
  if(prox67::valid(context)&&!prox67::same(context,in.context))retire(ni);
  context=in.context;
  const bool ac=actionCurrent(in),wc=workCurrent(in);
  // Every contextual A can install a native director parent, including herb
  // pickup and bowl-to-mortar actions which have no gesture work child. Keep
  // that native lifetime separate from the source table slot. An action which
  // already existed at admission cannot become this request's receipt.
  if(admitted&&!operationParent&&ac&&in.action.parent&&in.action.parent!=admissionBaseline&&
     in.action.tick>admittedTick)operationParent=in.action.parent;
  if(newA){pendingA=true;passengerTick=in.now;passengerParent=ac?in.action.parent:0;}
  if(pendingA&&!passengerParent&&ac&&in.action.parent&&in.action.tick>=passengerTick)passengerParent=in.action.parent;
  if(pendingA&&passengerParent&&ac&&in.action.tick>=passengerTick&&in.action.parent!=passengerParent){
   pendingA=false;passengerParent=0;passengerTick=0;
  }
  // A refused native press must not leave Gesture permanently in passenger
  // mode. Only a fresh native director observation proving NO action after
  // the admission interval can end this pending request; missing data cannot.
  if(pendingA&&!passengerParent&&!physicalUse&&ac&&!in.action.parent&&
     in.action.tick>=passengerTick&&in.now>=passengerTick&&in.now-passengerTick>250){
   pendingA=false;passengerTick=0;
  }
  // No bookEnable/read/page flag participates in this lifetime. A that has not
  // yet produced a native parent stays pending until native admission or true
  // context loss; a missing observation cannot hand the arms back mid-action.
  const auto& nativeTableD56=in.nativeFrame.table;
  const bool retortD56=ac&&in.action.parent&&nativeTableD56.actionKnown&&nativeTableD56.nativeRetortD56&&
   nativeTableD56.instance==in.context.instance&&nativeTableD56.lifetime==in.context.lifetime&&
   nativeTableD56.action==in.action.parent&&prox67::fresh(nativeTableD56.tick,in.now);
  out.passenger=physicalUse||pendingA||retortD56;
  auto& c=out.control;c.instance=in.context.instance;c.lifetime=in.context.lifetime;c.tick=in.now;
  c.poseEpoch=in.context.poseEpoch;c.sourceEpoch=in.context.sourceEpoch;c.modeEpoch=in.modeEpoch;c.enabled=true;c.passenger=out.passenger;c.trackedPresentation71=!mortarNativeRecovery70&&(!retortD56||minigame_hands45::freeTrackedMode(minigame_hands45::modeFor(minigame_hands45::Alchemy)));
  auto& sc=out.semiControl;sc.instance=c.instance;sc.lifetime=c.lifetime;sc.tick=f.tick;sc.sequence=f.sequence;
  sc.poseEpoch=f.poseEpoch;sc.sourceEpoch=f.sourceEpoch;sc.tracked[0]=f.hand[0].valid;sc.tracked[1]=f.hand[1].valid;
  sc.enabled=true;sc.manualUse=out.passenger;
  if(wc){sc.parent=in.work.parent;sc.child=in.work.child;sc.token=in.work.token;}
  if(out.passenger){selection.retire(ni);work.reset();admitted=false;useUntil=0;admittedSide=handSelectionSide=-1;admittedRef={};clearOrdinary();
   c.parent=passengerParent;out.nomination={in.context,{},in.now};out.gesture.status=gesture::Status::NativePassenger;return out;}
  prox67::Completed completed{};completed.context=in.context;completed.tick=in.nativeFrame.table.tick;
  completed.sequence=in.nativeFrame.table.sequence;completed.table=in.nativeFrame.table.refs;
  completed.offers=in.nativeFrame.offers;completed.count=prox67::Capacity;
  completed.complete=in.nativeFrame.table.instance==in.context.instance&&in.nativeFrame.table.player==in.context.player&&
   in.nativeFrame.table.lifetime==in.context.lifetime&&in.nativeFrame.table.playerEntity&&prox67::fresh(completed.tick,in.now);
  // D61: a rejected/never-started Grip request used to retain admission
  // forever, blocking even a fresh plate nomination after release/regrip.
  // Retire ONLY an unacquired local intent. The request is no longer emitted,
  // its original Grip is open, and a later completed native observation proves
  // both an idle director and empty native hands. Missing/stale observations,
  // any observed operation, binding or work keep the original owner alive.
  // This is not a native completion/cancel and emits no retry or item effect.
  const bool unacquiredD61=admitted&&admittedSide>=0&&admittedSide<2&&!operationParent&&
   !ordinaryCarried&&!mortarPair69&&!work.acquired&&!ordinaryReturning&&!ordinaryTransferD59&&
   !mortarStopping69&&!mortarReturning69&&!bridge::currentToolD67(in.tool[0],in.context,in.now,0)&&
    !bridge::currentToolD67(in.tool[1],in.context,in.now,1)&&!wc;
  const bool releasedD61=unacquiredD61&&prox67::usable(in.palm[unsigned(admittedSide)],admittedSide,ni)&&
   f.hand[admittedSide].valid&&in.palm[unsigned(admittedSide)].grip<.35&&f.hand[admittedSide].grip<.35f;
  if(releasedD61){if(!unacquiredReleaseTickD61)unacquiredReleaseTickD61=in.now;}
  else unacquiredReleaseTickD61=0;
  const auto& idleTableD61=in.nativeFrame.table;
  const bool idleAfterReleaseD61=releasedD61&&!priorUse&&in.now>=admittedTick&&in.now-admittedTick>250&&
   in.now>=useUntil&&completed.complete&&completed.sequence&&
   (!in.currentNativeSequence||completed.sequence<in.currentNativeSequence)&&idleTableD61.actionKnown&&!idleTableD61.action&&
   idleTableD61.handTypesKnown68&&idleTableD61.handType68[0]==21u&&idleTableD61.handType68[1]==21u&&
   idleTableD61.tick>unacquiredReleaseTickD61&&ac&&!in.action.parent&&in.action.tick>unacquiredReleaseTickD61;
  if(idleAfterReleaseD61){
   retire(ni);out.unacquiredRetiredD61=true;out.nomination={in.context,{},in.now};sc.manualUse=true;return out;
  }
  native::Observation observed=in.nativeSelected;bool selectedKnown=in.nativeSelectedKnown;
  if(!selectedKnown&&in.nativeSelectedTypeOnly==18u&&completed.complete&&completed.table[18].entity&&
     prox67::fresh(in.nativeTypeOnlyTick,in.now)){ // R8b: mounted bellows selected by type only -> its table member
   observed={in.context.instance,in.context.lifetime,in.nativeTypeOnlyTick,std::uint32_t(completed.table[18].entity),18u,false};
   selectedKnown=true;
  }
  const bool known=selectedKnown&&observed.instance==in.context.instance&&observed.lifetime==in.context.lifetime&&
   observed.entity&&observed.type<prox67::Capacity&&prox67::fresh(observed.tick,in.now)&&
   prox67::same(completed.table[observed.type],prox67::Ref{int(observed.type),observed.entity});
  if(known)ni.selected={in.context,{int(observed.type),observed.entity},in.nativeSelectionTick?in.nativeSelectionTick:observed.tick,-1,true};
  // INVESTIGATE_D67R1[POT_PROGRESS]: slot 6 remains a native contextual toggle.
  // MovePot vft 0x3A5C150 stores child+0x70; Start 0x6C62A4 calls 0x15FC374/0x15FC404
  // before queueing Henry's AlchemyUse. Up/down CAFs last 2.5/3.433 s, with native
  // Attach/Detach inside them. Prove the kettle's own progress clock and its
  // raised-state commit alongside that child before mapping hand travel to a
  // monotonic work interval. A faster Henry CAF alone cannot synchronize both.
  // R103 (mortar misbehaving again, /tmp/r103/KCD2VR.log 09:57:49.451-09:58:01.656): the left Grip was
  // admitted on the bowl next to the mortar (admitted=952226/0) but the game took the mortar + pestle
  // (09:57:50.507 toolL=952205/2 toolR=952214/3, UseMortar running). The bowl admission then waited for a
  // bowl grip that never came (JOINED_NATIVE_GRIP_REQUIRED), the mortar pour path never ran and only A
  // placed the mortar (09:58:01.656). When the game has the exact mortar pair joined in both hands, the
  // admission IS the mortar: move it to the mortar (left hand) with the same reset as a fresh mortar grip.
  if(admitted&&admittedRef.slot!=2&&admittedRef.slot!=3&&!ordinaryCarried&&completed.complete&&
     bridge::currentToolD67(in.tool[1],in.context,in.now,1)&&in.tool[1].type==2&&in.tool[1].entity==completed.table[2].entity&&
     bridge::currentToolD67(in.tool[0],in.context,in.now,0)&&in.tool[0].type==3&&in.tool[0].entity==completed.table[3].entity&&
     in.tool[0].character==in.tool[1].character&&in.tool[0].parent==in.tool[1].parent&&
     prox67::registered(completed,completed.table[2])&&prox67::usable(in.palm[1],1,ni)&&f.hand[1].valid){
   admittedRef=completed.table[2];admittedSide=1;admittedTable=completed.table;
   clearOrdinary();ordinaryWasHeld=false;plateAdmissionR93=false;plateBindingR93={};
   mortarPair69=mortarReleasePending69=mortarReleaseIntent70=mortarTiltToDishD56=mortarStopping69=mortarReturning69=mortarAttempted69=false;
   mortarLeft69=in.palm[1].grip>=.55;
   mortarRightGripD39=prox67::usable(in.palm[0],0,ni)&&f.hand[0].valid&&in.palm[0].grip>=.55;
   mortarRetryNoEntry72=mortarRetryOpen72=mortarContentsTransferredD75=false;
   mortarSerial69=mortarUseUntil69=mortarReturnTick69=0;mortarReturnParent69=mortarParent69=mortarChild69=0;mortarRequest69={};mortarDestination69={};
   mortarHome70={};mortarNativeRecovery70=false;
   if(in.controllerMortar70){
    const prox67::Offer* pot70=nullptr;const prox67::Offer* mortar70=nullptr;
    for(const auto& o:completed.offers)if(prox67::usable(o,completed,in.now)&&o.boundsValid){if(o.ref.slot==6)pot70=&o;if(o.ref.slot==2)mortar70=&o;}
    if(pot70&&mortar70)(void)mortarHome70.capture(pot70->ref.entity,mortar70->ref.entity,pot70->bounds,mortar70->bounds);
   }
   referenceRotationValid=::kcd2vr::workstation_sample46::finite(f.hand[1]);
   for(int j=0;j<4;++j)referenceRotation[j]=f.hand[1].rotation[j];
  }
  const auto isWorkType=[](int t) noexcept {return t==2||t==3||t==18||(t>=13&&t<=16);};
  const bool ordinary=admitted&&!isWorkType(admittedRef.slot);
  // The saved controller hand is not the native TakeObject hand. Capture only
  // this plate after this admitted pickup; a pre-existing opposite-hand tool
  // or replacement binding cannot be adopted into the occurrence.
  const bool plateModeR93=in.proximityCraft1&&minigame_hands45::freeTrackedMode(minigame_hands45::modeFor(minigame_hands45::Alchemy));
  if(ordinary&&admittedRef.slot==0&&admittedSide==1&&plateAdmissionR93&&plateModeR93&&
     !plateBindingR93&&operationParent&&ac&&(in.action.parent==operationParent||!in.action.parent)&&
     in.action.tick>admittedTick&&in.tool[0].entity==admittedRef.entity&&
     bridge::plateBindingCurrentR93(in.tool[0].binding,in.nativeFrame.table,in.tool[0],in.tool[1],admittedTick,f.poseEpoch,in.now))
      plateBindingR93=in.tool[0].binding;
  const bool plateMappedR93=ordinary&&admittedRef.slot==0&&admittedSide==1&&plateModeR93&&
      bridge::plateBindingCurrentR93(plateBindingR93,in.nativeFrame.table,in.tool[0],in.tool[1],admittedTick,f.poseEpoch,in.now);
  int ordinaryNativeSideR93=plateBindingR93?0:admittedSide;
  const auto ordinaryBinding=[&]() noexcept {
   if(!ordinary||admittedSide<0||!completed.complete||(plateBindingR93&&!plateMappedR93))return false;
   const auto& t=in.tool[unsigned(ordinaryNativeSideR93)];
   return t.joined&&t.parent==0&&t.side==ordinaryNativeSideR93&&t.type==admittedRef.slot&&t.entity==admittedRef.entity&&t.character&&
    t.instance==in.context.instance&&t.lifetime==in.context.lifetime&&t.poseEpoch==f.poseEpoch&&prox67::fresh(t.tick,in.now)&&
    prox67::registered(completed,admittedRef)&&prox67::usable(in.palm[unsigned(admittedSide)],admittedSide,ni)&&f.hand[admittedSide].valid;
  };
  const bool mortarType69=admitted&&(admittedRef.slot==2||admittedRef.slot==3);
  const auto pairTool69=[&](int side,int type){const auto& t=in.tool[side];
   return t.joined&&t.side==side&&t.type==type&&t.entity==completed.table[type].entity&&t.character&&t.binding&&
    t.instance==in.context.instance&&t.lifetime==in.context.lifetime&&t.poseEpoch==f.poseEpoch&&prox67::fresh(t.tick,in.now)&&
    prox67::usable(in.palm[side],side,ni)&&f.hand[side].valid;};
  const bool pairNow69=mortarType69&&pairTool69(0,3)&&pairTool69(1,2)&&
   in.tool[0].character==in.tool[1].character&&in.tool[0].parent==in.tool[1].parent;
  if(pairNow69){if(!mortarPair69){
    for(int j=0;j<4;++j)mortarNeutral69[j]=f.hand[1].rotation[j];
    mortarLeftBinding69=in.tool[1].binding;mortarRightBinding69=in.tool[0].binding;
   }else if(!alchemy_tool18::sameMortarGrip69(mortarLeftBinding69,in.tool[1].binding)||
            !alchemy_tool18::sameMortarGrip69(mortarRightBinding69,in.tool[0].binding)){
    retire(ni);out={};out.nomination={in.context,{},in.now};return out;
   }mortarPair69=true;
   if(wc&&in.work.kind==semi::Mortar){mortarParent69=in.work.parent;mortarChild69=in.work.child;}}
  // D70: an absent WORLD-palm publication must not orphan an already attached
  // native mortar pair. It authorizes NO tracking, Use, tool pose or work here.
  // Only exact current bindings retain semantic session identity; true tracking
  // epoch/context loss still retires at the frame boundary above. Resumption
  // uses fresh palms and the same pair, never a frozen pose or a new pickup.
  const bool retainedNativePairD70=mortarType69&&mortarPair69&&completed.complete&&ac&&
   bridge::currentToolD67(in.tool[0],in.context,in.now,0)&&bridge::currentToolD67(in.tool[1],in.context,in.now,1)&&
   alchemy_tool18::sameMortarGrip69(mortarRightBinding69,in.tool[0].binding)&&
   alchemy_tool18::sameMortarGrip69(mortarLeftBinding69,in.tool[1].binding)&&
   in.tool[0].parent==in.tool[1].parent&&in.tool[0].parent==in.action.parent;
  if(retainedNativePairD70&&(!prox67::usable(in.palm[0],0,ni)||!prox67::usable(in.palm[1],1,ni))){
   out.admitted=true;out.nomination={in.context,{},in.now};sc.manualUse=true;
   out.gesture.status=gesture::Status::InvalidFrame;return out;
  }
  // Carrying keeps actual Grip private but uses LEFT palm for a destination.
  // Held source objects cannot win merely by following that same palm.
  // Retail resolver8D20DC accepts selected6(pot) or0(dish/return), not2/3.
  if(mortarType69&&mortarPair69){
   for(auto& p:ni.palms)p.grip=0;ni.palms[0].valid=false;
   for(auto& o:completed.offers)if(o.ref.slot==2||o.ref.slot==3)o.valid=false;
   // D53: a right-pestle source becomes a LEFT-mortar destination owner.
   // Retire only the old source nomination before its deliberately disabled
   // right palm is tested as tracking loss. Keep the admitted pair/Grip/work.
   if(selection.side==0)selection.retire(ni);
  }
  const bool ordinaryBound=ordinaryBinding();
  // TakeIngredient records native hand membership for slots7..9; it need not
  // create an entity_right attachment. Keep that logical lifetime separate
  // from physical tool presentation, while using the same native Use/Return.
  const bool logicalIngredientD47=ordinary&&admittedSide==0&&admittedRef.slot>=7&&admittedRef.slot<=9;
  const bool logicalBoundD47=logicalIngredientD47&&completed.complete&&in.nativeFrame.table.handTypesKnown68&&
   in.nativeFrame.table.handType68[0]==unsigned(admittedRef.slot)&&prox67::registered(completed,admittedRef)&&
   prox67::usable(in.palm[0],0,ni)&&f.hand[0].valid;
  const bool ordinaryOwnedD47=ordinaryBound||logicalBoundD47;
  if(ordinaryOwnedD47&&!ordinaryCarried)ordinaryCarried=true;
  const auto heldContentsD72=[&](int side) noexcept -> prox67::Ref {
   if(!ni.handsOnlyD53)return {};
   if(side==1&&mortarType69&&mortarPair69&&pairNow69)return completed.table[2];
   if(side==admittedSide&&ordinaryOwnedD47&&(admittedRef.slot==0||logicalIngredientD47))return admittedRef;
   return {};
  };
  const auto contentsDestinationD72=[&](int side,const prox67::Offer& offer) noexcept {
   const auto source=heldContentsD72(side);
   return prox67::valid(source)&&!prox67::same(source,offer.ref)&&prox67::pourRecipientD72(offer,in.palm[side]);
  };
  if(in.palmRayD70&&ni.handsOnlyD53)for(int h=0;h<2;++h){
   std::uint32_t mask=0;
   for(const auto& offer:completed.offers)if(prox67::usable(offer,completed,in.now)&&
      ((prox67::valid(heldContentsD72(h))&&!prox67::same(heldContentsD72(h),offer.ref)&&
        (offer.ref.slot==0||offer.ref.slot==2||offer.ref.slot==6))?
        contentsDestinationD72(h,offer):prox67::palmCorridorD70(offer,in.palm[h],in.aimH21[h],in.now)))mask|=1u<<unsigned(offer.ref.slot);
   ni.acquisitionMaskH5[h]&=mask;
  }
  // A carried object's real Grip remains latched privately. Its pointing palm
  // may nominate a destination without pretending to make another grab. The
  // held object itself is excluded: its point follows this same palm and would
  // otherwise permanently win the nearest-candidate calculation.
  const bool rightRetortD70=ordinary&&ordinaryCarried&&ordinaryBound&&admittedSide==1&&admittedRef.slot==5&&
   bridge::currentToolD67(in.tool[1],in.context,in.now,1)&&
   prox67::usable(in.palm[0],0,ni)&&f.hand[0].valid&&
   (ni.acquisitionMaskH5[0]&(1u<<4))&&prox67::usable(completed.offers[4],completed,in.now)&&
   prox67::distanceToOffer(completed.offers[4],in.palm[0].point)<=prox67::enter(ni,completed.table[4],0);
  if(ordinary&&ordinaryCarried){
   for(auto& p:ni.palms)p.grip=0;
   if(!rightRetortD70)ni.palms[unsigned(1-admittedSide)].valid=false; // carrying destination, or exact opposite retort
   else ni.acquisitionMaskH5[0]&=1u<<4;
   for(auto& o:completed.offers)if(prox67::same(o.ref,admittedRef))o.valid=false;
  }
  // A live pickup policy change retires only unaccepted source intent. An
  // already attached ingredient/tool keeps its return, release and work state.
  if(!ni.handsOnlyD53&&pickupPolicySeenD39&&pickupReachD39!=ni.pickupReachD39&&!admitted)selection.retire(ni);
  pickupPolicySeenD39=true;pickupReachD39=ni.pickupReachD39;ni.sourceAcquisitionD39=!admitted;
  // Qualify only NEW source acquisition. Once admitted, ordinary carried-tool
  // return and mortar/pot destination nomination keep their original ALL mask.
  if(!admitted)for(int side=0;side<2;++side){
   if(!in.proximityCraft1)ni.acquisitionMaskH5[unsigned(side)]=sourceMaskH5(side);
   // New proximity ownership accepts either central hand. Native attachments
   // still decide which hand actually holds each tool; no object is reattached.
  }
  // A held Grip selects its hand, even when that hand has no eligible nearby
  // item. Empty hand selection must not silently become a head selection. Keep
  // native table/geometry and final selected-target admission unchanged.
  // Even with no nearby destination, the carried mortar's LEFT hand owns
  // selection; an empty destination must not fall back to distant head aim.
  int gripSelectionSide=in.proximityCraft1&&mortarType69&&mortarPair69?1:-1;
  if(in.proximityCraft1&&!(mortarType69&&mortarPair69)){
   // The acquired pair already owns both real Grips; its left destination
   // nomination above must not be reassigned to the original right source.
   const auto heldSide=[&](int side,double threshold){return side>=0&&side<2&&
    prox67::usable(in.palm[unsigned(side)],side,ni)&&f.hand[side].valid&&in.palm[unsigned(side)].grip>=threshold;};
   if(admitted&&heldSide(admittedSide,.35))gripSelectionSide=admittedSide;
   else if(heldSide(handSelectionSide,.35))gripSelectionSide=handSelectionSide;
   else if(heldSide(selection.side,.35))gripSelectionSide=selection.side;
   else for(int side=0;side<2;++side)if(heldSide(side,.55)){gripSelectionSide=side;break;}
   // D39: keeping an empty hand closed must not suppress the other hand's
   // real source pickup forever. Switch only before admission, only to a
   // deliberate non-quarantined Grip, and only when copied native bounds
   // prove an eligible source in the current pickup region. If neither hand
   // has a source, retain the empty hand owner so gaze cannot grab remotely.
   if(!admitted&&gripSelectionSide>=0&&completed.complete){
    const auto nearSource=[&](int side){
     if(selection.quarantined[unsigned(side)]||!heldSide(side,.35))return false;
     for(const auto& offer:completed.offers)if(offer.boundsValid&&prox67::usable(offer,completed,in.now)&&
        prox67::acquisitionAllowedH5(ni,side,offer.ref)&&
        prox67::contains(ni,offer.ref,side,prox67::distanceToOffer(offer,in.palm[unsigned(side)].point)))return true;
     return false;
    };
    const int opposite=1-gripSelectionSide;
    if(!nearSource(gripSelectionSide)&&heldSide(opposite,.55)&&nearSource(opposite))gripSelectionSide=opposite;
   }
   if(rightRetortD70)gripSelectionSide=0;
   if(gripSelectionSide>=0)ni.acquisitionMaskH5[unsigned(1-gripSelectionSide)]=0;
  }
  handSelectionSide=gripSelectionSide;
  auto decision=selection.step(completed,ni);
  if(gripSelectionSide>=0&&completed.complete&&prox67::same(selection.context,in.context)){
   decision.source=gripSelectionSide?prox67::Source::Left:prox67::Source::Right;
   decision.palm=in.palm[unsigned(gripSelectionSide)];
   // A released left mortar still owns its destination in mode3. It is an
   // OPEN hands-only nomination, not a claimed closed Grip that the native
   // selector must reject. Mode2 retains its original held-Grip contract.
   decision.gripSelectionOwnedCraft1=!ni.handsOnlyD53||decision.palm.grip>=.35;
   // The empty result carries the same completed table provenance as a normal
   // nomination. No reference, admission, or native effect is invented here.
  }
  // R96: mortar taken with the RIGHT hand. Native still puts the mortar in the
  // LEFT hand (toolL=mortar, toolR=pestle). KCD2VR2.log 17:55:23-17:55:33:
  // grip R/L=1/0 for 10 s, admittedSide=0, controlHeld=1,0, mortarRequest=0 the
  // whole time: left was never squeezed, so the left tilt and the pot under the
  // left palm were never looked at (only manual A at 17:55:34 placed it).
  // Here the left hand carries the mortar without a squeeze: the pot/dish
  // nearest to the LEFT palm (inside the normal enter radius) is the
  // destination, and the left tilt below decides the pour.
  const bool carriedLeftR96=mortarType69&&pairNow69&&admittedSide==0&&!mortarLeft69&&!mortarReleasePending69&&
   prox67::usable(in.palm[1],1,ni)&&f.hand[1].valid;
  if(carriedLeftR96&&!(decision.source==prox67::Source::Left&&prox67::mortarDestinationH21(decision.ref.slot))){
   double bestR96=std::numeric_limits<double>::infinity();
   for(const auto& o:completed.offers){
    if(!prox67::usable(o,completed,in.now)||!prox67::mortarDestinationH21(o.ref.slot))continue;
    const auto close=prox67::closest(o,in.palm[1].point);
    if(close.valid&&close.distance<=prox67::enter(ni,o.ref,1)&&close.distance<bestR96){
     bestR96=close.distance;decision.source=prox67::Source::Left;decision.ref=o.ref;decision.gap=close.distance;decision.palm=in.palm[1];}
   }
  }
  if(mortarType69&&pairNow69&&!in.proximityCraft1){
   auto real=ni;real.palms=in.palm;
   const bool releaseAim=in.palm[1].grip<.35&&(mortarLeft69||mortarStopping69||mortarReturning69);
   c.destinationSourceH21=completed.table[2];c.destinationReleaseH21=releaseAim;
   if(!(decision.source==prox67::Source::Left&&prox67::mortarDestinationH21(decision.ref.slot)&&decision.gap<=prox67::Enter)){
    auto aimed=prox67::mortarAimH21(completed,real,in.aimH21[1],completed.table[2],releaseAim);
    if(aimed.aimedDestinationH21&&(!(mortarStopping69||mortarReturning69)||prox67::same(aimed.ref,mortarDestination69))){aimed.hapticEdge=!prox67::same(aimed.ref,lastAimDestinationH21);decision=aimed;}
   }
  }
  if(in.palmRayD70&&ni.handsOnlyD53&&decision.source!=prox67::Source::Gaze){
   const int h=decision.source==prox67::Source::Left?1:0;
   decision.palmRayD70=true;decision.aimH21=in.aimH21[h];
   if(prox67::valid(decision.ref)&&contentsDestinationD72(h,completed.offers[unsigned(decision.ref.slot)])){
    decision.pourRecipientD72=true;decision.heldSourceH21=heldContentsD72(h);decision.palmRayD70=false;
   }
  }
  lastAimDestinationH21=decision.aimedDestinationH21?decision.ref:prox67::Ref{};
  // Assisted reach explicitly keeps the native head selector available when
  // the gripping hand has no eligible close source. CloseBounds deliberately
  // retains its empty-hand ownership. A fresh native completed selection AFTER
  // yielding is required; yesterday's hand-steered nominee cannot turn into a
  // distant pickup. The ordinary native action/attachment still owns success.
  bool gazeAssistD44=false;int gazeAdmissionSideD44=-1;
  const bool emptyAssistD44=in.proximityCraft1&&!ni.handsOnlyD53&&!admitted&&gripSelectionSide>=0&&
   ni.pickupReachD39==int(reach_craft1::PickupReachD39::Assisted)&&
   !prox67::valid(decision.ref)&&completed.complete&&decision.frameSequence&&
   prox67::same(selection.context,in.context);
  if(emptyAssistD44){
   if(!gazeYieldTickD44||gazeSideD44!=gripSelectionSide){gazeSideD44=gripSelectionSide;gazeYieldTickD44=in.now;}
   decision.source=prox67::Source::Gaze;decision.gripSelectionOwnedCraft1=false;
   decision.gripping=false;decision.ref={};
   const int type=int(observed.type);
   const auto deliberate=[&](int side){return side>=0&&side<2&&!selection.quarantined[unsigned(side)]&&
    prox67::usable(in.palm[unsigned(side)],side,ni)&&f.hand[side].valid&&
    in.palm[unsigned(side)].grip>=.55&&f.hand[side].grip>=.55f&&reach_craft1::allowed(type,side);};
   if(deliberate(gripSelectionSide))gazeAdmissionSideD44=gripSelectionSide;
   else if(deliberate(1-gripSelectionSide))gazeAdmissionSideD44=1-gripSelectionSide;
   bool offered=false,alreadyHeld=false;
   if(known)for(const auto& offer:completed.offers)if(offer.boundsValid&&prox67::usable(offer,completed,in.now)&&
      offer.ref.slot==type&&offer.ref.entity==observed.entity)offered=true;
   for(const auto& tool:in.tool)if(tool.joined&&tool.instance==in.context.instance&&tool.lifetime==in.context.lifetime&&
      tool.poseEpoch==f.poseEpoch&&prox67::fresh(tool.tick,in.now)&&tool.entity==observed.entity)alreadyHeld=true;
   gazeAssistD44=known&&offered&&!alreadyHeld&&gazeAdmissionSideD44>=0&&
    ni.selected.valid&&prox67::fresh(ni.selected.tick,in.now)&&ni.selected.tick>gazeYieldTickD44;
  }else if(admitted&&admittedViaGazeD44&&!ordinaryCarried&&!mortarPair69){
   // Keep the native selected source through its input pulse and delayed
   // native Attach. No distant destination nomination is introduced.
   decision.source=prox67::Source::Gaze;decision.ref={};decision.gripSelectionOwnedCraft1=false;decision.gripping=false;
  }else {gazeYieldTickD44=0;gazeSideD44=-1;}
  out.nomination={in.context,decision,in.now};
  bool topology=true;if(admitted)for(unsigned i=0;i<prox67::Capacity;++i)topology=topology&&prox67::same(admittedTable[i],completed.table[i]);
  if(admitted&&(!prox67::same(context,selection.context)||!prox67::registered(completed,admittedRef)||
    !topology||!prox67::usable(in.palm[unsigned(admittedSide)],admittedSide,ni))){
   admitted=false;work.reset();useUntil=0;clearOrdinary();auto real=ni;real.palms=in.palm;selection.retire(real);}
  // An admitted operation remains one operation through release/return. A new
  // input cannot retarget its live work child merely by hovering another item.
  if(admitted&&!ordinaryCarried&&!mortarPair69&&operationParent&&ac&&in.action.tick>admittedTick&&in.action.parent!=operationParent){
   admitted=false;work.reset();useUntil=0;operationParent=0;
   auto real=ni;real.palms=in.palm;selection.retire(real); // consumed Grip cannot retarget into a second contextual A
   out.nomination={in.context,{},in.now};sc.manualUse=true;return out; // discard the decision computed before retirement
  }
  const int nominatedSide=decision.source==prox67::Source::Left?1:decision.source==prox67::Source::Right?0:-1;
  const int nominatedType=decision.ref.slot;
  const bool nativeHandAllowed=in.proximityCraft1?reach_craft1::allowed(nominatedType,nominatedSide):sourceHandAllowedH5(nominatedType,nominatedSide);
  const bool nearAdmissionD44=decision.admissionReady&&decision.gripping&&known&&nativeHandAllowed&&
   std::isfinite(decision.gap)&&decision.gap<=prox67::enter(ni,decision.ref,nominatedSide)&&decision.latchedTick!=gripTick;
  if(!admitted&&(nearAdmissionD44||gazeAssistD44)){
   admitted=true;admittedViaGazeD44=gazeAssistD44;
   admittedRef=gazeAssistD44?ni.selected.ref:decision.ref;
   admittedSide=gazeAssistD44?gazeAdmissionSideD44:nominatedSide;
   admittedTick=in.now;unacquiredReleaseTickD61=0;gripTick=gazeAssistD44?gazeYieldTickD44:decision.latchedTick;
   holdHint=observed.hold;useUntil=holdHint?0:in.now+80;operationParent=0;
   admissionBaseline=ac?in.action.parent:0;admittedTable=completed.table;
   releasedAdmission=false;resourceResumeClosedR17=false;resourceChildR17=0;resourceTokenR17=0;resourceBindingR17={};
   mortarPair69=mortarLeft69=mortarRightGripD39=mortarReleasePending69=mortarReleaseIntent70=mortarTiltToDishD56=mortarStopping69=mortarReturning69=mortarAttempted69=false;
   // The physical admission already owns its Grip edge. Native attachment can
   // arrive much later; it must not replace that edge with an already-open hand.
   if(admittedRef.slot==2||admittedRef.slot==3){
    mortarLeft69=prox67::usable(in.palm[1],1,ni)&&f.hand[1].valid&&in.palm[1].grip>=.55;
    mortarRightGripD39=prox67::usable(in.palm[0],0,ni)&&f.hand[0].valid&&in.palm[0].grip>=.55;
   }
   mortarRetryNoEntry72=mortarRetryOpen72=mortarContentsTransferredD75=false;
   mortarSerial69=mortarUseUntil69=mortarReturnTick69=0;mortarReturnParent69=mortarParent69=mortarChild69=0;mortarRequest69={};mortarDestination69={};
  mortarHome70={};mortarNativeRecovery70=false;
   if(in.controllerMortar70&&(admittedRef.slot==2||admittedRef.slot==3)){
    const prox67::Offer* pot70=nullptr;const prox67::Offer* mortar70=nullptr;
    for(const auto& o:completed.offers)if(prox67::usable(o,completed,in.now)&&o.boundsValid){if(o.ref.slot==6)pot70=&o;if(o.ref.slot==2)mortar70=&o;}
    if(pot70&&mortar70)(void)mortarHome70.capture(pot70->ref.entity,mortar70->ref.entity,pot70->bounds,mortar70->bounds);
   }
   // D85 craft: copy the carafe's own current native bounds as its home. Only
   // the retail selection bounds are used; no mesh, slot or guessed offset.
   carafeHomeD85={};carafeHomeValidD85=false;
   if(admittedRef.slot>=13&&admittedRef.slot<=16)
    for(const auto& o:in.nativeFrame.offers)if(prox67::same(o.ref,admittedRef)&&o.boundsValid&&prox67::usable(o,completed,in.now)&&
       geometry68::valid(o.bounds)){carafeHomeD85=o.bounds;carafeHomeValidD85=true;break;}
   clearOrdinary();ordinaryWasHeld=!isWorkType(admittedRef.slot);
   plateAdmissionR93=admittedRef.slot==0&&admittedSide==1&&completed.complete&&
       in.nativeFrame.table.handTypesKnown68&&in.nativeFrame.table.handType68[0]==21&&in.nativeFrame.table.handType68[1]==21;
   referenceRotationValid=::kcd2vr::workstation_sample46::finite(f.hand[admittedSide]);
   for(int j=0;j<4;++j)referenceRotation[j]=f.hand[admittedSide].rotation[j];
  }
  out.admitted=admitted;c.admissionTickR22=admitted?admittedTick:0;
  if(!admitted){sc.manualUse=true;return out;} // no phantom work-clock lease
  const bool held=prox67::usable(in.palm[unsigned(admittedSide)],admittedSide,ni)&&in.palm[unsigned(admittedSide)].grip>=.35;
  if(mortarType69){
   const bool leftNow=prox67::usable(in.palm[1],1,ni)&&f.hand[1].valid&&in.palm[1].grip>=(mortarLeft69?.35:.55);
   mortarRightGripD39=prox67::usable(in.palm[0],0,ni)&&f.hand[0].valid&&in.palm[0].grip>=(mortarRightGripD39?.35:.55);
   if(mortarLeft69&&!leftNow)mortarReleasePending69=true;
   if(leftNow)mortarReleasePending69=false; // deliberate regrip before dispatch cancels pending release
   mortarLeft69=leftNow;
  }
  const bool resourceType=admittedRef.slot>=13&&admittedRef.slot<=16;
  if((resourceType||admittedRef.slot==18)&&!held)releasedAdmission=true;
  if(resourceType&&wc&&in.work.kind==semi::Resource&&ac&&operationParent&&
     in.action.parent==operationParent&&in.work.parent==operationParent){
   if(in.work.phase>=2||in.nativeReturnAuthorized)resourceResumeClosedR17=true;
   if(in.work.child&&in.work.token){
    if(resourceChildR17&&(resourceChildR17!=in.work.child||resourceTokenR17!=in.work.token))resourceResumeClosedR17=true;
    else {resourceChildR17=in.work.child;resourceTokenR17=in.work.token;}
   }
  }
  // A new admission above can change the physical side in this same step.
  ordinaryNativeSideR93=plateBindingR93?0:admittedSide;
  c.entity=admittedRef.entity;c.type=admittedRef.slot;c.side=ordinaryNativeSideR93;
  if(plateBindingR93){c.plateBindingR93=plateBindingR93;c.plateGestureSideR93=admittedSide;}
  if(resourceType&&admittedSide==1){
   // D85 craft: current left-hand carafe intent for the owner-thread clock.
   // Pot proximity/tilt use the pour gesture's own thresholds; home uses the
   // copied bounds above. Bits are input evidence only, never a native call.
   const bool palmD85=prox67::usable(in.palm[1],1,ni)&&f.hand[1].valid;
   float upD85[3]{},neutralD85[3]{},tiltD85=0;
   const bool tiltKnownD85=palmD85&&referenceRotationValid&&gesture::upVector(f.hand[1].rotation,upD85)&&gesture::upVector(referenceRotation,neutralD85);
   if(tiltKnownD85){float dot=0;for(int j=0;j<3;++j)dot+=upD85[j]*neutralD85[j];tiltD85=std::acos(std::clamp(dot,-1.f,1.f));}
   bool potKnownD85=false;
   // Known = native geometry exists; a missing palm sample only yields an
   // infinite gap (keep waiting), never a refusal of the whole carafe.
   double potGapD85=INFINITY,homeGapD85=INFINITY;
   for(const auto& o:completed.offers)if(o.ref.slot==6&&o.boundsValid&&prox67::usable(o,completed,in.now)){potKnownD85=true;
    const auto close=prox67::closest(o,in.palm[1].point);if(close.valid&&palmD85)potGapD85=close.distance;break;}
   const bool homeKnownD85=carafeHomeValidD85;
   if(carafeHomeValidD85&&palmD85){const auto close=geometry68::closest(carafeHomeD85,{in.palm[1].point.x,in.palm[1].point.y,in.palm[1].point.z});
    if(close.valid)homeGapD85=close.distance;}
   c.carafeD85=carafe_d85::intent(true,held,potKnownD85,potGapD85,homeKnownD85,homeGapD85,tiltKnownD85,tiltD85);
  }
  if(alchemy_tool18::nativeFixtureD56(admittedRef.slot)){
   // Hands may nominate the actual retort, but do not carry it, replace Hold A,
   // or advance its native work. Physical A passes through the host unchanged.
   out.use=false;sc.manualUse=true;c.held[0]=c.held[1]=false;
   out.gesture.status=gesture::Status::NativePassenger;return out;
  }
  const bool selectedStillSame=known&&observed.entity==admittedRef.entity&&int(observed.type)==admittedRef.slot;
  out.use=held&&!releasedAdmission&&selectedStillSame&&(holdHint||in.now<useUntil);
  if(rightRetortD70&&decision.source==prox67::Source::Right&&decision.ref.slot==4){
   out.use=false;sc.manualUse=true;c.parent=0;c.held[1]=ordinaryBound&&held;
   out.ordinaryHeld=c.held[1];out.gesture.status=gesture::Status::NativePassenger;
   return out; // exact stationary target is available to native A; flask is not put into the retort
  }
  if(!isWorkType(admittedRef.slot)){
   sc.manualUse=true;out.gesture.status=gesture::Status::GripRequired;
   const double grip=in.palm[unsigned(admittedSide)].grip;
   const bool realHeld=grip>=(ordinaryWasHeld?.35:.55),released=ordinaryWasHeld&&!realHeld;
   const auto nearDestination=[&](prox67::Ref target) noexcept {
    if(prox67::same(target,admittedRef)){
     // The game's own current source-A return context is allowed only when
     // its original offered point is still genuinely within ten centimeters.
     for(const auto& o:in.nativeFrame.offers)if(prox67::same(o.ref,target)&&prox67::usable(o,completed,in.now)&&
       prox67::distanceToOffer(o,in.palm[unsigned(admittedSide)].point)<=prox67::enter(ni,target,admittedSide))return true;
     return false;
    }
    if(decision.source==prox67::Source::Gaze||!prox67::same(decision.ref,target)||decision.gap>prox67::enter(ni,target,admittedSide))return false;
    for(const auto& o:completed.offers)if(prox67::same(o.ref,target)&&prox67::usable(o,completed,in.now)&&
      prox67::distanceToOffer(o,in.palm[unsigned(admittedSide)].point)<=prox67::enter(ni,target,admittedSide))return true;
    return false; // another nearby hand is not a remote Put for this carried tool
   };
   const bool transferEnabledD59=(in.gestureTransferD59||in.handsOnlyD53)&&in.proximityCraft1&&f.hand[admittedSide].valid&&
      (admittedSide==0||admittedRef.slot==0)&&
      (admittedRef.slot==0||(admittedRef.slot>=7&&admittedRef.slot<=9));
   const auto selfReturnD74=[&](prox67::Ref target) noexcept {
    return ni.handsOnlyD53&&prox67::same(target,admittedRef)&&(!plateBindingR93||plateMappedR93)&&
     (logicalBoundD47||bridge::ordinarySourceReturnD75(in.tool[unsigned(ordinaryNativeSideR93)],
       in.nativeFrame.table,in.context,admittedRef,ordinaryNativeSideR93,in.now));
   };
   const auto recipientD59=[&](prox67::Ref target) noexcept {
    return selfReturnD74(target)||(!prox67::same(target,admittedRef)&&(target.slot==0||target.slot==2||target.slot==6));
   };
   const auto localRecipientD59=[&](prox67::Ref target) noexcept {
    if(!recipientD59(target))return false;
    for(const auto& o:in.nativeFrame.offers)if(o.boundsValid&&prox67::same(o.ref,target)&&prox67::usable(o,completed,in.now))
     return selfReturnD74(target)?prox67::distanceToOffer(o,in.palm[admittedSide].point)<=prox67::enter(ni,target,admittedSide):
      in.handsOnlyD53&&in.palmRayD70?prox67::pourRecipientD72(o,in.palm[admittedSide]):
      prox67::distanceToOffer(o,in.palm[admittedSide].point)<=prox67::enter(ni,target,admittedSide);
    return false;
   };
   const auto cancelTransferD59=[&]() noexcept {
    ordinaryTransferD59=ordinaryTransferTiltD59=false;ordinaryIntentTickD59=0;
    ordinaryReleasePending=ordinaryQueued=ordinaryTiltIntentD56=false;out.transferStateD59=5;
    out.nomination={in.context,{},in.now}; // cancelled intent no longer steers the native selector
   };
   const auto pinTransferD59=[&]() noexcept {
    // Retain this release recipient while the exact native pickup or selector
    // catches up. A nearer adjacent bowl cannot steal an already-open hand.
    // The consumer rechecks current bounds and native final selection as usual.
    decision.source=admittedSide?prox67::Source::Left:prox67::Source::Right;decision.ref=ordinaryDestination;
    decision.palm=in.palm[admittedSide];decision.gripping=false;decision.admissionReady=false;
    decision.gripSelectionOwnedCraft1=realHeld;decision.sourceAcquisitionD39=false;
    decision.hapticEdge=false;decision.latchedTick=0;
    decision.selfReturnRecipientD74=selfReturnD74(ordinaryDestination);
    decision.pourRecipientD72=false;
    if(decision.selfReturnRecipientD74){decision.heldSourceH21=admittedRef;decision.palmRayD70=false;}
    else if(in.handsOnlyD53&&in.palmRayD70){decision.pourRecipientD72=true;decision.heldSourceH21=admittedRef;decision.palmRayD70=false;}
    for(const auto& o:in.nativeFrame.offers)if(prox67::same(o.ref,ordinaryDestination)){
     decision.gap=prox67::distanceToOffer(o,in.palm[admittedSide].point);break;
    }
    out.nomination={in.context,decision,in.now};
   };
   if(ordinaryTransferD59&&!transferEnabledD59&&!selfReturnD74(ordinaryDestination)&&!ordinaryReturning)cancelTransferD59();
   if(released){
    // Remember this physical destination while the engine catches up with the
    // selection/attachment. An open hand must not migrate to a different target.
    ordinaryReleasePending=false;ordinaryTiltIntentD56=false;
    if(nearDestination(decision.ref)){ordinaryDestination=decision.ref;ordinaryReleasePending=true;}
    else if(nearDestination(admittedRef)){ordinaryDestination=admittedRef;ordinaryReleasePending=true;}
    if(ordinaryReleasePending&&transferEnabledD59&&localRecipientD59(ordinaryDestination)){
     ordinaryTransferD59=true;ordinaryTransferTiltD59=false;ordinaryIntentTickD59=in.now;
    }
   }
   // D75: retain the one release while the native source selector catches up.
   // This also covers a release before delayed Attach: promote the existing
   // intent only after its exact ordinary binding arrives, without another A.
   // It does not grant a new vessel transfer to hourglass, flask or book.
   if(ordinaryReleasePending&&!ordinaryTransferD59&&!ordinaryReturning&&!realHeld&&
      selfReturnD74(ordinaryDestination)&&localRecipientD59(ordinaryDestination)){
    ordinaryTransferD59=true;ordinaryTransferTiltD59=false;ordinaryIntentTickD59=in.now;
   }
   if(ordinaryTransferD59&&!ordinaryReturning){
    if((realHeld&&!ordinaryTransferTiltD59)||!localRecipientD59(ordinaryDestination))cancelTransferD59();
    else pinTransferD59();
   }else if(realHeld||(ordinaryReleasePending&&!nearDestination(ordinaryDestination)))ordinaryReleasePending=false;
   ordinaryWasHeld=realHeld;
   if(!ordinaryCarried){if(ordinaryTransferD59)out.transferStateD59=1;return out;} // actual native attachment still pending
   const auto atDestination=[&](prox67::Ref target) noexcept {
    return known&&observed.entity==target.entity&&int(observed.type)==target.slot&&
      prox67::fresh(ni.selected.tick,in.now)&&nearDestination(target);
   };
   // D56: restore the prior held-plate overturn request, using this exact
   // physical plate, its pickup quaternion and current native-selected vessel.
   // The existing native action still owns DetachResource/Detach and quantity.
   // Only a proven idle director may start a new tilt request; a used tilt is
   // one-shot for this admission, including an unacknowledged/refused request.
   const bool idleTiltD56=ac&&!in.action.parent&&in.nativeFrame.table.actionKnown&&
       !in.nativeFrame.table.action&&prox67::fresh(in.nativeFrame.table.tick,in.now);
   const auto& plateD56=in.tool[ordinaryNativeSideR93].binding;
   const bool plateBoundD56=plateD56&&plateD56.instance==in.context.instance&&plateD56.player==in.context.player&&
       plateD56.character==in.tool[ordinaryNativeSideR93].character&&plateD56.entity==admittedRef.entity&&plateD56.side==ordinaryNativeSideR93&&plateD56.type==0&&
       plateD56.attachment&&plateD56.object&&plateD56.lifetime==in.context.lifetime&&!plateD56.action;
   ordinaryTiltIntentD56=ordinaryTransferD59&&ordinaryTransferTiltD59;
   const bool transferPickupD59=ac&&(!in.action.parent||in.action.parent==operationParent);
   if((in.handsOnlyD53||transferEnabledD59)&&plateBoundD56&&admittedRef.slot==0&&ordinaryBound&&realHeld&&
      !ordinaryTiltUsedD56&&!ordinaryReturning&&!ordinaryTransferD59&&(transferEnabledD59?transferPickupD59:idleTiltD56)&&
      (decision.ref.slot==2||decision.ref.slot==6)&&
      (transferEnabledD59?(nearDestination(decision.ref)&&localRecipientD59(decision.ref)):atDestination(decision.ref))&&referenceRotationValid){
    float upD56[3]{},neutralD56[3]{};
    if(gesture::upVector(f.hand[admittedSide].rotation,upD56)&&gesture::upVector(referenceRotation,neutralD56)){
     float dotD56=0;for(int j=0;j<3;++j)dotD56+=upD56[j]*neutralD56[j];
     if(std::acos(std::clamp(dotD56,-1.f,1.f))>=.45f){
      ordinaryDestination=decision.ref;ordinaryReleasePending=ordinaryTiltIntentD56=true;
      if(transferEnabledD59){ordinaryTransferD59=ordinaryTransferTiltD59=true;ordinaryIntentTickD59=in.now;pinTransferD59();}
     }
    }
   }
   if(ordinaryTransferD59&&ordinaryTransferTiltD59&&!ordinaryReturning){
    float upD59[3]{},neutralD59[3]{};float dotD59=0;
    bool tippedD59=realHeld&&referenceRotationValid&&gesture::upVector(f.hand[admittedSide].rotation,upD59)&&gesture::upVector(referenceRotation,neutralD59);
    if(tippedD59){for(int j=0;j<3;++j)dotD59+=upD59[j]*neutralD59[j];tippedD59=std::acos(std::clamp(dotD59,-1.f,1.f))>=.45f;}
    if(!tippedD59)cancelTransferD59();
   }
   const bool transferIdleD59=!ordinaryTransferD59||idleTiltD56;
   const bool transferSelectedD59=!ordinaryTransferD59||(known&&ni.selected.tick>=ordinaryIntentTickD59);
   if(ordinaryTransferD59&&!ordinaryReturning){
    // TakeIngredient publishes hand membership BEFORE its director returns to
    // idle. Sending the only 80ms Use pulse then loses the release to native
    // busy rejection. Keep the physical intent, not a repeated input pulse.
    if(ac&&in.action.parent&&in.action.parent!=operationParent){cancelTransferD59();}
    else out.transferStateD59=!ordinaryOwnedD47?1:!transferIdleD59?2:3;
   }
   if(ordinaryReleasePending&&!ordinaryReturning&&!ordinaryQueued&&ordinaryOwnedD47){
    if(transferIdleD59&&transferSelectedD59&&atDestination(ordinaryDestination)){ordinaryReturnTick=in.now;ordinaryBaseline=ac?in.action.parent:0;
     ordinaryQueued=true;out.ordinaryReturnRequested=true;}
   }
   if(ordinaryQueued){
    out.use=false;
    if(!atDestination(ordinaryDestination)||(realHeld&&!ordinaryTiltIntentD56))ordinaryQueued=false;
    else if(!priorUse&&transferIdleD59&&transferSelectedD59){ordinaryTiltUsedD56=ordinaryTiltUsedD56||ordinaryTiltIntentD56;ordinaryTiltIntentD56=false;ordinaryQueued=false;ordinaryReleasePending=false;ordinaryReturning=true;ordinaryUseUntil=in.now+80;ordinaryReturnTick=in.now;out.ordinaryReturnRequested=true;
     if(ordinaryTransferD59){out.transferStateD59=4;ordinaryTransferD59=ordinaryTransferTiltD59=false;ordinaryIntentTickD59=0;}}
   }
   if(ordinaryReturning){
    // Keep the exact accepted return target nominated through its Use pulse;
    // the carried supply is deliberately excluded from normal hover selection.
    if(!realHeld&&selfReturnD74(ordinaryDestination)&&nearDestination(ordinaryDestination))pinTransferD59();
    instant_d72::Receipt instantReceiptD72{};
    if(instant_d72::receipt.read(instantReceiptD72)&&instant_d72::completedReturn(instantReceiptD72,
       in.context.instance,in.context.lifetime,admittedTick,in.context.poseEpoch,in.context.sourceEpoch,
       in.modeEpoch,admittedRef.slot,ordinaryDestination.slot,ordinaryNativeSideR93,ordinaryReturnTick,in.now)){
     admitted=false;clearOrdinary();out.admitted=false;out.ordinaryReturnObserved=true;out.use=false;
     c.passenger=out.passenger=false;c.parent=0;c.held[0]=c.held[1]=false;return out;
    }
    if(!ordinaryReturnParent&&ac&&in.action.parent&&in.action.parent!=ordinaryBaseline&&in.action.tick>=ordinaryReturnTick)
     ordinaryReturnParent=in.action.parent;
    if(ordinaryReturnParent&&ac&&in.action.tick>=ordinaryReturnTick&&in.action.parent!=ordinaryReturnParent){
     admitted=false;clearOrdinary();out.admitted=false;out.ordinaryReturnObserved=true;out.use=false;return out;}
    // A refused request cannot consume controller ownership forever. This is
    // not an accepted Return: no retry is emitted without a real new grip edge.
    if(!ordinaryReturnParent&&in.now>=ordinaryReturnTick&&in.now-ordinaryReturnTick>250){
     ordinaryReturning=false;ordinaryUseUntil=0;
    }else{
     out.use=in.now<ordinaryUseUntil&&known&&observed.entity==ordinaryDestination.entity&&int(observed.type)==ordinaryDestination.slot;
     out.passenger=c.passenger=true;
     // The native Return still owns its action, Use and Detach. Saved Free /
     // 6DOF keep the exact still-attached tool on the tracked wrist until that
     // native detach; an open Grip must not restore authored arm motion.
     c.trackedPresentation71=minigame_hands45::freeTrackedMode(minigame_hands45::modeFor(minigame_hands45::Alchemy));
     c.held[0]=c.held[1]=false;c.parent=ordinaryReturnParent;
     c.gestureTransferD67=c.trackedPresentation71&&ordinaryReturnParent&&ordinaryReturnParent!=ordinaryBaseline&&
      ac&&in.action.parent==ordinaryReturnParent&&in.action.tick>=ordinaryReturnTick;
     out.gesture.status=gesture::Status::NativePassenger;
     if(ordinaryReturnParent)out.nomination={in.context,{},in.now};
     return out;
    }
   }
   out.ordinaryHeld=ordinaryBound&&realHeld;out.logicalIngredientHeldD47=logicalBoundD47&&realHeld;
   c.parent=0;c.held[admittedSide]=out.ordinaryHeld; // logical herbs cannot authorize a tool transform
   out.gesture.status=(out.ordinaryHeld||out.logicalIngredientHeldD47)?gesture::Status::WorkStopped:gesture::Status::GripRequired;
   return out;
  }
  if(mortarType69&&mortarPair69){
   const bool leftGrippedR96=mortarLeft69; // retry/rearm below keeps the real squeeze
   const bool leftHeld=mortarLeft69||carriedLeftR96,releaseLeft=mortarReleasePending69;
   const bool atDestination=known&&decision.source==prox67::Source::Left&&(decision.gap<=prox67::enter(ni,decision.ref,1)||decision.aimedDestinationH21)&&
    (decision.ref.slot==6||decision.ref.slot==0)&&decision.ref.entity==observed.entity&&
    decision.ref.slot==int(observed.type)&&prox67::fresh(ni.selected.tick,in.now);
   float up[3]{},neutral[3]{};float heldTiltD78R2=-1;bool pouring=false;
   if(leftHeld&&referenceRotationValid&&
      gesture::upVector(f.hand[1].rotation,up)&&gesture::upVector(mortarNeutral69,neutral)){
    float dot=0;for(int j=0;j<3;++j)dot+=up[j]*neutral[j];heldTiltD78R2=std::acos(std::clamp(dot,-1.f,1.f));
    pouring=atDestination&&(decision.ref.slot==6||(in.handsOnlyD53&&decision.ref.slot==0))&&heldTiltD78R2>=.45f;
   }
   // Only an explicit native pre-call refusal permits deliberate rearm. A
   // repeated release, elapsed timeout, unknown receipt or entered Stop does not.
   const auto& retryResult72=in.mortarResult69;
   const bool sameRetry72=mortarAttempted69&&!mortarStopping69&&!mortarReturning69&&pairNow69&&ac&&
    mortarRequest69.serial&&retryResult72.serial==mortarRequest69.serial&&
    retryResult72.instance==in.context.instance&&retryResult72.lifetime==in.context.lifetime&&
    retryResult72.stopEntry72==mortar69::StopEntry72::NotEntered&&
    (retryResult72.status==mortar69::Status::Refused||retryResult72.status==mortar69::Status::Revoked)&&
    retryResult72.tick>=mortarRequest69.tick&&retryResult72.tick<=in.now&&
    alchemy_tool18::sameMortarGrip69(mortarRequest69.left,in.tool[1].binding)&&
    alchemy_tool18::sameMortarGrip69(mortarRequest69.right,in.tool[0].binding)&&
    (!in.action.parent||in.action.parent==mortarRequest69.parent)&&
    (!wc||(in.work.kind==semi::Mortar&&in.work.parent==mortarRequest69.parent));
   if(!sameRetry72)mortarRetryNoEntry72=mortarRetryOpen72=false;
   else{
    if(prox67::fresh(retryResult72.tick,in.now))mortarRetryNoEntry72=true;
    // R96: a carried (unsqueezed) mortar re-arms after it is held upright again.
    if(mortarRetryNoEntry72&&(carriedLeftR96?(heldTiltD78R2>=0&&heldTiltD78R2<.35f):!leftGrippedR96))mortarRetryOpen72=true;
    if(mortarRetryNoEntry72&&mortarRetryOpen72&&(leftGrippedR96||carriedLeftR96)){
     mortarAttempted69=false;mortarRetryNoEntry72=mortarRetryOpen72=false;
     mortarSerial69=0;mortarRequest69={};
     if(!in.action.parent)mortarParent69=mortarChild69=0;
    }
   }
   if(!mortarStopping69&&!mortarReturning69&&!mortarAttempted69&&pairNow69&&atDestination&&(releaseLeft||(pouring&&!mortarContentsTransferredD75))){
    mortarReleaseIntent70=releaseLeft;mortarTiltToDishD56=!releaseLeft&&pouring&&decision.ref.slot==0;mortarReleasePending69=false;
    mortarDestination69=decision.ref;mortarSerial69=std::max(in.now,mortarSerial69+1);mortarStopping69=true;
    mortarRequest69={in.tool[1].binding,in.tool[0].binding,mortarSerial69,in.now,f.poseEpoch,f.sourceEpoch,in.modeEpoch,mortarParent69,mortarChild69};
   }
   if(mortarStopping69){
    out.use=false;sc.manualUse=true;c.held[0]=c.held[1]=false;
    // D78R2: a pending held pour is current physical intent, not a seven-second
    // reservation after the hand has left the recipient or become upright.
    // Release placement remains committed. The native owner alone decides
    // whether this exact Stop was still unentered when withdrawal arrived.
    // A temporary missing native-selected readback is not physical withdrawal.
    // Current left-hand geometry/tilt retains the intent while native admission
    // remains independently required before any subsequent contextual placement.
    const bool samePhysicalTiltD78R2=pairNow69&&leftHeld&&heldTiltD78R2>=.45f&&
     decision.source==prox67::Source::Left&&prox67::same(decision.ref,mortarDestination69)&&
     (decision.gap<=prox67::enter(ni,decision.ref,1)||decision.aimedDestinationH21);
    if(!mortarReleaseIntent70&&!mortarRequest69.withdrawTickD78R2&&!samePhysicalTiltD78R2)
     mortarRequest69.withdrawTickD78R2=in.now;
    if(mortarParent69)out.mortarStop69=mortarRequest69;
    const auto& result=in.mortarResult69;
    const bool withdrawnD78R2=mortarRequest69.withdrawTickD78R2&&
     result.serial==mortarRequest69.serial&&result.instance==in.context.instance&&
     result.lifetime==in.context.lifetime&&result.tick>=mortarRequest69.withdrawTickD78R2&&result.tick<=in.now&&
     result.status==mortar69::Status::WithdrawnD78R2&&result.stopEntry72==mortar69::StopEntry72::NotEntered;
    if(withdrawnD78R2){
     mortarStopping69=mortarAttempted69=mortarTiltToDishD56=false;
     mortarRequest69={};out.mortarStop69={};
     mortarDestination69={};mortarReleaseIntent70=false;
     // This sample only resumes the existing pair; it emits neither a second
     // contextual A nor a replacement Stop. A later current tilt may request it.
     sc.manualUse=false;
    }else{
    const bool stopped=ac&&!in.action.parent&&pairNow69&&
     (!mortarParent69||(result.serial==mortarSerial69&&result.instance==in.context.instance&&result.lifetime==in.context.lifetime&&
      result.status==mortar69::Status::ReadyForNativePlace));
    // R103 (/tmp/r103/KCD2VR.log 09:55:59.826-09:56:17.719): the tilt that picked the mortar up next to the
    // bowl (selected=952226/0) armed a pour INTO THE BOWL. The grind then finished natively (G2 MORTAR_HANDOFF
    // status=2 at 09:56:08.083) but the pour only continued over that same bowl; he tilted over the kettle
    // (selected=952224/6) and nothing happened for 9.6 s until A. Once the grind is ready, the CURRENT
    // recipient under the left palm wins: a held tilt there pours there, and opening the left hand over
    // the kettle/bowl ("release as drop in") places there. Before this the release was ignored because the
    // old bowl request was still pending (mortarStopping69 blocks a new request).
    const bool releaseHereR103=releaseLeft&&atDestination&&(decision.ref.slot==6||decision.ref.slot==0);
    if(stopped&&!priorUse&&prox67::valid(decision.ref)&&!prox67::same(decision.ref,mortarDestination69)&&
       ((!mortarReleaseIntent70&&pouring&&atDestination)||releaseHereR103)){
     if(releaseHereR103){mortarReleaseIntent70=true;mortarReleasePending69=false;mortarTiltToDishD56=false;}
     else mortarTiltToDishD56=decision.ref.slot==0;
     mortarDestination69=decision.ref;
    }
    if(stopped&&atDestination&&prox67::same(decision.ref,mortarDestination69)&&!priorUse){
     mortarStopping69=false;mortarReturning69=true;mortarAttempted69=true;mortarUseUntil69=in.now+80;mortarReturnTick69=in.now;
    }else if(result.serial==mortarSerial69&&(result.status==mortar69::Status::Refused||result.status==mortar69::Status::Revoked)){
     mortarStopping69=false;mortarAttempted69=true;
    }
    out.gesture.status=gesture::Status::ReleaseAwaitingNativeReturn;
    if(!mortarReturning69)return out;
    }
   }
   if(mortarReturning69){
    // D75 restores the D72 contents-only connection omitted by the later
    // six-family adapter. Only THIS held tilt, pair and pot earn the native Use
    // branch; release/home and mortar-to-dish retain their existing lifecycle.
    c.heldMortarPourD75=!mortarReleaseIntent70&&leftHeld&&pouring&&pairNow69&&mortarDestination69.slot==6&&atDestination;
    instant_d72::Receipt pouredD75{};
    if(instant_d72::receipt.read(pouredD75)&&instant_d72::completedMortarPourD75(pouredD75,
       in.context.instance,in.context.lifetime,admittedTick,f.poseEpoch,f.sourceEpoch,in.modeEpoch,mortarReturnTick69,in.now)){
     mortarContentsTransferredD75=true;mortarReturning69=mortarAttempted69=false;mortarUseUntil69=0;
     mortarParent69=mortarChild69=mortarReturnParent69=0;out.use=false;
     c.parent=0;c.held[0]=in.palm[0].grip>=.35;c.held[1]=leftHeld;c.heldMortarPourD75=false;
     sc.manualUse=true;return out;
    }
    sc.manualUse=true;out.passenger=c.passenger=true;c.trackedPresentation71=false;out.gesture.status=gesture::Status::NativeReturn;
    if(!mortarReturnParent69&&ac&&in.action.parent&&in.action.placeMortar69&&in.action.tick>=mortarReturnTick69)
     mortarReturnParent69=in.action.parent;
    c.parent=mortarReturnParent69;c.held[0]=c.held[1]=false;
    if(in.controllerMortar70&&mortarHome70.valid){
    sc.manualUse=false;out.passenger=c.passenger=false;c.held[0]=c.held[1]=pairNow69;c.controllerMortar70=true;c.trackedPresentation71=true;
    // Native target admission is already fixed. Current native bounds, rather
    // than a competing gaze selection, authorize pour and return positions.
    bool pot70=false,home70=false,dish70=false;float up70[3]{},neutral70[3]{};float tilt70=0;
    for(const auto& offer:completed.offers)if(prox67::usable(offer,completed,in.now)&&(offer.ref.slot==0||offer.ref.slot==6)){
     const auto close=prox67::closest(offer,in.palm[1].point);
     if(close.valid&&close.distance<=prox67::enter(ni,offer.ref,1)&&
        (!in.palmRayD70||!ni.handsOnlyD53||prox67::pourRecipientD72(offer,in.palm[1]))){
      if(offer.ref.slot==0&&prox67::same(offer.ref,mortarDestination69))dish70=true;
      if(offer.ref.slot==6&&prox67::same(offer.ref,mortarDestination69))pot70=true;
     }
    }
    for(const auto& offer:completed.offers)if(offer.ref.slot==6&&offer.boundsValid&&prox67::usable(offer,completed,in.now))
     home70=mortarHome70.atRest70(offer.ref.entity,completed.table[2].entity,offer.bounds,{in.palm[1].point.x,in.palm[1].point.y,in.palm[1].point.z});
    const bool orientation70=gesture::upVector(f.hand[1].rotation,up70)&&gesture::upVector(mortarNeutral69,neutral70);
    if(orientation70){float dot70=0;for(int j=0;j<3;++j)dot70+=up70[j]*neutral70[j];tilt70=std::acos(std::clamp(dot70,-1.f,1.f));}
    const bool place70=wc&&in.work.parent==mortarReturnParent69&&in.work.kind==semi::MortarPlace70;
    const bool pouring70=place70&&in.work.phase==0&&(mortarDestination69.slot==6||mortarTiltToDishD56);
    const bool pourTargetD56=mortarDestination69.slot==6?pot70:dish70;
    // Placement was intentionally admitted by release. Requiring a fresh Grip
    // here deadlocked that same native action until the user squeezed again.
    // Only the accepted dish/home and exact joined pair earn the remaining
    // interval. Release placement is upright; a held tilt request pours at the
    // exact destination, then requires the existing upright native-home phase.
    const bool placingGrip70=leftHeld||mortarReleaseIntent70;
    // D72: release already committed THIS native placement and recipient.
    // Its pouring phase needs the same retained release intent as its return
    // phase. A held pour still requires Grip; a different pair/parent/target
    // never borrows this permission.
    const bool earned70=orientation70&&place70&&(pouring70?(pairNow69&&placingGrip70&&pourTargetD56&&tilt70>=.45f):(pairNow69&&placingGrip70&&(in.work.phase==0?dish70:home70)&&tilt70<=.35f));
    if(place70&&in.work.phase<2&&!pairNow69)mortarNativeRecovery70=true;
    if(place70&&in.work.phase==1&&pairNow69&&home70&&!leftHeld)mortarReleaseIntent70=true;
    sc.hand=1;sc.totalSeconds=mortarPour70.step(in.work,f,earned70);sc.clockStop=!earned70;
    if(place70&&in.work.phase>=2)sc.manualUse=true; // real native Detach: cleanup resumes natively
    out.gesture.status=earned70?(pouring70?gesture::Status::Pouring:gesture::Status::NativeReturn):gesture::Status::WorkStopped;
    }
    if(mortarNativeRecovery70){
     sc.manualUse=true;out.passenger=c.passenger=true;c.trackedPresentation71=false;c.controllerMortar70=false;c.held[0]=c.held[1]=false;
     out.gesture.status=gesture::Status::NativeReturn; // native-only recovery; never reacquire mid-clip
    }
    out.use=!mortarReturnParent69&&in.now<mortarUseUntil69&&atDestination&&prox67::same(decision.ref,mortarDestination69);
    const auto& table=in.nativeFrame.table;
    if(mortarReturnParent69&&ac&&!in.action.parent&&table.handTypesKnown68&&table.handType68[0]==21&&table.handType68[1]==21){
     out.mortarPlaced69=true;out.use=false;admitted=false;out.admitted=false;mortarPair69=false;mortarReturning69=false;
     selection.retire(ni);out.nomination={in.context,{},in.now};
    }else if(!mortarReturnParent69&&in.now>=mortarReturnTick69&&in.now-mortarReturnTick69>250){
     mortarReturning69=false;out.passenger=c.passenger=false; // native refusal; explicit A remains recovery
    }
    return out;
   }
   if(pairNow69&&!wc){
    // Native grind ended, genuine pair retained. Carry it without inventing a
    // new work clock or discarding it merely because the old parent is gone.
    c.parent=0;c.held[0]=in.palm[0].grip>=.35;c.held[1]=leftHeld;sc.manualUse=true;
    out.use=false;out.gesture.status=gesture::Status::WorkStopped;return out;
   }
  }
  const auto joined=[&](int h,int type) noexcept {
   const auto& t=in.tool[unsigned(h)];return t.joined&&t.side==h&&t.type==type&&t.entity&&t.character&&
    t.instance==in.context.instance&&t.lifetime==in.context.lifetime&&t.poseEpoch==f.poseEpoch&&prox67::fresh(t.tick,in.now)&&
    prox67::same(completed.table[unsigned(type)],prox67::Ref{type,t.entity})&&wc&&(!t.parent||t.parent==in.work.parent)&&
    prox67::usable(in.palm[unsigned(h)],h,ni)&&f.hand[h].valid;
  };
  const int type=admittedRef.slot;bool nativeGrant=false;gesture::Input gi{};gi.enabled=true;gi.nativeReturnAuthorized=in.nativeReturnAuthorized;
  if(type>=13&&type<=16){gi.referenceRotationValid=referenceRotationValid;for(int j=0;j<4;++j)gi.referenceRotation[j]=referenceRotation[j];}
  const bool kindMatches=(type>=13&&type<=16&&in.work.kind==semi::Resource)||
   ((type==2||type==3)&&in.work.kind==semi::Mortar)||(type==18&&in.work.kind==semi::Bellows);
  if(wc&&!kindMatches)sc.manualUse=true;
  // Native Bellows queues work before publishing handType18. The already
  // admitted, current registered source can pause only this exact parent;
  // work credit and hand presentation still wait for the native grant below.
  if(type==18&&admittedSide==1&&held&&!releasedAdmission&&wc&&kindMatches&&ac&&
     in.action.parent==in.work.parent&&operationParent==in.work.parent){
   c.parent=in.work.parent;c.clockAdmissionD47=true;sc.hand=1;
  }
  gi.work={in.work.instance,in.work.parent,in.work.child,in.work.lifetime,in.work.tick,in.work.token,in.work.kind,in.work.phase,in.work.working,in.work.ready};
  const auto& nt=in.nativeFrame.table;
  const bool bellowsOwner=wc&&type==18&&admittedSide==1&&in.work.kind==semi::Bellows&&completed.complete&&
   nt.actionKnown&&nt.action==in.work.parent&&nt.handTypesKnown68&&nt.handType68[1]==18&&
   prox67::same(nt.refs[18],admittedRef)&&prox67::usable(in.palm[1],1,ni)&&f.hand[1].valid;
  if(wc&&((type>=13&&type<=16&&admittedSide==1&&in.work.kind==semi::Resource&&joined(1,type)&&in.tool[1].entity==admittedRef.entity)||
    ((type==2||type==3)&&in.work.kind==semi::Mortar&&joined(0,3)&&joined(1,2))||
    bellowsOwner))nativeGrant=true;
  // Native queue interception may precede native attachment publication. It is
  // enabled after exact input admission, but pose ownership below still waits
  // for the actual native entity/hand/type join.
  if(!nativeGrant){out.gesture.status=gesture::Status::GripRequired;return out;}
  c.parent=in.work.parent;operationParent=in.work.parent;
  if(bellowsOwner&&releasedAdmission){
   // Physical release can precede native READY/work publication. It must not
   // wait for a closed-Grip acquisition before restoring its own native return.
   // This resumes the genuine pump/return, not an abort or a synthetic Use.
   out.use=false;out.gesture.releaseRequested=true;out.gesture.status=gesture::Status::NativeReturn;
   out.gesture.parent=in.work.parent;out.gesture.child=in.work.child;out.gesture.token=in.work.token;
   out.gesture.hand=1;out.gesture.totalSeconds=work.totalSeconds;sc.hand=1;sc.totalSeconds=work.totalSeconds;
   sc.manualUse=true;c.held[0]=c.held[1]=false;return out;
  }
  if(bellowsOwner){
   gi.tableContact={true,false,in.context.instance,in.work.parent,in.context.lifetime,nt.tick,f.poseEpoch,f.sourceEpoch,
    admittedRef.entity,1,18,int(nt.handType68[1]),{}};
   gi.tableContact.rangeCraft1=in.proximityCraft1?float(reach_craft1::radius(18,1)):.10f;
   gi.palm[1]=in.world[1];
   for(const auto& o:completed.offers)if(prox67::same(o.ref,admittedRef)&&o.boundsValid&&prox67::usable(o,completed,in.now)){
    const auto point=prox67::closest(o,in.palm[1].point);if(point.valid){gi.tableContact.geometryValid=true;
     gi.tableContact.tick=o.tick;gi.tableContact.point[0]=float(point.point.x);gi.tableContact.point[1]=float(point.point.y);gi.tableContact.point[2]=float(point.point.z);}break;}
  }
  // Reclaim only the same actual LEFT attachment before a return request. A release
  // during delayed Attach must not permanently swallow a deliberate new Grip.
  // Phase0 may restore the hand lease, but never proves a work interval or that
  // native Use has not already happened. It therefore grants no work credit.
  const auto& resourceToolR17=in.tool[1];const auto& resourceBindingNowR17=resourceToolR17.binding;
  const bool resourceAttachedR17=resourceType&&ac&&operationParent&&in.action.parent==operationParent&&
   resourceToolR17.parent==operationParent&&resourceBindingNowR17.instance==in.context.instance&&
   resourceBindingNowR17.player==in.context.player&&resourceBindingNowR17.character==resourceToolR17.character&&
   resourceBindingNowR17.attachment&&resourceBindingNowR17.object&&resourceBindingNowR17.entity==admittedRef.entity&&
   resourceBindingNowR17.side==1&&resourceBindingNowR17.type==admittedRef.slot&&
   resourceBindingNowR17.action==operationParent&&resourceBindingNowR17.actionClass&&resourceBindingNowR17.lifetime==in.context.lifetime;
  if(resourceAttachedR17){
   if(resourceBindingR17&&!alchemy_tool18::same(resourceBindingR17,resourceBindingNowR17))resourceResumeClosedR17=true;
   else resourceBindingR17=resourceBindingNowR17;
  }
  if(resourceType&&releasedAdmission&&!resourceResumeClosedR17&&!work.acquired&&
     !in.nativeReturnAuthorized&&admittedSide==1&&in.palm[1].grip>=.55&&f.hand[1].grip>=.55f&&
     resourceAttachedR17&&resourceChildR17&&resourceChildR17==in.work.child&&resourceTokenR17==in.work.token&&
     in.work.phase>=0&&in.work.phase<2){
   releasedAdmission=false;useUntil=0;holdHint=false;out.use=false; // never another contextual A
  }
  // Grip may have been released while native Take/Attach was still pending.
  // Remember that real edge; do not require the gesture controller to acquire
  // a now-open hand before the exact newly installed native child can return.
  if(resourceType&&releasedAdmission){
   out.use=false;out.gesture.parent=in.work.parent;out.gesture.child=in.work.child;out.gesture.token=in.work.token;
   out.gesture.hand=1;out.gesture.totalSeconds=work.totalSeconds;out.gesture.clockStop=true;
   sc.hand=1;sc.totalSeconds=work.totalSeconds;c.held[0]=c.held[1]=false;
   if(!in.nativeReturnAuthorized&&in.work.phase==1&&in.work.working&&in.work.child&&in.work.token){
    resourceResumeClosedR17=true;out.gesture.enabled=true;out.gesture.releaseRequested=true;out.gesture.status=gesture::Status::ReleaseAwaitingNativeReturn;
   }else if((in.nativeReturnAuthorized||in.work.phase==2)&&in.work.child&&in.work.token){
    // The game's real Use marker has already executed. Restore native return;
    // never cancel it as though its recipe effect had not happened.
    resourceResumeClosedR17=true;out.gesture.releaseRequested=true;out.gesture.status=gesture::Status::NativeReturn;sc.manualUse=true;
   }else out.gesture.status=gesture::Status::GripRequired;
   return out;
  }
  // Native Attach/work may arrive after the original .55+ physical admission.
  // Its same held Resource uses retained Grip; never require a second squeeze.
  // D86: Use is the recipe event, not the end of the native particle/return
  // timeline. Continue only the same genuine attachment and original child;
  // no contextual A, regrip, second Use, global timer or speed multiplier.
  gi.resourceTailD86=resourceAttachedR17&&!releasedAdmission&&held&&admittedSide==1&&
   resourceChildR17==in.work.child&&resourceTokenR17==in.work.token&&in.work.kind==semi::Resource&&
   in.work.phase==2&&in.work.working;
  constexpr unsigned homeMaskD86=carafe_d85::Valid|carafe_d85::Grip|carafe_d85::HomeKnown|carafe_d85::Home;
  gi.resourceHomeD86=gi.resourceTailD86&&(c.carafeD85&homeMaskD86)==homeMaskD86;
  sc.resourceReturnHomeD86=gi.resourceHomeD86;
  gi.retainedResourceGripR23=resourceAttachedR17&&!releasedAdmission&&!resourceResumeClosedR17&&
   resourceChildR17==in.work.child&&resourceTokenR17==in.work.token&&in.work.phase==1&&in.work.working&&
   admittedSide==1&&held&&f.hand[1].grip>=.35f;
  gi.retainedMortarGripD39=(type==2||type==3)&&pairNow69&&mortarLeft69&&mortarRightGripD39;
  // D55 mode3 keeps the exact pestle on the tracked wrist until native detach.
  // Its visible owner must also permit grinding with an open right grip. The
  // current native pair, parent/work, left hold and actual contact/motion are
  // still mandatory; this does not acquire a tool or generate work at rest.
  gi.trackedPestleD56=(type==2||type==3)&&in.handsOnlyD53&&pairNow69&&mortarLeft69&&
   !mortarStopping69&&!mortarReturning69&&!mortarNativeRecovery70;
  gi.pestleParkedD47=(type==2||type==3)&&bridge::pestleParkedD47(in.pestlePresentationD47,in.tool[0],in.now,f.sourceEpoch,in.modeEpoch);
  for(int h=0;h<2;++h){const auto& t=in.tool[unsigned(h)];const bool needed=(type==2||type==3)||h==admittedSide;
   if(type==18)continue; // mounted table-contact lease never owns a held-tool transform
   if(!needed)continue;
   gi.grip[h]={true,in.context.instance,in.work.parent,in.context.lifetime,t.tick,f.poseEpoch,f.sourceEpoch,t.entity,h,t.type};
   gi.palm[h]=in.world[unsigned(h)];
   const bool retained=(type==2||type==3)?(h==0?mortarRightGripD39:mortarLeft69):
    (work.acquired||gi.retainedResourceGripR23);
   c.held[h]=(h==0&&gi.trackedPestleD56)||in.palm[unsigned(h)].grip>=(retained?.35:.55);
  }
  if(gi.pestleParkedD47)c.held[0]=false; // each hand's tool presentation is independent; grinding still needs both
  // G1R1: distance is to the actual copied native bounds, independently for
  // this carrying palm. A point produced by the gaze ray cannot move the pot's
  // recipient volume. This is conservative pot proximity, NOT a mesh mouth.
  for(const auto& o:completed.offers)if(o.ref.slot==6&&prox67::usable(o,completed,in.now)&&gi.palm[admittedSide].valid){
   const auto closest=prox67::closest(o,in.palm[unsigned(admittedSide)].point);
   if(closest.valid)gi.pot={true,in.context.instance,in.context.lifetime,o.tick,f.poseEpoch,f.sourceEpoch,o.ref.entity,6,
    {float(closest.point.x),float(closest.point.y),float(closest.point.z)},.20f};break;}
  if((type==2||type==3)&&gi.palm[1].valid)gi.mortar={true,in.context.instance,in.context.lifetime,gi.palm[1].tick,
   f.poseEpoch,f.sourceEpoch,in.tool[1].entity,2,{gi.palm[1].point[0],gi.palm[1].point[1],gi.palm[1].point[2]},.12f};
  if(type==2||type==3){
   gi.mortarContact.required=in.proximityCraft1;
   const prox67::Offer* pestle=nullptr;const prox67::Offer* mortar=nullptr;
   // Destination nomination excludes the two carried tools from completed.
   // Grinding still needs their unmodified, current native bound observations.
   for(const auto& o:in.nativeFrame.offers)if(o.boundsValid&&prox67::usable(o,completed,in.now)){
    if(o.ref.slot==3&&o.ref.entity==in.tool[0].entity)pestle=&o;
    if(o.ref.slot==2&&o.ref.entity==in.tool[1].entity)mortar=&o;
   }
   if(pestle&&mortar){const auto touch=mortar_contact_repair::contact(pestle->bounds,mortar->bounds);
    gi.mortarContact={in.proximityCraft1,touch.valid,in.context.instance,in.work.parent,in.context.lifetime,
     (std::min)(pestle->tick,mortar->tick),f.poseEpoch,f.sourceEpoch,pestle->ref.entity,mortar->ref.entity,touch.gap,touch.surfaceRadiusD41};}
  }
  out.gesture=work.step(gi,f,in.now);sc.parent=out.gesture.parent?out.gesture.parent:in.work.parent;
  sc.child=in.work.child;sc.token=in.work.token;sc.hand=out.gesture.hand;sc.totalSeconds=out.gesture.totalSeconds;sc.clockStop=out.gesture.clockStop;
  sc.use=out.gesture.use;sc.useUntil=out.gesture.useUntil;out.use=out.use||out.gesture.use;
  // Releasing the controller immediately retires its pose lease. The gesture
  // may keep the exact native work child paused until a proven native return;
  // releaseRequested is a request for the host, not automatic work completion.
  if(resourceType&&(out.gesture.releaseRequested||out.gesture.completedHeld))resourceResumeClosedR17=true;
  if(type==2||type==3){
   c.held[0]=c.held[0]&&(mortarRightGripD39||gi.trackedPestleD56)&&!gi.pestleParkedD47;
   c.held[1]=c.held[1]&&mortarLeft69;
  }else if(out.gesture.releaseRequested||!held){c.held[0]=c.held[1]=false;}
  if(out.gesture.status==gesture::Status::NativeReturn)sc.manualUse=true;
  return out;
 }
};
}
