#!/usr/bin/env python3
"""Count named evidence stages in the supplied log, without equating them with
human swing attempts or inferring native rejection from absent acknowledgments.
"""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import re

def fields(text):
    return dict(re.findall(r'\b([A-Za-z][A-Za-z0-9]*)=([^\s{}]+)',text))

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('log',type=Path);p.add_argument('--out',type=Path,required=True)
    a=p.parse_args();raw=a.log.read_bytes()
    counters={};queues=[];results=[];npc=[];commands=[];protected=[]
    for line_number,text in enumerate(raw.decode('utf-8',errors='replace').splitlines(),1):
        f=fields(text)
        event={'line':line_number,**f}
        if 'C54R3 HIT_CHAIN stage=COUNTS ' in text:
            counters[(f['session'],f['side'])]=event
        if 'C54R3 HIT_QUEUE ' in text and f.get('queued')=='1':queues.append(event)
        if 'H29 STRIKE_RESULT ' in text:results.append(event)
        if 'H29 STRIKE_NPC ' in text:npc.append(event)
        if 'C54R3 HIT_CHAIN stage=COMMAND_NATIVE_RETURN ' in text and f.get('why')=='EXISTING_ACTION_REFUSED_OR_NOT_CALLED':commands.append(event)
        if 'BODY_GATE ' in text and 'PROTECTED_ACTION' in text and ('masterstrike' in text or 'combat_combo_' in text):protected.append({'line':line_number,'text':text})
    def ids(rows,key):return {int(r[key]) for r in rows if key in r}
    qg=ids(queues,'gesture');rg=ids(results,'gesture');missing=sorted(qg-rg)
    onsets=Counter()
    for (session,side),row in counters.items():onsets[session]+=int(row['onsets'])
    result={
        'input':{'file':a.log.name,'bytes':len(raw),'sha256':hashlib.sha256(raw).hexdigest()},
        'interpretation':'Distinct named event stages, not human attempt/hit/miss denominators. Logger wrappers/repeated observations are not independent actions.',
        'recognizer_onsets':{'total':sum(onsets.values()),'by_session':dict(onsets),'final_counter_rows':list(counters.values())},
        'queued_contacts':{'rows':len(queues),'distinct_serials':len(ids(queues,'serial')),'distinct_gestures':len(qg)},
        'npc_before_admission_observations':{'rows':len(npc),'distinct_serials':len(ids(npc,'serial')),'distinct_gestures':len(ids(npc,'gesture'))},
        'native_results':{'rows':len(results),'distinct_serials':len(ids(results,'serial')),'distinct_gestures':len(rg),'verdicts':dict(Counter(r['verdict'] for r in results)),'routes':dict(Counter(r['route'] for r in results)),'combo_rows':[r for r in results if r.get('combo')=='1'],'combo_end_rows':[r for r in results if r.get('comboEnd')=='1']},
        'queued_gestures_without_exact_result':{'count':len(missing),'percentage_of_queued_gestures':100*len(missing)/len(qg) if qg else None,'gestures':missing,'first_queue_lines':{str(g):next(r['line'] for r in queues if int(r['gesture'])==g) for g in missing},'meaning':'No matching H29 STRIKE_RESULT for this gesture in this capture; not proof of a failed human hit or its cause.'},
        'action_refused_or_not_called':{'rows':len(commands),'called_values':dict(Counter(r.get('called','absent') for r in commands)),'lines':[r['line'] for r in commands],'meaning':'called=0 records do not prove the native game was called and refused.'},
        'protected_clip_examples':protected,
        'not_established':['a 19% human miss rate','all 15 missing exact results were failed native hits','a lost block haptic for the two asserted block cases','the direct route can never advance a combo in any context','the proposed larger timing settings improve this capture']}
    a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k in ('input','queued_contacts','native_results','action_refused_or_not_called')},indent=2))

if __name__=='__main__':main()
