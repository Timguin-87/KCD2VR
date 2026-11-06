#!/usr/bin/env python3
"""Run every actual XrInput method with explicit OpenXR/Win32 host doubles.

Default: compare current source to the supplied R104 Git baseline. Exported
archives without Git can run --skip-baseline or use --baseline-source DIR.
There is no native runtime, OpenXR SDK ABI, D3D, game, or HMD claim here.
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
FILES = ["src/subsystems/hud_ui_controls/runtime/xr_input.h",
         "src/subsystems/hud_ui_controls/runtime/xr_input.cpp", "src/camera_hook.h",
         "src/core/haptics.h", "src/core/snapshot_mailbox.h",
         "src/diagnostics/capture168.h", "src/vrik/claude_motion_d69.h"]
p = argparse.ArgumentParser(description=__doc__)
p.add_argument("--skip-baseline", action="store_true")
p.add_argument("--baseline-source", type=Path)
p.add_argument("--sanitize", action="store_true")
p.add_argument("--output", type=Path, default=ROOT.parent / "evidence/steamvr_input")
args = p.parse_args()
args.output.mkdir(parents=True, exist_ok=True)
compiler = shutil.which("clang++") or shutil.which("g++")
if not compiler:
    raise SystemExit("A C++17 compiler is required")

def read(name, baseline):
    if baseline and args.baseline_source:
        return (args.baseline_source / name).read_bytes()
    if baseline:
        return subprocess.check_output(["git", "show", f"{BASELINE}:{name}"], cwd=ROOT)
    return (ROOT / name).read_bytes()

def digest(data):
    return hashlib.sha256(data).hexdigest()

def run(name, baseline=False):
    raw = {path: read(path, baseline) for path in FILES}
    text = {path: data.decode().replace("\r\n", "\n") for path, data in raw.items()}
    if '"quick_access_r112.h"' in text[FILES[1]]:
        raw["src/quick_access_r112.h"] = read("src/quick_access_r112.h", baseline)
    header = text[FILES[0]]
    cpp = text[FILES[1]]
    camera = text[FILES[2]]
    pose = camera[camera.index("struct XrPose {"):camera.index("\n};", camera.index("struct XrPose {")) + 3]
    declarations = header[header.index("namespace kcd2vr {"):]
    implementation = cpp[cpp.index("namespace kcd2vr {"):]
    compiled = "namespace kcd2vr {\n" + pose + "\n}\n" + declarations + "\n" + implementation
    if "src/quick_access_r112.h" in raw:
        compiled = '#include <utility>\n#include "quick_access_r112.h"\n' + compiled
    fixture = ROOT / "tests/xr_input_r105_test.cpp"
    flags = ["-std=c++17", "-O2", "-Wall", "-Wextra", "-Wno-missing-field-initializers", "-pthread"]
    if args.sanitize:
        flags += ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
    with tempfile.TemporaryDirectory(prefix="xr-input-r105-") as tmp:
        tmp = Path(tmp)
        for path in [path for path in raw if path not in FILES[:3]]:
            out = tmp / path.removeprefix("src/")
            out.parent.mkdir(parents=True, exist_ok=True)
            out.write_bytes(raw[path])
        (tmp / "production_input_r105.inc").write_text(compiled)
        cmd = [compiler, *flags, "-DHAS_INVALIDATE_R105=" + str(int("void InvalidateFrameR105();" in header)),
               "-DHAS_SESSION_STOP_R105=" + str(int("void OnSessionStoppedR105();" in header)),
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
                   source_sha256={path: digest(data) for path, data in raw.items()},
                   compiled_production_sha256=digest(compiled.encode()),
                   fixture_sha256=digest(fixture.read_bytes()))
    (args.output / f"{name}_result.json").write_text(json.dumps(receipt, indent=2) + "\n")
    print(f"{name}: {receipt['checks']} checks, {receipt['failures']} failures")
    return receipt

current = run("current_sanitized" if args.sanitize else "current")
ok = current["returncode"] == 0 and current["failures"] == 0
if not args.skip_baseline:
    baseline = run("baseline_sanitized" if args.sanitize else "baseline", True)
    required = {"failed trigger clears old level", "failed squeeze clears old level",
                "failed stick clears both old axes", "failed right buttons clear old levels and edges",
                "failed left buttons clear old levels and edges", "focus loss neutralizes full current state",
                "failed sync neutralizes full current state"}
    ok &= baseline["returncode"] == 1 and required.issubset(baseline["failed_checks"])
else:
    print("baseline explicitly skipped; no original-source comparison claimed")
raise SystemExit(0 if ok else 1)
