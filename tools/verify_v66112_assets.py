"""Read-only asset provenance and animation-event audit; no game execution.

Missing entries in the declared event database mean 'not listed', not proof
that a native action has no timing/transition dependency on its animation.
"""
import argparse
import hashlib
import json
from pathlib import Path, PurePosixPath
import xml.etree.ElementTree as ET
import zipfile

from generate_combat_allowlist112 import derive


def audit(game, adb_archive):
    entries, provenance = derive(adb_archive)
    with zipfile.ZipFile(game / 'Data/IPL_Characters-part3.pak') as chars:
        path = 'Objects/characters/humans/male/skeleton/male.chrparams'
        params = chars.read(path)
    definitions = ET.fromstring(params).find('AnimationList')
    assert any(a.get('name') == '*' and a.get('path') == '*\\*.caf' for a in definitions)
    database = next(a.get('path').replace('\\', '/').lower() for a in definitions
                    if a.get('name') == '$AnimEventDatabase')
    with zipfile.ZipFile(game / 'Data/Animations.pak') as z:
        names = {n.lower(): n for n in z.namelist()}
        event_bytes = z.read(names[database])
        events = {}
        for a in ET.fromstring(event_bytes):
            events.setdefault(PurePosixPath(a.get('name').replace('\\', '/')).stem.lower(), []).append(a)
        spaces = {}
        for n in z.namelist():
            if n.lower().endswith('.bspace'):
                spaces.setdefault(PurePosixPath(n).stem.lower(), []).append(n)
        hashes = {path: hashlib.sha256(params).hexdigest(), database: hashlib.sha256(event_bytes).hexdigest()}

        def expand(name, visited):
            assert name not in visited, ('recursive blendspace', name)
            if name in spaces:
                assert len(spaces[name]) == 1, ('ambiguous blendspace', name)
                p = spaces[name][0]
                data = z.read(p)
                hashes[p] = hashlib.sha256(data).hexdigest()
                examples = list(ET.fromstring(data).iter('Example'))
                assert examples, ('empty blendspace', name)
                return [r for e in examples for r in expand(e.attrib['AName'].lower(), visited | {name})]
            rows = events.get(name, [])
            assert len(rows) <= 1, ('ambiguous CAF basename', name)
            return [{'alias': name, 'database_entry': bool(rows),
                     'events': sorted(set(e.get('name') for a in rows for e in a))}]

        aliases = {n: expand(n.lower(), set()) for n in entries}
    event_names = sorted(set(e for rows in aliases.values() for r in rows for e in r['events']))
    assert set(event_names) <= {'audio_trigger', 'foley'}, event_names
    with zipfile.ZipFile(game / 'Data/Tables.pak') as z:
        n = next(n for n in z.namelist() if n.lower().endswith('/combat_zone.xml'))
        data = z.read(n)
        hashes[n] = hashlib.sha256(data).hexdigest()
        zones = [dict(e.attrib) for e in ET.fromstring(data).iter() if 'combat_zone_id' in e.attrib]
    return {'alias_count': len(aliases), 'adb_file_count': len(provenance['source_sha256']),
            'declared_event_database': database, 'source_sha256': hashes,
            'event_names_present': event_names, 'aliases': aliases, 'combat_zones': zones,
            'boundary': 'No native clip/action timing, collision acceptance, or damage proof. Missing CAF event entries are not runtime validation.'}


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('--game', required=True, type=Path)
    p.add_argument('--adb', required=True, type=Path)
    p.add_argument('--out', required=True, type=Path)
    a = p.parse_args()
    report = audit(a.game, a.adb)
    a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps(report, indent=2) + '\n')
    print(f"PASS {report['alias_count']} aliases; declared events {report['event_names_present']}; native timing remains unverified")
