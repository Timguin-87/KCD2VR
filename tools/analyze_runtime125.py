"""Extract V124R1 headset evidence for the V125 review, without raw-log delivery."""
import argparse
from collections import Counter, defaultdict
import hashlib
import json
from pathlib import Path
from analyze_runtime124 import extract, fields


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('input', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    logs = [args.input / name for name in ('KCD2VR.log', 'KCD2VR-diagnostics.log')]
    main_log, diagnostic = [p.read_text(errors='replace') for p in logs]
    lines = diagnostic.splitlines()
    records = lambda text: [dict(time=s[1:24], **fields(s)) for s in lines if text in s]
    report = extract(main_log, diagnostic)
    report['sources'] = {p.name: dict(bytes=p.stat().st_size, sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in logs}
    report['camera_events'] = [s for s in main_log.splitlines() if 'recenter' in s.lower()]
    for key, tag in [('body_gates', 'BODY_GATE '), ('body_reanchors', 'SESSION_REANCHORED'),
                     ('arm_measurements', '-ARMS '), ('head_ownership', 'HEAD_AIM '),
                     ('weapon_contacts', 'WEAPON_CONTACT '), ('melee_intents', 'MELEE_INTENT '),
                     ('melee_inputs', 'MELEE_INPUT '), ('prediction_witnesses', 'PREDICTION_WITNESS '),
                     ('performance_windows', 'V66.122 PERF ')]:
        report[key] = records(tag)
    report['weapon_contact_counts'] = dict(Counter(r.get('response') for r in report['weapon_contacts']))
    report['melee_input_counts'] = dict(Counter('|'.join(f'{k}={r.get(k, "-")}' for k in ('edge', 'stroke', 'zone', 'rejected')) for r in report['melee_inputs']))
    grouped = defaultdict(list)
    for row in report['performance_windows']:
        grouped[(row['stage'], row['modesAtPrintF10'], row['modesAtPrintF12'])].append(row)
    report['performance_summary'] = [dict(stage=stage, f10_at_print=int(f10), f12_at_print=int(f12),
        windows=len(rows), calls=sum(int(r['calls']) for r in rows),
        weighted_avg_us=sum(float(r['avgUs'])*int(r['calls']) for r in rows)/sum(int(r['calls']) for r in rows),
        worst_window_avg_us=max(float(r['avgUs']) for r in rows), largest_call_us=max(float(r['maxUs']) for r in rows))
        for (stage, f10, f12), rows in sorted(grouped.items())]
    report['qualifications'] += [
        'Performance scopes are nested wall timers, not exclusive CPU cost or GPU/frame-time measurements. Do not add them.',
        'Old mode-at-print tags can include samples from a different rung. Weighted averages are indicative, not controlled A/B benchmarks.',
        'A contact with deferred=1 and plane=0 is not an impact. Old pointWorld in those rows is the model origin, not measured geometry.',
        'An upper-zone input reaching the native handler does not prove the native game chose an overhead animation outside combat.',
    ]
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    print(json.dumps({k: report[k] for k in ('runtime_identification', 'weapon_contact_counts', 'melee_input_counts', 'native_damage_summary')}, indent=2))


if __name__ == '__main__':
    main()
