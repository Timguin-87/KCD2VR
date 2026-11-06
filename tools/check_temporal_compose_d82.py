#!/usr/bin/env python3
"""Exercise the actual D82 recording adapters and queue claim, with negative controls.

This is a portable CPU/ownership test, not a D3D12/vendor GPU or headset test.
The generated harness extracts production functions from device_hook.cpp; it
does not reimplement their decisions. Native/Windows dependencies are tiny mocks.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess


def braced(source: str, token: str, struct: bool = False) -> str:
    start = source.index(token)
    at = source.index("{", start)
    depth = 0
    # Mask strings/comments without changing offsets before counting braces.
    masked = re.sub(r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                    lambda m: " " * len(m.group()), source)
    for end in range(at, len(source)):
        depth += (masked[end] == "{") - (masked[end] == "}")
        if not depth:
            return source[start:end + (2 if struct else 1)]
    raise AssertionError(token)


def fixture(source: str, eye: str) -> str:
    structures = "\n".join(braced(source, "struct " + n, True)
                           for n in ["V6612ListTag", "V6612SubmitItem"])
    functions = "\n".join(braced(source, token) for token in [
        "V6612ListTag* V6612FindLiveLocked(",
        "void V6612BindTemporalOutputD82(ID3D12GraphicsCommandList* list,std::uintptr_t output) noexcept\n{",
        "void V6612DiscardTemporalRecordingD82(ID3D12GraphicsCommandList* list) noexcept\n{",
        "void V6612ClearTemporalBindingsD82(ID3D12GraphicsCommandList* list) noexcept\n{",
        "void V6612RecordTemporalDrawD82(ID3D12GraphicsCommandList* list,UINT primitives,UINT instances) noexcept\n{",
        "void V6612ResetTracking() noexcept",
        "bool V6612ArchiveSubmitBatch(",
        "bool V6615IncomingLockedRightMainWriter(",
        "temporal_compose_d82::Stamp V6615IncomingRightCompositionD82(",
        "HRESULT __stdcall DeviceHook::DetourResetCommandListD82(",
        "void __stdcall DeviceHook::DetourClearCommandListStateD82(",
    ])
    execution = braced(source, "void __stdcall DeviceHook::DetourExecuteCommandLists(")
    pre = execution[execution.index("    const bool v6615_right_main_incoming ="):
                    execution.index("    bool v6615_left_ok=false;")]
    claim = execution[execution.index("    const auto d82_pair_frame="):
                      execution.index("        const bool right_ok=BridgeCaptureCurrentMainTargetToRight(0);")]
    return r'''
#include "temporal_compose_publication_d82.h"
#include "temporal_pair_watchdog_d85.h"
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <unordered_map>
namespace kcd2vr {
using UINT=unsigned;using HRESULT=int;using DWORD=unsigned;
#define __stdcall
constexpr HRESULT E_FAIL=-1;
struct ID3D12Resource{};struct ID3D12CommandList{};
struct ID3D12GraphicsCommandList:ID3D12CommandList{};
struct ID3D12CommandQueue{};struct ID3D12CommandAllocator{};struct ID3D12PipelineState{};
static DWORD lastError=0;static DWORD GetLastError(){return lastError;}
static void SetLastError(DWORD e){lastError=e;}
namespace diagnostic_trace{static void Write(const char*){}}
// D85 dlss: clock/status leaves used by the extracted starvation fallback.
static std::uint64_t mockNowMs=1;static std::uint64_t GetTickCount64(){return mockNowMs;}
namespace visible_failure{static unsigned published=0;static char last[160]{};
 static bool publish(const char* text,std::uint64_t){++published;std::snprintf(last,sizeof(last),"%s",text);return true;}}
static temporal_pair_d85::Watchdog g_d85_watchdog{};
''' + structures + r'''
static std::mutex g_v6612_list_mutex,s_bridge_capture_mutex;
constexpr std::size_t kV6613LiveListCap=384,kV6613SubmitCap=2048;
static std::array<V6612ListTag,kV6613LiveListCap> g_v6612_live_lists{};
static std::size_t g_v6612_live_count=0;
static std::array<V6612SubmitItem,kV6613SubmitCap> g_v6612_submit_items{};
static std::size_t g_v6612_submit_count=0;
static std::atomic<std::uint64_t> g_v6612_event_seq{0},g_v6615_last_left_main_batch{0};
static std::atomic<std::uint32_t> g_v6613_pair_frame{0},g_v6615_last_left_main_frame{0};
static std::atomic<bool> g_v6612_drain_requested{false},g_v6612_final_dump_done{false},
 g_v6614_right_submitted_seen{false},g_v6615_left_copy_queued{false},g_v6615_right_copy_queued{false},
 g_v6615_pair_published{false},g_v6613_pair_capture_active{false},g_v6618_live_stream_started{false},
 g_d82_temporal_tracking{false},g_v6615_active{false},g_d82_composition_observed{false};
static thread_local temporal_compose_d82::Scope g_d82_compose_scope{};
static thread_local bool g_d82_compose_draw_seen=false;
static ID3D12CommandQueue queueA{},queueB{};
static std::atomic<ID3D12CommandQueue*> s_captured_queue{&queueA};
static ID3D12Resource mainA{},mainB{};
static ID3D12Resource* s_bridge_current_main_target=&mainA;
static void OriginalSubmit(){}
static void(*s_original_execute_command_lists)()=&OriginalSubmit;
static unsigned resetCalls=0,clearCalls=0;
static HRESULT OriginalReset(ID3D12GraphicsCommandList*,ID3D12CommandAllocator*,ID3D12PipelineState*){++resetCalls;lastError=71;return 17;}
static void OriginalClear(ID3D12GraphicsCommandList*,ID3D12PipelineState*){++clearCalls;lastError=72;}
struct DeviceHook {
 using ResetCommandListFunc=HRESULT(*)(ID3D12GraphicsCommandList*,ID3D12CommandAllocator*,ID3D12PipelineState*);
 using ClearCommandListStateFunc=void(*)(ID3D12GraphicsCommandList*,ID3D12PipelineState*);
 static std::atomic<ResetCommandListFunc> s_original_reset_command_list_d82;
 static std::atomic<ClearCommandListStateFunc> s_original_clear_command_list_state_d82;
 static HRESULT DetourResetCommandListD82(ID3D12GraphicsCommandList*,ID3D12CommandAllocator*,ID3D12PipelineState*);
 static void DetourClearCommandListStateD82(ID3D12GraphicsCommandList*,ID3D12PipelineState*);
};
std::atomic<DeviceHook::ResetCommandListFunc> DeviceHook::s_original_reset_command_list_d82{&OriginalReset};
std::atomic<DeviceHook::ClearCommandListStateFunc> DeviceHook::s_original_clear_command_list_state_d82{&OriginalClear};
namespace native_upscaler_stereo_d82 {
 static std::uint64_t receiptEpoch=10;static std::uint32_t receiptFrame=700;static bool ready=false;
 static bool PairReady(std::uint64_t epoch,std::uint32_t frame){return ready&&epoch==receiptEpoch&&frame==receiptFrame;}
 static std::uint64_t boundEpoch=10;
 static bool FrameReady(std::uint32_t frame){return PairReady(boundEpoch,frame);}
}
static int g_read=0;
struct EyeFlow {std::uintptr_t pipeline{};};
namespace model {
 static std::unordered_map<std::uintptr_t,std::uint64_t> values;
 template<class T>static bool Read(int,std::uintptr_t base,std::uintptr_t offset,T& out){
  const auto at=values.find(base+offset);if(at==values.end())return false;
  out=static_cast<T>(at->second);return true;
 }
}
''' + functions + '\n' + braced(eye, "bool DirectFinalCompositionD82(") + r'''
struct Incoming {bool temporal{},nativeMain{};temporal_compose_d82::Stamp composition{};};
static Incoming BeforeSubmit(ID3D12CommandQueue* queue_this,UINT num_command_lists,ID3D12CommandList* const* command_lists){
''' + pre + r'''
 return {d82_temporal,v6615_right_main_incoming,d82_composition};
}
static bool AfterSubmit(const Incoming& incoming){
 const bool d82_temporal=incoming.temporal,v6615_right_main_incoming=incoming.nativeMain;
 const auto d82_composition=incoming.composition;
''' + claim + r'''
        return true;
    }
    return false;
}
} // namespace kcd2vr
using namespace kcd2vr;
static unsigned checks=0,failures=0;
#define CHECK(x) do{++checks;if(!(x)){++failures;std::printf("FAIL %u: %s\n",__LINE__,#x);}}while(false)
static void Arm(bool live=true){
 V6612ResetTracking();g_v6613_pair_capture_active=true;g_v6615_active=true;
 g_v6618_live_stream_started=live;g_d82_temporal_tracking=true;
 g_v6613_pair_frame=700;g_v6615_last_left_main_frame=700;g_v6615_left_copy_queued=true;
 g_d82_compose_scope={10,700,reinterpret_cast<std::uintptr_t>(&mainA)};
 s_bridge_current_main_target=&mainA;native_upscaler_stereo_d82::ready=false;
 s_original_execute_command_lists=&OriginalSubmit;
}
static void MarkMain(ID3D12GraphicsCommandList* list){
 std::lock_guard<std::mutex> lock(g_v6612_list_mutex);
 auto* t=V6612FindLiveLocked(list);t->main_barrier_mask=2;t->eye_mask=2;t->frame[1]=700;
}
int main(){
 ID3D12GraphicsCommandList early{},final{},other{};
 ID3D12CommandList* e[]={&early};ID3D12CommandList* f[]={&final};ID3D12CommandList* o[]={&other};
 for(bool live:{false,true}){
  Arm(live);MarkMain(&early);
  auto earlySubmit=BeforeSubmit(&queueA,1,e);
  CHECK(earlySubmit.nativeMain);CHECK(!earlySubmit.composition.Valid());
  (void)V6612ArchiveSubmitBatch(1,1,e);
  CHECK(!AfterSubmit(earlySubmit));CHECK(!g_v6615_right_copy_queued.load());
  native_upscaler_stereo_d82::ready=true;
  CHECK(!AfterSubmit(earlySubmit));CHECK(!g_v6615_right_copy_queued.load());
  // A real scope draw reuses a previously observed main RTV bind. The final
  // list deliberately has no generic main-writer tag, unlike the early list.
  V6612BindTemporalOutputD82(&final,reinterpret_cast<std::uintptr_t>(&mainA));
  V6612RecordTemporalDrawD82(&final,3,1);
  auto exact=BeforeSubmit(&queueA,1,f);CHECK(exact.composition.Valid());CHECK(!exact.nativeMain);
  CHECK(!BeforeSubmit(&queueB,1,f).composition.Valid());
  CHECK(!BeforeSubmit(&queueA,1,o).composition.Valid());
  native_upscaler_stereo_d82::receiptEpoch=11;CHECK(!AfterSubmit(exact));CHECK(!g_v6615_right_copy_queued.load());
  native_upscaler_stereo_d82::receiptEpoch=10;
  native_upscaler_stereo_d82::receiptFrame=701;CHECK(!AfterSubmit(exact));CHECK(!g_v6615_right_copy_queued.load());
  native_upscaler_stereo_d82::receiptFrame=700;
  g_v6613_pair_frame=701;CHECK(!AfterSubmit(exact));CHECK(!g_v6615_right_copy_queued.load());g_v6613_pair_frame=700;
  s_bridge_current_main_target=&mainB;CHECK(!AfterSubmit(exact));CHECK(!g_v6615_right_copy_queued.load());s_bridge_current_main_target=&mainA;
  s_original_execute_command_lists=nullptr;CHECK(!AfterSubmit(exact));CHECK(!g_v6615_right_copy_queued.load());s_original_execute_command_lists=&OriginalSubmit;
  (void)V6612ArchiveSubmitBatch(2,1,f);
  CHECK(!BeforeSubmit(&queueA,1,f).composition.Valid()); // same pointer cannot replay
  CHECK(AfterSubmit(exact));CHECK(g_v6615_right_copy_queued.load());CHECK(!AfterSubmit(exact));
 }
 Arm();V6612BindTemporalOutputD82(&final,reinterpret_cast<std::uintptr_t>(&mainA));
 V6612RecordTemporalDrawD82(&final,3,1);CHECK(BeforeSubmit(&queueA,1,f).composition.Valid());
 const unsigned beforeReset=resetCalls;
 CHECK(DeviceHook::DetourResetCommandListD82(&final,nullptr,nullptr)==17);
 CHECK(resetCalls==beforeReset+1);CHECK(lastError==71);
 CHECK(!BeforeSubmit(&queueA,1,f).composition.Valid());
 V6612RecordTemporalDrawD82(&final,3,1);CHECK(!BeforeSubmit(&queueA,1,f).composition.Valid());
 V6612BindTemporalOutputD82(&final,reinterpret_cast<std::uintptr_t>(&mainA));
 V6612RecordTemporalDrawD82(&final,3,1);CHECK(BeforeSubmit(&queueA,1,f).composition.Valid());
 const auto beforeClear=clearCalls;
 DeviceHook::DetourClearCommandListStateD82(&final,nullptr);
 CHECK(clearCalls==beforeClear+1);CHECK(lastError==72);
 CHECK(BeforeSubmit(&queueA,1,f).composition.Valid()); // commands survive ClearState
 CHECK(!V6612FindLiveLocked(&final)->temporal_recording.boundOutput);
 // After Reset, a cleared binding cannot create another composition receipt.
 (void)DeviceHook::DetourResetCommandListD82(&final,nullptr,nullptr);
 V6612BindTemporalOutputD82(&final,reinterpret_cast<std::uintptr_t>(&mainA));
 DeviceHook::DetourClearCommandListStateD82(&final,nullptr);
 V6612RecordTemporalDrawD82(&final,3,1);CHECK(!BeforeSubmit(&queueA,1,f).composition.Valid());
 // A later unscoped Draw into the same output invalidates the pending stamp.
 V6612BindTemporalOutputD82(&final,reinterpret_cast<std::uintptr_t>(&mainA));
 V6612RecordTemporalDrawD82(&final,3,1);CHECK(BeforeSubmit(&queueA,1,f).composition.Valid());
 g_d82_compose_scope={};V6612RecordTemporalDrawD82(&final,3,1);
 CHECK(!BeforeSubmit(&queueA,1,f).composition.Valid());
 // Wrong/absent TLS, wrong frame, or a non-main bind cannot acquire ownership.
 Arm();V6612BindTemporalOutputD82(&final,reinterpret_cast<std::uintptr_t>(&mainA));
 g_d82_compose_scope={};V6612RecordTemporalDrawD82(&final,3,1);CHECK(!BeforeSubmit(&queueA,1,f).composition.Valid());
 g_d82_compose_scope={10,701,reinterpret_cast<std::uintptr_t>(&mainA)};
 V6612RecordTemporalDrawD82(&final,3,1);CHECK(!BeforeSubmit(&queueA,1,f).composition.Valid());
 g_d82_compose_scope={10,700,reinterpret_cast<std::uintptr_t>(&mainA)};
 V6612BindTemporalOutputD82(&final,0);V6612RecordTemporalDrawD82(&final,3,1);CHECK(!BeforeSubmit(&queueA,1,f).composition.Valid());
 // Native0 still claims exactly the first main-writer submission. Its Reset
 // pass-through must not mutate the temporal registry or original arguments.
 Arm();MarkMain(&early);g_d82_temporal_tracking=false;
 auto native=BeforeSubmit(&queueA,1,e);CHECK(!native.temporal);CHECK(native.nativeMain);
 CHECK(AfterSubmit(native));CHECK(!AfterSubmit(native));
 auto* nativeTag=V6612FindLiveLocked(&early);const auto nativeGen=nativeTag->temporal_recording.generation;
 CHECK(DeviceHook::DetourResetCommandListD82(&early,nullptr,nullptr)==17);
 CHECK(nativeTag->temporal_recording.generation==nativeGen);CHECK(nativeTag->main_barrier_mask==2);
 nativeTag->temporal_recording.BindMain(reinterpret_cast<std::uintptr_t>(&mainA));
 DeviceHook::DetourClearCommandListStateD82(&early,nullptr);
 CHECK(nativeTag->temporal_recording.boundOutput==reinterpret_cast<std::uintptr_t>(&mainA));
 // Non-main binds invalidate existing state but do not exhaust the bounded map.
 Arm();CHECK(g_v6612_live_count==0);V6612BindTemporalOutputD82(&other,0);CHECK(g_v6612_live_count==0);
 // Fresh native owner/scheduler qualification is the extracted production
 // function, including no-owner direct path and fail-closed memory reads.
 EyeFlow flow{0x100000};void* context=reinterpret_cast<void*>(0x200000);
 model::values={{0x100018,0x200000},{0x2002C0,0}};
 CHECK(DirectFinalCompositionD82(flow,context));
 CHECK(!DirectFinalCompositionD82(flow,nullptr));
 CHECK(!DirectFinalCompositionD82(flow,reinterpret_cast<void*>(0x300000)));
 model::values[0x2002C0]=0x100000;
 CHECK(!DirectFinalCompositionD82(flow,context));
 model::values[0x1001A8]=0x100000;model::values[0x1001A0]=0;model::values[0x1018F4]=0;
 CHECK(DirectFinalCompositionD82(flow,context));
 model::values[0x1001A0]=1;CHECK(DirectFinalCompositionD82(flow,context));
 model::values[0x1018F4]=1;CHECK(!DirectFinalCompositionD82(flow,context));
 model::values[0x1001A0]=0;CHECK(DirectFinalCompositionD82(flow,context));
 model::values[0x1001A8]=0x300000;CHECK(!DirectFinalCompositionD82(flow,context));
 model::values[0x1001A8]=0x100000;model::values[0x2002C0]=0x300000;CHECK(!DirectFinalCompositionD82(flow,context));
 std::printf("D82 ACTUAL QUEUE/RESET ADAPTER: %u checks, %u failures\n",checks,failures);
 return failures?1:0;
}
'''


def check_wiring(source: str, eye: str, sig: str) -> list[str]:
    checks = []
    execute = braced(source, "void __stdcall DeviceHook::DetourExecuteCommandLists(")
    assert execute.index("V6615IncomingRightCompositionD82") < execute.index("s_original_execute_command_lists(queue_this")
    assert execute.index("s_original_execute_command_lists(queue_this") < execute.index("V6612ArchiveSubmitBatch(") < execute.index("ClaimRightCopy(")
    assert execute.index("BridgeCaptureCurrentMainTargetToLeft(0)") < execute.index("s_original_execute_command_lists(queue_this")
    checks.append("LEFT remains before original first-RIGHT submission; temporal stamp snapshot precedes submit, claim follows submit/archive")
    for name in ["DetourDrawInstanced", "DetourDrawIndexedInstanced"]:
        body = braced(source, "void __stdcall DeviceHook::" + name + "(")
        assert body.count("V6612RecordTemporalDrawD82(") == 2
        for segment in body.split("V6612RecordTemporalDrawD82(")[:-1]:
            assert "s_original_draw_" in segment
    checks.append("both actual Draw detours record after original, including performance-quiet branches")
    compose = braced(eye, "void __fastcall ComposeCallDetour(")
    for token in ["caller==g_wh+0x864659", "t_d82_final_compose_flow==f", "f->designStage==4&&eye==1", "in.valid&&out.valid", "DirectFinalCompositionD82(*f,context)", "CurrentUpscalerView(view)", "view.pipeline==f->pipeline", "BeginTemporalCompositionD82(view.epoch,view.frame)", "if(temporalScoped)stereo_design_capture::EndTemporalCompositionD82()"]:
        assert token in compose, token
    assert "{0x864654,5,{0xE8,0x13,0xF4,0xCD,0x01}}" in sig
    checks.append("native scope requires exact caller, FinalCompose owner, validated RIGHT TLS and textures; finally closes; exact call signature exists")
    installer = braced(source, "bool DeviceHook::InstallCommandListResetHookD82(")
    assert "kResetIndex=10" in installer
    assert "kClearStateIndex=11" in installer
    assert installer.index("s_original_reset_command_list_d82.store(original") < installer.index("s_hook_reset_command_list_d82->enable()")
    assert "s_hook_reset_command_list_d82.reset()" not in source
    assert "s_original_reset_command_list_d82.store(nullptr" not in source
    assert "s_hook_clear_command_list_state_d82.reset()" not in source
    assert installer.index("s_original_clear_command_list_state_d82.store(clearOriginal") < installer.index("s_hook_reset_command_list_d82->enable()")
    checks.append("Reset slot10/ClearState slot11 originals published before enable; hooks/originals retained on failure and uninstall")
    return checks


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--native-dll", type=Path)
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    source = (args.source / "src/device_hook.cpp").read_text()
    eye = (args.source / "src/native_stereo_eye_control_v6604.inc").read_text()
    sig = (args.source / "src/native_stereo_eye_signatures_v6604.inc").read_text()
    report = {"wiring": check_wiring(source, eye, sig), "runs": [], "negative_controls": [], "limits": "CPU ownership/ABI/static-image checks only; no Windows module link, vendor GPU output or headset validation"}
    if args.native_dll:
        import sys
        sys.path.insert(0, str(args.source / "tools"))
        from native_research112 import PE
        pe = PE(str(args.native_dll))
        for rva, size, payload in re.findall(r'\{(0x[0-9A-F]+),(\d+),\{([^}]+)\}\}, // D82', sig):
            expected = bytes(int(value, 16) for value in payload.split(","))
            assert len(expected) == int(size)
            assert pe.read(int(rva, 16), int(size)) == expected
        report["native_callsite"] = {"rva": "0x864654", "target": "0x2543A6C", "return": "0x864659", "sha256": hashlib.sha256(args.native_dll.read_bytes()).hexdigest()}
    generated = fixture(source, eye)
    (args.output / "actual_queue_adapter.cpp").write_text(generated)

    def run(label: str, file: Path, flags: list[str], expected: bool = True, include: Path | None = None) -> None:
        exe = args.output / label
        command = ["g++", "-std=c++20", "-Wall", "-Wextra", "-Werror", "-pthread", *flags,
                   "-I" + str(include or args.source / "src"), str(file), "-o", str(exe)]
        compiled = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (args.output / (label + ".compile.log")).write_text(compiled.stdout)
        if compiled.returncode: raise RuntimeError(label + " compile failed: " + compiled.stdout[:3000])
        env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0")
        result = subprocess.run([str(exe)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env)
        (args.output / (label + ".run.log")).write_text(result.stdout)
        assert (result.returncode == 0) == expected, (label, result.returncode, result.stdout)
        row = {"name": label, "exit_code": result.returncode, "output": result.stdout.strip()}
        report["runs" if expected else "negative_controls"].append(row)
        print(label, "PASS" if expected else "REJECTED", result.stdout.strip().splitlines()[-1])

    for label, file in [("policy", args.source / "tests/temporal_compose_d82_test.cpp"), ("actual_adapter", args.output / "actual_queue_adapter.cpp")]:
        for variant, flags in [("debug", ["-O0", "-g"]), ("optimized", ["-O2"]), ("sanitized", ["-O1", "-g", "-fsanitize=address,undefined", "-fno-omit-frame-pointer"])]:
            run(label + "_" + variant, file, flags)
    # Mutate the actual production adapter/decision, not a test's fake model.
    mutants = {
        "no_reset_invalidation": source.replace("g_v6612_live_lists[i].temporal_recording.Discard();break;", "break;"),
        "generic_barrier_claim": source.replace("ClaimRightCopy(g_v6615_right_copy_queued,d82_temporal,", "ClaimRightCopy(g_v6615_right_copy_queued,false,"),
        "no_sdk_receipt": source.replace("native_upscaler_stereo_d82::PairReady(d82_composition.epoch,d82_composition.frame);", "true;"),
        "no_clearstate_binding_reset": source.replace("    V6612ClearTemporalBindingsD82(list);", "    (void)list;"),
        "late_main_draw_keeps_stamp": source.replace("if(recording.ObserveUnscopedDraw(reinterpret_cast<std::uintptr_t>(list),primitives,instances))", "if(false&&recording.ObserveUnscopedDraw(reinterpret_cast<std::uintptr_t>(list),primitives,instances))"),
    }
    for name, mutant in mutants.items():
        assert mutant != source, name
        path = args.output / (name + ".cpp");path.write_text(fixture(mutant, eye))
        run(name, path, ["-O2", "-Wno-unused-function"], expected=False)
    header = (args.source / "src/temporal_compose_publication_d82.h").read_text()
    mutant_header = header.replace("composed.generation!=generation||", "")
    assert mutant_header != header
    mutant_include = args.output / "no_generation_match_include"
    mutant_include.mkdir(exist_ok=True)
    (mutant_include / "temporal_compose_publication_d82.h").write_text(mutant_header)
    run("no_generation_match", args.source / "tests/temporal_compose_d82_test.cpp", ["-O2"],
        expected=False, include=mutant_include)
    report["source_sha256"] = {str(p.relative_to(args.source)): hashlib.sha256(p.read_bytes()).hexdigest()
        for p in [args.source / "src/device_hook.cpp", args.source / "src/temporal_compose_publication_d82.h", args.source / "src/native_stereo_eye_control_v6604.inc", args.source / "src/native_stereo_eye_signatures_v6604.inc"]}
    (args.output / "RESULT.json").write_text(json.dumps(report, indent=2) + "\n")


if __name__ == "__main__":
    main()
