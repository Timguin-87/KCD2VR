#!/usr/bin/env python3
"""Portable D39 menu/utility regression runner; native leaves remain explicit.

The utility dispatch fixture includes the complete current production function,
not a reimplementation. Records the source hashes with every run.
"""
from pathlib import Path
import argparse
import hashlib
import json
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("output", type=Path)
parser.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[1])
parser.add_argument("--compiler", default="g++")
args = parser.parse_args()
root, output = args.source.resolve(), args.output.resolve()
output.mkdir(parents=True, exist_ok=True)
host = (root / "src/subsystems/hud_ui_controls/host.inl").read_text(encoding="utf-8-sig")
start = host.index("struct GestureFrame59c5")
end = host.index("static kcd2vr::gesture_talk59c5::Output PrepareGestureTalkH4", start)
dispatch = host[start:end]
assert "static void ApplyUtilityGestures54" in dispatch
(output / "utility_dispatch_d39_production.inc").write_text(dispatch, encoding="utf-8")
mod = (root / "src/mod.cpp").read_text(encoding="utf-8-sig")
frik = (root / "src/frik_pose_probe.cpp").read_text(encoding="utf-8-sig")
stow = (root / "src/subsystems/throw_drop/runtime/grip_stow_cb1.cpp").read_text(encoding="utf-8-sig")
supply = (root / "src/subsystems/throw_drop/runtime/action_owner_leaves.inl").read_text(encoding="utf-8-sig")

def function(text, signature):
    """Extract one balanced complete production function, retaining its body."""
    start = text.index(signature)
    brace = text.index("{", start)
    depth, state, pos = 0, "code", brace
    while pos < len(text):
        char, following = text[pos], text[pos + 1:pos + 2]
        if state == "line":
            if char == "\n": state = "code"
        elif state == "comment":
            if char == "*" and following == "/": state = "code"; pos += 1
        elif state in ['"', "'"]:
            if char == "\\": pos += 1
            elif char == state: state = "code"
        elif char == "/" and following == "/": state = "line"; pos += 1
        elif char == "/" and following == "*": state = "comment"; pos += 1
        elif char in ['"', "'"]: state = char
        elif char == "{": depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0: return text[start:pos + 1]
        pos += 1
    raise AssertionError("Unterminated production function: " + signature)

shared_names = ["bool PreciseContacts44(", "void SetPreciseContacts44(", "int HandsModeDs2(",
                "void SetHandsModeDs2(", "bool HandSeparationDs1(", "void SetHandSeparationDs1(",
                "float GetShoulderReachD38(", "void SetShoulderReachD38(",
                "bool NativeHandPhysicsH24(", "void SetNativeHandPhysicsH24("]
(output / "live_shared_setters_d39_production.inc").write_text("\n".join(function(frik, name) for name in shared_names) + "\n")
(output / "live_stow_setters_d39_production.inc").write_text("\n".join(function(stow, name) for name in ["bool Enabled(", "void SetEnabled(", "int Button(", "void SetButton("]) + "\n")
(output / "live_supply_setters_d39_production.inc").write_text("\n".join(function(supply, name) for name in ["void SetThrowingSupplyT2(", "bool ThrowingSupplyT2("]) + "\n")
read_start = mod.index("    auto& live=out.live_d39;", mod.index("void Mod::FillMenuState("))
(output / "live_mod_read_d39_production.inc").write_text(mod[read_start:mod.index("    out.enabled =", read_start)])
apply_start = mod.index("    // D39: all three hand modes share one owner")
(output / "live_mod_apply_d39_production.inc").write_text(mod[apply_start:mod.index("#define KCD2VR_M2_HOST_SLOT 71", apply_start)])
save_start = mod.index('    ok = wb("VrPreciseSurfaceContacts"')
save_end = mod.index('\n', mod.index('    ok = wb("VrVisorGestureD39"', save_start))
(output / "live_mod_save_d39_production.inc").write_text(mod[save_start:save_end] + "\n")
load_start = mod.index("    // D39: persist the complete NORMAL/PRECISE/PERFORMANCE")
load_end = mod.index('\n', mod.index('ReadIniBoolExplicit(explicit_ini,"Settings","VrVisorGestureD39"', load_start))
load = mod[load_start:load_end] + "\n"
smooth_start = mod.index("    // D39_SMOOTH_PREFERENCE_LOAD_BEGIN")
smooth_end = mod.index("    // D39_SMOOTH_PREFERENCE_LOAD_END", smooth_start)
load += mod[smooth_start:smooth_end] + "\n"
for key in ["VrCameraBodyHoldMs", "VrBowArmAim", "VrAlchemyOwnHeadH51", "VrAlchemyPickupReachD39"]:
    matches = [line for line in mod.splitlines() if '"' + key + '"' in line and "ReadIni" in line]
    assert len(matches) == 1, (key, matches)
    load += matches[0] + "\n"
