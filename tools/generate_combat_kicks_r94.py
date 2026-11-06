#!/usr/bin/env python3
"""R94: dedicated native SPECIAL (input class 2) COMBAT kick rows for every melee loadout.

Same method as generate_kicks_d72.py, but works on the in-tree companion sources only
(no donor pak, no retail Tables.pak needed):
  donor = each existing input-class-1 COMBAT kick row (action 3, attack type 4, zone 5)
          of a melee equipment partition (the retail combat partitions: longsword 4/4,
          polearm 7/7, one-hand swords group 10 / left group 2, blunt group 8 / left group 2,
          swords group 9 + shield 8, blunt group 8 + shield 8), NPC (oppMale) and Dog variant.
  copy  = the same row and the same Mannequin fragment (byte-identical animation/procedural
          children), only: input_class_id 1 -> 2, FragTags attack_heavy -> attack_special,
          a new GUID uuid5(D72 namespace, donor GUID + '/combat-special-r94'), the next
          free mn_option_index for that (family, Tags, FragTags).
Retail action-3 rows never use input class 2 (only CombatAttackMercy action 50 does),
so HEAVY attacks and the mercy kill cannot select these rows; only the R3 kick that
sends the SPECIAL input does. combat_attack_config (2, zone 5, class) entries for every
melee class already exist (C33/D72), checked here, not added.
Idempotent: a second run changes nothing.
"""
import argparse
import re
import sys
import uuid
from pathlib import Path
import xml.etree.ElementTree as ET
from xml.sax.saxutils import quoteattr

NAMESPACE = uuid.UUID('acb3b9fa-b1a2-4bc5-bec9-72d720d720d7')  # same namespace as D72
SUFFIX = '/combat-special-r94'
ATTACK = 'Libs/Tables/combat/combat_action_attack.xml'
CONFIG = 'Libs/Tables/combat/combat_attack_config__kcdvr.xml'
GENERATED = 'Animations/Mannequin/ADB/kcd_male_combat_generated.adb'
FAMILY = 'CombatAttackGen'
# right class / left class / right group / left group of the retail melee combat partitions
MELEE = {('4', '4', '-1', '-1'), ('7', '7', '-1', '-1'), ('-1', '-1', '10', '2'), ('-1', '-1', '8', '2'),
         ('-1', '8', '9', '-1'), ('-1', '8', '8', '-1')}
EQUIPMENT = ('r_weapon_class_id', 'l_weapon_class_id', 'r_weapon_group_id', 'l_weapon_group_id')
# retail combat_weapon_group_to_class (Tables.pak 1.5.6) for the groups used above
GROUPS = {'8': ['3', '5', '6'], '9': ['1', '2', '4', '16'], '10': ['1', '2', '4', '16']}


def attrs(raw):
    return dict(re.findall(r'(\w+)="([^"]*)"', raw))


def serialize_row(a):
    return '<combat_action_attack ' + ' '.join(k + '=' + quoteattr(v) for k, v in a.items()) + ' />'


def fragment_blocks(text):
    """(start, end, guid, tags, ftags) of every <Fragment ...>...</Fragment> in the family."""
    fam_open = text.index('<' + FAMILY + '>')
    fam_close = text.index('</' + FAMILY + '>')
    out = []
    for m in re.finditer(r'<Fragment [^>]*GUID="([^"]+)"[^>]*>', text[fam_open:fam_close]):
        start = fam_open + m.start()
        end = text.index('</Fragment>', start) + len('</Fragment>')
        head = m.group(0)
        tags = re.search(r' Tags="([^"]*)"', head)
        ftags = re.search(r' FragTags="([^"]*)"', head)
        out.append((start, end, m.group(1), tags.group(1) if tags else '', ftags.group(1) if ftags else ''))
    return out, fam_close


