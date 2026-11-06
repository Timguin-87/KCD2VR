#!/usr/bin/env python3
"""D78 actual reload intent/constraint/clock tests with explicit native leaves.

Generated slices and causal shadow sources remain outside the working tree.
Windows SEH is replaced by one scoped C++ cleanup in the existing adapter;
this does not validate Windows exceptions, ABI execution or headset output.
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
    pos = text.index("{", start) + 1
    depth = 1
    while depth:
        depth += (text[pos] == "{") - (text[pos] == "}")
        pos += 1
    return text[start:pos] + "\n"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--compiler", default="g++")
    args = parser.parse_args()
    out = args.out.resolve()
    assert ROOT not in out.parents
    out.mkdir(parents=True, exist_ok=True)
    ranged_path = ROOT / "src/subsystems/ranged_reload/crossbow_rifle_request_h21.inl"
    reload_path = ROOT / "src/subsystems/ranged_reload/reload_passenger_xr34.inl"
    header_path = ROOT / "src/subsystems/ranged_reload/crossbow_native_xr34.h"
    constraint_path = header_path.with_name("crossbow_constraint_d78.h")
    adapter_path = header_path.with_suffix(".inl")
    ranged, reload = ranged_path.read_text(), reload_path.read_text()
    raw = function(ranged, "static bool RangedRawIntentD78(")
    chest = reload[:reload.index("// Narrow presentation/source acquisition")]
    completed = function(reload, "static xcXR34::Source CompletedReloadPoseSourceXR34(")
    (out / "raw_source_d78.inc").write_text(raw + chest + completed)
    adapter = adapter_path.read_text()
    start = adapter.index("    __try{started=g_crossbow_xr34.startNewAfterRestore(")
    end_text = "__finally{(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}"
    end = adapter.index(end_text, start) + len(end_text)
    body = adapter[start:end]
    assert body.count("__try") == 1 and body.count("__finally") == 1
    host = body.replace("    __try{", "    {struct HostFinishD78 {ma::Token& token;bool& started;~HostFinishD78(){(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}} finish{token,started};", 1)
    host = host.replace("}\n    " + end_text, "}")
    assert "__finally" not in host
    (out / "native_adapter_host.inc").write_text(adapter[:start] + host + adapter[end:])
    names = ["crossbow_native_xr34_test.cpp", "crossbow_mechanism_d78_test.cpp"]
    sources = {}
    for name in names:
        code = (ROOT / "tests" / name).read_text().replace('"../src/', '"' + (ROOT / "src").as_posix() + '/')
        code = code.replace('"' + adapter_path.as_posix() + '"', '"' + (out / "native_adapter_host.inc").as_posix() + '"')
        sources[name] = code
        (out / name).write_text(code)
    coherent = (ROOT / "src/vrik/coherent_pose.inl").read_text()
    frik = (ROOT / "src/frik_pose_probe.cpp").read_text()
    final_wrist = coherent.index('#include "../subsystems/vrik/pose_final_wrist_d52.inl"')
    constraint = coherent.index("ConstrainReloadWristD78(", final_wrist)
    station = coherent.index('#include "station_hand_admission_d75.inl"', final_wrist)
    assert final_wrist < constraint < station
    assert "const ph::Pose (&raw)[2]" in reload
    assert "const bool (&valid)[2]" in reload
    assert "tx.propRequested5916c1,tx.propRequestedValid5916c1" in re.sub(r"\s+", "", coherent[final_wrist:station])
    skin = frik[frik.index("void __fastcall SkinningTransformCoreDetour("):]
    assert skin.index("CrossbowMechanismSkinningD78(") < skin.index("BowCandidate128(")
    runs = []

    def build_run(name, directory, flags, expected_failure=False):
        binary = directory / name
        command = [args.compiler, "-std=c++20", "-pthread", *flags,
                   "-I", str(ROOT / "src"), "-I", str(ROOT / "tests/syntax_shims"), "-I", str(out),
                   str(directory / "crossbow_mechanism_d78_test.cpp"), "-o", str(binary)]
        built = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (directory / (name + "_build.log")).write_text(built.stdout)
        if built.returncode:
            raise RuntimeError(name + " compile failed\n" + built.stdout)
        env = os.environ.copy()
        env["ASAN_OPTIONS"] = "detect_leaks=0"
        ran = subprocess.run([str(binary)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env)
        (directory / (name + ".log")).write_text(ran.stdout)
        passed = (ran.returncode == 1 and "FAIL_D78" in ran.stdout) if expected_failure else ran.returncode == 0
        runs.append({"name": name, "command": command, "exit_code": ran.returncode, "expected_failure": expected_failure,
                     "passed": passed, "output": ran.stdout})
        print(name + ": " + ran.stdout, end="", flush=True)
        if not passed:
            raise RuntimeError(name + " unexpected execution result")

    build_run("mechanism_debug", out, ["-O0", "-g"])
    build_run("mechanism_optimized", out, ["-O2"])
    build_run("mechanism_asan_ubsan", out, ["-O1", "-g", "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-no-pie"])
    mutations = {
        "feedback_from_allowed": (constraint_path.name, "return s.intentKnownD78?s.intentHandD78[side]:s.hand[side];", "return s.hand[side];"),
        "stale_mechanism_clock": (constraint_path.name, "!sameMechanismClockD78(p.binding.mechanismClockD78,p.clock)||", ""),
        "stale_string_clock": (constraint_path.name, "!sameMechanismClockD78(p.binding.stringClockD78,p.clock)||", ""),
        "linear_string_target": (constraint_path.name,
                                 "const Vec pin{p.binding.stringCurrentD78.x+c.stringGripD78.x,p.binding.stringCurrentD78.y+c.stringGripD78.y,\n            p.binding.stringCurrentD78.z+c.stringGripD78.z};",
                                 "const Vec pin{c.start.x+c.axis.x*c.progress*c.travel,c.start.y+c.axis.y*c.progress*c.travel,c.start.z+c.axis.z*c.progress*c.travel};"),
        "remote_contact_credit": (header_path.name, "!reloadContactDomainD78(contactDomain,spatial,intent)", "false"),
    }
    for name, (filename, old, new) in mutations.items():
        shadow = out / name
        shadow.mkdir(exist_ok=True)
        text = constraint_path.read_text()
        header = header_path.read_text()
        if filename == constraint_path.name:
            assert text.count(old) == 1
            text = text.replace(old, new)
        else:
            assert header.count(old) == 1
            header = header.replace(old, new)
        (shadow / constraint_path.name).write_text(text)
        header = re.sub(r'#include "([^"]+)"', lambda m: '#include "' +
                        ((shadow / m[1]) if m[1] == constraint_path.name else (header_path.parent / m[1]).resolve()).as_posix() + '"', header)
        (shadow / "client.h").write_text(header)
        for filename, code in sources.items():
            (shadow / filename).write_text(code.replace('"' + header_path.as_posix() + '"', '"' + (shadow / "client.h").as_posix() + '"'))
        build_run(name, shadow, ["-O0", "-g"], True)
    paths = [ranged_path, reload_path, header_path, constraint_path, adapter_path,
             ROOT / "src/subsystems/ranged_reload/crossbow_mechanism_d78.inl", Path(__file__).resolve()]
    paths += [ROOT / "tests" / name for name in names]
    result = {"passed": all(r["passed"] for r in runs), "runs": runs,
              "integration": {"constraint_before_arm_admission": True, "raw_inputs_const": True, "observer_before_pose_writers": True},
              "source_sha256": {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},
              "limits": ["Retail identity/memory/native events are explicit fixture leaves; no game or headset execution.",
                         "Windows SEH finally is one host-only scoped C++ substitute; no Windows SEH claim.",
                         "This suite does not prove actual native skeleton scheduling, mounted tool pixels or ammunition inventory outcome.",
                         "ASan and UBSan enabled; LeakSanitizer disabled on this host."]}
    (out / "RESULT.json").write_text(json.dumps(result, indent=2) + "\n")


if __name__ == "__main__":
    main()
