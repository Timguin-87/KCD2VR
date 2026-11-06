"""Summarize the bounded F5 lifecycle stream without inventing physical ownership.
Usage: python tools/report_f5_178r36c3.py KCD2VR-diagnostics.log --out f5-report.json
"""
from collections import Counter, defaultdict
from pathlib import Path
import argparse, hashlib, json, re

FIELDS = re.compile(r'(\w+)=([^\s]+)')

def number(record, key, default=0, base=10):
    try:
        return int(record.get(key, str(default)), base)
    except ValueError:
        return default

def summarize(lines):
    sessions = {}
    def session(sid):
        return sessions.setdefault(sid, dict(events=[], groups={}, coverage=None, end=None))
    for line_no, line in enumerate(lines, 1):
        r = dict(FIELDS.findall(line)); sid = number(r, 'session')
        if not sid:
            continue
        if ' F5_CAPTURE ' in line:
            session(sid)['coverage'] = r
        elif ' F5_GROUP ' in line:
            session(sid)['groups'][r.get('group', '?')] = r
        elif ' F8_LIFECYCLE_END ' in line:
            session(sid)['end'] = r
        elif ' F8_LIFECYCLE ' in line:
            r['line'] = line_no
            session(sid)['events'].append(r)
    result = []
    for sid, data in sessions.items():
        events = sorted(data['events'], key=lambda r: (number(r, 'tick'), r['line']))
        counts = Counter(r.get('event', '?') for r in events)
        end = data['end'] or {}
        loss = {k: number(end, k) for k in ('dropped', 'ledgerOverflow', 'identityConflicts', 'scopeLost', 'watchLost', 'budgetLost')}
        expected = {'PICKUP', 'HAND_CHANGE', 'ALCHEMY', 'REMOVAL', 'CONSTRAINT', 'PHYSIC_PARAMS'}
        groups_ready = set(data['groups']) == expected and all(number(r, 'ready') == 1 for r in data['groups'].values())
        coverage_ready = data['coverage'] is not None and number(data['coverage'], 'ready') == 1
        transport_complete = coverage_ready and groups_ready and bool(end) and not any(loss.values())
        objects = defaultdict(list)
        for r in events:
            key = number(r, 'wuid', base=16)
            if key:
                objects[f'{key:016X}'].append(r)
        items = []
        for key, records in objects.items():
            identities = sorted({(r.get('target'), r.get('entity')) for r in records if number(r, 'target')})
            intervals, active = [], {}
            for r in records:
                side, kind = number(r, 'side', -1), r.get('event')
                if kind == 'HAND_ENTER':
                    if side in active:
                        intervals.append(dict(side=side, start=active[side]['tick'], end=None, issue='duplicate enter; gap/ownership ambiguous'))
                    active[side] = r
                elif kind == 'HAND_LEAVE':
                    start = active.pop(side, None)
                    intervals.append(dict(side=side, start=start.get('tick') if start else None, end=r['tick'],
                                          duration_ms=number(r,'tick')-number(start,'tick') if start else None,
                                          evidence='NATIVE_HAND_SLOT_ONLY'))
            intervals.extend(dict(side=side, start=r['tick'], end=None, evidence='OPEN_AT_CAPTURE_END') for side,r in active.items())
            # A grip edge remains input intent. Do not attach it to the last
            # target, or infer a physical release from a native hand leave.
            grip_within = []
            for interval in intervals:
                if interval.get('start') is None or interval.get('end') is None:
                    continue
                for r in events:
                    if r.get('event') == 'GRIP_SAMPLE' and number(r,'side',-1) == interval['side'] and int(interval['start']) <= number(r,'tick') <= int(interval['end']):
                        grip_within.append(dict(line=r['line'], tick=r['tick'], reason=r.get('reason'), evidence='INPUT_DURING_NATIVE_HAND_INTERVAL'))
            items.append(dict(key=key, identities=identities, identity_ambiguous=len(identities)>1,
                              counts=dict(Counter(r['event'] for r in records)), native_hand_intervals=intervals,
                              grip_within_native_interval=grip_within,
                              physical_hold='UNPROVEN', destruction='UNOBSERVED'))
        calls = defaultdict(list)
        for r in events:
            if number(r,'call'):
                calls[number(r,'call')].append(r)
        physics = []
        for call, records in calls.items():
            submits = [r for r in records if r.get('event') == 'PHYSICS_SUBMIT']
            for submit in submits:
                returns = [r for r in records if r.get('event') == 'PHYSICS_ACTION_RETURN' and
                           (r.get('body'), r.get('target'), r.get('entity'), r.get('actionRaw')) ==
                           (submit.get('body'), submit.get('target'), submit.get('entity'), submit.get('actionRaw')) and
                           number(r,'tick') >= number(submit,'tick')]
                wrappers = [r for r in records if r.get('event') == 'SET_PHYSIC_PARAMS_RETURN']
                native = returns[0] if len(returns)==1 else None
                physics.append(dict(call=call, type=submit.get('actionRaw'), body=submit.get('body'), target=submit.get('target'),
                                    constraint_id=native.get('constraintId') if native else submit.get('constraintId'),
                                    parsed_fields_valid=number(submit,'valid')==1, native_return=native.get('resultRaw') if native else None,
                                    wrapper_returns=[r.get('resultRaw') for r in wrappers],
                                    cleanup='RETURN_OBSERVED_NOT_DESTRUCTION' if native else 'MISSING_NATIVE_RETURN'))
        missing = []
        for stage, kinds in [('native acquisition', {'PICKUP_REQUEST','PICKUP_RETURN'}),
                             ('attachment', {'ATTACH_REQUEST','ATTACH_RESULT'}),
                             ('native hand visit', {'HAND_ENTER','HAND_LEAVE'}),
                             ('parsed physics submission/return', {'PHYSICS_SUBMIT','PHYSICS_ACTION_RETURN'})]:
            if not kinds.issubset(counts): missing.append(stage)
        result.append(dict(session=sid, observer_coverage_ready=coverage_ready and groups_ready,
                           transport_complete=transport_complete, losses=loss, event_counts=dict(counts), items=items,
                           physics_calls=physics, missing_observations=missing,
                           scope='Selected/watched Henry interactions only; no global physics census',
                           verdict='OBSERVATIONS_CAPTURED' if transport_complete else 'INCOMPLETE_CAPTURE',
                           limitation='No physical grabbing or destruction is inferred from input, attachment, action return or capture end.'))
    return dict(sessions=result)

if __name__ == '__main__':
    ap=argparse.ArgumentParser();ap.add_argument('log',type=Path);ap.add_argument('--out',type=Path,required=True)
    a=ap.parse_args();blob=a.log.read_bytes();report=summarize(blob.decode('utf-8-sig',errors='replace').splitlines())
    report['provenance']=dict(path=str(a.log),sha256=hashlib.sha256(blob).hexdigest(),bytes=len(blob))
    a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    print(f"Wrote {len(report['sessions'])} sessions; transport completeness does not prove physical holding.")
