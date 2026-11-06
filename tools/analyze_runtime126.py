"""Extract V125R1 headset results for V126, retaining provenance rather than raw logs."""
import argparse
from collections import Counter, defaultdict
import hashlib
import json
from pathlib import Path
from analyze_runtime124 import extract, fields


def analyze(folder):
    files = [folder / n for n in ('KCD2VR.log', 'KCD2VR-diagnostics.log', 'KCD2VR.ini')]
    main, diagnostic, ini = [p.read_text(errors='replace') for p in files]
    report = extract(main, diagnostic)
    lines = diagnostic.splitlines()
    rows = lambda tag: [dict(time=s[1:24], **fields(s)) for s in lines if tag in s]
    report['sources'] = {str(p.resolve()): dict(bytes=p.stat().st_size, sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in files}
    for key, tag in [('bow_states', ' BOW_STATE '), ('body_gates', ' BODY_GATE '),
                     ('melee_inputs', ' MELEE_INPUT '), ('contacts', ' WEAPON_CONTACT '),
                     ('performance_windows', 'V66.122 PERF '), ('prediction_witnesses', ' PREDICTION_WITNESS '),
                     ('prediction_correlations', ' PREDICTION_CORRELATION '), ('melee_intents', ' MELEE_INTENT ')]:
        report[key] = rows(tag)
    report['catalogue_ready'] = [s for s in lines if 'CATALOG_READY' in s]
    report['bow_reasons'] = dict(Counter(x['reason'] for x in report['bow_states']))
    report['bow_cancel_events'] = sum(' BOW CANCEL ' in s for s in lines)
    commits = [r for r in report['melee_inputs'] if r.get('edge') == 'COMMIT' and r.get('handlerObserved') == '1']
    report['melee_commits'] = dict(total=len(commits), by_zone=dict(Counter(r['zone'] for r in commits)),
                                  by_stroke=dict(Counter(r['stroke'] for r in commits)))
    report['contact_samples'] = dict(Counter(r['response'] for r in report['contacts']))
    report['bow_release_requests'] = rows('PLAYER-ACTION REQUEST kind=6 ')
    report['holster_zones'] = rows(' HOLSTER_ZONE ')
    report['equipment_requests'] = rows(' EQUIPMENT_REQUEST ')
    report['equipment_acknowledgements'] = [r for r in report['holster_zones'] if r.get('ack') != 'NONE']
    groups = defaultdict(list)
    for r in report['performance_windows']:
        masks = [int(r.get(n, '0'), 16) for n in ('entryModeMaskF10', 'entryModeMaskF12', 'entryModeMaskF3', 'entryModeMaskF5')]
        if not all(m and not (m & (m-1)) for m in masks):
            continue
        groups[(r['stage'], *(m.bit_length()-1 for m in masks))].append(r)
    report['single_mode_performance'] = []
    for (stage, f10, f12, f3, f5), samples in sorted(groups.items()):
        calls = sum(int(r['calls']) for r in samples)
        report['single_mode_performance'].append(dict(stage=stage, f10=f10, f12=f12, f3=f3, f5=f5,
            windows=len(samples), calls=calls,
            weighted_us=sum(float(r['avgUs'])*int(r['calls']) for r in samples)/calls,
            query_us_per_scope=sum(float(r['avgPageQueryUs'])*int(r['calls']) for r in samples)/calls))
    report['camera_ini'] = [s for s in ini.splitlines() if s.startswith(('VrCameraBackOffsetMeters', 'VrCameraHeightOffsetMeters'))]
    report['qualifications'] += [
        'PERF scopes overlap and include wall time. Do not add nested scopes or infer FPS/GPU cost.',
        'Only windows with a single entry mode in every mask appear in single_mode_performance; locations/weapons still differ.',
        'avgPageQueryUs is total query time per timed scope, not time per individual API call.',
        'Contact samples with a plane prove the bounded static query found geometry; they are not accepted native damage.',
        'Two bow release requests are not two proven arrows; the user reported one successful shot.',
        'No native damage observations were logged; this does not establish that zero native hits occurred.',
        'Forward camera movement was reported by the user. The zoom hook logged suppression, but native position was not separated from HMD motion in this run.'
        ,'Holster ATTACHMENT_CHANGED acknowledges native hand-key membership only, not a verified rendered stowed mount. The user still reports hanging weapons.'
    ]
    return report


if __name__ == '__main__':
    p = argparse.ArgumentParser(); p.add_argument('input', type=Path); p.add_argument('output', type=Path); a = p.parse_args()
    report = analyze(a.input); a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    print(json.dumps({k: report[k] for k in ('runtime_identification', 'bow_reasons', 'melee_commits', 'contact_samples', 'native_damage_summary')}, indent=2))
