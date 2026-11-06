#!/usr/bin/env python3
"""Host regression checks for D76 retained entry and exact tool regrip.

The production client is unchanged in positive runs. The actual native adapter
uses one explicitly substituted C++ scope for Windows SEH finally on this host;
Windows structured-exception behavior and native animation outcomes are untested.
All generated/shadow sources stay outside the production source tree.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
REGRIP = """                // The native tool remains bound while this reload owns the
                // working hand. A real release/regrip may resume that same
                // read-back prop; it cannot acquire another tool or skip the
                // subsequent string contact and full stroke requirements.
                if(sample&&tool&&state_.toolTakenD72&&bindingCaptured_&&!grasp_&&neutral_&&held){
                    grasp_=true;neutral_=false;
                }
"""


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--compiler", default="g++")
    args = parser.parse_args()
    out = args.out.resolve()
    assert ROOT not in out.parents, "Generated files must remain outside source"
    out.mkdir(parents=True, exist_ok=True)
    adapter_path = ROOT / "src/subsystems/ranged_reload/crossbow_native_xr34.inl"
    header_path = ROOT / "src/subsystems/ranged_reload/crossbow_native_xr34.h"
    adapter = adapter_path.read_text()
    old = """    __try{started=g_crossbow_xr34.startNewAfterRestore(o,speed,now);
        startWhyD69=g_crossbow_xr34.startReasonD69()==xr34::StartReasonD69::CurrentOwner&&g_xr34_current_leaf_d69?
            g_xr34_current_leaf_d69:xr34::startReasonNameD69(g_crossbow_xr34.startReasonD69());}
    __finally{(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);} """.rstrip()
    new = """    { // Host-only C++ cleanup substitute; not Windows SEH validation.
        struct HostFinishD76 {ma::Token& token;bool& started;~HostFinishD76(){(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}} finish{token,started};
        started=g_crossbow_xr34.startNewAfterRestore(o,speed,now);
        startWhyD69=g_crossbow_xr34.startReasonD69()==xr34::StartReasonD69::CurrentOwner&&g_xr34_current_leaf_d69?
            g_xr34_current_leaf_d69:xr34::startReasonNameD69(g_crossbow_xr34.startReasonD69());}"""
    assert adapter.count(old) == 1
    host_adapter = adapter.replace(old, new)
    (out / "crossbow_adapter_host.inc").write_text(host_adapter)
    fixture_names = ["crossbow_native_xr34_test.cpp", "crossbow_entry_intent_d76_test.cpp", "crossbow_regrip_d76_test.cpp"]
    fixtures = {}
    for name in fixture_names:
        code = (ROOT / "tests" / name).read_text().replace('"../src/', '"' + (ROOT / "src").as_posix() + '/')
        code = code.replace('"' + adapter_path.as_posix() + '"', '"' + (out / "crossbow_adapter_host.inc").as_posix() + '"')
        (out / name).write_text(code)
        fixtures[name] = code
    runs = []
    plans = [
        ("retained_entry", "crossbow_entry_intent_d76_test.cpp", ["-O0", "-g"]),
        ("regrip_debug", "crossbow_regrip_d76_test.cpp", ["-O0", "-g"]),
        ("regrip_optimized", "crossbow_regrip_d76_test.cpp", ["-O2"]),
        ("regrip_asan_ubsan", "crossbow_regrip_d76_test.cpp", ["-O1", "-g", "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-no-pie"]),
        ("existing_adapter_regressions", "crossbow_native_xr34_test.cpp", ["-O2"]),
    ]
    for name, fixture, flags in plans:
        binary = out / name
        command = [args.compiler, "-std=c++20", "-pthread", *flags, "-I", str(ROOT / "src"), "-I", str(ROOT / "tests/syntax_shims"), str(out / fixture), "-o", str(binary)]
        built = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (out / (name + "_build.log")).write_text(built.stdout)
        if built.returncode:
            raise RuntimeError(name + " compile failed\n" + built.stdout)
        env = os.environ.copy()
        if "asan" in name:
            env["ASAN_OPTIONS"] = "detect_leaks=0"
        ran = subprocess.run([str(binary)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env)
        (out / (name + ".log")).write_text(ran.stdout)
        runs.append({"name": name, "command": command, "exit_code": ran.returncode, "output": ran.stdout})
        print(name + ": " + ran.stdout, end="")
        if ran.returncode:
            raise RuntimeError(name + " execution failed")
    # Exact causal negative: remove only the new regrip admission, retaining all
    # D76 entry-intent, native binding, clock and cleanup code unchanged.
    header = header_path.read_text()
    assert header.count(REGRIP) == 1
    negative = out / "without_regrip"
    negative.mkdir(exist_ok=True)
    shadow = header.replace(REGRIP, "")
    shadow = re.sub(r'#include "([^"]+)"', lambda match: '#include "' + (header_path.parent / match.group(1)).resolve().as_posix() + '"', shadow)
    (negative / "client.h").write_text(shadow)
    for name, code in fixtures.items():
        code = code.replace('"' + header_path.as_posix() + '"', '"' + (negative / "client.h").as_posix() + '"')
        (negative / name).write_text(code)
    binary = negative / "causal_control"
    command = [args.compiler, "-std=c++20", "-pthread", "-O0", "-g", "-I", str(ROOT / "src"), "-I", str(ROOT / "tests/syntax_shims"), str(negative / "crossbow_regrip_d76_test.cpp"), "-o", str(binary)]
    built = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    (negative / "build.log").write_text(built.stdout)
    if built.returncode:
        raise RuntimeError("Negative control did not compile\n" + built.stdout)
    ran = subprocess.run([str(binary)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    (negative / "run.log").write_text(ran.stdout)
    causal = ran.returncode == 1 and "FAIL_REGRIP" in ran.stdout and "g_crossbow_xr34.status().toolEngagedD72" in ran.stdout
    print("without_regrip: " + ran.stdout, end="")
    inputs = [adapter_path, header_path, Path(__file__).resolve()] + [ROOT / "tests" / name for name in fixture_names]
    report = {
        "passed": causal,
        "runs": runs,
        "causal_negative": {"passed": causal, "exit_code": ran.returncode, "output": ran.stdout, "mutation": "remove only the new instant Span regrip admission"},
        "source_sha256": {str(path.relative_to(ROOT)): hashlib.sha256(path.read_bytes()).hexdigest() for path in inputs},
        "host_adapter_sha256": hashlib.sha256(host_adapter.encode()).hexdigest(),
        "limits": ["One Windows SEH finally is replaced by scoped C++ cleanup in the private host adapter; SEH behavior is not tested.",
                   "Native clock events, prop geometry/placement readback and inventory outcomes are fixture leaves.",
                   "LeakSanitizer disabled on this managed host; ASan/UBSan enabled for the regrip case.",
                   "No headset, native prop-pixel, game executable or DLL execution."],
    }
    (out / "RESULT.json").write_text(json.dumps(report, indent=2) + "\n")
    return 0 if causal else 1


if __name__ == "__main__":
    raise SystemExit(main())
