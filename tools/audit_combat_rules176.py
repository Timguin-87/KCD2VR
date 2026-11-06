#!/usr/bin/env python3
"""Recheck local combo/source evidence without loading or modifying the game.

The saved localization columns are preserved evidence, not a freshly observed
installed localization. Raw recipe types, body subparts and action timing remain
different domains. This script never generates a live combo/damage policy.
"""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import re
import subprocess
import shutil
import xml.etree.ElementTree as ET
import zipfile

from native_research112 import PE

ROOT = Path(__file__).resolve().parents[1]
TABLE_SHA = 'a217cd744ee3aeaa4aefe30598995915bc0db1b245b30b0a2eecd814d9c7da1e'
RETAIL_SHA = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def audit(args):
    args.out.mkdir(parents=True, exist_ok=True)
    archive = args.tables.read_bytes()
    assert sha(archive) == TABLE_SHA, 'Different supplied Tables archive'
    catalogue_path = ROOT / 'review/V66.166/research/combos/combo_assets.json'
    catalogue = json.loads(catalogue_path.read_text())
    old_manifest = json.loads((ROOT / 'continuity/semantic_index/combo/source_manifest.json').read_text())
    examples = json.loads((ROOT / 'continuity/semantic_index/combo/selected_examples.json').read_text())
    members = []
    with zipfile.ZipFile(args.tables) as z:
        for row in old_manifest['members'].values():
            if row['archive'] != 'tables':
                continue
            data = z.read(row['member'])
            observed = sha(data)
            assert observed == row['sha256'], row['member']
            members.append({'member': row['member'], 'sha256': observed, 'matched': True})
        for row in catalogue['provenance']:
            if row['archive'] == 'Data/Tables.pak':
                assert sha(z.read(row['member'])) == row['sha256'], row['member']
        recipes = ET.fromstring(z.read('Libs/Tables/combat/CombatCombos__generated.xml')).findall('.//CombatCombo')
        saved = {(c['recipe']['Id'], c['recipe']['RightWeapon'], c['recipe']['LeftWeapon']): c
                 for c in catalogue['combos']}
        assert len(saved) == len(recipes) == 51
        for recipe in recipes:
            key = tuple(recipe.get(k) for k in ('Id', 'RightWeapon', 'LeftWeapon'))
            assert dict(recipe.attrib) == saved[key]['recipe'], key
            assert [dict(s.attrib) for s in recipe.findall('./CombatComboSteps/CombatComboStep')] == saved[key]['steps'], key
        classes = [dict(x.attrib) for x in ET.fromstring(z.read('Libs/Tables/item/weapon_class.xml')).iter() if x.get('id')]
        native_actions = {}
        for name in ('combat_action_attack', 'combat_action_sync_attack'):
            member = f'Libs/Tables/combat/{name}.xml'
            native_actions[member] = [dict(x.attrib) for x in ET.fromstring(z.read(member)).iter() if x.get('mn_fragment_guid')]
        selected = []
        for example in examples:
            guid = example['attributes']['GUID']
            rows = [{'source': member, 'attributes': row} for member, values in native_actions.items()
                    for row in values if row['mn_fragment_guid'] == guid]
            assert rows, guid
            selected.append({'guid': guid, 'combo_tags': example['combo_tags'],
                             'body_metadata_preserved': example['metadata_matches'],
                             'current_action_rows': rows})

    # Keep both localization columns: interpreting one as the native input enum
    # would hide the very mismatch the user asked to investigate.
    selected_names = {'Knee Strike', 'Pommel Strike', 'Mittelhaw', 'Shield Deflect',
                      'Crushing Blow', 'Stomach Slice', 'Knee Crusher', 'Zorn Ort',
                      'Fiore Halbschwerten'}
    semantic_rows = [c for c in catalogue['combos']
                     if c['localization']['perk_ui_name']['text_columns'][-1] in selected_names]
    report = {
        'schema': 1, 'tables_sha256': TABLE_SHA,
        'fresh_table_member_rechecks': members,
        'all_51_current_recipes_equal_saved_raw_attributes_and_steps': True,
        'localization': {'status': 'PRESERVED_ORIGINAL_EXTRACT_NOT_FRESH_INSTALLED_LOAD_ORDER',
                         'catalogue': str(catalogue_path.relative_to(ROOT)),
                         'catalogue_sha256': sha(catalogue_path.read_bytes()),
                         'original_provenance': [r for r in catalogue['provenance'] if r['archive'].startswith('Localization/')],
                         'both_text_columns_retained': True},
        'weapon_classes': classes,
        'selected_verbatim_descriptions_and_native_recipes': semantic_rows,
        'selected_current_action_timing_and_preserved_body_metadata': selected,
        'limits': ['Action duration/hit/start/withdraw fields are authored table data, not measured live combo windows.',
                   'Installed mod load order and current learned-perk state remain unobserved.',
                   'BodySubpartId, ray physical part, skeleton joint, material and GuardZone are distinct domains.',
                   'The generic repeat-target timeout is not a universal combo clock.',
                   'The selected body49 metadata names upper-front left leg, not an exact knee collider.']}

    pe = PE(args.whgame)
    assert sha(pe.data) == RETAIL_SHA and pe.stamp == 0x6A350E20
    native = []
    for label, address in [('combo_event_gate', 0xC5C52C), ('combo_filter_and_cursor', 0xC5C594),
                           ('combo_reset_dispatch', 0xC5E460), ('combo_reset', 0xC5E4C0)]:
        start, end = pe.function(address)
        assert start == address
        command = ['objdump', '-d', '-Mintel', f'--start-address=0x{pe.base+start:x}',
                   f'--stop-address=0x{pe.base+end:x}', str(args.whgame)]
        dest = args.out / f'{label}.asm'
        if shutil.which('objdump'):
            result = subprocess.run(command, check=True, capture_output=True, text=True)
            listing=result.stdout
        else:
            # Windows research hosts need not have GNU binutils. Same pinned
            # function bytes and ranges, printed as RVAs by optional Capstone.
            import capstone
            decoder=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_64)
            listing='\n'.join(f'{i.address-pe.base:08X} {i.bytes.hex():28} {i.mnemonic:8} {i.op_str}'
                for i in decoder.disasm(pe.read(start,end-start),pe.base+start))+'\n'
        dest.write_text(listing)
        native.append({'name': label, 'rva': f'{start:X}', 'end_exclusive': f'{end:X}',
                       'code_sha256': sha(pe.read(start, end-start)), 'listing': dest.name,
                       'entry': pe.read(start, min(24, end-start)).hex()})
    # Exact call/argument setup: state+270 getter feeds arg0; manager+E4
    # supplies arg1. This does not label E4's enum or prove accepted damage.
    assert pe.read(0xC5C55F, 7) == bytes.fromhex('4c8d83e4000000')
    call = pe.read(0xC5C572, 5)
    assert call == bytes.fromhex('e81d000000')
    report['native_combo_boundary'] = {'retail_sha256': RETAIL_SHA, 'functions': native,
        'unique_direct_step_caller_candidates': [f'{x:X}' for x in pe.calls(0xC5C594)],
        'verified_step_call': 'C5C572 -> C5C594; return C5C577',
        'input0_source': 'native property getter at current actor state+270',
        'input1_source': 'copied integer at combo manager+E4; enum still unqualified',
        'event_gate': 'manager bytes E8/EA; meanings not renamed to hit acceptance',
        'result': 'candidate filtering and cursor increments, not a damage/finisher-success return',
        'native_execution': False}

    logs = []
    for path in args.log:
        counts = Counter(); examples_log = {}; lines = 0
        digest = hashlib.sha256()
        with path.open('rb') as f:
            for lines, raw in enumerate(f, 1):
                digest.update(raw)
                text = raw.decode('utf-8', errors='replace').rstrip()
                for key, pattern in (
                    ('guard_edges', r'\bGUARD edge='), ('guard_motion', r'\bGUARD_MOTION\b'),
                    ('combo_events', r'\bCOMBO_EVENT\b'), ('combo_candidates', r'\bCOMBO_CANDIDATE\b'),
                    ('process_records', r'\bPROCESS_HIT\b'), ('submit_records', r'\bSUBMIT_HIT\b'),
                    ('melee_command_records', r'\bMELEE_COMMAND\b'), ('physical_melee_observations', r'\bMELEE_(?:INTENT|STROKE)\b')):
                    if re.search(pattern, text):
                        counts[key] += 1
                        if key not in examples_log: examples_log[key] = {'line': lines, 'text': text[:1800]}
                if 'COMBO_EVENT' in text and 'op=STEP_FILTER' in text:
                    counts['combo_step_filter'] += 1
                    if 'comparable=1' in text and 'observedFieldsChanged=1' in text:
                        counts['combo_comparable_changed_step'] += 1
        logs.append({'name': path.name, 'sha256': digest.hexdigest(), 'lines': lines,
                     'counts': dict(counts), 'first_examples': examples_log,
                     'meaning': 'Observed log records only. Missing/expired capture is not proof of feature absence; no V176 headset acceptance.'})
    report['latest_supplied_logs'] = logs
    report['status'] = 'STATIC_AND_HISTORICAL_EVIDENCE_ONLY'
    (args.out / 'rules_evidence176.json').write_text(json.dumps(report, indent=2, ensure_ascii=False)+'\n')
    print(json.dumps({'recipes_rechecked': 51, 'table_members_rechecked': len(members),
                      'native_function_ranges': len(native), 'logs': [{'name': r['name'], 'counts': r['counts']} for r in logs],
                      'native_execution': False}, indent=2))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--tables', type=Path, required=True)
    parser.add_argument('--whgame', type=Path, required=True)
    parser.add_argument('--log', type=Path, action='append', default=[])
    parser.add_argument('--out', type=Path, required=True)
    audit(parser.parse_args())