(output / "live_mod_load_d39_production.inc").write_text(load)
inputs = [
    "src/subsystems/experimental/live_settings_d39.h",
    "src/subsystems/hud_ui_controls/host.inl",
    "src/vrik/utility_gestures54.h",
    "src/mod.cpp", "src/vr_menu.cpp", "src/vr_menu.h",
    "tests/live_settings_d39_test.cpp", "tests/utility_dispatch_d39_fixture.cpp",
    "tests/utility_gestures54_test.cpp",
    "tests/live_owner_adapter_d39_fixture.cpp", "src/frik_pose_probe.cpp",
    "src/subsystems/throw_drop/runtime/grip_stow_cb1.cpp",
    "src/vrik/station_ws_j27.h", "src/subsystems/alchemy/reach_policy_craft1.h",
    "src/subsystems/throw_drop/host.inl", "src/subsystems/throw_drop/runtime/action_owner_leaves.inl",
    "tests/holster_history_hs_test.cpp", "src/vrik/physical_weapon.h", "src/vrik/equipment_gesture114.h",
    "src/vrik/smooth_lab_f2.h", "tests/smooth_lab_f2_test.cpp",
]
results = []
for name, source in [
    ("live_settings_d39", "tests/live_settings_d39_test.cpp"),
    ("utility_dispatch_d39", "tests/utility_dispatch_d39_fixture.cpp"),
    ("utility_gestures54", "tests/utility_gestures54_test.cpp"),
    ("live_owner_adapter_d39", "tests/live_owner_adapter_d39_fixture.cpp"),
    ("holster_history_hs", "tests/holster_history_hs_test.cpp"),
    ("smooth_lab_f2", "tests/smooth_lab_f2_test.cpp"),
]:
    executable = output / name
    command = [args.compiler, "-std=c++17", "-O2", "-Wall", "-Wextra", "-Werror",
               "-I", str(root / "src"), "-I", str(output), str(root / source), "-o", str(executable)]
    if name in {"holster_history_hs", "smooth_lab_f2"}:
        command.insert(6, "-Wno-misleading-indentation")  # Unchanged MSVC-era header formatting.
    built = subprocess.run(command, capture_output=True, text=True)
    result = {"name": name, "build_exit": built.returncode, "build_stdout": built.stdout, "build_stderr": built.stderr}
    if built.returncode == 0:
        run = subprocess.run([str(executable)], capture_output=True, text=True)
        result.update(exit=run.returncode, stdout=run.stdout, stderr=run.stderr)
    results.append(result)
receipt = {"source": str(root), "compiler": args.compiler, "retail_native_execution": False,
           "scope": "Production click reducer, Mod read/apply/save/load contributions, shared setting setters and complete utility dispatcher with explicit native/XR boundary doubles.",
           "inputs": {p: hashlib.sha256((root / p).read_bytes()).hexdigest() for p in inputs}, "results": results}
(output / "RESULT.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
for result in results:
    print(result["name"], "PASS" if result.get("exit") == 0 else "FAIL")
    stdout = result.get("stdout", "")
    print(stdout if result["name"] != "holster_history_hs" else stdout.splitlines()[-1] + "\n", end="")
    if result["build_stderr"]: print(result["build_stderr"])
    if result.get("stderr"): print(result["stderr"])
raise SystemExit(0 if all(r.get("exit") == 0 for r in results) else 1)
