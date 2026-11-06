#!/usr/bin/env python3
"""D85 UPSCALERS: exercise the production XeSS runtime with explicit libxess leaves.

The negative controls mutate one production rule at a time. A failed compile
does not count as a rejected regression. No GPU, Windows ABI or headset claim.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess


def run(root: Path, out: Path, name: str, flags: list[str], cxx: str,
        mutation: tuple[str, str] | None = None) -> dict:
    directory = out / name
    directory.mkdir(parents=True, exist_ok=True)
    include = root / "src"
    source = root / "src/xess_stereo_runtime_d85.h"
    original = source.read_text()
    if mutation:
        old, new = mutation
        if original.count(old) != 1:
            raise ValueError(f"{name}: expected exactly one production mutation anchor")
        include = directory / "src"
        include.mkdir(exist_ok=True)
        (include / source.name).write_text(original.replace(old, new))
    executable = directory / "fixture"
    command = [cxx, "-std=c++20", "-pthread", "-Wall", "-Wextra", "-Werror", *flags,
               "-I" + str(include), str(root / "tests/xess_stereo_d85_test.cpp"),
               "-o", str(executable)]
    compiled = subprocess.run(command, text=True, capture_output=True)
    (directory / "compile.log").write_text(compiled.stdout + compiled.stderr)
    result = {"name": name, "negative_control": mutation is not None,
              "command": command, "compile_returncode": compiled.returncode,
              "passed": False}
    if compiled.returncode:
        return result
    try:
        environment = os.environ.copy()
        if any("-fsanitize=" in flag for flag in flags):
            # LeakSanitizer cannot inspect /proc task state in this sandbox.
            # Address and undefined-behavior instrumentation remain enabled.
            environment["ASAN_OPTIONS"] = "detect_leaks=0"
            result["ASAN_OPTIONS"] = "detect_leaks=0"
        executed = subprocess.run([str(executable)], text=True, capture_output=True,
                                  timeout=20, env=environment)
        (directory / "run.log").write_text(executed.stdout + executed.stderr)
        result.update(run_returncode=executed.returncode,
                      output=(executed.stdout + executed.stderr).strip(),
                      passed=executed.returncode == (1 if mutation else 0))
    except subprocess.TimeoutExpired:
        result["output"] = "TIMEOUT: not accepted as a successful negative control"
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--cxx", default="g++")
    args = parser.parse_args()
    root, out = args.source.resolve(), args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    jobs = []
    for name, flags in (("debug", ["-O0", "-g"]), ("optimized", ["-O2"]),
                        ("asan_ubsan", ["-O1", "-g", "-no-pie", "-fsanitize=address,undefined",
                                        "-fno-omit-frame-pointer"])):
        jobs.append(run(root, out, name, flags, args.cxx))
    mutations = {
        "shared_eye_history": ("const Context chosen=eye?s->right:s->native;", "const Context chosen=s->native;"),
        "missing_discontinuity_reset": ("execute.resetHistory=(params->resetHistory||discontinuity)?1u:0u;",
                                        "(void)discontinuity;execute.resetHistory=params->resetHistory;"),
        "publish_after_left_only": ("return flags==11u&&receiptGeneration==generation",
                                    "return (flags&1u)&&receiptGeneration==generation"),
        "wrong_frame_receipt": ("e==epoch&&f==frame", "e==epoch&&(f==frame||true)"),
        "queue_sdk_lock_cycle": ("const auto generation=routingGeneration_.load(std::memory_order_seq_cst);\n        for(const auto& p:published_)",
                                 "std::lock_guard lock(mutex_);const auto generation=routingGeneration_.load(std::memory_order_seq_cst);\n        for(const auto& p:published_)"),
        "right_without_left": ("if(view.eye==1&&!(receipt.mask&1u))return Refuse(view,s);", ""),
        "right_only_reset": ("else if(!(receipt.resetMask&1u)){s->dirty[0]=s->dirty[1]=true;return Refuse(view,s);}", ""),
        "missing_pair_continuity": ("if(eye==0){\n            bool broken", "if(eye==0&&false){\n            bool broken"),
        "duplicate_eye_advance": ("if(receipt.failed||(receipt.mask&(1u<<view.eye)))return Refuse(view,s);",
                                  "if(receipt.failed)return Refuse(view,s);"),
        "init_not_mirrored": ("const auto r=api_.init(s->right,params);", "const auto r=kOk;"),
        "velocity_not_mirrored": ("const auto r=api_.velocity(s->right,x,y);", "const auto r=kOk;"),
        "destroy_skips_twin": ("const auto r=api_.destroy(s->right);", "const auto r=kOk;"),
        "ready_before_init": ("found|=s.valid&&s.initialized&&s.native", "found|=s.valid&&s.native"),
    }
    for name, mutation in mutations.items():
        jobs.append(run(root, out, "negative_" + name, ["-O2"], args.cxx, mutation))
    paths = ["src/xess_stereo_runtime_d85.h", "src/native_xess_stereo_d85.cpp",
             "tests/xess_stereo_d85_test.cpp", "tools/check_xess_stereo_d85.py"]
    report = {"passed": all(j["passed"] for j in jobs), "jobs": jobs,
              "source_sha256": {p: hashlib.sha256((root / p).read_bytes()).hexdigest() for p in paths},
              "limits": ["Actual production XeSS runtime; libxess, GPU resources and view identity are fixture leaves.",
                         "The publication check verifies no SDK mutex wait, not a measured game performance budget.",
                         "LeakSanitizer is disabled because sandbox /proc task inspection is unavailable; ASan and UBSan remain enabled.",
                         "No Windows DLL compilation, real GPU execution, frame capture or headset validation."]}
    (out / "RESULT.json").write_text(json.dumps(report, indent=2) + "\n")
    for job in jobs:
        print(job["name"], "PASS" if job["passed"] else "FAIL", job.get("output", "compile failed"))
    return 0 if report["passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
