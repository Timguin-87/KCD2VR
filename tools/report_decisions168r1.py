#!/usr/bin/env python3
"""Decision-led KCD2VR capture review. Offline only; no game access.

Retains V168's full-content duplicate detection, internal build markers and
source line excerpts. Verdicts describe evidence coverage, never gameplay success.
A feature whose live bridge does not exist is IMPLEMENTATION_REQUIRED, regardless
of how many passive records a log happens to contain.
"""
from __future__ import annotations
import argparse
from collections import Counter
import json
from pathlib import Path
import sys
from report_session168 import inspect_paths, integer

EXPECTED_BUILD = 'KCD2VR-V66.168R1-DECISION-CAPTURES'

def decisions(report: dict) -> dict:
    counts: Counter = Counter()
    current = []
    excluded = []
    partial = []
    for file in report['files']:
        # Never promote a historical/unknown binary to current from its filename.
        if EXPECTED_BUILD not in file['build_markers']:
            excluded.append({'source': file['source'], 'build_markers': file['build_markers']})
            continue
        current.append(file)
        counts.update(file['counts'])
        if not file['complete_read'] or file['captures_open_at_file_end']:
            partial.append(file['source'])
    def row(key: str, domain: str, evidence: str, missing: str, action: str,
            implementation: bool = False) -> dict:
        n = counts[evidence]
        status = ('IMPLEMENTATION_REQUIRED' if implementation else
                  'OBSERVED_NOT_PROVEN' if n else 'MISSING_OBSERVATION')
        return dict(gap=key, domain=domain, status=status, evidence_category=evidence,
                    recorded_count=n, missing=missing, next_code_decision=action)
    rows = [
        row('pose_contact_render_agreement','COMBAT/INTERACTION','pose_validity',
            'No complete same-sample triple of allowed collision pose, final skinning/attachment pose and native sweep pose.',
            'Join by exact sample/epoch/item and engine phase. Fix the first disagreement, not grace duration or a second pose writer.'),
        row('tracked_npc_body_part_damage','COMBAT','positive_damage_with_tracked_prediction_ids',
            'Prediction IDs are not live shape-to-NPC-bone mapping or proof that the rendered weapon touched that body part.',
            'Implement exact target/shape/part identity and match a contact to native processing before extending fist, pommel or shield geometry.', True),
        row('armed_R3_kick_selection','COMBAT','close_query',
            'CLOSE_QUERY is input criteria; it is not the selected action configuration or a kick acknowledgement.',
            'Trace/observe the selected config and constructed native action for the R3 ticket; only then change exact-player kick selection.', True),
        row('thrust_only_counter_coverage','COMBAT','master_factory',
            'Factory group match and VR permission are not final native counter acceptance, contact or damage.',
            'Correlate exact request/opponent/weapon through selected action and follow-up result; identify any counter route bypassing the opt-in factory.'),
        row('combo_contact_semantics','COMBAT','combo_event',
            'Native sequence progress alone does not identify an actual knee/abdomen/pommel contact.',
            'Join accepted native contact and weapon-qualified recipe progress; keep timing, armor and perks authoritative.'),
        row('real_held_object','INTERACTION','interaction_lifecycle',
            'No owned hand proxy or live create/update/remove hold constraint is implemented in this candidate.',
            'Implement one eligible loose-object constraint slice, including obstruction, release, removal and menu/tracking-loss cleanup. Do not request more passive pickup logs as a substitute.', True),
        row('door_and_workstation_adapters','INTERACTION','interaction_lifecycle',
            'Generic lifecycle records do not supply every door contact phase or smithing/alchemy tool state.',
            'Map exact native action/hand/tool phase and input owner; implement per-tool adapters instead of moving all objects as rigid props.', True),
        row('native_reload_hand_visual','COMBAT/INTERACTION','interaction_lifecycle',
            'Known event dispatcher/visual creation paths are not yet a qualified, controller-owned hand-arrow lifetime.',
            'Use the exact reload action and before/after hand and bow visual IDs; preserve native cleanup and two-visual handoff.', True),
        row('custom_pad_native_activity_laser','INTERACTION/HUD','input_owner',
            'Transport ready does not prove semantic remapping or menu hit-coordinate agreement.',
            'Compare raw control, chosen owner, emitted semantic and native receipt at context edges. Repair neutral handoff or mapping, not XI globally.'),
        row('HUD_surface_and_visibility','HUD','hud_coverage168r1',
            'CPU counters do not measure GPU cost, native visibility writer identity or an independently capturable alpha surface.',
            'Separate mask/retail draw/blocked-skip cost; trace specific element writers and HUD versus effects/menu surface ownership before replacing the render path.'),
    ]
    bypasses = []
    for file in current:
        for example in file.get('examples_first_n',{}).get('hud_coverage168r1',[]):
            if integer(example['text'],'contendedUnmasked') > 0:
                bypasses.append(dict(source=file['source'], **example))
    return {
        'schema':'KCD2VR_DECISION_REVIEW_168R1_v1', 'expected_build':EXPECTED_BUILD,
        'current_build_files':len(current), 'historical_or_unidentified_not_promoted':excluded,
        'partial_current_captures':partial, 'decisions':rows,
        'hud_unmasked_bypass_observed':bypasses,
        'limits':[
            'OBSERVED_NOT_PROVEN is not PASS. A counter or native call is not physical gameplay acceptance.',
            'Counts aggregate recorded lines, not deduplicated semantic actions across overlapping streams.',
            'No current-build files means latest behavior is untested; historical observations remain in the source review.',
            'Snapshots are bounded examples; counters are cumulative and individually atomic, not an atomic multivariate frame snapshot.',
            'A quiet/missing/truncated/dropped source cannot prove the native action never happened.',
            'IMPLEMENTATION_REQUIRED cannot be closed with another passive reference recording.'],
        'source_review':report}

