"""Exercise the actual material-wrapper generator, including table inheritance."""
import importlib.util
from pathlib import Path
import unittest
import xml.etree.ElementTree as ET
import json
import zipfile

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('material_wrappers', ROOT/'tools/gore_material_wrappers_d54.py')
m = importlib.util.module_from_spec(spec)
spec.loader.exec_module(m)

TABLE = b'''<database><CharacterComponents>
<Beard Name="Beard" FilePath="humans/male/beard/">
 <Elements><SkinElement EquipmentPart="beard" Model="../uberlod/uberlod_beard.skin" Material="beard_base/m_beard_default.mtl"/></Elements>
 <DerivedComponents>
  <Beard Name="m_beard_08"><Elements><SkinElement EquipmentPart="beard" Model="beards/m_beard_08.skin"/></Elements></Beard>
  <Beard Name="m_beard_06"><Elements><SkinElement EquipmentPart="beard" Model="beards/m_beard_06.skin" Material="beard_base/m_beard_small.mtl"/></Elements></Beard>
  <Beard Name="UniqueNPC"><Elements><SkinElement EquipmentPart="beard" Model="beards/m_beard_06.skin"/></Elements></Beard>
 </DerivedComponents>
</Beard>
<Hair Name="Hair" FilePath="humans/male/hair/">
 <Elements><SkinElement EquipmentPart="hair" Model="dummy" Material="hair_base/m_hair_05.mtl"/></Elements>
 <DerivedComponents>
  <Hair Name="m_hair_05"><Elements><SkinElement EquipmentPart="hair" Model="m_hair_05/m_hair_05.skin"/></Elements></Hair>
  <Hair Name="Uher"><Elements><SkinElement EquipmentPart="hair" Model="m_hair_05/m_hair_05.skin" Material="hair_base/m_hair_uher.mtl"/></Elements></Hair>
 </DerivedComponents>
</Hair></CharacterComponents></database>'''


def cdf(skin, material=''):
    root = ET.Element('CharacterDefinition')
    ET.SubElement(root, 'Model', File='objects/characters/humans/male/skeleton/male.chr', Material='Objects/characters/skeleton_nodraw')
    att = ET.SubElement(ET.SubElement(root, 'AttachmentList'), 'Attachment', Type='CA_SKIN', AName='part', Binding=skin, Flags='32')
    if material:
        att.set('Material', material)
    return ET.tostring(root)


def base_members():
    skins = ['objects/characters/humans/male/head/m_head_034/beards/m_beard_08.skin',
             'objects/characters/humans/male/head/m_head_024/beards/m_beard_06.skin',
             'objects/characters/humans/male/hair/m_hair_05/m_hair_05.skin']
    members = {s[:-5]+'.fly.cdf': cdf(s) for s in skins}
    for basename in ('m_beard_default', 'm_beard_small', 'm_hair_05'):
        for tint in range(4):
            members[f'objects/characters/humans/kcd2vr_tint/{basename}_t{tint}.mtl'] = b'<Material/>'
    return skins, members


MATERIALS = {'objects/characters/humans/male/beard/beard_base/m_beard_default.mtl',
             'objects/characters/humans/male/beard/beard_base/m_beard_small.mtl',
             'objects/characters/humans/male/hair/hair_base/m_hair_05.mtl',
             'objects/characters/humans/male/hair/hair_base/m_hair_uher.mtl'}


