// Actual default local writer and combat ownership, with retained native leaves.
// A combat-qualified squeeze must not replace gradual, tuned finger presentation.
#define main retained_finger_translation_main_h34
#include "finger_translation_h31_bridge.cpp"
#undef main
static bool drawDefaultH34(CoherentState6689& s,int side,const TrackingSnapshot& t,
                           const HandContact6695* contact=nullptr,float hover=0.f){
 return ApplyFingerLocals6689(s,side,side?t.left_grip:t.right_grip,
     side?t.left_trigger:t.right_trigger,contact,hover);
}
static void squeezeH34(TrackingSnapshot& t,int side,float grip,float trigger){
 if(side){t.left_grip=grip;t.left_trigger=trigger;}else{t.right_grip=grip;t.right_trigger=trigger;}
}
static bool sameDigitH34(const CoherentState6689& s,const std::array<QuatT28,kPoseCapacity6689>& before,int side,int digit){
 for(int k=0;k<s.fingers[side][digit].count;++k){const int id=s.fingers[side][digit].id[k];
  if(std::memcmp(&s.local[id],&before[id],sizeof(QuatT28)))return false;}
 return true;
}
#ifndef KCD2VR_F7_TRIAL_HOST
#define KCD2VR_F7_TRIAL_HOST 1
#endif
static bool trialDrawH7(CoherentState6689& s,int side,float g,float t,const HandContact6695* c=nullptr,float hover=0.f){
#if KCD2VR_F7_TRIAL_HOST
 return ApplyFingerLocals6689(s,side,g,t,c,hover,true);
#else
 return ApplyFingerLocals6689(s,side,g,t,c,hover); // Literal parent has no trial; actual default remains its only route.
#endif
}
static bool trialGeometryH7(const CoherentState6689& s,int side,float g,float t,std::uint64_t held,V3(&p)[6],FingerContactGeometry103& f,float hover=0){
#if KCD2VR_F7_TRIAL_HOST
 return PalmGeometry103(s,side,{},g,t,held,p,f,hover,true);
#else
 return PalmGeometry103(s,side,{},g,t,held,p,f,hover);
#endif
}
static void trialChecksH7(){
 static CoherentState6689 s{};init31(s);const auto anatomy=s.restLocal;
 for(int side=0;side<2;++side){
  ck31(ApplyFingerLocals6689(s,side,.41f,.23f,nullptr,.3f),"default partial baseline before absent native trial bank");const auto fallback=s.local;
  ck31(trialDrawH7(s,side,.41f,.23f,nullptr,.3f),"unavailable native curve transparently keeps default writer");
  for(int f=0;f<5;++f)ck31(sameDigitH34(s,fallback,side,f),"unavailable bank preserves exact default digits");
#if KCD2VR_F7_TRIAL_HOST
  // Explicit selected-curve leaf. Actual bounded decoder/CRC/publication is
  // exercised independently in the complete coherent-pose fixture.
  s.nativeCurveClosedH7[side]=s.closed[side];s.nativeCurveClosedH7[side].ready=true;
  for(int n=0;n<s.fingerCount[side];++n)QToQuatT(s.nativeCurveClosedH7[side].local[n],QP(pc::from_rotation_vector({.03*(side+1),.45+.004*n,-.02*side})));
#endif
  ck31(ApplyFingerLocals6689(s,side,0,0,nullptr,.3f),"accepted open baseline with actual tuning");const auto opened=s.local;
  ck31(trialDrawH7(s,side,0,0,nullptr,.3f),"trial open writer");
  for(int f=0;f<5;++f)ck31(sameDigitH34(s,opened,side,f),"trial preserves tuned open endpoint byte exactly");
  ck31(trialDrawH7(s,side,0,0),"trial plain open before channel split");const auto plainOpen=s.local;
  ck31(trialDrawH7(s,side,1,0),"trial Grip only");
  ck31(sameDigitH34(s,plainOpen,side,1),"trial Grip still cannot close index");
  ck31(trialDrawH7(s,side,0,1),"trial Trigger only");
  for(int f:{0,2,3,4}){ck31(trialDrawH7(s,side,0,0),"trial independent open comparison");const auto base=s.local;ck31(trialDrawH7(s,side,0,1),"trial independent Trigger comparison");ck31(sameDigitH34(s,base,side,f),"trial Trigger leaves nonindex open");}
  for(float input:{.1f,.35f,.60f,.89f,.90f,.91f,1.f}){
   ck31(trialDrawH7(s,side,input,input,nullptr,.4f),"trial continuous analog writer");
   V3 points[6]{};FingerContactGeometry103 geometry{};ck31(trialGeometryH7(s,side,input,input,0,points,geometry,.4f),"trial same contact geometry recipe");
   for(int f=0;f<5;++f){QuatT28 parent{};parent.q[3]=1;auto point=pc::Vec3{};
    const auto& chain=s.fingers[side][f];
    for(int k=0;k<chain.count;++k){const int id=chain.id[k];
     ck31(near31(PV(FromT(s.local[id])),PV(FromT(s.restLocal[id])),1e-12),"trial keeps every anatomical fixed translation at partial and terminal");
     const auto& c=geometry.curve[f];const auto q=pc::slerp(c.open[k],c.closed[k],geometry.input[f]);
     ck31(q&&pc::angle(PQ(QFromQuatT(s.local[id])),*q)<2e-6,"trial local writer and actual contact curve quaternion agree");
     parent=AbsoluteFromRelative(parent,s.local[id]);
#if KCD2VR_F7_TRIAL_HOST
     if(input>=.90f){const int n=[&](){int n=k;for(int j=0;j<f;++j)n+=s.fingers[side][j].count;return n;}();
      ck31(pc::angle(PQ(QFromQuatT(s.local[id])),PQ(QFromQuatT(s.nativeCurveClosedH7[side].local[n])))<2e-6,"90percent reaches raw independently mapped native-curve endpoint");}
#else
     if(input>=.90f)ck31(pc::angle(PQ(QFromQuatT(s.local[id])),pc::from_rotation_vector({.03*(side+1),.45+.004*(f*4+k),-.02*side}))<2e-6,"parent lacks requested raw native-curve terminal");
#endif
    }
    ck31(near31(PV(FromT(parent)),PV(points[f+1]),3e-7),"trial displayed chain tip equals actual contact FK tip");
   }
  }
  V3 held0[6]{},held1[6]{};FingerContactGeometry103 g0{},g1{};
  ck31(PalmGeometry103(s,side,{},.91f,.91f,123,held0,g0)&&trialGeometryH7(s,side,.91f,.91f,123,held1,g1),"held native geometry retains original owner");
  bool heldSame=std::memcmp(held0,held1,sizeof(held0))==0;
  for(int f=0;f<5;++f){const auto& a=g0.curve[f];const auto& b=g1.curve[f];
   heldSame=heldSame&&a.count==b.count&&g0.input[f]==g1.input[f]&&g0.enabled[f]==g1.enabled[f]&&near31(a.rootClosedShiftRatioH31,b.rootClosedShiftRatioH31,1e-12);
   for(unsigned k=0;k<a.count;++k)heldSame=heldSame&&pc::angle(a.open[k],b.open[k])<1e-12&&pc::angle(a.closed[k],b.closed[k])<1e-12&&near31(a.offset[k],b.offset[k],1e-12);}
  ck31(heldSame,"trial never changes held curve geometry (typed fields, no struct-padding comparison)");
  V3 fitPoints[6]{};FingerContactGeometry103 fitGeometry{};ck31(trialGeometryH7(s,side,1,1,0,fitPoints,fitGeometry),"trial contact-fit source uses selected native curve");
  const auto& middleCurve=fitGeometry.curve[2];const auto fit=pc::fit_finger_surface103(middleCurve,1,{}, {},{{.11,0,0},{-1,0,0}},.008);
  ck31(fit.valid,"actual production finger surface solver reads the selected trial curve");
  HandContact6695 contact{};contact.fingerContact[2]=true;contact.curl[2]=float(fit.curl);ck31(trialDrawH7(s,side,1,1,&contact),"actual fitted contact curl retains priority over terminal request");
  QuatT28 actualFit{};actualFit.q[3]=1;for(int k=0;k<s.fingers[side][2].count;++k)actualFit=AbsoluteFromRelative(actualFit,s.local[s.fingers[side][2].id[k]]);
  const auto finalFit=pc::finger_clearance103(middleCurve,contact.curl[2],{}, {},{{.11,0,0},{-1,0,0}},.008);
  ck31(finalFit.valid&&near31(PV(FromT(actualFit)),finalFit.tip,3e-7),"actual fitted trial renderer equals the collision solver's final FK tip");
  ck31(trialDrawH7(s,side,.6f,.4f),"trial partial before OFF");
  ck31(ApplyFingerLocals6689(s,side,.41f,.23f,nullptr,.3f),"trial OFF immediately restores accepted recipe");
  for(int f=0;f<5;++f)ck31(sameDigitH34(s,fallback,side,f),"OFF recipe byte identical despite retained native trial bank");
 }
 ck31(std::memcmp(anatomy.data(),s.restLocal.data(),sizeof(anatomy))==0,"trial does not write original anatomy");
}

