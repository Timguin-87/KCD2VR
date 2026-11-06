#!/usr/bin/env python3
"""Summarize V169R1 passive contact evidence without calling contacts 'hits'.
Input: full main/diagnostic logs or a ZIP of logs. No game/SDK dependency.
Raw ray bytes are witnesses, NOT decoded materials, NPCs, or borrowed pointers.
"""
from __future__ import annotations
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import re
import zipfile

LIMIT = 512 * 1024 * 1024
BUILD = re.compile(r"KCD2VR BUILD\s+(KCD2VR-[A-Za-z0-9_.-]+)")
FIELDS = re.compile(r"([A-Za-z][A-Za-z0-9_]*)=([^\s]+)")
EXPECTED = "KCD2VR-V66.169R1-PASSIVE-CONTACT"

def sources(paths: list[Path]):
    total = 0
    for path in paths:
        if zipfile.is_zipfile(path):
            with zipfile.ZipFile(path) as z:
                for info in z.infolist():
                    if info.is_dir() or not info.filename.lower().endswith('.log'):
                        continue
                    total += info.file_size
                    if total > LIMIT:
                        raise ValueError('Log input exceeds the 512 MiB uncompressed budget')
                    yield f'{path.name}::{info.filename}', z.read(info)
        else:
            total += path.stat().st_size
            if total > LIMIT:
                raise ValueError('Log input exceeds the 512 MiB budget')
            yield path.name, path.read_bytes()

def summarize(items):
    report = {'schema': 'kcd2vr.passive-contact.169r1.v1', 'files': [], 'duplicates': [],
              'builds': [], 'sources': {}, 'nativeMaterialRecords': 0,
              'nativeMaterialWithExactStackIds': 0, 'rawRayRecords': 0,
              'lastQueueCounters': {}, 'coverage': [], 'examples': [], 'decisions': []}
    seen = {}; builds = set(); episodes = set(); counts = {}
    for name, data in items:
        digest = hashlib.sha256(data).hexdigest()
        if digest in seen:
            report['duplicates'].append({'file': name, 'sameAs': seen[digest], 'sha256': digest})
            continue
        seen[digest] = name
        text = data.decode('utf-8-sig', errors='replace')
        local_builds = set(BUILD.findall(text)); builds.update(local_builds)
        report['files'].append({'file': name, 'sha256': digest, 'bytes': len(data), 'builds': sorted(local_builds)})
        for number, line in enumerate(text.splitlines(), 1):
            f = dict(FIELDS.findall(line))
            if 'V66.169R1 CONTACT_EVENT ' in line:
                key = f.get('source', 'UNKNOWN'); c = counts.setdefault(key, Counter())
                c['records'] += 1; c['phase:' + f.get('phase', 'UNKNOWN')] += 1
                c['status:' + f.get('status', 'UNKNOWN')] += 1
                if f.get('hapticQueued') == '1': c['hapticRequestsQueued'] += 1
                if f.get('eligible') == '1' and f.get('worldCoherent') == '0': c['worldMismatchRecords'] += 1
                if f.get('witness', '0') != '0': c['recordsWithRawWitness'] += 1
                if f.get('phase') == 'BEGIN':
                    identity = (digest, key) + tuple(f.get(k, '') for k in ('actor','key','generation','epoch','request','side','episode'))
                    if identity not in episodes: episodes.add(identity); c['observedBeginEpisodes'] += 1
                if len(report['examples']) < 40 and (f.get('phase') in ('BEGIN','SLIDE','SUSPENDED','END') or f.get('worldCoherent') == '0'):
                    report['examples'].append({'file': name, 'line': number, 'text': line})
            elif 'V66.169R1 CONTACT_RAY ' in line:
                report['rawRayRecords'] += 1
            elif 'V66.169R1 CONTACT_COVERAGE ' in line:
                report['coverage'].append({'file': name, 'line': number, 'text': line})
            elif 'V66.169R1 CONTACT_SUMMARY ' in line:
                report['lastQueueCounters'][name] = {k: f.get(k, 'UNKNOWN') for k in ('dropped','rawRateLimited')}
            elif 'V66.116 MATERIAL_EFFECT ' in line:
                report['nativeMaterialRecords'] += 1
                if f.get('prediction', '0') != '0' or f.get('sweep', '0') != '0' or f.get('contact', '0') != '0':
                    report['nativeMaterialWithExactStackIds'] += 1
    report['builds'] = sorted(builds)
    report['sources'] = {k: dict(v) for k, v in sorted(counts.items())}
    has_current = EXPECTED in builds
    report['currentBuildMarkerPresent'] = has_current
    report['decisions'].append('Only records with the new internal marker can contribute to V169R1 runtime acceptance.' if not has_current else
        'Internal marker found. Source records and queued haptics do not establish visible/physical or XR-delivered success.')
    world = counts.get('WEAPON_WORLD', Counter())
    if world['phase:SLIDE']:
        report['decisions'].append('Permitted sliding was observed. Passive sparks/audio remain IMPLEMENTATION_REQUIRED; do not force combat hits to produce them.')
    if any(c['status:UNKNOWN'] for c in counts.values()):
        report['decisions'].append('UNKNOWN contacts exist: inspect fresh-query/budget/ownership and final-pose flags. Do not restore movement through untested geometry.')
    if any(c['worldMismatchRecords'] for c in counts.values()):
        report['decisions'].append('Final publication world mismatch exists: coordinate the checked and committed world transform before adding effects.')
    report['decisions'].extend([
        'NPC hand geometry is NOT_QUERIED by these static rays/self proxies. Missing NPC records are not a negative collision test.',
        'Raw80 is not a decoded material or NPC identity. It may help validate exact retail hit-layout/material ownership without dereferencing recorded pointers.',
        'Native material records with current-stack IDs describe native effects, not effects emitted by the passive observer; timestamps alone are not a causal join.',
        'Self-torso and other-palm results are approximations, not full armor/finger meshes. Negative final gaps are unqualified, not resolved contacts.',
        'Queue counters count dropped/rate-limited observations; hapticRequestsQueued is not an OpenXR success count or damage count.'
    ])
    return report

def markdown(r):
    lines = ['# Passive contact evidence — V169R1', '',
             'Build markers: ' + (', '.join(r['builds']) or 'NONE'), '',
             'These are diagnostic records, not accepted-hit totals.', '',
             '| Source | Records | Begin episodes observed | Haptics queued | UNKNOWN records |',
             '|---|---:|---:|---:|---:|']
    for source, c in r['sources'].items():
        lines.append(f"| {source} | {c.get('records',0)} | {c.get('observedBeginEpisodes',0)} | {c.get('hapticRequestsQueued',0)} | {c.get('status:UNKNOWN',0)} |")
    lines.extend(['', '## Interpretation', ''] + [str(x) + '\n' for x in r['decisions']])
    lines.extend(['', '## Selected original lines', ''])
    for x in r['examples'][:12]:
        lines.extend([f"{x['file']}:{x['line']}", '```text', x['text'], '```', ''])
    return '\n'.join(lines)

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('inputs', type=Path, nargs='+')
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--markdown', type=Path)
    args = parser.parse_args()
    try:
        r = summarize(sources(args.inputs))
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(json.dumps(r, indent=2) + '\n', encoding='utf-8')
        if args.markdown:
            args.markdown.parent.mkdir(parents=True, exist_ok=True)
            args.markdown.write_text(markdown(r), encoding='utf-8')
    except (OSError, ValueError, zipfile.BadZipFile) as exc:
        parser.exit(2, f'Error: {exc}\n')
    return 0

if __name__ == '__main__':
    raise SystemExit(main())
