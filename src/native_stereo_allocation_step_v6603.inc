// Included inside native_stereo_preflight's private namespace. Initial creation
// and verified backed resize run before normal Standard Execute. The separate
// eye-control include owns its bounded experiment.
namespace rm=resource_model;
using ProvisionFn=std::uint8_t(__fastcall*)(const char*,void**,std::int32_t,std::int32_t,
                                         const rm::Color*,std::uint8_t,std::uint8_t,
                                         std::uint8_t,std::int32_t,std::uint32_t);
static_assert(sizeof(rm::Color)==16);
const char* AllocationName(int outcome) noexcept {
    switch(outcome) {
    case 1:return "BLOCKED";case 2:return "ATTEMPT-IN-FLIGHT";
    case 3:return "HELPER-RETURNED-FALSE";case 4:return "PROVISIONED-BASIC";
    case 5:return "POSTCONDITION-UNCONFIRMED";case 6:return "ALREADY-BACKED-NOT-MODIFIED";
    case 7:return "PROCESS-ATTEMPT-ALREADY-USED";case 8:return "CANCELLED-BEFORE-CALL";
    default:return "NOT-ATTEMPTED";
    }
}
void PrintAllocationEvidence(const char* stage,const model::Snapshot& s,const rm::Evidence& e) noexcept {
    Log("V66.12-ALLOC CONTEXT stage=%s epoch=%llu pipeline=%p crv=%p frame=%u selector=%u count=%llu "
        "renderer=%p renderThreadObject=%p tid=%lu nativeTidF0=%u nativeTidF4=%u loadingMode=%u "
        "threadRead=%d threadMatch=%d threadStable=%d resourcePipeline=%p ownerRead=%d schedule=%u defer=%u allocationDoesNotRender=1",
        stage,(unsigned long long)t_epoch,reinterpret_cast<void*>(s.pipeline),reinterpret_cast<void*>(s.crv),s.frame,s.selector,
        (unsigned long long)s.builtCount,reinterpret_cast<void*>(e.rt.renderer),reinterpret_cast<void*>(e.rt.threadObject),
        GetCurrentThreadId(),e.rt.idF0,e.rt.idF4,e.rt.loadingMode,e.rt.readable?1:0,e.rt.matches?1:0,e.rt.stable?1:0,
        reinterpret_cast<void*>(e.resourcePipeline),e.ownerRead?1:0,static_cast<unsigned>(s.scheduleEnabled),s.deferSetting);
    for(unsigned eye=0;eye<2;++eye) {
        const auto& m=e.texture[eye];const auto& v=e.view[eye];const auto& x=s.velocity[eye];
        Log("V66.12-ALLOC RESOURCE stage=%s epoch=%llu eye=%u object=%p status=%s backing=%p width=%u height=%u "
            "name=%s metaRead=%d metaStable=%d flags=0x%08X type=%u depth=%u mips=%u srcFormat=%u devFormat=%u customId=%d "
            "referenceRead=%d references=%u viewHeaderRead=%d viewBounds=%d viewCount=%u rtvHandle=2 rtvEntryRead=%d rtv=%p rtvKey=0x%016llX "
            "viewStable=%d descriptorValidity=UNVERIFIED gpuLifetime=UNVERIFIED",
            stage,(unsigned long long)t_epoch,eye,reinterpret_cast<void*>(x.object),model::Name(x.status),
            reinterpret_cast<void*>(x.backing),static_cast<unsigned>(x.width),static_cast<unsigned>(x.height),
            m.readable?m.name:"<unreadable>",m.readable?1:0,m.stable?1:0,m.flags,static_cast<unsigned>(m.type),
            static_cast<unsigned>(m.depth),static_cast<unsigned>(m.mipCount),static_cast<unsigned>(m.sourceFormat),
            static_cast<unsigned>(m.deviceFormat),m.customId,m.referencesRead?1:0,m.nativeReferences,v.headerRead?1:0,v.bounded?1:0,v.count,
            v.entryRead?1:0,reinterpret_cast<void*>(v.rtv),(unsigned long long)v.rtvKey,v.stable?1:0);
    }
}
std::uint8_t InvokeProvision(const rm::TextureMeta& right,const model::Texture& left,std::uintptr_t pipeline) {
    // Ten arguments. Argument 6's slot is unused in the inspected optimized
    // helper, but MUST remain present so arguments 7--10 have the right offsets.
    // The helper ORs in its base flags. Using right.flags preserves the right
    // object's existing flag family; copying all left flags would not do that.
    // Initial unbacked creation can hit the name registry and acquire a reference;
    // that one-shot path retains it until process exit. A backed target takes the
    // separately verified 0x73CFF0 resize branch instead. Native release/create
    // manages its device-resource lifetime; neither caller writes reference counts.
    const rm::Color transparent{};
    const auto provision=reinterpret_cast<ProvisionFn>(g_wh+rm::kHelperRva);
    std::uint8_t result=0;
    t_native_stage=2; // first-fault context: provision, not Standard Execute
    __try {
        result=provision(right.name,reinterpret_cast<void**>(pipeline+model::kVelocitySlots[1]),
                         static_cast<std::int32_t>(left.width),static_cast<std::int32_t>(left.height),
                         &transparent,1,0,rm::kVelocityFormat,right.customId,right.flags);
    } __except(RecordFault(GetExceptionInformation())) { /* always CONTINUE_SEARCH */ }
    t_native_stage=0;
    return result;
}
void AllocationStep(const model::Snapshot& before) {
    const auto tid=static_cast<std::uint32_t>(GetCurrentThreadId());
    const auto initial=rm::CaptureEvidence(g_read,before,g_wh,tid);
    PrintAllocationEvidence("BEFORE",before,initial);
    if(g_allocation_attempts.load(std::memory_order_acquire)!=0 || g_mutation_claimed.load(std::memory_order_acquire)) {
        g_allocation_outcome.store(7,std::memory_order_release);
        Log("V66.12-ALLOC SKIP reason=one-attempt-per-process attemptsTotal=%u noRetry=1 allocationDoesNotRender=1",g_allocation_attempts.load());
        return;
    }
    if(before.velocity[1].status==model::ResourceStatus::BasicFieldsPresent) {
        g_allocation_outcome.store(6,std::memory_order_release);
        Log("V66.12-ALLOC SKIP reason=right-already-backed initialCreationOnly=1 backedResizeOwner=PRE_STANDARD allocationDoesNotRender=1");return;
    }
    const char* rejected=rm::Reject(before,initial,g_wh);
    if(!rejected && g_inflight.load(std::memory_order_acquire)!=1)rejected="overlapping-standard-call";
    if(rejected) {
        g_allocation_outcome.store(1,std::memory_order_release);
        Log("V66.12-ALLOC BLOCKED reason=%s nativeCallIssued=0 allocationDoesNotRender=1",rejected);return;
    }
    Log("V66.12-ALLOC PLAN epoch=%llu helperRva=0x73CEF4 rightName=%s width=%u height=%u "
        "arg6=1 mips=0 format=0x0E customId=%d flags=0x%08X flagsSource=RIGHT-OBJECT clear=0,0,0,0 "
        "boundary=BEFORE-STANDARD-SCHEDULING count2=0 selectorWrites=0 replay=0",
        (unsigned long long)t_epoch,initial.texture[1].name,static_cast<unsigned>(before.velocity[0].width),
        static_cast<unsigned>(before.velocity[0].height),initial.texture[1].customId,initial.texture[1].flags);
    // Re-read after logging and before mutation; refusal leaves the original
    // Standard Execute invocation intact. This is not GPU lifetime pinning.
    const auto latest=model::Capture(g_read,before.pipeline,g_wh);
    const auto latestEvidence=rm::CaptureEvidence(g_read,latest,g_wh,tid);
    rejected=rm::Reject(latest,latestEvidence,g_wh);
    if(!rejected && !rm::SamePrerequisites(before,initial,latest,latestEvidence))rejected="preconditions-changed-before-call";
    if(rejected) {
        g_allocation_outcome.store(1,std::memory_order_release);
        Log("V66.12-ALLOC BLOCKED reason=%s nativeCallIssued=0 allocationDoesNotRender=1",rejected);return;
    }
    int running=Running;
    if(!g_state.compare_exchange_strong(running,Allocating,std::memory_order_acq_rel)) {
        g_allocation_outcome.store(8,std::memory_order_release);return;
    }
    bool unclaimed=false;
    if(!g_mutation_claimed.compare_exchange_strong(unclaimed,true,std::memory_order_acq_rel)) {
        g_allocation_outcome.store(7,std::memory_order_release);
        int allocating=Allocating;(void)g_state.compare_exchange_strong(allocating,Running,std::memory_order_acq_rel);return;
    }
    // Claim is the cancellation boundary. A call accepted here is never aborted
    // mid-allocation. Stop/Poll can disarm future work without freeing resources.
    g_allocation_outcome.store(2,std::memory_order_release);
    g_allocation_attempts.fetch_add(1,std::memory_order_acq_rel);
    Log("V66.12-ALLOC CALL-BEGIN epoch=%llu attemptsTotal=%u nativeRecovery=NOT-ATTEMPTED allocationDoesNotRender=1",
        (unsigned long long)t_epoch,g_allocation_attempts.load());
    const auto result=InvokeProvision(latestEvidence.texture[1],latest.velocity[0],latest.pipeline);
    g_allocation_return.store(static_cast<unsigned>(result),std::memory_order_release);
    const auto after=model::Capture(g_read,latest.pipeline,g_wh);
    const auto afterEvidence=rm::CaptureEvidence(g_read,after,g_wh,tid);
    PrintAllocationEvidence("AFTER",after,afterEvidence);
    const bool basic=rm::PreparedBasic(latest,latestEvidence,after,afterEvidence);
    const int outcome=!result?3:(basic?4:5);
    g_allocation_outcome.store(outcome,std::memory_order_release);
    Log("V66.12-ALLOC RESULT epoch=%llu helperReturned=1 returnByte=%u outcome=%s postconditionsBasic=%d "
        "nativeSlotUsed=1 manualRelease=0 registryReferencePolicy=RETAIN-TO-PROCESS-EXIT allocationDoesNotRender=1 stereoImages=NOT-TESTED",
        (unsigned long long)t_epoch,static_cast<unsigned>(result),AllocationName(outcome),basic?1:0);
    // Cancellation/timeout wins over a late successful allocation return.
    int allocating=Allocating;(void)g_state.compare_exchange_strong(allocating,Running,std::memory_order_acq_rel);
}

