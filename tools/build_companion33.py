"""C33/C34 companion pak: the C32C1 pak (C31 + printed-map resources, 26 members) with five members changed,
everything else byte-identical.

1. combat_action_attack.xml  - the 12 armed kick rows move from attack zone 3 (lower_left: no angle, no retail rows,
   the game turned R3 into an ordinary zone-3 slash and played nothing) to zone 5, the zone the retail unarmed kick
   uses (heavy input class, attack type 4 kick, aZ5).
2. kcd_male_combat_generated.adb - the 12 matching armed kick fragments follow (FragTags aZ3 -> aZ5). The animation is
   still the retail unarmed kick clip; no foreign asset.
3. combat_attack_config__kcdvr.xml - the old zone-3 heavy configs (100/101) are gone; new configs map the special
   input (attack_special, class 2) to zone 5 for armed fighters, the same way the retail fist config (id 18) does.
4. hit_reaction_type__kcdvr.xml - physical_coef for weak/minor/major hits and deaths (retail 0 for all of them), so the
   game's own physical hit reactions and the F9 multipliers have something to scale.
5. rpg_param__kcdvr.xml - comment text only (no mod names in shipped files); values unchanged.

Deterministic, fails closed: the input pak must be the pinned C32C1 artifact and every transform must match exactly.
usage: python tools/build_companion33.py companion/reference/C32C1/<pak> <output pak>
"""
import hashlib, re, sys, zipfile
from pathlib import Path

BASE_SHA = '24be84e89b5b37b8b75d63d54ea0b72fc5c9eebe08f322f49a73ecf130e37da4'  # C32C1 main companion (26 members)
ATTACK = 'Libs/Tables/combat/combat_action_attack.xml'
ADB = 'Animations/Mannequin/ADB/kcd_male_combat_generated.adb'
CONFIG = 'Libs/Tables/combat/combat_attack_config__kcdvr.xml'
REACT = 'Libs/Tables/action/hit_reaction_type__kcdvr.xml'
RPG = 'Libs/Tables/rpg/rpg_param__kcdvr.xml'
DATE = (2026, 10, 17, 0, 0, 0)
KICK_ROWS = 12

# (id, input class, zone, left class, right class, right group)
CONFIGS = [(102, 2, 5, 4, 4, None), (103, 2, 5, 7, 7, None), (104, 2, 5, -1, 0, None), (105, 2, 5, -1, 6, None),
           (106, 2, 5, 8, 4, None), (107, 2, 5, -1, -1, 1), (108, 2, 5, 8, -1, 1)]
# hit_reaction_type rows: id, name, death, tag, physical_coef
REACTIONS = [(1, 'Ragdoll', 'false', 'ragdoll', '1'), (2, 'AnimatedWeak', 'false', 'weak_hit', '0.15'),
             (3, 'AnimatedMinor', 'false', 'minor_hit', '0.25'), (4, 'AnimatedMajor', 'false', 'major_hit', '0.5'),
             (5, 'AnimatedDeath', 'true', 'death', '0.8')]
HEADER = ('<?xml version="1.0" encoding="us-ascii"?>\n<database xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" '
          'name="barbora" xsi:noNamespaceSchemaLocation="../database.xsd">\n')


def kick_attack(text):
    rows = re.findall(r'<combat_action_attack [^>]*/>', text)
    hits = [r for r in rows if 'attack_type_id="4"' in r and 'attack_zone_id="3"' in r and '+aZ3+kick+' in r]
    if len(hits) != KICK_ROWS:
        raise ValueError(f'attack table: expected {KICK_ROWS} zone-3 kick rows, found {len(hits)}')
    for r in hits:
        n = r.replace('attack_zone_id="3"', 'attack_zone_id="5"').replace('+aZ3+kick+', '+aZ5+kick+')
        text = text.replace(r, n, 1)
    if '+aZ3+' in text or 'attack_zone_id="3"' in text:
        raise ValueError('attack table: zone 3 left after the transform')
    return text


