"""Extract the Oct 3 failure evidence without promoting logged requests to hits."""
import argparse
from collections import Counter, defaultdict
import hashlib
import json
from pathlib import Path
import re


def analyze(folder):
    files = {}
    for name in ('KCD2VR.log', 'KCD2VR-diagnostics.log', 'KCD2VR.ini'):
        data = (folder/name).read_bytes()
        files[name] = dict(bytes=len(data), sha256=hashlib.sha256(data).hexdigest())
    lines = (folder/'KCD2VR-diagnostics.log').read_text(errors='replace').splitlines()
    buckets = defaultdict(Counter)
    changes, examples, head = [], {}, []
    sweeps, aliases = Counter(), Counter()
    max_angle, f3 = 0., 0
    contact_unavailable = 0
    for number, line in enumerate(lines, 1):
        stamp = line[1:24]
        m = re.search(r'V66.114-LAB .*F3=(\d) F5=(\d)', line)
        if m:
            f3 = int(m[1]); changes.append(dict(line=number, time=stamp, f3=f3, f5=int(m[2])))
        b = buckets[f3]
        if 'V66.90-LAB STATUS' in line:
            status = re.search(r'F3=(\d).*committedPose=(\d)', line)
            if status:
                buckets[int(status[1])]['status_committed_'+status[2]] += 1
        if 'POSE-REJECT' in line:
            reason = re.search(r'reason=(\S+)', line)[1]
            b['pose_reject_'+reason] += 1
            examples.setdefault('F3='+str(f3)+' reject '+reason, dict(line=number, text=line))
        if 'V66.89-POSE COMMIT' in line:
            b['pose_commit_log_records'] += 1
        if 'WEAPON_POSE rung=' in line:
            b['weapon_pose_log_records'] += 1
            m = re.search(r'contactKnown=(\d) blocked=(\d)', line)
            b['weapon_known_'+m[1]+'_blocked_'+m[2]] += 1
            a = float(re.search(r'controllerToAllowedDeg=([0-9.]+)', line)[1])
            if a > max_angle:
                max_angle = a; examples['maximum_weapon_angle'] = dict(line=number, text=line)
        if 'CONTACT_OBSERVER unavailable' in line:
            contact_unavailable += 1
            examples.setdefault('contact_unavailable', dict(line=number, text=line))
        if 'V66.113-REFERENCE-SWEEP' in line:
            caller = re.search(r'callerRva=(\S+)', line)[1]
            result = re.search(r'resultNonzero=(\d)', line)[1]
            sweeps[caller+' resultNonzero='+result] += 1
        if 'PLAYBACK CLIP' in line and 'role=HENRY' in line:
            m = re.search(r'layer=(\d+).*activated=(\d).*name="([^"]+)"', line)
            if m and m[2]=='1' and m[3]=='holditem_idle_sword_rhand':
                aliases[f'F3={f3} layer={m[1]}'] += 1
                examples.setdefault('holding_idle_F3='+str(f3), dict(line=number, text=line))
        if 'V66.107-HEAD' in line:
            head.append(dict(line=number, text=line))
        if 'COLLISION STATUS' in line:
            examples['last_static_collision_status'] = dict(line=number, text=line)
        if 'V66.112-MOTION STATUS' in line:
            examples['last_native_motion_status'] = dict(line=number, text=line)
    ini = (folder/'KCD2VR.ini').read_text(errors='replace')
    settings = {name: re.findall(r'^'+name+r'\s*=\s*(.*)$', ini, re.M)
                for name in ('HeadPoseScale','VrCameraHeightOffsetMeters','StandingEyeHeightM')}
    return dict(files=files, diagnostic_lines=len(lines), modes={k:dict(v) for k,v in sorted(buckets.items())},
                mode_transitions=changes, contact_observer_unavailable=contact_unavailable,
                reference_sweeps=dict(sweeps), sword_idle_samples=dict(aliases),
                max_logged_controller_to_allowed_degrees=max_angle, head_reanchors=head,
                relevant_ini=settings, examples=examples,
                caveat='Counts are log records, not frame counts. Sparse diagnostic absence does not prove a native action never occurred. Raw sweep results are not accepted hits.')


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('input',type=Path);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
    report=analyze(a.input);a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:report[k] for k in ('diagnostic_lines','modes','contact_observer_unavailable','reference_sweeps','sword_idle_samples','max_logged_controller_to_allowed_degrees')},indent=2))
