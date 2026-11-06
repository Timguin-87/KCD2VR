"""Patch the latest unified companion with the exact native Ammo carrier assets.

This tool belongs to the integrating chat. It never installs or rewrites its
input. plan_members is usable entirely in memory for source review tests.
"""
import argparse
import copy
import io
import json
from pathlib import Path
import sys
import xml.etree.ElementTree as ET
import zipfile

import companion_patch_t6 as base

REPO = Path(__file__).resolve().parents[4]
sys.path.insert(0, str(REPO / 'companion/optional_throwing_assets1'))
from build_asset_pack import Native, leaf_table, canonical
import ammo_roles_t17 as roles

LUA = 'Scripts/KCD2VR/native_ammo_supply_t18.lua'
TABLE = 'Libs/Tables/item/item__kcd2vr_native_ammo_t18.xml'
SOURCES = {
    base.SUPPLY_MEMBER: REPO / 'src/vrik/native_supply_t2.lua',
    LUA: Path(__file__).with_name('native_ammo_supply_t18.lua'),
}


def plan_members(members, native, *, include_carrier=True):
    expected = roles.rows(native)
    roles.validate(native, expected)
    by_guid = {r.get('Id').lower(): r for r in expected}
    by_name = {r.get('Name'): r for r in expected}
    found = {}
    for name, (_, data) in members.items():
        low = name.lower()
        if not (low.startswith('libs/tables/item/item') and low.endswith('.xml')):
            continue
        root = ET.fromstring(data.decode('utf-8-sig'))
        for row in root.iter():
            guid, label = row.get('Id', '').lower(), row.get('Name', '')
            if guid in by_guid or label in by_name:
                section=root.find('ItemClasses')
                base.require(root.tag == 'database' and section is not None and
                             section.get('version') == '8' and row in list(section),
                             'invalid native Ammo table schema: ' + name)
                base.require(guid in by_guid and label in by_name and by_guid[guid] is by_name[label],
                             'native Ammo identity conflict: ' + name)
                base.require(guid not in found, 'duplicate native Ammo role: ' + name)
                found[guid] = (name, row)
    base.require(not found or len(found) == 3, 'partial native Ammo role set refused')
    # Later carriers reuse only the exact finite role-table migration. They
    # must not need the retired executor sources, even as temporary payloads.
    replacements = {}
    if include_carrier:
        replacements = {name: path.read_bytes() for name, path in SOURCES.items()}
        base.require(b'native_ammo_supply_t18.lua' in replacements[base.SUPPLY_MEMBER], 'missing T18 loader')
        base.require(b'function kcd2vr_ammo_supply_t18' in replacements[LUA], 'missing T18 API')
    if not found:
        base.require(TABLE not in members, 'owned native Ammo table name occupied')
        replacements[TABLE] = leaf_table('ItemClasses', 8, expected)
    elif not all(canonical(row) == canonical(by_guid[guid]) for guid, (_, row) in found.items()):
        # Sole migration: the frozen T17 generator omitted BOTH BrokenGUIDs.
        # Every other attribute/child and the launcher must match exactly. No
        # unknown, mixed, self-referencing or externally modified role is edited.
        for guid, (name, row) in found.items():
            wanted=copy.deepcopy(by_guid[guid])
            if wanted.tag == 'Ammo':
                wanted.attrib.pop('BrokenItemClassId', None)
            base.require(name == TABLE and canonical(row) == canonical(wanted),
                         'unreviewed native Ammo role migration: ' + name)
        old_expected=copy.deepcopy(expected)
        for row in old_expected:
            if row.tag == 'Ammo':row.attrib.pop('BrokenItemClassId')
        root=ET.fromstring(members[TABLE][1].decode('utf-8-sig'))
        base.require(canonical(root) == canonical(ET.fromstring(leaf_table('ItemClasses',8,old_expected))),
                     'unreviewed native Ammo table migration')
        replacements[TABLE]=leaf_table('ItemClasses',8,expected)
    result = copy.deepcopy(members)
    for name, data in replacements.items():
        if name in result:
            info = result[name][0]
        else:
            base.require(name.casefold() not in {n.casefold() for n in result}, 'new asset path alias')
            info = zipfile.ZipInfo(name, (2026, 10, 23, 0, 0, 0))
            info.compress_type = zipfile.ZIP_DEFLATED
            info.external_attr = 0o100644 << 16
        result[name] = (info, data)
    changed = {n for n in result if n not in members or members[n][1] != result[n][1]}
    base.require(changed <= {base.SUPPLY_MEMBER, LUA, TABLE}, 'unrelated assets changed')
    base.require(all(result[n][1] == data for n, (_, data) in members.items() if n not in changed),
                 'unrelated payload not preserved')
    final_rows=[]
    for name, (_, data) in result.items():
        if name.lower().startswith('libs/tables/item/item') and name.lower().endswith('.xml'):
            final_rows.extend(row for row in ET.fromstring(data.decode('utf-8-sig')).iter() if row.get('Id'))
    try:
        roles.validate_overlay(native, final_rows)
    except AssertionError as error:
        raise base.PackageError(str(error)) from error
    return result, changed


