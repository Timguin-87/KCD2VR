#!/usr/bin/env python3
"""Compile the actual native alignment predicates against bounded synthetic memory.

No game DLL is loaded and no Windows/native execution is implied. --source can
select the immutable D36 tree to reproduce its tracked-boxing rejection.
"""
import argparse, hashlib, json, os, pathlib, subprocess

ROOT = pathlib.Path(__file__).resolve().parents[1]
FUNCTIONS = ("CurrentOrdinaryMotion59c3", "NativeFistMotionD37",
             "NativeMotionContext112", "NativeAlignmentOwned112",
             "NativePositionCommandOwned36", "NativePositionDispatch36")

def extract(text, name):
    marker = name + "("
    position = text.find(marker)
    if position < 0:
        if name == "NativeFistMotionD37":
            return "" # Historical baseline deliberately has no boxing owner.
        raise ValueError("Missing production function " + name)
    start = text.rfind("\nstatic ", 0, position) + 1
    brace = text.index("{", position)
    depth = 1
    end = brace + 1
    while depth:
        if text[end] == "{": depth += 1
        elif text[end] == "}": depth -= 1
        end += 1
    return text[start:end] + "\n"

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--source", type=pathlib.Path, default=ROOT)
    parser.add_argument("--out", type=pathlib.Path, required=True)
    args = parser.parse_args()
    source = args.source.resolve()
    out = args.out.resolve(); out.mkdir(parents=True, exist_ok=True)
    path = source / "src/subsystems/base/native_motion_owner.inl"
    data = path.read_bytes(); text = data.decode()
    generated = "\n".join(extract(text, n) for n in FUNCTIONS)
    (out / "generated_fist_alignment_d37.inc").write_text(generated)
    binary = out / "fist_alignment_d37_test"
    command = [os.environ.get("CXX", "g++"), "-std=c++17", "-O0", "-g0",
               "-I" + str(out), "-I" + str(source / "src"),
               str(ROOT / "tests/fist_alignment_d37_test.cpp"), "-pthread", "-o", str(binary)]
    compiled = subprocess.run(command, capture_output=True, text=True)
    (out / "compile.log").write_text(compiled.stdout + compiled.stderr)
    report = {"source": str(source), "source_sha256": hashlib.sha256(data).hexdigest(),
              "fixture_sha256": hashlib.sha256((ROOT / "tests/fist_alignment_d37_test.cpp").read_bytes()).hexdigest(),
              "extracted_sha256": hashlib.sha256(generated.encode()).hexdigest(),
              "compile_command": command, "run_command": [str(binary)],
              "compile_exit": compiled.returncode, "run_exit": None, "native_execution": False}
    if not compiled.returncode:
        binary.chmod(0o700)
        ran = subprocess.run([str(binary)], capture_output=True, text=True)
        (out / "run.log").write_text(ran.stdout + ran.stderr)
        report["run_exit"] = ran.returncode
        print(ran.stdout + ran.stderr, end="")
    print(json.dumps(report))
    (out / "results.json").write_text(json.dumps(report, indent=2) + "\n")
    return compiled.returncode or report["run_exit"] or 0

if __name__ == "__main__":
    raise SystemExit(main())
