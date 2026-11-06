#!/usr/bin/env python3
"""Run literal D82 startup boundaries with host filesystem/profile adapters."""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
from run_startup_d56r1 import function
ROOT = Path(__file__).resolve().parents[1]


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--skip-sanitizers', action='store_true')
    args = parser.parse_args()
    out = args.out.resolve(); out.mkdir(parents=True, exist_ok=True)
    source = ROOT/'src/subsystems/base/startup_setup_s15.inl'
    text = source.read_text()
    chunks = []
    for start, end in [('struct Field {', '// Panel-native'), ('struct Headset {', 'static constexpr int headsetControl'), ('struct State {', 'static constexpr int contentHeight')]:
        chunks.append(text[text.index(start):text.index(end, text.index(start))])
    signatures = ['static bool Parse(', 'static bool ReadBool(', 'static int ProductionMode(', 'static bool ReadSource(', 'static bool LaunchCompatible(', 'static void LoadLaunchOptions(', 'static bool CommitIni(', 'static bool SaveSource(']
    provenance = []
    for signature in signatures:
        body, line = function(text, signature)
        chunks.append(body)
        provenance.append(dict(signature=signature, line=line, sha256=hashlib.sha256(body.encode()).hexdigest()))
    actual = '\n'.join(chunks)
    (out/'startup_d82_actual.inc').write_text(actual)
    variants = [('release', ['-O2'])]
    if not args.skip_sanitizers:
        variants.append(('asan_ubsan', ['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer']))
    runs = []
    for name, flags in variants:
        exe = out/name
        command = ['g++', '-std=c++20', '-Wall', '-Wextra', '-Wno-misleading-indentation', *flags, '-I'+str(ROOT/'src'), '-I'+str(out), str(ROOT/'tests/stereo_upscaler_d82_test.cpp'), '-o', str(exe)]
        built = subprocess.run(command, text=True, capture_output=True)
        (out/(name+'.compile.log')).write_text(built.stdout+built.stderr)
        record = dict(name=name, command=command, compile_exit=built.returncode, passed=False)
        if built.returncode == 0:
            env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0')
            ran = subprocess.run([str(exe), str(out/(name+'_files'))], text=True, capture_output=True, env=env, timeout=30)
            (out/(name+'.run.log')).write_text(ran.stdout+ran.stderr)
            record.update(exit=ran.returncode, output=ran.stdout+ran.stderr, passed=ran.returncode==0)
        runs.append(record)
    negative = []
    mutations = [
        ('unverified_keep', 'static bool LaunchCompatible(',
         'return ReadSource(s,lines)&&startup_graphics_d80::canKeepConfig(s.productionMode,lines);', 'return true;'),
        ('stale_profile_mode', 'static bool SaveSource(',
         's.productionMode=ProductionMode(s.ini);', '/* fault: trusts stale UI mode */'),
        ('discard_existing_ini', 'static bool CommitIni(',
         'if(existed&&!CopyFileW(s.ini.c_str(),tmp.c_str(),FALSE))return false;', '/* fault: omitted existing INI copy */'),
    ]
    for name, signature, before, after in mutations:
        body, _ = function(actual, signature)
        if body.count(before) != 1:
            raise ValueError('Negative-control source contract changed: '+name)
        mutant = actual.replace(body, body.replace(before, after))
        folder = out / name; folder.mkdir(exist_ok=True)
        (folder/'startup_d82_actual.inc').write_text(mutant)
        exe = folder/'fixture'
        command = ['g++', '-std=c++20', '-O2', '-I'+str(folder), '-I'+str(ROOT/'src'), str(ROOT/'tests/stereo_upscaler_d82_test.cpp'), '-o', str(exe)]
        built = subprocess.run(command, text=True, capture_output=True)
        (folder/'compile.log').write_text(built.stdout+built.stderr)
        record = dict(name=name, compile_exit=built.returncode, rejected=False)
        if built.returncode == 0:
            ran = subprocess.run([str(exe), str(folder/'files')], text=True, capture_output=True, timeout=30)
            (folder/'run.log').write_text(ran.stdout+ran.stderr)
            record.update(exit=ran.returncode, rejected=ran.returncode==1 and 'FAIL line=' in ran.stderr)
        negative.append(record)
    paths = [source, ROOT/'src/subsystems/base/startup_graphics_d80.h', ROOT/'src/stereo_upscaler_d81r2.h', ROOT/'src/graphics_owner31.h', ROOT/'tests/stereo_upscaler_d82_test.cpp', Path(__file__)]
    report = dict(passed=all(item['passed'] for item in runs) and all(item['rejected'] for item in negative), source_sha256={str(p.relative_to(ROOT)):sha(p) for p in paths}, extracted=provenance, runs=runs, negative_controls=negative,
                  limits=['Actual functions execute with real temporary files and substituted Win32/profile leaves.', 'No native Windows dialog or profile API execution; platform encoding/locking behavior not validated.', 'ASAN/UBSAN enabled; LeakSanitizer disabled because sandbox process enumeration is unavailable.', 'No game, vendor GPU or headset execution.'])
    (out/'RESULT.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps(dict(passed=report['passed'], runs=[dict(name=r['name'], passed=r['passed'], output=r.get('output', 'compile failed')) for r in runs], negative_controls=negative), indent=2))
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
