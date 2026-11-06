"""Author dedicated native SPECIAL kick rows without changing native outcomes.

The new FreeAttack and CombatAttackGen entries are copies of the corresponding
retail kick graph and row. Only equipment, input-family tags and unique row IDs
change. HEAVY fire cannot select these additions. This generator preserves the
current donor, including the earlier melee kick and masterstrike extensions.
"""
import argparse
import copy
import hashlib
import json
from pathlib import Path
import re
import uuid
import xml.etree.ElementTree as ET
from xml.sax.saxutils import quoteattr
import zipfile

NAMESPACE = uuid.UUID('acb3b9fa-b1a2-4bc5-bec9-72d720d720d7')
ATTACK = 'Libs/Tables/combat/combat_action_attack.xml'
CONFIG = 'Libs/Tables/combat/combat_attack_config__kcdvr.xml'
GENERATED = 'Animations/Mannequin/ADB/kcd_male_combat_generated.adb'
DATABASE = 'Animations/Mannequin/ADB/kcd_male_combat_database.adb'
EQUIPMENT = ('r_weapon_class_id', 'l_weapon_class_id',
             'r_weapon_group_id', 'l_weapon_group_id')


def sha(data):
    return hashlib.sha256(data).hexdigest()


def serialize_row(tag, row):
    return '<' + tag + ' ' + ' '.join(k + '=' + quoteattr(v) for k, v in row.items()) + ' />'


def append_rows(text, parent, tag, rows):
    needle = '</' + parent + '>'
    assert text.count(needle) == 1
    return text.replace(needle, '\n'.join('\t\t' + serialize_row(tag, r) for r in rows) + '\n\t' + needle)


