#!/usr/bin/env python3
"""Compile actual crossbow adapter/client regressions with explicit native leaves.

Only one MSVC SEH cleanup scope is substituted in an external host copy. These
checks exercise production ownership, metadata, FIFO and hand/bolt policies;
they do not validate Windows exception delivery, native animation or headset output.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--compiler", default="g++")
    args = parser.parse_args()
    out = args.out.resolve()
    assert ROOT not in out.parents and out != ROOT
    out.mkdir(parents=True, exist_ok=True)
    header_path = ROOT / "src/subsystems/ranged_reload/crossbow_native_xr34.h"
    adapter_path = header_path.with_suffix(".inl")
    adapter = adapter_path.read_text()
    start = adapter.index("    __try{started=g_crossbow_xr34.startNewAfterRestore(")
    end_text = "__finally{(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}"
    end = adapter.index(end_text, start) + len(end_text)
    body = adapter[start:end]
    assert body.count("__try") == 1 and body.count("__finally") == 1
    host = body.replace("    __try{", "    {struct HostFinishD81 {ma::Token& token;bool& started;~HostFinishD81(){(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}} finish{token,started};", 1)
    host = host.replace("}\n    " + end_text, "}")
    assert "__finally" not in host
    adapted = adapter[:start] + host + adapter[end:]
    (out / "native_adapter_host.inc").write_text(adapted)
    names = ["crossbow_native_xr34", "crossbow_instant_d72", "crossbow_transition_d80", "crossbow_ownership_d81"]
    sources = {}
    for name in names:
        code = (ROOT / "tests" / (name + "_test.cpp")).read_text().replace('"../src/', '"' + (ROOT / "src").as_posix() + '/')
        code = code.replace('"' + adapter_path.as_posix() + '"', '"' + (out / "native_adapter_host.inc").as_posix() + '"')
        sources[name] = code
        (out / (name + "_test.cpp")).write_text(code)
    runs = []

    def build_run(name, suite, directory, flags, expected_failure=False):
        binary = directory / name
        command = [args.compiler, "-std=c++20", "-pthread", *flags,
                   "-I", str(ROOT / "src"), "-I", str(ROOT / "tests/syntax_shims"), "-I", str(out),
                   str(directory / (suite + "_test.cpp")), "-o", str(binary)]
        built = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (directory / (name + "_build.log")).write_text(built.stdout)
        if built.returncode:
            raise RuntimeError(name + " compile failed\n" + built.stdout)
        env = os.environ.copy()
        env["ASAN_OPTIONS"] = "detect_leaks=0"
        ran = subprocess.run([str(binary)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env)
        (directory / (name + ".log")).write_text(ran.stdout)
        passed = (ran.returncode == 1 and "FAIL_D81" in ran.stdout) if expected_failure else ran.returncode == 0
        runs.append({"name": name, "command": command, "exit_code": ran.returncode,
                     "expected_failure": expected_failure, "passed": passed, "output": ran.stdout})
        print(name + ": " + ran.stdout, end="", flush=True)
        if not passed:
            raise RuntimeError(name + " unexpected execution result")

    for suite in names:
        build_run(suite, suite, out, ["-O0", "-g"])
    build_run("ownership_optimized", "crossbow_ownership_d81", out, ["-O2"])
    build_run("ownership_asan_ubsan", "crossbow_ownership_d81", out,
              ["-O1", "-g", "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-no-pie"])
    # These reversions must make the corresponding regression fail. A test that
    # merely repeats the current source text would not detect both failures.
    mutations = {
        "physical_disabled_by_instant": (header_path, "return s.physicalD81||s.instantD72;", "return s.instantD72;"),
        "secondary_scope_aborts": (adapter_path, "nativeScope!=unsigned(scope))return;", "false)return;"),
    }
    for name, (path, old, new) in mutations.items():
        shadow = out / name
        shadow.mkdir(exist_ok=True)
        header = header_path.read_text()
        adapter_host = adapted
        if path == header_path:
            assert header.count(old) == 1
            header = header.replace(old, new)
        else:
            assert adapter_host.count(old) == 1
            adapter_host = adapter_host.replace(old, new)
        header = re.sub(r'#include "([^"]+)"', lambda m: '#include "' + (header_path.parent / m[1]).resolve().as_posix() + '"', header)
        (shadow / "client.h").write_text(header)
        (shadow / "native_adapter_host.inc").write_text(adapter_host)
        for suite, code in sources.items():
            code = code.replace('"' + header_path.as_posix() + '"', '"' + (shadow / "client.h").as_posix() + '"')
            code = code.replace('"' + (out / "native_adapter_host.inc").as_posix() + '"', '"' + (shadow / "native_adapter_host.inc").as_posix() + '"')
            (shadow / (suite + "_test.cpp")).write_text(code)
        build_run(name, "crossbow_ownership_d81", shadow, ["-O0", "-g"], True)
    paths = [header_path, adapter_path, Path(__file__).resolve()]
    paths += [ROOT / "tests" / (name + "_test.cpp") for name in names]
    result = {"passed": all(r["passed"] for r in runs), "runs": runs,
              "source_sha256": {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},
              "limits": ["Retail identity, memory, native API and animation-event outcomes are fixture leaves; game/headset NOT_RUN.",
                         "One Windows SEH finally uses a host-only C++ scope substitute; Windows ABI/SEH NOT_RUN.",
                         "No claim of native inventory, rendered bolt/tool visibility or actual pose-job scheduling.",
                         "ASan and UBSan enabled for D81 cases; LeakSanitizer disabled on this host."]}
    (out / "RESULT.json").write_text(json.dumps(result, indent=2) + "\n")


if __name__ == "__main__":
    main()
