#!/usr/bin/env python3
"""Copy complete production D51 callback bodies for host execution, with hashes.

No decisions are rewritten. Native IO/registers/engine leaves belong to the
explicit fixture. This extraction does not compile a Windows DLL or execute SEH.
"""
import argparse
import hashlib
import json
from pathlib import Path


def function(text, signature):
    start = text.index(signature)
    brace = text.index('{', start)
    depth, state, pos = 0, 'code', brace
    while pos < len(text):
        char, following = text[pos], text[pos + 1:pos + 2]
        if state == 'line':
            if char == '\n':
                state = 'code'
        elif state == 'comment':
            if char == '*' and following == '/':
                state = 'code'
                pos += 1
        elif state in ['"', "'"]:
            if char == '\\':
                pos += 1
            elif char == state:
                state = 'code'
        elif char == '/' and following == '/':
            state = 'line'
            pos += 1
        elif char == '/' and following == '*':
            state = 'comment'
            pos += 1
        elif char in ['"', "'"]:
            state = char
        elif char == '{':
            depth += 1
        elif char == '}':
            depth -= 1
            if depth == 0:
                return text[start:pos + 1]
        pos += 1
    raise AssertionError('Unterminated production function: ' + signature)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    root, out = args.source.resolve(), args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    specs = [
        ('src/subsystems/masterstrike_combos/physical_finisher_gate_cb1.inl', 'finisher_callbacks_d51_production.inc',
         lambda s: s[:s.index('static void UninstallFinisherCB1(')]),
        ('src/vrik/master_gate47.inl', 'master_callbacks_d51_production.inc',
         lambda s: s[:s.index('static bool ArmMasterGate47(')]),
        ('src/vrik/native_bridges168.inl', 'factory_callback_d51_production.inc',
         lambda s: function(s, 'static void MasterFactoryGate168(') + '\n'),
        ('src/vrik/native_attack_clock50c2.inl', 'clock_callback_d51_production.inc',
         lambda s: function(s, 'static void MotionClockMid50(') + '\n'),
        ('src/vrik/native_input117.inl', 'input_class_d62_production.inc',
         lambda s: function(s, 'static int NativeInputClass117(') + '\n'),
    ]
    receipt = {'source': str(root), 'method': 'complete unchanged callback blocks; native IO is explicit fixture leaves', 'blocks': []}
    for source, target, extract in specs:
        path = root / source
        block = extract(path.read_text())
        (out / target).write_text(block)
        receipt['blocks'].append({'source': source, 'source_sha256': hashlib.sha256(path.read_bytes()).hexdigest(),
                                  'generated': target, 'generated_sha256': hashlib.sha256(block.encode()).hexdigest()})
    (out / 'EXTRACTION.json').write_text(json.dumps(receipt, indent=2) + '\n')
    print('D51 production callbacks extracted:', len(specs))


if __name__ == '__main__':
    main()
