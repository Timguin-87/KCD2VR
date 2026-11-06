#!/usr/bin/env python3
"""Exercise literal native client/adapter and geometry with controlled API leaves.

Current checks run without Git. --baseline-ref or --baseline-source selects a
genuine original source/fixture set for the publication-handoff counterexample.
"""
from pathlib import Path
import argparse
import hashlib
import json
import re
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--out', type=Path, required=True)
parser.add_argument('--baseline-ref')
parser.add_argument('--baseline-source', type=Path)
args = parser.parse_args()
args.out.mkdir(parents=True, exist_ok=True)
compiler = shutil.which('g++') or shutil.which('clang++')
if not compiler:
    raise SystemExit('C++20 host compiler required')
paths = ['src/subsystems/ranged_reload/crossbow_native_xr34.h',
         'src/subsystems/ranged_reload/crossbow_native_xr34.inl',
         'src/subsystems/ranged_reload/crossbow_rifle_request_h21.inl',
         'tests/crossbow_native_xr34_test.cpp']


def get(path, baseline):
    if baseline and args.baseline_source:
        return (args.baseline_source / path).read_bytes()
    if baseline and args.baseline_ref:
        return subprocess.check_output(['git', 'show', args.baseline_ref + ':' + path], cwd=ROOT)
    return (ROOT / path).read_bytes()


def function(text, signature):
    a = text.index(signature)
    b = text.index('{', a)
    depth = 0
    for i in range(b, len(text)):
        if text[i] == '{':
            depth += 1
        elif text[i] == '}':
            depth -= 1
            if depth == 0:
                return text[a:i + 1]
    raise ValueError(signature)


def digest(data):
    return hashlib.sha256(data).hexdigest()


def run(baseline, optimization):
    label = ('baseline' if baseline else 'current') + '_' + optimization
    folder = args.out / label
    folder.mkdir(exist_ok=True)
    raw = {p: get(p, baseline) for p in paths}
    source = {p: data.decode().replace('\r\n', '\n') for p, data in raw.items()}
    header, adapter, route, fixture = (source[p] for p in paths)
    snapshot = 'bool(*spatial)(const Owner&,const Source&,Spatial&)' in header
    ranged = ROOT / 'src/subsystems/ranged_reload'
    local_header = re.sub(r'#include "([^"]+)"',
                          lambda m: '#include "' + (ranged / m[1]).resolve().as_posix() + '"', header)
    (folder / 'client.h').write_text(local_header)
    # Same explicit SEH-finally host substitute as the retained native suites.
    start = adapter.index('    __try{started=g_crossbow_xr34.startNewAfterRestore(')
    ending = '__finally{(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}'
    end = adapter.index(ending, start) + len(ending)
    replaced = adapter[start:end].replace('    __try{',
        '    {struct HostFinishR108 {ma::Token& token;bool& started;~HostFinishR108(){(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}} finish{token,started};', 1)
    replaced = replaced.replace('}\n    ' + ending, '}')
    adapter_host = adapter[:start] + replaced + adapter[end:]
    (folder / 'adapter_host.inc').write_text(adapter_host)
    fixture = fixture.replace('"../src/', '"' + (ROOT / 'src').as_posix() + '/')
    fixture = fixture.replace('"' + (ROOT / paths[0]).as_posix() + '"', '"client.h"')
    fixture = fixture.replace('"' + (ROOT / paths[1]).as_posix() + '"', '"adapter_host.inc"')
    (folder / 'native_fixture.cpp').write_text(fixture)
    spatial = function(route, 'static bool RangedSpatialXR34(')
    template = ROOT / 'tests/crossbow_source_snapshot_r108_test.cpp'
    generated = template.read_text().replace('@RANGED_SPATIAL@', spatial)
    (folder / 'test.cpp').write_text(generated)
    cmd = [compiler, '-std=c++20', '-' + optimization, '-UNDEBUG', '-pthread',
           '-DHAS_SOURCE_SNAPSHOT_R108=' + str(int(snapshot)),
           '-I' + str(folder), '-I' + str(ROOT / 'src'), '-I' + str(ROOT / 'tests/syntax_shims'),
           str(folder / 'test.cpp'), '-o', str(folder / 'test')]
    build = subprocess.run(cmd, text=True, capture_output=True)
    (folder / 'compile.txt').write_text(build.stdout + build.stderr)
    if build.returncode:
        raise RuntimeError(label + ' compile failed: ' + build.stderr[-5000:])
    result = subprocess.run([str(folder / 'test')], text=True, capture_output=True)
    (folder / 'output.txt').write_text(result.stdout + result.stderr)
    if not result.stdout.strip().startswith('{'):
        raise RuntimeError(label + ' produced no assertion record: ' + result.stderr[-3000:])
    receipt = json.loads(result.stdout)
    receipt.update(command=cmd, returncode=result.returncode, original_source=baseline,
                   source_hashes={p: digest(data) for p, data in raw.items()},
                   fixture_hash=digest(template.read_bytes()), runner_hash=digest(Path(__file__).read_bytes()),
                   extracted_spatial_hash=digest(spatial.encode()),
                   limits=['Literal native client, adapter, and RangedSpatialXR34 execute.',
                           'Native geometry, binding, memory/API and concurrent scheduler interleaving are explicit host leaves.',
                           'Only host finally is substituted; no Windows SEH, DLL, GPU or in-game result.'])
    (folder / 'RESULT.json').write_text(json.dumps(receipt, indent=2) + '\n')
    print(f"{label}: checks={receipt['checks']} failures={receipt['failures']} exit={result.returncode}", flush=True)
    return receipt


results = []
for optimization in ('O0', 'O2'):
    results.append(run(False, optimization))
    if args.baseline_ref or args.baseline_source:
        results.append(run(True, optimization))
positive = all(r['returncode'] == 0 and r['failures'] == 0 for r in results if not r['original_source'])
negative = all(r['returncode'] == 1 and
               'fresh same-owner publication handoff does not abandon manual reload' in r['failed_checks']
               for r in results if r['original_source'])
summary = {'passed': positive and negative,
           'baseline_requested': bool(args.baseline_ref or args.baseline_source),
           'results': [{k: r[k] for k in ('checks', 'failures', 'returncode', 'original_source')} for r in results]}
(args.out / 'RESULT.json').write_text(json.dumps(summary, indent=2) + '\n')
raise SystemExit(0 if summary['passed'] else 1)
