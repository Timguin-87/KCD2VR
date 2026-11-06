#!/usr/bin/env python3
"""Run the focused D39 source/host checks with explicit native execution limits.

Requires Python 3 and GCC/G++ on Linux or WSL. Output must be outside this source
tree. The historical all-tests suite has separate known failures; this runner
does not relabel those failures as successes. --with-packaging also requires
CMake and Make, and uses named imported dependency stubs for configuration only.
"""
from __future__ import annotations
import argparse
import concurrent.futures
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[1]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--jobs", type=int, default=2)
    parser.add_argument("--with-packaging", action="store_true")
    parser.add_argument("--cmake", default="cmake")
    parser.add_argument("--only", nargs="+", help="Run only named groups listed by --list")
    parser.add_argument("--list", action="store_true")
    args = parser.parse_args()
    output = args.out.resolve()
    if output == ROOT or ROOT in output.parents:
        parser.error("--out must be outside the source tree")
    output.mkdir(parents=True, exist_ok=True)
    python = sys.executable

    def tool(name: str, *arguments: object) -> list[str]:
        return [python, str(ROOT / "tools" / name), *(str(a) for a in arguments)]

    groups: dict[str, list[list[str]]] = {
        "source_partition": [tool("check_subsystems.py")],
        "shoulder_authority": [tool("verify_pose_authority_d39.py", "--output", output / "shoulder")],
        "hand_pair_adapter": [tool("verify_hand_pair_adapter_d39.py", "--output", output / "pair_adapter")],
        "head_frame_admission": [tool("verify_head_frame_admission_d39.py", "--output", output / "head_admission")],
        "head_camera_chain": [tool("verify_head_frame_chain_d39.py", "--output", output / "head_chain")],
        "native_bow_aliases": [tool("verify_bow_body_admission_d39.py", "--output", output / "bow_aliases")],
        "retained_camera": [tool("run_bow_camera_d37b.py", "--out", output / "retained_camera")],
        "retained_bow_identity": [tool("run_bow_camera_identity_d37b.py", "--out", output / "retained_bow")],
        "live_experiments": [tool("test_live_experiments_d39.py", output / "experiments")],
        "crafting": [tool("run_crafting_craft1.py", "--out", output / "crafting", "--jobs", 2,
                          "--tests", "input67", "acquisition_owner_h5", "physical_release_repair",
                          "mortar_geometry_repair", "smith_contact_dispatch_repair",
                          "smith_tool_contact_repair", "anvil_a3", "native_contact68",
                          "alchemy_tool_presentation_d39", "tool_attachment59c10",
                          "proximity_craft1", "presentation71", "foreaft_craft1")],
    }
    cpp = shutil.which("g++") or "g++"
    for name in ("hand_pair_contact_d39", "controller_authority_r38", "shoulder_reach_d38", "hands59c7f"):
        executable = output / name
        groups[name] = [[cpp, "-std=c++17", "-O2", "-I", str(ROOT / "src"),
                         str(ROOT / "tests" / (name + "_test.cpp")), "-o", str(executable)],
                        [str(executable)]]
    packaging = {
        "companion_source": [[python, str(ROOT / "tests/test_companion_source63.py")]],
        "companion_cache": [[python, str(ROOT / "tests/companion_source_d39_test.py")]],
        "companion_cmake": [tool("verify_companion_cmake_d39.py", "--cmake", args.cmake,
                                 "--out", output / "companion_cmake")],
        "companion_incremental": [tool("verify_companion_incremental_d39.py", "--cmake", args.cmake,
                                       "--out", output / "companion_incremental")],
    }
    if args.list:
        print("Default groups: " + ", ".join(groups))
        print("Packaging groups (--with-packaging): " + ", ".join(packaging))
        return 0
    if args.with_packaging:
        groups.update(packaging)
    if args.only:
        unknown = set(args.only) - set(groups)
        if unknown:
            parser.error("Unknown or unselected groups: " + ", ".join(sorted(unknown)))
        groups = {name: groups[name] for name in args.only}

    def run_group(name: str, commands: list[list[str]]) -> dict:
        started = time.monotonic()
        steps = []
        logfile = output / (name + ".log")
        with logfile.open("w", encoding="utf-8") as stream:
            for command in commands:
                stream.write("COMMAND " + json.dumps(command) + "\n")
                stream.flush()
                try:
                    run = subprocess.run(command, cwd=ROOT, stdout=stream, stderr=subprocess.STDOUT,
                                         timeout=900, env=dict(os.environ, PYTHONDONTWRITEBYTECODE="1"))
                    step = {"command": command, "exit": run.returncode}
                except (OSError, subprocess.TimeoutExpired) as error:
                    step = {"command": command, "exit": None, "error": str(error)}
                steps.append(step)
                if step["exit"] != 0:
                    break
        passed = len(steps) == len(commands) and all(s["exit"] == 0 for s in steps)
        print(name + ": " + ("PASS" if passed else "FAIL") + " (" + str(logfile) + ")", flush=True)
        return {"name": name, "passed": passed, "steps": steps, "log": str(logfile),
                "seconds": round(time.monotonic() - started, 3)}

    with concurrent.futures.ThreadPoolExecutor(max_workers=max(1, min(args.jobs, 4))) as pool:
        futures = [pool.submit(run_group, name, commands) for name, commands in groups.items()]
        results = [future.result() for future in futures]
    passed = all(result["passed"] for result in results)
    receipt = {"passed": passed, "scope": "Focused D39 host/source gates; not the entire historical suite",
               "native_windows_build": "NOT_RUN", "game_headset": "NOT_RUN",
               "build_identity": (ROOT / "src/build_identity.h").read_text().strip(),
               "runner_sha256": hashlib.sha256(Path(__file__).read_bytes()).hexdigest(), "results": results}
    (output / "D39_RESULTS.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(str(sum(result["passed"] for result in results)) + "/" + str(len(results)) + " groups passed")
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
