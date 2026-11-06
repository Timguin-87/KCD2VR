// Included by device_hook.cpp and the portable adapter replay. Only D3D12/OS
// leaves are substituted in the replay; these are the production functions.
namespace {
struct TemporalPinsD85 {
    ID3D12Resource* left{};ID3D12Resource* right{};ID3D12Fence* fenceOwner{};
    std::uint64_t generation{},fence{}; // 0=recorded/unsubmitted, UINT64_MAX=signal failed
};
std::array<TemporalPinsD85,64> g_temporal_pins_d85{};
ID3D12Fence* g_temporal_fence_d85=nullptr;
std::uint64_t g_temporal_fence_value_d85=0;
std::uint64_t g_temporal_publications_d85=0;
// F2 receipts only: this observer never authorizes a copy/publication. The
// existing transaction lock serializes its four bounded sampling lanes.
enum class TemporalCaptureTraceD85 : unsigned { RecordRefused, Recorded, SubmitRefused, Published };
struct TemporalCaptureTraceRateD85 {std::uint64_t session{},lastMs{};};
std::array<TemporalCaptureTraceRateD85,4> g_temporal_capture_trace_rate_d85{};
void TraceTemporalCaptureD85(TemporalCaptureTraceD85 stage,const char* reason,
    std::uint64_t epoch,std::uint32_t frame,ID3D12Resource* source,ID3D12GraphicsCommandList* list,
    D3D12_RESOURCE_STATES before,bool barrierHook,std::uint64_t execute=0,
    int modeSdkReady=-1,int gameQueue=-1,int signalled=-1,
    const temporal_capture_d85::Ticket* receipt=nullptr) noexcept {
    namespace cap=diagnostics::capture168;
    const auto now=GetTickCount64();
    if(!cap::active(cap::Domain::Bug,now))return;
    const auto session=cap::gate.session[unsigned(cap::Domain::Bug)].load(std::memory_order_acquire);
    if(!session)return;
    auto& rate=g_temporal_capture_trace_rate_d85[static_cast<unsigned>(stage)];
    // Refusals have separate lanes from successes so a working frame cannot
    // consume the first failure receipt. At most four rows per two seconds.
    if(rate.session==session&&now>=rate.lastMs&&now-rate.lastMs<2000)return;
    const auto owned=g_temporal_owner_d85.Current();
    const bool tracking=g_d82_temporal_tracking.load(std::memory_order_acquire);
    const bool hooks=g_d82_temporal_hooks_ready.load(std::memory_order_acquire);
    const bool late=g_v6624_late_window.load(std::memory_order_acquire);
    const bool sdk=native_upscaler_stereo_d82::PairReady(epoch,frame);
    // Name the failed original guard; all of its state is printed as well.
    // This classification runs only with F2 active and does not replace the
    // production admission predicate below.
    if(reason&&std::strcmp(reason,"entry-admission")==0){
        reason=!list?"missing-list":!source?"missing-source":
            before!=D3D12_RESOURCE_STATE_RENDER_TARGET?"source-state-not-render-target":
            !barrierHook?"barrier-hook-missing":!tracking?"temporal-inactive":
            !hooks?"capture-hooks-not-ready":!late?"late-window-closed":
            owned.recorded?"previous-recording-pending":!owned.generation?"attempt-missing":
            owned.cancelled?"attempt-cancelled":!owned.leftBoundary?"left-boundary-missing":
            !owned.rightComplete?"right-not-complete":owned.epoch!=epoch?"attempt-epoch-mismatch":
            owned.frame!=frame?"attempt-frame-mismatch":!sdk?"pair-sdk-receipt-missing":
            owned.source&&owned.source!=reinterpret_cast<std::uintptr_t>(source)?"attempt-source-mismatch":
            "entry-state-changed-before-receipt";
    }
    const char* stageName=stage==TemporalCaptureTraceD85::RecordRefused?"RECORD_REFUSED":
        stage==TemporalCaptureTraceD85::Recorded?"RECORDED":
        stage==TemporalCaptureTraceD85::SubmitRefused?"SUBMIT_WITHHELD":"PUBLISHED";
    if(!cap::active(cap::Domain::Bug,now)||
       session!=cap::gate.session[unsigned(cap::Domain::Bug)].load(std::memory_order_acquire))return;
    rate={session,now};
    char line[1152]{};std::snprintf(line,sizeof(line),
        "D85R5R4 TEMPORAL_CAPTURE stage=%s reason=%s session=%llu epoch=%llu frame=%u attempt=%llu attemptEpoch=%llu attemptFrame=%u recording=%llu "
        "list=%p source=%p attemptSource=%p before=0x%X barrierHook=%d temporal=%d hooks=%d late=%d leftBoundary=%d rightComplete=%d leftQueued=%d pending=%d cancelled=%d "
        "pairSdkReady=%d modeSdkReady=%d gameQueue=%d signalled=%d execute=%llu receiptAttempt=%llu receiptRecording=%llu receiptEpoch=%llu receiptFrame=%u gpuContent=UNVERIFIED",
        stageName,reason?reason:"unspecified",(unsigned long long)session,(unsigned long long)epoch,frame,
        (unsigned long long)owned.generation,(unsigned long long)owned.epoch,owned.frame,(unsigned long long)owned.recording,
        reinterpret_cast<void*>(list),reinterpret_cast<void*>(source),reinterpret_cast<void*>(owned.source),unsigned(before),
        barrierHook?1:0,tracking?1:0,hooks?1:0,late?1:0,owned.leftBoundary?1:0,owned.rightComplete?1:0,
        owned.leftQueued?1:0,owned.recorded?1:0,owned.cancelled?1:0,sdk?1:0,modeSdkReady,gameQueue,signalled,
        (unsigned long long)execute,(unsigned long long)(receipt?receipt->generation:0),
        (unsigned long long)(receipt?receipt->recording:0),(unsigned long long)(receipt?receipt->epoch:0),receipt?receipt->frame:0);
    diagnostic_trace::Write(line);
}
void ReleaseTemporalPinsD85(TemporalPinsD85& p) noexcept {
    if(p.left)p.left->Release();
    if(p.right)p.right->Release();
    if(p.fenceOwner)p.fenceOwner->Release();
    p={};
}
void ReapTemporalPinsD85() noexcept {
    for(auto& p:g_temporal_pins_d85)if(p.left&&p.fenceOwner&&p.fence&&p.fence!=UINT64_MAX){
        const auto completed=p.fenceOwner->GetCompletedValue();
        // UINT64_MAX is device removal, not proof of GPU completion.
        if(completed!=UINT64_MAX&&p.fence<=completed)ReleaseTemporalPinsD85(p);
    }
}
void ResetTemporalListD85(ID3D12GraphicsCommandList* list,bool succeeded) noexcept {
    const auto ticket=g_temporal_owner_d85.Current();
    if(!g_temporal_owner_d85.ResetList(reinterpret_cast<std::uintptr_t>(list),succeeded))return;
    for(auto& p:g_temporal_pins_d85)if(p.left&&p.generation==ticket.generation&&!p.fence)ReleaseTemporalPinsD85(p);
}
std::uint64_t RenderedPoseD85() noexcept {
    const auto latest=CameraHook::LastPoseFrameIndex();
    const auto matched=FrameSync::LatestRendererMatchedPoseFrame(50);
    return matched&&latest>=matched?matched:latest; // established V66.46 rule
}
}

