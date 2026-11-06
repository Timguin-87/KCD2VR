"""Compact, reproducible findings from the supplied V118 runtime pair; no raw corpus copy."""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import re


def summarize(main, diagnostics):
    inputs = [dict(path=str(p), bytes=p.stat().st_size,
                   sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in (main, diagnostics)]
    lines = diagnostics.read_text(encoding='utf-8-sig', errors='replace').splitlines()
    def selected(token):
        return [dict(line=i+1, text=s) for i, s in enumerate(lines) if token in s]
    requests = selected('PLAYER-ACTION REQUEST')
    sweeps = selected('NATIVE-WEAPON-SWEEP')
    poses = selected('WEAPON_POSE')
    def counts(rows, expression):
        return dict(Counter(re.search(expression, r['text'])[1] for r in rows if re.search(expression, r['text'])))
    return dict(inputs=inputs, diagnostic_lines=len(lines),
        build_lines=[s for s in main.read_text(errors='replace').splitlines() if 'KCD2VR BUILD' in s],
        requests_by_kind=counts(requests, r'kind=(\d+)'), request_count=len(requests),
        sweep_count=len(sweeps), sweep_callers=counts(sweeps, r'callerRva=([0-9A-F]+)'),
        sweep_mutations=counts(sweeps, r'mutation=(\S+)'),
        weapon_pose_count=len(poses),
        weapon_pose_contact_states=counts(poses, r'(contactKnown=\d blocked=\d sweepDeferred=\d)'),
        absent_or_present={s:len(selected(s)) for s in ['MELEE_INTENT', 'MELEE_INPUT', 'MATERIAL_EFFECT', 'GUARD edge=']},
        f12_transitions=selected('V66.94-F12 rung='), lab_transitions=selected('V66.114-LAB'),
        fist_events=selected('V66.110-FIST'),
        representative_sweeps=sweeps[:2],
        representative_contacts=[r for r in poses if 'blocked=1' in r['text']][:6],
        interpretation_limits=[
            'Requests returned from OnAction are not evidence of accepted attacks or hits.',
            'Sparse pose diagnostics are not a contact rate or a complete collision census.',
            'Deferred sweeps are incomplete queries, not confirmed surface impacts.',
            'No material classification or spark origin can be inferred from this pair.',
            'These logs identify V118, not the supplied V118R1 HUD baseline or V119 candidate.'])


if __name__ == '__main__':
    p=argparse.ArgumentParser();p.add_argument('--main',type=Path,required=True)
    p.add_argument('--diagnostics',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
    a=p.parse_args();r=summarize(a.main,a.diagnostics);a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_text(json.dumps(r,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({k:r[k] for k in ['request_count','sweep_count','sweep_callers','weapon_pose_contact_states']}))
