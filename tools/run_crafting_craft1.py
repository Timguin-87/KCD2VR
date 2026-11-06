#!/usr/bin/env python3
"""Run the dedicated CRAFT1 actual-header/adapter tests, never the game or a DLL.

Use clang++ or g++ on a host. Default ASan+UBSan excludes leak detection.
--baseline-zip is optional and must be the exact declared d6280d5d parent;
only the two new behavioral tests are added to a temporary parent copy.
A parent compile error is NOT a successful red test.
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
import tempfile
import zipfile

PARENT_SHA256 = "a4082516f2821da01eea08a2d81eee0066d95e8f01beca0aa65773bf69a246f4"
TESTS = {'near_selection67': 'tests/subsystems/alchemy/near_selection67_test.cpp', 'bridge67': 'tests/subsystems/alchemy/bridge67_test.cpp', 'geometry68': 'tests/subsystems/alchemy/geometry68_test.cpp', 'input67': 'tests/subsystems/alchemy/input67_test.cpp', 'gesture_work67': 'tests/subsystems/alchemy/gesture_work67_test.cpp', 'selection_intent72': 'tests/subsystems/alchemy/selection_intent72_test.cpp', 'acquisition_owner_h5': 'tests/subsystems/alchemy/acquisition_owner_h5_test.cpp', 'mortar_aim_h21': 'tests/subsystems/alchemy/mortar_aim_h21_test.cpp', 'mortar_input69': 'tests/subsystems/alchemy/mortar_input69_test.cpp', 'mortar_retry72': 'tests/subsystems/alchemy/mortar_retry72_test.cpp', 'mortar_pour70': 'tests/subsystems/alchemy/mortar_pour70_test.cpp', 'presentation71': 'tests/subsystems/alchemy/presentation71_test.cpp', 'binding70': 'tests/subsystems/alchemy/binding70_test.cpp', 'modes67': 'tests/subsystems/alchemy/modes67_test.cpp', 'alchemy_semi46': 'tests/alchemy_semi46_test.cpp', 'alchemy_input19c4': 'tests/alchemy_input19c4_test.cpp', 'semi_smith46': 'tests/semi_smith46_test.cpp', 'guided_crafting55': 'tests/guided_crafting55_test.cpp', 'bound_tools67': 'tests/subsystems/smithing/bound_tools67_test.cpp', 'proximity_craft1': 'tests/subsystems/alchemy/proximity_craft1_test.cpp', 'foreaft_craft1': 'tests/subsystems/smithing/foreaft_craft1_test.cpp'}
TESTS.update({"physical_release_repair": "tests/subsystems/alchemy/physical_release_repair_test.cpp", "mortar_geometry_repair": "tests/subsystems/alchemy/mortar_geometry_repair_test.cpp"})
TESTS.update({"smith_contact_dispatch_repair": "tests/smith_contact_dispatch_repair_test.cpp", "smith_tool_contact_repair": "tests/smith_tool_contact_repair_test.cpp"})
TESTS.update({"anvil_a3": "tests/anvil_a3_test.cpp", "native_contact68": "tests/subsystems/smithing/native_contact68_test.cpp"})
TESTS.update({"alchemy_tool_presentation_d39": "tests/alchemy_tool_presentation_d39_test.cpp", "tool_attachment59c10": "tests/tool_attachment59c10_test.cpp"})
TESTS.update({"mortar_motion_d41": "tests/subsystems/alchemy/mortar_motion_d41_test.cpp"})
REGRESSIONS = ("proximity_craft1", "foreaft_craft1")

def file_sha(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(2 * 1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()

def run_one(root: Path, destination: Path, name: str, compiler: str, sanitizers: bool) -> dict:
    destination.mkdir(parents=True, exist_ok=True)
    exe = destination / (name + (".exe" if os.name == "nt" else ""))
    command = [compiler, "-std=c++17", "-O1", "-g", "-UNDEBUG", "-fno-omit-frame-pointer"]
    if sanitizers:
        command += ["-fsanitize=address,undefined", "-fno-sanitize-recover=all"]
    command += ["-I", str(root / "src"), "-I", str(root / "tests"),
                "-I", str(root / "tests/subsystems/alchemy"), str(root / TESTS[name]), "-o", str(exe)]
    env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0", UBSAN_OPTIONS="print_stacktrace=1")
    result = {"name": name, "source": TESTS[name], "compile_command": command,
              "run_command": [str(exe)], "sanitizers": sanitizers, "native_game_execution": False}
    try:
        build = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                               env=env, timeout=180)
        (destination / (name + ".build.log")).write_text(build.stdout, encoding="utf-8")
        result["compile_exit"] = build.returncode
        if build.returncode == 0:
            run = subprocess.run([str(exe)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                 env=env, timeout=60)
            (destination / (name + ".run.log")).write_text(run.stdout, encoding="utf-8")
            result["run_exit"] = run.returncode
            result["output"] = run.stdout
        result["status"] = "PASS" if result.get("run_exit") == 0 else "FAIL"
    except subprocess.TimeoutExpired as error:
        result.update(status="TIMEOUT", error=str(error))
    except OSError as error:
        result.update(status="UNAVAILABLE", error=str(error))
    (destination / (name + ".json")).write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(f"{destination.name} {name}: {result['status']}", flush=True)
    return result

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", type=Path, required=True, help="Write host binaries/logs outside the source tree")
    parser.add_argument("--compiler", default=shutil.which("clang++") or shutil.which("g++"))
    parser.add_argument("--no-sanitizers", action="store_true")
    parser.add_argument("--jobs", type=int, default=2)
    parser.add_argument("--tests", nargs="+", choices=sorted(TESTS), help="Run only named current-source targets")
    parser.add_argument("--baseline-zip", type=Path)
    args = parser.parse_args()
    if not args.compiler:
        parser.error("No clang++/g++ found; use the standalone tests/crafting_craft1 CMake project for MSVC")
    root = Path(__file__).resolve().parents[1]
    destination = args.out.resolve()
    if destination == root or root in destination.parents:
        parser.error("--out must be outside the source tree")
    destination.mkdir(parents=True, exist_ok=True)
    extractor = subprocess.run([sys.executable, str(root / "tools/extract_alchemy_acquisition_public_h5.py"), "--check"],
                               text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=root)
    (destination / "public_extraction_check.log").write_text(extractor.stdout, encoding="utf-8")
    if extractor.returncode:
        print(extractor.stdout, file=sys.stderr)
        return 2
    results = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=max(1, args.jobs)) as pool:
        futures = [pool.submit(run_one, root, destination / "green", name, args.compiler,
                               not args.no_sanitizers) for name in (args.tests or TESTS)]
        results = [future.result() for future in futures]
    reds = []
    if args.baseline_zip:
        if file_sha(args.baseline_zip) != PARENT_SHA256:
            parser.error("baseline ZIP does not match the exact reviewed parent SHA-256")
        with tempfile.TemporaryDirectory(prefix="craft1_baseline_") as temp:
            parent = Path(temp)
            with zipfile.ZipFile(args.baseline_zip) as archive:
                for item in archive.infolist():
                    if item.is_dir() or not item.filename.startswith("KCD2VR_SOURCE/"):
                        continue
                    rel = Path(item.filename[len("KCD2VR_SOURCE/"):])
                    if rel.is_absolute() or ".." in rel.parts:
                        raise ValueError("Unsafe baseline member path")
                    if not rel.parts or rel.parts[0] not in {"src", "tests", "tools"}:
                        continue
                    target = parent / rel
                    target.parent.mkdir(parents=True, exist_ok=True)
                    target.write_bytes(archive.read(item))
            for name in REGRESSIONS:
                target = parent / TESTS[name]
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copyfile(root / TESTS[name], target)
                result = run_one(parent, destination / "red_parent", name, args.compiler, not args.no_sanitizers)
                result["expected_behavioral_red"] = (result.get("compile_exit") == 0 and
                    result.get("run_exit") == 1 and "FAIL" in result.get("output", "") and
                    "ERROR: AddressSanitizer" not in result.get("output", "") and
                    "runtime error:" not in result.get("output", ""))
                reds.append(result)
    summary = {"status": "PASS" if all(r["status"] == "PASS" for r in results) and
               all(r["expected_behavioral_red"] for r in reds) else "FAIL",
               "green_targets": results, "parent_regressions": reds,
               "engine_services": "Explicit native selection/attachments/phase leaves; no retail engine execution",
               "windows_dll_build": "NOT_RUN", "headset": "NOT_RUN"}
    (destination / "SUMMARY.json").write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"{sum(r['status']=='PASS' for r in results)}/{len(results)} selected patched targets passed; "
          f"{sum(r['expected_behavioral_red'] for r in reds)}/{len(reds)} expected parent reds", flush=True)
    return 0 if summary["status"] == "PASS" else 1

if __name__ == "__main__":
    raise SystemExit(main())
