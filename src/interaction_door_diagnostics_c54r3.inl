// Included after the native bounded read helpers, before SelectorDetour68.
namespace dd3=kcd2vr::diagnostics::door_path_c54r3;
static std::atomic<bool> g_door_diag_ready3{false};
static thread_local dd3::Counter g_door_diag_counts3;
static std::atomic<std::uint64_t> g_door_diag_entered3{0},g_door_diag_completed3{0},g_door_diag_boundary3{0},g_door_diag_activation3{0};
static thread_local dd3::Pass g_door_diag_pass3;
static bool DoorDiagActive3(std::uint64_t now) noexcept {
 return g_door_diag_ready3.load(std::memory_order_acquire)&&diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now);
}
static dd3::Pass* DoorDiagPass3(std::uint64_t now) noexcept {
 auto& p=g_door_diag_pass3;const auto& s=door_collector_c54::current;
 return DoorDiagActive3(now)&&p.detailed&&p.serial&&p.serial==s.serial&&p.interactor==s.interactor&&
  p.session==diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load(std::memory_order_acquire)?&p:nullptr;
}
static std::uint64_t DoorDiagBegin3(std::uintptr_t interactor,unsigned key,std::uintptr_t caller,unsigned depth,const char* scopeReason="NONE") noexcept {
 const auto now=std::uint64_t(GetTickCount64());if(!DoorDiagActive3(now))return 0;
 const auto activation=g_door_diag_activation3.load(std::memory_order_acquire);
 g_door_diag_entered3.fetch_add(1,std::memory_order_relaxed);
 auto& c=g_door_diag_counts3;const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load(std::memory_order_acquire);
 const bool detailed=c.begin(session,now,key==1,depth!=0,caller==g_wh_base+0x7F5727);
 if(!detailed&&key==1&&!depth&&caller==g_wh_base+0x7F5727){auto& health=diagnostics::interaction_cycle_h4::recorder;if(c.details>=dd3::DetailLimit)++health.collectorDetailBlocked;if(c.lines>=dd3::LineLimit)++health.collectorLineBlocked;}
 if(depth)return activation;auto& p=g_door_diag_pass3;p={};p.session=session;p.tick=now;p.interactor=interactor;p.serial=door_collector_c54::current.serial;p.detailed=detailed&&p.serial;
 if(key==1&&!p.serial&&c.refusal(now)&&c.line()){char line[400]{};std::snprintf(line,sizeof(line),"C54R3 F5_DOOR_SCOPE_REFUSED session=%llu callerRva=0x%llX key=%u depth=%u reason=%s source=NATIVE_COLLECTOR_PROOF detailRows=UNAVAILABLE noCandidateAbsenceClaim=1",
  (unsigned long long)session,(unsigned long long)(caller>=g_wh_base?caller-g_wh_base:0),key,depth,scopeReason?scopeReason:"UNKNOWN");diagnostic_trace::Write(line);}return activation;
}
static dd3::Candidate* DoorDiagRow3(std::uint64_t now) noexcept {
 auto* p=DoorDiagPass3(now);return p&&p->current>=0&&unsigned(p->current)<p->count?&p->rows[p->current]:nullptr;
}
static void DoorDiagBounds3(std::uintptr_t entity,std::uintptr_t out) noexcept {
 auto* c=DoorDiagRow3(GetTickCount64());if(!c||c->entity!=entity||!out)return;
 c->boundsKnown=SelectionCopy68(out,c->box,sizeof(c->box));dd3::stage(*c,dd3::Bounds);
}
static void DoorDiagPredicate3(dd3::Stage stage,int nativeResult,int returnedResult) noexcept {
 auto* c=DoorDiagRow3(GetTickCount64());if(!c)return;dd3::stage(*c,stage);
 if(stage==dd3::Facing)c->facing=returnedResult;else if(stage==dd3::HeldFilter)c->held=returnedResult;
 // Native and adapter results remain distinct. Reuse policy58 slots only for
 // their actual two flags; predicate originals are emitted as separate edges.
 if(nativeResult!=returnedResult&&g_door_diag_counts3.line()){
  char line[320]{};std::snprintf(line,sizeof(line),"C54R3 F5_DOOR_GATE session=%llu collector=%llu target=%u stage=%u native=%d returned=%d owner=CURRENT_EXISTING_ADAPTER writes=NONE",
   (unsigned long long)g_door_diag_pass3.session,(unsigned long long)g_door_diag_pass3.serial,c->id,unsigned(stage),nativeResult,returnedResult);diagnostic_trace::Write(line);
 }
}
static void DoorDiagEnd3(std::uintptr_t interactor,unsigned depth,std::uint64_t activation) noexcept {
 if(!activation)return;const auto now=std::uint64_t(GetTickCount64());
 if(activation!=g_door_diag_activation3.load(std::memory_order_acquire)||!DoorDiagActive3(now)){g_door_diag_boundary3.fetch_add(1,std::memory_order_relaxed);return;}
 g_door_diag_completed3.fetch_add(1,std::memory_order_relaxed);if(depth)return;
 auto& c=g_door_diag_counts3;auto* p=DoorDiagPass3(now);
 if(p){
  diagnostics::interaction_cycle_h4::recorder.collectorCandidateTruncated.fetch_add(p->dropped,std::memory_order_relaxed);
  // These rows are already copied by the original bounded collector. Its old
  // formatting allowance must not suppress our independent bounded mirror.
  for(unsigned i=0;i<p->count;++i){const auto& r=p->rows[i];
   {namespace h4=diagnostics::interaction_cycle_h4;h4::Event e{};e.stage=h4::Stage::Selector;e.tick=now;e.sourceTick=p->tick;e.collector=p->serial;e.target=r.id;e.press=r.press[0];e.otherPress=r.press[1];e.flags=r.stageMask;e.code=r.pointInside;e.detail=r.rankWord;e.value=r.distanceKnown?r.centreDistanceSq:-1;e.value2=r.segmentKnown?r.segmentDistance:-1;h4::copy(e.reason,sizeof(e.reason),dd3::stop(r));h4::copy(e.label,sizeof(e.label),"EXACT_RETAIL_SELECTOR_EXISTING_DETAILED_ROW");h4::observe(e,now);}
  }
  for(unsigned i=0;i<p->count;++i){const auto& r=p->rows[i];if(!c.line())break;
   char line[1720]{};std::snprintf(line,sizeof(line),
    "C54R3 F5_DOOR_CANDIDATE session=%llu collector=%llu target=%u ordinal=%u rankWord=%d stageMask=0x%X stop=%s policy58=%d/%d inside=%d centreD2Known=%d centreD2=%.6g segmentKnown=%d segmentDistance=%.6g facing=%d held=%d screen=%d descriptor=%d policy60=%d localKnown=%d local=(%.5g,%.5g,%.5g)->(%.5g,%.5g,%.5g) boxKnown=%d box=(%.5g,%.5g,%.5g)/(%.5g,%.5g,%.5g) segmentPoint=(%.5g,%.5g,%.5g) palmCurrentMask=%u palmAreaWorldMeters=%.5g/%.5g press=%llu/%llu nativeUse=SEPARATE_ACTION_RECEIPT unseen=-1 writes=NONE",
    (unsigned long long)p->session,(unsigned long long)p->serial,r.id,r.ordinal,r.rankWord,r.stageMask,dd3::stop(r),r.policy58[0],r.policy58[1],r.pointInside,r.distanceKnown,double(r.centreDistanceSq),r.segmentKnown,double(r.segmentDistance),r.facing,r.held,r.screen,r.descriptor,r.policy60,r.localKnown,
    double(r.local[0]),double(r.local[1]),double(r.local[2]),double(r.local[3]),double(r.local[4]),double(r.local[5]),r.boundsKnown,
    double(r.box[0]),double(r.box[1]),double(r.box[2]),double(r.box[3]),double(r.box[4]),double(r.box[5]),double(r.segmentPoint[0]),double(r.segmentPoint[1]),double(r.segmentPoint[2]),r.palmCurrent,double(r.palmDistance[0]),double(r.palmDistance[1]),(unsigned long long)r.press[0],(unsigned long long)r.press[1]);diagnostic_trace::Write(line);
  }
  if(c.line()){char line[460]{};std::snprintf(line,sizeof(line),"C54R3 F5_DOOR_SELECTOR session=%llu collector=%llu callerRva=7F5727 parent=7F55FC interactor=%p elapsedMs=%llu visited=%u doors=%u retained=%u truncated=%u sampling=FIRST4_THEN_250MS maximumDetails=512 actionAcceptance=NOT_IMPLIED",
    (unsigned long long)p->session,(unsigned long long)p->serial,reinterpret_cast<void*>(interactor),(unsigned long long)(now>=p->tick?now-p->tick:0),p->visited,p->doors,p->count,p->dropped);diagnostic_trace::Write(line);}
  p->detailed=false;
 }
 if(c.summary(now)){char line[520]{};std::snprintf(line,sizeof(line),"C54R3 F5_DOOR_FREQUENCY session=%llu thread=%lu calls=%llu objectCalls=%llu otherPolicyCalls=%llu nested=%llu unknownCaller=%llu elapsedMs=%llu detailed=%llu coalesced=%llu detailLines=%llu detailLineLimit=8192 captureOnly=1",
  (unsigned long long)c.session,(unsigned long)GetCurrentThreadId(),(unsigned long long)c.calls,(unsigned long long)c.objects,(unsigned long long)c.other,(unsigned long long)c.nested,(unsigned long long)c.unknownCaller,(unsigned long long)(now>=c.first?now-c.first:0),(unsigned long long)c.details,(unsigned long long)c.suppressed,(unsigned long long)c.lines);diagnostic_trace::Write(line);}
}
static bool DoorDiagClass3(std::uintptr_t entity,std::uint32_t& id) noexcept {
 std::uintptr_t klass=0,name=0;if(!SelectionVft68(entity,0x4072C78)||!SelectionRead68(entity+0xC,id)||!id||
  !SelectionRead68(entity+0x20,klass)||!SelectionVft68(klass,0x3B282A0)||!SelectionRead68(klass+0x10,name))return false;
 constexpr char expected[]="AnimDoor";for(unsigned i=0;i<sizeof(expected);++i){char value=0;if(!SelectionRead68(name+i,value)||value!=expected[i])return false;}return true;
}
template<dd3::Stage Stage> static void DoorDiagMid3(DetourModKit::hook::MidContext& ctx) noexcept {
 using namespace DetourModKit::hook;const auto now=std::uint64_t(GetTickCount64());auto* p=DoorDiagPass3(now);if(!p)return;
 const auto frame=gpr(ctx,Gpr::Rbp);if(gpr(ctx,Gpr::R15)!=p->interactor||!frame||(p->frame&&p->frame!=frame))return;p->frame=frame;
 if constexpr(Stage==dd3::Enumerated){
  ++p->visited;p->current=-1;std::uintptr_t entity=0;std::uint32_t id=0;
  if(!SelectionRead68(gpr(ctx,Gpr::Rbx)+8,entity)||!DoorDiagClass3(entity,id))return;
  ++p->doors;if(p->count>=dd3::Capacity){++p->dropped;return;}
  p->current=int(p->count++);auto& r=p->rows[p->current];r.id=id;r.entity=entity;r.ordinal=p->visited;
  (void)SelectionRead68(gpr(ctx,Gpr::Rbx),r.rankWord);dd3::stage(r,Stage);return;
 }
 if constexpr(Stage==dd3::Sorted){
  std::uintptr_t begin=0,end=0;if(!SelectionRead68(frame-0x60,begin)||!SelectionRead68(frame-0x58,end)||end<begin||(end-begin)%0x38||(end-begin)/0x38>4096){
   if(g_door_diag_counts3.line())diagnostic_trace::Write("C54R3 F5_DOOR_ORDER refused=NATIVE_VECTOR_LAYOUT_OR_READ unchanged=1");return;}
  std::uintptr_t broadphaseCount=0;float origin[3]{};const bool inputKnown=SelectionRead68(frame+0x38,broadphaseCount)&&SelectionCopy68(frame+0x80,origin,sizeof(origin));
  if(g_door_diag_counts3.line()){char line[520]{};std::snprintf(line,sizeof(line),"C54R3 F5_DOOR_INPUT session=%llu collector=%llu nativeEntityOriginKnown=%d origin=(%.5g,%.5g,%.5g) broadphaseCount=%llu visited=%u doorsObserved=%u sortedCount=%llu missingDoor=UPSTREAM_IF_NEVER_ENUMERATED originalBroadphase=UNCHANGED",
   (unsigned long long)p->session,(unsigned long long)p->serial,inputKnown,double(origin[0]),double(origin[1]),double(origin[2]),(unsigned long long)broadphaseCount,p->visited,p->doors,(unsigned long long)((end-begin)/0x38));diagnostic_trace::Write(line);}
  const auto count=(end-begin)/0x38,limit=std::min<std::uintptr_t>(count,32);
  diagnostics::interaction_cycle_h4::recorder.collectorOrderUnread.fetch_add(count-limit,std::memory_order_relaxed);
  for(std::uintptr_t i=0;i<limit;++i){struct Record{std::int32_t rankWord;float screenSq,worldSq,pad;std::uint32_t id;};Record r{};
   if(!SelectionCopy68(begin+i*0x38,&r,sizeof(r)))break;
   {namespace h4=diagnostics::interaction_cycle_h4;h4::Event e{};e.stage=h4::Stage::Order;e.tick=now;e.sourceTick=p->tick;e.collector=p->serial;e.target=r.id;e.code=int(i);e.detail=r.rankWord;e.value=r.screenSq;e.value2=r.worldSq;h4::copy(e.reason,sizeof(e.reason),"ACTUAL_SORTED_VECTOR_NOT_FINAL_USE");h4::observe(e,now);}
   // Keep the old early break: never enumerate additional native rows merely
   // because the new recorder has capacity. Account for unobserved remainder.
   if(!g_door_diag_counts3.line()){diagnostics::interaction_cycle_h4::recorder.collectorOrderUnread.fetch_add(limit-i-1,std::memory_order_relaxed);break;}
   char line[420]{};std::snprintf(line,sizeof(line),"C54R3 F5_DOOR_ORDER session=%llu collector=%llu rank=%llu total=%llu target=%u rankWord=%d screenSq=%.7g worldSq=%.7g truncated=%llu sort=RETAIL_1ED7270 finalUse=STILL_REQUIRED",
    (unsigned long long)p->session,(unsigned long long)p->serial,(unsigned long long)i,(unsigned long long)count,r.id,r.rankWord,double(r.screenSq),double(r.worldSq),(unsigned long long)(count-limit));diagnostic_trace::Write(line);}
  return;
 }
 auto* r=DoorDiagRow3(now);if(!r)return;dd3::stage(*r,Stage);const int al=int(gpr(ctx,Gpr::Rax)&0xff);
 if constexpr(Stage==dd3::Bounds){r->boundsKnown=SelectionCopy68(frame+0x18,r->box,sizeof(r->box));}
 if constexpr(Stage==dd3::Policy58){unsigned char flags[2]{};if(SelectionCopy68(gpr(ctx,Gpr::Rax),flags,sizeof(flags))){r->policy58[0]=flags[0];r->policy58[1]=flags[1];}}
 if constexpr(Stage==dd3::Inside||Stage==dd3::Outside){
  r->pointInside=Stage==dd3::Inside?1:0;r->localKnown=SelectionCopy68(frame-0x48,r->local,sizeof(r->local));
  if constexpr(Stage==dd3::Inside){r->policy60=al;r->distanceKnown=SelectionRead68(stack_pointer(ctx)+0x40,r->centreDistanceSq);}
  float inverse[12]{};door_hold65::Frame hands{};bool orderedBounds=r->boundsKnown;for(unsigned a=0;a<3;++a)orderedBounds=orderedBounds&&std::isfinite(r->box[a])&&std::isfinite(r->box[a+3])&&r->box[a]<=r->box[a+3];
  if(orderedBounds&&SelectionCopy68(frame-0x20,inverse,sizeof(inverse))&&door_hold65::frame.read(hands)){
   for(unsigned side=0;side<2;++side){const auto& h=hands.hand[side];r->press[side]=h.press;if(!door_hold65::current(h,now))continue;r->palmCurrent|=1u<<side;
    door19c4::Point closest{};float metres=0;
    const bool areaNear=door_near_area_c54::closest(inverse,r->box,h.point,closest,metres);
    if(areaNear||(std::isfinite(metres)&&metres>.20f))r->palmDistance[side]=metres; // -1 remains unknown on invalid affine/box

   }
  }
 }
 if constexpr(Stage==dd3::Segment){r->segmentKnown=true;r->segmentDistance=xmm(ctx,0).lane<float>(0);(void)SelectionCopy68(frame-0x30,r->segmentPoint,sizeof(r->segmentPoint));}
 if constexpr(Stage==dd3::Screen)r->screen=al;
 if constexpr(Stage==dd3::Descriptor)r->descriptor=al;
 if constexpr(Stage==dd3::Policy60)r->policy60=al;
}

