"""Extract the V120 regression run without shipping its raw logs."""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import re


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('input', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    paths = [args.input / name for name in ('KCD2VR.log', 'KCD2VR-diagnostics.log')]
    sources = {p.name: {'bytes': p.stat().st_size, 'sha256': hashlib.sha256(p.read_bytes()).hexdigest()} for p in paths}
    lines = paths[1].read_text(encoding='utf-8', errors='replace').splitlines()
    select = lambda text: [line for line in lines if text in line]
    def counts(text, pattern):
        return dict(Counter(m.group(1) for line in select(text) if (m := re.search(pattern, line))))
    groups = {}
    for name, tag in {
        'prediction': 'V66.120 PREDICTION ', 'native_sweep': 'NATIVE-WEAPON-SWEEP',
        'bow': 'BOW_STATE ', 'melee_input': 'MELEE_INPUT ', 'melee_intent': 'MELEE_INTENT ',
        'contacts': 'WEAPON_CONTACT ', 'holster': 'HOLSTER GESTURE',
        'head': 'HEAD_BODY ', 'pose_reject': 'POSE-REJECT ',
    }.items():
        selected = select(tag)
        groups[name] = {'records': len(selected), 'first': selected[:2], 'last': selected[-2:]}
    # These are sampled records, not frame counts or accepted gameplay outcomes.
    result = dict(sources=sources, first=lines[0], last=lines[-1], records=len(lines), groups=groups,
                  melee_edges=counts('MELEE_INPUT ', r'edge=(\w+)'),
                  melee_zones=counts('MELEE_INPUT edge=COMMIT', r'zone=(\d+)'),
                  melee_rejections=counts('MELEE_INPUT ', r'rejected=(\w+)'),
                  intent_reasons=counts('MELEE_INTENT ', r'reason=(\w+)'),
                  bow_reasons=counts('BOW_STATE ', r'reason=(\w+)'),
                  contact_deferred=counts('WEAPON_CONTACT ', r'deferred=(\d+)'),
                  prediction_reasons=counts('V66.120 PREDICTION ', r'reason=(\d+)'),
                  f12_changes=select('V66.88-F12-WEAPON rung='),
                  lab_changes=select('V66.114-LAB'),
                  standing_reference_changes=select('STANDING_REFERENCE'),
                  warnings=['Native input dispatch is not accepted damage or block proof.',
                            'Context reason 2 in V120 does not identify its failed predicate.',
                            'Contact deferred=1 records describe checked rotation prefixes, not material impacts.'])
    perf = {}
    for line in select('PERF'):
        m = re.search(r'(POSE_SOLVE|NATIVE_SKINNING|PHYSICAL_PLACEMENT|NATIVE_FRAME|EQUIPMENT_READ|NATIVE_STATE_READBACK).*?avgUs=([\d.]+)', line)
        if m and (m[1] not in perf or float(m[2]) > perf[m[1]]['avg_us']):
            perf[m[1]] = dict(avg_us=float(m[2]), record=line)
    result['worst_average_windows'] = perf
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({k: result[k] for k in ('records', 'melee_edges', 'melee_zones', 'bow_reasons', 'contact_deferred', 'prediction_reasons')}, indent=2))


if __name__ == '__main__':
    main()
