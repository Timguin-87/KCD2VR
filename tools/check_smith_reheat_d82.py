#!/usr/bin/env python3
"""Compile the actual station-pad regression; optionally require old code to fail."""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--baseline", type=Path, help="Unmodified D81R1 source root for a negative control")
    parser.add_argument("--output", type=Path, help="Keep compiler/runtime evidence in this directory")
    parser.add_argument("--compiler", default="g++")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    temporary = None
    if args.output:
        out = args.output.resolve()
        out.mkdir(parents=True, exist_ok=True)
    else:
        temporary = tempfile.TemporaryDirectory(prefix="kcd2vr-smith-d82-")
        out = Path(temporary.name)
    cases = [("d82", root, False)]
    if args.baseline:
        cases.append(("baseline_negative", args.baseline.resolve(), True))
    results = []
    for label, include_root, expected_failure in cases:
        exe = out / label
        command = [args.compiler, "-std=c++20", "-O2", "-I", str(include_root / "src"),
                   str(root / "tests/smith_reheat_d82_test.cpp"), "-o", str(exe)]
        compiled = subprocess.run(command, capture_output=True, text=True)
        (out / (label + "_compile.log")).write_text(compiled.stdout + compiled.stderr)
        result = {"case": label, "command": command, "compile_rc": compiled.returncode}
        if not compiled.returncode:
            executed = subprocess.run([str(exe)], capture_output=True, text=True)
            result.update(run_rc=executed.returncode, output=executed.stdout + executed.stderr)
            (out / (label + ".log")).write_text(result["output"])
            result["pass"] = (executed.returncode != 0 and "continuous post-reheat lift reaches native RT without a stop" in result["output"]) if expected_failure else executed.returncode == 0
        else:
            result["pass"] = False  # A compiler failure cannot satisfy a negative control.
        results.append(result)
    (out / "results.json").write_text(json.dumps(results, indent=2) + "\n")
    for result in results:
        print(result["case"], "PASS" if result["pass"] else "FAIL", result.get("output", "compile failure").strip())
    success = all(r["pass"] for r in results)
    if temporary:
        temporary.cleanup()
    return 0 if success else 1


if __name__ == "__main__":
    raise SystemExit(main())
