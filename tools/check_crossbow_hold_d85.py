#!/usr/bin/env python3
"""D85 xbow: paused first-pose / pose-join hold of the actual XR34 crossbow owner.

Builds tests/crossbow_hold_d85_test.cpp on top of the D82 production bridge:
the native clock adapter, clock Client, passenger consumer and completed-source
producer are the current production code (extracted unchanged, one Windows
__finally replaced by host scope cleanup exactly as check_crossbow_d82.py does).
Retail memory, native events, tracked poses and the string/lever pose producer
are explicit fixture leaves. No game runs.

Negative controls restore ONE old 80 ms window each and must fail behaviourally.
"""
from pathlib import Path
import argparse
import json
import os
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
HEADER = "subsystems/ranged_reload/crossbow_native_xr34.h"


def function(text, signature):
    start = text.index(signature)
    end = text.index("{", start) + 1
    depth = 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    return text[start:end] + "\n"


def overlay(out, header_text):
    """Symlinked copy of src with one replaced header (keeps #pragma once sane)."""
    src = out / "src"
    if src.exists():
        shutil.rmtree(src)
    src.mkdir(parents=True)
    target = src / HEADER
    parts = Path(HEADER).parts
    for entry in (ROOT / "src").iterdir():
        if entry.name != parts[0]:
            (src / entry.name).symlink_to(entry)
    level = ROOT / "src"
    made = src
    for part in parts[:-1]:
        level = level / part
        made = made / part
        made.mkdir(exist_ok=True)
        for entry in level.iterdir():
            if entry.name not in parts:
                (made / entry.name).symlink_to(entry)
    target.write_bytes(header_text)
    return src


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--compiler", default="g++")
    args = parser.parse_args()
    out = args.out.resolve()
    assert out != ROOT and ROOT not in out.parents
    out.mkdir(parents=True, exist_ok=True)
    adapter_path = ROOT / "src/subsystems/ranged_reload/crossbow_native_xr34.inl"
    passenger_path = adapter_path.with_name("reload_passenger_xr34.inl")
    ranged_path = adapter_path.with_name("crossbow_rifle_request_h21.inl")
    adapter = adapter_path.read_text()
    start = adapter.index("    __try{started=g_crossbow_xr34.startNewAfterRestore(")
    end_text = "__finally{(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}"
    end = adapter.index(end_text, start) + len(end_text)
    body = adapter[start:end]
    assert body.count("__try") == 1 and body.count("__finally") == 1
    host = body.replace("    __try{", "    {struct HostFinishD85 {ma::Token& token;bool& started;~HostFinishD85(){(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}} finish{token,started};", 1)
    host = host.replace("}\n    " + end_text, "}")
    assert "__finally" not in host
    adapter = adapter[:start] + host + adapter[end:]
    passenger = function(passenger_path.read_text(), "static bool ReloadPassengerCurrentXR34(")
    completed = function(ranged_path.read_text(), "static bool RangedRawIntentD78(")
    completed += function(passenger_path.read_text(), "static xcXR34::Source CompletedReloadPoseSourceXR34(")
    header = (ROOT / "src" / HEADER).read_bytes()
    runs = []

    def run(name, header_text=header, flags=("-O0", "-g"), negative=False):
        directory = out / name
        directory.mkdir(exist_ok=True)
        src = overlay(directory, header_text)
        fix = lambda value: value.replace('"../src/', '"' + src.as_posix() + '/')
        (directory / "native_adapter_host.inc").write_text(adapter)
        (directory / "passenger_consumer_d82_actual.inc").write_text(passenger)
        (directory / "completed_source_d82_actual.inc").write_text(completed)
        fixture = fix((ROOT / "tests/crossbow_native_xr34_test.cpp").read_text())
        fixture = fixture.replace('"' + (src / "subsystems/ranged_reload/crossbow_native_xr34.inl").as_posix() + '"', '"native_adapter_host.inc"')
        assert '"native_adapter_host.inc"' in fixture
        (directory / "crossbow_native_xr34_test.cpp").write_text(fixture)
        (directory / "crossbow_scope_d82_test.cpp").write_text(fix((ROOT / "tests/crossbow_scope_d82_test.cpp").read_text()))
        (directory / "crossbow_hold_d85_test.cpp").write_text(fix((ROOT / "tests/crossbow_hold_d85_test.cpp").read_text()))
        binary = directory / name
        command = [args.compiler, "-std=c++20", "-pthread", *flags, "-I", str(src), "-I", str(ROOT / "tests/syntax_shims"),
                   "-I", str(directory), str(directory / "crossbow_hold_d85_test.cpp"), "-o", str(binary)]
        built = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (directory / "build.log").write_text(built.stdout)
        if built.returncode:
            raise RuntimeError(name + " compile failed\n" + built.stdout[-4000:])
        env = os.environ.copy()
        env["ASAN_OPTIONS"] = "detect_leaks=0"
        ran = subprocess.run([str(binary)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env)
        (directory / "run.log").write_text(ran.stdout)
        passed = (ran.returncode != 0 and "FAIL_D85" in ran.stdout) if negative else ran.returncode == 0
        runs.append({"name": name, "exit_code": ran.returncode, "expected_failure": negative,
                     "passed": passed, "output": ran.stdout.strip()[-600:]})
        print(name + ": " + ran.stdout.strip().splitlines()[-1] if ran.stdout.strip() else name + ": (no output)", flush=True)
        if not passed:
            raise RuntimeError(name + " unexpected result")

    run("hold_d85")
    run("hold_d85_optimized", flags=("-O2",))
    run("hold_d85_asan_ubsan", flags=("-O1", "-g", "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-no-pie"))
    text = header.decode()
    old = "now<=UINT64_MAX-kPausedJoinMsD85?now+kPausedJoinMsD85:UINT64_MAX"
    assert text.count(old) == 1
    run("old_first_pose_80ms", text.replace(old, "now<=UINT64_MAX-80?now+80:UINT64_MAX").encode(), negative=True)
    old = "if(now<mechanismWaitD78_||now-mechanismWaitD78_>kPausedJoinMsD85){\r\n"
    assert text.count(old) == 1
    run("old_string_join_80ms", text.replace(old, "if(now<mechanismWaitD78_||now-mechanismWaitD78_>80){\r\n").encode(), negative=True)
    result = {"passed": all(r["passed"] for r in runs), "runs": runs,
              "limits": ["Game/headset and Windows build NOT_RUN.",
                         "Native memory, events, tracked poses, string/lever pose producer and scheduler are fixture leaves.",
                         "Proves the hold/fallback contract of the production Client, not that a live pose job arrives within 500 ms."]}
    (out / "RESULT.json").write_text(json.dumps(result, indent=2) + "\n")
    shutil.rmtree(out / "hold_d85" / "src", ignore_errors=True)


if __name__ == "__main__":
    main()