def generate(source, tables, native_adb, report):
    donor = source / 'companion/C63_UNIFIED/Mods/KCD2VR/Data/KCD2VR.pak'
    with zipfile.ZipFile(donor) as z:
        attack = z.read(ATTACK).decode('utf-8-sig')
        config = z.read(CONFIG).decode('utf-8-sig')
        generated = z.read(GENERATED).decode('utf-8-sig')
    native_bytes = native_adb.read_bytes()
    database = native_bytes.decode('utf-8-sig')
    with zipfile.ZipFile(tables) as z:
        retail_rows = [e.attrib for e in ET.fromstring(z.read(ATTACK)).iter('combat_action_attack')]
        groups = [e.attrib for e in ET.fromstring(z.read('Libs/Tables/combat/combat_weapon_group_to_class.xml')).iter('combat_weapon_group_to_class')]
        retail_config = [e.attrib for e in ET.fromstring(z.read('Libs/Tables/combat/combat_attack_config.xml')).iter('combat_attack_config')]
    rows = [e.attrib for e in ET.fromstring(attack).iter('combat_action_attack')]
    assert len(rows) == 233 and len(retail_rows) == 221
    by_id = {r['mn_fragment_guid']: r for r in retail_rows}
    free = by_id['e942fff4-f601-4378-9500-7f6ac20a44ad']
    ordinary = {opp: by_id[guid] for opp, guid in (
        ('NPC', '9c22a61c-fb64-3ac1-b0b8-1d325a2cced7'),
        ('Dog', 'e5c52bdb-ce2e-3f36-8214-f680b55738fb'))}
    fragments = {}
    for name, text in ((DATABASE, database), (GENERATED, generated)):
        for family in ET.fromstring(text).find('FragmentList'):
            for f in family:
                fragments[f.get('GUID')] = (name, family.tag, f)

    # Reuse every native on-foot melee equipment partition. The wildcard and
    # group semantics remain the native table's; no class is relabelled.
    profiles = {}
    for r in retail_rows:
        if r['action_type_id'] != '26' or r['mounted'] != 'false' or r['r_weapon_class_id'] == '12':
            continue
        f = fragments[r['mn_fragment_guid']][2]
        p = tuple(r[k] for k in EQUIPMENT)
        profiles[p] = (f.get('Tags', ''), '+'.join(t for t in f.get('FragTags', '').split('+') if t.startswith(('l_', 'r_'))), False)
    # Native global equipment tags: bow/crossbow occupy the left hand, rifle
    # the right. Class14/15 use the same native l_crossbow tag as class10.
    # Distinct options below retain exact table/GUID/option agreement.
    extras = [
        ('12', '9', '-1', '-1', 'l_bow', ''),
        ('9', '9', '-1', '-1', 'l_bow', ''),
        ('12', '10', '-1', '-1', 'l_crossbow', ''),
        ('10', '10', '-1', '-1', 'l_crossbow', ''),
        ('12', '14', '-1', '-1', 'l_crossbow', ''),
        ('14', '14', '-1', '-1', 'l_crossbow', ''),
        ('12', '15', '-1', '-1', 'l_crossbow', ''),
        ('15', '15', '-1', '-1', 'l_crossbow', ''),
        ('13', '12', '-1', '-1', 'r_rifle', ''),
        ('13', '13', '-1', '-1', 'r_rifle', ''),
        ('12', '0', '-1', '-1', 'l_dagger+r_noweapon', ''),
        ('12', '8', '-1', '-1', 'l_shield+r_noweapon', ''),
        ('12', '17', '-1', '-1', 'l_shield+r_noweapon', ''),
        ('-1', '17', '8', '-1', 'l_shield', 'r_bluntWeapon'),
        ('-1', '17', '9', '-1', 'l_shield', 'r_swords'),
    ]
    for rc, lc, rg, lg, tags, ftags in extras:
        profiles[(rc, lc, rg, lg)] = (tags, ftags, True)

    counts = {}
    for _, family, f in fragments.values():
        key = family, f.get('Tags', ''), f.get('FragTags', '')
        counts[key] = counts.get(key, 0) + 1
    additions = []
    clones = {DATABASE: [], GENERATED: []}
    lineage = []
    for equipment, (tags, equipment_tags, extra) in profiles.items():
        donors = [free] + (list(ordinary.values()) if extra else [])
        for original in donors:
            row = dict(original)
            row.update(dict(zip(EQUIPMENT, equipment)))
            row['input_class_id'] = '2'
            name, family, template = fragments[original['mn_fragment_guid']]
            frag = copy.deepcopy(template)
            semantic = [t for t in template.get('FragTags', '').split('+') if not t.startswith(('r_', 'l_'))]
            semantic = ['attack_special' if t == 'attack_heavy' else t for t in semantic]
            ftags = '+'.join(([equipment_tags] if equipment_tags else []) + semantic)
            guid = str(uuid.uuid5(NAMESPACE, original['mn_fragment_guid'] + '/' + '/'.join(equipment)))
            row.update(mn_fragment_guid=guid, mn_tags='+'.join(x for x in (tags, ftags) if x))
            option = counts.get((family, tags, ftags), 0)
            row['mn_option_index'] = str(option)
            counts[(family, tags, ftags)] = option + 1
            frag.set('GUID', guid)
            if tags:
                frag.set('Tags', tags)
            else:
                frag.attrib.pop('Tags', None)
            frag.set('FragTags', ftags)
            assert list(frag) and [ET.tostring(x) for x in frag] == [ET.tostring(x) for x in template]
            additions.append(row)
            clones[name].append((family, frag))
            lineage.append({'guid': guid, 'donor': original['mn_fragment_guid'], 'family': family,
                            'equipment': equipment, 'option': option, 'input': 2,
                            'graph_children_sha256': sha(b''.join(ET.tostring(x) for x in frag))})

    output = {ATTACK: append_rows(attack, 'combat_action_attacks', 'combat_action_attack', additions)}
    for name, text in ((DATABASE, database), (GENERATED, generated)):
        for family in sorted({f for f, _ in clones[name]}):
            needle = '</' + family + '>'
            assert text.count(needle) == 1
            inserted = '\n'.join(ET.tostring(f, encoding='unicode') for fam, f in clones[name] if fam == family)
            text = text.replace(needle, inserted + '\n\t\t' + needle)
        output[name] = text
    cfg = retail_config + [e.attrib for e in ET.fromstring(config).iter('combat_attack_config')]
    seen = {(r['combat_input_class_id'], r['combat_zone_id'], r['r_weapon_class_id'], r['l_weapon_class_id']) for r in cfg}
    next_id = max(int(r['combat_attack_config_id']) for r in cfg) + 1
    new_config = []
    for (rc, lc, rg, _), _ in profiles.items():
        right = [rc] if rg == '-1' else [g['weapon_class_id'] for g in groups if g['combat_weapon_group_id'] == rg]
        for right_class in right:
            key = ('2', '5', right_class, lc)
            if key in seen:
                continue
            seen.add(key)
            new_config.append(dict(combat_attack_config_id=str(next_id), combat_input_class_id='2', combat_zone_id='5',
                                   r_weapon_class_id=right_class, l_weapon_class_id=lc))
            next_id += 1
    output[CONFIG] = append_rows(config, 'combat_attack_configs', 'combat_attack_config', new_config)
    for name, text in output.items():
        ET.fromstring(text)
        path = source / 'companion/source' / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding='utf-8', newline='')
    receipt = {'native_adb_sha256': sha(native_bytes), 'donor_pak_sha256': sha(donor.read_bytes()),
               'retail_tables_sha256': sha(tables.read_bytes()), 'attack_rows_before': len(rows),
               'attack_rows_after': len(rows) + len(additions), 'added_config_rows': len(new_config),
               'profiles': len(profiles), 'lineage': lineage,
               'members': {k: sha(v.encode('utf-8')) for k, v in output.items()},
               'unchanged': 'All original attack/config rows and original fragment XML are preserved. All copied animation/procedural children are byte-identical after XML normalization. Native RPG, collision and action admission remain native.'}
    report.parent.mkdir(parents=True, exist_ok=True)
    report.write_text(json.dumps(receipt, indent=2) + '\n')
    print(json.dumps({k: receipt[k] for k in ('attack_rows_before', 'attack_rows_after', 'added_config_rows', 'profiles')}))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--tables', type=Path, required=True)
    parser.add_argument('--native-adb', type=Path, required=True)
    parser.add_argument('--report', type=Path, required=True)
    args = parser.parse_args()
    generate(args.source, args.tables, args.native_adb, args.report)
