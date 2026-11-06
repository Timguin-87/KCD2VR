"""Audit immutable C63 inputs: budgeted record counts are not swing probabilities."""
import argparse, bisect, collections, hashlib, json, re
from pathlib import Path

FIELD = re.compile(r'\b([A-Za-z][A-Za-z0-9_]*)=([^ ]+)')
EVENT = re.compile(r'^\[[^\]]+\]\s+\S+\s+(\S+)')
WANTED = ('GEOM_', 'VR_HIT_', 'HYBRID_', 'SYNC_CONTACT', 'CONTACT_RECEIPT',
          'SUBMIT_HIT', 'FINISHER_RECEIPT', 'IMPALE', 'MELEE_INPUT', 'F3_FEINT_CHAIN',
          'SWING_EFFORT', 'NATIVE_COST', 'MOTION_OWNED', 'COMBO_', 'MS_GATE')

def audit(folder, run):
    main, diag = folder / f'{run}-main.log', folder / f'{run}-diag.log'
    modes, builds = [], []
    for n, line in enumerate(main.read_text(errors='replace').splitlines(), 1):
        if 'KCD2VR BUILD ' in line: builds.append({'line': n, 'text': line})
        if re.search(r'\bF4_PRESS[12]_', line):
            modes.append({'time': line[1:24], 'mode': 'HYBRID' if 'F4_PRESS1_' in line else 'DIRECT', 'line': n})
    # Main hotkey warnings omit mode changes from VRIK/F10/UI. The actual
    # diagnostic mode receipt is authoritative, including initial defaults.
    main_modes = modes
    modes = []
    diagnostic_lines = diag.read_text(errors='replace').splitlines()
    for n, line in enumerate(diagnostic_lines, 1):
        if ' MODE state=' not in line: continue
        state = dict(FIELD.findall(line)).get('state', 'UNKNOWN')
        mode = 'HYBRID' if 'F4_PRESS1_' in state else 'DIRECT' if 'F4_PRESS2_' in state else 'OTHER_OR_OFF'
        modes.append({'time': line[1:24], 'mode': mode, 'state': state, 'line': n})
    stamps = [x['time'] for x in modes]
    events, reasons, samples, applications = {}, {}, [], {}
    for n, line in enumerate(diagnostic_lines, 1):
        match = EVENT.search(line)
        if not match: continue
        event = match[1]
        if not any(x in event for x in WANTED): continue
        index = bisect.bisect_right(stamps, line[1:24])-1
        mode = modes[index]['mode'] if index >= 0 else 'BEFORE_F4_EDGE'
        fields = dict(FIELD.findall(line))
        events.setdefault(mode, collections.Counter())[event] += 1
        key = (mode, event, fields.get('reason', fields.get('rejected', fields.get('answer', 'UNPRINTED'))))
        reasons[key] = reasons.get(key, 0)+1
        if event == 'SUBMIT_HIT' and all(x in line for x in ('effect=APPLICATION', 'applyNative=1', 'returned=1')):
            role = 'HenryAttacker' if re.search(r'attacker=\d+\(PLAYER\) target=', line) else 'HenryVictim' if re.search(r'target=\d+\(PLAYER\)', line) else 'Other'
            bank = applications.setdefault(mode, {}).setdefault(role, collections.Counter())
            bank['records'] += 1
            for name in ('blocked', 'perfectBlock', 'masterStrike', 'combo', 'comboEnd'):
                if fields.get(name) == '1': bank[name] += 1
        if event in ('CONTACT_RECEIPT', 'GEOM_HIT', 'GEOM_SKIP', 'VR_HIT_SKIPPED', 'VR_HIT_ANSWERED_BY_NPC', 'HYBRID_CONTACT', 'SYNC_CONTACT', 'FINISHER_RECEIPT') or 'IMPALE' in line:
            samples.append({'line': n, 'time': line[1:24], 'mode': mode, 'event': event, 'fields': fields, 'text': line})
    return {'run': run, 'hashes': {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in (main, diag)}, 'builds': builds,
            'modeEdges': modes, 'mainHotkeyEdges': main_modes, 'eventRecords': events, 'reasons': [{'mode': k[0], 'event': k[1], 'reason': k[2], 'records': v} for k, v in sorted(reasons.items())],
            'nativeApplications': applications, 'records': samples,
            'limits': ['Existing bounded/capture-gated records, not all physical swings. Do not calculate global acceptance rate.',
                       'Mode uses exact diagnostic MODE receipts; main hotkey warnings alone omit VRIK/F10/UI changes.',
                       'Native application and physical contact records overlap; do not sum. Missing reactionDispatch is unknown.',
                       'Raw collision part is not a proven model-joint or successor-corpse part namespace.']}

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('folder', type=Path); parser.add_argument('output', type=Path)
    args = parser.parse_args()
    result = [audit(args.folder, run) for run in ('older', 'latest')]
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    for run in result:
        print(json.dumps({k: v for k, v in run.items() if k not in ('records',)}, indent=2))
