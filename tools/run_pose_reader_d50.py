#!/usr/bin/env python3
"""Reproduce the completed-body reader fix using extracted production source."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys

p = argparse.ArgumentParser()
p.add_argument("--out", type=Path, required=True)
p.add_argument("--baseline-source", type=Path)
a = p.parse_args()
root = Path(__file__).resolve().parents[1]
out = a.out.resolve()
out.mkdir(parents=True, exist_ok=True)
cxx = os.environ.get("CXX", "g++")
cases = [("current", root, False, 0), ("sanitized", root, True, 0)]
if a.baseline_source:
    cases.insert(0, ("baseline_d49", a.baseline_source.resolve(), False, 1))
results = []

for name, source, sanitized, expected in cases:
    generated = out / (name + "_generated")
    extraction = [sys.executable, str(root / "tools/extract_pose_reader_d50.py"),
                  "--source", str(source), "--out", str(generated)]
    extracted = subprocess.run(extraction, cwd=root, capture_output=True, text=True)
    (out / (name + "_extract.log")).write_text(extracted.stdout + extracted.stderr)
    binary = out / name
    cmd = [cxx, "-std=c++23", "-O0", "-g0", "-pthread", "-I" + str(generated),
           "-I" + str(source / "src")]
    if sanitized:
        cmd += ["-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-fno-pie", "-no-pie"]
    cmd += [str(root / "tests/pose_reader_d50_test.cpp"), "-o", str(binary)]
    row = {"name": name, "source": str(source), "extraction": extraction,
           "extraction_exit": extracted.returncode, "compile": cmd,
           "compile_exit": None, "run_exit": None, "expected_exit": expected, "passed": False}
    if extracted.returncode == 0:
        compiled = subprocess.run(cmd, cwd=root, capture_output=True, text=True)
        row["compile_exit"] = compiled.returncode
        (out / (name + "_compile.log")).write_text(compiled.stdout + compiled.stderr)
        if compiled.returncode == 0:
            env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0", UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=1")
            ran = subprocess.run([str(binary)], cwd=root, capture_output=True, text=True, env=env)
            row["run_exit"] = ran.returncode
            (out / (name + "_run.log")).write_text(ran.stdout + ran.stderr)
            row["passed"] = ran.returncode == expected
            row["output"] = ran.stdout
            if expected:
                row["passed"] = row["passed"] and "19/30 checks PASS" in ran.stdout and ran.stdout.count("FAIL:") == 11
            else:
                row["passed"] = row["passed"] and "30/30 checks PASS" in ran.stdout
    if binary.exists():
        binary.unlink()
    results.append(row)
    print(("PASS " if row["passed"] else "FAIL ") + name, flush=True)

paths = ["src/frik_pose_probe.cpp", "src/core/snapshot_mailbox.h", "src/core/native_grasp13.h",
         "src/vrik/pose_math.h", "tests/pose_reader_d50_test.cpp", "tools/extract_pose_reader_d50.py"]
report = {"compiler": subprocess.run([cxx, "--version"], capture_output=True, text=True).stdout.splitlines()[0],
          "files": [{"path": path, "sha256": hashlib.sha256((root / path).read_bytes()).hexdigest()} for path in paths],
          "results": results, "native_execution": False,
          "limits": "Actual reader function, SnapshotMailbox, Stamp and acquisition policy. Minimal body payload; native context and deterministic changes during mailbox copy are explicit fixture leaves. No game, render/GPU or headset execution."}
(out / "RESULTS.json").write_text(json.dumps(report, indent=2) + "\n")
sys.exit(0 if all(row["passed"] for row in results) else 1)
