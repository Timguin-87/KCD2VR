"""Verify the complete optional H15 gore package and its reviewed T3 provenance."""
from pathlib import Path
import argparse
import hashlib
import json
import re
import xml.etree.ElementTree as ET
import zipfile

T3_PAK_SHA256 = 'f6acbd14f4b4aae62ea845d4f9a932a28e0d8770420160119bd4a3abbbea08c4'
T3_LUA_SHA256 = '10dbc1ed9edcfd85ddfcb12a43e30ffee4c94fed116156bf2deb6e9549b3b847'
H15_PAK_SHA256 = 'd63c3440d361d1ebdb5f0bd99389bb2e3aab117ee3b90c63d4b03019e0113bb5'
LUA_MEMBER = 'scripts/startup/zzkcd2vr_gore.lua'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def normalized(name):
    name = name.replace('\\', '/').rstrip('/')
    if not name or name.startswith('/') or ':' in name or any(
            part in ('', '.', '..') for part in name.split('/')):
        raise ValueError('Unsafe PAK member path: ' + repr(name))
    return name.casefold()


def verify(root):
    root = Path(root).resolve()
    folder = root/'companion/OptionalGoreH15/Mods/kcd2vr_gore'
    pak = folder/'Data/kcd2vr_gore.pak'
    source_lua = root/'gore_pak/Scripts/Startup/zzkcd2vr_gore.lua'
    source_manifest = root/'gore_pak/mod.manifest'
    manifest = folder/'mod.manifest'
    provenance = pak.with_name(pak.name + '.provenance.json')
    for path in (pak, source_lua, source_manifest, manifest, provenance):
        if not path.is_file():
            raise ValueError('Missing paired optional gore file: ' + str(path))
    script = source_lua.read_bytes()
    if not re.search(rb"G\.version\s*=\s*'61\.0-T3-H15'", script):
        raise ValueError('Unexpected optional gore Lua version')
    if not re.search(rb'G\.protocol\s*=\s*1\b', script):
        raise ValueError('Unexpected optional gore Lua protocol')
    manifest_bytes = manifest.read_bytes()
    if manifest_bytes != source_manifest.read_bytes():
        raise ValueError('Optional gore manifest/source pairing differs')
    xml = ET.fromstring(manifest_bytes)
    if xml.findtext('info/modid') != 'kcdvr_gore' or xml.findtext('info/version') != '61.0.1':
        raise ValueError('Unexpected optional gore manifest identity/version')
    record = json.loads(provenance.read_text(encoding='utf-8'))
    with zipfile.ZipFile(pak) as archive:
        members = {}
        for info in archive.infolist():
            key = normalized(info.filename)
            if key in members:
                raise ValueError('Duplicate normalized PAK member: ' + key)
            members[key] = info
        try:
            bad = archive.testzip()
        except Exception as exc:
            raise ValueError('PAK CRC/data check failed') from exc
        if bad is not None:
            raise ValueError('PAK CRC/data check failed: ' + bad)
        if LUA_MEMBER not in members or archive.read(members[LUA_MEMBER]) != script:
            raise ValueError('Embedded optional gore Lua/source pairing differs')
        actual_pak_sha = sha(pak.read_bytes())
        if actual_pak_sha != H15_PAK_SHA256:
            raise ValueError('PAK differs from the reviewed H15 artifact')
        if record.get('source_sha256') != T3_PAK_SHA256:
            raise ValueError('Provenance does not identify frozen Claude T3')
        if record.get('output_sha256') != actual_pak_sha or record.get('source_archive_modified') is not False:
            raise ValueError('Provenance output hash/source preservation differs')
        changed = record.get('changed', [])
        preserved = record.get('preserved', [])
        if len(changed) != 1 or normalized(changed[0]['name']) != LUA_MEMBER or \
                changed[0].get('previous_sha256') != T3_LUA_SHA256:
            raise ValueError('Provenance changed members differ from the T3 Lua replacement')
        if len(preserved) != 3357 or record.get('preserved_count') != len(preserved):
            raise ValueError('Provenance preserved asset count differs')
        covered = set()
        for item in changed + preserved:
            key = normalized(item['name'])
            if key in covered or key not in members:
                raise ValueError('Provenance duplicate/missing member: ' + key)
            payload = archive.read(members[key])
            if item.get('sha256') != sha(payload) or item.get('size') != len(payload):
                raise ValueError('Provenance member hash/size differs: ' + key)
            covered.add(key)
        if covered != set(members):
            raise ValueError('Provenance does not cover every PAK member')
    return {
        'verified': True, 'folder': str(folder), 'pak_sha256': actual_pak_sha,
        'lua_sha256': sha(script), 'manifest_sha256': sha(manifest_bytes),
        'source_t3_sha256': T3_PAK_SHA256, 'entries': len(members),
        'preserved_members': len(preserved), 'crc': 'PASS',
        'normalized_duplicates': 0, 'exact_lua_pair': True, 'exact_manifest_pair': True,
        'version': '61.0-T3-H15',
    }


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
    args = parser.parse_args()
    print(json.dumps(verify(args.root), indent=2))