static unsigned nativeReads28=0,nativeOriginal28=0;
static const QuatT28* __fastcall nativeGetter28(void*,int id){++nativeReads28;return &g_pose6689.original.at(id);}
static void __fastcall nativeOriginalLeaf28(void*){++nativeOriginal28;}
static void handObservationChecks28(){
 namespace h=kcd2vr::diagnostics::hands28;namespace cap=kcd2vr::diagnostics::capture168;
 static std::array<unsigned char,64> ownerBytes{};static std::array<unsigned char,500*0x30> jointBytes{};
 auto& s=g_pose6689;s={};init31(s);s.ready=true;s.owner=ownerBytes.data();s.character=reinterpret_cast<void*>(0x12345500);
 s.rig.head=472;s.rig.item_r=411;s.rig.item_l=446;
 void* table=jointBytes.data();std::memcpy(ownerBytes.data()+8,&table,8);
 for(unsigned i=0;i<s.count;++i){const auto p=std::int16_t(s.parent[i]);std::memcpy(jointBytes.data()+i*0x30+0x16,&p,2);}
 g_pose_epoch6689=61;g_pose_restore_fault6689=false;g_weapon_request_epoch6694=72;
 TrackingSnapshot poseSnapshot{};poseSnapshot.sequence=53;poseSnapshot.tick=hostTickCount64=70000;poseSnapshot.timeNs=12345678;poseSnapshot.source_epoch=6;poseSnapshot.right_grip=.65f;poseSnapshot.right_trigger=.3f;
 void* pose=s.original.data();void* owner=s.owner;void* skinChar=s.character;void* skinAnim=nullptr;void* desc=nullptr;
 const auto count=s.count;const auto rig=s.rig;PoseGetFn b1fn=&nativeGetter28;const bool rawOk=true,rigOk=true;
 auto validationTick=hostTickCount64;nativeReads28=0;cap::gate.mask=0;
 {CoherentTransaction6689 transaction;std::optional<KickOutputProbe23> kickOutput23;
 #include <finger_output_begin28.inc>
 g_skinning_core_original=&nativeOriginalLeaf28;
 #include <vrik_callback_original_h29r4.inc>
 transaction.Restore();
 #include <finger_output_finish28.inc>
 ck31(!fingerOutput28&&nativeReads28==0&&nativeOriginal28==1,"Hands OFF actual begin/original/finish slices do zero new native reads and original once");}
 cap::gate.select(cap::Domain::Hands,true,hostTickCount64);
 {CoherentTransaction6689 transaction;std::optional<KickOutputProbe23> kickOutput23;
 #include <finger_output_begin28.inc>
 #include <vrik_callback_original_h29r4.inc>
 transaction.Restore();
 #include <finger_output_finish28.inc>
 ck31(!fingerOutput28&&nativeReads28==0&&nativeOriginal28==2,"legacy Hands-only actual callback adds zero native hand reads and still forwards original once");}
 cap::gate.select(cap::Domain::HandReference,true,hostTickCount64);h::head=h::tail=0;
 std::optional<h::Record> native;
 {CoherentTransaction6689 transaction;std::optional<KickOutputProbe23> kickOutput23;
 #include <finger_output_begin28.inc>
 #include <vrik_callback_original_h29r4.inc>
 transaction.Restore();
 #include <finger_output_finish28.inc>
 native=fingerOutput28;
 ck31(bool(native)&&native->hand[0].count==20&&native->hand[1].count==20&&nativeReads28>40&&nativeOriginal28==3,"native reference captures both complete current mapped B1 hands and forwards original once");
 ck31(!native->published&&!native->hand[0].joints[0].finalKnown,"native-only observation never claims a committed VR endpoint");
 for(int side=0;side<2;++side)for(int j=0;j<20;++j){const auto& q=native->hand[side].joints[j];const auto actual=RelativeFromAbsolute(s.original[q.parent],s.original[q.id]);
  ck31(q.nativeKnown&&pc::angle(q.native.q,PQ(QFromQuatT(actual)))<1e-7&&near31(q.native.p,PV(FromT(actual))),"incoming locals come from actual native parent-child pair, not reconstructed curve");}}
 const auto before=nativeReads28;ck31(!BeginFingerOutput28(pose,owner,skinChar,skinAnim,count,rig,b1fn,poseSnapshot,hostTickCount64+99)&&nativeReads28==before,"99ms repeated callback performs no joint reads or record construction");
 hostTickCount64+=100;poseSnapshot.tick=hostTickCount64;poseSnapshot.sequence++;
 {CoherentTransaction6689 transaction;transaction.epoch=61;transaction.tracking=poseSnapshot;
 auto captured=BeginFingerOutput28(pose,owner,skinChar,skinAnim,count,rig,b1fn,poseSnapshot,hostTickCount64);ck31(bool(captured),"100ms periodic boundary opens exact next observation");
 s.freeObserved28[0]=false;ck31(ApplyFingerLocals6689(s,0,.65f,.3f),"same source executes real default sole finger writer");Compose6689(s);
 // Completion booleans are explicit controlled publication leaves. Their real
 // producer placement is checked by the production callback/static receipt;
 // this fixture exercises the exact final copier and all rejection joins.
 transaction.traceArmH29[0].fingerCall=transaction.traceArmH29[0].fingerOk=true;transaction.committed=transaction.restored=transaction.fingerPublished28=true;
 captured->originalReturned=true;FinishFingerOutput28(*captured,transaction);
 ck31(captured->published&&captured->hand[0].writer&&!captured->hand[0].trialActive,"committed same-transaction default recipe is explicitly selected");
 ck31(captured->hand[0].joints[0].finalKnown,"completed transaction exposes actual final local pair");
 ck31(!sameDigitH34(s,s.restLocal,0,1),"default nonzero Trigger produces a real non-open digit");
 s.contact[0].fingerContact[1]=true;s.contact[0].curl[1]=.02f;auto noClamp=*captured;FinishFingerOutput28(noClamp,transaction);
 ck31(!noClamp.hand[0].contact[1]&&std::abs(noClamp.hand[0].appliedCurl[1]-FreeFingerInputH7(1,.65f,.3f,false))<1e-9,"actual null-contact writer never reports a later stale contact clamp");
 HandContact6695 actualClamp{};actualClamp.fingerContact[2]=true;actualClamp.curl[2]=.11f;
 ck31(ApplyFingerLocals6689(s,0,.65f,.3f,&actualClamp),"actual passed contact controls the sole writer");Compose6689(s);s.contact[0]={};
 auto clamped=*captured;FinishFingerOutput28(clamped,transaction);ck31(clamped.hand[0].contact[2]&&std::abs(clamped.hand[0].appliedCurl[2]-.11f)<1e-9,"diagnostic retains actual staged writer clamp even if later contact state differs");
 transaction.fingerPublished28=false;auto rejected=*native;rejected.tick=hostTickCount64;FinishFingerOutput28(rejected,transaction);
 ck31(!rejected.published&&!rejected.hand[0].joints[0].finalKnown,"failed publication cannot borrow the successful current pose");
 transaction.fingerPublished28=true;auto replaced=*captured;replaced.epoch=60;FinishFingerOutput28(replaced,transaction);ck31(!replaced.published,"replaced body epoch cannot relabel final locals as current");
 s.nativeCurveClosedH7[0]=s.closed[0];s.nativeCurveClosedH7[0].ready=true;transaction.fingerTrialRequestedH7=true;transaction.fingerTrialH7[0]=true;
 ck31(ApplyFingerLocals6689(s,0,.65f,.3f,nullptr,0,true),"same source executes opt-in native trial curve");Compose6689(s);
 auto trial=*captured;FinishFingerOutput28(trial,transaction);ck31(trial.hand[0].trialRequested&&trial.hand[0].trialActive&&trial.hand[0].trialReady,"requested and actually applied native trial are distinguished");
 s.nativeCurveClosedH7[0].ready=false;transaction.fingerTrialH7[0]=false;auto fallback=*captured;FinishFingerOutput28(fallback,transaction);ck31(fallback.hand[0].trialRequested&&!fallback.hand[0].trialActive&&!fallback.hand[0].trialReady,"missing trial endpoint retains explicit default fallback");
 s.objectObserved28[0]=true;s.objectFollow28[0]=false;s.objectCandidate[0].ready=true;s.objectCandidate[0].since=60000;s.objectCandidate[0].lastSample=70000;s.objectCandidate[0].sequence=54;s.objectFingerKey[0]=123;s.objectFingerGeneration[0]=9;s.freeObserved28[0]=false;
 auto held=*captured;FinishFingerOutput28(held,transaction);ck31(held.hand[0].objectWriter&&held.hand[0].objectReady&&!held.hand[0].writer&&held.hand[0].objectSince==60000,"held cached native finger source and original candidate clock are observed independently");
 }
 cap::gate.select(cap::Domain::HandReference,false,hostTickCount64);std::string off;h::drain(hostTickCount64,[&](const char* line){off+=line;});ck31(off.empty(),"ending finite lease drops queued formatting, no recorder work after OFF");g_skinning_core_original=nullptr;s={};
}


