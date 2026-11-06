void SetReadPageMemoMs(unsigned ms) noexcept {g_page_memo_ms178r7.store(std::min(ms,150u),std::memory_order_relaxed);}
void NotifyNativeHandUse(int side,unsigned durationMs) noexcept {
    if(side<0||side>1)return;
    const auto until=std::uint64_t(GetTickCount64())+std::min(durationMs,4000u);
    g_native_use_until107[side].store(until,std::memory_order_release);
    g_native_wrist_until107[side].store(until,std::memory_order_release);
    // The existing serial publishes both deadlines. A controller-tool handoff
    // can retire only the serial it observed, never a later accepted action.
    g_native_use_serial107[side].fetch_add(1,std::memory_order_acq_rel);
}
