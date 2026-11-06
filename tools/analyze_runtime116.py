"""Compact, reproducible discoveries from the supplied V115R1 headset capture.

Line numbers are one-based. Counts are observations, never proof of native ABI.
No raw corpus is copied. --older-db queries the existing lossless V113 index.
"""
import argparse
import collections
import hashlib
import json
import re
import sqlite3
from pathlib import Path
from query_log_discovery113 import rows

PATTERNS = {
    'player_action': 'PLAYER-ACTION REQUEST', 'body_gate': 'BODY_GATE',
    'weapon_pose': 'WEAPON_POSE', 'pose_reject': 'POSE-REJECT',
    'reference_sweep': 'REFERENCE-SWEEP', 'physical_sweep': 'NATIVE-WEAPON-SWEEP',
    'contact': 'NATIVE_CONTACT', 'motion_status': 'V66.112-MOTION STATUS',
    'hand_use': 'V66.111-HAND-USE', 'held_use': 'HELD_HAND_USE',
    'enrich': 'V66.90-F3-ENRICH', 'native_entered': 'NATIVE_ACTION_ENTERED',
    'native_combat': 'V66.104 NATIVE_COMBAT', 'static_collision': 'COLLISION STATUS',
}


def analyze(log):
    data = log.read_bytes(); lines = data.decode('utf-8', errors='replace').splitlines()
    modes = (0, 0); transitions = []; groups = {}; actions = collections.Counter()
    callers = collections.defaultdict(collections.Counter)
    bow = []; rejects = collections.Counter()
    for number, line in enumerate(lines, 1):
        if 'changed=' in line and (m := re.search(r'F3=(\d+) F5=(\d+)', line)):
            modes = tuple(map(int, m.groups()))
            transitions.append(dict(line=number, stamp=line[1:24], F3=modes[0], F5=modes[1]))
        mode = f'F3={modes[0]},F5={modes[1]}'
        for name, token in PATTERNS.items():
            if token not in line:
                continue
            g = groups.setdefault(name, dict(count=0, by_mode=collections.Counter(), first=None, last=None))
            g['count'] += 1; g['by_mode'][mode] += 1
            record = dict(line=number, text=line)
            if g['first'] is None: g['first'] = record
            g['last'] = record
            if name == 'player_action' and (m := re.search(r'kind=(\d+)', line)):
                actions[f'{mode},kind={m[1]}'] += 1
                if int(m[1]) in (5, 6, 7): bow.append(record)
            if name in ('reference_sweep', 'physical_sweep') and (m := re.search(r'caller(?:Rva)?=(?:WHGame\+)?(?:0x)?([0-9A-Fa-f]+)', line)):
                callers[name][m[1].upper()] += 1
            if name == 'pose_reject' and (m := re.search(r'reason=([^ ]+)', line)):
                rejects[f'{mode},reason={m[1]}'] += 1
    return dict(file=log.name, bytes=len(data), sha256=hashlib.sha256(data).hexdigest(),
                lines=len(lines), transitions=transitions, groups=groups, player_actions=actions,
                sweep_callers=callers, pose_rejects=rejects, bow_lifecycle=bow)


def older(path):
    db = sqlite3.connect(path.resolve().as_uri()+'?mode=ro', uri=True)
    result = {}
    for family in ('V66.85-F3-COLLISION%', 'V66.90-F3-ENRICH%'):
        count = 0; examples = []; keys = collections.Counter(); targets = collections.Counter()
        for name, line, text in rows(db, family):
            if not re.search(r'\bn=\d+|raw\w*=|event=|eventSeq=|targetId=', text): continue
            count += 1
            if len(examples) < 2: examples.append(dict(path=name, line=line, text=text))
            if m := re.search(r'keys=\{([^}]+)\}', text): keys[m[1]] += 1
            if m := re.search(r'(?:targetId|entityId|idReadback)=(\d+)', text): targets[m[1]] += 1
        result[family] = dict(count=count, participant_keys=keys, target_ids=targets, examples=examples)
    db.close()
    return result


if __name__ == '__main__':
    p = argparse.ArgumentParser(); p.add_argument('--log', type=Path, required=True)
    p.add_argument('--older-db', type=Path); p.add_argument('--out', type=Path, required=True)
    a = p.parse_args(); result = analyze(a.log)
    if a.older_db: result['older_material_contact_records'] = older(a.older_db)
    a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    print(json.dumps(dict(lines=result['lines'], groups={k:v['count'] for k,v in result['groups'].items()},
                         player_actions=result['player_actions'], sweep_callers=result['sweep_callers']), indent=2))
