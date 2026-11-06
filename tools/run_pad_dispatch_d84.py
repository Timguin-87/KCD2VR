"""Compile literal pad publisher/dispatcher, with interleaved producer calls.

The optional immutable preimage must compile and fail an explicit coherence
assertion. Compile errors, process crashes and unrelated failures do not count.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess


def function(text, marker):
    at = text.index(marker)
    brace = text.index('{', at)
    depth = 1
    end = brace + 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}')
        end += 1
    return text[at:end]


def extract(root, destination):
    p = root / 'src/subsystems/hud_ui_controls/runtime/native_input_look_lab.cpp'
    text = p.read_text(encoding='utf-8-sig')
    anchor = 'kcd2vr::core::SnapshotMailbox<' if 'g_pad_snapshotD84' in text else 'std::atomic<float> g_pad_lx'
    start = text.index(anchor)
    globals_text = text[start:text.index('std::atomic<bool> g_pad_stick_keys', start)]
    segments = [globals_text, function(text, 'void PublishGamepadState('), function(text, 'void flush_gamepad_same_dispatch_thread(')]
    destination.mkdir(parents=True, exist_ok=True)
    literal = '\n\n'.join(segments) + '\n'
    (destination / 'pad_dispatch_source.inc').write_text(literal)
    (destination / 'source.json').write_text(json.dumps({'source': str(p), 'sha256': hashlib.sha256(p.read_bytes()).hexdigest(), 'extracted_sha256': hashlib.sha256(literal.encode()).hexdigest(), 'scope': 'Literal producer, pad publication storage, native flush; clock and emit leaves controlled. Real XI packing and bounded snapshot mailbox.'}, indent=2) + '\n')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--baseline', type=Path)
    parser.add_argument('--compiler', default='g++')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    out = args.output.resolve()
    rows = []
    for label, source in [('current', root)] + ([('baseline', args.baseline.resolve())] if args.baseline else []):
        if out == source or source in out.parents:
            parser.error('Evidence output must be outside the source and baseline')
        extract(source, out / label)
        configs = [('debug', ['-O0']), ('release', ['-O2']), ('sanitized', ['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-no-pie'])] if label == 'current' else [('debug', ['-O0'])]
        for name, flags in configs:
            binary = out / label / name
            cmd = [args.compiler, '-std=c++20', '-UNDEBUG', '-pthread', *flags, '-I', str(source / 'src'), '-I', str(out / label), str(root / 'tests/pad_dispatch_d84_test.cpp'), '-o', str(binary)]
            build = subprocess.run(cmd, capture_output=True, text=True)
            binary.with_suffix('.build.log').write_text(build.stdout + build.stderr)
            row = {'label': label, 'name': name, 'command': cmd, 'build_rc': build.returncode, 'passed': False}
            if build.returncode == 0:
                env = dict(os.environ)
                if name == 'sanitized':
                    # This workspace restricts /proc process inspection used by
                    # LSan. ASan bounds/UAF and UBSan stay enabled.
                    env['ASAN_OPTIONS'] = 'detect_leaks=0'
                run = subprocess.run([str(binary)], capture_output=True, text=True, env=env)
                binary.with_suffix('.run.log').write_text(run.stdout + run.stderr)
                row.update(run_rc=run.returncode, stdout=run.stdout, stderr=run.stderr)
                row['passed'] = run.returncode == 0 if label == 'current' else run.returncode == 1 and 'FAIL forward-to-side-B interleave=' in run.stdout and 'coherent-dispatch' in run.stdout and not run.stderr
            rows.append(row)
            print(f'{label}/{name}: {"PASS" if row["passed"] else "FAIL"}', flush=True)
    (out / 'results.json').write_text(json.dumps(rows, indent=2) + '\n')
    return 0 if all(r['passed'] for r in rows) else 1


if __name__ == '__main__':
    raise SystemExit(main())
