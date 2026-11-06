"""Focused evidence extraction. Never equate a collision candidate with accepted damage."""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import re
from analyze_runtime122 import analyze


def fields(line):
    out = dict(re.findall(r'(\w+)=("[^"]*"|\([^)]*\)|\{[^}]*\}|\S+)', line))
    return {k: v.strip('"') for k, v in out.items()}


def extract(main, diagnostic):
    lines = diagnostic.splitlines()
    report = analyze(lines)
    select = lambda text: [s for s in lines if text in s]
    records = lambda text: [dict(time=s[1:24], **fields(s)) for s in select(text)]
    report['runtime_identification'] = dict(
        markers=sorted(set(re.findall(r'KCD2VR BUILD (\S+)', main))),
        v123_arm_messages=len(select('V66.123-ARMS ')),
        v124_arm_messages=len(select('V66.124-ARMS ')),
        v109_retarget_messages=len(select('V66.109-REACH-RETARGET ')),
        rule='Supplied source ZIP name does not identify the executable that produced a log.')
    report['mode_timeline'] = [s for s in lines if 'V66.114-LAB tick=' in s or 'V66.94-F12 rung=' in s]
    report['handling_samples'] = dict(Counter(s.get('releaseReasonRL', 'missing') for s in records('HANDLING F12=')))
    report['handling_contact_states'] = dict(Counter(
        (re.search(r'R=\{([^}]*)\}', s)[1]).split('contactKnown:')[-1] for s in select('HANDLING F12=') if 'R={' in s))
    report['budget_skips_max'] = max((int(s['budgetSkips']) for s in records('CONTACT BUDGET ') if 'budgetSkips' in s), default=None)
    report['contact_paths'] = records('CONTACT_PATH session=')
    report['contact_roles'] = dict(Counter(s.get('HenryRole') for s in report['contact_paths']))
    # Material stages share a dispatcher serial. This join is causal within that
    # dispatcher only; it does not link a tracked trajectory or contact callback.
    materials = {}
    for s in records('MATERIAL_EFFECT session='):
        key = s['session'] + ':' + s['serial']
        stages = materials.setdefault(key, dict(lookup=[], execute=[], returned=[]))
        stages[{'1': 'lookup', '2': 'execute', '0': 'returned'}[s['stage']]].append(s)
    report['material_dispatches'] = materials
    report['material_lookup_counts'] = dict(Counter(
        f"{s['row']} | surface={s['surfaceId']} | effect={s['effectId']}"
        for x in materials.values() for s in x['lookup']))
    report['native_damage_observations'] = records('status=NATIVE_DAMAGE_OBSERVED ')
    report['native_damage_summary'] = dict(
        records=len(report['native_damage_observations']),
        positive_health=sum(float(s.get('healthDamage', 0)) > 0 for s in report['native_damage_observations']),
        positive_stamina=sum(float(s.get('staminaDamage', 0)) > 0 for s in report['native_damage_observations']),
        attribution='Native damage observations; no automatic tracked-blade attribution.')
    report['interaction_records'] = records('INTERACTION_BODY session=')
    report['prediction_records'] = records('V66.120 PREDICTION F12=')
    report['qualifications'] += [
        'Handling and pose rejection records are sampled diagnostics, not per-frame totals.',
        'Surface IDs are runtime IDs. XML column order does not establish their names.',
        'No contact-qualified parry/master strike is inferred from animation names or damage timing.',
        'Contact paths, material dispatcher serials and damage observations are separate causal domains.'
    ]
    return report


def main():
    p = argparse.ArgumentParser()
    p.add_argument('input', type=Path); p.add_argument('output', type=Path)
    a = p.parse_args()
    main_log = a.input/'KCD2VR.log'; diagnostic = a.input/'KCD2VR-diagnostics.log'
    report = extract(main_log.read_text(errors='replace'), diagnostic.read_text(errors='replace'))
    report['sources'] = {f.name: dict(bytes=f.stat().st_size, sha256=hashlib.sha256(f.read_bytes()).hexdigest())
                         for f in (main_log, diagnostic)}
    a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    print(json.dumps({k: report[k] for k in ('runtime_identification', 'reference_contact_paths', 'contact_roles', 'material_lookup_counts', 'native_damage_summary', 'budget_skips_max')}, indent=2))


if __name__ == '__main__': main()
