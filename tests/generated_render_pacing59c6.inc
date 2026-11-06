bool DeviceHook::ReadBridgePairIdentity59c6(std::uint64_t& serial, std::uint64_t& camera_frame)
{
    serial=0;camera_frame=0;
    std::unique_lock<std::mutex> lock(s_bridge_capture_mutex,std::try_to_lock);
    if(!lock.owns_lock())return false;
    const auto published=s_bridge_complete_pair_serial.load(std::memory_order_acquire);
    if(published&&g_v6634_pub_left&&g_v6634_pub_right&&g_v6634_pub_serial==published){
        serial=published;camera_frame=s_bridge_complete_pair_camera_frame.load(std::memory_order_acquire);
    }
    return true;
}


void XrManager::OnRenderThreadTick(ID3D12CommandQueue* queue)
{
    const auto tick = render_tick_count_.fetch_add(1, std::memory_order_relaxed) + 1;
    if (tick == 1)
        debug_trace::Write("first DIRECT-queue render-thread tick observed");
    if ((tick % 600) == 0)
        debug_trace::WriteCounter("direct_queue_render_ticks", tick);

    if ((!diagnostic_rendering_.load(std::memory_order_acquire) && !active_vr_bridge_.load(std::memory_order_acquire)) || queue != d3d12_queue_)
        return;

    ID3D12Resource* left = nullptr;
    ID3D12Resource* right = nullptr;
    ID3D12Resource* ui_image = nullptr;
    std::uint64_t receiptRequestH49=0,receiptCameraH49=0;
    bool fresh_present_pickup = false;
    std::uint64_t pickup_present_serial = 0;
    bool pickup_paired_r18 = false; // diagnostic source kind; never used by presentation ownership
    std::uint64_t pickup_wait_ms = 0;
    {
        std::unique_lock<std::mutex> lock(render_mutex_);
        if (!render_request_pending_ || render_request_in_progress_)
            return;

        // Cancel an obsolete scene request before touching its swapchain images.
        // Normal completion/cancellation ownership is unchanged; no timer, extra
        // GPU copy or producer teardown is introduced.
        if(!production_presentation59c2_.Current(render_request_presentation59c2_.ticket)) {
            render_request_success_=false;render_request_ui_copy_success_=false;
            render_request_pending_=false;render_request_done_=true;
            render_cv_.notify_all();return;
        }

        // R59.16C6: use this request's source/layer snapshot, not a newly read
        // global mode. Pair tokens are not desktop PRESENT sequence numbers.
        const bool bridge_active=active_vr_bridge_.load(std::memory_order_acquire)&&
                                 !diagnostic_rendering_.load(std::memory_order_acquire);
        if(bridge_active){
            pacing59c6::Input p{};
            p.paired=pacing59c6::Paired(render_request_presentation59c2_.policy.stereo);
            if(p.paired){
                p.known=DeviceHook::ReadBridgePairIdentity59c6(p.serial,p.camera);
                p.have=p.serial!=0;
                p.last=DeviceHook::V6634LastConsumedPairSerial();
                // Published L/R copies are already submitted on this game queue.
                p.submitted=true;
            }else{
                std::uint64_t tick=0,width=0;std::uint32_t height=0;
                p.known=true;
                p.have=DeviceHook::GetLatestPresentIdentity(&p.serial,&tick,&width,&height,&p.camera);
                p.submitted=DeviceHook::CurrentExecuteSubmittedPresent();
                p.last=last_submitted_desktop_serial59c6_.load(std::memory_order_acquire);
            }
            p.nowNs=pacing59c6::NowNs();p.enqueuedNs=render_request_enqueue_ns59c6_;
            p.hz=estimated_refresh_hz_.load(std::memory_order_acquire);
            p.mode=present_pacing_mode_.load(std::memory_order_acquire);
            p.gate=xr_render_gate_mode_.load(std::memory_order_acquire);
            p.expected=expected_present_camera_frame_.load(std::memory_order_acquire);
            const auto decision=pacing59c6::Decide(p);pacing59c6::Observe(p.paired,decision);
            if(!decision.take())return;
            receiptRequestH49=p.enqueuedNs;receiptCameraH49=p.camera;
            PacingReceiptH49(10,"SOURCE_PICKUP",p.enqueuedNs,0,p.serial,p.camera,0,int(decision.result),
                (p.paired?1u:0u)|(decision.fresh?2u:0u)|(p.submitted?4u:0u));
            fresh_present_pickup=decision.fresh;
            pickup_wait_ms=p.enqueuedNs&&p.nowNs>=p.enqueuedNs?(p.nowNs-p.enqueuedNs)/1000000ull:0;
            pickup_present_serial=p.have?p.serial:0;
            pickup_paired_r18=p.paired;
        }

        render_request_in_progress_ = true;
        render_presentation59c2_=render_request_presentation59c2_;
        left = render_request_images_[0];
        right = render_request_images_[1];
        ui_image = render_request_ui_image_;
        render_cv_.notify_all();
    }

    bool ok = active_vr_bridge_.load(std::memory_order_acquire) && !diagnostic_rendering_.load(std::memory_order_acquire)
        ? SubmitBridgeImagesOnRenderThread(left, right)
        : SubmitDiagnosticImagesOnRenderThread(left, right);
    PacingReceiptH49(11,"COPY_ROUTINE_RETURN_NOT_SCANOUT",receiptRequestH49,0,pickup_present_serial,receiptCameraH49,0,ok?1:0,0);
    bool ui_copy_ok = false;
    if (ok && ui_image && render_request_copy_ui_) {
        ID3D12Resource* menu = menu_quad_source_.load(std::memory_order_acquire);
        ui_copy_ok = menu && CopyMenuTextureToUiImage(menu, ui_image);
        if (!ui_copy_ok) debug_trace::Write("V63.0-F6/F7 UI texture copy failed; gameplay layer preserved");
    }

    if (active_vr_bridge_.load(std::memory_order_acquire) && !diagnostic_rendering_.load(std::memory_order_acquire)) {
        const auto count = fresh_present_pickup
            ? present_pacing_fresh_count_.fetch_add(1, std::memory_order_relaxed) + 1
            : present_pacing_reuse_count_.fetch_add(1, std::memory_order_relaxed) + 1;
        present_pacing_last_wait_ms_.store(pickup_wait_ms,std::memory_order_release);
        diagnostics::xr165::PickupObserved(pickup_present_serial,DeviceHook::BridgeCompletePairCount(),pickup_wait_ms,pickup_paired_r18); // W3 F2 r1 cadence, Bug recorder only
        present_pacing_last_serial_.store(pickup_present_serial,std::memory_order_release);
        if (count <= 8 || (count % 300ull) == 0) {
            char m[320]{};
            std::snprintf(m,sizeof(m),
                "R59.16C6 PRESENT_PACING mode=%d pickup=%s waitMs=%llu serial=%llu lastSourceToken=%llu blitOk=%d source=%s clock=STEADY_NS",
                present_pacing_mode_.load(std::memory_order_acquire),
                fresh_present_pickup ? "FRESH" : "REUSE",
                static_cast<unsigned long long>(pickup_wait_ms),
                static_cast<unsigned long long>(pickup_present_serial),
                static_cast<unsigned long long>(last_submitted_present_serial_.load(std::memory_order_acquire)),
                ok ? 1 : 0,pacing59c6::Paired(render_presentation59c2_.policy.stereo)?"CERTIFIED_PAIR":"DESKTOP_PRESENT");
            debug_trace::Write(m);
        }
    }

    {
        std::lock_guard<std::mutex> lock(render_mutex_);
        render_request_success_ = ok;
        render_request_ui_copy_success_ = ui_copy_ok;
        render_request_done_ = true;
        render_request_pending_ = false;
        render_request_in_progress_ = false;
    }
    render_cv_.notify_all();
}


