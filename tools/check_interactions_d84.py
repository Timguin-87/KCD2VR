"""Exercise the actual D84 interaction consumers; native leaves are substitutes.

Extraction is repeated from production for every run. The same fixture must fail
against the previous consumer, so passing cannot come only from test doubles.
This runner makes no Windows/game/headset or native physics execution claim.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def extract(source):
    text = source.read_text(encoding="utf-8-sig")
    pairs = (
        ("struct StagedProp596", "static bool __fastcall DeferredPickup596"),
        ("static bool PropInstantContextD78(", "static bool BuildStagedPickup596("),
        ("static bool PropPickupHandD81(", "static void PropPublish5916C1("),
        ("static void StagedPropTick596()", '#include "instant_pickup_native_d72.inl"'),
    )
    parts = []
    for start, end in pairs:
        first = text.index(start)
        parts.append(text[first:text.index(end, first)])
    return "\n".join(parts).rstrip() + "\n"


def run_case(build, name, test, flags, include=None, expected_failure=False):
    exe = build / name
    command = ["g++", "-std=c++23", "-g0", "-pthread", "-I" + str(ROOT / "src"), *flags]
    if include:
        command.append('-DPROP_ADAPTER_D78_INC="' + str(include) + '"')
    command.extend([str(ROOT / "tests" / test), "-o", str(exe)])
    compiled = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    # This executor cannot enumerate /proc tasks for LeakSanitizer. Address and
    # undefined-behaviour instrumentation remain enabled; leak checking is not.
    environment = dict(os.environ, ASAN_OPTIONS="detect_leaks=0:halt_on_error=1", UBSAN_OPTIONS="halt_on_error=1")
    executed = subprocess.run([str(exe)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=environment) if compiled.returncode == 0 else None
    (build / (name + ".log")).write_text(compiled.stdout + (executed.stdout if executed else ""))
    passed = compiled.returncode == 0 and executed is not None and (
        executed.returncode == 1 if expected_failure else executed.returncode == 0)
    result = {"name": name, "compile_exit": compiled.returncode,
              "run_exit": executed.returncode if executed else None,
              "expected_failure": expected_failure, "passed": passed,
              "command": command, "log": str(build / (name + ".log"))}
    print(name + ": " + ("PASS" if passed else "FAIL"))
    if executed and not expected_failure:
        print(executed.stdout.rstrip())
    if not passed:
        print(compiled.stdout + (executed.stdout if executed else ""))
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", type=Path, required=True)
    parser.add_argument("--baseline", type=Path, required=True, help="Previous immutable source directory")
    args = parser.parse_args()
    build = args.build_dir.resolve()
    if build == ROOT or ROOT in build.parents:
        raise SystemExit("Build output must be outside the source tree")
    build.mkdir(parents=True, exist_ok=True)
    sources = [ROOT / "src/vrik/staged_pickup596c2.inl", args.baseline / "src/vrik/staged_pickup596c2.inl"]
    generated = []
    for name, source in zip(("current", "baseline"), sources):
        target = build / (name + "_consumer.inc")
        target.write_text(extract(source))
        generated.append(target)
    checked_in = ROOT / "tests/prop_adapter5916c1.inc"
    if checked_in.read_text() != generated[0].read_text():
        raise SystemExit("Checked-in fixture does not match literal production extraction")
    results = []
    for name, flags in (("stow_debug", ["-O0"]), ("stow_optimized", ["-O2"]),
                        ("stow_sanitized", ["-O1", "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-no-pie"])):
        results.append(run_case(build, name, "pickup_stow_d84_test.cpp", flags, generated[0]))
    results.append(run_case(build, "stow_baseline_negative", "pickup_stow_d84_test.cpp", ["-O2"], generated[1], True))
    for name in ("prop_adapter5916c1", "pickup_admission_d78", "small_pickup_d81", "pickup_input_d80", "instant_pickup_d72"):
        results.append(run_case(build, name, name + "_test.cpp", ["-O2"]))
    native_tu = ["-O2", "-fms-extensions", "-D__fastcall=",
                 "-I" + str(ROOT / "tests/syntax_shims"), "-ffunction-sections",
                 "-fdata-sections", "-Wl,--gc-sections"]
    for name in ("native_stow_confirmation_d84", "alchemy_presentation_receipt_d84", "dice_snapshot_d84", "crafting_clock_d81"):
        results.append(run_case(build, name, name + "_test.cpp", native_tu))
    hashed = sources + generated + [checked_in, ROOT / "tests/pickup_stow_d84_test.cpp",
        ROOT / "tests/prop_adapter5916c1_test.cpp", ROOT / "tests/instant_pickup_d72_test.cpp",
        ROOT / "src/core/haptics.h", ROOT / "src/vrik/instant_pickup_native_d72.inl",
        ROOT / "src/vrik/pickup_use_producer19c3.inl", ROOT / "src/native_grasp59c13.inl",
        ROOT / "src/activity_input163.cpp", ROOT / "src/subsystems/dice/diagnostic_d84.inl",
        ROOT / "src/subsystems/alchemy/presentation_speed_d81.inl",
        ROOT / "tests/native_stow_confirmation_d84_test.cpp", ROOT / "tests/alchemy_presentation_receipt_d84_test.cpp",
        ROOT / "tests/dice_snapshot_d84_test.cpp", ROOT / "tests/crafting_clock_d81_test.cpp", Path(__file__)]
    report = {"all_passed": all(r["passed"] for r in results), "results": results,
              "source_sha256": {str(p.resolve()): hashlib.sha256(p.read_bytes()).hexdigest() for p in hashed},
              "production_consumer_literal": True, "native_leaves": "explicit substitutions",
              "native_physics_execution": False, "Windows_SEH_tested": False,
              "LeakSanitizer": "NOT_RUN: executor /proc task enumeration unavailable", "headset": "NOT_RUN"}
    (build / "report.json").write_text(json.dumps(report, indent=2) + "\n")
    return 0 if report["all_passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
