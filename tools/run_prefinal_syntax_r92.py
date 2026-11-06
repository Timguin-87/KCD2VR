#!/usr/bin/env python3
"""MSVC syntax-only check of actual translation units; no objects, DLL or install."""
from pathlib import Path
import argparse, json, subprocess, hashlib
ROOT = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser()
p.add_argument("--out", required=True)
p.add_argument("--openxr", default=r"E:/kcd2vr-build/840FDD568E4F-dll/_deps/openxr_sdk_source-build/include")
p.add_argument("sources", nargs="+", help="Source-relative .cpp paths")
a = p.parse_args()
out = Path(a.out).resolve(); out.mkdir(parents=True, exist_ok=True)
vs = Path(r"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat")
if not vs.exists(): raise SystemExit("MSVC vcvars64.bat not found")
sdk=Path(a.openxr).resolve()
if not (sdk/"openxr"/"openxr_platform.h").exists(): raise SystemExit("Full OpenXR generated include directory required via --openxr")
results=[]
for item in a.sources:
    source=(ROOT/item).resolve()
    if not source.is_file() or ROOT not in source.parents or source.suffix != ".cpp":
        raise SystemExit(f"Not a source .cpp: {source}")
    stem=source.stem
    cmd=out/(stem+".cmd");log=out/(stem+".log")
    cmd.write_text(f'@echo off\ncall "{vs}" >nul\ncd /d "{ROOT}"\ncl /nologo /std:c++latest /EHsc /Zs /DNOMINMAX /DWIN32_LEAN_AND_MEAN /I src /I deps\\DetourModKit\\include /I "{sdk}" "{source}" > "{log}" 2>&1\nexit /b %errorlevel%\n')
    result=subprocess.run(["cmd","/c",str(cmd)],cwd=out)
    row={"source":source.relative_to(ROOT).as_posix(),"sha256":hashlib.sha256(source.read_bytes()).hexdigest(),"exit_code":result.returncode,"log":str(log)}
    results.append(row);print(f"{row['source']}: {'PASS' if result.returncode==0 else 'FAIL'}",flush=True)
(out/"RESULT.json").write_text(json.dumps({"kind":"MSVC syntax only; no DLL link","results":results},indent=2)+"\n")
raise SystemExit(1 if any(r["exit_code"] for r in results) else 0)
