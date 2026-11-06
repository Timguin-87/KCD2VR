#!/usr/bin/env python3
"""Exercise the exact camera-reader boundary under deterministic host contention.

--source can point at an immutable earlier source tree for a before/after run.
Only generated extraction, host executables and evidence are written to --out.
Native packet/world reads and diagnostics are synthetic leaves; no game or XR
runtime is loaded. --sanitize enables ASan+UBSan on a compatible GCC/Clang host.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

from source_text import read_source

HERE = Path(__file__).resolve().parents[1]


def extract(root):
    expanded = read_source(root / "src/frik_pose_probe.cpp", root=root)
    context = (root / "src/vrik/tool_attachment59c10.inl").read_text()
    parts = [context[context.index("static const char* WorkstationContextRefusal19c4r2("):
                     context.index("static const char* WorkstationToolReason19c4r2(")]]
    for start, end in (
        ("static bool ReadCommittedPacketFor6695(", "static bool ReadF12HandDeltaV6557("),
        ("void SetSmoothBody51(", "void SetHeadRaiseMax59c9("),
        ("static kcd2vr::core::SnapshotMailbox<kcd2vr::camera_hold_e1::Sample> g_camera_hold_e1;", "bool MotionOwnedCamera47("),
    ):
        begin = expanded.index(start)
        parts.append(expanded[begin:expanded.index(end, begin)])
    return "\n".join(parts)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=HERE)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--cxx", default=os.environ.get("CXX", "g++"))
    parser.add_argument("--sanitize", action="store_true")
    args = parser.parse_args()
    root, out = args.source.resolve(), args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    body = extract(root)
    extraction = out / "generated_bow_camera_d37b.inc"
    extraction.write_text(body)
    fixture = HERE / "tests/bow_camera_d37b_test.cpp"
    binary = out / ("bow_camera_d37b_test.exe" if os.name == "nt" else "bow_camera_d37b_test")
    command = [args.cxx, "-std=c++23", "-O1", "-g", "-pthread"]
    if args.sanitize:
        command += ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
    command += ["-I" + str(out), "-I" + str(root / "tests/shims"), "-I" + str(root / "src"),
                str(fixture), "-o", str(binary)]
    built = subprocess.run(command, text=True, capture_output=True)
    (out / "compile.log").write_text(built.stdout + built.stderr)
    result = {
        "source_root": str(root), "extraction_sha256": hashlib.sha256(body.encode()).hexdigest(),
        "fixture_sha256": hashlib.sha256(fixture.read_bytes()).hexdigest(), "compile_command": command,
        "compile_returncode": built.returncode,
        "native_leaves": "Synthetic packet/world reads and diagnostics; two real host threads force contention",
        "native_game_headset": "NOT_EXECUTED", "sanitizers": "ASan+UBSan" if args.sanitize else "none",
    }
    if built.returncode == 0:
        binary.chmod(binary.stat().st_mode | 0o700)
        env = os.environ.copy()
        if args.sanitize:
            # LeakSanitizer cannot operate inside ptraced execution containers;
            # this fixture uses no manually allocated memory. ASan/UBSan remain on.
            env["ASAN_OPTIONS"] = "detect_leaks=0"
            result["leak_sanitizer"] = "disabled for ptrace-compatible host execution"
        run = subprocess.run([str(binary)], text=True, capture_output=True, env=env, timeout=30)
        (out / "run.log").write_text(run.stdout + run.stderr)
        result["run_returncode"] = run.returncode
        result["summary"] = run.stdout.splitlines()[-1:]
        print(run.stdout, end="")
        if run.stderr:
            print(run.stderr, end="")
    else:
        print(built.stderr, end="")
    (out / "results.json").write_text(json.dumps(result, indent=2) + "\n")
    return result.get("run_returncode", built.returncode)


if __name__ == "__main__":
    raise SystemExit(main())
