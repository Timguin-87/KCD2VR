// Actual DetourCreateCommandList body, fake D3D leaves, real Windows threads.
// The flag seam makes two callers deterministically observe the outer false
// flag before either enters the installation chain. It is not a timing guess.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>
#include <barrier>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>
#include <unordered_map>

struct ID3D12Device {};
struct ID3D12CommandAllocator {};
struct ID3D12PipelineState {};
struct ID3D12GraphicsCommandList {};
enum D3D12_COMMAND_LIST_TYPE { D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_TYPE_COPY };
namespace native_stereo_outer_lab {
int ClassifyModeShaderHash(std::uint64_t hash) { return hash == 0x1234 ? 7 : 0; }
}

static bool g_synchronize_outer = false;
static std::barrier g_outer_barrier(2);
static thread_local unsigned g_load_count = 0;
struct ObservedFlag {
    std::atomic<bool> value{false};
    bool load(std::memory_order order) {
        const bool result = value.load(order);
        if (g_synchronize_outer && g_load_count++ == 0) g_outer_barrier.arrive_and_wait();
        return result;
    }
    void store(bool v, std::memory_order order) { value.store(v, order); }
};
static std::atomic<int> g_original_calls{0}, g_primary_calls{0}, g_tail[5]{};
static std::atomic<int> g_active_chains{0}, g_max_active{0};
static bool g_fail_primary_once = false, g_fail_original = false, g_null_result = false;
static std::atomic<int> g_argument_errors{0};
static ID3D12Device g_device;
static ID3D12CommandAllocator g_allocator;
static const GUID g_iid = {0x56, 0, 0, {0}};
static thread_local ID3D12GraphicsCommandList g_list;

class DeviceHook {
public:
    using CreateCommandListFunc = HRESULT(__stdcall*)(ID3D12Device*, UINT,
        D3D12_COMMAND_LIST_TYPE, ID3D12CommandAllocator*, ID3D12PipelineState*, REFIID, void**);
    static inline CreateCommandListFunc s_original_create_command_list = nullptr;
    static inline ObservedFlag s_resource_barrier_hook_installed;
    struct ListState { ID3D12PipelineState* pso = nullptr; std::uint64_t shader_hash = 0; int lane = 0, shader_stage = 0; };
    struct ShaderHashes { std::uint64_t h[5]{}; };
    static inline std::mutex s_mode_pso_mutex;
    static inline std::unordered_map<ID3D12GraphicsCommandList*, ListState> s_mode_list_state;
    static inline std::unordered_map<ID3D12PipelineState*, ShaderHashes> s_pso_shader_hashes;
    static HRESULT __stdcall DetourCreateCommandList(ID3D12Device*, UINT,
        D3D12_COMMAND_LIST_TYPE, ID3D12CommandAllocator*, ID3D12PipelineState*, REFIID, void**);
    static bool InstallResourceBarrierHook(ID3D12GraphicsCommandList* p) {
        if (!p) { ++g_argument_errors; return false; }
        const int attempt = ++g_primary_calls;
        const int active = ++g_active_chains;
        int maximum = g_max_active.load();
        while (active > maximum && !g_max_active.compare_exchange_weak(maximum, active)) {}
        if (g_fail_primary_once && attempt == 1) {
            --g_active_chains;
            s_resource_barrier_hook_installed.store(false, std::memory_order_release);
            return false;
        }
        // Mirrors primary CAS semantics: an already-reserved primary returns
        // true, so the old unguarded caller would run its optional tail again.
        bool expected = false;
        s_resource_barrier_hook_installed.value.compare_exchange_strong(expected, true);
        Sleep(5);
        return true;
    }
    static bool Tail(ID3D12GraphicsCommandList* p, int i) {
        if (!p) ++g_argument_errors;
        ++g_tail[i];
        Sleep(1);
        if (i == 4) --g_active_chains;
        return true;
    }
    static bool InstallOMSetRenderTargetsHook(ID3D12GraphicsCommandList* p) { return Tail(p, 0); }
    static bool InstallOutputLayoutHooks(ID3D12GraphicsCommandList* p) { return Tail(p, 1); }
    static bool InstallSetPipelineStateHook(ID3D12GraphicsCommandList* p) { return Tail(p, 2); }
    static bool InstallGraphicsRootDescriptorTableHook(ID3D12GraphicsCommandList* p) { return Tail(p, 3); }
    static bool InstallDrawHooks(ID3D12GraphicsCommandList* p) { return Tail(p, 4); }
};

#include "d56r1_command_list_actual.inc"

