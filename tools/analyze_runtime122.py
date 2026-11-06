"""Extract focused runtime evidence; missing records are not successful checks."""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import re


def analyze(lines):
    select = lambda tag: [s for s in lines if tag in s]
    number = lambda s, key: re.search(r'\b' + re.escape(key) + r'=(-?[\d.]+)', s)
    gates = {}
    for line in select('PREDICTION_WITNESS '):
        match = re.search(r'gates=\{([^}]*)\}', line)
        if match:
            for name, count in re.findall(r'(\w+):(\d+)', match[1]):
                gates[name] = max(gates.get(name, 0), int(count))
    maxima = {}
    for tag, keys in (
        ('CAMERA_BODY ', ('targetErrorM', 'ageMs')),
        ('WEAPON_WORLD ', ('publishDeltaM', 'publishDeltaRad', 'rootDeltaM', 'rootDeltaRad', 'weaponWorldGapM', 'frameAgeMs')),
    ):
        for line in select(tag):
            for key in keys:
                match = number(line, key)
                if match and float(match[1]) >= 0 and (key not in maxima or float(match[1]) > maxima[key]['value']):
                    maxima[key] = dict(value=float(match[1]), record=line)
    camera_z = [float(m[1]) for s in select('CAMERA_BODY ') if (m := number(s, 'cameraMinusSolvedZ'))]
    perf = {}
    for line in select('V66.122 PERF '):
        name = re.search(r'stage=(\w+)', line)
        count, avg, maximum = (number(line, k) for k in ('calls', 'avgUs', 'maxUs'))
        if not all((name, count, avg, maximum)):
            continue
        p = perf.setdefault(name[1], dict(calls=0, total_wall_us=0, max_wall_us=0, page_queries=0))
        p['calls'] += int(count[1]); p['total_wall_us'] += int(count[1])*float(avg[1])
        p['max_wall_us'] = max(p['max_wall_us'], float(maximum[1]))
        if (q := number(line, 'pageQueries')): p['page_queries'] += int(q[1])
    for p in perf.values(): p['weighted_avg_wall_us'] = p['total_wall_us']/p['calls'] if p['calls'] else None
    tags = ('PREDICTION_WITNESS ', 'CAMERA_BODY ', 'WEAPON_WORLD ', 'V66.122 PERF ')
    present = {t.strip(): len(select(t)) for t in tags}
    paths = select('CONTACT_PATH session=')
    watches = {}
    for line in select('INTERACTION_BODY session='):
        seq, session = number(line, 'requestSeq'), number(line, 'session')
        stage = re.search(r'stage=(WATCH|RETURN)', line)
        if not all((seq, session, stage)): continue
        w = watches.setdefault(session[1]+':'+seq[1], dict(watch_records=0, native_return_records=0, bodies=[], callers=[]))
        if stage[1] == 'WATCH': w['watch_records'] += 1
        else:
            w['native_return_records'] += 1
            for field, output in (('body', 'bodies'), ('callerRva', 'callers')):
                m = re.search(r'\b'+field+r'=([0-9a-fA-F]+)', line)
                if m and m[1] not in w[output]: w[output].append(m[1])
    return dict(records=len(lines), diagnostic_records=present,
                missing_diagnostics=[k for k, v in present.items() if not v],
                prediction_first_failure_totals=gates,
                pose_reject_sample_counts=dict(Counter(m[1] for s in select('POSE-REJECT ') if (m := re.search(r'reason=(\w+)', s)))),
                largest_observed=maxima,
                camera_minus_solved_z_range=[min(camera_z), max(camera_z)] if camera_z else None,
                performance=perf,
                reference_contact_paths=dict(records=len(paths),
                    candidate_copy_records=sum(bool((m:=number(s,'candidateCopied')) and int(m[1])) for s in paths),
                    stop_masks=dict(Counter(m[1] for s in paths if (m:=number(s,'stopMask'))))),
                reference_interaction_watches=watches,
                qualifications=[
                    'Gate totals are cumulative maxima within one process log; not counts of sampled print lines.',
                    'A witness state is observed AFTER native return; it is not the earlier admission state.',
                    'Camera is one immersive eye, head is a bone. Their unmeasured offset need not be zero.',
                    'Camera/body records retain distinct sample identities; identical render pose is not proven.',
                    'Timing measures wall duration, may overlap and include scheduling; do not sum scope times as CPU cost.',
                    'A missing diagnostic is missing evidence, not a passing test or zero failures.',
                    'Contextual use is still not a held physical object constraint.',
                    'Contact-path masks describe native branches within one callback, not accepted damage or a parry.',
                    'No observed getter return does not mean no physics body. Null return is a distinct observed result; request watches expire after 3 seconds.'
                ])


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('input', type=Path, help='Folder containing the two game logs')
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    paths = [args.input/n for n in ('KCD2VR.log', 'KCD2VR-diagnostics.log')]
    result = analyze(paths[1].read_text(encoding='utf-8', errors='replace').splitlines())
    result['sources'] = {p.name: dict(bytes=p.stat().st_size, sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in paths}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    print(json.dumps({k: result[k] for k in ('diagnostic_records', 'missing_diagnostics', 'prediction_first_failure_totals')}, indent=2))


if __name__ == '__main__': main()