bool DeviceHook::RecordTemporalFinalMainD85(ID3D12GraphicsCommandList* list,ID3D12Resource* source,
                                           D3D12_RESOURCE_STATES before) noexcept {
    std::lock_guard<std::recursive_mutex> transaction(g_temporal_mutex_d85);
    const auto epoch=native_upscaler_stereo_d82::CurrentEpoch();
    const auto frame=g_v6624_late_frame.load(std::memory_order_acquire);
    if(!list||!source||before!=D3D12_RESOURCE_STATE_RENDER_TARGET||!s_original_resource_barrier||
       !g_d82_temporal_tracking.load(std::memory_order_acquire)||!g_d82_temporal_hooks_ready.load(std::memory_order_acquire)||
       !g_v6624_late_window.load(std::memory_order_acquire)||
       !g_temporal_owner_d85.CanRecord(epoch,frame,reinterpret_cast<std::uintptr_t>(source),
           native_upscaler_stereo_d82::PairReady(epoch,frame))){
        TraceTemporalCaptureD85(TemporalCaptureTraceD85::RecordRefused,"entry-admission",epoch,frame,
            source,list,before,s_original_resource_barrier!=nullptr);
        return false;
    }
    g_d85_watchdog.Reach(temporal_pair_d85::ScopeOpened);
    std::lock_guard<std::mutex> capture(s_bridge_capture_mutex);
    auto refused=[&](const char* reason) noexcept {
        TraceTemporalCaptureD85(TemporalCaptureTraceD85::RecordRefused,reason,epoch,frame,
            source,list,before,s_original_resource_barrier!=nullptr);
        return false;
    };
    if(source!=s_bridge_current_main_target)return refused("observed-main-target-changed");
    auto* device=s_captured_device.load(std::memory_order_acquire);
    if(!device)return refused("device-missing");
    ReapTemporalPinsD85();
    TemporalPinsD85* pins=nullptr;
    for(auto& p:g_temporal_pins_d85)if(!p.left){pins=&p;break;}
    if(!pins){g_d85_watchdog.Refuse(temporal_pair_d85::Check::CaptureLifetimePressure);return refused("capture-lifetime-slots-busy");} // bounded lifetime pressure
    if(!g_temporal_fence_d85&&FAILED(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&g_temporal_fence_d85))))return refused("capture-fence-create-failed");
    const auto desc=source->GetDesc();
    if(desc.Dimension!=D3D12_RESOURCE_DIMENSION_TEXTURE2D||!desc.Width||!desc.Height||desc.SampleDesc.Count!=1)return refused("capture-source-description");
    auto ensure=[&](ID3D12Resource*& target,D3D12_RESOURCE_DESC& saved) noexcept {
        const bool same=target&&saved.Dimension==desc.Dimension&&saved.Width==desc.Width&&saved.Height==desc.Height&&
            saved.Format==desc.Format&&saved.SampleDesc.Count==desc.SampleDesc.Count&&saved.Flags==desc.Flags;
        if(same)return true;
        if(target){target->Release();target=nullptr;saved={};}
        D3D12_HEAP_PROPERTIES heap{};heap.Type=D3D12_HEAP_TYPE_DEFAULT;heap.CreationNodeMask=heap.VisibleNodeMask=1;
        if(FAILED(device->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&desc,D3D12_RESOURCE_STATE_COMMON,
                nullptr,IID_PPV_ARGS(&target))))return false;
        saved=desc;return true;
    };
    if(!ensure(s_bridge_left_capture,s_bridge_left_desc)||!ensure(s_bridge_right_capture,s_bridge_right_desc))return refused("capture-resource-create-failed");
    // A published or XR-claimed slot is never a recording destination, including
    // startup/re-entry before the first newly certified pair.
    if(s_bridge_left_capture==g_v6634_pub_left||s_bridge_right_capture==g_v6634_pub_right||
       s_bridge_left_capture==g_v6634_xr_claim_left)return refused("capture-slot-published-or-xr-claimed");
    if(!g_temporal_owner_d85.Record(reinterpret_cast<std::uintptr_t>(source),
           reinterpret_cast<std::uintptr_t>(s_bridge_left_capture),reinterpret_cast<std::uintptr_t>(s_bridge_right_capture),
           reinterpret_cast<std::uintptr_t>(list)))return refused("record-reservation-rejected");
    pins->left=s_bridge_left_capture;pins->right=s_bridge_right_capture;
    pins->generation=g_temporal_owner_d85.Current().generation;pins->fence=0;
    pins->left->AddRef();pins->right->AddRef();
    pins->fenceOwner=g_temporal_fence_d85;pins->fenceOwner->AddRef();
    // Native RIGHT has finished. Record before the desktop HUD mirror and before
    // this very list transitions the engine's observed main target to PRESENT.
    D3D12_RESOURCE_BARRIER pre[2]{};
    for(auto& b:pre){b.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;b.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;}
    pre[0].Transition.pResource=source;pre[0].Transition.StateBefore=before;pre[0].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
    pre[1].Transition.pResource=pins->right;pre[1].Transition.StateBefore=D3D12_RESOURCE_STATE_COMMON;pre[1].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_DEST;
    s_original_resource_barrier(list,2,pre);
    list->CopyResource(pins->right,source);
    for(auto& b:pre)std::swap(b.Transition.StateBefore,b.Transition.StateAfter);
    s_original_resource_barrier(list,2,pre);
    g_v6615_right_copy_queued.store(true,std::memory_order_release);
    g_d85_watchdog.Reach(temporal_pair_d85::DrawRecorded);
    TraceTemporalCaptureD85(TemporalCaptureTraceD85::Recorded,"final-main-copy-recorded",epoch,frame,
        source,list,before,s_original_resource_barrier!=nullptr);
    return true;
}

