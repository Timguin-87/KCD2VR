#!/usr/bin/env python3
"""Exercise literal XrInput production methods using controlled OpenXR/Win32 leaves.

Current checks work without Git. To reproduce the original R111 negative control,
pass --baseline-ref c2a696b (in its source checkout) or --baseline-source DIR.
These tests establish source behavior, not SDK ABI, native DLL, runtime or HMD proof.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
FILES = ["src/subsystems/hud_ui_controls/runtime/xr_input.h",
         "src/subsystems/hud_ui_controls/runtime/xr_input.cpp", "src/camera_hook.h",
         "src/core/haptics.h", "src/core/snapshot_mailbox.h",
         "src/diagnostics/capture168.h", "src/vrik/claude_motion_d69.h"]
HELPER = "src/quick_access_r112.h"
PAD_HEADER = "src/subsystems/hud_ui_controls/runtime/native_vr_pad_premap.h"
PAD_CPP = "src/subsystems/hud_ui_controls/runtime/native_vr_pad_premap.cpp"
TOUCH_HEADER = "src/subsystems/hud_ui_controls/runtime/touch_bindings.h"
TOUCH_CPP = "src/subsystems/hud_ui_controls/runtime/touch_bindings.cpp"
UTILITY_HEADER = "src/subsystems/throw_drop/hand_claim_t24.h"
LAYOUT_R118 = "src/controller_layout_r118.h"
EXTRA = [PAD_HEADER, PAD_CPP, TOUCH_HEADER, TOUCH_CPP, UTILITY_HEADER,
         "src/ui_back_hold135r2.h", "src/pause_chord58c2.h", "src/ui_back_escape_m1.h",
         "src/debug_trace.h", "src/subsystems/vrmap/runtime/held_map_m1.h",
         "src/subsystems/vrmap/runtime/native_map_open_h25.h"]
p = argparse.ArgumentParser(description=__doc__)
p.add_argument("--source", type=Path, default=ROOT)
p.add_argument("--baseline-ref")
p.add_argument("--baseline-source", type=Path)
p.add_argument("--fallback-negative-ref", help="Original TouchBindings consumer only, with current input; isolated negative")
p.add_argument("--optimization", choices=["0", "2"], default="2")
p.add_argument("--sanitize", action="store_true")
p.add_argument("--output", type=Path, default=ROOT.parent / "evidence/input/actual_input")
args = p.parse_args()
if args.baseline_ref and args.baseline_source:
    p.error("select either --baseline-ref or --baseline-source")
args.output.mkdir(parents=True, exist_ok=True)
compiler = shutil.which("clang++") or shutil.which("g++")
if not compiler:
    raise SystemExit("A C++17 compiler is required")
fixture = ROOT / "tests/xr_input_r112_test.cpp"
fixture_helper = (ROOT / HELPER).read_bytes()

def read(name, baseline):
    if baseline and args.baseline_ref:
        return subprocess.check_output(["git", "show", f"{args.baseline_ref}:{name}"], cwd=args.source)
    return ((args.baseline_source if baseline else args.source) / name).read_bytes()

def digest(data):
    return hashlib.sha256(data).hexdigest()

def run(name, baseline=False, overrides=None, override_ref=None):
    raw = {path: read(path, baseline) for path in FILES + EXTRA}
    if overrides:
        raw.update(overrides)
    text = {path: data.decode().replace("\r\n", "\n") for path, data in raw.items()}
    header, cpp, camera = (text[path] for path in FILES[:3])
    if '"quick_access_r112.h"' in cpp:
        raw[HELPER] = read(HELPER, baseline)
    # R118: the pad premap / TouchBindings read the controller layout table (header only, no leaves).
    layout_r118 = "controller_layout_r118" in text[PAD_CPP] or "controller_layout_r118" in text[TOUCH_CPP]
    if layout_r118:
        raw[LAYOUT_R118] = read(LAYOUT_R118, baseline)
    pose_start = camera.index("struct XrPose {")
    pose = camera[pose_start:camera.index("\n};", pose_start) + 3]
    declarations = header[header.index("namespace kcd2vr {"):]
    implementation = cpp[cpp.index("namespace kcd2vr {"):]
    compiled = ('#include "controller_layout_r118.h"\n' if layout_r118 else "") + "namespace kcd2vr {\n" + pose + "\n}\n" + declarations + "\n" + implementation
    # Compile complete downstream methods. Only UtilityGate from hand_claim is
    # reachable here; extract its literal definition, not native claim readers.
    utility = text[UTILITY_HEADER]
    start = utility.index("struct UtilityGate {")
    gate = utility[start:utility.index("\n};", start) + 3]
    compiled += '\n#include "ui_back_hold135r2.h"\nnamespace kcd2vr::hand_claim_t24 {\n' + gate + '\n}\n'
    compiled += text[PAD_HEADER][text[PAD_HEADER].index("namespace kcd2vr::native_vr_pad_premap {"):]
    compiled += text[PAD_CPP][text[PAD_CPP].index("namespace kcd2vr::native_vr_pad_premap {"):]
    compiled += '\n#include "pause_chord58c2.h"\n#include "ui_back_escape_m1.h"\n#include "debug_trace.h"\n'
    compiled += '#include "subsystems/vrmap/runtime/held_map_m1.h"\n'
    compiled += text[TOUCH_HEADER][text[TOUCH_HEADER].index("namespace kcd2vr {"):]
    compiled += text[TOUCH_CPP][text[TOUCH_CPP].index("namespace kcd2vr {"):]
    flags = ["-std=c++17", "-O" + args.optimization, "-Wall", "-Wextra", "-Wno-missing-field-initializers", "-pthread"]
    if args.sanitize:
        flags += ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
    with tempfile.TemporaryDirectory(prefix="xr-input-r112-") as temp:
        tmp = Path(temp)
        for path, data in raw.items():
            if path in FILES[:3]:
                continue
            target = tmp / path.removeprefix("src/")
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_bytes(data)
        if HELPER not in raw:
            # Test-side mode control only. Original R111 never references it;
            # its literal production bodies remain unchanged for the negative.
            (tmp / "quick_access_r112.h").write_bytes(fixture_helper)
        (tmp / "production_input_r112.inc").write_text(compiled)
        cmd = [compiler, *flags, "-DHAS_FACE_TOUCH_R112=" + str(int("bool quick_access_touch_valid" in header)),
               "-I", str(tmp), str(fixture), "-o", str(tmp / "test")]
        build = subprocess.run(cmd, text=True, capture_output=True)
        (args.output / f"{name}_compile.txt").write_text(build.stdout + build.stderr)
        if build.returncode:
            raise RuntimeError(f"{name} compile failed; see {args.output / (name + '_compile.txt')}")
        env = os.environ.copy()
        if args.sanitize:
            env["ASAN_OPTIONS"] = "detect_leaks=0"
        result = subprocess.run([str(tmp / "test")], text=True, capture_output=True, env=env)
    (args.output / f"{name}_output.txt").write_text(result.stdout + result.stderr)
    receipt = json.loads(result.stdout.strip().splitlines()[-1])
    receipt.update(returncode=result.returncode, compiler=compiler, flags=flags,
                   baseline_ref=args.baseline_ref if baseline else None,
                   baseline_source=str(args.baseline_source) if baseline and args.baseline_source else None,
                   source_override_ref=override_ref,
                   source_override_files=sorted(overrides) if overrides else [],
                   source_sha256={path: digest(data) for path, data in raw.items()},
                   compiled_production_sha256=digest(compiled.encode()),
                   fixture_sha256=digest(fixture.read_bytes()),
                   runner_sha256=digest(Path(__file__).read_bytes()),
                   extra_fixture_configuration_helper_sha256=digest(fixture_helper) if HELPER not in raw else None,
                   limits=["Literal XrInput, native_vr_pad_premap and TouchBindings declarations/methods; actual UtilityGate/Back hold/map helpers; controlled OpenXR, clock, Win32 injection and diagnostic leaves.",
                           "No SDK ABI, full DLL, actual runtime profile, physical touch sensor, HMD or game claim.",
                           "Sanitizers disable leak detection in this host environment." if args.sanitize else "No sanitizer claim for this run."])
    (args.output / f"{name}_result.json").write_text(json.dumps(receipt, indent=2) + "\n")
    print(f"{name}: {receipt['checks']} checks, {receipt['failures']} failures")
    return receipt

suffix = "_sanitized" if args.sanitize else "_O" + args.optimization
current = run("current" + suffix)
ok = current["returncode"] == 0 and current["failures"] == 0
records = {"current": current}
if args.baseline_ref or args.baseline_source:
    baseline = run("baseline" + suffix, True)
    required = {"face-touch mode drives existing modifier while thumbrest is released",
                "face mode queries only its selected optional action",
                "released selected face sensors do not fall back to thumbrest",
                "failed profile query is unknown rather than claimed hardware loss",
                "successful null profile is distinguished without hardware diagnosis"}
    matched = required.issubset(baseline["failed_checks"])
    ok &= baseline["returncode"] == 1 and matched
    records.update(baseline=baseline, expected_baseline_failures=sorted(required), expected_baseline_failures_observed=matched)
else:
    print("original-source comparison not requested; current checks need no Git history")
if args.fallback_negative_ref:
    old_consumer = subprocess.check_output(["git", "show", f"{args.fallback_negative_ref}:{TOUCH_CPP}"], cwd=args.source)
    negative = run("original_fallback" + suffix, overrides={TOUCH_CPP: old_consumer}, override_ref=args.fallback_negative_ref)
    expected = {"actual fallback B release preserves eligible Quick Speak while face touch remains"}
    isolated = negative["returncode"] == 1 and set(negative["failed_checks"]) == expected
    ok &= isolated
    records.update(original_fallback_consumer_negative=negative, isolated_fallback_negative_observed=isolated)
records.update(passed=bool(ok), command=" ".join(__import__("sys").argv))
(args.output / "RESULT.json").write_text(json.dumps(records, indent=2) + "\n")
raise SystemExit(0 if ok else 1)
