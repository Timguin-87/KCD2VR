XrHudFrame XrPrepare(XrSession session, ID3D12Device* dev, ID3D12CommandQueue* q) noexcept {
    XrHudFrame out{};
    if(!g_enabled.load(std::memory_order_acquire)||session==XR_NULL_HANDLE||!dev||!q||x_failed)return out;
    const auto now=GetTickCount64();
    // Context changes must go through normal shutdown. Do not reuse a swapchain,
    // command allocator or source from another runtime/device/queue.
    if(x_sc!=XR_NULL_HANDLE&&(x_session59c7!=session||x_device59c7!=dev||
       (x_queue59c7&&x_queue59c7!=q)))return out;
    const auto epoch=g_source_epoch59c7.load(std::memory_order_acquire);
    auto previous=[&]() noexcept { XrHudFrame f{};if(x_has_released && x_released_epoch59c7==epoch && now>=x_released_ms && now-x_released_ms<=250){f.ok=true;f.swapchain=x_sc;f.width=x_w;f.height=x_h;}return f; };
    // Never reset an allocator still referenced by GPU work. Reuse the last
    // RELEASED image instead of stalling the headset thread for up to 500 ms.
    if(x_fence && x_fence->GetCompletedValue()<x_fv){++s_busy_skips;return previous();}
    if(x_inflight_source){x_inflight_source->Release();x_inflight_source=nullptr;}
    ID3D12Resource* cache=nullptr;std::uint64_t ms=0,serial=0,sourceEpoch=0;
    {std::lock_guard<std::mutex> lk(g_pub_mutex);if(g_pub_cache){cache=g_pub_cache;cache->AddRef();ms=g_pub_ms;serial=g_pub_serial;sourceEpoch=g_pub_epoch59c7;}}
    if(!cache)return previous();
    struct Rel{ID3D12Resource*r;~Rel(){if(r)r->Release();}} rel{cache};
    if(sourceEpoch!=epoch||!serial)return previous();
    const auto cd=cache->GetDesc();
    const bool resize=x_sc!=XR_NULL_HANDLE && (x_w!=int32_t(cd.Width)||x_h!=int32_t(cd.Height)||x_fmt!=cd.Format);
    if(resize && x_acquired){
        XrSwapchainImageWaitInfo wi{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};wi.timeout=0;
        const auto wr=xrWaitSwapchainImage(x_sc,&wi);
        if(wr==XR_TIMEOUT_EXPIRED){++s_wait_timeouts;return previous();}
        if(XR_FAILED(wr)){x_failed=true;++s_xr_copy_fail;return out;}
        XrSwapchainImageReleaseInfo ri{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        if(XR_FAILED(xrReleaseSwapchainImage(x_sc,&ri))){x_failed=true;++s_xr_copy_fail;return out;}
        x_acquired=false;x_has_released=false;
    }
    if(!XrEnsure(session,dev,cd))return previous();
    x_queue59c7=q;
    if(now<ms||now-ms>250){++s_age_expired162r1;return previous();}
    // Only a successful copy + release owns this identity. Never skip a pending
    // acquisition, and never extend the original content timestamp on reuse.
    if(!x_acquired&&x_has_released&&x_released_serial59c7==serial&&x_released_epoch59c7==sourceEpoch){
        ++s_same_reused59c7;return previous();
    }
    if(!x_acquired){
        XrSwapchainImageAcquireInfo ai{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
        if(XR_FAILED(xrAcquireSwapchainImage(x_sc,&ai,&x_index))){++s_xr_copy_fail;return previous();}
        x_acquired=true;
    }
    XrSwapchainImageWaitInfo wi{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};wi.timeout=0;
    const auto wr=xrWaitSwapchainImage(x_sc,&wi);
    // XR_TIMEOUT_EXPIRED is a positive result, NOT an image ready for use.
    // Keep the acquisition pending and wait it again next frame. No second
    // acquire and no release/reset/copy are legal on this timeout path.
    if(wr==XR_TIMEOUT_EXPIRED){++s_wait_timeouts;return previous();}
    if(XR_FAILED(wr)){x_failed=true;++s_xr_copy_fail;return out;}
    bool ok=x_imgs&&x_index<x_imgs->size();
    if(ok)ok=SUCCEEDED(x_alloc->Reset())&&SUCCEEDED(x_list->Reset(x_alloc,nullptr));
    if(ok){
        auto* img=(*x_imgs)[x_index];
        Barrier(x_list,cache,D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_COPY_SOURCE);
        Barrier(x_list,img,D3D12_RESOURCE_STATE_RENDER_TARGET,D3D12_RESOURCE_STATE_COPY_DEST);
        x_list->CopyResource(img,cache);
        Barrier(x_list,img,D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_RENDER_TARGET);
        Barrier(x_list,cache,D3D12_RESOURCE_STATE_COPY_SOURCE,D3D12_RESOURCE_STATE_COMMON);
        ok=SUCCEEDED(x_list->Close());
    }
    if(ok){ID3D12CommandList* ls[]={x_list};ok=DeviceHook::ExecuteCommandListsBypass(q,1,ls);}
    if(ok){
        // Retain the source reference until our submitted copy retires.
        cache->AddRef();x_inflight_source=cache;
        ++x_fv;ok=SUCCEEDED(q->Signal(x_fence,x_fv));
        if(!ok)x_failed=true; // no unproven allocator reuse after a failed signal
    }
    XrSwapchainImageReleaseInfo ri{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
    if(XR_FAILED(xrReleaseSwapchainImage(x_sc,&ri))){ok=false;x_failed=true;}
    x_acquired=false;
    if(!ok){x_has_released=false;++s_xr_copy_fail;return out;}
    x_released_serial59c7=serial;x_released_epoch59c7=sourceEpoch;
    x_has_released=true;x_released_ms=ms;++s_xr_shown;
    out.ok=true;out.swapchain=x_sc;out.width=x_w;out.height=x_h;return out;
}
