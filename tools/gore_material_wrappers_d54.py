"""Bind gore part materials to CA_SKIN, including inherited retail beard defaults.

This generates only XML CDFs and Lua lookup tables. No meshes, skeletons,
materials, textures, combat rules or installed files are changed. The input
main PAK is the exact D52 optional gore main PAK (or its complete delivery ZIP).
"""
from __future__ import annotations

import argparse
import copy
import hashlib
import io
import json
from pathlib import Path
import posixpath
import re
import zipfile
import xml.etree.ElementTree as ET

D52_PAK_SHA = '2dbcbe96dbc09a9df9124cde2720656e07c68d105f1a466c9ac0cf1a51a020cb'
TABLE_MEMBER = 'Libs/Tables/Character/CharacterComponent.xml'
MAP_MEMBER = 'Scripts/Startup/zzkcd2vr_gore_material_wrappers_d54.lua'


def norm(path):
    return path.replace('\\', '/').lower()


def sha(data):
    return hashlib.sha256(data).hexdigest()


def material_path(base, path):
    path = norm(path)
    if path.startswith('objects/'):
        return path
    return posixpath.normpath(posixpath.join(base, path))


def inherited_hair_materials(table):
    """Read EquipmentPart inheritance, not a flat scan of explicit Material.

    CharacterComponent's derived Beard/ Hair entries replace Model while
    retaining the ancestor EquipmentPart's Material when they omit it.
    Material candidates are retained as sets: ambiguous skins are not guessed.
    """
    result = {}
    rows = []

    def walk(node, base='', inherited=None, kind=''):
        if inherited is None:
            inherited = {}
        current = dict(inherited)
        if node.get('FilePath'):
            base = material_path('objects/characters/', node.get('FilePath'))
        if node.tag in ('Hair', 'Beard'):
            kind = node.tag
        for part in node.findall('./Elements/SkinElement'):
            equipment = part.get('EquipmentPart', '')
            old_material = current.get(equipment)
            explicit = part.get('Material')
            material = material_path(base, explicit) if explicit else old_material
            if material:
                current[equipment] = material
            model = norm(part.get('Model', ''))
            if (kind in ('Hair', 'Beard') and '/humans/' in base and
                    model.endswith('.skin') and material):
                gender = 'female' if '/female/' in base else 'male'
                key = (gender, posixpath.basename(model))
                result.setdefault(key, set()).add(material)
                rows.append(dict(component=node.get('Name'), kind=kind,
                                 model=model, material=material,
                                 inherited=not bool(explicit), gender=gender))
        for child in node:
            if child.tag != 'Elements':
                walk(child, base, current, kind)

    walk(ET.fromstring(table))
    return result, rows


def only_skin_wrapper(blob):
    try:
        root = ET.fromstring(blob)
    except ET.ParseError:
        return None
    attachments = root.findall('./AttachmentList/Attachment')
    if len(attachments) != 1 or attachments[0].get('Type') != 'CA_SKIN':
        return None
    if not norm(attachments[0].get('Binding', '')).endswith('.skin'):
        return None
    return root, attachments[0]


def with_material(root, material):
    root = copy.deepcopy(root)
    root.find('./AttachmentList/Attachment').set('Material', material)
    return ET.tostring(root, encoding='utf-8', xml_declaration=True) + b'\n'