def build(input_unified, out, game):
    input_unified, out = input_unified.resolve(), out.resolve()
    base.require(not out.exists() and out != input_unified and input_unified not in out.parents,
                 'output must be a new separate staging directory')
    files = base.read_tree(input_unified)
    frozen = {n: base.sha(data) for n, data in files.items()}
    metadata = json.loads(files[base.MARKER])
    base.require(metadata.get('pak_sha256') == frozen[base.PAK], 'stale input PAK metadata')
    distribution = {n[len(base.MOD):]: base.sha(data) for n, data in files.items()
                    if n.startswith(base.MOD) and n not in (base.PAK, base.MARKER)}
    base.require(metadata.get('distribution_files') == distribution, 'stale input distribution inventory')
    members = base.read_pak(files[base.PAK])
    base.validate_definitions(members, files)  # keep saved melee roles/receipts/localization
    after, changed = plan_members(members, Native(game))
    stream = io.BytesIO()
    with zipfile.ZipFile(stream, 'w') as archive:
        for info, data in after.values():
            archive.writestr(copy.copy(info), data)
    files[base.PAK] = stream.getvalue()
    verified = base.read_pak(files[base.PAK])
    base.require({n: d for n, (_, d) in verified.items()} == {n: d for n, (_, d) in after.items()},
                 'output PAK readback mismatch')
    changes = [{'member': n, 'before': base.sha(members[n][1]) if n in members else None,
                'after': base.sha(after[n][1])} for n in sorted(changed)]
    metadata['t18_parent'] = {'pak_sha256': frozen[base.PAK], 'input': str(input_unified)}
    metadata.update(pak_sha256=base.sha(files[base.PAK]), members=len(after), installed=False,
                    t18_native_ammo=changes)
    provenance = {r['path']: r for r in metadata.get('members_provenance', [])}
    for n in changed:
        row = copy.deepcopy(provenance.get(n, {'path': n, 'sources': []}))
        row['sha256'] = base.sha(after[n][1])
        row.setdefault('sources', []).append(str(SOURCES[n].resolve()) if n in SOURCES else str(Path(roles.__file__).resolve()))
        provenance[n] = row
    metadata['members_provenance'] = [provenance[n] for n in sorted(provenance)]
    for row in metadata.get('explicit_source_replacements', []):
        if row.get('member') in changed:
            n = row['member']; row['before'] = base.sha(members[n][1]) if n in members else None
            row['after'] = base.sha(after[n][1])
    files[base.MARKER] = (json.dumps(metadata, indent=2) + '\n').encode()
    base.require(base.read_tree(input_unified) == {n: (input_unified / n).read_bytes() for n in frozen},
                 'input member set changed while staging')
    base.require({n: base.sha((input_unified / n).read_bytes()) for n in frozen} == frozen,
                 'input bytes changed while staging')
    receipt = {'stage': 'T18_SOURCE_ASSET_OVERLAY', 'input_files': frozen,
               'changed_members': changes, 'unrelated_payloads_preserved': len(members) - len(changed & members.keys()),
               'output_files': {n: base.sha(d) for n, d in files.items()}, 'installed': False,
               'native_or_headset_executed': False}
    for n, data in files.items():
        target = out / n; target.parent.mkdir(parents=True, exist_ok=True); target.write_bytes(data)
    base.require(base.read_tree(out) == files, 'staged distribution readback mismatch')
    (out / 'T18_PACKAGE_RECEIPT.json').write_text(json.dumps(receipt, indent=2) + '\n', encoding='utf-8')
    return receipt


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--input-unified', required=True, type=Path)
    parser.add_argument('--out', required=True, type=Path)
    parser.add_argument('--game', required=True, type=Path)
    args = parser.parse_args()
    print(json.dumps(build(args.input_unified, args.out, args.game), indent=2))
