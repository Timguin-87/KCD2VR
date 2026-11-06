#!/usr/bin/env python3
"""D60A: how late the drawn hands are, from an existing F2 capture (no new recording).

Exact joins and one explicitly provisional time-window association:
  stage 13 HANDOFF_ENQUEUE   generation -> XR frame D (sample) and its display time (poseTime)
  stage 10 SOURCE_PICKUP     same generation, publication = camera frame C whose head rendered the image
  stage 14 PAIR_QUEUE_*      item = the image (source pair) -> its publish time
  stage 4  ACCEPTED_TRACKED_BODY_CAMERA_BASE publication -> stage 2 COMPLETED_MODEL_POSE (same publication)
           -> poseTime = display time of the XR family H that posed the body/hands
D-C uses the recorded generation/camera identity. D-H and same-family percentages
are PROVISIONAL: stage14 has no exact body-publication link, so the latest body
base in a preceding40ms window is only a hypothesis. Queue publication is not
GPU completion or headset scanout. Do not use D-H to tune prediction as measured truth.
Usage: hand_present_lag_d60a.py KCD2VR-diagnostics.log[.gz]
"""
import bisect, collections, gzip, re, sys
PAT = re.compile(r'F2_CAUSAL stage=(\d+) order=\d+ session=(\d+) tick=\d+ cpuNs=(\d+) thread=\d+ sample=(\d+) '
                 r'publication=(\d+) epoch=\d+ sourceEpoch=\d+ sourceNs=\d+ poseTime=(-?\d+) actor=\w+ object=\w+ '
                 r'item=(\w+) generation=(\d+) side=(-?\d+) code=-?\d+ flags=\d+ reason=(\S+)')
def main(path):
    op = gzip.open if path.endswith('.gz') else open
    sessions = collections.defaultdict(list)
    with op(path, 'rt', errors='replace') as f:
        for line in f:
            m = PAT.search(line)
            if m: sessions[m[2]].append(m.groups())
    head, hand, joint = collections.Counter(), collections.Counter(), collections.Counter()
    for L in sessions.values():
        L.sort(key=lambda e: int(e[2]))
        s13 = {e[7]: (int(e[3]), int(e[5])) for e in L if e[0] == '13'}
        frame_of_display = {int(e[5]): int(e[3]) for e in L if e[0] == '13'}
        pose_display = {e[4]: int(e[5]) for e in L if e[0] == '2' and e[8] == '0'}
        base = [(int(e[2]), e[4]) for e in L if e[0] == '4' and e[9] == 'ACCEPTED_TRACKED_BODY_CAMERA_BASE']
        base_t = [b[0] for b in base]
        published = {e[6]: int(e[2]) for e in L if e[0] == '14'}
        for e in L:
            if e[0] != '10' or e[7] not in s13 or e[6] not in published: continue
            D, _ = s13[e[7]]; C = int(e[4]); head[D - C] += 1
            j = bisect.bisect_right(base_t, published[e[6]]) - 1
            # PROVISIONAL temporal candidate, not evidence this body belongs to this source image.
            H = frame_of_display.get(pose_display.get(base[j][1])) if j >= 0 and published[e[6]] - base_t[j] <= 40_000_000 else None
            if H: hand[D - H] += 1; joint[(D - C, D - H)] += 1
    show = lambda c: ', '.join(f'{k}:{v} ({100*v/sum(c.values()):.1f}%)' for k, v in sorted(c.items())[:6])
    print('images joined (camera frame C -> submitting frame D):', sum(head.values()))
    print('D-C frames :', show(head))
    print('D-H frames (PROVISIONAL temporal association):', show(hand), '(gameplay frames with a tracked body)')
    print('same family for head and hands (PROVISIONAL, D-C == D-H):', f'{100*sum(v for k,v in joint.items() if k[0]==k[1])/max(1,sum(joint.values())):.1f}%')
if __name__ == '__main__':
    main(sys.argv[1])