def build_assets(members, table, native_materials):
    candidates, inheritance = inherited_hair_materials(table)
    canonical = {}
    for row in inheritance:
        # The generic component named for this skin is the asset's fallback.
        # Special NPC components can reuse its mesh with another material; all
        # such materials get distinct lookup wrappers, never become defaults.
        stem = posixpath.basename(row['model'])[:-5]
        if ((row['component'] and row['component'].lower() == stem) or
                posixpath.basename(row['material']) == stem + '.mtl'):
            key = (row['gender'], posixpath.basename(row['model']))
            canonical.setdefault(key, set()).add(row['material'])
    part_material = {}
    material_wrapper = {}
    tint_wrapper = {}
    wrapper_material = {}
    added = {}
    corrections = []
    unresolved = []
    for original, blob in sorted(members.items()):
        name = norm(original)
        if not name.endswith('.cdf'):
            continue
        parsed = only_skin_wrapper(blob)
        if parsed is None:
            continue
        root, att = parsed
        skin = norm(att.get('Binding'))
        material = norm(att.get('Material', ''))
        if material:
            material_wrapper.setdefault(skin, {}).setdefault(material, name)
            wrapper_material[name] = material
        # Only ordinary head part wrappers receive new defaults/tints. Existing
        # variants and source face wrappers retain their authored materials.
        if not name.endswith('.fly.cdf') or not ('/hair/' in skin or '/beard' in skin):
            continue
        gender = 'female' if '/female/' in skin else 'male'
        key = (gender, posixpath.basename(skin))
        all_choices = candidates.get(key, set())
        choices = canonical.get(key) or all_choices
        selected_default = material or (next(iter(choices)) if len(choices) == 1 else '')
        # Named native material events can select any exact table-supported
        # alternative; a missing/ambiguous default does not erase these choices.
        for alternative in sorted(all_choices):
            if alternative not in native_materials or alternative == selected_default:
                continue
            alternative_name = name[:-4] + '_d54_m' + sha(alternative.encode())[:12] + '.cdf'
            if alternative_name in members or alternative_name in added:
                raise ValueError('alternative wrapper collision: ' + alternative_name)
            added[alternative_name] = with_material(root, alternative)
            material_wrapper.setdefault(skin, {})[alternative] = alternative_name
            wrapper_material[alternative_name] = alternative
        if not material:
            if len(choices) != 1:
                unresolved.append(dict(wrapper=name, reason='ambiguous-or-no-table-material',
                                       candidates=sorted(choices)))
                continue
            material = next(iter(choices))
            if material not in native_materials:
                unresolved.append(dict(wrapper=name, reason='native-material-absent', material=material))
                continue
            # New path avoids changing the ordinary legacy wrapper semantics.
            bound_name = name[:-4] + '_d54.cdf'
            if bound_name in members or bound_name in added:
                raise ValueError('material wrapper collision: ' + bound_name)
            added[bound_name] = with_material(root, material)
            material_wrapper.setdefault(skin, {})[material] = bound_name
            wrapper_material[bound_name] = material
            corrections.append(dict(source=name, wrapper=bound_name, skin=skin,
                                    material=material, source_sha256=sha(blob),
                                    output_sha256=sha(added[bound_name])))
        part_material[skin] = material
        basename = posixpath.basename(material).removesuffix('.mtl')
        tint_names = []
        for tint in range(4):
            tint_material = f'objects/characters/humans/kcd2vr_tint/{basename}_t{tint}.mtl'
            # Never invent a material file, colour or texture. D52 already has
            # these four explicit fallback tints; only binding ownership changes.
            if tint_material not in members:
                tint_names = []
                break
            tint_name = name[:-4] + f'_d54_t{tint}.cdf'
            if tint_name in members or tint_name in added:
                raise ValueError('tint wrapper collision: ' + tint_name)
            tint_names.append((tint_name, tint_material))
        if tint_names:
            tint_wrapper[name] = []
            for tint_name, tint_material in tint_names:
                added[tint_name] = with_material(root, tint_material)
                tint_wrapper[name].append(tint_name)
                wrapper_material[tint_name] = tint_material
                material_wrapper.setdefault(skin, {})[tint_material] = tint_name
    # A native replacement can retain the untinted base's filename while its
    # actual shader resources carry per-NPC colour. Until the synchronous native
    # pointer copy succeeds, Lua needs the matching coloured fallback even when
    # it selected an exact-material wrapper. Alias existing sets, or add a set
    # only when this material's four tint files already exist in the D52 pack.
    for skin, variants in list(material_wrapper.items()):
        if not ('/hair/' in skin or '/beard' in skin):
            continue
        for material, ordinary in list(variants.items()):
            if '/kcd2vr_tint/' in material or ordinary in tint_wrapper:
                continue
            basename = posixpath.basename(material).removesuffix('.mtl')
            tint_materials = [f'objects/characters/humans/kcd2vr_tint/{basename}_t{n}.mtl' for n in range(4)]
            if not all(n in members for n in tint_materials):
                continue
            if all(n in variants for n in tint_materials):
                tint_wrapper[ordinary] = [variants[n] for n in tint_materials]
                continue
            blob = added.get(ordinary) or members.get(ordinary)
            parsed = only_skin_wrapper(blob) if blob else None
            if parsed is None:
                raise ValueError('missing material wrapper for tint: ' + ordinary)
            root, _ = parsed
            tint_wrapper[ordinary] = []
            for tint, tint_material in enumerate(tint_materials):
                tint_name = ordinary[:-4] + f'_d54_t{tint}.cdf'
                if tint_name in members or tint_name in added:
                    raise ValueError('alternative tint wrapper collision: ' + tint_name)
                added[tint_name] = with_material(root, tint_material)
                tint_wrapper[ordinary].append(tint_name)
                variants[tint_material] = tint_name
                wrapper_material[tint_name] = tint_material
    return added, dict(PartMaterialD54=part_material, MaterialWrapperD54=material_wrapper,
                      TintWrapperD54=tint_wrapper, WrapperMtlD52=wrapper_material), dict(
        corrections=corrections, unresolved=unresolved, inheritance=inheritance)


