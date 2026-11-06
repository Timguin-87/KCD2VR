#!/usr/bin/env python3
"""V66.95 continuity/contact report; never equates modeled/native records with visible success."""
from __future__ import annotations
import argparse,json,sys
from pathlib import Path
import analyze_v6694 as core
from analyze_v6690 import fields,integer
ROOT=Path(__file__).resolve().parents[1]

def analyze(text: str, profiles=None, links=None, catalog=None) -> dict:
    # Delegate the preserved raw-reference/alias/action decoders without losing
    # original line numbers. Restore literal messages after version normalization.
    original=text.splitlines()
    normalized=text.replace('V66.95-F12 ','V66.94-F12 ').replace('V66.95-FINGER ','V66.94-FINGER ').replace('V66.95-VRIK ','V66.94-VRIK ')
    r=core.analyze(normalized,profiles,links,catalog)
    for row in r['controller_core']['records']:
        row['message']=original[row['source_line']-1]
    ends=[];owners=[];budgets=[];statuses=[];gaps=[]
    for n,line in enumerate(original,1):
        target=None
        if 'V66.95-F12 SESSION_END ' in line:target=ends
        elif 'V66.95-F3-OWNER_BOUNDARY ' in line:target=owners
        elif 'V66.95-CONTACT BUDGET ' in line:target=budgets
        elif 'V66.92R1-COLLISION STATUS ' in line:target=statuses
        elif 'V66.90-PLAYBACK GAP ' in line:target=gaps
        if target is not None:
            row=fields(line);row.update(source_line=n,message=line);target.append(row)
    complete=[x for x in ends if integer(x.get('restorePending'),-1)==0]
    last=statuses[-1] if statuses else {}
    r.update(build='V66.95',status='LOG_OBSERVATIONS_NOT_HEADSET_PROOF')
    r['continuity_contact']={
        'session_end_records':ends,'reported_application_count_at_session_ends':sum(max(0,integer(x.get('applied'),0)) for x in ends),
        'session_end_count':len(ends),'session_ends_without_pending_restore':len(complete),
        'all_active_intervals_covered':False,'weapon_visible_continuity':'USER_OBSERVATION_REQUIRED',
        'owner_boundary_records':owners,'owner_mismatch_is_player_acceptance':False,
        'contact_budget_records':budgets,'collision_status_records':statuses,'latest_cumulative_collision_status':last,
        'collision_guaranteed':False,'blade_npc_dynamic_collision_implemented':False,
        'physical_grabbing_implemented':False,'playback_gaps':gaps}
    r['limitations'] += [
        'SESSION_END counts are observed counter snapshots, not a complete timeline or proof of native restoration/visible movement.',
        'Do not add cumulative collision query/hit counters from different status rows.',
        'Static hand/arm contact uses bounded ray approximations; no swept blade, dynamic-object, NPC or torso rigid-body physics is claimed.',
        'A busy/incomplete query is unknown. A native fault is not a clear scene. A slow call cannot be interrupted by this adapter.',
        'F5 retains native action/target diagnostics. Physical acquisition, hold/stow/drop and door manipulation are not implemented.',
        'The revised player-neutral cache is an authored candidate; final relaxed/fist mesh clearance remains user acceptance.']
    return r

def markdown(r: dict) -> str:
    c=r['continuity_contact'];last=c['latest_cumulative_collision_status']
    lines=['# V66.95 continuity and contact report','',
      f"Observed F12 session-end records: **{c['session_end_count']}**; summed applications at those ends: **{c['reported_application_count_at_session_ends']}**.",
      'An ACTIVE/CONTROL_STARTED record by itself is not a final placement count. Intervals without SESSION_END remain incomplete.',
      '', '## Static-contact backend','',
      f"Latest cumulative state: `{last.get('state','NOT_RECORDED')}`; queries `{last.get('queries','unknown')}`, hits `{last.get('hits','unknown')}`, faults `{last.get('faults','unknown')}`.",
      'A returned query or hit is not proof of mesh collision, blade collision or grabbing. Budgets stop subsequent calls; they do not preempt a blocked native call.',
      '', '## Precise owner/playback boundaries','']
    for row in c['owner_boundary_records']+c['playback_gaps']:
        lines.append(f"Line {row['source_line']}: `{row['message'].replace('`','')}`")
    lines+=['','## Session totals and query budgets','']
    for row in c['session_end_records']+c['contact_budget_records']:
        lines.append(f"Line {row['source_line']}: `{row['message'].replace('`','')}`")
    lines+=['',core.markdown(r).replace('# V66.94 controller-core and native-event report','## Preserved controller and native-event records',1),
       '', 'Physical F5 grabbing is not implemented; native request/readback is not a held-item lifecycle.']
    return '\n'.join(lines)+'\n'

def main() -> int:
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('log',type=Path);ap.add_argument('--out',type=Path,default=Path('v6695_report'));args=ap.parse_args()
    try:
        d=ROOT/'continuity/semantic_index/combo'
        def load(p):return json.loads(p.read_text(encoding='utf8')) if p.exists() else {}
        r=analyze(args.log.read_text(encoding='utf8',errors='replace'),load(d/'recipe_profiles.json'),load(d/'combo_fragment_links.json'),load(ROOT/'continuity/semantic_index/combat_animation_catalog.json'))
        args.out.parent.mkdir(parents=True,exist_ok=True)
        args.out.with_suffix('.json').write_text(json.dumps(r,indent=2)+'\n',encoding='utf8')
        args.out.with_suffix('.md').write_text(markdown(r),encoding='utf8')
        print('Wrote Markdown/JSON; vanilla comparison only, no visual or physical success inferred.')
    except (OSError,ValueError,KeyError,TypeError) as exc:
        print(f'Analysis failed: {exc}',file=sys.stderr);return 1
    return 0
if __name__=='__main__':raise SystemExit(main())
