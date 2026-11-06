// Native game thread; copied physics-frame input.
// C7G: visual latch (the blade display follows the corpse part).
// C32 VrImpaleHang: the corpse part follows the BLADE instead - one per-part set_velocity on the dead
// body's articulated entity per native frame, only while he hangs on it. Released by pulling the blade out along
// its line, the native kick reaching him, a push with the free hand, or when he gets caught on something.
// A living actor is never driven: only a native-dead victim (lethal receipt) is moved.
// C40 VrImpaleByArmour (default OFF): any undefended native thrust with native damage may latch; depth = our model
// from the stroke energy and the NATIVE armour of the struck zone (impale40.h). Living victim = STUCK (the display
// keeps the blade in his wound, nothing native written), dead victim = the C32 HANG with that depth. The living
// display uses one nearest evaluated joint approximation and never hands it to a corpse/HANG body on later death.
// The victim's body is skipped by OUR blade queries while latched.
 // DS-F91: the ONE visible line (Miki's rule) - the impale hold's say() fills the
 // visible-failure mailbox, drained on the game thread. File scope: a function
 // cannot live inside the tick body.
 static void SayImpale61(const char* t) noexcept {if(t&&*t)kcd2vr::visible_failure::publish(t,std::uint64_t(GetTickCount64()));}
