// V66.00. Included by native_stereo_reaudit_v6599.inc, same anonymous namespace.
// CPU orchestration/queue evidence only; never advertise these counters as GPU draws.
using V6600OneFn=void(__fastcall*)(void*);
// V66.02: Standard Execute alone returns a byte consumed by +0x8648AA.
using V6600StandardFn=std::uint8_t(__fastcall*)(void*);
using V6600DispatchFn=void(__fastcall*)(void*,void*,void*,std::uintptr_t,int,int);
using V6600QueueFn=void(__fastcall*)(void*,void*);
struct V6600Counters { std::uint64_t dispatch=0,queueCalls=0,queueAppends=0,deform=0,history=0; };
thread_local V6600Counters g_v6600_counts{};
thread_local std::uint64_t g_v6600_standard_serial=0;
thread_local std::uintptr_t g_v6600_active_crv=0;
std::atomic<std::uint64_t> g_v6600_serial{0};
std::atomic<bool> g_v6601_update_observer{false};
V6600StandardFn g_v6600_standard_original=nullptr;
V6600OneFn g_v6600_deform_original=nullptr,g_v6600_history_original=nullptr;
V6600DispatchFn g_v6600_dispatch_original=nullptr;V6600QueueFn g_v6600_queue_original=nullptr;
std::optional<DetourModKit::hook::Hook> g_v6600_standard_hook,g_v6600_dispatch_hook,g_v6600_queue_hook,g_v6600_deform_hook,g_v6600_history_hook;
std::uintptr_t V6600Ptr(std::uintptr_t p) noexcept {return p?SafeReadPtr(reinterpret_cast<const void*>(p)):0;}
std::uint32_t V6600U32(std::uintptr_t p) noexcept {return p?SafeReadU32(reinterpret_cast<const void*>(p)):~0u;}
void V6600PipelineLog(const char* tag,void* pipeline,std::uint64_t n,std::uintptr_t caller) noexcept {
 if(!V6599LogNow(n))return;
 const auto p=reinterpret_cast<std::uintptr_t>(pipeline),c=V6600Ptr(p+0x1978);
 Tracef("V66.00-NATIVE %s n=%llu commandSerial=%llu replay=%d diag=%d crv=%p sameCrv=%d frame=%u selector=%u count=%u pipeline=%p pipeVt=0x%llX phase=%u flags=0x%X dispatchMode=%u filter=0x%X output=%p callerRva=0x%llX cpuUploadEpoch=%llu cpuUploadHash=%016llX tid=%lu",
  tag,(unsigned long long)n,(unsigned long long)g_v6599_scope.serial,g_v6595_replay_guard?1:0,V6601EffectiveDiag(),reinterpret_cast<void*>(c),c==reinterpret_cast<std::uintptr_t>(g_v6599_scope.crv)?1:0,
  unsigned(V6600U32(c+0x2c)),unsigned(V6600U32(c+0x8ee0)),unsigned(V6600U32(c+0xa330)),pipeline,(unsigned long long)WhRva(V6600Ptr(p)),unsigned(V6600U32(p+0x1974)),unsigned(V6600U32(p+0x2c)),unsigned(V6600U32(p+0x1868)),unsigned(V6600U32(p+0x186c)),reinterpret_cast<void*>(V6600Ptr(c+0x8d78)),(unsigned long long)caller,(unsigned long long)g_v6599_upload_count,(unsigned long long)g_v6599_last_upload_hash,(unsigned long)GetCurrentThreadId());
}
void V6600ObserveUpdate(void* pipeline,bool enter) noexcept {
 if(!g_v6601_update_observer.load(std::memory_order_acquire))return;
 const auto n=g_v6600_serial.fetch_add(1,std::memory_order_relaxed)+1;
 V6600PipelineLog(enter?"UPDATE-IN":"UPDATE-OUT",pipeline,n,0); // FrameSync owns the actual detour.
}
void DisableV6600NativeHooks() noexcept {
 g_v6601_update_observer.store(false,std::memory_order_release);
#define V6600_DROP(H) if(H){(void)H->disable();H.reset();}
 V6600_DROP(g_v6600_history_hook);V6600_DROP(g_v6600_deform_hook);V6600_DROP(g_v6600_queue_hook);V6600_DROP(g_v6600_dispatch_hook);V6600_DROP(g_v6600_standard_hook);
#undef V6600_DROP
 g_v6600_standard_original=nullptr;g_v6600_deform_original=nullptr;g_v6600_history_original=nullptr;g_v6600_dispatch_original=nullptr;g_v6600_queue_original=nullptr;
}
#include "native_stereo_signatures_v6600.inc"
