#!/usr/bin/env python3
"""V66.178R59 companion table/ADB generator - authored from the player's own vanilla KCD2 files.

Learned from (not copied from) "Master Strike With All Weapons" (nexus 3611): that mod clones the retail
master-strike rows of the sword groups to the blunt group (and longsword -> halberd), clones the referenced
Mannequin fragments under new GUIDs with the weapon tags substituted, and clones the opponent's synchronised
master-strike-hit rows the same way. We do exactly that recipe here, with our own GUIDs (uuid5 of the source
GUID), from vanilla, as `<table>__kcdvr.xml` ROW PATCHES (KCD2 merges them by key; no whole table is
replaced), except the ADB which has no patch mechanism and is shipped whole.

The armed kick is our own: the retail unarmed kick is the attack_heavy row at attack zone 5 (attack type 4)
and `attack_special` resolves to zone 5 only for the unarmed class (combat_attack_config); every armed class
uses zone 5 for the low stab. Zones 3/4 (lower_left/right) exist in the tables and the fragment tag set but
no retail row uses them, so the armed kick lives at attack zone 3: one row per retail equipment profile,
the unarmed kick fragment cloned with that profile's weapon tags and aZ5 -> aZ3. The VR mod announces zone
3 + attack_heavy on R3 (physical_weapon.h Kick59); the AI prices it with the added zone-distance rows.

Usage: gen_companion59.py --tables <Libs/Tables dir> --adb <kcd_male_combat_generated.adb> --out <companion/source>
"""
import argparse, os, re, sys, uuid

NS = uuid.UUID('6b1c2d2e-5f41-4a6d-9c8e-2d5e7a9f1c59')  # KCD2VR companion namespace

def rows(text, tag):
    return [(m.group(0), dict(re.findall(r'(\w+)="([^"]*)"', m.group(1)))) for m in re.finditer(r'<%s ([^>]*)/>' % tag, text)]

def attrs_to_xml(tag, a):
    return '<%s %s />' % (tag, ' '.join('%s="%s"' % (k, v) for k, v in a.items()))

def new_guid(src, purpose):
    return str(uuid.uuid5(NS, 'kcd2vr:%s:%s' % (src, purpose)))

def subst_tags(tags, mapping):
    out = []
    for t in tags.split('+'):
        if not t: continue
        out.append(mapping.get(t, t))
    return '+'.join(out)

def patch_file(root_open, root_close, table_open, table_close, body):
    return ('<?xml version="1.0" encoding="us-ascii"?>\n%s\n\t%s\n%s\t%s\n%s\n' % (root_open, table_open, body, table_close, root_close))

def heads(text):
    # database root element and the plural table element with its version, taken from the vanilla file
    m = re.search(r'<database[^>]*>', text); root = m.group(0)
    t = re.search(r'<(\w+) version="(\d+)">', text)
    return root, '</database>', '<%s version="%s">' % (t.group(1), t.group(2)), '</%s>' % t.group(1)