def lua_literal(value):
    if isinstance(value, str):
        return json.dumps(value, ensure_ascii=True)
    if isinstance(value, list):
        return '{' + ','.join(lua_literal(v) for v in value) + '}'
    return '{\n' + ''.join(' [' + lua_literal(k) + ']=' + lua_literal(v) + ',\n'
                           for k, v in sorted(value.items())) + '}'


def lua_maps(maps):
    text = ('-- D54: exact CA_SKIN material owner; fallback tints are not per-NPC colour proof.\n'
            'KCD2VR_Gore=KCD2VR_Gore or {}\n')
    for key, value in maps.items():
        if key == 'WrapperMtlD52':
            # Startup traversal order is not part of the contract. The older
            # D52 table may load later and assign its table; retain this
            # independent D54 table so new wrappers cannot lose their binding.
            text += 'KCD2VR_Gore.WrapperMtlD54=' + lua_literal(value) + '\n'
            text += 'KCD2VR_Gore.WrapperMtlD52=KCD2VR_Gore.WrapperMtlD52 or {}\n'
            text += 'for k,v in pairs(KCD2VR_Gore.WrapperMtlD54) do KCD2VR_Gore.WrapperMtlD52[k]=v end\n'
        else:
            text += 'KCD2VR_Gore.' + key + '=' + lua_literal(value) + '\n'
    return text.encode('ascii')


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--seed', type=Path, required=True)
    ap.add_argument('--tables', type=Path, required=True)
    ap.add_argument('--native-pak', type=Path, action='append', required=True)
    ap.add_argument('--out', type=Path, required=True)
    args = ap.parse_args()
    with zipfile.ZipFile(args.seed) as source:
        nested = 'Mods/kcd2vr_gore/Data/kcd2vr_gore.pak'
        pak_data = source.read(nested) if nested in source.namelist() else args.seed.read_bytes()
    if sha(pak_data) != D52_PAK_SHA:
        raise SystemExit('Unreviewed gore main PAK input')
    with zipfile.ZipFile(io.BytesIO(pak_data)) as source:
        members = {norm(n): source.read(n) for n in source.namelist()}
    with zipfile.ZipFile(args.tables) as z:
        table = z.read(TABLE_MEMBER)
    native_materials = set()
    for path in args.native_pak:
        with zipfile.ZipFile(path) as z:
            native_materials.update(norm(n) for n in z.namelist() if n.lower().endswith('.mtl'))
    added, maps, report = build_assets(members, table, native_materials)
    added[MAP_MEMBER] = lua_maps(maps)
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    artifact = out/'kcd2vr_gore_d54_material_wrappers.pak'
    if artifact.exists():
        raise SystemExit('Choose a fresh output directory; no overwrite')
    with zipfile.ZipFile(artifact, 'w', zipfile.ZIP_DEFLATED, compresslevel=6) as target:
        for name, data in sorted(added.items()):
            target.writestr(name, data)
    with zipfile.ZipFile(artifact) as target:
        if target.testzip():
            raise SystemExit('Generated archive CRC failure')
        for name, data in added.items():
            if target.read(name) != data:
                raise SystemExit('Generated archive readback mismatch')
    report.update(input_main_pak_sha256=sha(pak_data), table_member=TABLE_MEMBER,
                  table_sha256=sha(table), native_archives=[str(p) for p in args.native_pak],
                  generated_members=len(added), generated_bytes=sum(map(len, added.values())),
                  archive_bytes=artifact.stat().st_size, archive_sha256=sha(artifact.read_bytes()),
                  material_bound_skins=len(maps['PartMaterialD54']),
                  tint_bound_wrappers=len(maps['TintWrapperD54']),
                  members=[dict(path=n, sha256=sha(d), bytes=len(d), replaces=False)
                           for n, d in sorted(added.items())],
                  generator_sha256=sha(Path(__file__).read_bytes()),
                  limitations=['Native per-NPC runtime tint is not reproduced by fallback palette.',
                               'No mesh, bone, texture, physics or source face changes.',
                               'Asset/host proof only; native render appearance needs headset validation.'])
    (out/'GORE_MATERIAL_WRAPPERS_D54.json').write_text(json.dumps(report, indent=2)+'\n')
    (out/'GORE_MATERIAL_MAPS_D54.json').write_text(json.dumps(maps, indent=2)+'\n')
    print(json.dumps({k:report[k] for k in ('generated_members','generated_bytes','archive_bytes',
        'archive_sha256','material_bound_skins','tint_bound_wrappers')}, indent=2))


if __name__ == '__main__':
    main()
