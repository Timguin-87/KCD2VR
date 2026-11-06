#!/usr/bin/env python3
"""Build the two canonical XR35 fixtures against their real source dependencies.

Only host fixtures run. WHGame.dll, ASI, native game and headset are NOT_RUN.
The embedded read-only H36 query/getter are mechanically compared before build;
only their native/storage leaves are fixture-owned. No constant-free policy.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import time

SHARED_SHA = "41d2e66014197bf3e7f2ebf1fc83b5d7f32a10aa1098a1a441fb7104a345165a"
OLD_MAIN_SHA = "702948e5ef7ea19b2ae40ef36356b8b478a2d9cd34a8f9bb4fd29a99b7b0b37f"
OLD_21_MESSAGES_SHA = "ca32608917a4307ab4365c09d0e24101708b5825940f7300f67c428b676d53e5"
TESTS = ("crossbow_native_xr34", "reload_working_hand_h36")


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")


def marked(text: str, marker: str) -> str:
    pattern = rf"// BEGIN {marker}[^\n]*\n(.*?)// END {marker}"
    found = re.findall(pattern, text, re.S)
    assert len(found) == 1, f"Expected unique fixture marker: {marker}"
    return found[0].strip()


def verify(source: Path) -> dict:
    test = read(source / "tests/crossbow_native_xr34_test.cpp")
    frik = read(source / "src/frik_pose_probe.cpp")
    start = frik.index("ThrowAuthorityT22 ThrowEquipmentAdmissionT25(")
    stop = frik.index("bool ReadThrowModelGripT18(", start)
    assert marked(test, "ACTUAL_RETAINED_EQUIPMENT_QUERY") == frik[start:stop].strip(), \
        "Fixture must use EXACT current retained query (storage leaves may be explicit)"
    body = read(source / "src/subsystems/physics/runtime/native_weapon_body_t4.cpp")
    getter = marked(test, "ACTUAL_H36_BIT4_GETTER")
    assert getter in body, "Fixture bit4 getter differs from actual production getter"
    shared = source / "src/core/mutual_admission_t25.h"
    assert sha(shared.read_bytes()) == SHARED_SHA, "Shared admission contract bytes changed"
    main = test.split("#ifndef KCD2VR_XR34_NATIVE_NO_MAIN", 1)[1]
    assert sha(main.encode()) == OLD_MAIN_SHA, "Existing canonical main/asserts must remain intact"
    h36 = read(source / "tests/reload_working_hand_h36_test.cpp")
    messages = re.findall(r'ckH36\([^;]+,"([^"\n]+)"\);', h36)
    assert sha(json.dumps(messages[:21], separators=(",", ":")).encode()) == OLD_21_MESSAGES_SHA, \
        "Original 21 H36 checks changed or reordered"
    assert 'ckH36(checksH36==21,' in h36, "Original check count guard absent"
    assert '#include "../src/core/mutual_admission_t25.h"' in test
    assert '#include "../src/subsystems/throw_drop/hand_claim_t24.h"' in test
    return {"retained_query_exact": True, "bit4_getter_exact": True,
            "shared_contract_sha256": SHARED_SHA, "original_main_sha256": OLD_MAIN_SHA,
            "original_21_messages_sha256": OLD_21_MESSAGES_SHA,
            "original_21_preserved": True, "h36_total_checks": 39}


def vcvars_path(requested: str | None) -> Path:
    if requested:
        path = Path(requested)
    else:
        vswhere = Path(os.environ.get("ProgramFiles(x86)", "C:/Program Files (x86)")) / \
            "Microsoft Visual Studio/Installer/vswhere.exe"
        if vswhere.is_file():
            install = subprocess.check_output([str(vswhere), "-latest", "-products", "*",
                "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
                "-property", "installationPath"], text=True).strip()
            path = Path(install) / "VC/Auxiliary/Build/vcvars64.bat"
        else:
            path = Path("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat")
    assert path.is_file(), "Pass --vcvars pointing to an installed VS2022 vcvars64.bat"
    return path


def dependency_receipt(source: Path, folder: Path, test_path: Path) -> dict:
    data = json.loads((folder / "inputs.json").read_text(encoding="utf-8-sig"))["Data"]
    assert Path(data["Source"]).resolve() == test_path.resolve(), "Wrong compiled fixture source"
    dependencies = [test_path] + [Path(p) for p in data["Includes"]]
    own = {}
    root = str(source.resolve()).casefold() + os.sep
    exe_time = (folder / "test.exe").stat().st_mtime_ns
    for path in dependencies:
        if str(path.resolve()).casefold().startswith(root):
            assert path.stat().st_mtime_ns <= exe_time, f"Dependency edited after executable: {path}"
            own[path.resolve().relative_to(source.resolve()).as_posix()] = sha(path.read_bytes())
    for expected in ("src/core/mutual_admission_t25.h",
            "src/subsystems/throw_drop/hand_claim_t24.h",
            "src/subsystems/ranged_reload/crossbow_native_xr34.inl"):
        assert expected.casefold() in {p.casefold() for p in own}, f"Missing actual dependency: {expected}"
    return {"source_dependencies": own,
            "source_dependencies_report_sha256": sha((folder / "inputs.json").read_bytes()),
            "test_exe_sha256": sha((folder / "test.exe").read_bytes())}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[1])
    ap.add_argument("--out", required=True, type=Path)
    ap.add_argument("--vcvars")
    ap.add_argument("--reuse-existing", action="store_true",
                    help="Verify existing outputs without rebuilding; requires dependency timestamps and PASS logs")
    args = ap.parse_args()
    source, out = args.source.resolve(), args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    verified = verify(source)
    vcvars = None if args.reuse_existing else vcvars_path(args.vcvars)
    production = ("src/frik_pose_probe.cpp", "src/subsystems/physics/runtime/native_weapon_body_t4.cpp")
    policy_before = {p: sha((source / p).read_bytes()) for p in production}
    runs = []
    for test in TESTS:
        for config in ("Debug", "ASAN"):
            folder = out / f"{test}_{config}"
            folder.mkdir(exist_ok=True)
            test_path = source / f"tests/{test}_test.cpp"
            if not args.reuse_existing:
                sanitizer = "/fsanitize=address " if config == "ASAN" else ""
                cmd = (f'@echo off\ncall "{vcvars}" >nul\n'
                    'if errorlevel 1 exit /b %errorlevel%\n'
                    f'cd /d "{folder}"\n'
                    f'cl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG {sanitizer}'
                    f'/I "{source / "src"}" /sourceDependencies inputs.json "{test_path}" /Fe:test.exe\n'
                    'if errorlevel 1 exit /b %errorlevel%\ntest.exe\nexit /b %errorlevel%\n')
                (folder / "run.cmd").write_text(cmd, encoding="utf-8")
                run = subprocess.run(["cmd", "/d", "/c", str(folder / "run.cmd")],
                                     stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                (folder / "run.log").write_bytes(run.stdout)
                assert run.returncode == 0, f"Compile/run failed: {folder / 'run.log'}"
                (folder / "exit.json").write_text(json.dumps({"exit_code": 0}), encoding="utf-8")
            log = read(folder / "run.log")
            phrase = ("XR34 actual native adapter/clock contracts PASS" if test == TESTS[0] else
                      "XR34 retained working-hand debt: 39 checks")
            assert phrase in log and "error C" not in log and "ERROR: AddressSanitizer" not in log, \
                f"Missing PASS or build failure: {folder / 'run.log'}"
            runs.append({"test": test, "configuration": config, "passed": True,
                "existing_output_verified": args.reuse_existing,
                "log_sha256": sha((folder / "run.log").read_bytes()),
                "command_sha256": sha((folder / "run.cmd").read_bytes()),
                **dependency_receipt(source, folder, test_path)})
    verify(source)
    assert policy_before == {p: sha((source / p).read_bytes()) for p in production}, "Policy moved during run"
    receipt = {"schema": "XR35_CANONICAL_FIXTURE_CLOSURE_R5", "pass": True,
        "source": str(source), "timestamp_unix": time.time(), "checks": verified,
        "native_storage_and_call_leaves": "EXPLICIT_HOST_FIXTURE",
        "native_game": "NOT_RUN", "headset": "NOT_RUN", "runtime_dll": "NOT_BUILT",
        "production_policy_inputs": policy_before,
        "test_inputs": {f"tests/{t}_test.cpp": sha((source / f"tests/{t}_test.cpp").read_bytes()) for t in TESTS},
        "runner_sha256": sha(Path(__file__).read_bytes()), "runs": runs}
    (out / "RECEIPT.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print("XR35 canonical fixture closure PASS: 4/4; original suite intact; H36 original21 plus extensions39")
    print(out / "RECEIPT.json")
    return 0


if __name__ == "__main__":
    sys.exit(main())
