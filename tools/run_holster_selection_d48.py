#!/usr/bin/env python3
"""Test the actual H36 ledger, native equipment dispatcher, and completion rules.

Only native function leaves and attachment-graph reads are simulated. Production
stage/selection/preparation/entry/return/retirement code is extracted verbatim.
The unchanged D46 source may be supplied as a compiled counterfactual.
"""
import argparse
import hashlib
import json
import re
import subprocess
from pathlib import Path
from run_input_regressions_r38 import function

HERE = Path(__file__).resolve().parents[1]
REPLACEMENTS = {
    'reinterpret_cast<bool(__fastcall*)(void*,std::uint64_t)>(WhBase()+0x8CF158)': 'HostStowD48',
    'reinterpret_cast<void(__fastcall*)(void*,std::uint64_t,int)>(WhBase()+0x8CEA48)': 'HostDrawD48',
    'reinterpret_cast<void(__fastcall*)(void*,int)>(WhBase()+0x8EEA04)': 'HostSelectD48',
    'reinterpret_cast<void(__fastcall*)(void*)>(WhBase()+0x2AA24A0)': 'HostLegacyDrawD48',
    'reinterpret_cast<void(__fastcall*)(void*)>(WhBase()+0x8EE994)': 'HostLegacyStowD48',
}


def run(source, out, baseline=False):
    out.mkdir(parents=True, exist_ok=True)
    paths = [source / 'src/subsystems/throw_drop' / name for name in
             ('holster_retirement_h36.inl', 'native_equipment_leaves.inl', 'holster_completion_h36.inl')]
    h36, leaves, completion = [path.read_text() for path in paths]
    native = (source / 'src/vrik/physical_native.inl').read_text()
    current = 'PhysLeftEquipmentEnterH36' in h36
    prototypes = native[native.index('static bool PhysLeftEquipmentBeginH36(') if current else
                        native.index('static void PhysLeftEquipmentBeginH36('):native.index('#include "../subsystems/throw_drop/native_equipment_leaves.inl"')]
    structs = '\n'.join(re.search(r'struct ' + name + r' \{.*?\n\};', completion, re.S).group()
                        for name in ('LeftSocketH36', 'LeftBindingH36', 'LeftStowDestinationsH36'))
    (out / 'selection_structs_d48.inc').write_text(structs + '\n' + function(completion, 'SameSocketGraphH36'))
    body = h36[h36.index('struct LeftEquipmentObligationH36 {'):]
    leaf = leaves[:leaves.index('static bool PhysEquipmentEntries62(')] + leaves[leaves.index('static bool PhysEquipmentCommand6698('):]
    for old, new in REPLACEMENTS.items():
        assert old in leaf, old
        leaf = leaf.replace(old, new)
    (out / 'selection_actual_d48.inc').write_text(prototypes + '\n' + leaf + '\n' + body)
    flags = ['-DD48_CURRENT'] if current else []
    command = ['g++', '-std=c++20', '-O1', '-pthread', '-Wno-misleading-indentation',
               '-I' + str(source / 'src'), '-I' + str(out), *flags,
               str(HERE / 'tests/holster_selection_d48_test.cpp'), '-o', str(out / 'selection_test')]
    c = subprocess.run(command, capture_output=True, text=True)
    (out / 'compile.log').write_text(c.stdout + c.stderr)
    row = {'source': str(source), 'command': command, 'compile_returncode': c.returncode,
           'sha256': {str(x.relative_to(source)): hashlib.sha256(x.read_bytes()).hexdigest() for x in paths},
           'native_leaf_substitutions': REPLACEMENTS, 'native_execution': False,
           'attachment_reads': 'explicit simulated graph; production retirement predicates unchanged'}
    if c.returncode:
        print(c.stderr)
        row['passed'] = False
    else:
        r = subprocess.run([str(out / 'selection_test')], capture_output=True, text=True)
        (out / 'run.log').write_text(r.stdout + r.stderr)
        row['run_returncode'] = r.returncode
        row['passed'] = r.returncode == 0 if not baseline else r.returncode != 0 and \
            'FAIL exact original left cross-set draw graph completes without permanent debt' in r.stdout and \
            'FAIL failed binding preflight never invokes native draw or marks entered' in r.stdout
        print(r.stdout, end='')
    (out / 'results.json').write_text(json.dumps(row, indent=2) + '\n')
    return row


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--source', type=Path, default=HERE)
    p.add_argument('--out', type=Path, required=True)
    p.add_argument('--baseline', type=Path)
    a = p.parse_args()
    out = a.out.resolve()
    rows = [run(a.source.resolve(), out / 'current')]
    if a.baseline:
        rows.append(run(a.baseline.resolve(), out / 'baseline', baseline=True))
    passed = all(x['passed'] for x in rows)
    (out / 'RESULTS.json').write_text(json.dumps({'passed': passed, 'runs': rows}, indent=2) + '\n')
    print('D48 actual selection and left draw lifecycle:', 'PASS' if passed else 'FAIL')
    return 0 if passed else 1


if __name__ == '__main__':
    raise SystemExit(main())
