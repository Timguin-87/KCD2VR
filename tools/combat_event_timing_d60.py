#!/usr/bin/env python3
"""Read-only join of existing decoded clips to exact retail authored events.

Does not decode another corpus or produce gameplay timing/clock parameters.
Local hand angular velocity is descriptive; it is never a contact/damage marker.
AnimEvent/Blend attributes are preserved raw: no invented absolute layer clock.
"""
import argparse
import hashlib
import json
import math
from pathlib import Path
import xml.etree.ElementTree as ET
import zipfile

ADB = 'Animations/Mannequin/ADB/kcd_male_combat_generated.adb'
EVENTS = 'Animations/humans/male/male.animevents'
RETAIL_HASHES = {
    ADB: '2155d4c466efc9c83f298d593d0a14123b4a1e3b33452d122aecca38cf3e200e',
    EVENTS: 'b884f960abc117e3c0d406a20fa3e1f14d79ccc25d0b4258e58d6c0bc3df6a26',
}


def digest(data):
    return hashlib.sha256(data).hexdigest()


def angular_peak(samples, seconds_per_frame):
    """Return actual key interval, with quaternion sign/scale and dt respected."""
    if not math.isfinite(seconds_per_frame) or seconds_per_frame <= 0:
        return {'status': 'INVALID_SECONDS_PER_FRAME'}
    best = None
    invalid = 0
    for before, after in zip(samples, samples[1:]):
        if len(before) != 5 or len(after) != 5 or not all(math.isfinite(v) for v in before + after):
            invalid += 1
            continue
        dt = (after[0] - before[0]) * seconds_per_frame
        na = math.sqrt(sum(v*v for v in before[1:]))
        nb = math.sqrt(sum(v*v for v in after[1:]))
        if dt <= 0 or na <= 1e-12 or nb <= 1e-12:
            invalid += 1
            continue
        dot = abs(sum(a*b for a, b in zip(before[1:], after[1:])) / (na*nb))
        radians = 2 * math.acos(min(1.0, dot))
        speed = radians / dt
        if best is None or speed > best['radiansPerSecond']:
            best = {'keyFrameBegin': before[0], 'keyFrameEnd': after[0],
                    'deltaSeconds': dt, 'radiansPerSecond': speed}
    return {'status': 'LOCAL_ROTATION_ONLY_NOT_CONTACT' if best else 'NO_VALID_INTERVAL',
            'peak': best, 'invalidIntervals': invalid}


def fragment_index(raw):
    index = {}
    for group in ET.fromstring(raw).find('FragmentList'):
        if 'Combo' not in group.tag and 'MasterStrike' not in group.tag:
            continue
        for fragment in group:
            animations = [a.attrib for a in fragment.findall('.//Animation')]
            procedures = []
            for layer_num, layer in enumerate(fragment.findall('ProcLayer')):
                blend = {}
                for node in layer:
                    if node.tag == 'Blend':
                        blend = dict(node.attrib)
                    elif node.tag == 'Procedural':
                        procedures.append({'layer': layer_num, 'type': node.get('type', ''),
                                           'precedingBlendRaw': dict(blend),
                                           'parameters': [{p.tag: dict(p.attrib)} for p in node.findall('./ProceduralParams/*')]})
            row = {'group': group.tag, 'fragment': dict(fragment.attrib),
                   'animations': animations, 'procedures': procedures}
            for animation in animations:
                alias = animation.get('name', '').lower()
                if alias:
                    index.setdefault(alias, []).append(row)
    return index


def join_clip(name, clip, event_index, retail, companion):
    spf = clip.get('secondsPerFrame', 0)
    joints = clip.get('joints', [])
    if isinstance(joints, dict):
        joints = joints.values()
    hands = {}
    for joint in joints:
        side = joint.get('joint')
        if side in ('LeftHand', 'RightHand'):
            hands[side] = angular_peak(joint.get('rot', []), spf)
    start, end = clip.get('startFrame'), clip.get('endFrame')
    span = (end-start)*spf if isinstance(start, (int, float)) and isinstance(end, (int, float)) and end >= start and spf > 0 else None
    path = clip.get('path', '').replace('\\', '/').lower()
    return {'clip': name, 'path': path, 'startFrame': start, 'endFrame': end,
            'secondsPerFrame': spf, 'authoredFrameSpanSeconds': span, 'localHandMotion': hands,
            'cafEventAttributesRaw': event_index.get(path, []),
            'retailFragments': retail.get(name.lower(), []),
            'currentCompanionFragments': companion.get(name.lower(), [])}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--decoded', type=Path, nargs='+', required=True)
    p.add_argument('--retail-pak', type=Path, required=True)
    p.add_argument('--companion-pak', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    a = p.parse_args()
    provenance = []
    with zipfile.ZipFile(a.retail_pak) as z:
        raw = {n: z.read(n) for n in RETAIL_HASHES}
    for n, b in raw.items():
        actual = digest(b)
        if actual != RETAIL_HASHES[n]:
            raise ValueError('Exact retail member mismatch: ' + n)
        provenance.append({'archive': str(a.retail_pak), 'member': n, 'sha256': actual, 'bytes': len(b)})
    with zipfile.ZipFile(a.companion_pak) as z:
        current = z.read(ADB)
    provenance.append({'archive': str(a.companion_pak), 'member': ADB, 'sha256': digest(current), 'bytes': len(current)})
    events = {}
    for animation in ET.fromstring(raw[EVENTS]):
        events.setdefault(animation.get('name', '').replace('\\', '/').lower(), []).extend(dict(e.attrib) for e in animation if e.tag == 'event')
    retail = fragment_index(raw[ADB])
    companion = fragment_index(current)
    rows = []
    for path in a.decoded:
        b = path.read_bytes()
        provenance.append({'path': str(path), 'sha256': digest(b), 'bytes': len(b)})
        data = json.loads(b)
        if data.get('meta', {}).get('quat') != 'xyzw':
            raise ValueError('Unrecognized quaternion layout in ' + str(path))
        rows.extend(join_clip(n, c, events, retail, companion) for n, c in data['clips'].items())
    report = {'method': 'Exact path/alias join; existing decoder inputs only. Raw native events are not accepted runtime outcomes.',
              'runtimeChanges': 'NONE', 'timingAuthority': 'NOT_ESTABLISHED', 'provenance': provenance,
              'summary': {'clips': len(rows), 'clipsWithCafEvents': sum(bool(r['cafEventAttributesRaw']) for r in rows),
                          'clipsWithAnimationHit': sum(any(e.get('name') == 'AnimationHit' for e in r['cafEventAttributesRaw']) for r in rows),
                          'clipsWithRetailFragment': sum(bool(r['retailFragments']) for r in rows),
                          'clipsWithCompanionFragment': sum(bool(r['currentCompanionFragments']) for r in rows)},
              'clips': rows}
    a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(report['summary']))
    print('Authored events and local hand motion remain separate; no runtime timing imported.')


if __name__ == '__main__':
    main()