// D85R5R4: the old sample-9 provisioner intentionally never resized a backed
// RIGHT. LEFT continued to follow native render-size changes, leaving stereo
// permanently rejected by velocity-size. Keep that validation and repair its
// input before normal Standard Execute, never between the two eye passes.
std::uint64_t g_velocity_resize_attempts=0; // owning render thread only
struct VelocityResizeRetryState {
    std::uintptr_t pipeline{};
    std::array<std::uintptr_t,2> objects{};
    std::array<std::uint32_t,2> sizes{};
    std::uint64_t retryAt{};
};
VelocityResizeRetryState g_velocity_resize_retry{}; // owning render thread only
void DeferVelocityResize(std::uintptr_t pipeline,const std::array<std::uintptr_t,2>& objects,
                          const std::array<std::uint32_t,2>& sizes,std::uint64_t now) noexcept {
    // Refused preconditions can persist. Deep reads retry at most every 250 ms
    // for this exact tuple; a new object or render size is considered immediately.
    g_velocity_resize_retry={pipeline,objects,sizes,now+250};
}
void ResizeLog(const char* stage,const char* why,const model::Snapshot& s,const rm::Evidence& e,
               unsigned result=0,bool confirmed=false) noexcept {
    using diagnostics::capture168::Domain;
    if(!diagnostic_trace::EnabledFor(Domain::Bug))return;
    const auto& l=s.velocity[0];const auto& r=s.velocity[1];
    char line[1300]{};
    std::snprintf(line,sizeof(line),
        "D85R4 VELOCITY_RESIZE stage=%s reason=%s epoch=%llu frame=%u attempt=%llu "
        "pipeline=%p phase=%u selector=%u threadVerified=%d LEFT=%ux%u RIGHT=%ux%u "
        "objects=%p/%p backing=%p/%p rtv=%p/%p rightFlags=0x%08X rightReferences=%u "
        "helperReturn=%u postconditionsBasic=%d lifetime=NATIVE_RETIREMENT geometryWrites=0 GPU_CONTENT_UNVERIFIED",
        stage,why?why:"none",(unsigned long long)t_epoch,s.frame,(unsigned long long)g_velocity_resize_attempts,
        reinterpret_cast<void*>(s.pipeline),s.phase,s.selector,e.rt.matches&&e.rt.stable?1:0,
        unsigned(l.width),unsigned(l.height),unsigned(r.width),unsigned(r.height),
        reinterpret_cast<void*>(l.object),reinterpret_cast<void*>(r.object),
        reinterpret_cast<void*>(l.backing),reinterpret_cast<void*>(r.backing),
        reinterpret_cast<void*>(e.view[0].rtv),reinterpret_cast<void*>(e.view[1].rtv),
        e.texture[1].flags,e.texture[1].nativeReferences,result,confirmed?1:0);
    diagnostic_trace::WriteFor(Domain::Bug,line);
}
bool RejectAfterVelocityResizeFailure(const char* operation) noexcept {
    if(!g_velocity_resize_failed.load(std::memory_order_acquire))return false;
    using diagnostics::capture168::Domain;
    if(diagnostic_trace::EnabledFor(Domain::Bug)) {
        char line[256]{};
        std::snprintf(line,sizeof(line),"D85R4 VELOCITY_RESIZE stage=REFUSED operation=%s "
            "reason=earlier-unconfirmed-native-resize restartRequired=1",operation);
        diagnostic_trace::WriteFor(Domain::Bug,line);
    }
    return true;
}
void ResizeBackedRightVelocity(std::uintptr_t pipeline) {
    if(g_velocity_resize_failed.load(std::memory_order_acquire))return;
    // Healthy frames do only three small reads. Full snapshots, name scans and
    // logging happen on a mismatch; they are not a new per-frame diagnostic.
    static_assert(model::kVelocitySlots[1]==model::kVelocitySlots[0]+sizeof(std::uintptr_t));
    std::array<std::uintptr_t,2> objects{};
    std::array<std::uint32_t,2> packedSize{};
    if(!model::Read(g_read,pipeline,model::kVelocitySlots[0],objects) || !objects[0] || !objects[1] ||
       !model::Read(g_read,objects[0],0x90,packedSize[0]) ||
       !model::Read(g_read,objects[1],0x90,packedSize[1]))return;
    if(packedSize[0]==packedSize[1]) {g_velocity_resize_retry={};return;}
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    const auto& retry=g_velocity_resize_retry;
    if(retry.pipeline==pipeline && retry.objects==objects && retry.sizes==packedSize && now<retry.retryAt)return;
    g_velocity_resize_retry={};
    const auto before=model::Capture(g_read,pipeline,g_wh);
    // Unbacked creation stays under the original one-attempt process budget.
    if(!rm::NeedsBackedResize(before)) {DeferVelocityResize(pipeline,objects,packedSize,now);return;}
    const auto tid=static_cast<std::uint32_t>(GetCurrentThreadId());
    const auto initial=rm::CaptureEvidence(g_read,before,g_wh,tid);
    const char* why=rm::RejectBackedResize(before,initial,g_wh);
    if(!why && g_inflight.load(std::memory_order_acquire)!=1)why="overlapping-standard-call";
    if(why){DeferVelocityResize(pipeline,objects,packedSize,now);ResizeLog("BLOCKED",why,before,initial);return;}
    ResizeLog("BEFORE",nullptr,before,initial);
    const auto latest=model::Capture(g_read,pipeline,g_wh);
    const auto evidence=rm::CaptureEvidence(g_read,latest,g_wh,tid);
    why=rm::RejectBackedResize(latest,evidence,g_wh);
    if(!why && !rm::SameResizePrerequisites(before,initial,latest,evidence))why="preconditions-changed-before-call";
    if(!why && g_inflight.load(std::memory_order_acquire)!=1)why="overlapping-standard-call";
    if(why){DeferVelocityResize(pipeline,objects,packedSize,now);ResizeLog("BLOCKED",why,latest,evidence);return;}
    int running=Running;
    if(!g_state.compare_exchange_strong(running,Allocating,std::memory_order_acq_rel))return;
    ++g_velocity_resize_attempts;
    const auto result=InvokeProvision(evidence.texture[1],latest.velocity[0],latest.pipeline);
    const auto after=model::Capture(g_read,pipeline,g_wh);
    const auto current=rm::CaptureEvidence(g_read,after,g_wh,tid);
    const bool confirmed=result && rm::PreparedBackedResize(latest,evidence,after,current,g_wh);
    ResizeLog("AFTER",confirmed?nullptr:"native-resize-unconfirmed-restart-required",after,current,result,confirmed);
    if(!confirmed) {
        // A failed native mutation is not a dimension mismatch to retry every
        // frame. Its ownership is unconfirmed: retain engine state, publish no
        // new pair, and latch the failure independently of g_state: Stop or
        // timeout can have changed Allocating while the native call ran.
        // Invalidate this temporal generation even if a cached pair existed.
        // Never guess a rollback or turn a failed return into a native exception.
        g_velocity_resize_failed.store(true,std::memory_order_release);
        char failure[320]{};std::snprintf(failure,sizeof(failure),
            "VELOCITY_RESIZE terminal=unconfirmed-native-resize epoch=%llu frame=%u LEFT=%ux%u RIGHT=%ux%u helperReturn=%u restartRequired=1",
            (unsigned long long)t_epoch,after.frame,unsigned(after.velocity[0].width),unsigned(after.velocity[0].height),
            unsigned(after.velocity[1].width),unsigned(after.velocity[1].height),unsigned(result));
        diagnostic_trace::WriteStatus(failure); // once: the failure latch prevents another native call
        native_upscaler_stereo_d82::EndEpoch(t_epoch);
        g_last_run_passed.store(false,std::memory_order_release);
        g_design_pair_serial.store(0,std::memory_order_release);
    }
    int allocating=Allocating;
    (void)g_state.compare_exchange_strong(allocating,confirmed?Running:Faulted,std::memory_order_acq_rel);
}
