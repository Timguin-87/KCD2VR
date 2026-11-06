#!/usr/bin/env python3
"""Read-only bellows evidence extraction from the original diagnostic log.

Counts emitted observations, not every game frame. Timestamp coincidence is
reported without claiming raw XR and native WORLD poses are the same sample.
"""
import argparse
from collections import Counter
import hashlib
import json
import math
from pathlib import Path
import re


def audit(path, out):
    sessions = {}
    stages, sources, inputs, roots, alchemy, excerpts = [], [], [], [], [], []
    digest = hashlib.sha256()
    with path.open('rb') as stream:
        for number, raw in enumerate(stream, 1):
            digest.update(raw)
            line = raw.decode('utf-8', errors='replace').rstrip('\r\n')
            if not line.startswith('['):
                continue
            stamp = line[1:24]
            fields = dict(re.findall(r'(\w+)=([^ ]+)', line))
            row = {'line': number, 'time': stamp, **fields}
            if ' VRIK_F7 POSE ' in line:
                session = fields.get('session', '?')
                if session not in sessions:
                    sessions[session] = {'rows': 0, 'first': stamp, 'last': stamp, 'first_line': number}
                sessions[session]['rows'] += 1
                sessions[session]['last'] = stamp
                sessions[session]['last_line'] = number
            selected = False
            if ' D42_F5 kind=SMITH_BELLOWS event=STAGE ' in line:
                stages.append(row)
                selected = True
            elif ' C71 F5_CRAFT_SOURCE ' in line and fields.get('slot') == '2':
                sources.append(row)
                selected = True
            elif ' C71 F5_CRAFT_INPUT ' in line and fields.get('kind') == 'SMITHING':
                inputs.append(row)
                selected = fields.get('phase') == '8'
            elif ' C71 F5_CRAFT_NATIVE_ROOT ' in line:
                roots.append(row)
                selected = '00:55:22' <= stamp[11:] <= '00:55:51'
            elif ' D42_F5 kind=ALCHEMY ' in line and fields.get('admitted', '').endswith('/18'):
                alchemy.append(row)
                selected = True
            if selected:
                excerpts.append(f'{number}: {line}')
    phase8 = [r for r in stages if r.get('phase') == '8']
    handle_rows = [r for r in sources if r.get('phase') == '8' and r.get('entity') == '983380']
    valid_gaps = [float(r['gapR']) for r in handle_rows if float(r.get('gapR', '-1')) >= 0]
    credit = [r for r in alchemy if r.get('status') == 'BELLOWS_PUMP_CREDIT']
    example = None
    for target in handle_rows:
        matching = [r for r in inputs if r.get('tick') == target.get('tick') and
                    r.get('instance') == target.get('instance') and r.get('life') == target.get('life')]
        if not matching:
            continue
        palm = matching[-1]
        xyz = lambda key, record: list(map(float, record[key].split(',')))
        a, b = xyz('sourceWorld', target), xyz('palmWorldR', palm)
        delta = [x-y for x, y in zip(a, b)]
        example = {'source': target, 'palm_input': palm, 'delta_metres': delta,
                   'distance_metres': math.sqrt(sum(x*x for x in delta)),
                   'horizontal_metres': math.hypot(delta[0], delta[1]),
                   'vertical_metres': abs(delta[2]),
                   'join_limit': 'Same reported tick, instance and lifetime; source has no input sequence, so exact same-pose-sample identity is unproven.'}
        break
    interval = {'first': phase8[0]['time'], 'last': phase8[-1]['time']} if phase8 else None
    overlaps = [session for session, bounds in sessions.items() if interval and
                bounds['first'] <= interval['last'] and bounds['last'] >= interval['first']]
    result = {
        'input': str(path.resolve()), 'sha256': digest.hexdigest(), 'total_lines': number,
        'scope': 'Emitted observation evidence, not headset success or a complete native event stream',
        'f7_pose_sessions': sessions, 'smith_phase8_window': interval, 'overlapping_f7_sessions': overlaps,
        'smith_bellows_emitted_rows': len(stages), 'smith_phase8_emitted_rows': len(phase8),
        'smith_reasons': dict(Counter(r.get('reason') for r in stages)),
        'smith_acquired_rows': sum(r.get('acquired') == '1' for r in stages),
        'smith_requested_rt_rows': sum(r.get('requestedRT') == '1' for r in stages),
        'smith_phase8_source_rows': len(handle_rows),
        'smith_phase8_source_gapR_min_max_metres': [min(valid_gaps), max(valid_gaps)] if valid_gaps else None,
        'smith_source_palm_example': example,
        'alchemy_bellows_credit_rows': len(credit),
        'alchemy_max_reported_cumulative_credit_seconds': max((float(r.get('credit', 0)) for r in alchemy), default=0),
        'alchemy_first_credit': credit[0] if credit else None,
        'alchemy_native_hold_hints': dict(Counter(r.get('nativeHoldHint') for r in alchemy)),
        'native_heat_or_recipe_result': 'UNPROVEN',
    }
    out.mkdir(parents=True, exist_ok=True)
    (out/'LOG_EVIDENCE.json').write_text(json.dumps(result, indent=2)+'\n')
    (out/'selected_original_rows.txt').write_text('\n'.join(excerpts)+'\n')
    return result


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('log', type=Path)
    parser.add_argument('--out', type=Path, required=True)
    arguments = parser.parse_args()
    result = audit(arguments.log, arguments.out)
    print(json.dumps({key: result[key] for key in [
        'sha256', 'f7_pose_sessions', 'smith_phase8_window', 'overlapping_f7_sessions',
        'smith_bellows_emitted_rows', 'smith_phase8_emitted_rows', 'smith_reasons',
        'smith_acquired_rows', 'smith_requested_rt_rows', 'smith_phase8_source_gapR_min_max_metres',
        'alchemy_bellows_credit_rows', 'alchemy_max_reported_cumulative_credit_seconds']}, indent=2))
