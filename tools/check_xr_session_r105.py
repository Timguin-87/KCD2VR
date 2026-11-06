#!/usr/bin/env python3
"""Compile actual startup/lifecycle/reference blocks against controlled host leaves.

No copied lifecycle policy: the production functions and RunFrame boundary are
extracted verbatim. Resolution/GPU/frame-rendering code is outside this fixture.
Use --skip-baseline in exports without Git, or --baseline-source for an R104 tree.
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
BASELINE = "5cead364a424ac6abefd2d556e39ac61c9c6e907"
p = argparse.ArgumentParser(description=__doc__)
p.add_argument("--skip-baseline", action="store_true")
p.add_argument("--baseline-source", type=Path)
p.add_argument("--sanitize", action="store_true")
p.add_argument("--output", type=Path, default=ROOT.parent / "evidence/steamvr_input/session")
args = p.parse_args()
args.output.mkdir(parents=True, exist_ok=True)
compiler = shutil.which("clang++") or shutil.which("g++")
if not compiler:
    raise SystemExit("C++17 compiler required")

def source(path, baseline):
    if baseline and args.baseline_source:
        return (args.baseline_source / path).read_bytes()
    if baseline:
        return subprocess.check_output(["git", "show", f"{BASELINE}:{path}"], cwd=ROOT)
    return (ROOT / path).read_bytes()

def digest(data):
    return hashlib.sha256(data).hexdigest()

def run(label, baseline=False):
    raw = source("src/xr_manager.cpp", baseline)
    cpp = raw.decode().replace("\r\n", "\n")
    ensure = cpp[cpp.index("bool XrManager::EnsureInitialized("):cpp.index("bool XrManager::ClearD3D12Image(")].rstrip()
    signature = ensure.splitlines()[0].replace("XrManager::", "")
    has_helpers = "void XrManager::PollSessionEventsR105()" in cpp
    helpers = cpp[cpp.index("void XrManager::ResetUiAnchorsR105()"):cpp.index("bool XrManager::RunFrame(")] if has_helpers else ""
    frame_start = cpp.index("bool XrManager::RunFrame(")
    frame_end = cpp.index("    // V58 live XR-resolution apply.", frame_start)
    admission = cpp[frame_start:frame_end]
    if not has_helpers:
        event_start = cpp.index("    XrEventDataBuffer event{ XR_TYPE_EVENT_DATA_BUFFER };", frame_end)
        admission += cpp[event_start:cpp.index("    XrFrameState frame_state", event_start)]
    admission += "    return true; // Fixture boundary: frame work after lifecycle admission is not executed.\n}\n"
    ref_start = cpp.index("    const auto due=std::remove_if(pending_reference_changes125_")
    reference = cpp[ref_start:cpp.index("    // V49: derive the active headset cadence", ref_start)]
    fixture_path = ROOT / "tests/xr_session_r105_test.cpp"
    fixture = fixture_path.read_text()
    blocks = {"INIT_DECLARATION": signature, "ENSURE_INITIALIZED": ensure,
              "LIFECYCLE_HELPERS": helpers, "FRAME_ADMISSION": admission,
              "REFERENCE_BLOCK": reference}
    for key, value in blocks.items():
        fixture = fixture.replace("@" + key + "@", value)
    flags = ["-std=c++17", "-O2", "-Wall", "-Wextra", "-Wno-unused-parameter", "-pthread"]
    if args.sanitize:
        flags += ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
    with tempfile.TemporaryDirectory(prefix="xr-session-r105-") as tmp:
        tmp = Path(tmp)
        (tmp / "test.cpp").write_text(fixture)
        cmd = [compiler, *flags, "-DHAS_SESSION_R105=" + str(int(has_helpers)),
               "-DHAS_INIT_CANCEL_R105=" + str(int("keep_running" in signature)),
               str(tmp / "test.cpp"), "-o", str(tmp / "test")]
        build = subprocess.run(cmd, text=True, capture_output=True)
        (args.output / f"{label}_compile.txt").write_text(build.stdout + build.stderr)
        if build.returncode:
            raise RuntimeError(f"{label} compile failed; see {args.output / (label + '_compile.txt')}")
        env = os.environ.copy()
        if args.sanitize:
            env["ASAN_OPTIONS"] = "detect_leaks=0"
        result = subprocess.run([str(tmp / "test")], text=True, capture_output=True, env=env)
    (args.output / f"{label}_output.txt").write_text(result.stdout + result.stderr)
    receipt = json.loads(result.stdout.strip().splitlines()[-1])
    receipt.update(returncode=result.returncode, compiler=compiler, flags=flags,
                   source_sha256=digest(raw), fixture_sha256=digest(fixture_path.read_bytes()),
                   extracted_sha256={key: digest(value.encode()) for key, value in blocks.items()})
    (args.output / f"{label}_result.json").write_text(json.dumps(receipt, indent=2) + "\n")
    print(f"{label}: {receipt['checks']} checks, {receipt['failures']} failures")
    return receipt

current = run("current_sanitized" if args.sanitize else "current")
ok = current["returncode"] == 0 and current["failures"] == 0
if not args.skip_baseline:
    baseline = run("baseline_sanitized" if args.sanitize else "baseline", True)
    required = {"READY after 25 seconds starts without timeout latch",
                "STOPPING ends native session and retires local output receipts",
                "stopped session continues polling events", "READY resumes same stopped native session"}
    ok &= baseline["returncode"] == 1 and required.issubset(baseline["failed_checks"])
else:
    print("baseline explicitly skipped; no original-source comparison claimed")
raise SystemExit(0 if ok else 1)
