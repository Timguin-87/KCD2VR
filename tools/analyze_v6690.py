#!/usr/bin/env python3
"""Read V66.90 diagnostics without inferring damage/finisher permission from names.
Usage: python tools/analyze_v6690.py KCD2VR-diagnostics.log --out replay_report
Writes JSON and Markdown beside the requested output prefix; never changes source logs.
"""
from __future__ import annotations
import argparse,bisect,collections,json,re,sys
from pathlib import Path
FIELDS=re.compile(r'([A-Za-z0-9_]+)=(?:"([^"\r\n]*)"|\'([^\'\r\n]*)\'|([^\s]+))')
def fields(line):
    return {m[0]:next((v for v in m[1:] if v!=''),'') for m in FIELDS.findall(line)}
def integer(v,default=0):
    try:return int(v)
    except (ValueError,TypeError):return default

def analyze(text: str,catalog: dict|None=None):
    snapshots={};gaps=[];events=[];starts=collections.Counter();kinds=collections.Counter();lab=[];drops=0;truncated=set()
    source_lines=text.splitlines()
    for number,line in enumerate(source_lines,1):
        if 'droppedTotal=' in line:drops=max(drops,integer(fields(line).get('droppedTotal')))
        if '-ANIM-START ' in line:
            f=fields(line);starts[(f.get('role','UNKNOWN'),f.get('name','<unknown>'))]+=1
        if 'V66.90-' not in line:continue
        f=fields(line);f['source_line']=number
        if 'V66.90-LAB ' in line:lab.append(f)
        elif 'V66.90-PLAYBACK SNAP ' in line:
            key=(f.get('epoch'),f.get('seq'));snapshots[key]={'header':f,'clips':[]}
        elif 'V66.90-PLAYBACK CLIP ' in line:
            key=(f.get('epoch'),f.get('seq'))
            snapshots.setdefault(key,{'header':{'epoch':f.get('epoch'),'seq':f.get('seq'),'tick':f.get('tick'),'role':f.get('role'),'orphan':True},'clips':[]})['clips'].append(f)
        elif 'V66.90-PLAYBACK GAP ' in line:gaps.append(f)
        elif 'V66.90-PLAYBACK TRUNCATED ' in line:truncated.add(f.get('seq'))
        else:
            for tag in ('F3-EQUIPMENT','F3-ENRICH','F3-CONTACT_CONTEXT','F5-CONTEXT','F5-ACTION REQUEST','F5-ACTION RETURN'):
                if 'V66.90-'+tag in line:
                    f['kind']=tag;events.append(f);kinds[tag]+=1;break
    observed=collections.Counter();queued=collections.Counter();advancing=collections.Counter()
    for snap in snapshots.values():
        header=snap['header'];header['incomplete']=bool(header.get('orphan') or header.get('seq') in truncated or integer(header.get('entries'),len(snap['clips']))!=len(snap['clips']))
        for c in snap['clips']:
            key=(c.get('role','UNKNOWN'),c.get('name','<unknown>'))
            (observed if c.get('activated')=='1' else queued)[key]+=1
            if c.get('activated')=='1' and c.get('progression')=='ADVANCED':advancing[key]+=1
    by_epoch=collections.defaultdict(list)
    for snap in snapshots.values():
        h=snap['header']
        if h.get('role')=='HENRY' and not h.get('incomplete'):by_epoch[h.get('epoch')].append(snap)
    for rows in by_epoch.values():rows.sort(key=lambda x:integer(x['header'].get('tick')))
    for event in events:
        event['native_accepted_hit']='UNKNOWN';event['native_finisher_eligibility']='UNKNOWN'
        # Only temporal, same-epoch association to an ACTUALLY LOGGED complete
        # snapshot. A serial reference to an un-emitted in-memory sample is not
        # a basis for inventing the sample's clips.
        rows=by_epoch.get(event.get('epoch'),[]);ticks=[integer(r['header'].get('tick')) for r in rows];tick=integer(event.get('tick'),-1)
        i=bisect.bisect_right(ticks,tick)-1
        event['prior_playback']=None
        if i>=0 and tick>=0 and 0<=tick-ticks[i]<=500:
            h=rows[i]['header'];start=ticks[i]
            invalidated=any(g.get('epoch')==event.get('epoch') and g.get('role')=='HENRY' and start<=integer(g.get('tick'))<=tick for g in gaps)
            if not invalidated:event['prior_playback']={'seq':h.get('seq'),'age_ms':tick-start,'relation':'TEMPORAL_ONLY_NOT_CAUSATION','activated_names':[c.get('name') for c in rows[i]['clips'] if c.get('activated')=='1']}
    data=(catalog or {}).get('clips',{})
    clip_rows=[]
    for role,name in sorted(set(observed)|set(queued)|set(starts)):
        associations=data.get(name,[])
        clip_rows.append({'role':role,'name':name,'activated_samples':observed[(role,name)],'queued_samples':queued[(role,name)],'observed_advances':advancing[(role,name)],'successful_start_records':starts[(role,name)],'static_fragment_candidates':sorted(set(a['fragment'] for a in associations)),'static_association_count':len(associations),'runtime_fragment':'UNKNOWN','clip_blend_weight':'UNKNOWN','victim_health':'UNKNOWN','native_finisher_eligibility':'UNKNOWN'})
    return {'build':'V66.90','status':'OBSERVATIONS_ONLY','source_lines':len(source_lines),'snapshots':list(snapshots.values()),'snapshot_count':len(snapshots),'gaps':gaps,'log_queue_dropped_total':drops,'events':events,'event_counts':dict(kinds),'clips':clip_rows,'lab_transitions_and_status':lab,'limitations':['Segment component is not whole-action phase.','Activated flag is not verified contributing blend weight.','Names map to static candidate fragments, not live action identity.','Contact-associated entity is not automatically the victim.','Contacts/animations do not prove native damage or finisher eligibility.','Bounded sampling can miss short clips; log gaps/truncation are explicit.','Current prerecorded/native physics, physical VR grabs and gesture holsters are different capabilities.']}