void DeviceHook::FinishTemporalMainSubmissionD85(ID3D12CommandQueue* queue,
    const temporal_capture_d85::Ticket& ticket,std::uint64_t execute) noexcept {
    // Caller holds transaction across original ExecuteCommandLists. This lock
    // also serializes Reset/mode/slot changes; it never acquires an SDK mutex.
    if(!g_temporal_owner_d85.Matches(ticket)){
        TraceTemporalCaptureD85(TemporalCaptureTraceD85::SubmitRefused,"recording-receipt-mismatch",ticket.epoch,ticket.frame,
            reinterpret_cast<ID3D12Resource*>(ticket.source),reinterpret_cast<ID3D12GraphicsCommandList*>(ticket.list),
            D3D12_RESOURCE_STATE_RENDER_TARGET,s_original_resource_barrier!=nullptr,execute,-1,-1,-1,&ticket);
        return;
    }
    g_d85_watchdog.Reach(temporal_pair_d85::StampSubmitted);
    const bool gameQueue=queue==s_captured_queue.load(std::memory_order_acquire);
    const bool ready=g_d82_temporal_tracking.load(std::memory_order_acquire)&&
        native_upscaler_stereo_d82::PairReady(ticket.epoch,ticket.frame);
    const bool publish=g_temporal_owner_d85.Publishable(ticket,ready,gameQueue);
    const auto owned=g_temporal_owner_d85.Current(); // includes a LEFT copy queued after recording
    std::lock_guard<std::mutex> capture(s_bridge_capture_mutex);
    // The copy has been submitted even if its mode or SDK receipt was invalidated.
    // Keep the resources alive until this queue's signal actually retires.
    const auto value=++g_temporal_fence_value_d85;
    const bool signalled=g_temporal_fence_d85&&SUCCEEDED(queue->Signal(g_temporal_fence_d85,value));
    for(auto& p:g_temporal_pins_d85)if(p.left&&p.generation==owned.generation&&!p.fence)
        p.fence=signalled?value:UINT64_MAX;
    if(!publish||!signalled){
        const char* reason=owned.cancelled?"attempt-cancelled":!owned.leftQueued?"left-copy-missing":
            !owned.rightComplete?"right-not-complete":!ready?"mode-or-sdk-not-ready":
            !gameQueue?"wrong-game-queue":!signalled?"capture-fence-signal-failed":"publication-guard";
        TraceTemporalCaptureD85(TemporalCaptureTraceD85::SubmitRefused,reason,ticket.epoch,ticket.frame,
            reinterpret_cast<ID3D12Resource*>(ticket.source),reinterpret_cast<ID3D12GraphicsCommandList*>(ticket.list),
            D3D12_RESOURCE_STATE_RENDER_TARGET,s_original_resource_barrier!=nullptr,execute,ready?1:0,gameQueue?1:0,signalled?1:0,&ticket);
    }
    if(publish&&signalled){
        auto serial=(std::uint64_t(owned.frame)<<32)^execute;if(!serial)serial=execute?execute:1;
        const auto left=owned.leftPose,right=owned.rightPose;
        const auto mode=g_v6638_pair_pose_mode.load(std::memory_order_acquire);
        std::uint64_t pose=right;
        if(mode==1&&left)pose=left;
        else if(mode==2&&right)pose=right;
        else if(mode==3)pose=left?left:right;
        else if(mode==4&&left&&right){const auto lo=std::min(left,right),hi=std::max(left,right);pose=lo+(hi-lo)/2;}
        g_v6638_left_pose_frame.store(left,std::memory_order_release);
        g_v6638_right_pose_frame.store(right,std::memory_order_release);
        g_v6638_shared_pose_frame.store(pose,std::memory_order_release);
        s_bridge_left_submitted_serial.store(serial,std::memory_order_release);
        s_bridge_right_submitted_serial.store(serial,std::memory_order_release);
        V6634PublishWriteSlotLocked(reinterpret_cast<ID3D12Resource*>(owned.left),reinterpret_cast<ID3D12Resource*>(owned.right),serial,
            native_upscaler_stereo_d82::PairOwner{owned.epoch,native_upscaler_stereo_d82::BoundTechnology()});
        s_bridge_complete_pair_camera_frame.store(pose,std::memory_order_release);
        s_bridge_complete_pair_serial.store(serial,std::memory_order_release);
        s_bridge_complete_pair_count.fetch_add(1,std::memory_order_acq_rel);
        native_upscaler_stereo_d82::NotifyPublished(owned.epoch,owned.frame);
        g_v6615_pair_published.store(true,std::memory_order_release);
        g_v6618_live_stream_started.store(true,std::memory_order_release);
        g_d85_watchdog.Published(false,GetTickCount64());
        TraceTemporalCaptureD85(TemporalCaptureTraceD85::Published,"final-main-copy-submitted",ticket.epoch,ticket.frame,
            reinterpret_cast<ID3D12Resource*>(ticket.source),reinterpret_cast<ID3D12GraphicsCommandList*>(ticket.list),
            D3D12_RESOURCE_STATE_RENDER_TARGET,s_original_resource_barrier!=nullptr,execute,1,1,1,&ticket);
        const auto n=++g_temporal_publications_d85;
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Bug,GetTickCount64())&&(n<=4||n%120==0)){
            char line[512]{};std::snprintf(line,sizeof(line),
                "D85R6 TEMPORAL_OUTPUT pair=%llu epoch=%llu frame=%u attempt=%llu recording=%llu execute=%llu source=%p left=%p right=%p leftPose=%llu rightPose=%llu completion=RIGHT_FINISHED_FINAL_MAIN_PRESENT copy=BEFORE_DESKTOP_HUD gpuContent=UNVERIFIED",
                (unsigned long long)n,(unsigned long long)owned.epoch,owned.frame,(unsigned long long)owned.generation,
                (unsigned long long)owned.recording,(unsigned long long)execute,reinterpret_cast<void*>(owned.source),
                reinterpret_cast<void*>(owned.left),reinterpret_cast<void*>(owned.right),(unsigned long long)left,(unsigned long long)right);
            diagnostic_trace::Write(line);
        }
    }
    g_temporal_owner_d85.Finish(ticket);
    ReapTemporalPinsD85();
}
