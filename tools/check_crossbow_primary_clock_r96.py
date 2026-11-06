#!/usr/bin/env python3
"""Compile the exact current primary retain function and optional before-file.

Only native ownership/binding/readback leaves are supplied by the fixture. The
real owner equality, receipt types and SnapshotMailbox are retained. This does
not claim native callback scheduling or game/HMD execution.
"""
from pathlib import Path
import argparse
import hashlib
import json
import subprocess

SOURCE = 'src/subsystems/ranged_reload/crossbow_rifle_request_h21.inl'
PREFIX = 'static bool RangedRetainPrimaryPlacementR95('


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def block(text, prefix):
    at = text.index(prefix);start = text.index('{', at);depth = 1;end = start + 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}');end += 1
    if text[end:end + 1] == ';':end += 1
    return text[at:end]


def run(root, source, out, compiler):
    out.mkdir(parents=True, exist_ok=True)
    ranged = source.read_text();frik = (root / 'src/frik_pose_probe.cpp').read_text()
    parts = [block(frik, p) for p in ['struct QuatT28 ', 'struct Matrix34V6550 ', 'struct RangedSocketXR34 ']]
    parts.append(block(ranged, 'struct RangedPlacedXR34 '))
    (out / 'crossbow_primary_types_r96.inc').write_text('\n'.join(parts) + '\n')
    literal = block(ranged, PREFIX)
    function = out / 'crossbow_primary_function_r96.inc';function.write_text(literal + '\n')
    fixture = root / 'tests/crossbow_primary_clock_r96_test.cpp';exe = out / 'test'
    cmd = [compiler, '-std=c++20', '-O2', '-pthread', '-I', str(out), '-I', str(root / 'src'),
           str(fixture), '-o', str(exe)]
    built = subprocess.run(cmd, capture_output=True, text=True)
    (out / 'build.log').write_text(built.stdout + built.stderr)
    record = {'source': str(source), 'source_sha256': sha(source), 'command': cmd, 'compile': built.returncode,
              'exact_function_utf8_lf_sha256': hashlib.sha256(literal.encode()).hexdigest(),
              'function_include_sha256': sha(function), 'fixture_sha256': sha(fixture),
              'types_sha256': sha(out / 'crossbow_primary_types_r96.inc')}
    if built.returncode == 0:
        ran = subprocess.run([str(exe)], capture_output=True, text=True)
        (out / 'run.log').write_text(ran.stdout + ran.stderr)
        record.update(run=ran.returncode, stdout=ran.stdout, stderr=ran.stderr)
    return record


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1])
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--before-file', type=Path)
    p.add_argument('--compiler', default='g++')
    args = p.parse_args();root = args.source.resolve();out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    runs = {'current': run(root, root / SOURCE, out / 'current', args.compiler)}
    if args.before_file:runs['before'] = run(root, args.before_file.resolve(), out / 'before', args.compiler)
    passed = runs['current'].get('run') == 0
    if args.before_file:
        passed = passed and runs['before']['compile'] == 0 and runs['before'].get('run') == 1 and \
            'FAIL publication_during_native_owner_check' in runs['before'].get('stdout', '')
    result = {'passed': passed, 'runs': runs,
              'dependencies': {p: sha(root / p) for p in ['src/core/snapshot_mailbox.h',
                  'src/subsystems/ranged_reload/crossbow_native_xr34.h', 'src/frik_pose_probe.cpp']},
              'scope': 'Literal production primary retain function with real mailbox/equality/types; controlled native leaves and deterministic scheduler interleaving. Windows/game/HMD NOT_RUN.'}
    (out / 'RESULT.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({'passed': passed, 'runs': {n: {'compile': r['compile'], 'run': r.get('run'),
          'function_sha256': r['exact_function_utf8_lf_sha256']} for n, r in runs.items()},
          'result': str(out / 'RESULT.json')}, indent=2))
    return 0 if passed else 1


if __name__ == '__main__':raise SystemExit(main())
