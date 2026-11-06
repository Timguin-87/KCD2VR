"""Compiler selection for portable host tests; run cl from a VS developer shell."""
from pathlib import Path
import os

def command(source, executable, includes, compiler=None):
    compiler = compiler or os.environ.get("CXX", "g++")
    if Path(compiler).name.lower() in ("cl", "cl.exe"):
        return [compiler, "/nologo", "/std:c++20", "/O2", "/EHsc", "/UNDEBUG", "/DNOMINMAX",
                "/utf-8", *["/I" + str(p) for p in includes], str(source),
                "/Fo" + str(executable.with_suffix(".obj")), "/Fe" + str(executable)]
    return [compiler, "-std=c++20", "-O2", "-UNDEBUG", "-pthread",
            *[arg for p in includes for arg in ("-I", str(p))], str(source), "-o", str(executable)]

def executable_path(path):
    return path.with_suffix(".exe") if os.name == "nt" else path
