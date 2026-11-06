"""Run focused D40 production-adapter regressions without loading game code.

Example: python tools/run_npc_physics_d40.py --output ../analysis/npc_physics
Optionally supply --baseline /path/to/immutable/D39/source for the before/after
native-adapter reproduction. The baseline is read only.
"""
from pathlib import Path
import argparse
import concurrent.futures
import json
import os
import re
import shlex
import subprocess
import time

def expected_baseline_failure(returncode, stdout, stderr):
    """Only a normally completed fixture with explicit failed assertions counts."""
    labels = [line for line in stdout.splitlines() if line.startswith("FAIL ")]
    summary = re.search(r"^D40 actual corpse adapter: (\d+) checks, (\d+) failures$", stdout, re.MULTILINE)
    abnormal = any(marker in (stdout + stderr).lower() for marker in
                   ("sanitizer", "runtime error:", "segmentation fault", "core dumped", "terminate called"))
    return (returncode == 1 and not stderr.strip() and not abnormal and summary is not None
            and 0 < int(summary.group(2)) == len(labels) <= int(summary.group(1)))


def output_is_outside_sources(output, *sources):
    output = output.resolve()
    return all(source is None or (output != source.resolve() and source.resolve() not in output.parents)
               for source in sources)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--baseline", type=Path)
    parser.add_argument("--compiler", default=os.environ.get("CXX", "g++"))
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    output = args.output.resolve()
    baseline = args.baseline.resolve() if args.baseline else None
    if not output_is_outside_sources(output, root, baseline):
        parser.error("--output must be outside both the current source tree and the optional immutable baseline source tree")
    output.mkdir(parents=True, exist_ok=True)
    compiler = shlex.split(args.compiler)
    generated = output / "generated"
    subprocess.run([os.sys.executable, str(root / "tools/extract_corpse_drag_d40.py"), str(generated)], check=True)
    names = ["lab58", "npc_grab_policy60", "npc_grab_bridge60", "corpse_drag_policy60", "npc_push60",
             "corpse_grip_cg1", "corpse_body_owner_d40", "corpse_drag_native_d40",
             "impale_successor_h18_coordinator", "impale_constraint_h21", "native_articulated_phase_h21"]
    jobs = [(name, root, generated, False) for name in names]
    if baseline:
        before = output / "generated_d39"
        subprocess.run([os.sys.executable, str(root / "tools/extract_corpse_drag_d40.py"), str(before), "--source-root", str(baseline)], check=True)
        jobs.append(("corpse_drag_native_d40", baseline, before, True))

    def run(job):
        name, source, include, is_baseline = job
        label = ("baseline_d39_" if is_baseline else "") + name
        exe = output / (label + "_test")
        command = compiler + ["-std=c++20", "-O0", "-UNDEBUG", "-pthread", "-I", str(source / "src"), "-I", str(include)]
        if is_baseline:
            command += ["-DKCD2VR_D40_ADAPTER_BASELINE"]
        command += [str(root / "tests" / (name + "_test.cpp")), "-o", str(exe)]
        began = time.monotonic()
        build = subprocess.run(command, capture_output=True, text=True)
        (output / (label + ".build.log")).write_text(build.stdout + build.stderr, encoding="utf-8")
        row = {"test": label, "source_root": str(source), "build_command": command,
               "build_rc": build.returncode, "baseline_expected_failure": is_baseline, "passed": False}
        if not build.returncode:
            result = subprocess.run([str(exe)], capture_output=True, text=True)
            row.update(run_rc=result.returncode, output=result.stdout + result.stderr)
            row["passed"] = (expected_baseline_failure(result.returncode, result.stdout, result.stderr)
                             if is_baseline else result.returncode == 0)
            (output / (label + ".run.log")).write_text(row["output"], encoding="utf-8")
        row["seconds"] = round(time.monotonic() - began, 3)
        print(f"{label}: {'PASS' if row['passed'] else 'FAIL'}", flush=True)
        return row

    with concurrent.futures.ThreadPoolExecutor(max_workers=2) as pool:
        rows = list(pool.map(run, jobs))
    report = {"scope": "Host fixtures of actual production adapters; native leaves mocked, game DLL not loaded/executed.",
              "results": rows, "passed": all(row["passed"] for row in rows)}
    (output / "npc_physics_d40_results.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    return 0 if report["passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
