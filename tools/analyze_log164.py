"""Small provenance-preserving extraction; does not copy the historical raw-log corpus."""
import argparse
import hashlib
import json
import re
from pathlib import Path


def analyze(path):
    data = path.read_bytes()
    lines = data.decode('utf-8-sig', errors='replace').splitlines()
    def matches(text):
        return [dict(line=i, text=s) for i, s in enumerate(lines, 1) if text in s]
    groups = {k: matches(k) for k in ('KCD2VR BUILD', 'HAND_CONTACT', 'HIGGS_TARGET',
              'CONTACT_PATH', 'PROCESS_HIT', 'SUBMIT_HIT', 'V66.120 PREDICTION',
              'AUTHORITY', 'ATTACHMENT_WORLD', 'CLOSE_QUERY', 'SELF_CONTACT', 'RIGHT_BELT')}
    gaps = {}
    for label in ('torsoGap', 'otherHandGap'):
        entries = []
        sentinel = 0
        for e in groups['HAND_CONTACT']:
            m = re.search(r'\b' + label + r'M?=(-?[\d.]+)', e['text'])
            if not m:
                continue
            value = float(m[1])
            if value == -1:
                sentinel += 1
            elif value < 0:
                entries.append(e)
        gaps[label] = dict(negative_observations=entries, unavailable_sentinels=sentinel)
    selected = {}
    for key, entries in groups.items():
        if key == 'HAND_CONTACT':
            continue
        selected[key] = entries if len(entries) <= 16 else entries[:4] + entries[-12:]
    return dict(path=str(path), sha256=hashlib.sha256(data).hexdigest(), bytes=len(data), lines=len(lines),
                counts={k: len(v) for k, v in groups.items()}, gaps=gaps, selected=selected,
                interpretation=[
                    'Counts are recorded lines, often rate limited, not exhaustive event totals.',
                    'CONTACT_PATH READY is installation evidence, not a contact event.',
                    'Native returned hit and redirected sweep totals alone do not prove the same strike.',
                    'Pre-V164 waitingNative included unrelated attachments; not a local failure/FPS measurement.',
                    'HAND_CONTACT gap=-1 is unavailable, not one metre of penetration.',
                    'V162R1 logs do not validate V163 or V164 runtime behavior.'
                ])


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('logs', nargs='+', type=Path)
    p.add_argument('--out', required=True, type=Path)
    a = p.parse_args()
    result = [analyze(path) for path in a.logs]
    a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    for r in result:
        print(r['path'], r['sha256'], r['counts'])