struct DoorDiagPin3 {std::uintptr_t rva;const char* hex;void(*callback)(DetourModKit::hook::MidContext&);};
static constexpr DoorDiagPin3 kDoorDiagPins3[]{
 {0x63FECA,"488b7308488bce488b06ff50088bd0488d4d48e80e070000488b06488bce8b3b",&DoorDiagMid3<dd3::Enumerated>},
 {0x63FF97,"803800740a807801000f8418050000488b06488bceff9000010000488d95e800",&DoorDiagMid3<dd3::Policy58>},
 {0x640131,"84c00f8485030000440f2f4424400f867903000033c0f30f10440524f30f5844",&DoorDiagMid3<dd3::Inside>},
 {0x64026E,"4c8d45d0488d5518488d4db8e851654400410f2ec37a060f8433020000488b06",&DoorDiagMid3<dd3::Outside>},
 {0x64027F,"410f2ec37a060f8433020000488b06488bceff9000010000f30f106dd44d8d45",&DoorDiagMid3<dd3::Segment>},
 {0x640363,"84c00f8453010000410f28d94c8d8580000000488d5548e83560830084c00f84",&DoorDiagMid3<dd3::Screen>},
 {0x64037F,"84c00f8437010000498b0424488bd6498bccff506084c00f8422010000f20f10",&DoorDiagMid3<dd3::Descriptor>},
 {0x640394,"84c00f8422010000f20f104424488b442450f30f104c246c488b0d55d52e04f3",&DoorDiagMid3<dd3::Policy60>},
 {0x6404F7,"488bb51002000048b8cdcccccccccccccc49beb76ddbb66ddbb66d488bce488b",&DoorDiagMid3<dd3::Sorted>},
};
static std::optional<DetourModKit::hook::Hook> g_door_diag_hooks3[sizeof(kDoorDiagPins3)/sizeof(kDoorDiagPins3[0])];
static bool g_door_diag_failed3=false,g_door_diag_prepared3=false;
static const char* g_door_diag_failure3="NONE";static std::uintptr_t g_door_diag_failure_rva3=0;
// Retain the actual install result before rollback can report another error.
// DMK's error labels are static; copy bounded text on this control path only.
static unsigned g_door_diag_backend_code3=0,g_door_diag_backend_extra3=0;
static std::uintptr_t g_door_diag_backend_detail3=0;
static char g_door_diag_backend_name3[80]="NONE",g_door_diag_backend_where3[96]="NONE";
static void DoorDiagClearBackend3() noexcept {
 g_door_diag_backend_code3=g_door_diag_backend_extra3=0;g_door_diag_backend_detail3=0;
 std::snprintf(g_door_diag_backend_name3,sizeof(g_door_diag_backend_name3),"NONE");
 std::snprintf(g_door_diag_backend_where3,sizeof(g_door_diag_backend_where3),"NONE");
}
template<class Error> static void DoorDiagBackend3(const Error& error) noexcept {
 const auto name=DetourModKit::to_string(error.code);
 g_door_diag_backend_code3=unsigned(error.code);g_door_diag_backend_detail3=error.detail;g_door_diag_backend_extra3=error.extra;
 std::snprintf(g_door_diag_backend_name3,sizeof(g_door_diag_backend_name3),"%.*s",int(name.size()),name.data());
 std::snprintf(g_door_diag_backend_where3,sizeof(g_door_diag_backend_where3),"%s",error.where?error.where:"UNKNOWN");
}
static void DoorDiagDisable3() noexcept {
 g_door_diag_ready3.store(false,std::memory_order_release);
 for(auto& h:g_door_diag_hooks3)if(h){try {if(h->is_enabled()&&!h->disable())g_door_diag_failed3=true;}catch(...){g_door_diag_failed3=true;}}
}
static bool DoorDiagEnable3() noexcept {
 if(g_door_diag_ready3.load(std::memory_order_acquire))return true;
 g_door_diag_activation3.fetch_add(1,std::memory_order_acq_rel); // entry token; old in-flight selectors cannot join a new capture
 if(g_door_diag_failed3)return false;
 DoorDiagClearBackend3();g_door_diag_failure3="NONE";g_door_diag_failure_rva3=0;
 if(!g_wh_base||!retail_build112::Ready(g_wh_base)||!g_selection_ready68.load(std::memory_order_acquire)){g_door_diag_failure3="RETAIL_OR_EXISTING_SELECTOR_NOT_READY";return false;}
 if(!g_door_diag_prepared3){
  for(const auto& p:kDoorDiagPins3){unsigned char bytes[32]{};for(unsigned i=0;i<32;++i){const auto hex=[](char v){return v>='0'&&v<='9'?v-'0':v-'a'+10;};bytes[i]=static_cast<unsigned char>((hex(p.hex[2*i])<<4)|hex(p.hex[2*i+1]));}
   if(!MatchEntry(g_wh_base+p.rva,bytes,sizeof(bytes))){g_door_diag_failure3="EXACT_BYTE_PIN_MISMATCH";g_door_diag_failure_rva3=p.rva;g_door_diag_failed3=true;return false;}}
  HMODULE module=nullptr;if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
   reinterpret_cast<LPCWSTR>(&DoorDiagEnable3),&module)||!module){g_door_diag_failure3="CALLBACK_MODULE_PIN_FAILED";g_door_diag_failed3=true;return false;}
  try {using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
   for(unsigned i=0;i<sizeof(kDoorDiagPins3)/sizeof(kDoorDiagPins3[0]);++i){const auto& p=kDoorDiagPins3[i];
    auto h=mid_at(MidRequest{"kcd2vr_f5_door_diagnostics_c54r3",Target{DetourModKit::Address{g_wh_base+p.rva}},options},p.callback);
    if(!h){DoorDiagBackend3(h.error());g_door_diag_failure3="MID_CREATE_FAILED";g_door_diag_failure_rva3=p.rva;g_door_diag_failed3=true;DoorDiagDisable3();return false;}g_door_diag_hooks3[i]=std::move(*h);}
   g_door_diag_prepared3=true;
  }catch(...){g_door_diag_failure3="MID_CREATE_EXCEPTION";g_door_diag_failed3=true;DoorDiagDisable3();return false;}
 }
 try{for(unsigned i=0;i<sizeof(kDoorDiagPins3)/sizeof(kDoorDiagPins3[0]);++i){auto& h=g_door_diag_hooks3[i];
   const auto refuse=[&](){g_door_diag_failure3="MID_ENABLE_FAILED";g_door_diag_failure_rva3=kDoorDiagPins3[i].rva;g_door_diag_failed3=true;DoorDiagDisable3();return false;};
   if(!h)return refuse();
   if(!h->is_enabled()){const auto enabled=h->enable();if(!enabled){DoorDiagBackend3(enabled.error());return refuse();}}
   if(!h->is_enabled())return refuse();}}
 catch(...){g_door_diag_failure3="MID_ENABLE_EXCEPTION";g_door_diag_failed3=true;DoorDiagDisable3();return false;}
 g_door_diag_ready3.store(true,std::memory_order_release);return true;
}
static void DoorDiagReportReady3(bool ready,unsigned mask) noexcept {
 char line[800]{};std::snprintf(line,sizeof(line),"C54R3 F5_DOOR_CAPTURE_READY ready=%d mask=0x%X expectedMask=0x1FF seams=9 selectorCaller=7F5727 originalOnly=1 nativeMemoryWrites=NONE captureOffPatches=DISABLED failure=%s failedRva=0x%llX backend=%s backendCode=0x%X backendWhere=%s backendDetail=0x%llX backendExtra=%u",
  ready?1:0,mask,ready?"NONE":g_door_diag_failure3,(unsigned long long)g_door_diag_failure_rva3,
  g_door_diag_backend_name3,g_door_diag_backend_code3,g_door_diag_backend_where3,
  (unsigned long long)g_door_diag_backend_detail3,g_door_diag_backend_extra3);diagnostic_trace::Write(line);
}