static void* observedEntity28=nullptr;static unsigned itemIdReads28=0,itemWorldReads28=0,mutateItem28=0;static void* observedManager28=nullptr;
static std::uint32_t __fastcall itemId28(void* entity){++itemIdReads28;std::uint32_t id=0;std::memcpy(&id,static_cast<unsigned char*>(entity)+0x0C,4);return id;}
static const Matrix34V6550* __fastcall itemWorld28(void* entity){++itemWorldReads28;
 if(mutateItem28==1){std::uint32_t id=502;std::memcpy(static_cast<unsigned char*>(entity)+0x0C,&id,4);}
 if(mutateItem28==2)hostTickCount64+=kcd2vr::diagnostics::capture168::Duration+1;
 if(mutateItem28==3)kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::HandReference,true,hostTickCount64);
 if(mutateItem28==4)++g_pose_epoch6689;
 if(mutateItem28==5){void* other=reinterpret_cast<void*>(0x1234);std::memcpy(static_cast<unsigned char*>(observedManager28)+0x70,&other,8);}
 return reinterpret_cast<const Matrix34V6550*>(static_cast<unsigned char*>(entity)+0x58);
}
static void* fixtureEntityLookup28(std::uint32_t id){return id==501?observedEntity28:nullptr;}
// Actual native attachment observation body. Only the already-existing entity
// system lookup is replaced; world/ID getters execute controlled real functions.
namespace diagnostics=kcd2vr::diagnostics;
#define EntityById6694 fixtureEntityLookup28
#define HandAttachmentReceipt28 fixtureAttachmentObserver28
#include <hand_attachment_observer28.inc>
#undef HandAttachmentReceipt28
#undef EntityById6694
static void itemObservationChecks28(){
 namespace h=kcd2vr::diagnostics::hands28;namespace c=kcd2vr::diagnostics::capture168;
 static std::vector<unsigned char> tables(0x60D4B8+0x400);
 static std::array<unsigned char,192> entity{};static std::array<unsigned char,192> character{};
 static std::array<unsigned char,192> manager{};static std::array<unsigned char,192> attachment{};static std::array<unsigned char,32> self{};
 const auto oldBase=g_wh_pinned104.load();const auto base=reinterpret_cast<std::uintptr_t>(tables.data())-0x3A657C0ull;g_wh_pinned104=base;
 auto put28=[](void* at,const auto& value){std::memcpy(at,&value,sizeof(value));};
 const auto entityVft=base+kCEntityVftRva,characterVft=base+0x40602F8ull,attachmentVft=base+0x3A657C0ull;
 put28(entity.data(),entityVft);put28(character.data(),characterVft);put28(attachment.data(),attachmentVft);
 EntityIdGetterFn idGetter=&itemId28;EntityWorldTmGetterFn worldGetter=&itemWorld28;
 put28(reinterpret_cast<void*>(entityVft+8),idGetter);put28(reinterpret_cast<void*>(entityVft+0x100),worldGetter);
 void* cm=character.data();void* am=manager.data();const char* name="entity_right";
 put28(manager.data()+0x70,cm);put28(attachment.data()+0x28,am);put28(attachment.data()+0x10,name);
 std::uint32_t id=501;put28(self.data()+8,id);put28(entity.data()+0x0C,id);
 Matrix34V6550 tm{};tm.m[0]=tm.m[5]=tm.m[10]=1;tm.m[3]=.2f;tm.m[7]=.3f;tm.m[11]=.4f;put28(entity.data()+0x58,tm);
 observedEntity28=entity.data();observedManager28=manager.data();g_henry_char_attachment_manager=manager.data();g_committed_pose_valid6689=false;
 g_pose_epoch6689=81;g_weapon_request_epoch6694=82;hostTickCount64=90000;h::head=h::tail=0;c::gate.mask=0;
 fixtureAttachmentObserver28(self.data(),attachment.data(),hostTickCount64);
 ck31(itemIdReads28==0&&itemWorldReads28==0&&h::head==h::tail,"default OFF native attachment adds zero cached-world/ID getter calls");
 c::gate.select(c::Domain::Hands,true,hostTickCount64);fixtureAttachmentObserver28(self.data(),attachment.data(),hostTickCount64);
 ck31(itemIdReads28==0&&itemWorldReads28==0&&h::head==h::tail,"legacy Hands-only native/no-placement attachment adds zero cached-world/ID getter calls");
 c::gate.select(c::Domain::HandReference,true,hostTickCount64);fixtureAttachmentObserver28(self.data(),attachment.data(),hostTickCount64);
 ck31(itemIdReads28==1&&itemWorldReads28==1&&h::head!=h::tail,"native/reference attachment callback reaches actual existing cached-world getter without a VR commit or placement");
 auto measured=h::queue[h::tail];h::tail=h::head;
 ck31(measured.itemRoute==3&&!measured.placed&&!measured.itemPair&&measured.actualMatrixKnown&&measured.actualItemKnown&&measured.entity==501,"native no-placement measurement remains explicitly unpaired and never claims a VR body/item join");
 ck31(near31(measured.actualItem.p,{.2f,.3f,.4f}),"measured item world position comes from actual current cached CEntity matrix");
 const auto worldBefore=itemWorldReads28;hostTickCount64+=99;fixtureAttachmentObserver28(self.data(),attachment.data(),hostTickCount64);ck31(itemWorldReads28==worldBefore,"native attachment diagnostic obeys100ms getter cadence");
 hostTickCount64+=1;fixtureAttachmentObserver28(self.data(),attachment.data(),hostTickCount64);ck31(itemWorldReads28==worldBefore+1,"100ms native attachment callback admits next measured world pose");h::tail=h::head;
 for(unsigned mutation=1;mutation<=5;++mutation){hostTickCount64+=100;put28(entity.data()+0x0C,id);put28(manager.data()+0x70,cm);g_pose_epoch6689=81;c::gate.select(c::Domain::HandReference,true,hostTickCount64);mutateItem28=mutation;
  fixtureAttachmentObserver28(self.data(),attachment.data(),hostTickCount64);mutateItem28=0;
  const bool empty=h::head==h::tail;
  if(mutation==1){ck31(!empty&&!h::queue[h::tail].actualMatrixKnown,"entity replacement inside world getter cannot masquerade as an old measured matrix");h::tail=h::head;}
  else ck31(empty,"expiry/session/body/character replacement inside native getter cannot enqueue old-episode data");
 }
 c::gate.select(c::Domain::HandReference,false,hostTickCount64);g_henry_char_attachment_manager=nullptr;g_wh_pinned104=oldBase;observedEntity28=nullptr;observedManager28=nullptr;g_pose_epoch6689=61;
}

