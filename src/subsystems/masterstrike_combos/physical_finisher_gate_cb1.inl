// Exact-retail per-candidate finisher filter. CB4: ON by default (Miki: the finisher needs its physical move).
// The original native selector still runs once and chooses its own technique.
using FinisherQueryFnCB1=void**(__fastcall*)(void*,void**,int);
std::optional<DetourModKit::hook::Hook> g_finisher_hook_cb1,g_finisher_candidate_cb1;
FinisherQueryFnCB1 g_finisher_original_cb1=nullptr;
std::atomic<bool> g_finisher_ready_cb1{false};
std::atomic<std::uint64_t> g_cb1_queries{0},g_cb1_ready{0},g_cb1_refused{0},g_cb1_physical_ok{0},g_cb1_native_only{0},g_cb1_selected{0};
constexpr std::uintptr_t kFinisherQueryCB1=0x9397CC;
constexpr unsigned char kFinisherQueryEntryCB1[]{0x48,0x89,0x5C,0x24,0x10,0x44,0x89,0x44,0x24,0x18,0x55,0x56,0x57,0x41,0x54,0x41};
// 939310 -> 9397CC call and the scan entry that clears +0xE8 (mov [rdi+0E8h],sil) / reads +0xE0 zone.
constexpr unsigned char kFinisherCallSiteCB1[]{0xE8,0x3B,0x04,0x00,0x00};
constexpr unsigned char kFinisherScanCB1[]{0x44,0x8B,0x97,0xE0,0x00,0x00,0x00,0x48,0x8B,0xDE,0x40,0x88,0xB7,0xE8,0x00,0x00,0x00};
// blade point height above the current opponent's feet. opponent = Henry state+0xCF8 (C_CombatActor or
// player actor), its entity = +0x1F8 -> +0x38 (vft 4072C78), world TM at entity+0x58 (row major, z = m[11]).
// R3 (Codex review): the striking end of a pommel strike is the pommel, not the blade tip.
// CB4: h = release height, hc = where this stroke's line meets the opponent's body (ContactHeightCB4).
static bool OpponentHeightCB1(std::uintptr_t actor,bool pommel,double& h,double& hc) noexcept {
    h=0;hc=0;const auto base=g_combo_base6691.load(std::memory_order_relaxed);
    std::uintptr_t state=0,opp=0,vft=0,owner=0,entity=0;float tm[12]{};float tip[3]{},was[3]{};
    if(!base||!ComboRead6691(actor+0x210,&state,8)||!state||!ComboRead6691(state+0xCF8,&opp,8)||!opp||opp==actor||
       !ComboRead6691(opp,&vft,8)||(vft!=base+0x3E3FAC8&&vft!=base+0x3A80610)||
       !ComboRead6691(opp+0x1F8,&owner,8)||!owner||!ComboRead6691(owner+0x38,&entity,8)||!entity||
       !ComboRead6691(entity,&vft,8)||vft!=base+0x4072C78||!ComboRead6691(entity+0x58,tm,sizeof(tm)))return false;
    if(!frik_pose_probe::WeaponPathWorldCB4(pommel,tip,was))return false;
    const double feet=tm[11];h=double(tip[2])-feet;
    const double p[3]{tip[0],tip[1],tip[2]},q[3]{was[0],was[1],was[2]};
    if(!kcd2vr::finisher_cb1::ProjectContactCB4(p,q,tm[3],tm[7],feet,hc))return false;
    return std::isfinite(feet)&&std::isfinite(h)&&h>-.5&&h<2.6&&hc>-.5&&hc<2.6;
}
// The outer hook identifies Henry once. The exact native row boundary then
// evaluates ONLY the candidate the game is about to instantiate. Rejection
// advances that iterator, allowing another matching native recipe to succeed.
// D60: one bounded F3 receipt follows the actual query, including native refusal
// and native-only paths. It never contributes to the filter's authority.
struct FinisherReceiptD60 {
    std::uint64_t query=0,gesture=0,session=0,tick=0;
    unsigned candidateCalls=0,matched=0,policyPass=0,rejected=0;
    unsigned nativeCandidateCallsR94=0,foreignCandidateCallsR94=0;
    bool nativeBoundaryObservedR94=false;
    const char* candidateGap="NONE";
};
// Observe the original selector even when the optional filter is bypassed.
// TLS scope follows this exact call stack; nested queries cannot borrow it.
struct FinisherObservationR94 {FinisherReceiptD60* receipt=nullptr;std::uintptr_t manager=0;};
thread_local FinisherObservationR94 g_finisherObservationR94{};
struct FinisherObserveScopeR94 {
    FinisherObservationR94 previous=g_finisherObservationR94;
    FinisherObserveScopeR94(FinisherReceiptD60* receipt,void* manager) noexcept {
        g_finisherObservationR94={receipt,reinterpret_cast<std::uintptr_t>(manager)};
    }
    ~FinisherObserveScopeR94(){g_finisherObservationR94=previous;}
};
struct QueryScopeCB1 {
    diagnostics::combo::Snapshot snapshot{};std::uintptr_t stateBegin=0;int requestedZone=-1;
    kcd2vr::finisher_cb1::Kind kind=kcd2vr::finisher_cb1::Kind::None;
    FinisherReceiptD60* receiptD60=nullptr;
    kcd2vr::final_combo_d72::Permit finalD72{};
};
thread_local const QueryScopeCB1* g_query_scope_cb1=nullptr;
// A nested native-only query must not borrow the outer VR candidate scope.
struct QueryRestoreD51 {
    const QueryScopeCB1* previous=g_query_scope_cb1;
    QueryRestoreD51() noexcept {g_query_scope_cb1=nullptr;}
    ~QueryRestoreD51(){g_query_scope_cb1=previous;}
    QueryRestoreD51(const QueryRestoreD51&)=delete;
};
static bool FinisherTraceBudgetD42(std::uint64_t now) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return false;
    static thread_local kcd2vr::finisherdiag_d42::Budget budget;
    return budget.take(diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load(std::memory_order_acquire),now);
}
static bool FinisherReceiptCurrentD60(const FinisherReceiptD60& r) noexcept {
    using namespace diagnostics::capture168;
    return r.query&&gate.session[unsigned(Domain::Combat)].load(std::memory_order_acquire)==r.session&&
        active(Domain::Combat,GetTickCount64());
}
static void FinisherQueryTraceD60(const FinisherReceiptD60& r,const QueryScopeCB1* scope,
    void* manager,int zone,const char* reason,void** out,void** returned) noexcept {
    if(!FinisherReceiptCurrentD60(r))return;
    // The selector's existing return contract is void**. Do not dereference a
    // result on the null-output pass-through path, which the filter never owned.
    const auto reference=out&&returned==out?reinterpret_cast<std::uintptr_t>(*out):0;
    char line[1200]{};std::snprintf(line,sizeof(line),
        "D42_F3 FINISHER_QUERY_D60 query=%llu gesture=%llu session=%llu tick=%llu manager=%llX actor=%llX zone=%d scope=%s snapshot=%s truncated=%d candidateCalls=%u matched=%u policyPass=%u rejected=%u candidateGap=%s nativeCandidateCalls=%u foreignCandidateCalls=%u candidateObservation=%s nativeResult=%s reference=%llX originalCalls=1 gestureJoin=%s selectedRow=NOT_PROVEN actionEntry=NOT_OBSERVED actualRegionContact=NOT_PROVEN acceptedHit=UNOBSERVED writes=NONE",
        (unsigned long long)r.query,(unsigned long long)r.gesture,(unsigned long long)r.session,(unsigned long long)r.tick,
        (unsigned long long)reinterpret_cast<std::uintptr_t>(manager),(unsigned long long)(scope?scope->snapshot.identity.actor:0),zone,reason,
        scope?diagnostics::combo::StatusName(scope->snapshot.status):"NOT_READ",scope?scope->snapshot.truncated:0,
        r.candidateCalls,r.matched,r.policyPass,r.rejected,r.candidateGap,r.nativeCandidateCallsR94,r.foreignCandidateCallsR94,
        !r.nativeBoundaryObservedR94?"UNOBSERVED_HOOK_UNAVAILABLE":r.nativeCandidateCallsR94?"OBSERVED":"NONE_AT_INSTALLED_BOUNDARY",
        !out?"NULL_OUTPUT_NOT_INSPECTED":!returned?"NULL_RESULT_POINTER":returned!=out?"DIFFERENT_OUTPUT_NOT_INSPECTED":reference?"REFERENCE":"NO_REFERENCE",(unsigned long long)reference,
        r.gesture?"OWNED_INPUT_STACK_NOT_ACTION_ACCEPTANCE":"NO_OWNED_GESTURE");
    diagnostic_trace::Write(line);
}
static void FinisherCandidateTraceD42(const QueryScopeCB1& scope,const diagnostics::combo::Candidate& candidate,
    const kcd2vr::finisher_cb1::Row* row,kcd2vr::finisher_cb1::Verdict verdict,bool region,double height,double contact) noexcept {
    if(!scope.receiptD60||!FinisherReceiptCurrentD60(*scope.receiptD60))return;
    const auto now=std::uint64_t(GetTickCount64());if(!FinisherTraceBudgetD42(now))return;
    char raw[33]{};for(unsigned i=0;i<16;++i)std::snprintf(raw+i*2,3,"%02X",unsigned(candidate.techniqueRaw[i]));
    char line[1000]{};std::snprintf(line,sizeof(line),
        "D42_F3 FINISHER_CANDIDATE query=%llu gesture=%llu tick=%llu session=%llu actor=%llX manager=%llX recipe=%llX ordinal=%u cursorSteps=%u nativeZone=%d guidBytes=%s mapped=%s release=%s verdict=%s heightKnown=%d releaseM=%.3f predictedContactM=%.3f region=%u feature=%u motion=%u eligibility=PRECONTACT_ONLY actualRegionContact=NOT_PROVEN nativeTechnique=NOT_DISPATCHED",
        (unsigned long long)scope.receiptD60->query,(unsigned long long)scope.receiptD60->gesture,
        (unsigned long long)now,(unsigned long long)scope.receiptD60->session,
        (unsigned long long)scope.snapshot.identity.actor,(unsigned long long)scope.snapshot.identity.manager,(unsigned long long)candidate.recipe,
        unsigned(candidate.ordinal),unsigned(candidate.requiredSteps),scope.requestedZone,raw,row?row->name:"UNMAPPED",
        kcd2vr::finisher_cb1::name(scope.kind),kcd2vr::finisher_cb1::name(verdict),region,region?height:0.,region?contact:0.,
        row?unsigned(row->region):0,row?unsigned(row->feature):0,row?unsigned(row->motion):0);
    diagnostic_trace::Write(line);
}
static void CandidateMidCB1(DetourModKit::hook::MidContext& ctx) noexcept {
    using namespace DetourModKit::hook;namespace cb=kcd2vr::finisher_cb1;
    if(auto* r=g_finisherObservationR94.receipt){
        if(gpr(ctx,Gpr::Rdi)==g_finisherObservationR94.manager)++r->nativeCandidateCallsR94;
        else ++r->foreignCandidateCallsR94;
    }
    const auto* scope=g_query_scope_cb1;if(!scope)return;
    auto* receipt=scope->receiptD60;if(receipt)++receipt->candidateCalls;
    const auto gap=[&](const char* why){if(receipt)receipt->candidateGap=why;};
    if(!g_finisher_ready_cb1.load(std::memory_order_acquire)||(!scope->finalD72.serial&&!cb::PhysicalFilterEnabledD61())){gap("FILTER_REVOKED");return;}
    const auto& s=scope->snapshot;
    if(gpr(ctx,Gpr::Rdi)!=s.identity.manager){gap("MANAGER_CHANGED");return;}
    const auto staleD72=[&](const char* why){
        gap(why);if(scope->finalD72.serial){instruction_pointer(ctx)=g_combo_base6691.load(std::memory_order_relaxed)+0x939946;
            if(receipt)++receipt->rejected;g_cb1_refused.fetch_add(1,std::memory_order_relaxed);}
    };
    const auto state=gpr(ctx,Gpr::R13),pair=gpr(ctx,Gpr::Rsi);
    if(state<scope->stateBegin||(state-scope->stateBegin)%40){staleD72("STATE_POINTER_CHANGED");return;}
    const auto index=(state-scope->stateBegin)/40;if(index>=s.statesCopied){staleD72("STATE_OUTSIDE_SNAPSHOT");return;}
    std::uintptr_t pairBegin=0,recipe=0;std::uint32_t cursor=0,steps=0;
    if(!ComboRead6691(state+8,&cursor,4)||cursor!=s.states[index].cursor||
       !ComboRead6691(state+0x10,&pairBegin,8)||pair<pairBegin||(pair-pairBegin)%24||
       !ComboRead6691(pair,&recipe,8)||!ComboRead6691(pair+8,&steps,4)||cursor+1!=steps){staleD72("CURSOR_OR_RECIPE_CHANGED");return;}
    const auto ordinal=(pair-pairBegin)/24;
    const diagnostics::combo::Candidate* candidate=nullptr;
    for(unsigned i=0;i<s.candidatesCopied;++i){const auto& c=s.candidates[i];
        if(c.state==index&&c.ordinal==ordinal&&c.recipe==recipe&&c.requiredSteps==steps){candidate=&c;break;}}
    if(!candidate){staleD72("CANDIDATE_NOT_IN_SNAPSHOT");return;} // never reject an unobserved recipe
    if(scope->finalD72.serial){
        const auto& p=scope->finalD72;const auto* physical=cb::findRow(candidate->techniqueRaw.data());
        std::uintptr_t target=0;int input=-1;unsigned char slot=0;
        const bool accepted=kcd2vr::final_combo_d72::live.load(std::memory_order_acquire)==p.serial&&
            kcd2vr::final_combo_d72::selected.load(std::memory_order_acquire)!=p.serial&&
            kcd2vr::final_combo_d72::fresh(p,GetTickCount64())&&
            kcd2vr::final_combo_d72::candidate(p,*candidate,cursor)&&physical&&
            kcd2vr::final_combo_d72::matches(*physical,p.contact)&&gpr(ctx,Gpr::Rax)==p.nativeStep&&
            ComboRead6691(p.state+0xCF8,&target,8)&&target==p.target&&
            ComboRead6691(p.state+0x248,&input,4)&&input==p.input&&
            ComboRead6691(p.state+0x868,&slot,1)&&slot==1;
        if(receipt)++receipt->matched;
        if(accepted){if(receipt)++receipt->policyPass;g_cb1_physical_ok.fetch_add(1,std::memory_order_relaxed);return;}
        staleD72("D72_OTHER_OR_CHANGED_FINAL_CONTACT");return;
    }
    const auto* row=cb::findRow(candidate->techniqueRaw.data());double h=0,hc=0;
    const bool region=cb::needsRegion(row)&&OpponentHeightCB1(s.identity.actor,scope->kind==cb::Kind::Pommel,h,hc);
    const auto verdict=cb::judgePath(row,scope->kind,region,h,hc);
    if(receipt)++receipt->matched;
    FinisherCandidateTraceD42(*scope,*candidate,row,verdict,region,h,hc);
    g_cb1_ready.fetch_add(1,std::memory_order_relaxed);
    if(verdict==cb::Verdict::Ok){if(receipt)++receipt->policyPass;g_cb1_physical_ok.fetch_add(1,std::memory_order_relaxed);return;}
    if(verdict==cb::Verdict::NativeOnly||verdict==cb::Verdict::NoRelease){if(receipt)++receipt->policyPass;g_cb1_native_only.fetch_add(1,std::memory_order_relaxed);return;}
    // Exact retail 939946 advances RSI and reloads the scan selector. No native
    // flags/state/recipe are written and no technique or damage is invented.
    if(!g_finisher_ready_cb1.load(std::memory_order_acquire)||!cb::PhysicalFilterEnabledD61()){gap("FILTER_REVOKED_BEFORE_SKIP");return;}
    instruction_pointer(ctx)=g_combo_base6691.load(std::memory_order_relaxed)+0x939946;
    if(receipt)++receipt->rejected;
    g_cb1_refused.fetch_add(1,std::memory_order_relaxed);
    // say why, where Miki sees it (rate limited) + the receipt. the next native candidate is scanned by 939946.
    const auto now=std::uint64_t(GetTickCount64());static std::uint64_t lastShown=0;
    char wantText[64]{};cb::want(row,wantText,sizeof(wantText));
    if(!lastShown||now-lastShown>1500){lastShown=now;char m[96]{};
        std::snprintf(m,sizeof(m),"combo %s finisher needs %s",row?row->name:candidate->name.data(),wantText);
        (void)kcd2vr::visible_failure::publish(m,now);}

}
static void** __fastcall FinisherQueryDetourCB1(void* mgr,void** out,int zone) noexcept {
    const auto original=g_finisher_original_cb1;
    if(!original){if(out)*out=nullptr;return out;}
    QueryRestoreD51 restore; // mask any outer scope before every native-only path
    FinisherReceiptD60 receipt{};
    // Reserve the summary before candidate detail can consume this query's
    // budget. F3 OFF adds no clock call, native read, formatting or allocation.
    if(diagnostics::capture168::gate.mask.load(std::memory_order_acquire)&(1u<<unsigned(diagnostics::capture168::Domain::Combat))){
        const auto now=std::uint64_t(GetTickCount64());
        if(FinisherTraceBudgetD42(now)){static std::atomic<std::uint64_t> serial{0};
            receipt.query=serial.fetch_add(1,std::memory_order_relaxed)+1;receipt.tick=now;
            receipt.session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load(std::memory_order_acquire);
            receipt.gesture=kcd2vr::finisher_cb1::activeRelease==kcd2vr::finisher_cb1::Kind::None?0:kcd2vr::finisher_cb1::activeGestureD60;}}
    const auto native=[&](const char* reason,const QueryScopeCB1* scope=nullptr,bool filtered=false){
        const bool observed=receipt.query&&g_finisher_ready_cb1.load(std::memory_order_acquire);
        receipt.nativeBoundaryObservedR94=observed;
        FinisherObserveScopeR94 observer(observed?&receipt:nullptr,mgr);
        void** result=original(mgr,out,zone);
        receipt.nativeBoundaryObservedR94=observed&&g_finisher_ready_cb1.load(std::memory_order_acquire);
        if(filtered&&result&&*result)g_cb1_selected.fetch_add(1,std::memory_order_relaxed);
        if(receipt.query)FinisherQueryTraceD60(receipt,scope,mgr,zone,reason,out,result);
        return result;
    };
    if(!g_finisher_ready_cb1.load(std::memory_order_acquire))return native("HOOK_RETIRED_NATIVE_PASS");
    if(!out)return native("NULL_OUTPUT_NATIVE_PASS");
    kcd2vr::final_combo_d72::Permit final{};
    const auto* dispatched=kcd2vr::final_combo_d72::dispatched;
    const bool captured=dispatched?(final=*dispatched,true):kcd2vr::final_combo_d72::read(final,GetTickCount64());
    const bool finalOwned=captured&&final.manager==reinterpret_cast<std::uintptr_t>(mgr)&&
        (dispatched||final.input==zone);
    if(!finalOwned&&kcd2vr::finisher_cb1::nativeValidationTestD61.load(std::memory_order_acquire))return native("D61_NATIVE_FINISHER_VALIDATION_TEST");
    if(!finalOwned&&!kcd2vr::finisher_cb1::enabled.load(std::memory_order_relaxed))return native("FILTER_DISABLED_NATIVE_PASS");
    const auto base=g_combo_base6691.load(std::memory_order_acquire);
    const auto expected=reinterpret_cast<std::uintptr_t>(frik_pose_probe::LastPlayerEntity());
    if(!base||!expected)return native("NO_HENRY_OWNER_NATIVE_PASS");
    QueryScopeCB1 scope{};scope.requestedZone=zone;auto reader=ComboRead6691;
    scope.snapshot=diagnostics::combo::Capture(reader,diagnostics::combo::Layout{base},reinterpret_cast<std::uintptr_t>(mgr),expected);
    scope.kind=kcd2vr::finisher_cb1::activeRelease;
    if(scope.snapshot.status!=diagnostics::combo::Status::Ok)return native("SNAPSHOT_FAILED_NATIVE_PASS",&scope);
    if(scope.snapshot.truncated)return native("SNAPSHOT_TRUNCATED_NATIVE_PASS",&scope);
    if(finalOwned&&final.actor==scope.snapshot.identity.actor&&final.player==scope.snapshot.identity.player&&
       final.entity==scope.snapshot.identity.entity){
        std::uintptr_t state=0,target=0;std::uint32_t targetId=0;
        if(ComboRead6691(final.actor+0x210,&state,8)&&state==final.state&&
           ComboRead6691(state+0xCF8,&target,8)&&target==final.target&&
           ComboRead6691(final.targetEntity+0xC,&targetId,4)&&targetId==final.targetId){
            scope.finalD72=final;receipt.gesture=final.gesture;
            scope.kind=final.physicalKind==3?kcd2vr::finisher_cb1::Kind::Pommel:
                final.physicalKind==2?kcd2vr::finisher_cb1::Kind::Thrust:kcd2vr::finisher_cb1::Kind::Cut;
        }
    }
    if(scope.kind==kcd2vr::finisher_cb1::Kind::None)return native("NO_OWNED_RELEASE_NATIVE_PASS",&scope);
    if(!ComboRead6691(reinterpret_cast<std::uintptr_t>(mgr)+0xC0,&scope.stateBegin,8))return native("STATE_UNREADABLE_NATIVE_PASS",&scope);
    scope.receiptD60=receipt.query?&receipt:nullptr;
    g_query_scope_cb1=&scope;
    g_cb1_queries.fetch_add(1,std::memory_order_relaxed);
    void** result=native(scope.finalD72.serial?"D72_ACTUAL_FINAL_CONTACT":"PHYSICAL_CANDIDATE_FILTER",&scope,true);
    if(scope.finalD72.serial&&out&&result==out&&*out)
        kcd2vr::final_combo_d72::selected.store(scope.finalD72.serial,std::memory_order_release);
    return result;
}
static void UninstallFinisherCB1() noexcept {
    kcd2vr::final_combo_d72::hookReady.store(false,std::memory_order_release);kcd2vr::final_combo_d72::revoke();
    g_finisher_ready_cb1.store(false,std::memory_order_release);
    if(g_finisher_hook_cb1)(void)g_finisher_hook_cb1->disable();
    if(g_finisher_candidate_cb1)(void)g_finisher_candidate_cb1->disable();
    // Keep both handles and original trampoline pinned for callbacks already
    // entered. A retired/partially installed pair is not reacquired this session.
}
static bool InstallFinisherCB1(std::uintptr_t b) noexcept {
    if(g_finisher_hook_cb1||g_finisher_candidate_cb1)return g_finisher_ready_cb1.load(std::memory_order_acquire);
    const auto pin=[&](std::uintptr_t rva,const unsigned char* bytes,std::size_t n){
        return Readable(reinterpret_cast<void*>(b+rva),n)&&!std::memcmp(reinterpret_cast<void*>(b+rva),bytes,n);};
    if(!retail_build112::Ready(b)||!pin(kFinisherQueryCB1,kFinisherQueryEntryCB1,sizeof(kFinisherQueryEntryCB1))||
       !pin(0x93938C,kFinisherCallSiteCB1,sizeof(kFinisherCallSiteCB1))||!pin(0x9398BD,kFinisherScanCB1,sizeof(kFinisherScanCB1)))return false;
    constexpr unsigned char call[]{0xE8,0x3C,0xC1,0xFF,0xFF,0x48,0x8B,0x4F,0x08};
    constexpr unsigned char next[]{0x48,0x83,0xC6,0x18,0x44,0x8B,0x55,0xB7};
    constexpr unsigned char inputFamilyD72[]{0x44,0x39,0x60,0x10,0x0F,0x84,0x43,0x07,0x6E,0xFE,0xE9,0x29,0x07,0x6E,0xFE};
    constexpr unsigned char chargedD72[]{0x41,0x0F,0xBA,0xEF,0x0E,0xE9,0x23,0x08,0x6E,0xFE};
    if(!pin(0x93999B,call,sizeof(call))||!pin(0x939946,next,sizeof(next))||
       !pin(0x225923E,inputFamilyD72,sizeof(inputFamilyD72))||!pin(0x2259256,chargedD72,sizeof(chargedD72)))return false;
    auto mid=DetourModKit::hook::mid_at(DetourModKit::hook::MidRequest{"kcd2vr_cb1_native_candidate",DetourModKit::hook::Target{DetourModKit::Address{b+0x93999B}},DetourModKit::hook::Options{}},&CandidateMidCB1);
    if(!mid)return false;g_finisher_candidate_cb1=std::move(*mid);
    DetourModKit::hook::InlineRequest q{"kcd2vr_cb1_combo_finisher_physical",DetourModKit::hook::Target{DetourModKit::Address{b+kFinisherQueryCB1}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(q),&FinisherQueryDetourCB1);if(!h){UninstallFinisherCB1();return false;}
    g_finisher_hook_cb1=std::move(*h);g_finisher_original_cb1=g_finisher_hook_cb1->original<FinisherQueryFnCB1>();
    if(!g_finisher_original_cb1||!g_finisher_candidate_cb1->enable()||!g_finisher_hook_cb1->enable()){UninstallFinisherCB1();return false;}
    g_finisher_ready_cb1.store(true,std::memory_order_release);kcd2vr::final_combo_d72::hookReady.store(true,std::memory_order_release);return true;
}
