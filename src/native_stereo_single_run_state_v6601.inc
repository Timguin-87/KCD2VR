// Included after legacy state declarations. Only the owning command thread changes
// Progress; the input worker communicates using atomics. Hooks stay installed.
std::atomic<bool> g_v6601_owned{false},g_v6601_ready{false},g_v6601_busy{false};
std::atomic<int> g_v6601_request{0},g_v6601_public_phase{0}; // requests: 1 start, 2 stop
std::atomic<unsigned long> g_v6601_owner_tid{0};
std::atomic<std::uint64_t> g_v6601_heartbeat{0};
std::atomic<bool> g_v6601_fault{false};
kcd2vr::single_run::Progress g_v6601_progress{};
std::uint64_t g_v6601_epoch=0;
thread_local bool g_v6601_command_active=false;
thread_local kcd2vr::single_run::Phase g_v6601_command_phase=kcd2vr::single_run::Phase::Off;
thread_local std::uint64_t g_v6601_normal_execute=0,g_v6601_command_serial=0;
thread_local void* g_v6601_command_crv=nullptr;
thread_local std::uint64_t g_v6601_snapshot_serial=0;
thread_local void* g_v6601_snapshot_crv=nullptr;

int V6601EffectiveBridgeStage() noexcept {
    if(g_v6601_owned.load(std::memory_order_acquire))
        return g_v6601_command_active && kcd2vr::single_run::Count2(g_v6601_command_phase) ? 3 : 0;
    return g_bridge_stage.load(std::memory_order_acquire);
}
int V6601EffectiveDiag() noexcept {
    if(g_v6601_owned.load(std::memory_order_acquire))
        return g_v6601_command_active ? kcd2vr::single_run::Diag(g_v6601_command_phase) : 0;
    return g_v6594_diag_mode.load(std::memory_order_acquire);
}