def kick_adb(text):
    frags = re.findall(r'<Fragment [^>]*FragTags="[^"]*\+aZ3\+kick\+[^"]*"[^>]*>', text)
    if len(frags) != KICK_ROWS:
        raise ValueError(f'adb: expected {KICK_ROWS} aZ3 kick fragments, found {len(frags)}')
    for f in frags:
        text = text.replace(f, f.replace('+aZ3+kick+', '+aZ5+kick+'), 1)
    if '+aZ3+' in text:
        raise ValueError('adb: aZ3 tag left after the transform')
    return text


def config_xml():
    body = ['\t<combat_attack_configs version="1">\n',
            '\t\t<!-- KCD2VR C33: R3 = the special attack input. Armed, it maps to zone 5 like the retail fist config 18,\n'
            '\t\t     so the kick rows (attack type 4, aZ5) are the ones the game can pick. -->\n']
    for i, inp, zone, left, right, group in CONFIGS:
        g = f' r_combat_weapon_group_id="{group}"' if group is not None else ''
        body.append(f'\t\t<combat_attack_config combat_attack_config_id="{i}" combat_input_class_id="{inp}" combat_zone_id="{zone}" '
                    f'l_weapon_class_id="{left}"{g} r_weapon_class_id="{right}" />\n')
    body.append('\t</combat_attack_configs>\n</database>\n')
    return HEADER + ''.join(body)


def reaction_xml():
    body = ['\t<hit_reaction_types version="1">\n',
            '\t\t<!-- KCD2VR C33: retail physical_coef is 0 for every reaction type, so nothing physical reaches the body.\n'
            '\t\t     These give the game\'s own physical hit reactions (and the F9 multipliers) something to scale. -->\n']
    for i, name, death, tag, coef in REACTIONS:
        body.append(f'\t\t<hit_reaction_type hit_reaction_type_id="{i}" hit_reaction_type_name="{name}" '
                    f'is_death_reaction="{death}" mn_tag="{tag}" physical_coef="{coef}" />\n')
    body.append('\t</hit_reaction_types>\n</database>\n')
    return HEADER.replace('xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" name="barbora"',
                          'name="barbora" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"') + ''.join(body)


def rpg_clean(text):
    new = re.sub(r'<!-- KCD2VR R59\.16C8 .*?-->',
                 '<!-- KCD2VR R59.16C8 (Miki: "better enemy AI - react faster, more aggressive"). Row-patch file; vanilla values\n'
                 '\t\t     in the comments. Block / perfect-block / master-strike probabilities are NOT changed. -->', text, count=1, flags=re.S)
    if new == text:
        raise ValueError('rpg_param: header comment not found')
    if 'Overhaul' in new:
        raise ValueError('rpg_param: mod name left')
    return new


def build(src, out):
    src, out = Path(src), Path(out)
    if src.resolve() == out.resolve():
        raise ValueError('refusing to overwrite the input pak')
    data = src.read_bytes()
    if hashlib.sha256(data).hexdigest() != BASE_SHA:
        raise ValueError('input pak fingerprint is not the pinned C32C1 companion')
    changed = {}
    with zipfile.ZipFile(src) as z:
        names = z.namelist()
        changed[ATTACK] = kick_attack(z.read(ATTACK).decode('ascii')).encode('ascii')
        changed[ADB] = kick_adb(z.read(ADB).decode('ascii')).encode('ascii')
        changed[CONFIG] = config_xml().encode('ascii')
        changed[REACT] = reaction_xml().encode('ascii')
        changed[RPG] = rpg_clean(z.read(RPG).decode('ascii')).encode('ascii')
        tmp = out.with_suffix('.tmp')
        with zipfile.ZipFile(tmp, 'w') as o:
            for n in names:
                info = zipfile.ZipInfo(n, DATE)
                info.compress_type = zipfile.ZIP_DEFLATED
                info.external_attr = 0o644 << 16
                o.writestr(info, changed.get(n, z.read(n)))
    tmp.replace(out)
    h = hashlib.sha256(out.read_bytes()).hexdigest()
    return {'pak_sha256': h, 'members': len(names), 'changed': sorted(changed)}


if __name__ == '__main__':
    print(build(sys.argv[1], sys.argv[2]))
