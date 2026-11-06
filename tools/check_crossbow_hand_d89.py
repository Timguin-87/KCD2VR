#!/usr/bin/env python3
"""R89 xbow: build and run tests/crossbow_hand_d89_test.cpp against the real
crossbow_hand_d89.h, then against mutated copies (negative controls). Every
mutation reintroduces a known wrong behaviour and must make the test FAIL.

usage: python tools/check_crossbow_hand_d89.py --out /abs/dir
"""
import argparse, os, pathlib, shutil, subprocess, sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
HDR = "src/subsystems/ranged_reload/crossbow_hand_d89.h"
TEST = "tests/crossbow_hand_d89_test.cpp"
MUTATIONS = {
    # Claude D83 defect found by the D83 review: abs() turns a push into a pull.
    "abs_pull": ("const Vec moved=in.hand-start_;const double along=dot(moved,axis);\n            const double lateral=length(moved-axis*along);",
                 "const Vec moved=in.hand-start_;const double along=std::abs(dot(moved,axis));\n            const double lateral=length(moved-axis*dot(moved,axis));"),
    # bind before removing the hand bolt (native 289C930 removes first)
    "bind_before_remove": ("    if(!n.remove()||n.handId())return false;      // 289C930: remove the hand bolt FIRST\n"
                           "    if(!n.bind()||!n.loaded())return false;       // then 289C988 binds the ammo to the weapon\n",
                           "    if(!n.bind()||!n.loaded())return false;\n    if(!n.remove()||n.handId())return false;\n"),
    # accept a bolt the game put in the wrong hand
    "any_hand": ("    if(side!=0){(void)n.remove();return false;}", "    (void)side;"),
    # notify/tags before the cocked byte (native 2895F30 cocks first)
    "notify_before_cock": ("    if(!n.cock()||!n.cocked())return false;       // 2895F30 order: cock, then notify+tags\n    return n.cockNotify();",
                           "    if(!n.cockNotify())return false;\n    return n.cock()&&n.cocked();"),
    # swallowing the game's immediate (r8b!=0) cock+bind path too
    "defer_immediate": ("in.b==0&&in.henryEquipment", "in.henryEquipment"),
    # nock anywhere on release (no rail check)
    "nock_anywhere": ("&&segmentDistance(in.hand,in.rest,in.rail)<=kRailReach)s.command=Command::Nock;", ")s.command=Command::Nock;"),
    # grab without a neutral release first
    "no_neutral": ("const bool edge=down&&neutral_;", "const bool edge=down;"),
    # R96: idle hands never give the request back (old "can't even natively reload")
    "never_give_back": ("return now>=parkedAt&&now-parkedAt>=windowMs?Park::GiveBack:Park::Keep;", "return Park::Keep;"),
    # R96: replaying a parked request into an action that already runs
    "replay_into_running": ("if(xr34Owned||nativeInFlight)return Park::Drop;", "if(xr34Owned)return Park::Drop;"),
    # R96: pass state never ends (a session latch on native reloads)
    "pass_latch": ("        end();return false;\n    }\n};", "        return true;\n    }\n};"),
    # R111: heavy crossbow back on the crank (the hand pull switch ignored)
    "heavy_crank_back": ("{return lightClass(c)||(c==15&&heavyHandPull)||(c==14&&mediumHandPull);}", "{(void)heavyHandPull;return lightClass(c)||(c==14&&mediumHandPull);}"),
    # R117: goat's foot back on the spanner (the medium hand pull switch ignored)
    "medium_spanner_back": ("{return lightClass(c)||(c==15&&heavyHandPull)||(c==14&&mediumHandPull);}", "{(void)mediumHandPull;return lightClass(c)||(c==15&&heavyHandPull);}"),
    # R117: the medium chest grip opens the tool stage again
    "medium_chest_tool": ("            if(stringPullClass(in.classId,in.heavyHandPull,in.mediumHandPull)){", "            if(stringPullClass(in.classId,in.heavyHandPull,false)){"),
    # R117: medium give-back after 1.5 s (spanner animation)
    "medium_window_ignored": ("return cocked==0&&((c==15&&heavyHandPull)||(c==14&&mediumHandPull))?", "(void)mediumHandPull;return cocked==0&&(c==15&&heavyHandPull)?"),
    # R111: heavy hand pull keeps the forward crank axis / crank travel
    "heavy_pull_forward": ("if(in.classId==15&&!in.heavyHandPull){axis=axis*-1.;need=in.crankTravel;}", "if(in.classId==15){axis=axis*-1.;need=in.crankTravel;}"),
    # R111: heavy give-back window ignored (cranequin animation after 1.5 s)
    "heavy_window_ignored": ("std::uint64_t windowMs=kNativeWindowMs) noexcept {", "std::uint64_t windowMs_=kNativeWindowMs) noexcept {(void)windowMs_;const std::uint64_t windowMs=kNativeWindowMs;"),
}

def build_run(src_root, out, name):
    exe = out / f"crossbow_hand_d89_{name}"
    cmd = ["g++", "-std=c++20", "-O1", "-fsanitize=address,undefined", "-I", str(src_root / "src"),
           str(ROOT / TEST), "-o", str(exe)]
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode:
        return None, r.stderr[-2000:]
    r = subprocess.run([str(exe)], capture_output=True, text=True)
    return r.returncode, r.stdout.strip().splitlines()[-1] if r.stdout.strip() else r.stderr[-500:]

def main():
    ap = argparse.ArgumentParser(); ap.add_argument("--out", required=True); a = ap.parse_args()
    out = pathlib.Path(a.out).resolve(); out.mkdir(parents=True, exist_ok=True)
    ok = True
    rc, msg = build_run(ROOT, out, "real")
    print("real:", msg); ok &= rc == 0
    text = (ROOT / HDR).read_text()
    for name, (old, new) in MUTATIONS.items():
        if text.count(old) != 1:
            print(f"mutation {name}: anchor not found exactly once"); ok = False; continue
        root = out / f"mut_{name}"; hdr = root / HDR
        hdr.parent.mkdir(parents=True, exist_ok=True); hdr.write_text(text.replace(old, new))
        shutil.copyfile(ROOT / "src/subsystems/ranged_reload/crossbow_hand_owner_r91.h", hdr.parent / "crossbow_hand_owner_r91.h")
        shutil.copyfile(ROOT / "src/subsystems/ranged_reload/crossbow_refusal_d85.h", hdr.parent / "crossbow_refusal_d85.h")
        rc, msg = build_run(root, out, name)
        caught = rc not in (0, None)
        print(f"negative control {name}: {'FAILED as expected' if caught else 'NOT CAUGHT'} ({msg})")
        ok &= caught
        shutil.rmtree(root, ignore_errors=True)
    print("RESULT", "PASS" if ok else "FAIL"); return 0 if ok else 1

if __name__ == "__main__":
    sys.exit(main())
