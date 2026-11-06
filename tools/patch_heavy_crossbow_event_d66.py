#!/usr/bin/env python3
"""Reproduce the one-event heavy-crossbow native-data correction; never install.

Only the exact retail male.animevents member is accepted. The original native
handler already understands DespawnLever; its misspelled heavy event was ignored.
No CAF timing, sound, other event, table, native ABI or character pose is changed.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import xml.etree.ElementTree as ET
import zipfile

MEMBER = 'Animations/humans/male/male.animevents'
SOURCE_SHA256 = 'b884f960abc117e3c0d406a20fa3e1f14d79ccc25d0b4258e58d6c0bc3df6a26'
CLIP = 'animations/humans/male/shooting_guns/crossbow/crossbow_heavy_charging_phase1.caf'
OLD = b'name="DespawLever"'
NEW = b'name="DespawnLever"'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def patch(raw):
    if sha(raw) != SOURCE_SHA256:
        raise ValueError('Exact retail male.animevents fingerprint mismatch')
    tree = ET.fromstring(raw)
    clips = [c for c in tree if c.tag == 'animation' and c.get('name') == CLIP]
    if len(clips) != 1:
        raise ValueError('Expected exactly one heavy-crossbow CAF event group')
    events = [e for e in clips[0] if e.get('name') == 'DespawLever']
    if len(events) != 1 or events[0].get('time') != '0.8178125' or events[0].get('endTime') != '0.8178169':
        raise ValueError('Heavy event identity/timing differs')
    if raw.count(OLD) != 1:
        raise ValueError('Expected exactly one misspelled event name')
    at = raw.index(OLD)
    result = raw[:at] + NEW + raw[at + len(OLD):]
    # This is precisely one inserted ASCII n; no XML serializer/reformatting.
    insertion = at + len(b'name="Despaw')
    if result[:insertion] != raw[:insertion] or result[insertion:insertion+1] != b'n' or result[insertion+1:] != raw[insertion:]:
        raise ValueError('Unrelated event bytes changed')
    after = ET.fromstring(result)
    corrected = [e for c in after if c.get('name') == CLIP for e in c if e.get('name') == 'DespawnLever']
    if len(corrected) != 1 or corrected[0].attrib != {**events[0].attrib, 'name': 'DespawnLever'}:
        raise ValueError('Corrected event attributes differ')
    return result, {
        'schema': 1, 'source_member': MEMBER, 'source_sha256': sha(raw),
        'source_bytes': len(raw), 'output_sha256': sha(result), 'output_bytes': len(result),
        'clip': CLIP, 'before': dict(events[0].attrib), 'after': dict(corrected[0].attrib),
        'edit': {'kind': 'one_ascii_n_insertion', 'offset': insertion},
        'unchanged_prefix_sha256': sha(raw[:insertion]),
        'unchanged_suffix_sha256': sha(raw[insertion:]),
        'other_bytes_identical': True, 'game_files_written': False,
        'native_handler': {'WHGame_sha256': 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7',
                           'timestamp': '6A350E20', 'dispatcher_rva': '289CCD0',
                           'recognized_crc32': '3D6870EB', 'unrecognized_typo_crc32': '441B89A7'},
        'scope': 'One authored heavy-reload event for every actor using that exact CAF; native visibility handler retained',
        'headset': 'NOT_RUN',
    }


def atomic_write(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    temp = path.with_name(path.name + '.d66.tmp')
    temp.write_bytes(data)
    os.replace(temp, path)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--retail-pak', required=True, type=Path)
    ap.add_argument('--source', default=Path(__file__).resolve().parents[1], type=Path)
    ap.add_argument('--self-test', action='store_true')
    args = ap.parse_args()
    source = args.source.resolve()
    if 'steamapps' in {p.lower() for p in source.parts}:
        raise SystemExit('Source-generation output must not be a game installation')
    with zipfile.ZipFile(args.retail_pak) as z:
        exact = [n for n in z.namelist() if n.casefold() == MEMBER.casefold()]
        if exact != [MEMBER]:
            raise ValueError('Missing, renamed or duplicated retail member')
        raw = z.read(MEMBER)
    result, receipt = patch(raw)
    if args.self_test:
        rejected = []
        for name, invalid in [('changed_byte', b'X' + raw[1:]), ('already_patched', result),
                              ('truncated', raw[:-1]), ('duplicate_event', raw + OLD)]:
            try:
                patch(invalid)
            except ValueError:
                rejected.append(name)
            else:
                raise AssertionError('Accepted invalid input: ' + name)
        assert len(rejected) == 4 and len(result) == len(raw) + 1
        receipt['focused_checks'] = {'exact_single_byte_edit': 'PASS', 'rejected_inputs': rejected}
    target = source / 'companion/source' / MEMBER
    manifest = source / 'repair_evidence/d65/heavy_crossbow_event/GENERATED_MANIFEST.json'
    atomic_write(target, result)
    atomic_write(manifest, (json.dumps(receipt, indent=2) + '\n').encode())
    print(json.dumps({'output': str(target), 'output_sha256': receipt['output_sha256'],
                      'bytes': len(result), 'exact_one_byte_insertion': True,
                      'focused_checks': receipt.get('focused_checks'), 'game_files_written': False}, indent=2))


if __name__ == '__main__':
    main()
