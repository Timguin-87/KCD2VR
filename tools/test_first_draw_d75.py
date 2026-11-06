#!/usr/bin/env python3
"""D78 review: execute current visibility and strict melee attachment code on Windows.

The engine, equipment publisher, tracker and renderer are substituted leaves.
This proves callback ordering/ownership, not game pixels or DLL compatibility.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess

ROOT = Path(__file__).resolve().parent.parent


def function(text, signature):
    start = text.index(signature)
    opening = text.index("{", start)
    depth = 1
    position = opening + 1
    while depth:
        if text[position] == "{":
            depth += 1
        elif text[position] == "}":
            depth -= 1
        position += 1
    return text[start:position]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out", "--output", dest="output", type=Path, required=True)
    parser.add_argument("--compiler", default="cl")
    args = parser.parse_args()
    if Path(args.compiler).name.lower() not in ("cl", "cl.exe", "clang-cl", "clang-cl.exe"):
        parser.error("D78 executes the actual __try/__finally native call. Use cl/clang-cl in a Windows developer environment; no C++ exception substitute is used.")
    args.output = args.output.resolve()
    assert ROOT not in args.output.parents, "Keep generated review evidence outside the source tree"
    args.output.mkdir(parents=True, exist_ok=True)
    frik = (ROOT / "src/frik_pose_probe.cpp").read_text()
    physical = (ROOT / "src/vrik/physical_native.inl").read_text()
    ranged = (ROOT / "src/subsystems/ranged_reload/crossbow_rifle_request_h21.inl").read_text()
    equipment = (ROOT / "src/subsystems/physics/native_equipment_snapshot.inl").read_text()
    tracking = (ROOT / "src/vrik/weapon_tracking.inl").read_text()
    stowed = (ROOT / "src/subsystems/ranged_reload/stowed_bow_visibility.inl").read_text()
    native = frik[frik.index("    const bool nativeOk=[&]() {"):]
    native = native[:native.index("    }();") + len("    }();")]
    # Exact full physical-mode dispatch prefix; lower diagnostic/controller
    # modes are outside the new core feature and outside this fixture.
    dispatch = function(frik, "static void __fastcall EntityAttachmentUpdateDetourV6688(")
    dispatch = dispatch[:dispatch.index("    const int controllerMode=")] + "}\n"
    dispatch = dispatch.replace('#include "vrik/native_activity_passthrough46.inl"',
                                (ROOT / "src/vrik/native_activity_passthrough46.inl").read_text())
    extracts = {
        "native_hand": function(tracking, "static bool NativeHandKey6697("),
        "candidate_item": function(tracking, "static bool NativeCandidateItemId6699("),
        "item_world": function(equipment, "static bool PhysReadItemWorld54("),
        "item_description": function(equipment, "static bool PhysDescribeItem54("),
        "entity_identity": function(physical, "static bool PhysEntityIdentity135("),
        "stowed_forget": function(stowed, "static void StowedBowForget59c14("),
        "stowed_release": function(stowed, "static bool StowedBowRelease59c14("),
        "native_attach_scope": function(frik, "static bool InvokeItemAttachOperationD78(") + "\nstatic bool DrawNativeOperationD75(void* owner,void* item,int side) {\n" + native + "\nreturn nativeOk;\n}\n",
        "native_solver_attachment": (ROOT / "src/subsystems/physics/held_attachment_owner.inl").read_text(),
        "crossbow_wrist": function(ranged, "static ph::Pose RangedWristPoseD72("),
        "crossbow_weapon_attachment": function(ranged, "static bool CrossbowWeaponAttachmentD72("),
        "physical_mode_dispatch": dispatch,
    }
    for name, code in extracts.items():
        (args.output / (name + ".inc")).write_text(code)
    # The real physical placement route must use universal admission, actual
    # current placement/readback, and must retain pending hides on read gaps.
    pf = function(physical, "static PhysicalAttachmentResult101 PhysAttachmentResultImpl101(")
    assert "drawWeaponD75=wdrawD75::weaponClass(item.profile.classId)" in pf
    assert "pendingRevealD72=drawWeaponD75&&" in pf
    assert pf.index("RangedDrawBeginD72(drawBindingD72)") < pf.index("PhysFrame6698 frame{}")
    assert pf.index("PlacementReadback6689(") < pf.index("RangedDrawPlacedD72(drawBindingD72,verify,verifyOk&&placementCurrentD75)")
    assert "if(nativeOwnerD75)RangedDrawRetireAttachmentD72(self,attachment);" in pf
    assert pf.index("WeaponDrawMayStowD75(equipment,item)") < pf.index("if(!item.held){restore(0,true);")
    assert dispatch.index("WeaponDrawBeforeAttachmentD75(") < dispatch.index("NativeHeldAttachmentT4(") < dispatch.index("CrossbowAttachmentXR34(")
    production = ["src/subsystems/ranged_reload/ranged_draw_policy_d72.h",
                  "src/subsystems/ranged_reload/ranged_draw_visibility_d72.inl",
                  "src/subsystems/ranged_reload/weapon_draw_scope_d75.h",
                  "src/subsystems/ranged_reload/first_draw_binding_d75.inl",
                  "src/subsystems/ranged_reload/crossbow_rifle_request_h21.inl",
                  "src/vrik/weapon_tracking.inl",
                  "src/subsystems/physics/native_equipment_snapshot.inl",
                  "tests/first_draw_d75_test.cpp", "tools/test_first_draw_d75.py",
                  "src/subsystems/ranged_reload/stowed_bow_visibility.inl",
                  "src/subsystems/physics/held_attachment_owner.inl",
                  "src/vrik/physical_native.inl", "src/frik_pose_probe.cpp", "tests/native_attach_scope_d78_test.cpp"]
    results = []
    for mode, flags, fixture in [("debug", ["/Od", "/Zi"], "first_draw_d75_test.cpp"),
                        ("optimized", ["/O2"], "first_draw_d75_test.cpp"),
                        ("asan", ["/Od", "/Zi", "/fsanitize=address"], "first_draw_d75_test.cpp"),
                        ("native_scope_seh", ["/Od", "/Zi"], "native_attach_scope_d78_test.cpp")]:
        binary = args.output / ("weapon_draw_" + mode + ".exe")
        command = [args.compiler, "/nologo", "/std:c++20", "/EHsc", "/MD", "/W3", *flags,
                   "/I" + str(ROOT / "src"), "/I" + str(args.output),
                   str(ROOT / "tests" / fixture), "/Fe:" + str(binary),
                   "/Fo:" + str(args.output / (mode + ".obj")),
                   "/Fd:" + str(args.output / (mode + ".pdb"))]
        if fixture == "native_attach_scope_d78_test.cpp":
            command.append('/DKCD2VR_NATIVE_ATTACH_D78_EXTRACT="' + (args.output / "native_attach_scope.inc").as_posix() + '"')
        built = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (args.output / (mode + "_build.log")).write_text(built.stdout)
        if built.returncode:
            print(built.stdout)
            raise SystemExit(built.returncode)
        run_env = os.environ.copy()
        run = subprocess.run([str(binary)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=run_env)
        (args.output / (mode + ".log")).write_text(run.stdout)
        print(run.stdout, end="")
        results.append({"mode": mode, "exit_code": run.returncode, "output": run.stdout})
        if run.returncode:
            raise SystemExit(run.returncode)
    # Causal controls operate only on private extracted/shadow files. They must
    # compile successfully and fail the specific expected assertion, never merely
    # report an unrelated compiler or process failure as evidence.
    visibility = (ROOT / "src/subsystems/ranged_reload/ranged_draw_visibility_d72.inl").read_text()
    tick_line = "    for(int side=0;side<2;++side){auto& debt=g_ranged_draw_d72[side];auto& accepted=g_weapon_draw_accepted_d75[side];"
    live_line = "    if(!WeaponDrawHeldNowD75(binding,item?item->item:debt.item,item?equipment.manager:debt.manager))return;"
    assert visibility.count(tick_line) == visibility.count(live_line) == 1
    controls = [
        ("without_early_guard", None, dispatch.replace("    WeaponDrawBeforeAttachmentD75(self,attachment);\n", ""),
         "every eligible family/hand concealed before native WAITING callback"),
        ("timer_reveal", visibility.replace(tick_line, tick_line + "\n        if(debt.owned&&now>=debt.since&&now-debt.since>=750){(void)RangedDrawReleaseD72(side);continue;}"), None,
         "long delay and stale metadata never reveal"),
        ("without_live_readback", visibility.replace(live_line, "    // Causal negative: omit current native hand/item graph after readback."), None,
         "strict D76 melee graph is rechecked after native placement/readback"),
    ]
    negatives = []
    for name, shadow, negative_dispatch, expected in controls:
        folder = args.output / name
        folder.mkdir(exist_ok=True)
        for extract_name, code in extracts.items():
            if extract_name == "physical_mode_dispatch" and negative_dispatch is not None:
                code = negative_dispatch
            (folder / (extract_name + ".inc")).write_text(code)
        if shadow is not None:
            header = folder / "subsystems/ranged_reload/ranged_draw_visibility_d72.inl"
            header.parent.mkdir(parents=True, exist_ok=True)
            header.write_text(shadow)
        binary = folder / "causal_control.exe"
        command = [args.compiler, "/nologo", "/std:c++20", "/EHsc", "/MD", "/Od", "/Zi",
                   "/I" + str(folder), "/I" + str(ROOT / "src"),
                   str(ROOT / "tests/first_draw_d75_test.cpp"), "/Fe:" + str(binary),
                   "/Fo:" + str(folder / "causal_control.obj"), "/Fd:" + str(folder / "causal_control.pdb")]
        built = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (folder / "build.log").write_text(built.stdout)
        if built.returncode:
            raise RuntimeError("Causal control did not compile: " + name + "\n" + built.stdout)
        run = subprocess.run([str(binary)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (folder / "run.log").write_text(run.stdout)
        causal = run.returncode == 1 and expected in run.stdout
        negatives.append({"name": name, "exit_code": run.returncode, "causal": causal, "output": run.stdout})
        print(name + ": " + run.stdout, end="")
        if not causal:
            raise RuntimeError("Causal control failed to catch the expected regression: " + name)
    receipt = {"result": "PASS", "executed_actual_sources": list(extracts), "modes": results, "causal_negatives": negatives,
               "source_sha256": {p: hashlib.sha256((ROOT / p).read_bytes()).hexdigest() for p in production},
               "extraction_sha256": {name: hashlib.sha256(code.encode()).hexdigest() for name, code in extracts.items()},
               "limits": ["Engine lookups, entity writes, equipment/tracking publication are host substitutes.",
                          "Complete physical callback is source-wiring checked; its writer leaf in dispatch is substituted.",
                          "Windows MSVC AddressSanitizer only; UBSan/LeakSanitizer not claimed.",
                          "Actual __try/__finally helper also executed with real RaiseException under /EHsc; original callbacks are fixture leaves.",
                          "No headset, renderer pixel, game executable or Windows DLL validation."]}
    (args.output / "RESULT.json").write_text(json.dumps(receipt, indent=2) + "\n")


if __name__ == "__main__":
    main()