static void ImpalePartTick59c7g() noexcept {
 namespace pa=kcd2vr::partanchor59c7g;namespace g=kcd2vr::gore32;namespace i40=kcd2vr::impale40;
 namespace bodyOwnerD40=kcd2vr::corpse_body_owner_d40;
 static bodyOwnerD40::Lease motorD40;bool keepMotorD40=false;
 struct RetireMotorD40 {bodyOwnerD40::Lease& lease;bool& keep;
  ~RetireMotorD40(){if(!keep)bodyOwnerD40::claims.release(lease);}} retireMotorD40{motorD40,keepMotorD40};
 static pa::Anchor anchor;static pa::Identity candidate;
 static std::uint64_t serial=0,nextTry=0;static unsigned tries=0;
 namespace successorH18=kcd2vr::impale_successor_h18;static successorH18::Wait victimWaitH18;
 static bool successorAnchorH18=false,firstPinPublishedH18=false;static std::uint64_t lastWitnessSampleH18=0;static unsigned successorRefusalsH18=0;
 static g::Hang hang;static int side=-1;static ph::Vec3 local{};static bool entryBlood=false;
 static g::V3 lastPalm{};static std::uint64_t lastPalmTick=0;
 // R101: the hanging body follows the blade as one piece (corpse_follow_r101.h); the hung part's velocity
 // is speed/acceleration limited. Rest = the body's own part distances at the first hang drive.
 static kcd2vr::corpse_follow_r101::Rest followR101;static kcd2vr::corpse_follow_r101::Shaper shapeR101;static std::uint64_t followLogR101=0;
 // DS-F91 (Claude CL-IMPALE60): the real constraint hold (the constraint drives
 // the corpse; the velocity servo is the fallback when no weapon body provider
 // is registered or the add is refused).
 namespace ih60=kcd2vr::impale_hold60;static ih60::Hold hold60;static bool hold60Tried=false;
 // H21: holding Grip retains lodged intent at rest; only the SAME completed
 // blade samples drive experimental feel. Opposite-hand motion is never blade motion.
 namespace feel60=kcd2vr::impale_feel60;static feel60::State feelState60;
 static g::V3 lastBlade60{};static std::uint64_t lastBladeTick60=0,lastBladeSerial60=0;
 static bool sayFilled61=false;if(!sayFilled61){ih60::say.store(&SayImpale61,std::memory_order_release);sayFilled61=true;}
 static int mode40=0;static double depth40=0;static bool through40=false;static i40::Stuck stuck40;static std::uint64_t latched40=0,death40=0; // C40: 0 = C32/C7G latch, 1 STUCK, 2 HANG
 // Not native-dead in the ORIGINAL receipt, including unknown death; this is
 // display ownership and never a claim of native living/corpse classification.
 static bool livingDisplay40=false;static LivingImpaleC40 livingAnchor40{};static successorH18::Source livingSource40{};
 static std::uint64_t acquiredLivingSerial40=0; // a retired display cannot reselect a joint from its same old request
 // H18: original native lethal-victim wait, at most4500ms/20Hz. A successor
 // requires a NEW current contact; old body/rawPart are never transferred.
 // R128: plate armour on the struck zone of THIS killing thrust (captured at admission; the verdict mailbox is overwritten by later hits)
 static bool plateR128=false,plateKnownR128=false;static double plateStabR128=-1;static int plateZoneR128=-1;
 namespace i47=kcd2vr::impale47;static i47::RagdollWait wait47;static const char* why47="";static bool waitLogged47=false,gaveUp47done=false;static std::uint64_t refusalSeen47=0;
 auto now=std::uint64_t(GetTickCount64());PhysFrame6698 frame{};
 static kcd2vr::impale_observe_h7::Schedule observeH7;unsigned observationH7=0;
 if(observeH7.take(now,g_pose_epoch6689.load(),kcd2vr::contact176::recoveryEpoch55c1.load(),g_weapon_request_epoch6694.load(),
     g_gameplay_allowed.load()&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load(),observationH7))
   ObserveImpaleOwnersH7(observeH7.id,now-observeH7.began,observationH7);

 const auto log47=[&](const char* stage,const char* extra){
  // These callers are one-shot transaction edges, not per-frame pose samples.
  // Keep them visible outside F3 too, otherwise the quiet whitelist is inert.
  char line[420]{};
  std::snprintf(line,sizeof(line),"C47 IMPALE stage=%s serial=%llu target=%u part=%d %s",stage,(unsigned long long)serial,candidate.target,candidate.part,extra?extra:"");
  diagnostic_trace::Write(line);};
 const auto gaveUp47=[&](const char* reason){
  if(!candidate.serial||anchor.active||gaveUp47done)return;gaveUp47done=true;char e[200]{};std::snprintf(e,sizeof(e),"reason=%s waitedMs=%llu polls=%u lastRoot=%s",reason,
   (unsigned long long)wait47.waitedMs(std::uint64_t(GetTickCount64())),wait47.polls,why47&&*why47?why47:"NONE");log47("GAVE_UP",e);
  char t128[96]{};SayImpale61(i47::gaveUpLine(reason,t128,sizeof(t128)));}; // I10: a lethal stab that started an impale and then gave up says why, once (R128: with the reason code; log only unless F2 records)
 const auto log40=[&](const char* by,double depth){
  if(anchor.active){char e[200]{};std::snprintf(e,sizeof(e),"reason=%s heldMs=%llu depthLeft=%.2f mode=%s",by,(unsigned long long)(hang.since&&now>=hang.since?now-hang.since:0),depth,mode40==1?"STUCK":mode40==2?"HANG40":"HANG");log47("RELEASED",e);}
  if(!mode40||!latched40)return;char line[260]{};std::snprintf(line,sizeof(line),"C40 IMPALE_RELEASE serial=%llu target=%u part=%d mode=%s by=%s heldMs=%llu depthLeft=%.2f through=%d nativeWrites=%s",
   (unsigned long long)serial,candidate.target,candidate.part,mode40==2?"HANG":"STUCK",by,(unsigned long long)(now-latched40),depth,through40?1:0,mode40==2?"PART_VELOCITY(corpse)":"NONE");
  diagnostic_trace::Write(line);};
 // H21: raw queued constraint calls have no typed articulated lifetime proof.
 // Preserve unresolved entered ownership without replay or a competing motor.
 const auto dropHold60=[&](){
  kcd2vr::impale_constraint_h21::Cancel(serial);
  // No raw-pointer Remove/retry. Until typed articulated source+buddy status
  // exists, entered ownership stays quarantined rather than forgetting an ID.
  if(hold60.beginRelease())hold60.quarantine();hold60Tried=false;feelState60={};lastBladeTick60=lastBladeSerial60=0;
 };
 const auto reset=[&](){kcd2vr::impale_death_speed_r106::End();dropHold60();bodyOwnerD40::claims.release(motorD40);keepMotorD40=false;kcd2vr::blood49::partWound55.publish({});anchor.clear();candidate={};tries=0;hang={};side=-1;entryBlood=false;mode40=0;depth40=0;through40=false;stuck40={};latched40=death40=0;followR101.clear();shapeR101.reset();
  victimWaitH18={};successorAnchorH18=firstPinPublishedH18=false;lastWitnessSampleH18=0;successorRefusalsH18=0;
  wait47={};why47="";waitLogged47=false;gaveUp47done=false;livingDisplay40=false;livingAnchor40={};livingSource40={};g_impale_pin178r15.publish({});g_impale_skip40.publish({});
  plateR128=plateKnownR128=false;plateStabR128=-1;plateZoneR128=-1;};
 // H21: cleanup requires exact retained source/buddy and typed Absent; no raw retry.
 const auto readyH18=g_phys_ready_sample6698.load(std::memory_order_acquire);
 const bool copiedFrameR96R1=readyH18&&g_phys_frame6698.read(frame);
 const auto readyAfterR96R1=g_phys_ready_sample6698.load(std::memory_order_acquire);
 const bool haveFrame47=copiedFrameR96R1&&frame.input.sample==readyH18&&readyH18==readyAfterR96R1;
 // Owner observation and frame publication can advance after the entry clock.
 // Validate this copied request against a clock read after its publication.
 now=std::uint64_t(GetTickCount64());
 if(haveFrame47&&frame.output.impaleRefusal47&&frame.output.impaleRefusalSerial47!=refusalSeen47){ // C47: a killing thrust the weapon side did not latch
  refusalSeen47=frame.output.impaleRefusalSerial47;
  if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){const int c=frame.output.impaleRefusal47;char line[320]{};
   std::snprintf(line,sizeof(line),"C47 IMPALE stage=REFUSED_WEAPON_SIDE serial=%llu side=%d reason=%s%s%s tipToContact=%.2f hang=%d",
    (unsigned long long)frame.output.impaleRefusalSerial47,frame.output.impaleRefusalSide47,i47::refusalName(c),c<20?"/":"",
    c<20?kcd2vr::contact58c1::name(kcd2vr::contact58c1::LethalThrustRefusalC2(frame.output.impaleRefusalReason47)):"",frame.output.impaleRefusalTipM47,frame.input.impaleHang32?1:0);
   diagnostic_trace::Write(line);}}
 // Live context is checked again after native callbacks; a copied ON frame
 // cannot write after switching to native reference, a menu or another owner.
 const auto contextCurrentR93=[&]() noexcept {return
    !kcd2vr::diagnostics::lab::NativeReference()&&
    kcd2vr::contact176::fresh(frame.input.tick,std::uint64_t(GetTickCount64()),80)&&frame.request==g_weapon_request_epoch6694.load()&&
    frame.input.bodyEpoch==g_pose_epoch6689.load()&&g_gameplay_allowed.load()&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&
    !animation_filter::nativeBodyOwned112.load();};
 const bool hangMode=frame.input.impaleHang32;
 const auto& out=frame.output;
 const bool scaleCurrentH18=std::isfinite(frame.world[7])&&std::isfinite(frame.input.worldScale)&&
  frame.world[7]>=.1f&&frame.world[7]<=10.f&&std::abs(double(frame.world[7])-frame.input.worldScale)<=1e-5;
 const auto sourceH18=[&](int hand){successorH18::Source s{};
  if(hand>=0&&hand<2){s.key=frame.profiles[hand].key;s.generation=frame.profiles[hand].generation;}
  s.epoch=frame.input.bodyEpoch;s.request=frame.request;s.tracking=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);
  s.recovery=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);s.policy=g_pose_policy_generation57.load(std::memory_order_acquire);return s;};
 const auto liveSourceH18=[&]() noexcept {
  const auto currentNow=std::uint64_t(GetTickCount64());
  return side>=0&&side<2&&scaleCurrentH18&&victimWaitH18.active&&victimWaitH18.source==sourceH18(side)&&
   (firstPinPublishedH18||(currentNow>=victimWaitH18.began&&currentNow-victimWaitH18.began<i47::kCorpseWaitH18))&&
   kcd2vr::contact176::fresh(frame.input.tick,currentNow,80)&&frame.input.allowed&&frame.input.tracked[side]&&frame.input.grip[side]>=.25&&
   frame.request==g_weapon_request_epoch6694.load()&&frame.input.bodyEpoch==g_pose_epoch6689.load()&&
   g_gameplay_allowed.load()&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&!animation_filter::nativeBodyOwned112.load()&&
   NativeImpaleWeaponCurrentH18(frame,side);
 };
 // A nonzero ready/mailbox publication race is not a retired weapon request.
 // Retain only an already admitted, never-pinned victim wait. The copied packet
 // supplies negative/current-owner checks, never permission to query, pin or
 // drive a body before that publication is committed. No wait clock is renewed.
 if(!haveFrame47&&copiedFrameR96R1&&readyH18&&readyAfterR96R1&&
    frame.input.sample>=readyH18&&readyAfterR96R1>=readyH18&&
    candidate.serial&&candidate.serial==serial&&victimWaitH18.active&&victimWaitH18.serial==serial&&wait47.active&&tries&&
    !anchor.active&&!firstPinPublishedH18&&!livingDisplay40&&hangMode&&frame.input.impaleEnabled178r15&&
    g::impaleHang.load(std::memory_order_acquire)&&!out.impaleActive&&!out.impaleReleased&&out.impaleLatchRequest&&
    out.impaleSerial==serial&&out.impaleEntity==candidate.target&&out.impaleSide32==side&&
    contextCurrentR93()&&liveSourceH18()&&frame.input.haveWorld&&frame.profiles[side].primary&&frame.profiles[side].melee()&&
    ph::finite(out.weapons[side])){
  const auto worldR96R1=PhysWorld6698(frame.world);
  const auto tipR96R1=ph::point(worldR96R1,ph::point(out.weapons[side],frame.profiles[side].tip)*double(frame.world[7]));
  if(!victimWaitH18.current(now,sourceH18(side),tipR96R1,true,true)&&wait47.within(std::uint64_t(GetTickCount64()))){
   g_impale_pin178r15.publish({});g_impale_skip40.publish({});return;
  }
 }
 if(!haveFrame47||!frame.input.impaleEnabled178r15||
    (frame.input.impaleHang32&&!g::impaleHang.load(std::memory_order_acquire))||!contextCurrentR93()||
    (!frame.input.impaleHang32&&F4Rung178r10()!=3&&!frame.input.impale40)){
  if(anchor.active)log40("GATE_OR_MODE_OFF",depth40);
  else if(candidate.serial)gaveUp47(!haveFrame47?"NO_FRAME":!frame.input.impaleEnabled178r15?"IMPALE_OFF":animation_filter::nativeBodyOwned112.load()?"NATIVE_BODY_OWNED":!g_gameplay_allowed.load()?"GAMEPLAY_NOT_ALLOWED":"FRAME_EPOCH_OR_MODE");
  reset();return;
 }
 if(candidate.serial&&victimWaitH18.active){
  const bool validSide=side>=0&&side<2;const auto worldH18=PhysWorld6698(frame.world);
  const auto tipH18=validSide?ph::point(worldH18,ph::point(out.weapons[side],frame.profiles[side].tip)*double(frame.world[7])):ph::Vec3{};
  const bool heldH18=validSide&&NativeImpaleWeaponCurrentH18(frame,side)&&frame.profiles[side].primary&&frame.profiles[side].melee()&&frame.input.grip[side]>=.25&&
   frame.input.haveWorld&&scaleCurrentH18;
  const bool trackedH18=validSide&&frame.input.allowed&&frame.input.tracked[side];
  const char* whyH18=anchor.active?(!(victimWaitH18.source==sourceH18(side))?"VICTIM_SOURCE_CHANGED":
   !heldH18||!trackedH18?"WEAPON_RELEASED_OR_TRACKING_LOST":nullptr):victimWaitH18.current(now,sourceH18(side),tipH18,heldH18,trackedH18);
  if(whyH18){if(anchor.active)log40(whyH18,depth40);else gaveUp47(whyH18);reset();return;}
 }
 if(candidate.serial&&(candidate.epoch!=frame.input.bodyEpoch||candidate.request!=frame.request)){if(anchor.active)log40("EPOCH",depth40);else gaveUp47("EPOCH");reset();}
 if(livingDisplay40&&candidate.serial&&(!frame.input.impale40||!(livingSource40==sourceH18(side)))){log40("DISPLAY_OPTION_OR_SOURCE_CHANGED",depth40);reset();return;}
 if(out.impaleReleased||(!out.impaleActive&&!out.impaleLatchRequest)){
  // The immutable lethal-victim wait is independent of recognizer completion.
  // No body may be driven without the SAME weapon request accepting a first pin.
  if(victimWaitH18.active&&!anchor.active){
   why47="WAIT_VICTIM_WEAPON_PIN_OWNER_ABSENT";
   if(!wait47.within(now)){gaveUp47("ORIGINAL_VICTIM_WAIT_EXPIRED");reset();}return;
  }
  if(!anchor.active&&candidate.serial)gaveUp47("WEAPON_SIDE_REQUEST_ENDED");
  if(anchor.active)log40(out.impaleRelease40==8?"WEAPON_SIDE_LIMIT":out.impaleRelease40==7?"WEAPON_SIDE_PIN_LOST":"WEAPON_SIDE",stuck40.active?stuck40.cap:depth40);
  reset();return;}
 if(out.impaleLatchRequest&&out.impaleSerial!=serial&&!anchor.active&&kcd2vr::native_kill_nf::StandDown(reinterpret_cast<std::uintptr_t>(animation_filter::PlayerSkeletonAnim()),now))return; // I10/NF2: Henry's live native kill owns this kill
 if(out.impaleLatchRequest&&out.impaleSerial!=serial){
  const auto targetD56=out.impaleCorpseR15?out.impaleCorpseCandidateR15.target:out.impaleEntity;
  const auto identityD56=out.impaleCorpseR15?out.impaleCorpseCandidateR15.identity:
      (out.impaleHit47.serial==out.impaleSerial?out.impaleHit47.targetIdentity:frame.input.receipt58c1.targetIdentity);
  void* victimD56=FlinchEntityById178r11(targetD56);void* characterD56=victimD56?CharacterForEntity6689(victimD56):nullptr;
  const auto eligibleD56=gore32n::EligibilityD56(targetD56,identityD56,characterD56,now);
  if(eligibleD56==gore32n::eligibilityD56::Result::Pending)return;
  if(eligibleD56!=gore32n::eligibilityD56::Result::MaleHuman){reset();serial=out.impaleSerial;return;}
  if(candidate.serial&&!anchor.active)gaveUp47("SUPERSEDED_BY_NEW_THRUST");
  reset();serial=out.impaleSerial;nextTry=now;
  if(serial==acquiredLivingSerial40)return;
  if(out.impaleCorpseR15){
   // N2 corpse stab: no native receipt exists. Admit only the game-thread candidate of this exact serial, the same
   // entity identity, still game-killed (gore32::KnownDead) and the current weapon owner; then the normal HANG path
   // (root Ready via ImpaleRoot47, victim wait, hold60) runs unchanged.
   // N2 review (Codex P1): consume the request-owned copy the weapon admitted, never icr::latest (a later contact
   // may have replaced it before this request was seen).
   namespace icr=kcd2vr::impale_corpse_r15;const icr::Candidate cs=out.impaleCorpseCandidateR15;
   const bool haveC=icr::requestOwned(cs,serial,out.impaleEntity,out.impaleSide32);
   void* entC=haveC?FlinchEntityById178r11(cs.target):nullptr;
   const int sd=out.impaleSide32;
   const char* whyC=!haveC?"CORPSE_STAB_CANDIDATE_GONE":!entC||reinterpret_cast<std::uintptr_t>(entC)!=cs.identity?"CORPSE_ENTITY_CHANGED":
    !g::KnownDead(cs.target,cs.identity,now)?"CORPSE_NOT_GAME_KILLED":!hangMode?"HANG_MODE_OFF":sd<0||sd>1||sd!=cs.side?"SIDE_INVALID":
    cs.epoch!=frame.input.bodyEpoch||cs.request!=frame.request?"EPOCH":
    cs.key!=frame.profiles[sd].key||cs.generation!=frame.profiles[sd].generation?"WEAPON_CHANGED":nullptr;
   if(whyC){char e[160]{};std::snprintf(e,sizeof(e),"reason=%s mode=CORPSE_STAB",whyC);log47("GAVE_UP",e);reset();return;}
   candidate={serial,cs.epoch,cs.request,cs.target,cs.identity,cs.collider,cs.part};
   side=sd;local=out.impaleLocal32;tries=1;
   const auto worldC=PhysWorld6698(frame.world);const auto& profileC=frame.profiles[side];const double scaleC=frame.world[7];
   const auto tipC=ph::point(worldC,ph::point(out.weapons[side],profileC.tip)*scaleC);
   const auto gripC=ph::point(worldC,ph::point(out.weapons[side],profileC.grip[side].p)*scaleC);
   if(!NativeImpaleWeaponCurrentH18(frame,side)||!victimWaitH18.begin(serial,cs.tick,side,sourceH18(side),tipC,tipC-gripC)){gaveUp47("VICTIM_ADMISSION_INVALID");reset();return;}
   wait47.begin(cs.tick,i47::kCorpseWaitH18,50);
   {char e[260]{};std::snprintf(e,sizeof(e),"mode=CORPSE_STAB power=%.2f speed=%.2f material=%d pick=%s tipToContact=%.2f depth=%.2f collider=%llX deadByGame=1 nativeHit=NONE",
     cs.power,cs.speed,cs.bodyMaterial,out.impalePick47>=0?i47::name(i47::Pick(out.impalePick47)):"NONE",out.impaleTipM47,out.impaleDepth47,(unsigned long long)cs.collider);log47("REQUESTED",e);}
   observeH7.begin({serial,candidate.epoch,kcd2vr::contact176::recoveryEpoch55c1.load(),candidate.request,candidate.target,candidate.entity,candidate.body},now);
  }else{
  // C47: the receipt the weapon side admitted travels with the request; a later receipt (the blade touching the falling corpse)
  // may already have replaced the live copy, and its 80 ms freshness is measured on the weapon side
  const bool fromWeapon47=out.impaleHit47.serial==serial&&out.impaleHit47.target==out.impaleEntity;
  const auto& hit=fromWeapon47?out.impaleHit47:frame.input.receipt58c1;
  const bool same=hit.serial==serial&&hit.target==out.impaleEntity;
  const bool lethalThrust=same&&(fromWeapon47?(hit.death==1&&hit.kind==1&&hit.targetIdentity!=0):
   kcd2vr::contact58c1::lethalThrust(hit,frame.input.tick,hit.key,hit.generation,frame.input.bodyEpoch,frame.request));
  const float needE44=kcd2vr::contact58c1::impaleKillEnergy44.load(std::memory_order_relaxed);const bool strong44=kcd2vr::contact58c1::strongEnough44(hit,needE44);
  const bool lethal=lethalThrust&&strong44;
  // C44 (Miki: "impalements should be EVERY kill move while stabbing with a high energy stab"): the game's own lethal thrust
  // (death=1) with stroke energy >= VrImpaleKillEnergy = HANG, no armour model. Edge receipt for every lethal thrust.
  if(lethalThrust&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){char line[300]{};std::snprintf(line,sizeof(line),
   "C44 IMPALE_CANDIDATE serial=%llu target=%u kind=THRUST death=1 energy=%.2f need=%.2f dmg=%.2f material=%d verdict=%s hang=%d rules=NATIVE_KILL_UNCHANGED",
   (unsigned long long)serial,hit.target,double(kcd2vr::contact58c1::thrustPower96(hit)),double(needE44),double(hit.damage),hit.bodyMaterial,strong44?"LETHAL_IMPALE":"TOO_WEAK_BELOW_VrImpaleKillEnergy",hangMode?1:0);diagnostic_trace::Write(line);}
  bool c40=same&&!lethal&&frame.input.impale40&&(fromWeapon47||i40::candidateThrust(hit,frame.input.tick,hit.key,hit.generation,frame.input.bodyEpoch,frame.request));
  if(lethalThrust&&!strong44&&!c40){ // G1b (Miki's rule): a killing stab that will not impale says why, once, in game
   char t[140]{};
   if(!frame.input.impale40)std::snprintf(t,sizeof(t),"impale: stab too weak (power %.2f, need %.2f) - armour model off",double(kcd2vr::contact58c1::thrustPower96(hit)),double(needE44));
   else std::snprintf(t,sizeof(t),"impale: stab too weak (power %.2f, need %.2f) and no armour candidate (%s)",double(kcd2vr::contact58c1::thrustPower96(hit)),double(needE44),fromWeapon47?"weapon receipt":"receipt not fresh");
   SayImpale61(t);}
  if(c40){ // C40: depth from energy x native zone armour; a refusal leaves no latch (the weapon side times out); G1b: the refusal is said in game
   i40::Verdict v{};const bool haveV=i40::verdict.read(v)&&v.serial==hit.serial&&v.target==hit.target;
   const auto zone=i40::zoneFromMaterial(hit.bodyMaterial);const int zi=zone==i40::Zone::Unknown?2:int(zone);
   const double armour=haveV&&v.armour==i40::ArmourStatus::Read?double(v.stab[zi]):-1.0;
   double bladeM=0;const int sd=out.impaleSide32;
   if(sd>=0&&sd<2){const auto& pr=frame.profiles[sd];const double ws0=(frame.world[7]>.1f&&frame.world[7]<10.f)?double(frame.world[7]):1.0;
    const double dx=pr.tip.x-pr.grip[sd].p.x,dy=pr.tip.y-pr.grip[sd].p.y,dz=pr.tip.z-pr.grip[sd].p.z;bladeM=std::sqrt(dx*dx+dy*dy+dz*dz)*ws0;}
   const auto d=haveV?i40::depthFor(v.energy,armour,hit.damage,hit.death==1,bladeM,i40::Tuning::live()):i40::Depth{};
   char line[520]{};std::snprintf(line,sizeof(line),
    "C40 IMPALE_CANDIDATE serial=%llu target=%u zone=%s%s material=%d armourStab=%.1f armour=%s hardness=%.2f energy=%.2f need=%.2f dmg=%.2f death=%d blade=%.2f verdict=%s depth=%.2f through=%d mode=%s rules=NATIVE_HIT_UNCHANGED",
    (unsigned long long)serial,hit.target,i40::name(zone),zone==i40::Zone::Unknown?"(ASSUMED_BODY)":"",hit.bodyMaterial,armour,haveV?i40::name(v.armour):"NO_VERDICT",d.hardness,haveV?double(v.energy):-1.0,d.needEnergy,
    double(hit.damage),hit.death,bladeM,haveV?i40::name(d.refusal):"NO_ARMOUR_VERDICT",d.depth,d.through?1:0,d.refusal!=i40::Refusal::None?"NONE":hit.death==1&&hangMode?"HANG":"STUCK");
   diagnostic_trace::Write(line);
   if(d.refusal==i40::Refusal::None){depth40=d.depth;through40=d.through;mode40=hit.death==1&&hangMode?2:1;livingDisplay40=hit.death!=1;}
   else{c40=false;char t[160]{};
    if(!haveV)std::snprintf(t,sizeof(t),"impale: no armour reading for him (%s)",i40::name(v.armour));
    else if(d.refusal==i40::Refusal::TooWeak)std::snprintf(t,sizeof(t),"impale: too weak for his %s armour (energy %.2f, need %.2f)",i40::name(zone),haveV?double(v.energy):0.0,d.needEnergy);
    else if(d.refusal==i40::Refusal::TooShallow)std::snprintf(t,sizeof(t),"impale: blade too short / too shallow (%.0f cm)",d.depth*100.0);
    else if(d.refusal==i40::Refusal::NoWound)std::snprintf(t,sizeof(t),"impale: native stab damage was zero (cause not established)");
    else std::snprintf(t,sizeof(t),"impale: refused (%s)",i40::name(d.refusal));
    SayImpale61(t);}
  }
  if(lethal||c40){
   // The weapon admitted this immutable native completion while it was fresh.
   // Its longer victim wait never changes/re-applies damage or renews the hit.
   if(out.impaleSide32<0||out.impaleSide32>1||!hit.serial||!hit.tick||!hit.attempt||!hit.targetIdentity||!hit.collider||hit.rawPart<0||
      !hit.continuation||!hit.seen||!hit.accepted||!hit.applied||!hit.fields||!hit.bodyContact||(hit.death!=1&&!livingDisplay40)||hit.kind!=1||
      !kcd2vr::contact58c1::piercing97(hit)||
      hit.blocked!=0||hit.perfect!=0||hit.parry!=0||!std::isfinite(hit.damage)||!(hit.damage>0)||
      hit.key!=frame.profiles[out.impaleSide32].key||hit.generation!=frame.profiles[out.impaleSide32].generation||
      hit.epoch!=frame.input.bodyEpoch||hit.request!=frame.request)return;
   candidate={serial,hit.epoch,hit.request,hit.target,hit.targetIdentity,hit.collider,hit.rawPart};
   if(lethal&&!mode40){ // R128: plate on the struck zone? (the game's per-zone stab defence of THIS hit, read by impale40n::Evaluate)
    i40::Verdict v128{};if(!i40::verdict.read(v128))v128={};
    const auto p128=i40::plateR128(v128,hit.serial,hit.target,hit.bodyMaterial,double(i40::hardArmour.load(std::memory_order_relaxed)));
    plateR128=p128.plate;plateKnownR128=p128.known;plateStabR128=p128.stab;plateZoneR128=int(p128.zone);}
   tries=mode40==1?4:hangMode?22:3; // C32: the dead body may need ~1 s to become a ragdoll (weapon side waits 1200 ms)
   if(lethal&&hit.death==1&&mode40!=1)kcd2vr::impale_death_speed_r106::Begin(hit.target,now); // R106: his death clip runs faster until the ragdoll pin
   side=out.impaleSide32;local=out.impaleLocal32;
   if(livingDisplay40)livingSource40=sourceH18(side);
   if(mode40!=1&&(mode40==2||hangMode)){
    const auto worldH18=PhysWorld6698(frame.world);const auto& profileH18=frame.profiles[side];
    const double scaleH18=frame.world[7];
    const auto tipH18=ph::point(worldH18,ph::point(out.weapons[side],profileH18.tip)*scaleH18);
    const auto gripH18=ph::point(worldH18,ph::point(out.weapons[side],profileH18.grip[side].p)*scaleH18);
    if(!NativeImpaleWeaponCurrentH18(frame,side)||!victimWaitH18.begin(serial,hit.tick,side,sourceH18(side),tipH18,tipH18-gripH18)){gaveUp47("VICTIM_ADMISSION_INVALID");reset();return;}
    tries=1;wait47.begin(hit.tick,i47::kCorpseWaitH18,50);
   }
   {char e[260]{};std::snprintf(e,sizeof(e),"mode=%s fromWeaponReceipt=%d energy=%.2f dmg=%.2f material=%d pick=%s tipToContact=%.2f depth=%.2f collider=%llX",
     mode40==1?"C40_STUCK":mode40==2?"C40_HANG":hangMode?"HANG":"VISUAL_LATCH",fromWeapon47?1:0,double(hit.energy),double(hit.damage),hit.bodyMaterial,
     out.impalePick47>=0?i47::name(i47::Pick(out.impalePick47)):"NONE",out.impaleTipM47,out.impaleDepth47,(unsigned long long)hit.collider);log47("REQUESTED",e);}
   observeH7.begin({serial,candidate.epoch,kcd2vr::contact176::recoveryEpoch55c1.load(),candidate.request,candidate.target,candidate.entity,candidate.body},now);
  }else if(same&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){char line[260]{};std::snprintf(line,sizeof(line),
   "C47 IMPALE stage=GAVE_UP serial=%llu target=%u reason=%s",(unsigned long long)serial,hit.target,lethalThrust?"TOO_WEAK_BELOW_VrImpaleKillEnergy":"NOT_A_NATIVE_KILLING_THRUST");diagnostic_trace::Write(line);}
  else if(!same&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){char line[200]{};std::snprintf(line,sizeof(line),
   "C47 IMPALE stage=GAVE_UP serial=%llu target=%u reason=RECEIPT_NOT_SAME_SERIAL",(unsigned long long)serial,out.impaleEntity);diagnostic_trace::Write(line);}
  } // I10/N2: end of the receipt (non-corpse) admission
 }
 if(!candidate.serial||candidate.serial!=out.impaleSerial)return;
 const bool weaponQualified59=side>=0&&side<2&&!out.handlingOnly55c1[side]&&
    out.active[side]&&out.owned[side]&&out.contactKnown[side]&&!out.sweepDeferred115[side];
 // Includes the race where native published its first pin just before this
 // frame lost reach/contact authority: an active anchor must release, not drive
 // the corpse toward a controller-only display pose.
 if(anchor.active&&!weaponQualified59){log40("WEAPON_CONTACT_AUTHORITY_LOST",depth40);reset();return;}
 const bool hangNow=mode40?mode40==2:hangMode;
 const auto hw=PhysWorld6698(frame.world);const double ws=(frame.world[7]>.1f&&frame.world[7]<10.f)?double(frame.world[7]):1.0;
 const auto toG=[](ph::Vec3 v){return g::V3{v.x,v.y,v.z};};
 // blade axis in weapon-local model units (grip -> tip) and a stuck point moved back along it by a depth in metres
 const auto backAlong=[&](double metres){
  if(side<0||side>1)return;const auto& pr=frame.profiles[side];
  const ph::Vec3 a{pr.tip.x-pr.grip[side].p.x,pr.tip.y-pr.grip[side].p.y,pr.tip.z-pr.grip[side].p.z};const double al=std::sqrt(a.x*a.x+a.y*a.y+a.z*a.z);
  if(al>1e-6&&ws>1e-6){const double k=metres/ws/al;local=ph::Vec3{local.x-a.x*k,local.y-a.y*k,local.z-a.z*k};}};
 // C59: expiry is checked BEFORE a native query or Anchor::acquire. A Ready
 // result after its ORIGINAL deadline cannot turn an expired wait into a latch.
 if(!anchor.active&&wait47.active&&!wait47.within(now)){
  gaveUp47("RAGDOLL_DEADLINE_OR_CLOCK_REWIND");reset();return;
 }
 if(!anchor.active&&tries&&now>=nextTry){
  // A reach/contact recovery may retain the immutable lethal request, but its
  // direct-hand display is not a checked weapon pose. No anchor or body drive
  // until this same weapon has a current full contact-qualified solution.
  if(!weaponQualified59){
   nextTry=now+50;why47="WEAPON_NOT_CONTACT_QUALIFIED";
   { // C69: say WHY the wait is not polling (once per request); 2026-10-19 log gave up silently after 1500 ms.
    static unsigned long long lastSerial69=0;
    if(lastSerial69!=(unsigned long long)serial){lastSerial69=(unsigned long long)serial;char e[260]{};const bool ok=side>=0&&side<2;
     std::snprintf(e,sizeof(e),"side=%d handlingOnly=%d active=%d owned=%d contactKnown=%d sweepDeferred=%d",side,ok?int(out.handlingOnly55c1[side]):-1,
      ok?int(out.active[side]):-1,ok?int(out.owned[side]):-1,ok?int(out.contactKnown[side]):-1,ok?int(out.sweepDeferred115[side]):-1);log47("WAIT_NOT_QUALIFIED",e);}
   }
   if(wait47.active&&wait47.step(now,i47::Poll::NotYet)==i47::WaitResult::GaveUp){
    gaveUp47("RAGDOLL_DEADLINE_OR_CLOCK_REWIND");reset();}
   return;
  }
  --tries;nextTry=now+50;ph::Pose part{};double scale=0;
  if(wait47.active){ // C52: wait for an exactly identified death body/part only
   tries=1;std::uintptr_t root=0;const char* why="";auto poll=i47::Poll::NotYet;
   // Reuse THIS completed frame's synchronous checked contact. It cannot be a
   // remembered impact, a repeated sample, or a query on the old dying skeleton.
   bool recontactH18=false;
   if(victimWaitH18.active&&frame.input.sample!=lastWitnessSampleH18&&
      frame.publishWorldDeltaM122<=.005&&frame.publishWorldDeltaRad122<=.01&&
      scaleCurrentH18&&
      successorH18::currentBladeContact(frame.input,out,frame.profiles[side],side)){
    lastWitnessSampleH18=frame.input.sample;pa::Identity freshH18{};
    const bool observedH18=ReadImpaleSuccessorH18(candidate,out.contact119[side],true,freshH18,part,scale,why);
    if(!liveSourceH18()){gaveUp47("SUCCESSOR_SOURCE_CHANGED_DURING_STATUS");reset();return;}
    if(observedH18){
     const auto contactH18=ph::point(hw,out.contact119[side].point*ws);
     if(anchor.acquire(freshH18,part,scale,contactH18)){
      char proofH18[290]{};std::snprintf(proofH18,sizeof(proofH18),
       "mapping=NEW_CURRENT_CORPSE_CONTACT kind=2 oldRawPartReused=0 extraWorldQueries=0 oldColliderValue=%llX oldRawPart=%d currentCorpse=%llX newRawPart=%d sample=%llu ageMs=%llu",
       (unsigned long long)candidate.body,candidate.part,(unsigned long long)freshH18.body,freshH18.part,
       (unsigned long long)frame.input.sample,(unsigned long long)(now-frame.input.tick));
      candidate=freshH18;successorAnchorH18=true;recontactH18=true;poll=i47::Poll::Ready;root=candidate.body;
      local=ph::point(ph::inverse(out.weapons[side]),ph::point(ph::inverse(hw),contactH18)*(1./ws));
      log47("ANCHORED",proofH18);
     }else why="SUCCESSOR_CONTACT_ANCHOR_REFUSED";
    }else if(successorRefusalsH18<4&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
     ++successorRefusalsH18;char proofH18[230]{};const auto& contact=out.contact119[side].native172;
     std::snprintf(proofH18,sizeof(proofH18),"reason=%s currentContact=%llX rawPart=%d sample=%llu authority=NONE",why,
      (unsigned long long)contact.collider,contact.part,(unsigned long long)frame.input.sample);log47("SUCCESSOR_CONTACT_REFUSED",proofH18);
    }
   }
   if(!recontactH18)poll=ImpaleRoot47(candidate,root,why);
   // Native root/status calls may consume real time. The entry tick cannot
   // authorize an acquisition beyond the ORIGINAL victim deadline.
   if(!anchor.active&&!wait47.within(std::uint64_t(GetTickCount64()))){gaveUp47("ORIGINAL_WAIT_EXPIRED_DURING_NATIVE_QUERY");reset();return;}
   if(poll==i47::Poll::Ready&&!recontactH18){
    // C52: same foreign entity is NOT a collider/part mapping. Preserve the
    // killing contact's exact articulated body and raw part; never swap to a
    // replacement corpse body or a spatially nearest part.
    if(root!=candidate.body){
     char e[220]{};std::snprintf(e,sizeof(e),"reason=CORPSE_BODY_MAPPING_UNPROVEN entity=%llX hitCollider=%llX currentRoot=%llX rawPart=%d mapping=NONE",
      (unsigned long long)candidate.entity,(unsigned long long)candidate.body,(unsigned long long)root,candidate.part);
     log47("ROOT_MAPPING_REFUSED",e);why="CORPSE_BODY_MAPPING_UNPROVEN";poll=i47::Poll::Fatal;
    }
    else if(ReadImpalePart59c7g(candidate,part,scale)){
     if(!wait47.within(std::uint64_t(GetTickCount64()))||(victimWaitH18.active&&!liveSourceH18())){
      gaveUp47("ORIGINAL_WAIT_OR_SOURCE_EXPIRED_DURING_PART_STATUS");reset();return;}
     if(anchor.acquire(candidate,part,scale,out.impaleTipWorld))log47("ANCHORED","mapping=EXACT_HIT_BODY_AND_RAW_PART");
     else{why="PART_TOO_FAR_FROM_CONTACT";poll=i47::Poll::NotYet;}
    }else{why="EXACT_STRUCK_PART_UNAVAILABLE";poll=i47::Poll::NotYet;}
   }
   why47=why;
   if(!anchor.active&&!waitLogged47&&poll!=i47::Poll::Ready){waitLogged47=true;char e[180]{};std::snprintf(e,sizeof(e),"root=%s windowMs=%llu pollHz=20 successor=FRESH_CURRENT_CONTACT_REQUIRED",why,(unsigned long long)(wait47.until-wait47.since));log47("WAIT_RAGDOLL",e);}
   const auto res=wait47.step(now,anchor.active?i47::Poll::Ready:poll);
   if(anchor.active)tries=0;
   else if(res==i47::WaitResult::GaveUp){gaveUp47(poll==i47::Poll::Fatal?why:"NO_EXACT_RAGDOLL_OR_NEW_CORPSE_CONTACT_IN_ORIGINAL_WAIT");tries=0;}
  }else if(livingDisplay40){
   if(AcquireLivingImpaleC40(candidate,out.impaleTipWorld,livingAnchor40,part,scale)&&anchor.acquire(candidate,part,scale,out.impaleTipWorld)){
    tries=0;acquiredLivingSerial40=serial;char e[220]{};std::snprintf(e,sizeof(e),"mapping=DISPLAY_NEAREST_EVALUATED_JOINT_APPROXIMATION joint=%d originalRawPart=%d contactLocalOffset=RETAINED nativeBodyDrive=NONE",livingAnchor40.joint,candidate.part);log47("ANCHORED",e);
   }
  }else if(ReadImpalePart59c7g(candidate,part,scale)&&anchor.acquire(candidate,part,scale,out.impaleTipWorld))tries=0;
  if(anchor.active)kcd2vr::impale_death_speed_r106::End(); // R106: the ragdoll is pinned - his clip speed goes back
  if(anchor.active||!tries){char line[280]{};std::snprintf(line,sizeof(line),
   "C7G IMPALE_PART serial=%llu target=%u part=%d stage=%s mode=%s rootFallback=NONE",
   (unsigned long long)serial,candidate.target,candidate.part,anchor.active?"LATCHED":"REJECTED_BODY_OR_PART",mode40==1?"C40_STUCK":hangNow?"C32_HANG":"VISUAL_LATCH");
   diagnostic_trace::Write(line);}
  if(anchor.active&&hangNow&&side>=0&&side<2){
   const auto& pr=frame.profiles[side];
   const ph::Vec3 axisL=ph::Vec3{pr.tip.x-pr.grip[side].p.x,pr.tip.y-pr.grip[side].p.y,pr.tip.z-pr.grip[side].p.z};
   const double al=std::sqrt(axisL.x*axisL.x+axisL.y*axisL.y+axisL.z*axisL.z);
   double depth=al>1e-6?((pr.tip.x-local.x)*axisL.x+(pr.tip.y-local.y)*axisL.y+(pr.tip.z-local.z)*axisL.z)/al*ws:.15;
   if(mode40==2){backAlong(depth40);depth=depth40;} // C40: the corpse is drawn onto the blade by the modelled depth
   const double pickDepthR128=depth;
   if(!mode40){ // R128 (Miki 6 Nov: "the blade was not deep enough ... let it impale them on that fully, only plate armor makes this")
    // the stuck point moves back along the blade to the guard (grip->tip length minus 8 cm, at most 1 m); the hang
    // servo then draws the body onto it. Plate on the struck zone keeps the old shallow point.
    const double full=g::fullDepthR128(depth,al*ws,plateR128,g::impaleFullDepthR128.load(std::memory_order_relaxed));
    if(full>depth+.005){backAlong(full-depth);depth=full;}
    if(plateR128&&g::impaleFullDepthR128.load(std::memory_order_relaxed)){char t[96]{};
     std::snprintf(t,sizeof(t),"impale: plate armour on his %s (stab %.0f >= %.0f) - blade stays shallow",i40::name(i40::Zone(plateZoneR128)),plateStabR128,double(i40::hardArmour.load(std::memory_order_relaxed)));SayImpale61(t);}
   }
   hang.start(now,depth);
   {char e[300]{};std::snprintf(e,sizeof(e),"mode=HANG depth=%.2f pickDepth=%.2f fullDepth=%d plate=%d armourKnown=%d zone=%s zoneStab=%.1f massKg=%.0f releases=PULL_OUT|KICK|HAND_PUSH|SWING_OFF|GRIP|30S",
     hang.depth,pickDepthR128,g::impaleFullDepthR128.load(std::memory_order_relaxed)?1:0,plateR128?1:0,plateKnownR128?1:0,i40::name(i40::Zone(plateZoneR128)),plateStabR128,
     double(g::clampHangMass44(g::hangMassKg.load(std::memory_order_relaxed))));log47("LATCHED",e);}
  }
  if(anchor.active&&mode40==1)stuck40.start(now,depth40);
  if(anchor.active&&mode40){latched40=now;char line[260]{};std::snprintf(line,sizeof(line),"C40 IMPALE_LATCHED serial=%llu target=%u part=%d mode=%s depth=%.2f through=%d ownBladeSkipsVictim=1 nativeCollision=UNCHANGED",
   (unsigned long long)serial,candidate.target,candidate.part,mode40==2?"HANG":"STUCK",depth40,through40?1:0);diagnostic_trace::Write(line);}
 }
 if(!anchor.active)return;
 ph::Pose part{};double scale=0;ph::Vec3 world{};ImpalePin178r15 pin{};
 const char* currentCorpseWhyH18="";
 if(!(livingDisplay40?ReadLivingImpaleC40(candidate,livingAnchor40,part,scale):
       successorAnchorH18?ReadImpaleCurrentCorpseH18(candidate,part,scale,currentCorpseWhyH18):ReadImpalePart59c7g(candidate,part,scale))||
    (victimWaitH18.active&&!liveSourceH18())||
    !anchor.update(candidate,part,scale,world)){
  log40("PART_IDENTITY_OR_POSE",stuck40.active?stuck40.cap:depth40);
  anchor.clear();tries=0;hang={};diagnostic_trace::Write("C7G IMPALE_PART stage=RELEASE_IDENTITY_OR_POSE");reset();return;}
 // C32 (review): an old visual latch alive when hang mode was switched on has no hang -> let it go, never a pin without a driver
 if(hangNow&&!hang.active&&mode40!=1){log40("NO_HANG_STATE",depth40);anchor.clear();candidate={};tries=0;side=-1;reset();return;}
 if(mode40)g_impale_skip40.publish({candidate.entity,candidate.body,now});
 // R128: the corpse hanging on the blade (not a living STUCK victim) is skipped by OUR blade queries while the
 // full-depth option is on, so the blade stays in him (npc_contact172.inl PrepareNpcQuery172 checks it again).
 if(mode40!=1&&hangNow&&hang.active&&g::impaleFullDepthR128.load(std::memory_order_relaxed))
  g_impale_skip40.publish({candidate.entity,candidate.body,now,candidate.target,true});
 if((mode40==1||hangNow)&&side>=0&&side<2){
  const auto& pr=frame.profiles[side];const auto& wp=out.weapons[side];
  const auto tipW=ph::point(hw,ph::point(wp,pr.tip)*ws),gripW=ph::point(hw,ph::point(wp,pr.grip[side].p)*ws);
  g::V3 axis=toG(tipW)-toG(gripW);const double al=g::len(axis);axis=al>1e-6?axis*(1/al):g::V3{0,0,1};
  // the other hand, when it is not on this weapon
  const int other=1-side;const auto palmW=toG(ph::point(hw,frame.input.hands[other].p*ws));
  const g::V3 toPalm=palmW-toG(gripW);const double along=std::clamp(g::dot(toPalm,axis),0.,al);
  bool freePalm=g::len(toPalm-axis*along)>.12&&!out.support[side];g::V3 palmVel{};
  if(lastPalmTick&&now>lastPalmTick&&now-lastPalmTick<=100)palmVel=(palmW-lastPalm)*(1000.0/double(now-lastPalmTick));
  else freePalm=false;
  lastPalm=palmW;lastPalmTick=now;
  const auto reach=kcd2vr::kick26::reachTick.load(std::memory_order_acquire);
  g::V3 kd=toG(world)-g::V3{hw.p.x,hw.p.y,hw.p.z};kd.z=0;const double kl=g::len(kd);
  const bool kick=kcd2vr::kick26::ReleaseImpale(now,reach)&&kl<=1.4; // C32 (review): only a kick that can reach him (leg reach), not one into the air
  if(!entryBlood){entryBlood=true;gore32n::BloodV(toG(world),axis*-1.0,candidate.target,"hit_passed_sword_stab_player",1.3f);
   if(mode40!=1){const g::V3 w49=toG(world),d49=axis*-1.0;const double p49[3]{w49.x,w49.y,w49.z},n49[3]{d49.x,d49.y,d49.z}; // C49 BLOODY FIGHTS: a dead body on the blade = kill move
    (void)kcd2vr::blood49::offerKill(kcd2vr::blood49::Kill::Impale,candidate.target,candidate.entity,p49,n49,-1,now);}}
  if(mode40==1){ // C40 STUCK: living victim, measured from our physical blade; no native write
   kcd2vr::contact58c1::Receipt r40{};
   if(hangMode&&!livingDisplay40&&kcd2vr::contact58c1::latest.read(r40)&&r40.serial&&r40.serial!=death40&&r40.target==candidate.target&&r40.targetIdentity==candidate.entity&&r40.death==1){
    death40=r40.serial; // native death while the blade is in him: the corpse now hangs on it
    const g::V3 rel=toG(tipW)-toG(world);const double penNow=std::clamp(g::dot(rel,axis),.05,std::max(.05,depth40));
    local=out.impaleLocal32;backAlong(penNow);hang.start(now,penNow);mode40=2;depth40=penNow;stuck40={};
    {const g::V3 w49=toG(world),d49=axis*-1.0;const double p49[3]{w49.x,w49.y,w49.z},n49[3]{d49.x,d49.y,d49.z}; // C49: he died on the blade
     (void)kcd2vr::blood49::offerKill(kcd2vr::blood49::Kill::Impale,candidate.target,candidate.entity,p49,n49,-1,now);}
    char line[220]{};std::snprintf(line,sizeof(line),"C40 IMPALE_MODE serial=%llu target=%u STUCK->HANG deathReceipt=%llu depth=%.2f",(unsigned long long)serial,candidate.target,(unsigned long long)r40.serial,penNow);
    diagnostic_trace::Write(line);
   }else{
    const g::V3 rel=toG(tipW)-toG(world);const double pen=g::dot(rel,axis);const double lateral=g::len(rel-axis*pen);
    const double pw[3]{palmW.x,palmW.y,palmW.z},pv[3]{palmVel.x,palmVel.y,palmVel.z},wd[3]{world.x,world.y,world.z};
    i40::StuckIn si{};si.now=now;si.pen=pen;si.lateral=lateral;si.kick=kick;si.palmPush=freePalm&&i40::palmPush(pw,pv,wd);
    const auto rel40=stuck40.step(si);
    if(rel40!=i40::Release::None){
     if(rel40==i40::Release::PullOut||rel40==i40::Release::Kick||rel40==i40::Release::HandPush)gore32n::BloodV(toG(world),axis,candidate.target,"hit_passed_sword_stab_player",1.4f);
     log40(i40::name(rel40),std::max(0.0,pen));reset();return; // no pin = the weapon lets go; the living victim stays fully native
    }
   }
  }
  if(mode40!=1&&hangNow&&hang.active){
   const auto bladeW=ph::point(hw,ph::point(wp,local)*ws);
   g::HangIn in{};in.now=now;in.bladePoint=toG(bladeW);in.bladeAxis=axis;in.wound=toG(world);
   in.freePalm=freePalm;in.palm=palmW;in.palmVelocity=palmVel;in.kick=kick;in.kickDirection=kl>1e-6?kd*(1/kl):axis;
   const bool physicsLabH21=kcd2vr::collab_h21::npcPhysics();
   // Exact corpse-part servo ownership belongs to VrImpaleHang. F9 only
   // admits its separate rigid-constraint experiment; never broad-enable it.
   {
    const bodyOwnerD40::Key bodyKey{bodyOwnerD40::Slot::ImpaleServo,candidate.entity,candidate.body,serial,candidate.epoch,candidate.part};
    if(!bodyOwnerD40::claims.acquire(motorD40,bodyKey)){
     log40("BODY_OWNED_BY_HAND_OR_PENDING_IMPALE",depth40);reset();return;}
    keepMotorD40=true;
   }
   const auto servoCurrentR93=[&]() noexcept {return frame.input.impaleHang32&&
    g::impaleHang.load(std::memory_order_acquire)&&bodyOwnerD40::claims.current(motorD40);};
   // Only the completed SAME blade pose supplies motion. The other palm above
   // is reserved for an intentional push, never a blade-speed/release signal.
   feel60::Blade blade60{};blade60.serial=serial;blade60.tick=frame.input.tick;blade60.ragdoll=true;
   blade60.gripHeld=frame.input.grip[side]>=.25;
   const auto bladePoint60=toG(bladeW);
   bool bladeMotion60=lastBladeSerial60==serial&&lastBladeTick60&&frame.input.tick>lastBladeTick60&&frame.input.tick-lastBladeTick60<=80;
   if(bladeMotion60){const auto velocity=(bladePoint60-lastBlade60)*(1000.0/double(frame.input.tick-lastBladeTick60));
    blade60.motion[0]=float(velocity.x);blade60.motion[1]=float(velocity.y);blade60.motion[2]=float(velocity.z);}
   if(frame.input.tick!=lastBladeTick60||lastBladeSerial60!=serial){lastBlade60=bladePoint60;lastBladeTick60=frame.input.tick;lastBladeSerial60=serial;}
   if(!blade60.gripHeld){log40("GRIP_RELEASED",depth40);reset();return;}
   auto o=hang.step(in);
   // R128: the DS-F108 feel layer (impale_feel60.h) is NOT applied to the velocity servo any more. It multiplied
   // every hang velocity by ragdollDrag 35 % (and the along-blade part by another 47.5 %): the servo REPLACES the
   // part's velocity each frame, so only a third of the hold was left and gravity won ("slided down from the blade").
   // It was written for the constraint hold's motor (F9 lab), which keeps its own path.

   // Blood/material callbacks above can reenter native code. Recheck the exact
   // successor/source immediately before a corpse write; no retry on refusal.
   const auto driveCurrentH18=[&]() noexcept {
    if(!contextCurrentR93()||!NativeImpaleWeaponCurrentH18(frame,side)||(victimWaitH18.active&&!liveSourceH18()))return false;
    ph::Pose checkedPart{};double checkedScale=0;const char* why="";
    const bool current=successorAnchorH18?ReadImpaleCurrentCorpseH18(candidate,checkedPart,checkedScale,why):ReadImpalePart59c7g(candidate,checkedPart,checkedScale);
    return current&&contextCurrentR93()&&std::isfinite(checkedScale)&&std::abs(checkedScale-scale)<=.0001&&
     NativeImpaleWeaponCurrentH18(frame,side)&&(!victimWaitH18.active||liveSourceH18());
   };
   if(physicsLabH21&&servoCurrentR93()&&driveCurrentH18()){
    const bool staged=StageImpaleConstraintH21(frame,candidate,side,gripW,tipW,world,now);
    if(!staged&&!hold60Tried){hold60Tried=true;
     if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
      char why[260]{};std::snprintf(why,sizeof(why),"H21 IMPALE_HOLD refused=%s fallback=CURRENT_CORPSE_VELOCITY",kcd2vr::impale_constraint_h21::Failure());diagnostic_trace::Write(why);}}
   }else if(!physicsLabH21)kcd2vr::impale_constraint_h21::Cancel(serial);
   const bool constraintMotor=kcd2vr::impale_constraint_h21::MotorBlocked();
   // R101 (Miki: limbs/head stretch like latex): one part pinned to the blade at up to 6 m/s while ~40 kg hang
   // on soft ragdoll joints opened the joints. Now: the hung part is capped (HangTuning 3 m/s, 40 m/s^2) and the
   // rest of the body follows it (stretched parts get its velocity, keep their swing, lose only the stretch).
   if(o.drive&&!hold60.motorBlocked()&&!constraintMotor&&driveCurrentH18()&&servoCurrentR93()){
    const auto shaped=shapeR101.step({o.velocity.x,o.velocity.y,o.velocity.z},now,{g::HangTuning{}.maxSpeed,40});
    o.velocity={shaped.x,shaped.y,shaped.z};
    const auto f=gore32n::BodyFollowR101(followR101,candidate.body,candidate.part,serial,-1,now,o.velocity);
    // F3 (combat recorder) row only, like the other impale rows; 2 per second at most.
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)&&(!followLogR101||now<followLogR101||now-followLogR101>=500)){followLogR101=now;
     char row[260]{};std::snprintf(row,sizeof(row),"R101 IMPALE_FOLLOW serial=%llu target=%u part=%d ok=%d parts=%d stretched=%d writes=%d refused=%d worstM=%.3f guarded=%d v=%.2f",
      (unsigned long long)serial,candidate.target,candidate.part,f.ok?1:0,f.parts,f.plan.stretched,f.writes,f.refused,f.plan.worst,f.plan.guarded?1:0,g::len(o.velocity));
     diagnostic_trace::Write(row);}
   }
   if(o.drive&&!hold60.motorBlocked()&&!constraintMotor&&(!driveCurrentH18()||!servoCurrentR93()||!gore32n::PartVelocity(candidate.body,candidate.part,o.velocity))){ // Only this exact current corpse part; native RPG/kill rules remain unchanged.
    char line[180]{};std::snprintf(line,sizeof(line),"C33 IMPALE_RELEASE serial=%llu target=%u reason=DRIVE_REFUSED_OR_FAULT",(unsigned long long)serial,candidate.target);
    diagnostic_trace::Write(line);log40("DRIVE_REFUSED_OR_FAULT",o.depth);reset();return;}
   if(o.release!=g::Release::None){
    if(!hold60.motorBlocked()&&!constraintMotor&&g::finite(o.releaseVelocity)&&driveCurrentH18()&&servoCurrentR93())(void)gore32n::PartVelocity(candidate.body,candidate.part,o.releaseVelocity);
    if(g::gush(o.release)){gore32n::BloodV(toG(world),axis,candidate.target,"hit_passed_sword_stab_player",1.6f);
     gore32n::BloodV(toG(world),axis,candidate.target,"hit_passed_sword_slash_player",1.4f);}
    char line[220]{};std::snprintf(line,sizeof(line),"C32 IMPALE_RELEASE serial=%llu target=%u part=%d reason=%s heldMs=%llu depthLeft=%.2f",
     (unsigned long long)serial,candidate.target,candidate.part,g::name(o.release),(unsigned long long)(now-hang.since),o.depth);
    diagnostic_trace::Write(line);log40(g::name(o.release),o.depth);
    reset();return; // no pin = the weapon lets go
   }
  }
 }
 // Copy only the already qualified original collider/raw-part anchor. This is
 // cosmetic metadata and grants no body writes, hit or longer impale lifetime.
 if(mode40!=1&&hangNow&&hang.active){
  kcd2vr::blood49::PartWound55 wound{};wound.serial=candidate.serial;wound.tick=now;wound.epoch=candidate.epoch;wound.request=candidate.request;
  wound.target=candidate.target;wound.identity=candidate.entity;wound.point[0]=world.x;wound.point[1]=world.y;wound.point[2]=world.z;
  const auto& pr=frame.profiles[side];const auto& wp=out.weapons[side];
  const auto tip=ph::point(hw,ph::point(wp,pr.tip)*ws),grip=ph::point(hw,ph::point(wp,pr.grip[side].p)*ws);
  const auto direction=pm::unit(grip-tip);if(direction){wound.dir[0]=direction->x;wound.dir[1]=direction->y;wound.dir[2]=direction->z;}
  kcd2vr::blood49::partWound55.publish(wound);
 }
 if(victimWaitH18.active&&!liveSourceH18()){reset();return;}
 // Skeleton/material reads above may reenter native callbacks. A display pin
 // retains this completed source; it cannot cross a source/context replacement
 // or acquire a new80ms freshness window while those reads were in progress.
 if(livingDisplay40&&(!(livingSource40==sourceH18(side))||frame.request!=g_weapon_request_epoch6694.load()||
    frame.input.bodyEpoch!=g_pose_epoch6689.load()||!kcd2vr::contact176::fresh(frame.input.tick,std::uint64_t(GetTickCount64()),80)||
    !g_gameplay_allowed.load()||g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||animation_filter::nativeBodyOwned112.load())){reset();return;}
 firstPinPublishedH18=true;
 pin.valid=true;pin.serial=serial;pin.tick=now;pin.x=float(world.x);pin.y=float(world.y);pin.z=float(world.z);
 pin.mode40=mode40;pin.depth40=float(mode40==1?stuck40.cap:depth40);
 g_impale_pin178r15.publish(pin);
}
