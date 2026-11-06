"""Summarize copied F11 native records; never infer inventory/shot acceptance."""
from pathlib import Path
from collections import Counter, defaultdict
import argparse, hashlib, json, re

def fields(line):
    return dict(re.findall(r'(\w+)=([^\s]+)',line))

def summarize(text):
    calls=defaultdict(list);coverage=[];ends=[];candidates=Counter()
    for number,line in enumerate(text.splitlines(),1):
        if 'V66.178R36C2 RELOAD_NATIVE_COVERAGE ' in line: coverage.append(fields(line));continue
        if 'V66.178R36C2 RELOAD_NATIVE_END ' in line: ends.append(fields(line));continue
        if 'V66.178R36C2 RELOAD_NATIVE ' not in line: continue
        e=fields(line);e['line']=number
        if e.get('owner')!='HENRY':candidates[e.get('stage','?')+':'+e.get('event','?')]+=1;continue
        calls[(e.get('session'),e.get('call'))].append(e)
    result=[]
    for (session,call),events in calls.items():
        stages=defaultdict(list)
        for e in events: stages[e['stage']].append(e)
        before=next(iter(stages['HAND_BEGIN']),{})
        after=next(iter(stages['HAND_END']),{})
        nock=next(iter(stages['NOCK_END']),{})
        same_action=len({e.get('action') for e in events})==1
        same_owner=len({(e.get('actor'),e.get('henry')) for e in events})==1
        paired=same_action and same_owner and before.get('handRead')=='1' and after.get('handRead')=='1'
        cleared=bool(paired and before.get('create')=='0' and before.get('handId') not in ('0',None) and after.get('handId')=='0')
        nocked=bool(nock.get('nockRead')=='1' and nock.get('nockId') not in ('0',None))
        # Native call token, validated owner/action, ordered snapshots: no
        # joining by timestamps, event hash, weapon class or recycled address.
        transfer=bool(cleared and nocked and same_action and same_owner and
                      after['line']<nock['line'] and stages['TRANSFER'] and stages['AMMO_BIND'] and stages['NOCK_BEGIN'])
        result.append(dict(session=session,call=call,stages=[e['stage'] for e in events],
            exact_call_consistent=same_action and same_owner,hand_pair=bool(paired),
            temporary_visual_cleared=cleared,nocked_visual_present=nocked,
            hand_clear_then_nock_same_native_call=transfer,
            distinct_visual_ids=bool(transfer and before['handId']!=nock['nockId']),records=events))
    complete=bool(coverage) and all(e.get('ready')=='1' for e in coverage) and bool(ends) and \
        all(all(e.get(k)=='0' for k in ('dropped','budgetRejected','scopeLost')) for e in ends) and \
        {e.get('session') for e in coverage} <= {e.get('session') for e in ends}
    return dict(coverage=coverage,capture_ends=ends,recording_complete=complete,
        unqualified_candidates=dict(candidates),henry_calls=result,
        visual_transfers=sum(c['hand_clear_then_nock_same_native_call'] for c in result),
        inventory_delta='UNOBSERVED',shot_acceptance='UNOBSERVED',projectile_cleanup='UNOBSERVED',
        physical_grabbing='NOT_IMPLEMENTED',candidate_owner_join='NEVER_INFERRED_FROM_ACTION_POINTER')

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('log',type=Path);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
    data=a.log.read_bytes();report=summarize(data.decode('utf-8-sig',errors='replace'))
    report['source']=dict(path=str(a.log),bytes=len(data),sha256=hashlib.sha256(data).hexdigest())
    a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    print(f"Henry calls: {len(report['henry_calls'])}; visual transfers: {report['visual_transfers']}; complete recording: {report['recording_complete']}")
