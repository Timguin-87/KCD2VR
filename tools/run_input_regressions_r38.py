#!/usr/bin/env python3
"""Replay R38 input regressions against extracted production decisions.

Accepts --source for an immutable before/after comparison. The production
functions and adapter clauses are regenerated on every run; no native game
calls or operating-system input are executed.
"""
import argparse
import json
import re
import subprocess
from pathlib import Path

HERE = Path(__file__).resolve().parents[1]


def function(text, name):
    start = re.search(r"(?m)^.*\b" + re.escape(name) + r"\([^;]*?\)\s*(?:noexcept\s*)?\{", text)
    if start is None:
        raise RuntimeError("Missing production function: " + name)
    brace = text.index("{", start.start())
    depth = 1
    end = brace + 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    return text[start.start():end] + "\n"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--source", type=Path, default=HERE)
    parser.add_argument("--out", type=Path, required=True)
    args = parser.parse_args()
    root, out = args.source.resolve(), args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    holster = (root / "src/subsystems/throw_drop/holster_gestures.inl").read_text()
    helpers = function(holster, "PhysGestureMount126")
    for name in ("PhysMainWeaponR27", "PhysAnotherMainHeldR27", "PhysDrawCandidatePolicyR38", "PhysDrawCandidate126"):
        if name in holster:
            helpers += function(holster, name)
    helpers += function(holster, "PhysOverShoulderGrabJ28")
    (out / "r38_holster_helpers.inc").write_text(helpers)

    represented = holster[holster.index("     bool heldKeysRepresentedR15=true;"):holster.index("     for(int i=0;i<e.count&&i<8;++i)")]
    direct = re.search(r"const bool direct=PhysDrawCandidate126\(e,i\);", holster).group()
    free_hand = re.search(r"const bool grabbingHandFreeR26=[^;]+;", holster).group()
    switch = re.search(r"switchHS2\[i\]=[^;]+;", holster).group()
    (out / "r38_switch_clause.inc").write_text(represented + direct + "\n" + free_hand + "\n" + switch + "\n")
    lost = re.search(r"const bool switchLostR15=[^;]+;", holster).group()
    (out / "r38_switch_lost.inc").write_text(lost + "\n")

    host = (root / "src/subsystems/hud_ui_controls/host.inl").read_text()
    utility = host[host.index("struct GestureFrame59c5"):host.index("static kcd2vr::gesture_talk59c5::Output PrepareGestureTalkH4")]
    (out / "r38_utility_host.inc").write_text(utility)

    mod = (root / "src/mod.cpp").read_text()
    load = re.search(r"(?m)^\s*(?:const bool heldMapLegacyR38=[^;]+;\s*)?g_held_map59c10r10\.store\(ReadIniBoolExplicit\(explicit_ini,\"Settings\",\"VrHeldParchmentMapDs7\",[^;]+;", mod).group()
    (out / "r38_map_config.inc").write_text(load)

    pose = (root / "src/subsystems/base/pose_physical_executor.inl").read_text()
    begin = re.search(r"(?m)^\s*(?:const )?auto holsterInput=ph::holster_tracking_input_h43", pose).start()
    finish = pose.index("                PhysHolsterGestures6698", begin)
    (out / "r38_holster_adapter.inc").write_text(pose[begin:finish])

    fixture = HERE / "tests/input_regressions_r38_test.cpp"
    command = ["g++", "-std=c++17", "-O0", "-g0", "-Wall", "-Wextra",
               "-Wno-misleading-indentation", "-Wno-unused-parameter", "-Wno-unused-function",
               "-DR38_HAVE_SWITCH_PRIORITY=" + str(int("switchCandidates" in helpers)),
               "-I" + str(out), "-I" + str(root / "src"), str(fixture), "-o", str(out / "input_regressions_r38")]
    built = subprocess.run(command, capture_output=True, text=True)
    (out / "compile.log").write_text(built.stdout + built.stderr)
    result = {"source": str(root), "command": command, "compile_returncode": built.returncode}
    if built.returncode == 0:
        binary = out / "input_regressions_r38"
        binary.chmod(binary.stat().st_mode | 0o700)
        run = subprocess.run([str(binary)], capture_output=True, text=True)
        (out / "run.log").write_text(run.stdout + run.stderr)
        result["run_returncode"] = run.returncode
        print(run.stdout, end="")
    else:
        print(built.stderr)
    (out / "results.json").write_text(json.dumps(result, indent=2) + "\n")
    return result.get("run_returncode", built.returncode)


if __name__ == "__main__":
    raise SystemExit(main())
