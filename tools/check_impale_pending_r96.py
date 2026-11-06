"""R96 actual-kernel late impale recovery and verbatim original-source control."""
from pathlib import Path
import argparse
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys

from host_compiler import command, executable_path

ROOT = Path(__file__).resolve().parents[1]
HEADER = Path("src/vrik/impale_pending59.h")
ORIGINAL_SHA256 = "c3172a63e5c49329a53d80e2737a07d2bb5a73d90989bf4e1220b915ea8ef4cd"


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def original_header(current):
    # Reconstruct and verify the byte-for-byte R95 source, including CRLF.
    # A failed digest is a failed fixture setup, never a successful negative.
    restored = current.replace(b'#include "impale47.h"\r\n', b'')
    restored = restored.replace(
        b'     // Recovery retains only the original H18 request lifetime; restore never renews it.\r\n', b'')
    restored = restored.replace(b'>impale47::kWeaponWaitH18||', b'>1700||')
    if hashlib.sha256(restored).hexdigest() != ORIGINAL_SHA256:
        raise RuntimeError("Original-source reconstruction differs from recorded R95 bytes")
    return restored


def local_dependencies(entry):
    """All transitive quoted includes: a conservative source dependency set."""
    pending = list(entry)
    visited = set()
    while pending:
        path = pending.pop().resolve()
        if path in visited:
            continue
        visited.add(path)
        for name in re.findall(r'^\s*#\s*include\s*"([^"]+)"', path.read_text(encoding="utf-8"), re.M):
            for candidate in (path.parent / name, ROOT / "src" / name, ROOT / "tests" / name):
                if candidate.is_file():
                    pending.append(candidate)
                    break
    return {str(p.relative_to(ROOT)): sha256(p) for p in sorted(visited) if ROOT in p.parents}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--compiler", default=None)
    args = parser.parse_args()
    out = args.out.resolve()
    if out == ROOT or ROOT in out.parents:
        parser.error("Evidence must be outside the source tree")
    out.mkdir(parents=True, exist_ok=True)
    baseline = out / "original_source"
    baseline.mkdir(exist_ok=True)
    original = original_header((ROOT / HEADER).read_bytes())
    (baseline / "impale_pending59.h").write_bytes(original)
    # The negative compiles the same literal production kernel and fixture,
    # changing only this exact header. No reference implementation is involved.
    shadow = out / "original_source_tree"
    shutil.copytree(ROOT / "src", shadow / "src", dirs_exist_ok=True)
    (shadow / HEADER).write_bytes(original)
    entries = [ROOT / "tests/impale_pending_r96_test.cpp", ROOT / "tests/impale_pending59_test.cpp"]
    before = local_dependencies(entries)
    before["tools/check_impale_pending_r96.py"] = sha256(Path(__file__))
    before["tools/host_compiler.py"] = sha256(ROOT / "tools/host_compiler.py")
    rows = []

    def run(name, fixture, mode, source_root=ROOT, negative=False):
        folder = out / name
        folder.mkdir(exist_ok=True)
        exe = executable_path(folder / "test")
        cmd = command(ROOT / "tests" / fixture, exe, [source_root / "src", ROOT / "tests"], args.compiler)
        msvc = Path(cmd[0]).name.lower() in ("cl", "cl.exe")
        if mode == "debug":
            cmd = [("/Od" if msvc else "-O0") if flag in ("/O2", "-O2") else flag for flag in cmd]
        if mode == "asan_ubsan":
            if msvc:
                cmd.insert(1, "/fsanitize=address")
            else:
                cmd = ["-O1" if flag == "-O2" else flag for flag in cmd]
                cmd[1:1] = ["-g", "-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
                if sys.platform.startswith("linux"):
                    cmd.insert(1, "-no-pie")
        built = subprocess.run(cmd, capture_output=True, text=True)
        (folder / "build.log").write_text(built.stdout + built.stderr)
        row = dict(name=name, fixture=fixture, mode=mode, command=cmd,
                   compile_rc=built.returncode, expected_failure=negative, passed=False)
        if built.returncode == 0:
            env = dict(os.environ)
            env["ASAN_OPTIONS"] = "halt_on_error=1:detect_leaks=0"
            env["UBSAN_OPTIONS"] = "halt_on_error=1:print_stacktrace=1"
            result = subprocess.run([str(exe)], capture_output=True, text=True, env=env)
            (folder / "run.log").write_text(result.stdout + result.stderr)
            row.update(run_rc=result.returncode, stdout=result.stdout, stderr=result.stderr)
            expected = "FAIL_IMPALE_PENDING_R96 late recovery retains original request after1700ms"
            if negative:
                row["passed"] = result.returncode == 1 and result.stdout.strip() == expected and not result.stderr.strip()
                row["expected_assertion"] = expected
            elif fixture == "impale_pending_r96_test.cpp":
                matched = re.fullmatch(r"PASS_IMPALE_PENDING_R96 assertions=(\d+)\n", result.stdout)
                row["passed"] = result.returncode == 0 and matched is not None and not result.stderr.strip()
                row["assertions"] = int(matched.group(1)) if matched else None
            else:
                row["passed"] = result.returncode == 0 and "PASS C59 actual kernel" in result.stdout and not result.stderr.strip()
                row["assertions"] = sum(line.startswith("PASS ") and not line.startswith("PASS C59 ")
                                        for line in result.stdout.splitlines())
        rows.append(row)
        print(name + ": " + ("PASS" if row["passed"] else "FAIL"), flush=True)

    run("debug", "impale_pending_r96_test.cpp", "debug")
    run("optimized", "impale_pending_r96_test.cpp", "optimized")
    run("asan_ubsan", "impale_pending_r96_test.cpp", "asan_ubsan")
    run("legacy_pending", "impale_pending59_test.cpp", "optimized")
    run("original_1700ms_negative", "impale_pending_r96_test.cpp", "optimized", shadow, True)
    after = local_dependencies(entries)
    after["tools/check_impale_pending_r96.py"] = sha256(Path(__file__))
    after["tools/host_compiler.py"] = sha256(ROOT / "tools/host_compiler.py")
    report = dict(
        passed=bool(rows) and all(row["passed"] for row in rows) and before == after,
        results=rows, source_sha256=after, sources_stable_during_run=before == after,
        original_source_sha256=ORIGINAL_SHA256,
        original_source_path=str(baseline / "impale_pending59.h"),
        changed_production=[str(HEADER)],
        limits=[
            "Compiles and executes actual Pending and physical::step capture/recovery/reset/rejoin callers.",
            "Native hit outcomes, tracking, and sweep replies are deterministic fixture inputs; no native memory, DLL, game, or headset validation.",
            "The original R95 header must match its recorded SHA256 and compile successfully before its exact late-recovery assertion failure counts.",
            "This fixes the admitted pending-request lifetime only. It neither changes feature attribution nor makes historical serial11 Blade or low-energy evidence eligible.",
        ],
    )
    (out / "RESULT.json").write_text(json.dumps(report, indent=2) + "\n")
    return 0 if report["passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