int main(){
 static CoherentState6689 s{};init31(s);const auto anatomy=s.restLocal;
 hostTickCount64=1000;const auto now=std::uint64_t(GetTickCount64());
 g_census_mode=5;g_weapon_control_mode=0;g_gameplay_allowed=true;
 FistOwner101 owner{};owner.empty=true;owner.player=reinterpret_cast<void*>(1);owner.manager=reinterpret_cast<void*>(2);
 owner.tick=now;owner.epoch=7;owner.request=g_fist_epoch101.load();g_fist_owner101.publish(owner);
 TrackingSnapshot t{};t.allowed=t.head_valid=t.left_valid=t.right_valid=true;t.tick=now;
 for(int side=0;side<2;++side){
  ft31::setShiftMmH31(side,15);ft31::setDistalMedialDegH31(side,10);
  squeezeH34(t,side,.61f,.61f);
  ck31(QualifiedCombatFistH34(t,7,side,now),"61 percent still owns a combat fist");
  ck31(drawDefaultH34(s,side,t),"default qualified squeeze reaches actual finger writer");
  const int middle=s.fingers[side][2].id[0];
  const double shift=pc::length(PV(FromT(s.local[middle]))-PV(FromT(s.restLocal[middle])));
  ck31(std::abs(shift-.015*double(.61f))<2e-8,"qualified61percent retains gradual saved middle translation");
  bool tunedNotRaw=false;for(int n=0;n<s.fingerCount[side];++n){const int id=s.fingerIds[side][n];
   tunedNotRaw|=pc::angle(PQ(QFromQuatT(s.local[id])),PQ(QFromQuatT(s.closed[side].local[n])))>.01;
   if(id!=middle)ck31(near31(PV(FromT(s.local[id])),PV(FromT(s.restLocal[id])),1e-12),"all other anatomical local offsets unchanged");
  }
  ck31(tunedNotRaw,"combat qualification no longer forces the untuned closed endpoint");
  const auto qualified=s.local;
  owner.empty=false;g_fist_owner101.publish(owner);
  ck31(!QualifiedCombatFistH34(t,7,side,now)&&drawDefaultH34(s,side,t),"noncombat ownership uses same analogue presentation");
  for(int digit=0;digit<5;++digit)ck31(sameDigitH34(s,qualified,side,digit),"visual curl independent of native empty combat qualification");
  owner.empty=true;g_fist_owner101.publish(owner);
  squeezeH34(t,side,.60f,.60f);ck31(!QualifiedCombatFistH34(t,7,side,now)&&drawDefaultH34(s,side,t),"strict60percent remains below combat qualification");
  const auto partial=s.local;squeezeH34(t,side,.61f,.61f);ck31(drawDefaultH34(s,side,t),"next analogue squeeze renders normally");
  for(int digit=0;digit<5;++digit)for(int k=0;k<s.fingers[side][digit].count;++k){const int id=s.fingers[side][digit].id[k];
   ck31(pc::angle(PQ(QFromQuatT(s.local[id])),PQ(QFromQuatT(partial[id])))<.03,"no visual snap when crossing combat threshold");}
  squeezeH34(t,side,0,0);ck31(drawDefaultH34(s,side,t),"accepted open recipe");const auto opened=s.local;
  squeezeH34(t,side,1,0);ck31(drawDefaultH34(s,side,t),"grip-only recipe");
  ck31(sameDigitH34(s,opened,side,1),"Grip never closes the index");
  bool gripChanged=false;for(int digit:{0,2,3,4})gripChanged|=!sameDigitH34(s,opened,side,digit);
  ck31(gripChanged,"Grip independently closes nonindex fingers");
  squeezeH34(t,side,0,1);ck31(drawDefaultH34(s,side,t),"trigger-only recipe");
  for(int digit:{0,2,3,4})ck31(sameDigitH34(s,opened,side,digit),"Trigger preserves nonindex open recipe");
  ck31(!sameDigitH34(s,opened,side,1),"Trigger independently closes index");
  squeezeH34(t,side,0,0);ck31(drawDefaultH34(s,side,t),"return to accepted open recipe");
  for(int digit=0;digit<5;++digit)ck31(sameDigitH34(s,opened,side,digit),"open endpoint remains byte-identical after closing");
  HandContact6695 contact{};contact.fingerContact[2]=true;contact.curl[2]=.2f;
  squeezeH34(t,side,1,1);ck31(drawDefaultH34(s,side,t,&contact),"combat squeeze retains actual fitted contact curl");
  ck31(std::abs(pc::length(PV(FromT(s.local[middle]))-PV(FromT(s.restLocal[middle])))-.003)<2e-8,"surface-fit20percent owns the saved middle translation");
  ck31(drawDefaultH34(s,side,t),"full analogue squeeze retains saved correction");
  ck31(std::abs(pc::length(PV(FromT(s.local[middle]))-PV(FromT(s.restLocal[middle])))-.015)<2e-8,"full squeeze retains15mm correction instead of untuned native override");
 }
 ck31(std::memcmp(anatomy.data(),s.restLocal.data(),sizeof(anatomy))==0,"anatomical source and bone lengths untouched");
 kcd2vr::fists::Sample semantic{};
 for(int side=0;side<2;++side){semantic.grip[side]=semantic.trigger[side]=.61f;
  ck31(kcd2vr::fists::held(semantic,side),"actual native punch classifier still accepts61percent");
  semantic.grip[side]=.60f;ck31(!kcd2vr::fists::held(semantic,side),"actual punch Grip boundary remains strictly above60percent");
  semantic.grip[side]=.61f;semantic.trigger[side]=.60f;ck31(!kcd2vr::fists::held(semantic,side),"actual punch Trigger boundary remains strictly above60percent");}
 trialChecksH7();
 handObservationChecks28();
 itemObservationChecks28();
 std::printf("DEFAULT_FINGER_H34 %u production-connected checks; native/headset NOT_EXECUTED\n",checks31);return 0;
}
