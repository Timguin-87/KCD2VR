"""F9 native-held/passive-hand diagnostic enrollment and original receipt tests."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--output', type=Path, required=True)
    ap.add_argument('--baseline', type=Path)
    args = ap.parse_args()
    root = Path(__file__).resolve().parents[1]
    out = args.output.resolve()
    if out == root or root in out.parents:
        ap.error('Evidence output must be outside source')
    out.mkdir(parents=True, exist_ok=True)
    source = root / 'src/vrik/passive_contact_service169r1.inl'
    (out / 'passive_contact_service_source.inc').write_bytes(source.read_bytes())
    hashes = [{'path': str(root / p), 'sha256': hashlib.sha256((root / p).read_bytes()).hexdigest()} for p in (
        'src/diagnostics/physics_stage_d42.h', 'src/diagnostics/contact_capture_d84.h',
        'src/diagnostics/passive_contact169r1.h', 'src/vrik/passive_contact_service169r1.inl')]
    (out / 'source.json').write_text(json.dumps({'sources': hashes, 'scope': 'Actual diagnostic producer headers and literal passive service; copied native counters and unrelated bow drain are controlled leaves; no gameplay adapter substitute.'}, indent=2) + '\n')
    jobs = []
    for name in ('physics_capture_d84', 'physics_stage_d42', 'passive169r1'):
        configs = [('debug', ['-O0'])]
        if name == 'physics_capture_d84':
            configs += [('release', ['-O2']), ('sanitized', ['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-no-pie'])]
        for mode, flags in configs:
            jobs.append((name, mode, root / 'tests' / (name + '_test.cpp'), flags, False))
    if args.baseline:
        # This specifically tests an old producer against the new recorder
        # registry. An old header lacking F9 cannot manufacture that coverage.
        before = args.baseline.resolve() / 'src/diagnostics/physics_stage_d42.h'
        text = before.read_text(encoding='utf-8-sig').replace('#include "capture168.h"', '#include "diagnostics/capture168.h"')
        (out / 'baseline_stage.inc').write_text(text)
        fixture = out / 'baseline_f9.cpp'
        fixture.write_text('#include "baseline_stage.inc"\n#include <cstdio>\nint main(){namespace c=kcd2vr::diagnostics::capture168;namespace p=kcd2vr::diagnostics::physics_d42;c::gate.select(c::Domain::Physics,true,1000);if(!p::lease(1000).on()){std::puts("FAIL D84 preimage F9 stage lease absent");return 1;}return 0;}\n')
        jobs.append(('baseline_f9', 'debug', fixture, ['-O0'], True))
    results = []
    for name, mode, fixture, flags, expected_fail in jobs:
        binary = out / (name + '_' + mode)
        cmd = ['g++', '-std=c++20', '-UNDEBUG', '-pthread', *flags, '-I', str(root / 'src'), '-I', str(out), str(fixture), '-o', str(binary)]
        build = subprocess.run(cmd, capture_output=True, text=True)
        binary.with_suffix('.build.log').write_text(build.stdout + build.stderr)
        row = dict(name=name, mode=mode, command=cmd, build_rc=build.returncode, expected_failure=expected_fail, passed=False)
        if not build.returncode:
            env = dict(os.environ)
            if mode == 'sanitized':
                env['ASAN_OPTIONS'] = 'detect_leaks=0'
            run = subprocess.run([str(binary)], capture_output=True, text=True, env=env)
            binary.with_suffix('.run.log').write_text(run.stdout + run.stderr)
            row.update(run_rc=run.returncode, stdout=run.stdout, stderr=run.stderr)
            row['passed'] = run.returncode == 0 if not expected_fail else run.returncode == 1 and 'FAIL D84 preimage F9 stage lease absent' in run.stdout and not run.stderr
        results.append(row)
        print(f'{name}/{mode}: {"PASS" if row["passed"] else "FAIL"}', flush=True)
    (out / 'results.json').write_text(json.dumps(results, indent=2) + '\n')
    return 0 if all(row['passed'] for row in results) else 1


if __name__ == '__main__':
    raise SystemExit(main())
