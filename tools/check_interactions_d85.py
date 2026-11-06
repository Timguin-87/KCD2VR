#!/usr/bin/env python3
"""D85 interact lane: run the production-path tests and their negative controls.

Every C++ test compiles the current production code (literal extraction or the
complete owner TU fixture). Each negative control compiles the SAME test against
the D84R1 baseline sources and must fail. Native leaves are substitutions; no
game, Windows SEH, Lua 5.1 VM or headset runs here.

  python3 tools/check_interactions_d85.py --build-dir <outside tree> --baseline <D84R1 source root>
"""
import argparse
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
NATIVE = ["-fms-extensions", "-D__fastcall=", "-ffunction-sections", "-fdata-sections", "-Wl,--gc-sections"]


def extract_small(source, out):
    text = source.read_text(encoding="utf-8-sig")
    out.write_text(text[text.index("static bool SmallInstantPickupAllowedD81("):].rstrip() + "\n")


def extract_consumer(source, out):
    sys.path.insert(0, str(ROOT / "tools"))
    import importlib.util
    spec = importlib.util.spec_from_file_location("d84", ROOT / "tools/check_interactions_d84.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    out.write_text(module.extract(source))


def run(build, name, cwd, sources, flags, expect_fail=False):
    exe = build / name
    cmd = ["g++", "-std=c++23", "-O1", "-pthread", "-I" + str(Path(cwd) / "../src"), "-I" + str(Path(cwd) / "syntax_shims"), *flags, *sources, "-o", str(exe)]
    comp = subprocess.run(cmd, cwd=cwd, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0:halt_on_error=1", UBSAN_OPTIONS="halt_on_error=1")
    ex = subprocess.run([str(exe)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env) if comp.returncode == 0 else None
    ok = comp.returncode == 0 and ex is not None and ((ex.returncode != 0) if expect_fail else ex.returncode == 0)
    out = (ex.stdout if ex else comp.stdout[-2000:]).strip()
    print(f"{name}: {'PASS' if ok else 'FAIL'}{' (negative control)' if expect_fail else ''}\n  {out.splitlines()[-1] if out else ''}")
    return {"name": name, "passed": ok, "expect_fail": expect_fail, "compile": comp.returncode,
            "run": ex.returncode if ex else None, "output": out[-1500:]}


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--build-dir", type=Path, required=True)
    ap.add_argument("--baseline", type=Path, required=True)
    a = ap.parse_args()
    build = a.build_dir.resolve()
    if build == ROOT or ROOT in build.parents:
        raise SystemExit("build dir must be outside the source tree")
    build.mkdir(parents=True, exist_ok=True)
    base = a.baseline.resolve()
    # Baseline test tree: D84R1 sources with the D85 test files beside them.
    btests = build / "baseline_tree/tests"
    if (build / "baseline_tree").exists():
        shutil.rmtree(build / "baseline_tree")
    shutil.copytree(base / "tests", btests)
    os.symlink(base / "src", build / "baseline_tree/src")
    for f in ("small_pickup_native_weight_d85_test.cpp", "instant_pickup_hand_d85_test.cpp", "native_release_velocity_d85_test.cpp",
              "prop_kinematic_throw_d85_test.cpp", "native_stow_confirmation_d84_test.cpp", "instant_pickup_d72_test.cpp",
              "prop_adapter5916c1_test.cpp", "pickup_admission_d78_test.cpp"):
        shutil.copy(ROOT / "tests" / f, btests / f)
    extract_small(base / "src/vrik/staged_pickup596c2.inl", build / "small_base.inc")
    extract_consumer(base / "src/vrik/staged_pickup596c2.inl", build / "consumer_base.inc")
    t = ROOT / "tests"
    san = ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
    r = []
    r.append(run(build, "small_pickup_native_weight_d85", t, ["small_pickup_native_weight_d85_test.cpp"], san))
    # Same current headers; only the extracted D84R1 admission consumer differs.
    r.append(run(build, "small_pickup_native_weight_d85_baseline", t, ["small_pickup_native_weight_d85_test.cpp"],
                 ['-DSMALL_PICKUP_D81_INC="%s"' % (build / "small_base.inc")], True))
    r.append(run(build, "instant_pickup_hand_d85", t, ["instant_pickup_hand_d85_test.cpp"], san))
    r.append(run(build, "instant_pickup_hand_d85_baseline", btests, ["instant_pickup_hand_d85_test.cpp"], [], True))
    r.append(run(build, "native_release_velocity_d85", t, ["native_release_velocity_d85_test.cpp"], NATIVE))
    r.append(run(build, "native_release_velocity_d85_baseline", btests, ["native_release_velocity_d85_test.cpp"], NATIVE + ["-DD85_BASELINE"], True))
    r.append(run(build, "native_stow_take_tick_d85", t, ["native_stow_confirmation_d84_test.cpp"], NATIVE))
    r.append(run(build, "native_stow_take_tick_d85_baseline", btests, ["native_stow_confirmation_d84_test.cpp"], NATIVE, True))
    r.append(run(build, "prop_kinematic_throw_d85", t, ["prop_kinematic_throw_d85_test.cpp"], san))
    r.append(run(build, "prop_kinematic_throw_d85_baseline", btests, ["prop_kinematic_throw_d85_test.cpp"],
                 ['-DPROP_ADAPTER_D78_INC="%s"' % (build / "consumer_base.inc")], True))
    # Corpse Lua: actual shipped script with light-userdata entity handles.
    lab = "companion/Mods/kcd2vr_lab/Scripts/Startup/zzkcd2vr_lab.lua"
    for name, script, src, neg in (
            ("corpse_entity_handle_d85", "corpse_entity_handle_d85_test.py", ROOT / "companion/source/zzkcd2vr_corpse_d72.lua", False),
            ("corpse_entity_handle_d85_baseline", "corpse_entity_handle_d85_test.py", base / "companion/source/zzkcd2vr_corpse_d72.lua", True),
            ("small_pickup_lua_d85", "small_pickup_lua_d85_test.py", ROOT / lab, False),
            ("small_pickup_lua_d85_baseline", "small_pickup_lua_d85_test.py", base / lab, True)):
        if not src.exists():
            print(f"{name}: SKIPPED (no {src})")
            r.append({"name": name, "passed": False, "skipped": True})
            continue
        p = subprocess.run([sys.executable, str(t / script), str(src)], text=True,
                           stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        ok = (p.returncode != 0) if neg else p.returncode == 0
        print(f"{name}: {'PASS' if ok else 'FAIL'}{' (negative control)' if neg else ''}\n  {p.stdout.strip().splitlines()[-1]}")
        r.append({"name": name, "passed": ok, "expect_fail": neg, "output": p.stdout[-1500:]})
    report = {"all_passed": all(x["passed"] for x in r), "results": r, "native_leaves": "substituted",
              "headset": "NOT_RUN", "lua51": "NOT_RUN (Lua 5.4 via ctypes)", "windows_seh": "NOT_RUN"}
    (build / "report_d85.json").write_text(json.dumps(report, indent=2) + "\n")
    return 0 if report["all_passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
