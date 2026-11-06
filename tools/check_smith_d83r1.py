#!/usr/bin/env python3
"""Exercise actual smith adapters and require identified older behaviors to fail.

Host C++ tests run production controller, final pad, floor and attachment code.
They do not emulate the game, prove native hit quality or replace a headset run.
"""
import argparse
import concurrent.futures
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--baseline-d82", type=Path)
    parser.add_argument("--claude", type=Path, help="Full D81R1 tree with original Claude D83 overlay")
    parser.add_argument("--output", type=Path)
    parser.add_argument("--compiler", default="g++")
    parser.add_argument("--jobs", type=int, default=3)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    temporary = None
    if args.output:
        out = args.output.resolve()
        out.mkdir(parents=True, exist_ok=True)
    else:
        temporary = tempfile.TemporaryDirectory(prefix="kcd2vr-smith-d83r1-")
        out = Path(temporary.name)
    # A failure counts as a negative control only after successful compilation
    # and the exact identified assertion. Compiler failures are always errors.
    cases = []
    def case(label, source, include_root=root, flags=("-O2",), mode=None, fails=None):
        cases.append((label, source, include_root, flags, mode, fails))
    transitions = "smith_transitions_d83r1_test.cpp"
    placement = "smith_lt_alignment_d83r1_test.cpp"
    for name, flags in (("debug", ("-O0", "-g")), ("release", ("-O2",)),
                        ("ubsan", ("-O1", "-g", "-fsanitize=undefined", "-fno-sanitize-recover=all"))):
        case("transitions_" + name, transitions, flags=flags)
        case("placement_" + name, placement, flags=flags)
    for name in ("smith_reheat_d82", "smith_native_height_d59r1", "smith_recovery_d67",
                 "smith_recovery_lifetime_d67r1", "smith_height_retirement_d62", "smith_fall_d81",
                 "smith_native_floor_d56", "smith_floor_owner_d62", "smith_assist_d80",
                 "smith_assist_input_d80", "smith_support_lifecycle_d56",
                 "smith_hammer_grip_d80", "smith_stroke_wiring_d57"):
        case(name, name + "_test.cpp")
    if args.baseline_d82:
        baseline = args.baseline_d82.resolve()
        case("d82_phase15_negative", transitions, baseline, mode="phase15",
             fails="phase15 preserves the current owned recovery lift")
        case("d82_lt_axis_negative", transitions, baseline, mode="lt",
             fails="held LT keeps actual native fore/aft workpiece input")
    if args.claude:
        claude = args.claude.resolve()
        case("claude_reheat_negative", transitions, claude, mode="reheat",
             fails="continuous post-reheat lift reaches native RT without a stop")
        case("claude_fall_clock_negative", transitions, claude, mode="fall",
             fails="clock, epoch, tool loss or disabled recovery cannot preserve fall rearming")
        case("claude_lt_placement_negative", placement, claude,
             fails="held LT visible tool stays at the current native tool translation")

    def execute(spec):
        label, filename, include_root, flags, mode, fails = spec
        exe = out / label
        command = [args.compiler, "-std=c++20", *flags, "-I", str(include_root / "src"),
                   "-I", str(root / "tests"), str(root / "tests" / filename), "-o", str(exe)]
        result = {"case": label, "source": filename, "include_root": str(include_root),
                  "command": command, "expected_failure": fails}
        try:
            compiled = subprocess.run(command, capture_output=True, text=True, timeout=90)
            result["compile_rc"] = compiled.returncode
            (out / (label + "_compile.log")).write_text(compiled.stdout + compiled.stderr)
            result["pass"] = False
            if compiled.returncode == 0:
                env = dict(os.environ, UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
                executed = subprocess.run([str(exe)] + ([mode] if mode else []),
                                          capture_output=True, text=True, timeout=30, env=env)
                result.update(run_rc=executed.returncode, output=executed.stdout + executed.stderr)
                (out / (label + ".log")).write_text(result["output"])
                result["pass"] = (executed.returncode != 0 and fails in result["output"]) if fails else executed.returncode == 0
        except subprocess.TimeoutExpired as error:
            result.update(pass_=False, error=str(error))
            result["pass"] = False
        finally:
            # Keep readable evidence, not platform-specific test executables.
            exe.unlink(missing_ok=True)
        return result

    with concurrent.futures.ThreadPoolExecutor(max_workers=max(1, args.jobs)) as pool:
        results = list(pool.map(execute, cases))
    evidence = {"scope": "Production host adapters; game/headset/native-hit outcomes NOT_RUN",
                "compiler": subprocess.check_output([args.compiler, "--version"], text=True).splitlines()[0],
                "cases": results, "pass": all(r["pass"] for r in results)}
    source_paths = sorted({"tests/" + case[1] for case in cases} | {
        "src/vrik/semi_smith46.h", "src/vrik/semi_workstations46.h",
        "src/vrik/smith_native_floor_d56.inl", "src/vrik/smith_tool36.h",
        "src/subsystems/smithing/assist_d80.h", "src/subsystems/smithing/native_floor_d56.h",
        "tools/check_smith_d83r1.py"})
    evidence["sha256"] = {path: hashlib.sha256((root / path).read_bytes()).hexdigest() for path in source_paths}
    (out / "RESULT.json").write_text(json.dumps(evidence, indent=2) + "\n")
    for result in results:
        print(result["case"], "PASS" if result["pass"] else "FAIL", result.get("output", "compile failure").strip())
    if temporary:
        temporary.cleanup()
    return 0 if evidence["pass"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