def markdown(data: dict) -> str:
    lines=['# KCD2VR diagnostic decision report', '',
           f"Expected build: `{data['expected_build']}`. Matching log files: **{data['current_build_files']}**.", '',
           '**These are evidence-coverage verdicts, not a gameplay pass certificate.**','']
    if not data['current_build_files']:
        lines += ['No matching current-build log was supplied. Historical logs were retained but not treated as a test of the latest binary.','']
    for row in data['decisions']:
        lines += [f"## {row['gap']}: {row['status']}",
                  f"Domain: {row['domain']}. Recorded `{row['evidence_category']}` lines: {row['recorded_count']}.", '',
                  '**Missing:** '+row['missing'], '', '**Next code decision:** '+row['next_code_decision'], '']
    if data['hud_unmasked_bypass_observed']:
        lines += ['## HUD contention', 'A snapshot reports a nonzero cumulative count for the inherited unmasked bypass. This may include an earlier capture in the same process; it does not by itself assign the bypass to this individual window.', '']
    lines += ['## Reading limits', *['- '+x for x in data['limits']], '',
              'Exact source line excerpts, duplicate hashes, internal timestamps and capture transitions are retained in the JSON report.', '']
    return '\n'.join(lines)

def main() -> int:
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('logs',nargs='+',type=Path)
    ap.add_argument('--out',required=True,type=Path)
    ap.add_argument('--markdown',type=Path)
    ap.add_argument('--max-mib',type=int,default=512)
    args=ap.parse_args()
    if args.max_mib<1: ap.error('--max-mib must be positive')
    data=decisions(inspect_paths(args.logs,args.max_mib*1024*1024))
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8')
    if args.markdown:
        args.markdown.parent.mkdir(parents=True,exist_ok=True)
        args.markdown.write_text(markdown(data),encoding='utf-8')
    print(f"{data['current_build_files']} matching build logs; decision report -> {args.out}")
    return 1 if data['source_review']['errors'] else 0
if __name__=='__main__': sys.exit(main())