class WrapperTests(unittest.TestCase):
    def test_inherited_beard08_material(self):
        candidates, rows = m.inherited_hair_materials(TABLE)
        self.assertEqual(candidates[('male', 'm_beard_08.skin')], {next(x for x in MATERIALS if 'default' in x)})
        self.assertTrue(next(x for x in rows if x['component'] == 'm_beard_08')['inherited'])

    def test_generic_component_wins_fallback_but_unique_variant_retained(self):
        skins, members = base_members()
        added, maps, report = m.build_assets(members, TABLE, MATERIALS)
        self.assertTrue(maps['PartMaterialD54'][skins[1]].endswith('m_beard_small.mtl'))
        self.assertTrue(maps['PartMaterialD54'][skins[2]].endswith('m_hair_05.mtl'))
        self.assertIn(next(x for x in MATERIALS if 'uher' in x), maps['MaterialWrapperD54'][skins[2]])
        self.assertEqual(report['unresolved'], [])

    def test_full_path_keys_prevent_beard_head_collision(self):
        skins, members = base_members()
        other = skins[0].replace('034', '099')
        members[other[:-5]+'.fly.cdf'] = cdf(other)
        _, maps, _ = m.build_assets(members, TABLE, MATERIALS)
        self.assertNotEqual(maps['MaterialWrapperD54'][skins[0]], maps['MaterialWrapperD54'][other])

    def test_tint_bound_to_skin_not_skeleton(self):
        skins, members = base_members()
        added, maps, _ = m.build_assets(members, TABLE, MATERIALS)
        tint = maps['TintWrapperD54'][skins[0][:-5]+'.fly.cdf'][2]
        root = ET.fromstring(added[tint])
        self.assertEqual(root.find('Model').get('Material'), 'Objects/characters/skeleton_nodraw')
        att = root.find('./AttachmentList/Attachment')
        self.assertEqual(att.get('Binding'), skins[0])
        self.assertEqual(att.get('Flags'), '32')
        self.assertTrue(att.get('Material').endswith('m_beard_default_t2.mtl'))

    def test_legacy_members_unchanged_no_geometry_generated(self):
        _, members = base_members()
        before = dict(members)
        added, _, _ = m.build_assets(members, TABLE, MATERIALS)
        self.assertEqual(before, members)
        self.assertTrue(all(n.endswith('.cdf') for n in added))
        self.assertFalse(set(added) & set(members))

    def test_no_missing_material_guess(self):
        skins, members = base_members()
        _, maps, report = m.build_assets(members, TABLE, set())
        self.assertNotIn(skins[0], maps['PartMaterialD54'])
        self.assertTrue(all(x['reason'] == 'native-material-absent' for x in report['unresolved']))

    def test_missing_tint_has_no_partial_palette(self):
        skins, members = base_members()
        del members['objects/characters/humans/kcd2vr_tint/m_beard_default_t3.mtl']
        added, maps, _ = m.build_assets(members, TABLE, MATERIALS)
        self.assertNotIn(skins[0][:-5]+'.fly.cdf', maps['TintWrapperD54'])
        self.assertFalse(any('m_beard_08.fly_d54_t' in n for n in added))

    def test_explicit_authored_material_is_preserved(self):
        skins, members = base_members()
        exact = next(x for x in MATERIALS if 'uher' in x)
        members[skins[2][:-5]+'.fly.cdf'] = cdf(skins[2], exact)
        _, maps, _ = m.build_assets(members, TABLE, MATERIALS)
        self.assertEqual(maps['PartMaterialD54'][skins[2]], exact)
        self.assertEqual(maps['MaterialWrapperD54'][skins[2]][exact], skins[2][:-5]+'.fly.cdf')

    def test_ambiguous_unnamed_default_refused(self):
        skins, members = base_members()
        table = TABLE.replace(b'Name="m_hair_05"', b'Name="OtherNPC"').replace(b'hair_base/m_hair_05.mtl', b'hair_base/m_hair_other.mtl')
        materials = MATERIALS | {'objects/characters/humans/male/hair/hair_base/m_hair_other.mtl'}
        _, maps, report = m.build_assets(members, table, materials)
        self.assertNotIn(skins[2], maps['PartMaterialD54'])
        self.assertEqual(len(maps['MaterialWrapperD54'][skins[2]]), 2)
        self.assertEqual(report['unresolved'][0]['reason'], 'ambiguous-or-no-table-material')

    def test_hair_x_component_keeps_matching_asset_material(self):
        skins, members = base_members()
        table = TABLE.replace(b'Name="m_hair_05"', b'Name="x_hair_05"')
        _, maps, _ = m.build_assets(members, table, MATERIALS)
        self.assertTrue(maps['PartMaterialD54'][skins[2]].endswith('m_hair_05.mtl'))

    def test_exact_default_wrapper_has_matching_tint_alias(self):
        skins, members = base_members()
        _, maps, _ = m.build_assets(members, TABLE, MATERIALS)
        material = maps['PartMaterialD54'][skins[0]]
        exact = maps['MaterialWrapperD54'][skins[0]][material]
        self.assertEqual(maps['TintWrapperD54'][exact], maps['TintWrapperD54'][skins[0][:-5]+'.fly.cdf'])

    def test_unshipped_alternative_tint_is_not_invented(self):
        skins, members = base_members()
        _, maps, _ = m.build_assets(members, TABLE, MATERIALS)
        material = next(x for x in MATERIALS if 'uher' in x)
        exact = maps['MaterialWrapperD54'][skins[2]][material]
        self.assertNotIn(exact, maps['TintWrapperD54'])

    def test_named_helmet_variant_kept_and_lookup_added(self):
        skin = 'objects/characters/humans/male/clothing/head/cap/cap27/cap27.skin'
        mat = skin[:-5]+'_m01.mtl'
        name = skin[:-5]+'.fly_m01.cdf'
        added, maps, _ = m.build_assets({name:cdf(skin, mat)}, TABLE, MATERIALS)
        self.assertEqual(added, {})
        self.assertEqual(maps['MaterialWrapperD54'][skin][mat], name)

    def test_multiple_skin_character_not_rewritten(self):
        skins, members = base_members()
        name = skins[0][:-5]+'.fly.cdf'
        root = ET.fromstring(members[name])
        ET.SubElement(root.find('AttachmentList'), 'Attachment', Type='CA_SKIN', Binding='other.skin')
        members[name] = ET.tostring(root)
        _, maps, _ = m.build_assets(members, TABLE, MATERIALS)
        self.assertNotIn(skins[0], maps['PartMaterialD54'])

    def test_lua_maps_merge_existing_wrapper_map(self):
        text = m.lua_maps({'PartMaterialD54':{'a':'b'}, 'WrapperMtlD52':{'c':'d'}}).decode()
        self.assertIn('WrapperMtlD52=KCD2VR_Gore.WrapperMtlD52 or {}', text)
        self.assertIn('KCD2VR_Gore.WrapperMtlD52[k]=v', text)

    def test_shipped_overlay_material_ownership_and_hashes(self):
        folder = ROOT/'mods/KCD2VRGore/d54'
        maps = json.loads((folder/'GORE_MATERIAL_MAPS_D54.json').read_text())
        manifest = json.loads((folder/'GORE_MATERIAL_WRAPPERS_D54.json').read_text())
        archive = folder/'kcd2vr_gore_d54_material_wrappers.pak'
        self.assertEqual(m.sha(archive.read_bytes()), manifest['archive_sha256'])
        with zipfile.ZipFile(archive) as z:
            self.assertIsNone(z.testzip())
            self.assertEqual(set(z.namelist()), {x['path'] for x in manifest['members']})
            for item in manifest['members']:
                data = z.read(item['path'])
                self.assertEqual(m.sha(data), item['sha256'])
                if item['path'].endswith('.cdf'):
                    _, att = m.only_skin_wrapper(data)
                    self.assertEqual(att.get('Material'), maps['WrapperMtlD52'][item['path']])
                    self.assertEqual(maps['MaterialWrapperD54'][att.get('Binding')][att.get('Material')], item['path'])
            for ordinary, variants in maps['TintWrapperD54'].items():
                self.assertEqual(len(variants), 4)
                for tint, variant in enumerate(variants):
                    self.assertIn(variant, z.namelist())
                    self.assertTrue(maps['WrapperMtlD52'][variant].endswith(f'_t{tint}.mtl'))
        for skin, tail in [
            ('objects/characters/humans/male/head/m_head_034/beards/m_beard_08.skin', 'm_beard_default.mtl'),
            ('objects/characters/humans/male/head/m_head_024/beards/m_beard_06.skin', 'm_beard_small.mtl'),
            ('objects/characters/humans/male/hair/m_hair_05/m_hair_05.skin', 'm_hair_05.mtl')]:
            self.assertTrue(maps['PartMaterialD54'][skin].endswith(tail))


if __name__ == '__main__':
    unittest.main()
