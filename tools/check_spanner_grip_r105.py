#!/usr/bin/env python3
"""Compile the literal spanner pose builder and attachment writer on a host.

R108: the class 14 spanner grip is the clip's animated hand-slot relation at
the SpawnLever edge (R103); the R105 rig rest-pose relation is printed in the
F11 row only. Current-source checks need no Git history. Optional
--baseline-source accepts the R105 crossbow_rifle_request_h21.inl for a
compiled negative control (it fails the spawn edge grip check).
Native ownership, memory and entity APIs are explicit fixture leaves; this is
not an engine, DLL, mesh or headset test.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def function(text, signature):
    start = text.index(signature)
    brace = text.index("{", start)
    depth = 0
    for at in range(brace, len(text)):
        if text[at] == "{":
            depth += 1
        elif text[at] == "}":
            depth -= 1
            if depth == 0:
                return text[start:at + 1] + "\n"
    raise ValueError(signature)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--compiler", default=os.environ.get("CXX", "g++"))
    parser.add_argument("--baseline-source", type=Path)
    parser.add_argument("--baseline-repro-only", action="store_true")
    args = parser.parse_args()
    out = args.out.resolve()
    if out == ROOT or ROOT in out.parents:
        raise SystemExit("Evidence must be outside the source tree")
    out.mkdir(parents=True, exist_ok=True)
    paths = {
        "ranged": ROOT / "src/subsystems/ranged_reload/crossbow_rifle_request_h21.inl",
        "frik": ROOT / "src/frik_pose_probe.cpp",
        "coherent": ROOT / "src/vrik/coherent_pose.inl",
        "physical": ROOT / "src/vrik/physical_native.inl",
        "kernel": ROOT / "src/vrik/physical_weapon.h",
        "fixture": ROOT / "tests/spanner_grip_r105_test.cpp",
        "runner": Path(__file__),
    }
    source = {key: path.read_text() for key, path in paths.items()}
    fixture = source["fixture"]
    runs = []
    extracted = {}

    def prepare(directory, ranged):
        directory.mkdir(parents=True, exist_ok=True)
        builder = function(ranged, "static void BuildCrossbowPosesXR34(")
        helpers = ["static V3 Add(", "static V3 Sub(", "static V3 Mul(", "static float Dot(",
                   "static V3 Cross(", "static float Len(", "static V3 FromT(", "static void ToT(",
                   "static Q4 QConj(", "static Q4 QMul(", "static Q4 QNormalize(", "static Q4 QFromQuatT(",
                   "static void QToQuatT(", "static V3 QRotateV(", "static QuatT28 RelativeFromAbsolute(",
                   "static QuatT28 AbsoluteFromRelative("]
        math = "".join(function(source["frik"], sig) for sig in helpers)
        math += function(source["coherent"], "static bool ValidPose6689(")
        math += function(source["physical"], "static ph::Pose PhysPose6698(")
        math += function(source["physical"], "static ph::Pose PhysWorld6698(")
        math += function(source["physical"], "static ph::Pose PhysCurrentWorld164(")
        kernel = "namespace kcd2vr::physical { namespace m=pose_core; using m::Vec3; using m::Quat;\n"
        kernel += "struct Pose { Quat q{}; Vec3 p{}; };\n"
        for sig in ("inline Pose compose(", "inline Pose inverse(", "inline Vec3 point(", "inline bool finite("):
            kernel += function(source["kernel"], sig)
        kernel += "}\n"
        types = source["frik"][source["frik"].index("struct RangedSocketXR34 {"):source["frik"].index("struct CommittedFrame6695 {")]
        types += ranged[ranged.index("struct RangedGripXR34 {"):ranged.index("static core::SnapshotMailbox<RangedMechanismD78>")]
        code = "".join(function(ranged, sig) for sig in (
            "static pm::Vec3 RangedPointMatrixXR34(", "static bool RangedPlacementStillCurrentXR34(",
            "static bool RangedSameGripBindingXR34(", "static bool RangedSameSocketXR34(",
            "static bool RangedReadHandSocketXR34(", "static bool RangedRetainHandPlacementR95(",
            "static bool RangedRetainHeavyPlacementR95(", "static ph::Pose RangedWristPoseD72(",
            "static bool CrossbowHeavyToolAttachmentD72(",
            "static bool CrossbowAttachmentXR34(")) + builder
        uses_reference = "const QuatT28* reference" in builder[:builder.index("{")]
        caller = re.findall(r"BuildCrossbowPosesXR34\(packet,s\.original\.data\(\),s\.count[^;]*;", source["coherent"])
        if len(caller) != 1:
            raise RuntimeError("Expected one production coherent spanner call")
        if uses_reference:
            expected_call = "BuildCrossbowPosesXR34(packet,s.original.data(),s.count,s.rest.data(),s.rig.hand_r,s.rig.hand_l);"
            if caller[0] != expected_call:
                raise RuntimeError("Production coherent call does not supply this skeleton's reference and hand joints")
            code = "#define SPANNER_REFERENCE_R105 1\n" + code
        # The current path compiles its literal coherent call. The historical
        # builder accepts its original three arguments in the negative control.
        caller_code = caller[0] if uses_reference else "BuildCrossbowPosesXR34(packet,s.original.data(),s.count);"
        for name, content in (("spanner_kernel_actual.inc", kernel), ("spanner_math_actual.inc", math),
                              ("spanner_types_actual.inc", types), ("spanner_path_actual.inc", code),
                              ("spanner_caller_actual.inc", caller_code + "\n")):
            (directory / name).write_text(content)
            extracted[str((directory / name).relative_to(out))] = hashlib.sha256(content.encode()).hexdigest()
        (directory / "spanner_grip_r105_test.cpp").write_text(fixture)

    def run(name, directory, flags, expectation="pass", extra=()):
        exe = directory / name
        dep = directory / (name + ".d")
        cmd = [args.compiler, "-std=c++20", "-UNDEBUG", "-pthread", "-Wall", "-Wextra",
               "-Wno-unused-function", "-Wno-misleading-indentation", "-I", str(ROOT / "src"),
               "-I", str(directory), "-MMD", "-MF", str(dep), *flags,
               str(directory / "spanner_grip_r105_test.cpp"), "-o", str(exe)]
        build = subprocess.run(cmd, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (directory / (name + "_build.log")).write_text(build.stdout)
        if build.returncode:
            raise RuntimeError(name + " compile failed\n" + build.stdout[-6000:])
        env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0:halt_on_error=1", UBSAN_OPTIONS="halt_on_error=1")
        result = subprocess.run([str(exe), *extra], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env)
        (directory / (name + ".log")).write_text(result.stdout)
        passed = (result.returncode == 0 if expectation == "pass" else
                  result.returncode == 1 and "FAIL_R108 spawn edge grip" in result.stdout)
        dependencies = []
        if dep.exists():
            dependencies = [Path(p).resolve() for p in dep.read_text().replace("\\\n", " ").split(":", 1)[1].split()]
        runs.append({"name": name, "command": cmd, "arguments": list(extra), "expected": expectation,
                     "exit_code": result.returncode, "passed": passed, "output": result.stdout,
                     "dependency_sha256": {str(p): hashlib.sha256(p.read_bytes()).hexdigest() for p in dependencies if p.is_file()}})
        print(name + ": " + result.stdout.rstrip(), flush=True)
        exe.unlink(missing_ok=True)
        if not passed:
            raise RuntimeError(name + " unexpected result")

    if args.baseline_repro_only:
        prepare(out, source["ranged"])
        run("baseline_repro", out, ["-O0", "-g"], "reject_animated_grip")
    else:
        prepare(out, source["ranged"])
        run("current_debug", out, ["-O0", "-g"])
        run("current_optimized", out, ["-O2"])
        run("current_asan_ubsan", out, ["-O1", "-g", "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-no-pie"])
        if args.baseline_source:
            original = args.baseline_source.resolve()
            shadow = out / "baseline"
            prepare(shadow, original.read_text())
            run("original_r105_negative", shadow, ["-O0", "-g"], "reject_animated_grip")
            run("original_retained_timing", shadow, ["-O2"], extra=("--retained-only",))
    result = {"passed": all(r["passed"] for r in runs), "runs": runs,
              "source_sha256": {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in paths.values()},
              "extracted_sha256": extracted,
              "baseline_source": str(args.baseline_source.resolve()) if args.baseline_source else None,
              "baseline_sha256": hashlib.sha256(args.baseline_source.read_bytes()).hexdigest() if args.baseline_source else None,
              "limits": ["Literal production pose builder, hand socket reader, same-binding check, retention, hand attachment writer and heavy tool attachment writer execute.",
                         "Native lifetime/ownership, engine memory, entity placement/readback, body and passenger APIs are explicit fixtures.",
                         "Host transform examples are analytic fixtures, not captured native spanner asset geometry.",
                         "No Windows DLL build, native animation evaluation, visual mesh alignment, headset or in-game reload success claim.",
                         "Leak detection disabled for this host; address and undefined-behavior sanitizers remain enabled."]}
    (out / "RESULT.json").write_text(json.dumps(result, indent=2) + "\n")


if __name__ == "__main__":
    main()
