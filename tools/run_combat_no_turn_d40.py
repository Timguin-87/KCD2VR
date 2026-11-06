#!/usr/bin/env python3
"""Compile literal production motion adapters and verify pins against a real PE.

The native leaf, clock, copied tracking mailboxes, hook library and process-memory
reads are explicit host substitutes. The supplied PE is read, never executed.
An optional baseline uses its actual gates and late/position adapters; where its
rotation hook is absent, the fixture calls its explicit native leaf. This makes
the regression negative honest about the boundary being substituted.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess

from native_research112 import PE
from run_fist_alignment_d37 import extract

ROOT = Path(__file__).resolve().parents[1]
EXPECTED_DLL = "bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7"
COMMON_FUNCTIONS = (
    "CurrentOrdinaryMotion59c3", "NativeFistMotionD37", "NativeMotionContext112",
    "NativeAlignmentOwned112", "NativePositionCommandOwned36", "NativePositionDispatch36",
    "NativePositionDetour36", "NativeAlignmentResult112", "NativeAlignmentDetour112",
    "NativeMotionContract112",
)
ROTATION_FUNCTIONS = (
    "NativeRotationCommandOwnedD40", "NativeRotationDispatchD40", "NativeRotationDetourD40",
    "NativeRotationContractD40", "NativeRotationArmD40",
)
PUBLIC_FUNCTIONS = (
    "SetCombatNoStep55", "CombatNoStep55", "SetCombatNoTurnD40", "CombatNoTurnD40",
    "CombatNoTurnReadyD40", "CombatNoTurnVetoCountD40",
)
LEGACY_PINS = {0x14095E8: 14, 0xA70FA3: 7, 0xA71B60: 15, 0x9E37A8: 16,
               0x9E2D50: 13, 0xA7EB53: 8, 0x3A633B0 + 7 * 8: 8}


def function(text, name, static=True):
    if static:
        return extract(text, name)
    match = re.search(r"^(?:void|bool|std::uint64_t) " + re.escape(name) + r"\(", text, re.M)
    if not match:
        raise ValueError("missing public production function " + name)
    start = match.start()
    brace = text.index("{", match.end())
    depth, end = 1, brace + 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    return text[start:end] + "\n"


def rotation_pins(source_text, pe):
    body = function(source_text, "NativeRotationContractD40")
    arrays = {}
    for name, size, data in re.findall(r"const unsigned char (\w+)\[(\d+)\]\{([^}]+)\};", body):
        values = bytes(int(x.strip(), 0) for x in data.split(",") if x.strip())
        if len(values) != int(size):
            raise ValueError("pin array length mismatch " + name)
        arrays[name] = values
    rows = []
    for address, name in re.findall(r"pin\((0x[0-9A-Fa-f]+),(\w+)\)", body):
        rva = int(address, 0)
        expected = arrays[name]
        actual = pe.read(rva, len(expected))
        rows.append({"rva": hex(rva), "name": name, "length": len(actual),
                     "production_expected": expected.hex(), "dll_actual": actual.hex(),
                     "matches": actual == expected})
    return rows


def run(source, out, pe, native_pins, sanitize):
    out.mkdir(parents=True, exist_ok=True)
    path = source / "src/subsystems/base/native_motion_owner.inl"
    data = path.read_bytes()
    text = data.decode().replace("\r\n", "\n")
    present = "static bool NativeRotationCommandOwnedD40(" in text
    globals_end=text.index("// D42 selection") if "// D42 selection" in text else text.index("// R59C3:")
    globals_text = text[text.index("using NativeAlignmentQuery112="):globals_end]
    generated = f"#define D40_HAS_ROTATION {int(present)}\n" + globals_text
    has_free="static bool NativeNoTurnSelectedD42(" in text
    generated += f"#define D42_HAS_FREE_TARGETING {int(has_free)}\n"
    if has_free:
        generated += function(text,"NativeNoTurnSelectedD42")+function(text,"NativeFreeViewContextD42")
    generated += "\n".join(function(text, n) for n in COMMON_FUNCTIONS)
    if present:
        generated += "\n".join(function(text, n) for n in ROTATION_FUNCTIONS)
    generated += function(text, "NativeMotionArm112")
    public_path = source / "src/frik_pose_probe.cpp"
    if present:
        public = public_path.read_text()
        generated += "\n".join(function(public, n, False) for n in PUBLIC_FUNCTIONS)
        if has_free:generated += function(public,"CombatFreeViewContextD42",False)
    (out / "generated_combat_no_turn_d40.inc").write_text(generated)
    pins = dict(LEGACY_PINS)
    pins.update((int(row["rva"], 0), row["length"]) for row in native_pins)
    segments = []
    for rva, size in sorted(pins.items()):
        actual = pe.read(rva, size)
        segments.append("codeD40.push_back({0x%X,{%s}});" %
                        (rva, ",".join("0x%02x" % b for b in actual)))
    (out / "generated_combat_rotation_pins_d40.inc").write_text("\n".join(segments) + "\n")
    fixture = ROOT / "tests/combat_no_turn_d40_test.cpp"
    compiler = os.environ.get("CXX", "cl" if os.name == "nt" else "g++")
    msvc = Path(compiler).name.lower() in ("cl", "cl.exe")
    binary = out / ("combat_no_turn_d40_test.exe" if os.name == "nt" else "combat_no_turn_d40_test")
    if msvc:
        if sanitize:
            raise ValueError("This runner's combined address/undefined sanitizer mode requires a GNU-compatible compiler.")
        # Run from an x64 VS developer prompt (or VsDevCmd). This builds the
        # host fixture only; it does not link or execute the game DLL.
        command = [compiler, "/nologo", "/std:c++17", "/EHsc", "/Od",
                   "/I" + str(out), "/I" + str(source / "src"), str(fixture),
                   "/Fe" + str(binary), "/Fo" + str(out / "combat_no_turn_d40_test.obj")]
    else:
        command = [compiler, "-std=c++17", "-O0", "-g0", "-pthread",
                   "-I" + str(out), "-I" + str(source / "src"), str(fixture), "-o", str(binary)]
    if sanitize:
        command[1:1] = ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
    compiled = subprocess.run(command, text=True, capture_output=True)
    (out / "compile.log").write_text(compiled.stdout + compiled.stderr)
    report = {"source": str(source), "source_sha256": hashlib.sha256(data).hexdigest(),
              "public_source_sha256": hashlib.sha256(public_path.read_bytes()).hexdigest(),
              "fixture_sha256": hashlib.sha256(fixture.read_bytes()).hexdigest(),
              "extracted_sha256": hashlib.sha256(generated.encode()).hexdigest(),
              "production_rotation_present": present, "compile_command": command,
              "compile_exit": compiled.returncode, "run_exit": None, "sanitizers": sanitize,
              "native_execution": False, "windows_build": False, "headset_execution": False,
              "native_leaves": "native mover routine, hook library, clock, bounded process reads, copied ownership mailboxes"}
    if not compiled.returncode:
        env = dict(os.environ)
        if sanitize:
            env["ASAN_OPTIONS"] = "detect_leaks=0"
        ran = subprocess.run([str(binary)], text=True, capture_output=True, env=env)
        (out / "run.log").write_text(ran.stdout + ran.stderr)
        report["run_exit"] = ran.returncode
        summary = re.search(r"D40 actual queued-rotation adapter: (\d+) checks, (\d+) failures;", ran.stdout)
        if summary:
            report["checks"], report["failures"] = map(int, summary.groups())
        print(ran.stdout.splitlines()[-1] if ran.stdout else ran.stderr[:1000])
    else:
        print((compiled.stdout + compiled.stderr)[:5000])
    (out / "results.json").write_text(json.dumps(report, indent=2) + "\n")
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=ROOT)
    parser.add_argument("--baseline", type=Path)
    parser.add_argument("--dll", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--sanitize", action="store_true")
    args = parser.parse_args()
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    pe = PE(args.dll)
    sha = hashlib.sha256(pe.data).hexdigest()
    if sha != EXPECTED_DLL:
        raise SystemExit("Supplied DLL is not the exact verified retail binary; no native pins accepted.")
    source = args.source.resolve()
    pins = rotation_pins((source / "src/subsystems/base/native_motion_owner.inl").read_text(), pe)
    contract = {"dll_sha256": sha, "timestamp": hex(pe.stamp), "image_base": hex(pe.base),
                "pins": pins, "all_match": bool(pins) and all(p["matches"] for p in pins),
                "native_execution": False}
    (out / "native_pins.json").write_text(json.dumps(contract, indent=2) + "\n")
    current = run(source, out / "current", pe, pins, args.sanitize)
    result = {"native_contract": contract, "current": current}
    okay = contract["all_match"] and current["compile_exit"] == 0 and current["run_exit"] == 0
    if args.baseline:
        baseline = run(args.baseline.resolve(), out / "baseline", pe, pins, args.sanitize)
        result["baseline"] = baseline
        # A baseline must actually compile/run, retain protected-context checks,
        # and expose multiple ordinary-action regressions. A crash is not proof.
        negative = (baseline["compile_exit"] == 0 and baseline["run_exit"] == 1 and
                    not baseline["production_rotation_present"] and baseline.get("checks", 0) > 50 and
                    10 <= baseline.get("failures", 0) < baseline.get("checks", 0))
        result["baseline_reproduces_missing_early_veto"] = negative
        okay = okay and negative
    result["passed"] = bool(okay)
    (out / "results.json").write_text(json.dumps(result, indent=2) + "\n")
    print("D40 source adapter and native pins:", "PASS" if okay else "FAIL")
    return 0 if okay else 1


if __name__ == "__main__":
    raise SystemExit(main())
