#!/usr/bin/env python3
"""Analyze native combo cursor/candidate episodes plus inherited V66.90 playback.
No guessed damage, anatomical contact, learned state or execution authorization.
"""
from __future__ import annotations
import argparse,collections,json,sys
from pathlib import Path
import analyze_v6690 as prior
ROOT=Path(__file__).resolve().parents[1]
UNKNOWN_FIELDS=('accepted_hit','combo_accepted','finisher_eligibility','victim_health','physical_contact_region','weapon_contact_feature','native_learned_state','native_active_fragment')
def unknowns():return {k:'UNKNOWN' for k in UNKNOWN_FIELDS}
def recipe_candidates(row,profiles,profile):
    if row.get('nameExact')!='1':return []
    wanted=row.get('name','').lower();out=[]
    names=list(profiles.get('profiles',{})) if profile=='unknown' else [profile]
    for selected in names:
        for r in profiles.get('profiles',{}).get(selected,[]):
            a=r['recipe']
            if a.get('Id','').lower()!=wanted:continue
            # Length can disqualify a candidate, but does not identify native enums.
            if len(r['steps'])!=prior.integer(row.get('requiredSteps'),-1):continue
            out.append({'profile':selected,'recipe':a,'steps':r['steps'],'technique_perk_link':r.get('technique_perk_link'),'source_comment':r.get('source_comment'),'match_basis':'NAME_AND_LENGTH_ONLY_WEAPON_TECHNIQUE_UNRESOLVED','live_recipe_verified':False,'physical_sequence':None})
    return out

def analyze(text,profiles=None,links=None,profile='unknown',catalog=None):
    profiles=profiles or {};links=links or [];episodes={};bindings=[];status=[];sessions=collections.defaultdict(list);session='UNSPECIFIED';malformed=[]
    for line_no,line in enumerate(text.splitlines(),1):
        f=prior.fields(line)
        if 'V66.91-F3-COMBO_SESSION ' in line:session=f.get('session','UNSPECIFIED')
        sessions[session].append(line)
        if 'V66.91-F3-COMBO_BINDING ' in line:bindings.append({'line':line_no,'text':line})
        if 'V66.91-F3-COMBO_STATUS ' in line:status.append(f)
        kind=next((k for k in ('EVENT','SNAPSHOT','CANDIDATE') if f'V66.91-F3-COMBO_{k} ' in line),None)
        if not kind:continue
        if not all(k in f for k in ('session','epoch','call')):malformed.append(line_no);continue
        key=(f['session'],f['epoch'],f['call']);e=episodes.setdefault(key,{'session':f['session'],'epoch':f['epoch'],'call':f['call'],'event':None,'snapshots':{},'duplicate_records':False})
        f['source_line']=line_no
        if kind=='EVENT':
            if e['event'] is not None:e['duplicate_records']=True
            e['event']=f
        else:
            side=f.get('side');
            if side not in ('BEFORE','AFTER'):malformed.append(line_no);continue
            s=e['snapshots'].setdefault(side,{'header':None,'rows':[]})
            if kind=='SNAPSHOT':
                if s['header'] is not None:e['duplicate_records']=True
                s['header']=f
            else:s['rows'].append(f)
    old_reports=[]
    for sid,lines in sessions.items():
        rp=prior.analyze('\n'.join(lines),catalog);rp['session']=sid;old_reports.append(rp)
    all_events=[]
    for e in episodes.values():
        event=e['event'] or {};e.update(unknowns());e['complete_headers']=bool(e['event'] and not e['duplicate_records'] and all(e['snapshots'].get(x,{}).get('header') for x in ('BEFORE','AFTER')))
        complete_native=e['complete_headers'] and event.get('epochStable')=='1' and event.get('comparable')=='1'
        for s in e['snapshots'].values():
            h=s['header'] or {};expected=prior.integer(h.get('expectedRows'),-1)
            ids=[r.get('row') for r in s['rows']]
            s['detail_complete']=h.get('details')=='1' and len(ids)==len(set(ids))==expected and set(ids)=={str(i) for i in range(max(0,expected))}
            s['detail_status']='COMPLETE' if s['detail_complete'] else ('NOT_EMITTED' if h.get('details')=='0' and not ids else 'INCOMPLETE_OR_ORPHAN')
            s['decoded_complete']=h.get('status')=='OK' and h.get('truncated')=='0'
            complete_native=complete_native and s['decoded_complete']
            for row in s['rows']:
                row['static_recipe_candidates']=recipe_candidates(row,profiles,profile) if s['detail_complete'] and s['decoded_complete'] else []
                row['native_weapon_selectors_interpreted']=False;row['native_technique_uuid_decoded']=False
        e['native_comparison_usable']=bool(complete_native)
        e['observed_fields_changed']=(event.get('observedFieldsChanged')=='1') if complete_native else None
        e['interpretation']='NATIVE_CURSOR_CANDIDATE_CHANGE' if e['observed_fields_changed'] else ('NO_CAPTURED_FIELD_CHANGE' if complete_native else 'INCOMPLETE_NO_NEGATIVE_CONCLUSION')
        e['prior_playback']=None
        rows=[s for rp in old_reports if rp['session']==e['session'] for s in rp['snapshots'] if s['header'].get('epoch')==e['epoch'] and s['header'].get('role')=='HENRY' and not s['header'].get('incomplete') and 0<=prior.integer(event.get('tick'),-1)-prior.integer(s['header'].get('tick'),-1000)<=500]
        if rows:
            nearest=max(rows,key=lambda s:prior.integer(s['header'].get('tick')));tick=prior.integer(event.get('tick'));start=prior.integer(nearest['header'].get('tick'))
            gaps=[g for rp in old_reports if rp['session']==e['session'] for g in rp['gaps'] if g.get('epoch')==e['epoch'] and g.get('role')=='HENRY' and start<=prior.integer(g.get('tick'))<=tick]
            if not gaps:e['prior_playback']={'relation':'TEMPORAL_ONLY_NOT_CAUSAL','seq':nearest['header'].get('seq'),'age_ms':tick-start,'activated_names':[c['name'] for c in nearest['clips'] if c.get('activated')=='1']}
        all_events.append(e)
    by_clip=collections.defaultdict(list)
    for f in links:
        for c in f.get('clips',[]):
            name=c.get('name');
            if name:by_clip[name].append(f)
    for rp in old_reports:
        for c in rp['clips']:
            candidates=[]
            for f in by_clip.get(c['name'],[]):
                regions=[{'id':h.get('attributes',{}).get('BodySubpartId'),'name':(h.get('body_subpart') or {}).get('body_subpart_name'),'coefficient_raw':h.get('attributes',{}).get('AttackCoef')} for m in f.get('metadata_matches',[]) for h in m.get('hits',[])]
                candidates.append({'database':f.get('source'),'family':f.get('family'),'fragment':f.get('attributes'),'combo_tags':f.get('combo_tags'),'authored_regions':regions,'action_rows':f.get('action_rows'),'relation':'STATIC_CLIP_CANDIDATE_NOT_LIVE_HIT_OR_FINISHER'})
            c['combo_semantic_candidates']=candidates;c.update(unknowns())
    return {'build':'V66.91','status':'READ_ONLY_NATIVE_STATE_AND_STATIC_CANDIDATES','recipe_profile_annotation':profile,'effective_game_load_order':'UNKNOWN','profile_summary':profiles.get('summary',{}),'episodes':all_events,'episode_count':len(all_events),'bindings':bindings,'native_observer_status':status,'malformed_lines':malformed,'legacy_recordings':old_reports,'limitations':['STEP_FILTER entry is not an accepted hit.','RESET or disappearing candidates is not proof of a completed combo or execution.','Raw manager arguments/selectors/GUID bytes have no inferred native enum meaning.','Candidate names and lengths do not establish weapon-qualified recipe identity.','Static authored body subparts are not actual contact regions.','Activated clip is not proof of contributing blend weight; names may be shared by death and normal actions.','Profile selection is an analyst annotation, not proof a mod was loaded.','Rate limits, truncation, native replacement and queue loss can leave incomplete episodes.','No anatomical recognizer, physical holding, haptics, NPC reactions or damage injection is implemented.']}

