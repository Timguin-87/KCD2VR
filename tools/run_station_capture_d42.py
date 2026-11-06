#!/usr/bin/env python3
"""Compile/run the changed station diagnostic adapters using actual MSVC.

Native selection, attachment and native-return leaves in these fixtures are
explicit host substitutes. Passing is not a recipe/physics/headset result.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
from prepare_dmk_dependency import prepare as prepare_dependency

parser = argparse.ArgumentParser()
parser.add_argument("--out", required=True, type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
out = args.out.resolve()
out.mkdir(parents=True, exist_ok=True)
dmk = prepare_dependency(root / "deps/DetourModKit", out / "_deps/DetourModKit")
if os.name != "nt":
    parser.error("This runner requires Windows/MSVC; use the individual C++ fixtures on other hosts.")
vswhere = Path(os.environ.get("ProgramFiles(x86)", "C:/Program Files (x86)")) / "Microsoft Visual Studio/Installer/vswhere.exe"
found = subprocess.check_output([str(vswhere), "-latest", "-products", "*", "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-property", "installationPath"], text=True).strip()
vcvars = Path(found) / "VC/Auxiliary/Build/vcvars64.bat"
for path in (root, out, vcvars):
    if any(c in str(path) for c in ('"', '%', '\r', '\n')):
        parser.error("CMD-special characters in the source/output/toolchain path are unsupported.")
cases = [
    ("station_capture", "tests/subsystems/alchemy/station_capture_d42_test.cpp", []),
    ("native_contact", "tests/subsystems/smithing/native_contact68_test.cpp", []),
    ("mortar_motion", "tests/subsystems/alchemy/mortar_motion_d41_test.cpp", []),
    ("proximity", "tests/subsystems/alchemy/proximity_craft1_test.cpp", ["/DKCD2VR_CRAFT_PROXIMITY_V1", "/DKCD2VR_CLOSE_PICKUP_D39"]),
    ("alchemy_finish", "tests/alchemy_finish_h41_test.cpp", ["/std:c++latest", "/MD", "/Gy", "/D_DISABLE_STRING_ANNOTATION", "/D_DISABLE_VECTOR_ANNOTATION", "/I" + str(dmk / "include")]),
]
script = ["@echo off", f'call "{vcvars}" >nul', "if errorlevel 1 exit /b 1"]
for name, relative, extra in cases:
    command = ["cl", "/nologo", "/std:c++20", "/EHsc", "/W3", "/I" + str(root / "src"), *extra,
               str(root / relative), "/Fo" + str(out / (name + ".obj")), "/Fe" + str(out / (name + ".exe"))]
    if name == "alchemy_finish":
        command += ["/link", "/INCREMENTAL:NO", "/OPT:REF"]
        command += [str(dmk / "lib" / lib) for lib in ("libDetourModKit.lib", "safetyhook.lib", "Zydis.lib", "Zycore.lib")]
        command += ["bcrypt.lib", "User32.lib", "Shell32.lib", "Ole32.lib", "Advapi32.lib", "Version.lib", "Dbghelp.lib"]
    script.extend([subprocess.list2cmdline(command), "if errorlevel 1 exit /b 1", f'"{out / (name + ".exe")}"', "if errorlevel 1 exit /b 1"])
script.append("exit /b 0")
cmd = out / "run.cmd"
cmd.write_text("\n".join(script) + "\n", encoding="utf-8")
run = subprocess.run([os.environ.get("COMSPEC", "cmd.exe"), "/d", "/c", str(cmd)], cwd=out, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
(out / "results.txt").write_text(run.stdout, encoding="utf-8")
manifest = {"exit_code": run.returncode, "compiler": "MSVC vcvars64", "native_engine_and_headset": "NOT_RUN",
            "tests": [{"source": relative, "sha256": hashlib.sha256((root / relative).read_bytes()).hexdigest()} for _, relative, _ in cases]}
(out / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
print(run.stdout, end="")
raise SystemExit(run.returncode)
