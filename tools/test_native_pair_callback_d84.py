#!/usr/bin/env python3
"""Compose exact supplied retail callback bytes with the production pair reader.

Writes copyrighted native slices only to the chosen local evidence directory;
do not include the generated .bin file in source or a redistributable package.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

from native_research112 import PE

ROOT = Path(__file__).resolve().parents[1]
SHA = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
RANGES = [(0x90D04C, 0x90D0CA), (0x27590C8, 0x27590ED),
          (0x2759320, 0x2759345), (0x399FBC, 0x399FEA)]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--dll', type=Path, required=True)
    parser.add_argument('--source', type=Path, default=ROOT)
    parser.add_argument('--baseline', type=Path)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    pe = PE(args.dll)
    seal = hashlib.sha256(pe.data).hexdigest()
    if seal != SHA:
        parser.error('Exact reviewed retail DLL SHA-256 required')
    blob = out / 'native_callback_ranges.bin'
    chunks = [pe.read(start, end-start) for start, end in RANGES]
    blob.write_bytes(b''.join(chunks))
    records = []
    cases = [('current', args.source, False, ['-O2'])]
    if args.baseline:
        cases.append(('D83R1_negative', args.baseline, True, ['-O2']))
    fixture = ROOT / 'tests/native_pair_callback_d84_test.cpp'
    for name, source, negative, flags in cases:
        command = [os.environ.get('CXX', 'g++'), '-std=c++20', *flags,
                   '-I' + str(source.resolve() / 'src'), str(fixture), '-o', str(out/name)]
        build = subprocess.run(command, text=True, capture_output=True)
        (out/(name+'.build.log')).write_text(build.stdout+build.stderr)
        record = dict(name=name, source=str(source.resolve()), command=command,
                      compile_exit=build.returncode, expected_negative=negative)
        if build.returncode == 0:
            run = subprocess.run([str(out/name), str(blob)], text=True, capture_output=True)
            output = run.stdout+run.stderr
            (out/(name+'.log')).write_text(output)
            record.update(run_exit=run.returncode, output=output,
                          passed=run.returncode==(1 if negative else 0))
            if negative:
                record['passed'] &= 'FAIL production reader accepts native-constructed callback chain' in output
        else:
            record['passed'] = False
        records.append(record)
        print(name, 'PASS' if record['passed'] else 'FAIL', record.get('output','')[:3000])
    report = dict(passed=all(r['passed'] for r in records), retail_sha256=seal,
                  native_ranges=[dict(start=hex(s), end=hex(e), sha256=hashlib.sha256(c).hexdigest())
                                 for (s,e),c in zip(RANGES,chunks)],
                  fixture_sha256=hashlib.sha256(fixture.read_bytes()).hexdigest(),
                  tests=records, limits=['Actual native wrapper constructor, two actual member-functor clones and actual source clear executed.',
                                        'Allocation and temporary callback destruction are controlled leaves.',
                                        'Native pair admission, animation damage events and live game/headset NOT_RUN.'])
    (out/'RESULT.json').write_text(json.dumps(report, indent=2)+'\n')
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
