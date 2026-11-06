// Included in combat_diag_probe.cpp's private namespace. READ-ONLY native callbacks.
// No new game polling, entity setters, input, physics or damage/health writes.
#include "native_combo_fingerprint.inl"
// V176: optional copied settings evidence. No accessor call or settings mutation.
std::atomic<bool> g_combo_timing_ready176{false};
std::atomic<bool> g_combo_close_stack_ready{false};
static bool ComboTimingFingerprint176(std::uintptr_t b) noexcept {
    struct Pin {std::uint32_t rva;unsigned char bytes[16];};
    static constexpr Pin pins[]{
        {0x10AACEB,{0xf3,0x44,0x0f,0x10,0x0d,0x1c,0xf5,0xfe,0x02,0x48,0x8d,0x05,0x85,0xb0,0xd9,0x02}},
        {0x10AAD9A,{0x48,0x8d,0x93,0x84,0x01,0x00,0x00,0x48,0x89,0x44,0x24,0x20,0x41,0xb9,0x02,0x00}},
        {0x10AADAC,{0x48,0x8d,0x0d,0x9d,0xba,0xd9,0x02,0x41,0x0f,0x28,0xd1,0xe8,0x50,0xdd,0x32,0x01}},
        {0x9D59BB,{0x48,0x8d,0x05,0xae,0xf3,0xbf,0x04,0x48,0x83,0xc4,0x28,0xc3,0x48,0x8d,0x0d,0x9a}},
        {0x9111EF,{0xe8,0xa0,0x47,0x0c,0x00,0xf3,0x0f,0x10,0xb0,0x84,0x01,0x00,0x00,0xe8,0x93,0x47}},
        {0x911218,{0xf3,0x0f,0x59,0x88,0x8c,0x01,0x00,0x00,0xf3,0x0f,0x59,0xca,0x0f,0x57,0xd2,0xf3}},
    };
    for(const auto& pin:pins)if(!Readable(reinterpret_cast<void*>(b+pin.rva),16)||
        std::memcmp(reinterpret_cast<void*>(b+pin.rva),pin.bytes,16))return false;
    return b!=0;
}
// Packed start tick/count; no lock, shared non-atomic state, or native work.
std::atomic<std::uint64_t> g_combo_burst176{0};
static bool ComboBurst176(std::uint64_t now) noexcept {
    if(!now||now>(UINT64_MAX>>3))return false;
    auto old=g_combo_burst176.load(std::memory_order_relaxed);
    for(unsigned retry=0;retry<8;++retry){
        const auto start=old>>3,count=old&7u;
        const bool reset=!start||now<start||now-start>=100;
        if(!reset&&count>=4)return false;
        const auto next=reset?(now<<3)|1u:(old+1);
        if(g_combo_burst176.compare_exchange_weak(old,next,std::memory_order_relaxed))return true;
    }
    return false;
}
using ComboStepFn6691=void(__fastcall*)(void*,const std::int32_t*,const std::int32_t*);
namespace nhp134 {namespace combo59c17 {
std::uint64_t BeginStepCB2(void* manager,kcd2vr::combo_lineage19::Owner& saved) noexcept;
void RealStepCB2(void* manager,const kcd2vr::combo_lineage19::Owner& saved,std::uint64_t generation,
    const kcd2vr::diagnostics::combo::Snapshot& before,const kcd2vr::diagnostics::combo::Snapshot& after) noexcept;
}} // CB2, native_combo_grace.inl
using ComboResetFn6691=void(__fastcall*)(void*);
std::optional<DetourModKit::hook::Hook> g_combo_step_hook6691,g_combo_reset_hook6691;
ComboStepFn6691 g_combo_step_original6691=nullptr;
ComboResetFn6691 g_combo_reset_original6691=nullptr;
std::atomic<std::uintptr_t> g_combo_base6691{0};
std::uint64_t g_combo_session6691=0; // set before enabling hooks; immutable thereafter
std::atomic<std::uint64_t> g_combo_calls6691{0},g_combo_exact6691{0},g_combo_rejected6691{0},g_combo_limited6691{0},g_combo_decode6691{0},g_combo_nested6691{0},g_combo_sequence6691{0},g_combo_last6691{0},g_combo_detail6691{0};
std::atomic<std::uint64_t> g_combo_empty_reset_last6691{0},g_combo_empty_reset_skipped6691{0};
static thread_local unsigned g_combo_depth6691=0;
static std::atomic<unsigned> g_combo_gate_once6695{0};
static bool ComboRead6691(std::uint64_t a,void*out,std::size_t n) noexcept {
    return ReadCopy6689(reinterpret_cast<const void*>(static_cast<std::uintptr_t>(a)),out,n);
}
static bool ComboBudget6691(std::atomic<std::uint64_t>& stamp,std::uint64_t now,std::uint64_t interval) noexcept {
    auto prev=stamp.load(std::memory_order_relaxed);
    if(prev&&now>=prev&&now-prev<interval)return false;
    return stamp.compare_exchange_strong(prev,now,std::memory_order_relaxed);
}
struct ComboRequest6691 {
    std::uint64_t tick=0,epoch=0,seq=0,callerRva=0,captureSession=0,consumed=0;
    kcd2vr::combo_lineage19::Owner lineage{};unsigned window=0;
    const char* link="NONE";bool slotSnapshotKnown=false,record=false;
};
static bool ComboLease176(const ComboRequest6691& q) noexcept {
    return q.record&&ObserveCombat168r1()&&q.epoch==(diagnostics::lab::Snapshot()>>4)&&
        q.captureSession==diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load(std::memory_order_acquire);
}
static ComboRequest6691 BeginCombo6691(void*self) noexcept {
    ComboRequest6691 q{};if(!ObserveCombat168r1())return q;
    g_combo_calls6691.fetch_add(1,std::memory_order_relaxed);
    if(g_combo_depth6691){g_combo_nested6691.fetch_add(1,std::memory_order_relaxed);return q;}
    auto reader=ComboRead6691;diagnostics::combo::Identity id{};
    const diagnostics::combo::Layout layout{g_combo_base6691.load(std::memory_order_relaxed)};
    diagnostics::combo::IdentityTrace trace{};
    const auto expected=reinterpret_cast<std::uintptr_t>(frik_pose_probe::LastPlayerEntity());
    if(!diagnostics::combo::Identify(reader,layout,reinterpret_cast<std::uintptr_t>(self),expected,id,&trace)){
        g_combo_rejected6691.fetch_add(1,std::memory_order_relaxed);
        // Only the first native-reference occurrence of each exact failing gate
        // per process. Live modes do not dump an ownership graph.
        const auto bit=1u<<unsigned(trace.gate);
        if(diagnostics::lab::Get(diagnostics::lab::Key::F3)==1&&!(g_combo_gate_once6695.fetch_or(bit)&bit)){
            const auto now=static_cast<std::uint64_t>(GetTickCount64()),ct=g_contact_henry_tick6695.load();
            char m[850]{};std::snprintf(m,sizeof(m),"V66.95-F3-OWNER_BOUNDARY gate=%s manager=%llX actor=%llX player=%llX entity=%llX entityId=%u expectedHenry=%llX observedVtable=%llX expectedVtable=%llX nativeContactCombat=%llX contactAgeMs=%llu authorization=UNCHANGED_REJECT evidence=PARTIAL_CHAIN",
                diagnostics::combo::GateName(trace.gate),(unsigned long long)trace.partial.manager,(unsigned long long)trace.partial.actor,(unsigned long long)trace.partial.player,(unsigned long long)trace.partial.entity,trace.partial.entityId,(unsigned long long)expected,(unsigned long long)trace.observedVtable,(unsigned long long)trace.expectedVtable,(unsigned long long)g_contact_henry_combat6695.load(),(unsigned long long)(ct&&now>=ct?now-ct:UINT64_MAX));diagnostic_trace::Write(m);
        }return q;
    }
    g_combo_exact6691.fetch_add(1,std::memory_order_relaxed);q.seq=g_combo_sequence6691.fetch_add(1)+1;
    q.tick=GetTickCount64();q.epoch=diagnostics::lab::Snapshot()>>4;
    q.captureSession=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load(std::memory_order_acquire);
    // Bounded short burst preserves adjacent RESET/STEP observations.
    if(!ComboBurst176(q.tick)){g_combo_limited6691.fetch_add(1);return q;}
    q.record=true;
    // Copied provenance at this callback's entry, never a recent-time join or
    // an admission ticket. Stored leases are not revalidated by this observer.
    namespace lineage=kcd2vr::combo_lineage19;
    const auto matches=[&](const lineage::Owner& o){return lineage::valid(o)&&o.manager==id.manager&&o.actor==id.actor&&o.key.player==id.player;};
    if(lineage::offer&&matches(*lineage::offer)){q.lineage=*lineage::offer;q.link="SAME_STACK_OFFER_NOT_ACCEPTANCE";}
    else {lineage::SlotLeaseF1 slot{};std::uint64_t consumed=0;
        if(lineage::nativeSlot.read(slot,consumed)&&matches(slot.owner)){
            q.lineage=slot.owner;q.window=slot.window;q.consumed=consumed;q.slotSnapshotKnown=true;q.link="STORED_NATIVE_SLOT_NOT_REVALIDATED";
        }else {lineage::Owner saved{};if(lineage::grace.read(saved)&&matches(saved)){
            q.lineage=saved;q.link="STORED_HIT_GRACE_NOT_REVALIDATED";
        }}
    }
    return q;
}
static void EmitComboSnapshot6691(const ComboRequest6691&q,const char*side,const diagnostics::combo::Snapshot&s,bool detailed) noexcept {
    char summary[300]{};std::size_t used=0;
    for(std::uint32_t i=0;i<s.statesCopied;++i){const auto&st=s.states[i];const int n=std::snprintf(summary+used,sizeof(summary)-used,"%s%u:%u:%u",i?",":"",st.index,st.cursor,st.candidateCount);if(n<0||std::size_t(n)>=sizeof(summary)-used)break;used+=n;}
    char m[1800]{};std::snprintf(m,sizeof(m),"V66.91-F3-COMBO_SNAPSHOT session=%llX tick=%llu epoch=%llu call=%llu side=%s status=%s truncated=%d statesTotal=%u statesCopied=%u candidatesCopied=%u availableTotal=%u lastStepFlag=%u trigSlotsRaw=%u trigNaturalSlotsRaw=%u stateCursorCandidateCounts=\"%s\" details=%d expectedRows=%u",
        (unsigned long long)g_combo_session6691,(unsigned long long)q.tick,(unsigned long long)q.epoch,(unsigned long long)q.seq,side,diagnostics::combo::StatusName(s.status),s.truncated,s.stateCount,s.statesCopied,s.candidatesCopied,s.availableCount,s.lastStepFlag,s.triggerSlotsRaw,s.triggerNaturalSlotsRaw,summary,detailed,detailed?s.candidatesCopied:0);diagnostic_trace::Write(m);
    if(!detailed)return;
    for(std::uint32_t i=0;i<s.candidatesCopied;++i){const auto&c=s.candidates[i];char hex[33]{};for(unsigned j=0;j<16;++j)std::snprintf(hex+2*j,3,"%02X",unsigned(c.techniqueRaw[j]));
        std::snprintf(m,sizeof(m),"V66.91-F3-COMBO_CANDIDATE session=%llX tick=%llu epoch=%llu call=%llu side=%s row=%u state=%u ordinal=%u recipe=%llX name=\"%s\" nameExact=%d recipeIdRaw=%u requiredSteps=%u stepRows=%u flagsRaw=%u extraRaw=%u selectorsRaw=\"%d,%d,%d,%d\" techniqueBytesHex=%s binding=CANDIDATE_NOT_ACCEPTED",
            (unsigned long long)g_combo_session6691,(unsigned long long)q.tick,(unsigned long long)q.epoch,(unsigned long long)q.seq,side,i,c.state,c.ordinal,(unsigned long long)c.recipe,c.name.data(),c.nameExact,c.recipeIdRaw,c.requiredSteps,c.stepCount,c.flagsRaw,c.extraRaw,c.selectorsRaw[0],c.selectorsRaw[1],c.selectorsRaw[2],c.selectorsRaw[3],hex);diagnostic_trace::Write(m);
    }
}
static void EmitComboEpisode6691(const ComboRequest6691&q,const char*op,const diagnostics::combo::Episode&e,bool a0ok,std::int32_t a0,bool a1ok,std::int32_t a1) noexcept {
    if(!ComboLease176(q))return;
    if(std::strcmp(op,"RESET")==0 && e.comparable && !e.changed && e.before.stateCount==0 &&
       !ComboBudget6691(g_combo_empty_reset_last6691,q.tick,1000)){g_combo_empty_reset_skipped6691.fetch_add(1);return;}
    if(e.before.status!=diagnostics::combo::Status::Ok||e.after.status!=diagnostics::combo::Status::Ok)g_combo_decode6691.fetch_add(1);
    const bool epochStable=q.epoch==(diagnostics::lab::Snapshot()>>4);
    // A short slot may open, advance and reset inside 500ms. The already-copied
    // changed episode retains its candidate rows; quiet repeats keep the budget.
    const bool detailed=epochStable&&(e.changed||ComboBudget6691(g_combo_detail6691,q.tick,500));
    const auto stamp=animation_filter::DiagnosticPlaybackStamp();
    diagnostics::combo::CloseStack close{};unsigned closeFrames=0;bool closeSampled=false;
    // Unwind only a changed, nonempty Henry reset from the exact window-false
    // delegate, with F3 still live. No new hook, native reads, polling or
    // per-frame stack walk. Native callbacks have already returned exactly once.
    if(std::strcmp(op,"RESET")==0&&q.callerRva==0xC5B45D&&e.comparable&&e.changed&&e.before.stateCount&&
       g_combo_close_stack_ready.load(std::memory_order_acquire)&&ComboLease176(q)){
        closeSampled=true;
        std::array<void*,32> frames{};std::array<std::uintptr_t,32> rvas{};
        closeFrames=std::min<unsigned>(CaptureStackBackTrace(0,32,frames.data(),nullptr),32);
        const auto base=g_combo_base6691.load(std::memory_order_relaxed);
        for(unsigned i=0;i<closeFrames;++i){const auto pc=reinterpret_cast<std::uintptr_t>(frames[i]);
            if(pc>=base&&pc-base<0x6000000ull)rvas[i]=pc-base;}
        close=diagnostics::combo::InspectCloseStack(rvas.data(),closeFrames);
        if(!ComboLease176(q))return;
    }
    char m[1800]{};std::snprintf(m,sizeof(m),"V66.91-F3-COMBO_EVENT session=%llX tick=%llu epoch=%llu call=%llu op=%s callerRva=%llX manager=%llX actor=%llX player=%llX entity=%u identity=EXACT_PLAYER_CHAIN nativeReturned=1 epochStable=%d comparable=%d observedFieldsChanged=%d arg0Readable=%d arg0Raw=%d arg1Readable=%d arg1Raw=%d argumentEnums=UNKNOWN playbackSeq=%llu playbackAgeMs=%llu rateSkippedTotal=%llu decodeFailuresTotal=%llu nestedSkippedTotal=%llu quietEmptyResets=%llu acceptedHit=UNKNOWN comboAccepted=UNKNOWN finisherEligibility=UNKNOWN victimHealth=UNKNOWN observedScope=CURSORS_AND_CANDIDATES_ONLY closeSampled=%d closeCallerRva=%llX closePath=%s closeFrameMatches=%u stackFrames=%u predicateDetail=UNKNOWN",
        (unsigned long long)g_combo_session6691,(unsigned long long)q.tick,(unsigned long long)q.epoch,(unsigned long long)q.seq,op,(unsigned long long)q.callerRva,(unsigned long long)e.before.identity.manager,(unsigned long long)e.before.identity.actor,(unsigned long long)e.before.identity.player,e.before.identity.entityId,epochStable,epochStable&&e.comparable,epochStable&&e.changed,a0ok,a0,a1ok,a1,(unsigned long long)stamp.serial,(unsigned long long)(stamp.tick&&q.tick>=stamp.tick?q.tick-stamp.tick:UINT64_MAX),(unsigned long long)g_combo_limited6691.load(),(unsigned long long)g_combo_decode6691.load(),(unsigned long long)g_combo_nested6691.load(),(unsigned long long)g_combo_empty_reset_skipped6691.load(),int(closeSampled),(unsigned long long)close.callerRva,closeSampled?diagnostics::combo::ClosePath(close):"NOT_SAMPLED",close.matches,closeFrames);diagnostic_trace::Write(m);
    EmitComboSnapshot6691(q,"BEFORE",e.before,detailed);EmitComboSnapshot6691(q,"AFTER",e.after,detailed);
    if(!ComboLease176(q))return;
    const auto& owner=q.lineage;
    std::snprintf(m,sizeof(m),"H15 F3_COMBO_LINK call=%llu tick=%llu captureSession=%llu manager=%llX actor=%llX link=%s capturedAt=NATIVE_CALLBACK_ENTRY gesture=%llu key=%016llX generation=%llu body=%llu request=%llu recovery=%llu player=%llX release=%llu action=%llX target=%u targetActor=%llX targetEntity=%llX ownerTick=%llu ownerAgeMs=%llu modeGeneration=%llu mode=%u nativeSlotSnapshotKnown=%d slotWindowMsRaw=%u consumedReleaseHighwaterRaw=%llu nativeCause=UNKNOWN nativeAcceptance=UNKNOWN writes=NONE",
        (unsigned long long)q.seq,(unsigned long long)q.tick,(unsigned long long)q.captureSession,(unsigned long long)e.before.identity.manager,(unsigned long long)e.before.identity.actor,q.link,
        (unsigned long long)owner.key.gesture,(unsigned long long)owner.key.weapon,(unsigned long long)owner.key.generation,(unsigned long long)owner.key.body,(unsigned long long)owner.key.request,(unsigned long long)owner.key.recovery,(unsigned long long)owner.key.player,
        (unsigned long long)owner.release,(unsigned long long)owner.action,owner.targetId,(unsigned long long)owner.target,(unsigned long long)owner.targetEntity,(unsigned long long)owner.tick,
        (unsigned long long)(owner.tick&&q.tick>=owner.tick?q.tick-owner.tick:UINT64_MAX),(unsigned long long)owner.modeGeneration,owner.mode,int(q.slotSnapshotKnown),q.window,(unsigned long long)q.consumed);diagnostic_trace::Write(m);
    if(!ComboLease176(q))return;
    float duration=0,minWeight=0;
    const auto base=g_combo_base6691.load(std::memory_order_acquire);
    const bool settingOk=g_combo_timing_ready176.load(std::memory_order_acquire)&&base&&
        ReadCopy6689(reinterpret_cast<void*>(base+0x55D4D70+0x184),&duration,sizeof(duration))&&
        ReadCopy6689(reinterpret_cast<void*>(base+0x55D4D70+0x18C),&minWeight,sizeof(minWeight))&&
        std::isfinite(duration)&&std::isfinite(minWeight);
    if(!ComboLease176(q))return;
    std::snprintf(m,sizeof(m),"V66.176-F3-COMBO_TIMING tick=%llu epoch=%llu call=%llu op=%s copiedSettingsValid=%d comboSlotDuration=%.9g windowMinWeight=%.9g effectiveDuration=UNKNOWN startEvent=UNQUALIFIED contactToNextInput=UNMEASURED mutation=NONE",
        (unsigned long long)q.tick,(unsigned long long)q.epoch,(unsigned long long)q.seq,op,settingOk,
        settingOk?double(duration):0.0,settingOk?double(minWeight):0.0);diagnostic_trace::Write(m);
}
static void __fastcall ComboStepDetour6691(void*self,const std::int32_t*a0,const std::int32_t*a1) noexcept {ClearReadableCache178r23();kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::ComboStep};kcd2vr::attack_xi178r8::HookTimer178r22 ht22;
    const auto original=g_combo_step_original6691;if(!original)return;
    // Timing proof is independent of F3/recording. All reads are bounded at
    // this actual native callback, never polled from the input/render thread.
    kcd2vr::combo_lineage19::Owner cb2Owner{};
    const auto cb2Generation=nhp134::combo59c17::BeginStepCB2(self,cb2Owner);
    const auto cb2Capture=[&](){auto reader=ComboRead6691;return diagnostics::combo::Capture(reader,diagnostics::combo::Layout{g_combo_base6691.load()},reinterpret_cast<std::uintptr_t>(self),reinterpret_cast<std::uintptr_t>(frik_pose_probe::LastPlayerEntity()));};
    const auto cb2Before=cb2Generation?cb2Capture():diagnostics::combo::Snapshot{};
    const auto cb2Finish=[&](){if(cb2Generation)nhp134::combo59c17::RealStepCB2(self,cb2Owner,cb2Generation,cb2Before,cb2Capture());};
    auto q=BeginCombo6691(self);if(!q.record){{kcd2vr::attack_xi178r8::OriginalTimer178r22 ot22;kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::ComboStep};original(self,a0,a1);}cb2Finish();return;}
    const auto caller=KCD2VR_COMBO_CALLER(),base=g_combo_base6691.load();q.callerRva=(caller>=base&&caller-base<0x6000000ull)?caller-base:0;
    std::int32_t v0=0,v1=0;
    const bool ok0=q.record&&ReadCopy6689(a0,&v0,4),ok1=q.record&&ReadCopy6689(a1,&v1,4);
    // No mutex is held through the original native call. Nested callbacks pass through.
    ++g_combo_depth6691;
    const auto e=diagnostics::combo::Around(q.record,[&](){kcd2vr::attack_xi178r8::OriginalTimer178r22 ot22;kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::ComboStep};original(self,a0,a1);},[&](){if(!ComboLease176(q))return diagnostics::combo::Snapshot{};auto reader=ComboRead6691;return diagnostics::combo::Capture(reader,diagnostics::combo::Layout{g_combo_base6691.load()},reinterpret_cast<std::uintptr_t>(self),reinterpret_cast<std::uintptr_t>(frik_pose_probe::LastPlayerEntity()));});
    --g_combo_depth6691;cb2Finish();EmitComboEpisode6691(q,"STEP_FILTER",e,ok0,v0,ok1,v1);
}
static void __fastcall ComboResetDetour6691(void*self) noexcept {ClearReadableCache178r23();kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::ComboReset};kcd2vr::attack_xi178r8::HookTimer178r22 ht22;
    const auto original=g_combo_reset_original6691;if(!original)return;
    auto q=BeginCombo6691(self);if(!q.record){kcd2vr::attack_xi178r8::OriginalTimer178r22 ot22;kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::ComboReset};original(self);return;}
    const auto caller=KCD2VR_COMBO_CALLER(),base=g_combo_base6691.load();q.callerRva=(caller>=base&&caller-base<0x6000000ull)?caller-base:0;
    ++g_combo_depth6691;
    const auto e=diagnostics::combo::Around(q.record,[&](){kcd2vr::attack_xi178r8::OriginalTimer178r22 ot22;kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::ComboReset};original(self);},[&](){if(!ComboLease176(q))return diagnostics::combo::Snapshot{};auto reader=ComboRead6691;return diagnostics::combo::Capture(reader,diagnostics::combo::Layout{g_combo_base6691.load()},reinterpret_cast<std::uintptr_t>(self),reinterpret_cast<std::uintptr_t>(frik_pose_probe::LastPlayerEntity()));});
    --g_combo_depth6691;EmitComboEpisode6691(q,"RESET",e,false,0,false,0);
}
#include "physical_finisher_gate_cb1.inl" // CB1 physical combo finisher (9397CC)
static void UninstallCombo6691() noexcept {
    UninstallFinisherCB1();
    g_combo_timing_ready176.store(false,std::memory_order_release);
    g_combo_close_stack_ready.store(false,std::memory_order_release);
    if(g_combo_step_hook6691){(void)g_combo_step_hook6691->disable();g_combo_step_hook6691.reset();}g_combo_step_original6691=nullptr;
    if(g_combo_reset_hook6691){(void)g_combo_reset_hook6691->disable();g_combo_reset_hook6691.reset();}g_combo_reset_original6691=nullptr;
}
static bool InstallCombo6691(std::uintptr_t b) noexcept {
    if(!ComboFingerprint6691(b))return false;g_combo_base6691.store(b);
    g_combo_timing_ready176.store(ComboTimingFingerprint176(b),std::memory_order_release);
    g_combo_close_stack_ready.store(retail_build112::Ready(b),std::memory_order_release);
    FILETIME ft{};GetSystemTimeAsFileTime(&ft);g_combo_session6691=(std::uint64_t(ft.dwHighDateTime)<<32)|ft.dwLowDateTime;
    char session[180]{};std::snprintf(session,sizeof(session),"V66.91-F3-COMBO_SESSION session=%llX schema=1 policy=READ_ONLY",(unsigned long long)g_combo_session6691);diagnostic_trace::Write(session);
    DetourModKit::hook::InlineRequest a{"kcd2vr_v6691_combo_progress_observe",DetourModKit::hook::Target{DetourModKit::Address{b+0xC5C594}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(a),&ComboStepDetour6691);if(!h)return false;
    g_combo_step_hook6691=std::move(*h);g_combo_step_original6691=g_combo_step_hook6691->original<ComboStepFn6691>();
    if(!g_combo_step_original6691||!g_combo_step_hook6691->enable()){UninstallCombo6691();return false;}
    DetourModKit::hook::InlineRequest c{"kcd2vr_v6691_combo_reset_observe",DetourModKit::hook::Target{DetourModKit::Address{b+0xC5E4C0}},DetourModKit::hook::Options{}};
    auto r=DetourModKit::hook::inline_at(std::move(c),&ComboResetDetour6691);if(!r){UninstallCombo6691();return false;}
    g_combo_reset_hook6691=std::move(*r);g_combo_reset_original6691=g_combo_reset_hook6691->original<ComboResetFn6691>();
    if(!g_combo_reset_original6691||!g_combo_reset_hook6691->enable()){UninstallCombo6691();return false;}
    if(InstallFinisherCB1(b))diagnostic_trace::Write("CB1 optional finisher candidate filter installed; directional lead-ins remain native; anatomical contact and paired NPC alignment are unproved");
    else {diagnostic_trace::Write("CB1 COMBO_FINISHER unavailable: exact 9397CC/939310 pins failed, native finisher rule only");
        (void)kcd2vr::visible_failure::publish("physical combo finisher off: game build pins do not match",std::uint64_t(GetTickCount64()));}
    return true;
}
static void ComboStatus6691() noexcept {
    char m[700]{};std::snprintf(m,sizeof(m),"V66.91-F3-COMBO_STATUS hooksReady=%d nativeCalls=%llu exactPlayerCalls=%llu identityRejected=%llu rateSkipped=%llu decodeFailures=%llu nestedSkipped=%llu quietEmptyResets=%llu policy=READ_ONLY noProgressRecords=NOT_PROOF_NO_COMBO",
        bool(g_combo_step_original6691&&g_combo_reset_original6691),(unsigned long long)g_combo_calls6691.load(),(unsigned long long)g_combo_exact6691.load(),(unsigned long long)g_combo_rejected6691.load(),(unsigned long long)g_combo_limited6691.load(),(unsigned long long)g_combo_decode6691.load(),(unsigned long long)g_combo_nested6691.load(),(unsigned long long)g_combo_empty_reset_skipped6691.load());diagnostic_trace::Write(m);
}
