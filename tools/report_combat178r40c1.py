"""Extract bounded combat evidence from an existing log; never execute the game."""
import argparse
import collections
import hashlib
import json
import re
from pathlib import Path


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--log', type=Path, required=True)
    ap.add_argument('--out', type=Path, required=True)
    a = ap.parse_args()
    raw = a.log.read_bytes()
    lines = raw.decode('utf-8', errors='replace').splitlines()
    groups = {key: [] for key in ('build', 'capture', 'modes', 'summary', 'hit', 'wait', 'guard')}
    tokens = {'build': 'KCD2VR BUILD ', 'capture': 'V66.168 CAPTURE ', 'modes': 'F4_RUNG',
              'summary': 'GEOM_HIT_SUMMARY ', 'hit': 'GEOM_LIVE_HIT ',
              'wait': 'GEOM_LIVE_WAIT ', 'guard': 'F4_BLOCK_EDGE '}
    rejected = collections.Counter()
    strokes = collections.Counter()
    rung_last = None
    releases = []
    for n, line in enumerate(lines, 1):
        for group, token in tokens.items():
            if token in line:
                groups[group].append({'line': n, 'text': line})
        if 'GEOM_SKIP ' in line or 'GEOM_HIT ' in line:
            reason = re.search(r'(?:reason|rejected)=([^ ]+)', line)
            if reason:
                rejected[reason[1]] += 1
        if ' RUNG1 active=' in line:
            reason = re.search(r'stroke=([^ ]+)', line)
            if reason:
                strokes[reason[1]] += 1
            rung_last = {'line': n, 'text': line}
        if ' GUARD edge=' in line:
            releases.append({'line': n, 'text': line})
    report = {'input': str(a.log), 'sha256': hashlib.sha256(raw).hexdigest(),
              'bytes': len(raw), 'lines': len(lines), 'first': lines[0], 'last': lines[-1],
              'record_counts': {key: len(value) for key, value in groups.items()},
              'bounded_records': {key: value[:80] for key, value in groups.items()},
              'guard_edges': releases[:80], 'skip_record_counts': dict(rejected),
              'sampled_stroke_reasons': dict(strokes), 'last_rung_summary': rung_last,
              'interpretation': 'Detail lines are capture/rate limited; counters may be cumulative across mode intervals. Input/handler receipts are not proof of health damage or reaction playback.'}
    a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps({'record_counts': report['record_counts'], 'skip_record_counts': dict(rejected),
                      'sampled_stroke_reasons': dict(strokes)}, indent=2))


if __name__ == '__main__':
    main()
