#!/usr/bin/env python3
"""V66.94 controller-core / F3/F5 report. Does not infer visual success or native hit permission."""
from __future__ import annotations
import argparse, collections, json, sys
from pathlib import Path
import analyze_v6691 as previous
from analyze_v6690 import fields, integer
ROOT=Path(__file__).resolve().parents[1]

def analyze(text: str, profiles=None, links=None, catalog=None) -> dict:
    result=previous.analyze(text,profiles,links,'supplied-retail',catalog)
    result['build']='V66.94';result['status']='SOURCE_LOG_OBSERVATIONS_NOT_HEADSET_PROOF'
    markers=('V66.94-F12 ','V66.94-FINGER ','V66.94-ALIAS ','V66.94-F3 COVERAGE','V66.94-VRIK ',
             'V66.92R1-POSE-REJECT ','V66.94-F5 ')
    records=[];raw=collections.defaultdict(list)
    for number,line in enumerate(text.splitlines(),1):
        for marker in markers:
            if marker not in line:continue
            row=fields(line);row['source_line']=number;row['message']=line;row['channel']=marker.strip()
            records.append(row)
            if 'V66.94-FINGER RAW_REFERENCE ' in line:
                key=(row.get('side'),row.get('category'),row.get('sample'))
                raw[key].append(row)
            break
    raw_groups=[]
    for (side,category,sample),rows in raw.items():
        counts={integer(r.get('count'),-1) for r in rows}
        indices=[integer(r.get('index'),-1) for r in rows]
        complete=(len(counts)==1 and next(iter(counts))>0 and len(indices)==len(set(indices))==next(iter(counts)) and set(indices)==set(range(next(iter(counts)))))
        raw_groups.append({'side':side,'category':category,'sample':sample,'complete_logged_array':complete,
          'authored_endpoint_verified':False,'rows':rows})
    clips=[c for report in result['legacy_recordings'] for c in report['clips']]
    unresolved=[c for c in clips if c['name'].startswith('<') or c['name'] in ('CAF filepath disabled','<unknown>')]
    events=[e for report in result['legacy_recordings'] for e in report['events']]
    result['controller_core']={'records':records,'raw_reference_groups':raw_groups,
      'reported_reference_ready':sum('CURVE_REFERENCE_READY ' in r['message'] for r in records),
      'reported_cache_loaded':any('REFERENCES_LOADED ' in r['message'] for r in records),
      'reported_weapon_control_started':sum('CONTROL_STARTED ' in r['message'] for r in records),
      'weapon_visual_success':'USER_OBSERVATION_REQUIRED',
      'logged_clip_rows':len(clips),'unresolved_clip_rows':len(unresolved),
      'native_event_counts':dict(collections.Counter(e['kind'] for e in events))}
    result['limitations'] += [
        'CURVE_REFERENCE_READY identifies a complete source-derived candidate, not visible finger clearance.',
        'CONTROL_STARTED/readback is not proof the visible sword moved or remained controlled.',
        'A raw diagnostic finger array is not automatically the authored relaxed/fist endpoint.',
        'F3/F5 are finite-rate observers, not physical object grabbing, holster gestures or damage injection.',
        'Vanilla recipe data is an offline reference; the effective runtime data/load order remains unknown.']
    return result

def markdown(r: dict) -> str:
    c=r['controller_core']
    lines=['# V66.94 controller-core and native-event report','',
      f"Weapon-control start records: **{c['reported_weapon_control_started']}**. Complete curve-reference announcements: **{c['reported_reference_ready']}**. Cache load announced: **{c['reported_cache_loaded']}**.",
      f"Named or unresolved role/clip rows: **{c['logged_clip_rows']}**; unresolved rows: **{c['unresolved_clip_rows']}**.",
      '', 'Neither a readback nor a READY flag establishes a visible headset result.',
      '', '## Native event coverage', '', '```json',json.dumps(c['native_event_counts'],indent=2),'```',
      '', '## State changes and failure reasons','']
    # Preserve detailed arrays in JSON; avoid turning 44 joints into a giant Markdown dump.
    for row in c['records']:
        if 'RAW_REFERENCE ' not in row['message']:lines.append(f"Line {row['source_line']}: `{row['message'].replace('`','')}`")
    if not c['records']:lines+=['No V66.94 core records are present. This is not evidence that no user action occurred.']
    lines+=['','## Raw finger arrays','']
    for group in c['raw_reference_groups']:
        lines.append(f"Hand {group['side']}, category {group['category']}, sample {group['sample']}: complete logged array = {group['complete_logged_array']}; authored endpoint = unverified.")
    lines+=['',previous.markdown(r).replace('# V66.91 combo progression diagnostic report','## Retained native combo progression report',1)]
    return '\n'.join(lines)+'\n'

def main() -> int:
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('log',type=Path);ap.add_argument('--out',type=Path,default=Path('v6694_report'));args=ap.parse_args()
    try:
        data=ROOT/'continuity/semantic_index/combo'
        def read(path):return json.loads(path.read_text(encoding='utf8')) if path.exists() else {}
        r=analyze(args.log.read_text(encoding='utf8',errors='replace'),read(data/'recipe_profiles.json'),read(data/'combo_fragment_links.json'),read(ROOT/'continuity/semantic_index/combat_animation_catalog.json'))
        args.out.parent.mkdir(parents=True,exist_ok=True)
        args.out.with_suffix('.json').write_text(json.dumps(r,indent=2)+'\n',encoding='utf8')
        args.out.with_suffix('.md').write_text(markdown(r),encoding='utf8')
        print(f"Wrote {args.out.with_suffix('.md')} and JSON. Vanilla comparison only; no visual/hit success inferred.")
    except (OSError,ValueError,KeyError,TypeError) as exc:
        print(f'Analysis failed: {exc}',file=sys.stderr);return 1
    return 0
if __name__=='__main__':raise SystemExit(main())
