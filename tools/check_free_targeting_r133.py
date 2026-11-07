#!/usr/bin/env python3
"""R133 free targeting 3-way mode: source wiring check + the pure host test.

Checks that every owner is actually connected (INI read + save, Experiments switch cycling 0/1/2, the cone
lease re-lease, the camera hold, the body frame sites, the stick turn, the opponent edge) and compiles/runs
tests/free_targeting_plus_r133_test.cpp. No game, no headset.
"""
import argparse, os, subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def text(rel):
    return (ROOT / rel).read_text(encoding="utf-8", errors="replace")


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--out", type=Path, required=True)
    a = p.parse_args()
    a.out.mkdir(parents=True, exist_ok=True)
    checks = []

    def ck(ok, what):
        checks.append((bool(ok), what))

    rt = text("src/subsystems/experimental/live_runtime_d40.inl")
    ck('ReadIniIntExplicit(explicit_ini,"Settings","VrFreeTargetingModeR133",-1)' in rt, "INI reads VrFreeTargetingModeR133")
    ck(rt.count('"VrFreeTargeting",true') == 1 and rt.count('"VrFreeTargeting",false') == 1, "old VrFreeTargeting read twice (presence test)")
    ck('wi("VrFreeTargetingModeR133"' in rt and 'wb("VrFreeTargeting"' in rt, "INI save writes the new key and the old bool")
    ck("free_targeting_d42::setMode(" in rt, "menu apply + INI load go through setMode")
    ls = text("src/subsystems/experimental/live_settings_d39.h")
    ck("case Id::FreeTargetingD42:d.freeTargetingModeR133=" in ls, "Experiments switch cycles the mode")
    ck('L"FREE PLUS' in ls and 'L"OFF (GAME LOCK + CAMERA)"' in ls, "Experiments label names the 3 modes")
    mod = text("src/mod.cpp")
    ck("int freeMode=0) noexcept {count=0;freeModeR133=freeMode;" in mod, "cone lease takes the mode")
    ck("if(modeR133!=g_combat_cvars52.freeModeR133)" in mod, "mode change re-leases the cone")
    ck('"R133 FREE_TARGETING mode=' in mod and '"R133 FREE_PLUS fight_end' in mod, "one row per mode change / held fight")
    ck("r::rotateStick(pad.lx,pad.ly,offsetR133)" in mod and "ApplyRoomscalePad58(pad" in mod and
       mod.index("r::rotateStick(pad.lx") < mod.index("ApplyRoomscalePad58(pad,padCtx"), "stick turn before the dodge/follow injection")
    ch = text("src/camera_hook.cpp")
    ck("r133::step(holdR133,in)" in ch and "for(auto& v:visual_basis)" in ch, "camera hook runs the hold and turns the view basis")
    fr = text("src/frik_pose_probe.cpp")
    ck("static CalFrameR133T CalFrameR133()" in fr, "body frame helper")
    sites = {"src/subsystems/vrik/pose_tracking_goals.inl": 1, "src/vrik/weapon_tracking.inl": 1,
             "src/vrik/native_combat104.inl": 3, "src/subsystems/roomscale/pose_producer_d76.inl": 1}
    for rel, n in sites.items():
        ck(text(rel).count("CalFrameR133()") == n, "body frame offset used in " + rel)
    raw = sum(text(rel).count("{g_cal_frame_qx.load(),g_cal_frame_qy.load(),g_cal_frame_qz.load(),g_cal_frame_qw.load()}") for rel in sites)
    ck(raw == 0, "no raw calibrated frame left in the per-frame body consumers")
    hc = text("src/diagnostics/henry_combat_context_h20.inl")
    ck("free_targeting_r133::opponentMs.store" in hc, "opponent edge published from C_CombatPlayer update")
    tc = text("src/subsystems/vrik/tracked_camera_service.inl")
    ck("free_targeting_r133::rootYawTick.store" in tc, "root yaw published with the tracked camera base")
    pm = text("src/subsystems/hud_ui_controls/runtime/native_vr_pad_premap.cpp")
    ck("free_targeting_r133::userTurnMs.store" in pm, "player's own turn input published")

    cxx = os.environ.get("CXX", "g++")
    exe = a.out / "ft133"
    b = subprocess.run([cxx, "-std=c++20", "-O1", "-Wall", "-Wextra", "-I", str(ROOT / "src"),
                        str(ROOT / "tests/free_targeting_plus_r133_test.cpp"), "-o", str(exe)], capture_output=True, text=True)
    ck(b.returncode == 0, "host test builds")
    run = subprocess.run([str(exe)], capture_output=True, text=True) if b.returncode == 0 else None
    ck(run is not None and run.returncode == 0, "host test passes")
    fails = [w for ok, w in checks if not ok]
    report = "\n".join(("PASS " if ok else "FAIL ") + w for ok, w in checks)
    (a.out / "report.txt").write_text(report + "\n" + (run.stdout if run else b.stderr))
    print(report)
    if run:
        print(run.stdout.strip())
    print(f"R133 wiring: {len(checks) - len(fails)}/{len(checks)} PASS")
    raise SystemExit(1 if fails else 0)


if __name__ == "__main__":
    main()