def markdown(report):
    lines=['# V66.90 recorded animation / equipment / interaction summary','',f"Complete and partial snapshot records: **{report['snapshot_count']}**. Queue drops reported: **{report['log_queue_dropped_total']}**.",'','Native accepted hits, victim health, combo state and finisher eligibility are **UNKNOWN**. No kill recommendation is generated.','']
    if not report['snapshot_count']:lines+=['No V66.90 sampled-playback records are present. Start-only or older logs do not establish that no animations played.','']
    lines+=['## Recorded counts','',json.dumps(report['event_counts'],indent=2),'','## Clips','', '| Role | Clip | Activated samples | Queued samples | Observed advances | Static fragment candidates (not live identity) |','|---|---|---:|---:|---:|---|']
    def safe(s):return str(s).replace('|','/').replace('\n',' ')
    for c in report['clips']:
        candidates=', '.join(c['static_fragment_candidates']) or 'unmapped'
        lines.append('| '+' | '.join(map(safe,[c['role'],c['name'],c['activated_samples'],c['queued_samples'],c['observed_advances'],candidates]))+' |')
    lines+=['','## Interpretation boundaries','']+['- '+s for s in report['limitations']]
    return '\n'.join(lines)+'\n'
def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('log',type=Path);ap.add_argument('--out',type=Path,default=Path('v6690_report'));ap.add_argument('--catalog',type=Path,default=Path(__file__).resolve().parents[1]/'continuity/semantic_index/combat_animation_catalog.json');a=ap.parse_args()
    try:
        text=a.log.read_text(encoding='utf8',errors='replace');catalog=json.loads(a.catalog.read_text(encoding='utf8')) if a.catalog.exists() else {};report=analyze(text,catalog)
        a.out.parent.mkdir(parents=True,exist_ok=True);a.out.with_suffix('.json').write_text(json.dumps(report,indent=2),encoding='utf8');a.out.with_suffix('.md').write_text(markdown(report),encoding='utf8');print(f"{report['snapshot_count']} snapshots; {len(report['clips'])} distinct role/clip rows. Wrote {a.out.with_suffix('.md')} and JSON.")
    except (OSError,ValueError) as e:print('Analysis failed:',e,file=sys.stderr);return 1
    return 0
if __name__=='__main__':raise SystemExit(main())