def markdown(r):
    lines=['# V66.91 combo progression diagnostic report','',f"Native episodes: **{r['episode_count']}**. Recipe reference: **{r['recipe_profile_annotation']}**; effective game load order remains **UNKNOWN**.",'','These are native cursor/candidate changes, not verified accepted hits or completed combos.','', '## Native observations','', '| Session / epoch / call | Operation | Complete comparison | Observed fields changed | Before → after state counts | Candidate detail |','|---|---|---|---|---|---|']
    for e in r['episodes']:
        ev=e['event'] or {};a=e['snapshots'].get('BEFORE',{});b=e['snapshots'].get('AFTER',{});ah=a.get('header') or {};bh=b.get('header') or {}
        lines.append(f"| {e['session']}/{e['epoch']}/{e['call']} | {ev.get('op','UNKNOWN')} | {e['native_comparison_usable']} | {e['observed_fields_changed']} | {ah.get('statesTotal','?')} → {bh.get('statesTotal','?')} | {a.get('detail_status','MISSING')} / {b.get('detail_status','MISSING')} |")
    if not r['episodes']:lines+=['','No native combo episodes were captured. Check binding/status records; this is not evidence that no combo occurred.']
    lines+=['','## Captured combo-related clips','']
    for rp in r['legacy_recordings']:
        for c in rp['clips']:
            cs=c.get('combo_semantic_candidates',[])
            if not cs:continue
            families=sorted({f['family'] for f in cs});regions=sorted({str(g['id'])+':'+str(g['name']) for f in cs for g in f['authored_regions']})
            lines+=[f"- `{c['name']}` ({c['role']}): {c['activated_samples']} activated samples; static families {', '.join(families)}; authored region candidates {', '.join(regions) or 'not mapped'}. Actual contact/authorization UNKNOWN."]
    lines+=['','## Interpretation boundaries','']+['- '+s for s in r['limitations']]
    lines+=['','Full candidate recipes, technique/perk links, raw fields, event details and old playback reports are retained in the JSON.']
    return '\n'.join(lines)+'\n'

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('log',type=Path);ap.add_argument('--out',type=Path,default=Path('v6691_report'));ap.add_argument('--recipe-profile',choices=['unknown','supplied-retail','simple-combos'],default='unknown');ap.add_argument('--data-dir',type=Path,default=ROOT/'continuity/semantic_index/combo');a=ap.parse_args()
    try:
        profiles=json.loads((a.data_dir/'recipe_profiles.json').read_text());links=json.loads((a.data_dir/'combo_fragment_links.json').read_text());catalog=json.loads((ROOT/'continuity/semantic_index/combat_animation_catalog.json').read_text());r=analyze(a.log.read_text(encoding='utf8',errors='replace'),profiles,links,a.recipe_profile,catalog)
        a.out.parent.mkdir(parents=True,exist_ok=True);a.out.with_suffix('.json').write_text(json.dumps(r,indent=2)+'\n');a.out.with_suffix('.md').write_text(markdown(r));print(f"{r['episode_count']} native episodes; report written. Native hit/finisher authority remains unknown.")
    except (OSError,ValueError,KeyError) as e:print('Analysis failed:',e,file=sys.stderr);return 1
    return 0
if __name__=='__main__':raise SystemExit(main())
