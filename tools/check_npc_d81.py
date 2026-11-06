#!/usr/bin/env python3
"""Actual NPC translation unit, scene policy and Lua 5.1 receipt regressions.

Windows hook/engine/VM functions are explicit fixture leaves. XML execution is
limited to the real registration gate/wait path; no native AI/headset claim.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--compiler", default="g++")
    parser.add_argument("--lua-path", type=Path, help="Existing site-package directory containing lupa.lua51")
    args = parser.parse_args()
    out = args.out.resolve()
    assert out != ROOT and ROOT not in out.parents
    out.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    if args.lua_path:
        env["PYTHONPATH"] = str(args.lua_path.resolve()) + (os.pathsep + env["PYTHONPATH"] if env.get("PYTHONPATH") else "")
    env["ASAN_OPTIONS"] = "detect_leaks=0"
    runs = []

    def run(name, command, expected_failure=False):
        result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env)
        (out / (name + ".log")).write_text(result.stdout)
        passed = result.returncode != 0 if expected_failure else result.returncode == 0
        runs.append({"name": name, "command": command, "exit_code": result.returncode,
                     "passed": passed, "expected_failure": expected_failure, "output": result.stdout})
        print(name + ": " + ("PASS" if passed else "FAIL"), flush=True)
        if not passed:
            raise RuntimeError(name + "\n" + result.stdout)

    for suite in ["lively_npc_d73", "npc_dialogue_d76", "npc_hmd_target_d76"]:
        binary = out / suite
        run(suite + "_build", [args.compiler, "-std=c++20", "-pthread", "-O0", "-g", "-D__fastcall=",
                               "-I", str(ROOT / "src"), "-I", str(ROOT / "tests/syntax_shims"),
                               "-ffunction-sections", "-fdata-sections", str(ROOT / "tests" / (suite + "_test.cpp")),
                               "-Wl,--gc-sections", "-o", str(binary)])
        run(suite, [str(binary)])
    binary = out / "npc_callbacks_sanitized"
    run("npc_callbacks_sanitized_build", [args.compiler, "-std=c++20", "-pthread", "-O1", "-g", "-D__fastcall=",
                                          "-I", str(ROOT / "src"), "-I", str(ROOT / "tests/syntax_shims"),
                                          "-ffunction-sections", "-fdata-sections", "-fsanitize=address,undefined",
                                          "-fno-omit-frame-pointer", "-no-pie", str(ROOT / "tests/npc_hmd_target_d76_test.cpp"),
                                          "-Wl,--gc-sections", "-o", str(binary)])
    run("npc_callbacks_sanitized", [str(binary)])
    run("lua_lifecycle", [sys.executable, str(ROOT / "tests/test_lively_npc_d73.py")])
    run("lua_receipts", [sys.executable, str(ROOT / "tests/lively_npc_idle_d80_test.py"), str(out / "lua_result.json")])
    # Restore the original permanent OFF wait in an external XML copy. The
    # delayed-first-packet case must fail while native scene/membership is valid.
    xml = ROOT / "companion/source/AI/npc/basic/switch/runDialog.xml"
    original = xml.read_text()
    old = '<Else canSkip="1">\n                            <Success />'
    broken = '<Else canSkip="1">\n                            <Wait duration="\'-1\'" timeType="GameTime" doFail="false" variation="" />'
    assert original.count(old) == 1
    negative = out / "permanent_receipt_wait.xml"
    negative.write_text(original.replace(old, broken, 1))
    check = out / "negative_receipt.py"
    check.write_text("import importlib.util, pathlib, unittest, sys\n"
                     + "spec=importlib.util.spec_from_file_location('npc_actual'," + repr(str(ROOT / "tests/lively_npc_idle_d80_test.py")) + ")\n"
                     + "module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)\n"
                     + "module.XML=pathlib.Path(" + repr(str(negative)) + ")\n"
                     + "suite=unittest.TestSuite([module.Idle('test_delayed_first_packet_reaches_same_native_dialogue')])\n"
                     + "result=unittest.TextTestRunner(verbosity=2).run(suite)\n"
                     + "sys.exit(0 if result.wasSuccessful() else 1)\n")
    run("permanent_receipt_wait_negative", [sys.executable, str(check)], True)
    paths = [xml, ROOT / "companion/source/Scripts/Startup/zzkcd2vr_life_d73.lua",
             ROOT / "src/subsystems/dialogues/runtime/npc_lookat_hmd.cpp",
             ROOT / "src/subsystems/dialogues/runtime/lively_npc_d73.inl",
             ROOT / "src/subsystems/dialogues/lively_npc_d73.h", Path(__file__).resolve()]
    paths += [ROOT / "tests" / name for name in ["npc_hmd_target_d76_test.cpp", "lively_npc_d73_test.cpp",
              "npc_dialogue_d76_test.cpp", "lively_npc_idle_d80_test.py", "test_lively_npc_d73.py"]]
    result = {"passed": all(r["passed"] for r in runs), "runs": runs,
              "source_sha256": {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},
              "lua_package_path": str(args.lua_path.resolve()) if args.lua_path else "PYTHONPATH/default environment",
              "limits": ["Windows API/hooks, native dialogue, script packet call and engine link/timer/entity functions are explicit fixture leaves.",
                         "Actual C++ TU and Lua 5.1 script run; host XML runner covers only receipt gates, waits and Lua.",
                         "Native AI scheduler, final NPC animations/head pose and headset NOT_RUN.",
                         "ASan/UBSan enabled for callbacks; LeakSanitizer disabled on this host."]}
    (out / "RESULT.json").write_text(json.dumps(result, indent=2) + "\n")


if __name__ == "__main__":
    main()
