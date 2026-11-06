#!/usr/bin/env python3
"""Run production map Reader/Capture and existing D75 quest protocol checks."""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--compiler", default="g++")
    args = parser.parse_args()
    out = args.out.resolve()
    assert out != ROOT and ROOT not in out.parents
    out.mkdir(parents=True, exist_ok=True)
    path = ROOT / "src/subsystems/vrmap/runtime/held_map_feed_t2.cpp"
    source = path.read_text()
    start = source.index("struct Reader {")
    end = source.index("\nbool Flag(", start)
    reader = source[start:end]
    (out / "map_reader_d81_actual.inc").write_text(reader)
    test = ROOT / "tests/map_reader_d81_test.cpp"
    (out / test.name).write_text(test.read_text())
    runs = []

    def build_run(name, source_path, directory, flags, expected_failure=False):
        binary = directory / name
        command = [args.compiler, "-std=c++20", "-O0", *flags, "-I", str(ROOT / "src"),
                   "-I", str(directory), str(source_path), "-o", str(binary)]
        built = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (directory / (name + "_build.log")).write_text(built.stdout)
        if built.returncode:
            raise RuntimeError(name + " compile failed\n" + built.stdout)
        env = os.environ.copy();env["ASAN_OPTIONS"] = "detect_leaks=0"
        result = subprocess.run([str(binary)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env)
        (directory / (name + ".log")).write_text(result.stdout)
        passed = (result.returncode == 1 and "FAIL_MAP_READER_D81" in result.stdout) if expected_failure else result.returncode == 0
        runs.append({"name": name, "command": command, "exit_code": result.returncode,
                     "expected_failure": expected_failure, "passed": passed, "output": result.stdout})
        print(name + ": " + result.stdout, end="", flush=True)
        if not passed:raise RuntimeError(name + " unexpected result")

    build_run("reader", out / test.name, out, ["-g"])
    build_run("reader_sanitized", out / test.name, out,
              ["-O1", "-g", "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-no-pie"])
    for suite in ["map_quest_d75", "map_quest_service_d75"]:
        build_run(suite, ROOT / "tests" / (suite + "_test.cpp"), out, [])
    negative = out / "repeated_query"
    negative.mkdir(exist_ok=True)
    needle = "        __try{std::memcpy(to,reinterpret_cast<void*>(from),size);return true;}"
    assert reader.count(needle) == 1
    shadow = reader.replace(needle, "        MEMORY_BASIC_INFORMATION page{};(void)VirtualQuery(reinterpret_cast<void*>(from),&page,sizeof(page));\n" + needle)
    (negative / "map_reader_d81_actual.inc").write_text(shadow)
    (negative / test.name).write_text(test.read_text())
    build_run("repeated_query", negative / test.name, negative, [], True)
    paths = [path, ROOT / "src/held_map_snapshot_t2.h", test, Path(__file__).resolve(),
             ROOT / "tests/map_quest_d75_test.cpp", ROOT / "tests/map_quest_service_d75_test.cpp"]
    result = {"passed": all(x["passed"] for x in runs), "runs": runs,
              "source_sha256": {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},
              "limits": ["Production Reader and Capture operate on allocated fixture bytes; production native API outcomes are not measured.",
                         "Host exception macros are syntax substitutes; Windows access-violation containment NOT_RUN.",
                         "170ms hitch attribution to 500ms quest capture is an inference, not a native sampled profile.",
                         "Post-fix FPS, latency and headset visibility NOT_RUN; freshness thresholds unchanged."]}
    (out / "RESULT.json").write_text(json.dumps(result, indent=2) + "\n")


if __name__ == "__main__":
    main()
