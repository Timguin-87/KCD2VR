"""Run the literal native impale coordinator; optional R96 source control, no Git required."""
from pathlib import Path
import argparse
import hashlib
import json
import os
import re
import subprocess
import sys

from host_compiler import command, executable_path

ROOT = Path(__file__).resolve().parents[1]
NATIVE = Path("src/subsystems/gore/native_impale_part.inl")
R96_SHA256 = "588cf48b9852f9743449f8d68d4e03722053170eb063d777dd891ff6159bd29b"


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def dependencies(paths):
    pending = list(paths)
    found = set()
    while pending:
        path = pending.pop().resolve()
        if path in found:
            continue
        found.add(path)
        for name in re.findall(r'^\s*#\s*include\s*"([^"]+)"', path.read_text(encoding="utf-8"), re.M):
            for candidate in (path.parent / name, ROOT / "src" / name, ROOT / "tests" / name):
                if candidate.is_file():
                    pending.append(candidate)
                    break
    return {str(path.relative_to(ROOT)): digest(path) for path in sorted(found) if ROOT in path.parents}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--compiler", default=None)
    parser.add_argument("--baseline-source", type=Path,
                        help="Optional original R96 native_impale_part.inl file or complete source directory")
    args = parser.parse_args()
    out = args.out.resolve()
    if out == ROOT or ROOT in out.parents:
        parser.error("Evidence must be outside the source tree")
    out.mkdir(parents=True, exist_ok=True)
    entries = [ROOT / NATIVE, ROOT / "tests/impale_native_pending_r96r1_test.cpp",
               ROOT / "tests/impale_successor_h18_coordinator_test.cpp",
               ROOT / "tests/impale_native_eligibility_r93_fixture.h", Path(__file__), ROOT / "tools/host_compiler.py"]
    before = dependencies(entries)
    current = (ROOT / NATIVE).read_bytes()
    baseline = None
    baseline_path = None
    if args.baseline_source:
        baseline_path = args.baseline_source.resolve()
        if baseline_path.is_dir():
            baseline_path /= NATIVE
        baseline = baseline_path.read_bytes()
        if hashlib.sha256(baseline).hexdigest() != R96_SHA256:
            raise RuntimeError("Baseline must match the recorded b226426 R96 native coordinator bytes")

    original_fixture = (ROOT / "tests/impale_successor_h18_coordinator_test.cpp").read_text()
    fixture = original_fixture[:original_fixture.index("int main(){")]
    old_include = '#include "../src/subsystems/gore/native_impale_part.inl"'
    if fixture.count(old_include) != 1:
        raise RuntimeError("Cannot locate the literal production include in coordinator fixture")
    fixture = fixture.replace(old_include, '#include "native_impale_part_r96r1_actual.inc"')
    clock = "static std::uint64_t nowH18=1000;static std::uint64_t GetTickCount64(){return nowH18;}"
    hook = """static std::uint64_t nowH18=1000;static void (*clockReadHookR96R1)()=nullptr;
static std::uint64_t GetTickCount64(){const auto value=nowH18;
 if(clockReadHookR96R1){const auto fn=clockReadHookR96R1;clockReadHookR96R1=nullptr;fn();}return value;}"""
    if fixture.count(clock) != 1:
        raise RuntimeError("Cannot locate fixture clock leaf")
    fixture = fixture.replace(clock, hook)
    rows = []

    def build_run(name, code, mode="optimized", cases=("all",), negative=False, legacy=False):
        folder = out / name
        folder.mkdir(exist_ok=True)
        actual = folder / "native_impale_part_r96r1_actual.inc"
        actual.write_bytes(code)
        (folder / "impale_coordinator_r96r1_fixture.inc").write_text(fixture)
        test = ROOT / "tests/impale_native_pending_r96r1_test.cpp"
        if legacy:
            test = folder / "legacy.cpp"
            test.write_text('#include "impale_native_eligibility_r93_fixture.h"\n#include "' +
                            (ROOT / "tests/impale_successor_h18_coordinator_test.cpp").as_posix() + '"\n')
        exe = executable_path(folder / "test")
        cmd = command(test, exe, [folder, ROOT / "src", ROOT / "tests"], args.compiler)
        msvc = Path(cmd[0]).name.lower() in ("cl", "cl.exe")
        if mode == "debug":
            cmd = [("/Od" if msvc else "-O0") if flag in ("/O2", "-O2") else flag for flag in cmd]
        if mode == "asan_ubsan":
            if msvc:
                cmd.insert(1, "/fsanitize=address")
            else:
                cmd = ["-O1" if flag == "-O2" else flag for flag in cmd]
                cmd[1:1] = ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
                if sys.platform.startswith("linux"):
                    cmd.insert(1, "-no-pie")
        built = subprocess.run(cmd, capture_output=True, text=True)
        (folder / "build.log").write_text(built.stdout + built.stderr)
        if built.returncode:
            rows.append(dict(name=name, command=cmd, compile_rc=built.returncode,
                             expected_failure=negative, passed=False))
            print(name + ": COMPILE_FAILED", flush=True)
            return
        if os.name != "nt":
            exe.chmod(exe.stat().st_mode | 0o100)
        env = dict(os.environ)
        env["ASAN_OPTIONS"] = "halt_on_error=1:detect_leaks=0"
        env["UBSAN_OPTIONS"] = "halt_on_error=1:print_stacktrace=1"
        failures = {
            "publication": "FAIL_IMPALE_NATIVE_R96R1 same pending request resumes after publication mismatch",
            "clock": "FAIL_IMPALE_NATIVE_R96R1 post-entry publication is not consumed as clock rewind",
        }
        for case in cases:
            result = subprocess.run([str(exe), case], capture_output=True, text=True, env=env)
            (folder / (case + ".log")).write_text(result.stdout + result.stderr)
            row = dict(name=name, case=case, mode=mode, command=cmd, compile_rc=built.returncode,
                       source_sha256=hashlib.sha256(code).hexdigest(),
                       production_include_sha256=digest(actual), run_rc=result.returncode,
                       expected_failure=negative, stdout=result.stdout, stderr=result.stderr, passed=False)
            if negative:
                row["expected_assertion"] = failures[case]
                row["passed"] = result.returncode == 1 and result.stdout.strip() == failures[case] and not result.stderr.strip()
            elif legacy:
                row["passed"] = result.returncode == 0 and "PASS H18 actual coordinator" in result.stdout and not result.stderr.strip()
            else:
                match = re.fullmatch(r"PASS_IMPALE_NATIVE_R96R1 checks=(\d+) case=" + case + r"; actual coordinator, native/time/status leaves controlled\n", result.stdout)
                row["passed"] = result.returncode == 0 and match is not None and not result.stderr.strip()
                row["checks"] = int(match.group(1)) if match else None
            rows.append(row)
            print(name + "/" + case + ": " + ("PASS" if row["passed"] else "FAIL"), flush=True)

    build_run("debug", current, "debug")
    build_run("optimized", current)
    build_run("asan_ubsan", current, "asan_ubsan")
    build_run("retained_h18", current, legacy=True)
    if baseline is not None:
        build_run("original_r96_negative", baseline, cases=("publication", "clock"), negative=True)
    after = dependencies(entries)
    report = dict(
        passed=bool(rows) and all(row["passed"] for row in rows) and before == after,
        runs=rows, source_sha256=after, sources_stable_during_run=before == after,
        baseline_source=str(baseline_path) if baseline_path else None,
        baseline_source_sha256=hashlib.sha256(baseline).hexdigest() if baseline is not None else None,
        original_source_controls_run=baseline is not None,
        fixture_transforms=["Use the existing coordinator fixture prefix before its legacy main.",
                            "Point its production include at a byte-identical current or original native coordinator.",
                            "Add a deterministic callback to the existing clock leaf to model publication after a clock read."],
        limitations=["Actual full native coordinator executes with controlled native owner, body/status, clock and action leaves; no game, DLL/link or headset execution.",
                     "Only an admitted, unanchored, never-pinned wait is retained through the qualified nonzero publication mismatch. Zero ready, unavailable reads and true source/context/owner changes retain their existing retirement behavior.",
                     "ASan and UBSan run on GCC; LeakSanitizer is disabled because this managed environment cannot inspect task state under ptrace. MSVC supports ASan only.",
                     "Sparse Blade/Tip contact selection, damage/energy thresholds and native part mapping are unchanged."],
    )
    (out / "RESULT.json").write_text(json.dumps(report, indent=2) + "\n")
    return 0 if report["passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