def main():
    ap = argparse.ArgumentParser(); ap.add_argument('--tables', required=True); ap.add_argument('--adb', required=True); ap.add_argument('--out', required=True)
    a = ap.parse_args()
    T = lambda *p: os.path.join(a.tables, *p)
    adb = open(a.adb, encoding='utf-8', errors='strict', newline='').read()
    out_tables = os.path.join(a.out, 'Libs', 'Tables')
    report = []

    # ---------------------------------------------------------------- master strikes (perfect_block, action 55)
    pb_text = open(T('combat', 'combat_action_perfect_block.xml'), encoding='utf-8', newline='').read()
    pb = rows(pb_text, 'combat_action_perfect_block')
    blunt_map = {'r_swords': 'r_bluntWeapon', 'r_shortSwords': 'r_bluntWeapon'}
    halberd_map = {'l_longsword': 'l_halberd', 'r_longsword': 'r_halberd'}
    ms_rows, frag_clones = [], []  # (section, src_guid, new_guid, mapping)
    for raw, r in pb:
        if r['action_type_id'] != '55': continue
        if r['r_weapon_group_id'] in ('9', '10'):
            n = dict(r); n['r_weapon_group_id'] = '8'; n['mn_tags'] = subst_tags(r['mn_tags'], blunt_map)
            n['mn_fragment_guid'] = new_guid(r['mn_fragment_guid'], 'blunt-ms'); ms_rows.append(n)
            frag_clones.append((r['mn_fragment_id'], r['mn_fragment_guid'], n['mn_fragment_guid'], blunt_map))
        elif r['r_weapon_class_id'] == '4':
            n = dict(r); n['r_weapon_class_id'] = '7'
            if n.get('l_weapon_class_id') == '4': n['l_weapon_class_id'] = '7'
            n['mn_tags'] = subst_tags(r['mn_tags'], halberd_map)
            n['mn_fragment_guid'] = new_guid(r['mn_fragment_guid'], 'halberd-ms'); ms_rows.append(n)
            frag_clones.append((r['mn_fragment_id'], r['mn_fragment_guid'], n['mn_fragment_guid'], halberd_map))
    h = heads(pb_text)
    os.makedirs(os.path.join(out_tables, 'combat'), exist_ok=True)
    open(os.path.join(out_tables, 'combat', 'combat_action_perfect_block__kcdvr.xml'), 'w', encoding='utf-8', newline='\n').write(
        patch_file(h[0], h[1], h[2], h[3], ''.join('\t\t%s\n' % attrs_to_xml('combat_action_perfect_block', n) for n in ms_rows)))
    report.append('master strike rows: %d (blunt group 8 from groups 9/10, halberd from longsword)' % len(ms_rows))

    # ---------------------------------------------------------------- victim side (sync_perfect_block_hit, action 56)
    sh_text = open(T('combat', 'combat_action_sync_perfect_block_hit.xml'), encoding='utf-8', newline='').read()
    sh = rows(sh_text, 'combat_action_sync_perfect_block_hit')
    oblunt_map = {'or_shortSwords': 'or_bluntWeapon', 'or_swords': 'or_bluntWeapon'}
    ohalberd_map = {'ol_longsword': 'ol_halberd', 'or_longsword': 'or_halberd'}
    hit_rows = []
    for raw, r in sh:
        if r['action_type_id'] != '56': continue
        if r['opp_r_weapon_group_id'] in ('9', '10'):
            n = dict(r); n['opp_r_weapon_group_id'] = '8'; n['mn_tags'] = subst_tags(r['mn_tags'], oblunt_map)
            n['mn_fragment_guid'] = new_guid(r['mn_fragment_guid'], 'blunt-hit'); hit_rows.append(n)
            frag_clones.append((r['mn_fragment_id'], r['mn_fragment_guid'], n['mn_fragment_guid'], oblunt_map))
        elif r['opp_r_weapon_class_id'] == '4':
            n = dict(r); n['opp_r_weapon_class_id'] = '7'
            if n.get('opp_l_weapon_class_id') == '4': n['opp_l_weapon_class_id'] = '7'
            n['mn_tags'] = subst_tags(r['mn_tags'], ohalberd_map)
            n['mn_fragment_guid'] = new_guid(r['mn_fragment_guid'], 'halberd-hit'); hit_rows.append(n)
            frag_clones.append((r['mn_fragment_id'], r['mn_fragment_guid'], n['mn_fragment_guid'], ohalberd_map))
    h = heads(sh_text)
    open(os.path.join(out_tables, 'combat', 'combat_action_sync_perfect_block_hit__kcdvr.xml'), 'w', encoding='utf-8', newline='\n').write(
        patch_file(h[0], h[1], h[2], h[3], ''.join('\t\t%s\n' % attrs_to_xml('combat_action_sync_perfect_block_hit', n) for n in hit_rows)))
    report.append('master strike victim rows: %d' % len(hit_rows))

    # ---------------------------------------------------------------- armed kicks (attack rows at zone 3)
    at_text = open(T('combat', 'combat_action_attack.xml'), encoding='utf-8', newline='').read()
    at = rows(at_text, 'combat_action_attack')
    def frag_attrs(guid):
        m = re.search(r'<Fragment ([^>]*GUID="%s"[^>]*)>' % guid, adb)
        return dict(re.findall(r'(\w+)="([^"]*)"', m.group(1))) if m else None
    kicks = {opp: next(r for raw, r in at if r['mn_fragment_guid'] == g) for opp, g in
             (('NPC', '9c22a61c-fb64-3ac1-b0b8-1d325a2cced7'), ('Dog', 'e5c52bdb-ce2e-3f36-8214-f680b55738fb'))}
    profiles = {}
    for raw, r in at:
        if r['action_type_id'] == '3' and r['input_class_id'] == '1' and r['attack_zone_id'] in ('1', '2') and \
           r.get('player') != 'true' and r['r_weapon_class_id'] != '12' and (r['r_weapon_class_id'] != '-1' or r['r_weapon_group_id'] != '-1'):
            f = frag_attrs(r['mn_fragment_guid'])
            if not f: continue
            key = (r['r_weapon_class_id'], r['l_weapon_class_id'], r['r_weapon_group_id'], r['l_weapon_group_id'], f['Tags'],
                   '+'.join(t for t in f['FragTags'].split('+') if t.startswith(('r_', 'l_'))), 'Dog' if r.get('opp_actor_classes') == 'Dog' else 'NPC')
            profiles.setdefault(key, True)
    kick_rows = []
    for (rc, lc, rg, lg, gtags, ftags, opp) in sorted(profiles):
        src = kicks[opp]; sf = frag_attrs(src['mn_fragment_guid'])
        base = [t for t in sf['FragTags'].split('+') if not t.startswith(('r_', 'l_'))]
        base = ['aZ3' if t == 'aZ5' else t for t in base]
        fragtags = '+'.join(([ftags] if ftags else []) + base)
        n = dict(src)  # Retail registers z_spatial_grid as XML-supported string type 5; retain donor data.
        n['attack_zone_id'] = '3'; n['r_weapon_class_id'] = rc; n['l_weapon_class_id'] = lc
        n['r_weapon_group_id'] = rg; n['l_weapon_group_id'] = lg
        n['mn_tags'] = '+'.join(([gtags] if gtags else []) + [fragtags])
        purpose = 'kick-%s-%s-%s-%s-%s' % (rc, lc, rg, lg, opp)
        n['mn_fragment_guid'] = new_guid(src['mn_fragment_guid'], purpose); kick_rows.append(n)
        frag_clones.append((src['mn_fragment_id'], src['mn_fragment_guid'], n['mn_fragment_guid'], {'__Tags__': gtags, '__FragTags__': fragtags}))
    h = heads(at_text)
    open(os.path.join(out_tables, 'combat', 'combat_action_attack__kcdvr.xml'), 'w', encoding='utf-8', newline='\n').write(
        patch_file(h[0], h[1], h[2], h[3], ''.join('\t\t%s\n' % attrs_to_xml('combat_action_attack', n) for n in kick_rows)))
    report.append('armed kick rows (zone 3): %d over %d equipment profiles' % (len(kick_rows), len(profiles)))

    # zone distances: the AI prices zone 3 like the retail low zone (so a low guard blocks the kick as it blocks the retail one)
    zd_text = open(T('combat', 'combat_zone_distance.xml'), encoding='utf-8', newline='').read()
    zd = rows(zd_text, 'combat_zone_distance'); have = {(r['src_combat_zone_id'], r['dst_combat_zone_id']) for raw, r in zd}
    zrows = []
    for raw, r in zd:
        if r['dst_combat_zone_id'] == '5' and r['src_combat_zone_id'] in ('0', '1', '2', '5') and (r['src_combat_zone_id'], '3') not in have:
            n = dict(r); n['dst_combat_zone_id'] = '3'; zrows.append(n)
    for raw, r in zd:
        if r['src_combat_zone_id'] == '5' and r['dst_combat_zone_id'] in ('0', '1', '2') and ('3', r['dst_combat_zone_id']) in have: pass
    if ('3', '3') not in have:
        n = dict(next(r for raw, r in zd if r['src_combat_zone_id'] == '5' and r['dst_combat_zone_id'] == '5')); n['src_combat_zone_id'] = '3'; n['dst_combat_zone_id'] = '3'; zrows.append(n)
    h = heads(zd_text)
    open(os.path.join(out_tables, 'combat', 'combat_zone_distance__kcdvr.xml'), 'w', encoding='utf-8', newline='\n').write(
        patch_file(h[0], h[1], h[2], h[3], ''.join('\t\t%s\n' % attrs_to_xml('combat_zone_distance', n) for n in zrows)))
    report.append('zone-distance rows for zone 3: %d' % len(zrows))

    # attack config: heavy + zone 3 must be a legal pair for every armed class (retail already allows it for the
    # short-weapon group, longsword, halberd and shield+longsword; dagger and flail are added)
    cfg_text = open(T('combat', 'combat_attack_config.xml'), encoding='utf-8', newline='').read()
    cfg = rows(cfg_text, 'combat_attack_config'); maxid = max(int(r['combat_attack_config_id']) for raw, r in cfg)
    crow = []
    for i, cls in enumerate(('0', '6')):
        crow.append({'combat_attack_config_id': str(max(100, maxid + 1) + i), 'combat_input_class_id': '1', 'combat_zone_id': '3', 'l_weapon_class_id': '-1', 'r_weapon_class_id': cls})
    h = heads(cfg_text)
    open(os.path.join(out_tables, 'combat', 'combat_attack_config__kcdvr.xml'), 'w', encoding='utf-8', newline='\n').write(
        patch_file(h[0], h[1], h[2], h[3], ''.join('\t\t%s\n' % attrs_to_xml('combat_attack_config', n) for n in crow)))
    report.append('attack config rows: %d' % len(crow))

    # ---------------------------------------------------------------- ragdoll reaction coefficient (F9 companion)
    hr_text = open(T('action', 'hit_reaction_type.xml'), encoding='utf-8', newline='').read()
    hr = rows(hr_text, 'hit_reaction_type'); rag = dict(next(r for raw, r in hr if r['hit_reaction_type_id'] == '1')); rag['physical_coef'] = '1'
    h = heads(hr_text); os.makedirs(os.path.join(out_tables, 'action'), exist_ok=True)
    open(os.path.join(out_tables, 'action', 'hit_reaction_type__kcdvr.xml'), 'w', encoding='utf-8', newline='\n').write(
        patch_file(h[0], h[1], h[2], h[3], '\t\t%s\n' % attrs_to_xml('hit_reaction_type', rag)))
    report.append('hit_reaction_type Ragdoll row: physical_coef 0 -> 1')

    # ---------------------------------------------------------------- ADB fragment clones
    added = 0; missing = []
    sections = {}
    for section, src, new, mapping in frag_clones:
        m = re.search(r'(<Fragment [^>]*GUID="%s"[^>]*>.*?</Fragment>)' % src, adb, re.S)
        if not m: missing.append(src); continue
        frag = m.group(1)
        head = re.match(r'<Fragment ([^>]*)>', frag); fa = dict(re.findall(r'(\w+)="([^"]*)"', head.group(1)))
        fa['GUID'] = new
        if '__Tags__' in mapping:
            fa['Tags'] = mapping['__Tags__']; fa['FragTags'] = mapping['__FragTags__']
        else:
            fa['Tags'] = subst_tags(fa.get('Tags', ''), mapping); fa['FragTags'] = subst_tags(fa.get('FragTags', ''), mapping)
        newhead = '<Fragment %s>' % ' '.join('%s="%s"' % (k, v) for k, v in fa.items())
        sections.setdefault(section, []).append(newhead + frag[head.end():]); added += 1
    for section, frags in sections.items():
        close = '\n\t\t</%s>' % section
        i = adb.find('<%s>' % section); j = adb.find(close, i)
        assert i >= 0 and j > i, section
        adb = adb[:j] + '\n\t\t\t' + '\n\t\t\t'.join(frags) + adb[j:]
    os.makedirs(os.path.join(a.out, 'Animations', 'Mannequin', 'ADB'), exist_ok=True)
    open(os.path.join(a.out, 'Animations', 'Mannequin', 'ADB', 'kcd_male_combat_generated.adb'), 'w', encoding='utf-8', newline='').write(adb)
    report.append('ADB fragments cloned: %d (missing sources: %d)' % (added, len(missing)))
    print('\n'.join(report))
    return 1 if missing else 0

if __name__ == '__main__':
    sys.exit(main())