static HRESULT __stdcall Original(ID3D12Device* device, UINT node, D3D12_COMMAND_LIST_TYPE,
    ID3D12CommandAllocator* alloc, ID3D12PipelineState*, REFIID iid, void** out) {
    ++g_original_calls;
    if (device != &g_device || node != 3 || alloc != &g_allocator || &iid != &g_iid) ++g_argument_errors;
    if (out) *out = g_null_result ? nullptr : &g_list;
    return g_fail_original ? E_ACCESSDENIED : S_OK;
}
static HRESULT Call(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT,
                    ID3D12PipelineState* state = nullptr, bool null_out = false) {
    void* out = nullptr;
    return DeviceHook::DetourCreateCommandList(&g_device, 3, type, &g_allocator,
                                               state, g_iid, null_out ? nullptr : &out);
}
static bool Check(bool condition, const char* description) {
    std::printf("%s %s\n", condition ? "PASS" : "FAIL", description);
    return condition;
}
static bool TailCount(int expected) {
    for (auto& count : g_tail) if (count.load() != expected) return false;
    return true;
}

int main(int argc, char** argv) {
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
    if (argc != 2) return 2;
    DeviceHook::s_original_create_command_list = Original;
    bool ok = true;
    if (!std::strcmp(argv[1], "concurrent")) {
        g_synchronize_outer = true;
        HRESULT first = E_FAIL, second = E_FAIL;
        std::thread a([&] { first = Call(); });
        std::thread b([&] { second = Call(); });
        a.join(); b.join();
        g_synchronize_outer = false;
        ok &= Check(first == S_OK && second == S_OK && g_original_calls == 2, "both native creations preserved");
        ok &= Check(g_primary_calls == 1 && TailCount(1), "two outer-false contenders install each hook once");
        ok &= Check(g_max_active == 1 && g_active_chains == 0, "complete startup chain has one owner");
        ok &= Check(Call() == S_OK && g_primary_calls == 1 && TailCount(1), "steady state does not install again");
    } else if (!std::strcmp(argv[1], "primary_failure_retry")) {
        g_fail_primary_once = true;
        ok &= Check(Call() == S_OK && g_primary_calls == 1 && TailCount(0), "primary failure preserves create and skips all optional hooks");
        ok &= Check(!DeviceHook::s_resource_barrier_hook_installed.value.load(), "failed primary remains retryable");
        ok &= Check(Call() == S_OK && g_primary_calls == 2 && TailCount(1), "retry installs primary and optional tail once");
        ok &= Check(Call() == S_OK && g_primary_calls == 2 && TailCount(1), "successful retry is not replaced");
    } else if (!std::strcmp(argv[1], "invalid_paths")) {
        DeviceHook::s_original_create_command_list = nullptr;
        ok &= Check(Call() == E_FAIL, "missing original returns failure");
        DeviceHook::s_original_create_command_list = Original;
        g_fail_original = true;
        ok &= Check(Call() == E_ACCESSDENIED, "native error propagated unchanged");
        g_fail_original = false;
        ok &= Check(Call(D3D12_COMMAND_LIST_TYPE_DIRECT, nullptr, true) == S_OK, "null output pointer does not install");
        g_null_result = true;
        ok &= Check(Call() == S_OK, "null output object does not install");
        g_null_result = false;
        ok &= Check(Call(D3D12_COMMAND_LIST_TYPE_COPY) == S_OK, "non-direct command list does not install");
        ok &= Check(g_primary_calls == 0 && TailCount(0), "all ineligible paths leave chain untouched");
    } else if (!std::strcmp(argv[1], "pso_state")) {
        DeviceHook::s_resource_barrier_hook_installed.store(true, std::memory_order_release);
        ID3D12PipelineState pso;
        DeviceHook::ShaderHashes hashes; hashes.h[2] = 0x1234;
        DeviceHook::s_pso_shader_hashes.emplace(&pso, hashes);
        ok &= Check(Call(D3D12_COMMAND_LIST_TYPE_DIRECT, &pso) == S_OK, "initial PSO creation succeeds");
        const auto& st = DeviceHook::s_mode_list_state.at(&g_list);
        ok &= Check(st.pso == &pso && st.shader_hash == 0x1234 && st.lane == 7 && st.shader_stage == 3,
                    "existing PSO and shader classification tail preserved");
        ok &= Check(g_primary_calls == 0 && TailCount(0), "PSO bookkeeping needs no startup reinstall");
    } else return 2;
    ok &= Check(g_argument_errors == 0, "native arguments and list pointers forwarded");
    return ok ? 0 : 1;
}
