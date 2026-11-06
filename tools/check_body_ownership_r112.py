#!/usr/bin/env python3
"""R112 literal chest-bias and stowed-packet regression, with R111 negative control.

Native memory/renderer and the rest of the coherent solver are not emulated.
The tested math, production torso block, stowed helper and packet writer are
copied without rewriting their behavior. Run with --skip-baseline in an export
without Git history, or give --baseline-source for an unpacked R111 source.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]
BASELINE = 'c2a696b'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def block(text, marker):
    start = text.index(marker)
    opening = text.index('{', start)
    depth = 0
    for i in range(opening, len(text)):
        if text[i] == '{':
            depth += 1
        elif text[i] == '}':
            depth -= 1
            if not depth:
                return text[start:i + 1]
    raise ValueError(marker)


def extract(source, out, baseline=False, baseline_source=None):
    out.mkdir(parents=True, exist_ok=True)
    files = {}
    def read(path, optional=False):
        if baseline and baseline_source is None:
            result = subprocess.run(['git', 'show', BASELINE + ':' + path], cwd=source,
                                    capture_output=True)
            if result.returncode:
                if optional:
                    return ''
                raise RuntimeError(result.stderr.decode())
            data = result.stdout
        else:
            p = (baseline_source if baseline else source) / path
            if not p.exists() and optional:
                return ''
            data = p.read_bytes()
        files[path] = sha(data)
        return data.decode().replace('\r\n', '\n')
    frik = read('src/frik_pose_probe.cpp')
    coherent = read('src/vrik/coherent_pose.inl')
    torso = read('src/subsystems/vrik/pose_torso_stage.inl')
    anatomy = read('src/subsystems/vrik/pose_anatomy.inl')
    helper = read('src/subsystems/vrik/pose_stowed_reference_r112.inl', optional=True)
    math = '\n'.join(block(frik, marker) + (';' if marker.startswith('struct ') else '')
                     for marker in ('struct QuatT28 ', 'struct V3 ', 'struct Q4 ',
                                    'static V3 Add(', 'static V3 Sub(', 'static V3 Mul(',
                                    'static float Dot(', 'static V3 Cross(', 'static float Len(',
                                    'static V3 Normalize(', 'static V3 FromT(', 'static void ToT(',
                                    'static Q4 QConj(', 'static Q4 QMul(', 'static Q4 QNormalize(',
                                    'static Q4 QFromQuatT(', 'static void QToQuatT(',
                                    'static V3 QRotateV(', 'static QuatT28 RelativeFromAbsolute(',
                                    'static QuatT28 AbsoluteFromRelative('))
    math += '\n' + block(coherent, 'static bool ValidPose6689(') + '\n'
    first = torso.index('        const bool natural128=')
    end = torso.index('        const double baseChest169r2=', first)
    chest = torso[first:end]
    publication = block(coherent, '    if(tx.stowedFullD63)for(')
    anatomy_wiring = '    BuildStowedReferenceR112(s,rig);'
    if helper:
        if anatomy.count(anatomy_wiring) != 1:
            raise RuntimeError('stowed anatomy helper must be called once')
        if anatomy.index(anatomy_wiring) < anatomy.index('for(int k=rig.head;k>=0;k=s.parent[k])'):
            raise RuntimeError('stowed references must follow validated trunk map')
        if 'StowedReferencePoseR112(s,i,tx.selected,model)' not in publication:
            raise RuntimeError('completed packet must consume qualified stowed reference')
        init = anatomy_wiring + '\n'
    else:
        init = '// Baseline has no stowed reference cache initialization.\n'
    slices = {'body_math_r112.inc': math, 'body_chest_r112.inc': chest,
              'body_stowed_r112.inc': helper, 'body_stowed_init_r112.inc': init,
              'body_stowed_publish_r112.inc': publication}
    for name, text in slices.items():
        (out / name).write_text(text)
    return {'source_sha256': files,
            'literal_slice_sha256': {name: sha(text.encode()) for name, text in slices.items()},
            'stowed_reference_wiring': bool(helper)}


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--source', type=Path, default=ROOT)
    ap.add_argument('--out', type=Path, required=True)
    ap.add_argument('--baseline-source', type=Path)
    ap.add_argument('--skip-baseline', action='store_true')
    ap.add_argument('--sanitizers', action='store_true')
    args = ap.parse_args()
    root, out = args.source.resolve(), args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    fixture = root / 'tests/body_ownership_r112_test.cpp'
    results = []
    for baseline in ([False] if args.skip_baseline else [False, True]):
        label = 'baseline' if baseline else 'current'
        destination = out / label
        receipt = extract(root, destination, baseline, args.baseline_source)
        for opt in ['-O0', '-O2']:
            name = label + opt
            exe = destination / ('body_ownership_r112' + opt)
            command = [os.environ.get('CXX', 'g++'), '-std=c++20', opt, '-pthread',
                       '-I' + str(destination), '-I' + str(root / 'src'), str(fixture), '-o', str(exe)]
            if args.sanitizers:
                command[1:1] = ['-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-fno-pie', '-no-pie']
            built = subprocess.run(command, capture_output=True, text=True)
            (destination / (name + '.build.log')).write_text(built.stdout + built.stderr)
            result = {'name': name, 'command': command, 'compile': built.returncode,
                      'fixture_sha256': sha(fixture.read_bytes()), 'provenance': receipt}
            if built.returncode == 0:
                env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0', UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
                ran = subprocess.run([str(exe)], capture_output=True, text=True, env=env)
                output = ran.stdout + ran.stderr
                (destination / (name + '.run.log')).write_text(output)
                failures = [line for line in output.splitlines() if line.startswith('FAIL ')]
                expected = (ran.returncode != 0 and failures and
                            all('R112_FIXED:' in line for line in failures)) if baseline else ran.returncode == 0
                result.update(run=ran.returncode, failures=failures, passed=bool(expected), output=output)
            else:
                result.update(passed=False)
            results.append(result)
            print(name, 'PASS' if result['passed'] else 'FAIL', flush=True)
    report = {'passed': all(r['passed'] for r in results), 'results': results,
              'baseline': 'SKIPPED' if args.skip_baseline else str(args.baseline_source or BASELINE),
              'sanitizers': args.sanitizers, 'leak_detection': False,
              'limits': ['Literal production torso/publisher/helper only; full coherent solver not compiled.',
                         'No native animation curve, retail renderer, Windows DLL or HMD executed.',
                         'No actor world travel or native additional attachment transform changed.']}
    (out / 'RESULT.json').write_text(json.dumps(report, indent=2) + '\n')
    print(out / 'RESULT.json')
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
