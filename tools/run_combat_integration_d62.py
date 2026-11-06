#!/usr/bin/env python3
"""Run D62 connected combat regressions and optional D61 negative controls.

Executes complete production callback blocks through the existing extractor.
Native memory, engine calls, hook placement and player input remain fixture
boundaries. This does not build a Windows DLL or assert headset acceptance.
"""
import argparse
import hashlib
import json
import xml.etree.ElementTree as ET
import os
from pathlib import Path
import subprocess
import sys


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--baseline', type=Path)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    fixtures = Path(__file__).resolve().parents[1]
    source, out = args.source.resolve(), args.out.resolve()
    if out == source or source in out.parents:
        parser.error('--out must be outside the source tree')
    out.mkdir(parents=True, exist_ok=True)
    cases = ['native_master_backups_d51', 'finisher_backups_d51', 'physical_finisher_cb1', 'combo_physical_route_d62']
    results = []
    compiler = os.environ.get('CXX', 'g++')
    compiler_version = subprocess.run([compiler, '--version'], capture_output=True, text=True, check=True).stdout

    for negative, production in [(False, source)] + ([(True, args.baseline.resolve())] if args.baseline else []):
        folder = out / ('d61_negative' if negative else 'current')
        folder.mkdir(parents=True, exist_ok=True)
        generated = folder / 'generated'
        subprocess.run([sys.executable, str(fixtures/'tools/extract_combat_backups_d51.py'),
                        '--source', str(production), '--out', str(generated)], check=True)
        for case in cases:
            test = fixtures/'tests'/(case+'_test.cpp')
            test_input = test
            # The standalone pure policy test uses a relative production
            # include. Normalize ONLY that include so the negative control
            # actually consumes the supplied D61 policy, not today's source.
            if case == 'physical_finisher_cb1':
                test_input = folder/(case+'_test.cpp')
                test_input.write_text(test.read_text().replace(
                    '#include "../src/subsystems/masterstrike_combos/physical_finisher_cb1.h"',
                    '#include "subsystems/masterstrike_combos/physical_finisher_cb1.h"'))
            exe, dep = folder/case, folder/(case+'.d')
            cmd = [compiler, '-std=c++17', '-O1', '-g', '-pthread', '-D__fastcall=',
                   '-fsanitize=address,undefined', '-fno-sanitize-recover=all',
                   '-fno-omit-frame-pointer', '-fno-pie', '-no-pie',
                   '-MMD', '-MF', str(dep), '-I'+str(production/'src'),
                   '-I'+str(generated), '-I'+str(fixtures/'tests'), str(test_input), '-o', str(exe)]
            build = subprocess.run(cmd, capture_output=True, text=True)
            (folder/(case+'.build.log')).write_text(build.stdout+build.stderr)
            row = {'case': case, 'negative_control': negative, 'production': str(production),
                   'fixture': str(test), 'fixture_sha256': digest(test),
                   'compiled_fixture_sha256': digest(test_input), 'command': cmd, 'build_exit': build.returncode}
            if not build.returncode:
                env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0', UBSAN_OPTIONS='print_stacktrace=1:halt_on_error=1')
                run = subprocess.run([str(exe)], capture_output=True, text=True, env=env)
                row.update(run_exit=run.returncode, output=run.stdout+run.stderr)
                (folder/(case+'.run.log')).write_text(row['output'])
            row['dependencies'] = {}
            if dep.exists():
                for token in dep.read_text().replace('\\\n', ' ').split()[1:]:
                    path = Path(token)
                    if path.is_file(): row['dependencies'][str(path.resolve())] = digest(path)
            output = row.get('output', '')
            row['passed'] = row.get('run_exit') == (1 if negative else 0)
            if negative:
                row['passed'] = row['passed'] and 'FAIL ' in output and 'AddressSanitizer' not in output and 'runtime error:' not in output
            results.append(row)
            print(('D61 expected assertion failures: ' if negative else '')+case+': '+('PASS' if row['passed'] else 'FAIL'), flush=True)
            print(output[-1800:] if not row['passed'] else output.splitlines()[-1] if output else build.stderr[-1800:], flush=True)
    pins = ['src/subsystems/masterstrike_combos/physical_finisher_cb1.h',
            'src/vrik/master_gate47.inl', 'src/subsystems/masterstrike_combos/physical_finisher_gate_cb1.inl',
            'src/subsystems/masterstrike_combos/combo_zone_assist_cz.h', 'src/vrik/native_input117.inl']
    catalog = source/'tests/fixtures/native_short_combos32/CombatCombos__generated.xml'
    catalog_sha = digest(catalog)
    expected_sha = '048e138ee061ef2003c0ce050bc9f4aa42aed5b6740a97500ca92594603a13e6'
    if catalog_sha != expected_sha:
        raise ValueError('Supplied retail recipe fixture has changed; native final-step proof refused')
    selected = []
    for row in ET.parse(catalog).getroot().findall('CombatCombos/CombatCombo'):
        if (row.get('UIGroup'), row.get('Id')) in [('Blunt','C15'),('BluntShield','C15'),('Halberd','C05')]:
            steps = row.findall('CombatComboSteps/CombatComboStep')
            last = dict(steps[-1].attrib)
            if last.get('AttackType') != 'slash' or last.get('IsCharged','0') != '0':
                raise ValueError('Native final input/charge proof changed')
            selected.append({'group':row.get('UIGroup'),'name':row.get('Id'),
                             'technique':row.get('CombatTechniqueId'),'steps':len(steps),'final':last})
    if len(selected)!=3: raise ValueError('Missing exact native recipe row')
    receipt = {'compiler': compiler_version, 'tests': results, 'passed': all(r['passed'] for r in results),
               'native_recipe_proof': {'path':str(catalog), 'sha256':catalog_sha, 'rows':selected},
               'production_hashes': {p:digest(source/p) for p in pins},
               'source_rules': 'original native selection, eligibility, costs and outcome unchanged; no new ABI/hooks',
               'native_game': 'NOT_RUN', 'Windows_SEH': 'NOT_EXECUTED', 'DLL': 'NOT_BUILT', 'headset': 'NOT_RUN'}
    (out/'RESULTS.json').write_text(json.dumps(receipt, indent=2)+'\n')
    return 0 if receipt['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
