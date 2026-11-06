#!/usr/bin/env python3
"""Check the actual reload admission -> passenger -> first completed source.

Retail native memory, engine effects, tracked pose and owner-lock contention
are explicit fixture leaves. The native clock adapter, passenger consumer,
source producer and clock client are current production code. No game runs.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]


def function(text, signature):
    start = text.index(signature)
    end = text.index("{", start) + 1
    depth = 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    return text[start:end] + "\n"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--compiler", default="g++")
    parser.add_argument("--skip-legacy", action="store_true")
    args = parser.parse_args()
    out = args.out.resolve()
    assert out != ROOT and ROOT not in out.parents
    out.mkdir(parents=True, exist_ok=True)
    if not args.skip_legacy:
        subprocess.run([sys.executable, str(ROOT / "tools/check_crossbow_d81.py"),
                        "--out", str(out / "legacy"), "--compiler", args.compiler], check=True)
    adapter_path = ROOT / "src/subsystems/ranged_reload/crossbow_native_xr34.inl"
    passenger_path = adapter_path.with_name("reload_passenger_xr34.inl")
    ranged_path = adapter_path.with_name("crossbow_rifle_request_h21.inl")
    adapter = adapter_path.read_text()
    start = adapter.index("    __try{started=g_crossbow_xr34.startNewAfterRestore(")
    end_text = "__finally{(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}"
    end = adapter.index(end_text, start) + len(end_text)
    body = adapter[start:end]
    assert body.count("__try") == 1 and body.count("__finally") == 1
    host = body.replace("    __try{", "    {struct HostFinishD82 {ma::Token& token;bool& started;~HostFinishD82(){(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}} finish{token,started};", 1)
    host = host.replace("}\n    " + end_text, "}")
    assert "__finally" not in host
    adapter = adapter[:start] + host + adapter[end:]
    passenger = function(passenger_path.read_text(), "static bool ReloadNativeClockCurrentD86(")
    passenger += function(passenger_path.read_text(), "static bool ReloadPassengerCurrentXR34(")
    passenger += function(passenger_path.read_text(), "static bool ReadReloadPassengerXR34(")
    passenger += function(passenger_path.read_text(), "static bool ReloadPoseWriterCurrentXR34(")
    completed = function(ranged_path.read_text(), "static bool RangedRawIntentD78(")
    completed += function(passenger_path.read_text(), "static xcXR34::Source CompletedReloadPoseSourceXR34(")
    test = (ROOT / "tests/crossbow_scope_d82_test.cpp").read_text()
    fixture = (ROOT / "tests/crossbow_native_xr34_test.cpp").read_text()
    for_path = lambda value: value.replace('"../src/', '"' + (ROOT / "src").as_posix() + '/')
    fixture, test = for_path(fixture), for_path(test)
    runs = []

    def run(name, adapter_code=adapter, passenger_code=passenger, flags=("-O0", "-g"), negative=False):
        directory = out / name
        directory.mkdir(exist_ok=True)
        (directory / "native_adapter_host.inc").write_text(adapter_code)
        (directory / "passenger_consumer_d82_actual.inc").write_text(passenger_code)
        (directory / "completed_source_d82_actual.inc").write_text(completed)
        (directory / "crossbow_scope_d82_test.cpp").write_text(test)
        (directory / "crossbow_native_xr34_test.cpp").write_text(fixture.replace('"' + str(adapter_path) + '"', '"native_adapter_host.inc"'))
        binary = directory / name
        command = [args.compiler, "-std=c++20", "-pthread", *flags,
                   "-I", str(ROOT / "src"), "-I", str(ROOT / "tests/syntax_shims"),
                   "-I", str(directory), str(directory / "crossbow_scope_d82_test.cpp"), "-o", str(binary)]
        built = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (directory / "build.log").write_text(built.stdout)
        if built.returncode:
            raise RuntimeError(name + " compile failed\n" + built.stdout)
        env = os.environ.copy()
        env["ASAN_OPTIONS"] = "detect_leaks=0"
        ran = subprocess.run([str(binary)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env)
        (directory / "run.log").write_text(ran.stdout)
        passed = (ran.returncode == 1 and "FAIL_D82" in ran.stdout) if negative else ran.returncode == 0
        runs.append({"name": name, "command": command, "exit_code": ran.returncode,
                     "expected_failure": negative, "passed": passed, "output": ran.stdout})
        print(name + ": " + ran.stdout, end="", flush=True)
        if not passed:
            raise RuntimeError(name + " unexpected result")

    run("production_bridge")
    run("production_bridge_optimized", flags=("-O2",))
    run("production_bridge_asan_ubsan", flags=("-O1", "-g", "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-no-pie"))
    # Each old defect is restored independently. These must fail the same
    # behavioral checks, not merely a string/source-style assertion.
    old = "!xr34::selectedScopeClockEntryD82(&XR34ReadBytes,character,layer,layers,c.id,entries,c.nativeTransitionD80,observed)"
    assert adapter.count(old) == 1
    run("old_clock_one_layer", adapter_code=adapter.replace(old, "(layers!=1||!xr34::selectedClockEntryD81(&XR34ReadBytes,character,layer,c.id,entries))"), negative=True)
    old = "!xcXR34::selectedScopeClockEntryD82(&RangedReadXR34,character,layer,layers,p.clip.id,entry,p.clip.nativeTransitionD80)"
    assert passenger.count(old) == 1
    run("old_passenger_one_layer", passenger_code=passenger.replace(old, "(layers!=1||!xcXR34::selectedClockEntryD81(&RangedReadXR34,character,layer,p.clip.id,entry))"), negative=True)
    old = "if(!infoRead||info.list!=c.list||info.animation!=c.id||segment!=c.segment)return previous;"
    assert adapter.count(old) == 1
    run("old_foreign_scan_publication", adapter_code=adapter.replace(old, old.replace("return previous;", "{XR34PublishNative();return previous;}")), negative=True)
    old = "if(!tx||!g_xr34_boundary_d72.controller){"
    assert adapter.count(old) == 1
    run("old_foreign_busy_revocation", adapter_code=adapter.replace(old, old + "xr34::RevokeNativeParent(reinterpret_cast<void*>(g_crossbow_xr34.status().owner.parent));"), negative=True)
    old = "if(XR34Read(reinterpret_cast<std::uintptr_t>(event)+0x1C,crc)&&!XR34NativeEventEffectD82(crc))return;"
    assert adapter.count(old) == 1
    run("old_inert_event_cancellation", adapter_code=adapter.replace(old, "(void)crc;"), negative=True)
    paths = [adapter_path.with_suffix(".h"), adapter_path, passenger_path, ranged_path,
             adapter_path.with_name("reload_diagnostics_d48.h"), adapter_path.with_name("crossbow_clock_entry_d87.h"),
             ROOT / "tests/crossbow_native_xr34_test.cpp", ROOT / "tests/crossbow_scope_d82_test.cpp", Path(__file__).resolve()]
    result = {"passed": all(r["passed"] for r in runs), "legacy_run": not args.skip_legacy, "runs": runs,
              "production_slice_sha256": {"passenger_consumer": hashlib.sha256(passenger.encode()).hexdigest(),
                                           "raw_intent_and_completed_source": hashlib.sha256(completed.encode()).hexdigest()},
              "source_sha256": {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},
              "limits": ["Game/headset and Windows build NOT_RUN; one Windows SEH cleanup uses host C++ scope cleanup.",
                         "Native memory, equipment/animation context, event effects, tracked pose and scheduler contention are fixture leaves.",
                         "No native inventory, rendered bolt/tool, hand-string contact, heavy lever geometry or actual pose-job scheduling validation.",
                         "The actual passenger/current-clock and first completed-source functions execute unchanged; full VRIK solver is outside this fixture."]}
    (out / "RESULT.json").write_text(json.dumps(result, indent=2) + "\n")


if __name__ == "__main__":
    main()