def generate(root, check_only=False):
    base = root / 'companion/source'
    raw = (base / ATTACK).read_bytes()
    attack_text = (raw[3:] if raw.startswith(b'\xef\xbb\xbf') else raw).decode('utf-8')  # keep mixed line endings byte-exact
    adb_text = (base / GENERATED).read_bytes().decode('ascii')
    config_text = (base / CONFIG).read_text(encoding='utf-8-sig')
    rows = [attrs(r) for r in re.findall(r'<combat_action_attack ([^>]*)/>', attack_text)]
    existing = {r['mn_fragment_guid'] for r in rows}
    donors = [r for r in rows if r['action_type_id'] == '3' and r['attack_type_id'] == '4' and r['input_class_id'] == '1'
              and r['attack_zone_id'] == '5' and tuple(r[k] for k in EQUIPMENT) in MELEE]
    assert len(donors) == 12, len(donors)
    frags, fam_close = fragment_blocks(adb_text)
    by_guid = {f[2]: f for f in frags}
    counts = {}
    for f in frags:
        counts[(f[3], f[4])] = counts.get((f[3], f[4]), 0) + 1
    new_rows, new_frags = [], []
    for d in donors:
        guid = str(uuid.uuid5(NAMESPACE, d['mn_fragment_guid'] + SUFFIX))
        if guid in existing:
            continue
        start, end, _, tags, ftags = by_guid[d['mn_fragment_guid']]
        assert 'attack_heavy' in ftags.split('+'), ftags
        nftags = '+'.join('attack_special' if t == 'attack_heavy' else t for t in ftags.split('+'))
        block = adb_text[start:end]
        head_end = block.index('>') + 1
        head = block[:head_end].replace('GUID="' + d['mn_fragment_guid'] + '"', 'GUID="' + guid + '"', 1)
        head = head.replace('FragTags="' + ftags + '"', 'FragTags="' + nftags + '"', 1)
        new_frags.append(head + block[head_end:])
        option = counts.get((tags, nftags), 0)
        counts[(tags, nftags)] = option + 1
        row = dict(d)
        row.update(input_class_id='2', mn_fragment_guid=guid, mn_option_index=str(option),
                   mn_tags='+'.join(x for x in (tags, nftags) if x))
        new_rows.append(row)
    # config: (2, zone 5, rclass, lclass) must exist for every class these rows serve
    cfg = {(a['combat_input_class_id'], a['combat_zone_id'], a['r_weapon_class_id'], a['l_weapon_class_id'])
           for a in (attrs(r) for r in re.findall(r'<combat_attack_config ([^>]*)/>', config_text))}
    missing = []
    for d in donors:
        rc, lc, rg, _ = (d[k] for k in EQUIPMENT)
        for c in ([rc] if rg == '-1' else GROUPS[rg]):
            if ('2', '5', c, lc) not in cfg:
                missing.append((c, lc))
    assert not missing, ('missing class-2 zone-5 config', missing)
    if check_only or not new_rows:
        print('R94 combat kicks: %d rows to add, %d present' % (len(new_rows), 12 - len(new_rows)))
        return len(new_rows)
    nl = '\n'
    needle = '</combat_action_attacks>'
    assert attack_text.count(needle) == 1
    attack_text = attack_text.replace(needle, nl.join('\t\t' + serialize_row(r) for r in new_rows) + nl + '\t' + needle)
    anl = '\r\n'
    insert = anl.join('\t\t\t' + f for f in new_frags) + anl + '\t\t'
    adb_text = adb_text[:fam_close] + insert + adb_text[fam_close:]
    ET.fromstring(attack_text.encode('utf-8'))
    ET.fromstring(adb_text.encode('ascii'))
    raw_attack = (base / ATTACK).read_bytes()
    bom = raw_attack.startswith(b'\xef\xbb\xbf')
    (base / ATTACK).write_bytes((b'\xef\xbb\xbf' if bom else b'') + attack_text.encode('utf-8'))
    (base / GENERATED).write_bytes(adb_text.encode('ascii'))
    print('R94 combat kicks: added %d rows / %d fragments' % (len(new_rows), len(new_frags)))
    return len(new_rows)


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1])
    p.add_argument('--check', action='store_true')
    a = p.parse_args()
    generate(a.source, a.check)
