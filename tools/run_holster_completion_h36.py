"""Compile the production-composed H36 fixture with MSVC; never runs the game."""
from pathlib import Path
import argparse
import hashlib
import json
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("output", type=Path)
parser.add_argument("--asan", action="store_true")
parser.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[1])
args = parser.parse_args()
source, output = args.source.resolve(), args.output.resolve()
output.mkdir(parents=True, exist_ok=True)
vswhere = Path("C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe")
vs = subprocess.check_output([str(vswhere), "-latest", "-products", "*", "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-property", "installationPath"], text=True).strip()
if not vs:
    raise SystemExit("No installed MSVC toolchain")
inputs = ["tests/holster_completion_h36_bridge.cpp", "tests/bridge_external_stubs112.cpp", "src/subsystems/doors/module.cpp"]
flags = "/fsanitize=address /Zi " if args.asan else ""
command = '\n'.join([
    "@echo off",
    f'call "{Path(vs) / "VC/Auxiliary/Build/vcvars64.bat"}" >nul',
    "if errorlevel 1 exit /b %errorlevel%",
    f'cd /d "{output}"',
    'cl /nologo /std:c++latest /EHsc /Gy /Gw /bigobj /UNDEBUG /D_ReturnAddress=HostReturnAddress /DH36R1_HAS_COMPLETION=1 /DH36R2_HAS_INITIAL_OWNER=1 '
    + flags + f'/FI"{source / "tests/host_test_runtime.h"}" '
    + ' '.join(f'/I "{source / p}"' for p in ["tests/syntax_shims", "tests/openxr_sdk", "src"])
    + ' ' + ' '.join(f'"{source / p}"' for p in inputs)
    + ' /Fetest.exe /link /OPT:REF /INCREMENTAL:NO /STACK:33554432',
    "if errorlevel 1 exit /b %errorlevel%", "test.exe", "exit /b %errorlevel%", ""
])
(output / "build.cmd").write_text(command)
proc = subprocess.run(["cmd", "/c", str(output / "build.cmd")], capture_output=True, text=True)
receipt = {"source": str(source), "asan": args.asan, "exit": proc.returncode,
           "stdout": proc.stdout, "stderr": proc.stderr, "retail_native_execution": False,
           "inputs": {p: hashlib.sha256((source / p).read_bytes()).hexdigest() for p in inputs}}
(output / "RESULT.json").write_text(json.dumps(receipt, indent=2) + "\n")
print(json.dumps(receipt, indent=2))
raise SystemExit(proc.returncode)
