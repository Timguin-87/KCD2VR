// Included inside lab58n after the legacy articulated-part path. The script
// callback is a distinct, exact-retail original-first game-thread owner. It is
// the only writer of these fallback states; no worker calls engine/Lua APIs.
namespace sd72=kcd2vr::saved_corpse_d72;
namespace cg72=kcd2vr::corpse_grip_cg1;
struct SavedDragD72 {
 cg72::Publisher publisher{};cg72::Key key{};corpseMotorD40::Lease motor{};
 std::uintptr_t character=0;std::uint64_t vm=0,press=0,lastSearch=0,lastReport=0,loadEpisodeR96=0;
 sd72::Vec observed{};bool held=false;
 SavedCorpsePartR95 nativePart{};kcd2vr::partanchor59c7g::Anchor anchor;L::DragState servo{};
 kcd2vr::corpse_follow_r101::Rest followR101{};kcd2vr::corpse_follow_r101::Shaper shapeR101{}; // R101: reset with s={} on retire
 L::V3 commandR101{}; // R101: the servo wish before the shaper (host tests check the retained grip offset with it)
};
static SavedDragD72 g_savedDragD72[2];
// R101 two hands: the part the OTHER hand drives on this same articulated body (-1 none), from either
// corpse route (legacy articulated g_drag or this saved route). Both hands may hold one body (R96).
static int OtherHandPartR101(int side,std::uintptr_t body) noexcept {
 if(side<0||side>1||!body)return -1;const int o=1-side;
 const auto& saved=g_savedDragD72[o];if(saved.held&&saved.key.root==body)return saved.key.part;
 if(g_drag[o].held&&g_dragRoot[o]==body)return g_drag[o].part;
 return -1;
}
static std::atomic<bool> g_savedActiveD72[2]{};
static unsigned g_savedSequenceD72=0;
static std::uint64_t g_savedSearchReportD72[2]{};
static std::uint64_t g_savedQueryReportD85R5R1[2]{};
static std::atomic_flag g_savedBusyD72=ATOMIC_FLAG_INIT;
static std::atomic<std::uint64_t> g_savedServiceTickD72{0};
static std::atomic<bool> g_savedProviderInstalledD84{false};
static std::atomic<bool> g_savedClearD72[2]{};
static std::atomic<std::uint64_t> g_savedClearDueD72[2]{};
// D85R5R2: no candidate/held owner means discovery is due at 10 Hz, not at
// every script callback. Wanted is deliberately atomic-only: idle callbacks
// return before native_script_t2::Vm/VirtualQuery and before hand/native reads.
static std::atomic<std::uint64_t> g_savedSearchDueD85R5R2[2]{};
static bool SavedTakeSearchD85R5R2(int side,std::uint64_t now) noexcept {
 auto& due=g_savedSearchDueD85R5R2[side];
 if(!kcd2vr::corpse_search_d85r5r2::deadlineDue(due.load(std::memory_order_acquire),now))return false;
 due.store(now+kcd2vr::corpse_search_d85r5r2::kSearchMs,std::memory_order_release);return true;
}
struct SavedUnlockD72 {~SavedUnlockD72(){g_savedBusyD72.clear(std::memory_order_release);}};
static bool SavedDragOwnsD72(int side) noexcept {return side>=0&&side<2&&g_savedActiveD72[side].load(std::memory_order_acquire);}
// R90: Experiments 'Corpse search' = ONLY ON SQUEEZE (R89): an unowned hand is
// searched only inside its squeeze window (and still at most at the R2 10 Hz
// budget). ALWAYS (R2, default) leaves the R2 idle budget exactly as it was.
static kcd2vr::corpse_grip_cg1::SearchWindowR89 g_savedSearchR89[2];
static bool SavedSqueezeRouteR90() noexcept {return L::corpseSqueezeSearchR90.load(std::memory_order_acquire);}
// Updated every script tick (same game thread as the service, idempotent per
// tick) so an opened hand rearms the window even while nothing else runs.
static bool SavedSearchWindowR90(int side,std::uint64_t now) noexcept {
 float grip=0;const bool fresh=GripValueR89(side,now,grip);
 return g_savedSearchR89[side].update(fresh,grip,now);
}
static bool SavedWantedD72() noexcept {
 const auto now=std::uint64_t(GetTickCount64());
 if(SavedDragOwnsD72(0)||SavedDragOwnsD72(1)||
    (g_savedClearD72[0].load()&&now>=g_savedClearDueD72[0].load())||
    (g_savedClearD72[1].load()&&now>=g_savedClearDueD72[1].load()))return true;
 if(L::active(L::Sub::CorpseDrag)&&SavedSqueezeRouteR90()){ // R90: R89 route
  bool any=false;
  for(int side=0;side<2;++side)if(SavedSearchWindowR90(side,now)&&
   kcd2vr::corpse_search_d85r5r2::deadlineDue(g_savedSearchDueD85R5R2[side].load(),now))any=true;
  return any;
 }
 return L::active(L::Sub::CorpseDrag)&&
  (kcd2vr::corpse_search_d85r5r2::deadlineDue(g_savedSearchDueD85R5R2[0].load(),now)||
   kcd2vr::corpse_search_d85r5r2::deadlineDue(g_savedSearchDueD85R5R2[1].load(),now));
}
static void SavedReportD72(int side,const char* event,const char* reason,const SavedDragD72& s,std::uint64_t now,sd72::Vec observed={}) noexcept {
 if(!L::captureActiveD84(now))return;
 char line[512]{};std::snprintf(line,sizeof(line),
  "D42_F3 F9_HAND_DRAG_D72 side=%s event=%s reason=%s entity=%u identity=%p body=%p scriptToken=%llu gripPress=%llu held=%d part=%d nativePoint=(%.4f,%.4f,%.4f) observedDelta=%.5f route=ARTICULATED_PART_SET_VELOCITY_R95 target=COMMITTED_RAW_HAND acquisition=RAW_CONTROLLER contact=RETAINED constraints=NONE",
  side?"L":"R",event,reason,s.key.entity,reinterpret_cast<void*>(s.key.identity),reinterpret_cast<void*>(s.key.root),
  (unsigned long long)s.key.scriptToken,(unsigned long long)s.press,s.held?1:0,s.key.part,observed.x,observed.y,observed.z,sd72::distance(observed,s.observed));
 diagnostic_trace::WriteFor(L::captureDomainD84(now),line);
}
static bool SavedCallD72(const kcd2vr::native_script_t2::Context& context,int side,char op,sd72::Vec hand,sd72::Reply& reply) noexcept {
 auto& s=g_savedDragD72[side];unsigned seq=++g_savedSequenceD72;if(!seq)seq=++g_savedSequenceD72;
 char input[384]{},output[512]{};
 const int n=std::snprintf(input,sizeof(input),"D72|%c|%llu_%llu_%llu|%d|%u|%u|%llu|%.9g|%.9g|%.9g",op,
  (unsigned long long)context.generation,(unsigned long long)g_pose_epoch6689.load(),(unsigned long long)kcd2vr::collab_h21::epoch.load(),
  side,seq,s.key.entity,(unsigned long long)s.key.scriptToken,hand.x,hand.y,hand.z);
 return n>0&&std::size_t(n)<sizeof(input)&&context.Current()&&context.CallPacket("kcd2vr_corpse_d72",input,output,sizeof(output))&&
  context.Current()&&sd72::parse(output,seq,reply);
}
static bool SavedQueryAuditDueD85R5R1(int side,std::uint64_t now) noexcept {
 using cap=kcd2vr::diagnostics::capture168::Domain;
 if(!kcd2vr::diagnostics::capture168::active(cap::Physics,now))return false;
 auto& last=g_savedQueryReportD85R5R1[side];
 if(last&&now>=last&&now-last<2000)return false;
 last=now;return true;
}
static void SavedQueryAuditD85R5R1(int side,const sd72::Reply& answer,sd72::Vec hand,std::uint64_t now,double nativeQueryUs=-1) noexcept {
 using cap=kcd2vr::diagnostics::capture168::Domain;
 const auto& a=answer.audit;
 if(!a.valid||!kcd2vr::diagnostics::capture168::active(cap::Physics,now))return;
 char row[896]{};
 std::snprintf(row,sizeof(row),
  "D85R5R2 F9_CORPSE_QUERY side=%s outcome=%s entries=%u tables=%u boneAPI=%u velocityAPI=%u queryableCurrent=%u deathAPIMissing=%u deathErrors=%u dead=%u idUnreadable=%u eligible=%u scanned=%u boneCalls=%u boneErrors=%u finiteBones=%u boundsAudit=%d boundsCalls=%u boundsRejected=%u boundsUnknown=%u queryAudit=%d sphereCalls=%u sphereReuse=%u nativeQueryUs=%.3f nearestM=%.5f nearestId=%u nearestBone=%u handWorld=(%.4f,%.4f,%.4f) queryCoverageM=1.6 shareRadiusM=2.4 shareMaxMs=30 nominateM=.45 acquireM=.25 boundsMarginM=.65 observerExtraScan=0 motion=UNOBSERVED",
  side?"L":"R",answer.result,a.entries,a.tables,a.boneMethods,a.velocityMethods,a.current,a.deathMissing,a.deathErrors,a.dead,
  a.idUnreadable,a.eligible,a.scanned,a.boneCalls,a.boneErrors,a.finitePoints,a.boundsValid?1:0,
  a.boundsCalls,a.boundsRejected,a.boundsUnknown,a.queryValid?1:0,a.queryCalls,a.queryReuse,nativeQueryUs,a.nearest,a.nearestId,a.nearestBone,hand.x,hand.y,hand.z);
 diagnostic_trace::WriteFor(cap::Physics,row);
}
static void SavedRetireD72(int side,const char* why,std::uint64_t now,const kcd2vr::native_script_t2::Context* context=nullptr) noexcept {
 // Release never writes velocity. A missing/stopped VM leaves one private-slot
 // cleanup request, consumed when the native VM owner next runs.
 bool cleared=false;sd72::Reply unused{};
 if(context&&context->Current())cleared=SavedCallD72(*context,side,'R',{},unused)&&!std::strcmp(unused.result,"RELEASED");
 g_savedClearD72[side].store(!cleared,std::memory_order_release);g_savedClearDueD72[side].store(now);
 auto& s=g_savedDragD72[side];if(s.key.entity)SavedReportD72(side,"RELEASE",why,s,now,s.observed);
 corpseMotorD40::claims.release(s.motor);s.publisher.side=side;s.publisher.clear();
 self_collision_r103n::Release(kcd2vr::self_collision_r103::corpseSavedOwner(side),now?now:std::uint64_t(GetTickCount64())); // R103: Henry collides with it again once clear
 cg72::claims.livePress[side].store(0,std::memory_order_release);cg72::claims.held(side,false);
 const auto search=s.lastSearch,next=s.publisher.next;s={};s.publisher.side=side;s.publisher.next=next;s.lastSearch=search;
 g_savedActiveD72[side].store(false,std::memory_order_release);
}
// The existing public interaction position is the post-contact wrist. Feeding
// that back into the held-body motor would stop the body against its own hand.
// D78 uses the SAME completed packet's pre-contact controller intent for
// acquisition AND drive (the H40/prop source), never display lead. A corpse
// contact must not clamp the cursor used to select or move that same corpse.
static bool SavedHandIntentReasonD84(int side,float& x,float& y,float& z,const char*& why) noexcept {
 const auto refuse=[&](const char* text){why=text;return false;};
 if(side<0||side>1)return refuse("HAND_INDEX");
 if(g_census_mode.load()<2)return refuse("COMPLETED_HAND_MODE_OFF");
 if(diagnostics::lab::NativeReference())return refuse("NATIVE_REFERENCE_OWNS_HANDS");
 if(!CalibrationReady())return refuse("CALIBRATION_NOT_READY");
 CommittedFrame6695 p{};
 if(!ReadCommittedPacket6695(p))return refuse("NO_COMMITTED_HAND_PACKET");
 const auto now=std::uint64_t(GetTickCount64()); // compare after the packet read; a fresh publication may advance the clock
 if(!p.valid[side])return refuse("COMMITTED_HAND_INVALID");
 if(!p.propRequestedValid5916c1[side])return refuse("RAW_HAND_INTENT_UNAVAILABLE");
 if(!TrackingHandUsable111(side,now))return refuse("HAND_TRACKING_UNUSABLE");
 if(!p.stamp.tick||now<p.stamp.tick||now-p.stamp.tick>80)return refuse("COMMITTED_HAND_STALE");
 if(HandClaimBlockedT24(side,p.stamp.actor,p.stamp.epoch,g_weapon_request_epoch6694.load()))return refuse("HAND_CLAIM_OWNED_ELSEWHERE");
 const auto position=FromT(HandWorld6694(p.propRequested5916c1[side],p.world));
 if(!std::isfinite(position.x)||!std::isfinite(position.y)||!std::isfinite(position.z))return refuse("RAW_HAND_NONFINITE");
 x=position.x;y=position.y;z=position.z;why=nullptr;return true;
}
static bool SavedHandIntentD72(int side,float& x,float& y,float& z) noexcept {
 const char* why=nullptr;return SavedHandIntentReasonD84(side,x,y,z,why);
}
static bool SavedHandD72(int side,std::uint64_t now,sd72::Vec& hand,const char** reason=nullptr) noexcept {
 const auto refuse=[&](const char* text){if(reason)*reason=text;return false;};
 if(!L::active(L::Sub::CorpseDrag))return refuse("CORPSE_DRAG_NOT_SELECTED");
 if(!retail_build112::Ready(WhBase()))return refuse("RETAIL_NOT_QUALIFIED");
 if(!DragUiOk())return refuse("GAME_CONTEXT_OWNS_HANDS");
 if(!DragInputCurrentCg1(side,now))return refuse("TRACKING_OR_INPUT_NOT_CURRENT");
 if(!HandEmptyW5(side))return refuse("HAND_OCCUPIED");
 if(animation_filter::nativeBodyOwned112.load())return refuse("NATIVE_BODY_OWNS_HANDS");
 if(kcd2vr::observed_load_recovery_r96::observe().loading)return refuse("SAVE_LOADING");
 if(g_dragFault.load()||gore32n::g_drive_fault33.load())return refuse("PHYSICS_FAULT_AWAITING_LOAD");
 float x=0,y=0,z=0;const char* why=nullptr;
 if(!SavedHandIntentReasonD84(side,x,y,z,why))return refuse(why);
 hand={x,y,z};if(!sd72::finite(hand))return refuse("RAW_HAND_NONFINITE");
 if(reason)*reason=nullptr;return true;
}
// Reuse the parent-transfer layout already audited at native GetParent 83A8F4
// (geometry_hit178r27::ImpulseEntityParent57). It is read-only; no parent patch.
static bool SavedOwnerD72(const SavedDragD72& s,Matrix34V6550& world) noexcept {
 const auto visit=kcd2vr::observed_load_recovery_r96::observe();
 if(visit.loading||s.loadEpisodeR96!=visit.episode)return false;
 if(!s.key.entity||s.key.epoch!=g_pose_epoch6689.load()||!kcd2vr::collab_h21::current(s.key.physicsEpoch)||
  s.key.actor!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load()))return false;
 void* e=FlinchEntityById178r11(s.key.entity);std::uint32_t id=0;std::uintptr_t parentLink=0,parent=0;
 return reinterpret_cast<std::uintptr_t>(e)==s.key.identity&&e!=g_last_player_entity.load()&&PhysVft6698(e,kCEntityVftRva)&&
  Read(static_cast<unsigned char*>(e)+0xC,id)&&id==s.key.entity&&Read(static_cast<unsigned char*>(e)+0x98,parentLink)&&
  (!parentLink||(Read(reinterpret_cast<void*>(parentLink+0x18),parent)&&!parent))&&
  reinterpret_cast<std::uintptr_t>(CharacterForEntity6689(e))==s.character&&s.character&&
  GetEntityWorldTmV6552(e,id,world)&&
  FlinchEntityById178r11(id)==e;
}
static void SavedServiceD72(const kcd2vr::native_script_t2::Context& context) noexcept {
 if(g_savedBusyD72.test_and_set(std::memory_order_acquire))return;SavedUnlockD72 unlock;
 g_savedServiceTickD72.store(context.now,std::memory_order_release);
 // Budget each unowned hand before any current-hand/entity/VM requalification.
 // An active owner, native release and outstanding cleanup retain their cadence.
 bool inspectD85R5R2[2]{};
 const bool squeezeR90=SavedSqueezeRouteR90();
 for(int side=0;side<2;++side){
  // R90: ONLY ON SQUEEZE spends the R2 search budget only inside the squeeze window.
  const bool windowR90=!squeezeR90||(L::active(L::Sub::CorpseDrag)&&SavedSearchWindowR90(side,context.now));
  inspectD85R5R2[side]=SavedDragOwnsD72(side)||
   (L::active(L::Sub::CorpseDrag)&&windowR90&&SavedTakeSearchD85R5R2(side,context.now));
 }
 for(int side=0;side<2;++side)if(g_savedClearD72[side].load()&&context.now>=g_savedClearDueD72[side].load()){
  sd72::Reply unused{};const bool cleared=SavedCallD72(context,side,'R',{},unused)&&!std::strcmp(unused.result,"RELEASED");
  g_savedClearD72[side].store(!cleared);g_savedClearDueD72[side].store(context.now+500);
 }
 // Existing stage and script callbacks must be the same game-thread owner;
 // fail closed rather than race legacy part-state on another native thread.
 if(g_stage_thread596.load()!=GetCurrentThreadId()){
  for(int side=0;side<2;++side){
   if(SavedDragOwnsD72(side))SavedRetireD72(side,"NATIVE_OWNER_THREAD_CHANGED",context.now,&context);
   else if(L::active(L::Sub::CorpseDrag)&&L::captureActiveD84(context.now)&&
    (!g_savedSearchReportD72[side]||context.now<g_savedSearchReportD72[side]||context.now-g_savedSearchReportD72[side]>=2000)){
    SavedReportD72(side,"REFUSED","SCRIPT_FRAME_OWNER_THREAD_MISMATCH",g_savedDragD72[side],context.now);
    g_savedSearchReportD72[side]=context.now;
   }
  }return;
 }
 for(int side=0;side<2;++side){auto& s=g_savedDragD72[side];s.publisher.side=side;
  if(SavedDragOwnsD72(side)&&s.loadEpisodeR96!=kcd2vr::observed_load_recovery_r96::observe().episode){
   SavedRetireD72(side,"OBSERVED_SAVE_LOAD",context.now,&context);continue;
  }
  if(!inspectD85R5R2[side])continue;
  sd72::Vec hand{};const auto now=std::uint64_t(GetTickCount64());
  if(squeezeR90&&SavedDragOwnsD72(side)&&!s.held){ // R90 (R89): a nomination ends when the hand opens
   float gripR90=0;const bool freshR90=GripValueR89(side,now,gripR90);
   if(!kcd2vr::corpse_grip_cg1::closingR89(freshR90,gripR90)){SavedRetireD72(side,"HAND_OPENED",now,&context);continue;}
  }
  const char* handWhy=nullptr;const bool handCurrent=SavedHandD72(side,now,hand,&handWhy);
  if(!handCurrent||!context.Current()){
   if(SavedDragOwnsD72(side)){SavedRetireD72(side,handCurrent?"SCRIPT_CONTEXT_CHANGED":handWhy,now,&context);}
   else if(L::captureActiveD84(now)&&(!g_savedSearchReportD72[side]||now<g_savedSearchReportD72[side]||now-g_savedSearchReportD72[side]>=2000)){
    SavedReportD72(side,"REFUSED",handCurrent?"SCRIPT_CONTEXT_CHANGED":handWhy,s,now);g_savedSearchReportD72[side]=now;
   }
   continue;
  }
  if(!SavedDragOwnsD72(side)){
   if(g_savedClearD72[side].load())continue; // retire the old VM slot before any replacement token
   if(g_drag[side].held||g_dragPendingCg1[side].valid)continue; // exact native-part path retains priority
   s.lastSearch=now; // discovery was admitted once by SavedTakeSearchD85R5R2
   const auto searchVisitR96=kcd2vr::observed_load_recovery_r96::observe();
   if(searchVisitR96.loading)continue;
   sd72::Reply answer{};
   const bool recordQueryD85R5R1=SavedQueryAuditDueD85R5R1(side,now);
   // Timing is sampled only with this finite F9 query. The elapsed wall time
   // includes Lua/engine dispatch; it is not a GPU frame-time or FPS claim.
   const auto queryStartD85R5R2=recordQueryD85R5R1?std::chrono::steady_clock::now():std::chrono::steady_clock::time_point{};
   const bool queried=SavedCallD72(context,side,recordQueryD85R5R1?'n':'N',hand,answer);
   if(queried&&recordQueryD85R5R1){
    const double elapsed=std::chrono::duration<double,std::micro>(std::chrono::steady_clock::now()-queryStartD85R5R2).count();
    SavedQueryAuditD85R5R1(side,answer,hand,now,elapsed);
   }
   if(!queried||(std::strcmp(answer.result,"CANDIDATE")&&std::strcmp(answer.result,"NEAR"))||!answer.id||!answer.token){
    if(!g_savedSearchReportD72[side]||now-g_savedSearchReportD72[side]>=2000){
     SavedReportD72(side,"SEARCH",queried?answer.result:"NATIVE_PACKET_UNAVAILABLE",s,now);g_savedSearchReportD72[side]=now;}
    continue;
   }
   void* e=FlinchEntityById178r11(answer.id);
   // R95 main-body join below is authoritative; the unused legacy root query
   // must not veto an independently qualified relinquished articulated part.
   if(!e||e==g_last_player_entity.load())continue;
   s.key={};s.key.actor=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load());s.key.identity=reinterpret_cast<std::uintptr_t>(e);
   s.key.epoch=g_pose_epoch6689.load();s.key.entity=answer.id;
   SavedCorpsePartR95 native{};
   // R96 (Miki: "move dead bodies with my hands, left or right, or both"): the other hand may already hold this
   // body; this hand then takes the nearest OTHER part (the claims forbid two hand motors on one part).
   const auto& otherR96=g_savedDragD72[1-side];
   const int excludeR96=otherR96.held&&otherR96.key.identity==s.key.identity?otherR96.key.part:-1;
   if(!ReadSavedCorpsePartR95(answer.id,s.key.identity,0,-1,{hand.x,hand.y,hand.z},native,excludeR96)||
      kcd2vr::pose_core::length(native.pose.p-ph::Vec3{hand.x,hand.y,hand.z})>.45){s.key={};continue;}
   s.nativePart=native;s.key.root=native.body;s.key.part=native.part;
   s.key.physicsEpoch=kcd2vr::collab_h21::epoch.load();s.key.route=cg72::Route::SavedArticulatedPartR95;s.key.scriptToken=answer.token;
   s.character=reinterpret_cast<std::uintptr_t>(CharacterForEntity6689(e));s.vm=context.generation;s.observed=answer.point;
   s.loadEpisodeR96=searchVisitR96.episode;
   Matrix34V6550 world{};
   if(!SavedOwnerD72(s,world)){SavedReportD72(side,"SEARCH","CANDIDATE_NATIVE_OWNER_OR_REACH_CHANGED",s,now,answer.point);s.key={};continue;}
   g_savedActiveD72[side].store(true,std::memory_order_release);
   SavedReportD72(side,!std::strcmp(answer.result,"NEAR")?"NOMINATION":"CANDIDATE","NATIVE_DEAD_QUERY",s,now,answer.point);
  }
  Matrix34V6550 world{};sd72::Reply answer{};
  if(s.vm!=context.generation||!SavedOwnerD72(s,world)||!SavedCallD72(context,side,'V',hand,answer)||
   answer.id!=s.key.entity||answer.token!=s.key.scriptToken||
   (s.held?std::strcmp(answer.result,"HELD"):(std::strcmp(answer.result,"CANDIDATE")&&std::strcmp(answer.result,"NEAR")))||
   !SavedOwnerD72(s,world)){
   SavedRetireD72(side,"CURRENT_ENTITY_OR_DEAD_QUERY_CHANGED",now,&context);continue;
  }
  SavedCorpsePartR95 native{};
  if(!ReadSavedCorpsePartR95(s.key.entity,s.key.identity,s.key.root,s.key.part,{hand.x,hand.y,hand.z},native)||
     native.geometry!=s.nativePart.geometry||native.proxyGeometry!=s.nativePart.proxyGeometry||std::abs(native.scale-s.nativePart.scale)>.0001){
   SavedRetireD72(side,"CURRENT_ARTICULATED_PART_CHANGED",now,&context);continue;
  }
  const double reach=kcd2vr::pose_core::length(native.pose.p-ph::Vec3{hand.x,hand.y,hand.z});
  if(!s.held&&reach>.45){SavedRetireD72(side,"PART_OUT_OF_REACH",now,&context);continue;}
  const bool pendingOnly=!s.held&&reach>.25;
  const auto available=s.publisher.publish(s.key,now,s.held,pendingOnly,
   g_weapon_request_epoch6694.load(std::memory_order_acquire),kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire));s.key=available.key;
  cg72::Request request{};const bool requested=cg72::handRequests[side].read(request)&&cg72::claims.authority(side,request)&&
   cg72::permits(request,available,now)&&GripHeldW5(side)&&(!s.held||request.press==s.press);
  if(s.held&&!requested){SavedRetireD72(side,"GRIP_RELEASE_OR_CLAIM_LOST",now,&context);continue;}
  if(!requested)continue;
  sd72::Vec latest{};
  if(!SavedHandD72(side,std::uint64_t(GetTickCount64()),latest)||!SavedOwnerD72(s,world)){SavedRetireD72(side,"OWNER_CHANGED_BEFORE_WRITE",now,&context);continue;}
  if(!s.held){
   kcd2vr::hand_only_door_h40::Admission admission;
   corpseMotorD40::Key motor{side?corpseMotorD40::Slot::LeftHand:corpseMotorD40::Slot::RightHand,s.key.identity,s.key.root,request.press,s.key.epoch,s.key.part};
   if(!admission.take(1u<<side)||!corpseMotorD40::claims.acquire(s.motor,motor))continue;
   if(!cg72::claims.authority(side,request)||!SavedCallD72(context,side,'A',latest,answer)||std::strcmp(answer.result,"ACQUIRED")||
    answer.id!=s.key.entity||answer.token!=s.key.scriptToken||!SavedOwnerD72(s,world)||
    !ReadSavedCorpsePartR95(s.key.entity,s.key.identity,s.key.root,s.key.part,{latest.x,latest.y,latest.z},native)||
    native.geometry!=s.nativePart.geometry||native.proxyGeometry!=s.nativePart.proxyGeometry||
    kcd2vr::pose_core::length(native.pose.p-ph::Vec3{latest.x,latest.y,latest.z})>.25||
    !s.anchor.acquire({request.press,s.key.epoch,s.key.scriptToken,s.key.entity,s.key.identity,s.key.root,s.key.part},native.pose,native.scale,{latest.x,latest.y,latest.z})){
    SavedRetireD72(side,"ACQUIRE_REFUSED",now,&context);continue;
   }
   s.held=true;s.press=request.press;s.observed={latest.x,latest.y,latest.z};cg72::claims.held(side,true);
   s.servo={true,s.key.entity,s.key.identity,s.key.part,now,0};
   SavedReportD72(side,"GRAB","NATIVE_PART_LOCAL_ANCHOR_NO_FIRST_WRITE",s,now,s.observed);continue;
  }
  if(!corpseMotorD40::claims.current(s.motor)||!cg72::claims.authority(side,request)){SavedRetireD72(side,"MOTOR_OWNER_LOST",now,&context);continue;}
  // Script validation above proved current native death. Recheck the exact
  // part and current input AFTER all native callbacks, immediately before Action.
  ph::Vec3 point{};
  if(!SavedHandD72(side,std::uint64_t(GetTickCount64()),latest)||!SavedOwnerD72(s,world)||
     !ReadSavedCorpsePartR95(s.key.entity,s.key.identity,s.key.root,s.key.part,{latest.x,latest.y,latest.z},native)||
     native.geometry!=s.nativePart.geometry||native.proxyGeometry!=s.nativePart.proxyGeometry||
     !s.anchor.update(s.anchor.owner,native.pose,native.scale,point)||!context.Current()||
     !SavedHandD72(side,std::uint64_t(GetTickCount64()),latest)||!SavedOwnerD72(s,world)||
     !corpseMotorD40::claims.current(s.motor)||!cg72::claims.authority(side,request)){
   SavedRetireD72(side,"PART_OR_CONTEXT_CHANGED_BEFORE_WRITE",now,&context);continue;
  }
  L::DragInput motion{};motion.now=now;motion.gripHeld=motion.uiOk=motion.handEmpty=motion.handValid=true;
  motion.entityQualified=motion.partValid=true;motion.hand={latest.x,latest.y,latest.z};motion.part={point.x,point.y,point.z};
  const auto command=L::stepDrag(s.servo,motion);
  s.commandR101=command.v;
  // R101 (Miki: limbs/head stretch like latex): the part speed is capped (2.5 m/s) and rate limited (30 m/s^2),
  // and the rest of the body follows it instead of hanging on soft joints behind one overwritten part.
  gore32n::g::V3 driveR101{};
  if(command.event==L::DragEvent::Drive){
   const auto shaped=s.shapeR101.step({command.v.x,command.v.y,command.v.z},now,{L::DragTuning{}.maxSpeed,30});
   driveR101={shaped.x,shaped.y,shaped.z};
   const auto f=gore32n::BodyFollowR101(s.followR101,s.key.root,s.key.part,s.press,OtherHandPartR101(side,s.key.root),now,driveR101);
   if(L::captureActiveD84(now)&&(!s.lastReport||now-s.lastReport>=500)){char row[300]{};
    std::snprintf(row,sizeof(row),"R101 F9_CORPSE_FOLLOW side=%s entity=%u part=%d ok=%d parts=%d stretched=%d writes=%d refused=%d worstM=%.3f guarded=%d pair=%d v=%.2f",
     side?"L":"R",s.key.entity,s.key.part,f.ok?1:0,f.parts,f.plan.stretched,f.writes,f.refused,f.plan.worst,f.plan.guarded?1:0,f.plan.pairLimited?1:0,gore32n::g::len(driveR101));
    diagnostic_trace::WriteFor(L::captureDomainD84(now),row);}
  }
  if(command.event!=L::DragEvent::Drive||!gore32n::PartVelocity(s.key.root,s.key.part,driveR101)){
   SavedRetireD72(side,"PART_SERVO_RELEASE_OR_NATIVE_REFUSAL",now,&context);continue;
  }
  // R103 (the dragged body pushed Henry away): while held, the ragdoll ignores living capsules (re-applied only if missing)
  self_collision_r103n::Exclude(kcd2vr::self_collision_r103::corpseSavedOwner(side),kcd2vr::self_collision_r103::Kind::Corpse,
   s.key.entity,s.key.identity,s.key.root,s.key.part,now);
  if(!s.lastReport||now-s.lastReport>=500){SavedReportD72(side,"DRIVE","EXACT_NATIVE_PART_ACTION_RETURNED",s,now,{point.x,point.y,point.z});s.lastReport=now;s.observed={point.x,point.y,point.z};}
 }
}
// The native frame must retire claims even if the script callback/VM stops.
// Same tiny 80 ms input grace; no extended frozen owner and no cleanup write.
static void SavedWatchdogD72(std::uint64_t now) noexcept {
 if(!SavedDragOwnsD72(0)&&!SavedDragOwnsD72(1))return;
 if(g_savedBusyD72.test_and_set(std::memory_order_acquire))return;SavedUnlockD72 unlock;
 const auto last=g_savedServiceTickD72.load(std::memory_order_acquire);
 for(int side=0;side<2;++side)if(SavedDragOwnsD72(side)){
  sd72::Vec hand{};
  if(!last||now<last||now-last>80||!SavedHandD72(side,now,hand)||
   (g_savedDragD72[side].held&&!GripHeldW5(side)))SavedRetireD72(side,"NATIVE_WATCHDOG_OR_INPUT_RELEASE",now);
 }
}
static void InstallSavedDragD72() noexcept {
 static bool ready=false;static std::uint64_t next=0;const auto now=std::uint64_t(GetTickCount64());
 if(ready||now<next)return;next=now+1000;
 ready=kcd2vr::native_script_t2::Register(&SavedServiceD72,&SavedWantedD72)&&kcd2vr::native_script_t2::Install();
 g_savedProviderInstalledD84.store(ready,std::memory_order_release);
 if(ready&&L::captureActiveD84(now))diagnostic_trace::WriteFor(L::captureDomainD84(now),"D42_F3 F9_HAND_DRAG_D72 provider=READY source=NATIVE_DEAD_QUERY supported=SAVED_CORPSES actuation=ARTICULATED_PART_SET_VELOCITY_R95 gate=SELECTED_CORPSE_DRAG noTimer=1");
}
// Called by the existing once-per-second physics observer. Copied atomics
// distinguish installed transport from a callback that actually reached this
// client; observing the provider never calls Lua or enrolls corpse gameplay.
static void SavedProviderDiagnosticsD84(std::uint64_t now) noexcept {
 if(!L::captureActiveD84(now))return;
 const auto tick=g_savedServiceTickD72.load(std::memory_order_acquire);
 char row[320]{};std::snprintf(row,sizeof(row),
  "D84 PHYS_CORPSE_PROVIDER selected=%d installed=%d callbackAgeMs=%lld candidateOrHeldRL=%d/%d queryOutcome=SEE_HAND_RECEIPTS bodyMotion=NOT_INFERRED",
  L::active(L::Sub::CorpseDrag)?1:0,g_savedProviderInstalledD84.load(std::memory_order_acquire)?1:0,
  tick&&now>=tick?static_cast<long long>(now-tick):-1LL,SavedDragOwnsD72(0)?1:0,SavedDragOwnsD72(1)?1:0);
 diagnostic_trace::WriteFor(L::captureDomainD84(now),row);
}
