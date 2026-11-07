"""Actual PAK-overlay builder tests. Lua payloads are never executed here."""
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest
import zipfile

REPO = Path(__file__).resolve().parents[3]
spec = importlib.util.spec_from_file_location("companion_patch_t6", REPO / "src/subsystems/throw_drop/resources/companion_patch_t6.py")
builder = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = builder
spec.loader.exec_module(builder)


def zipped(entries):
    stream = io.BytesIO()
    with zipfile.ZipFile(stream, "w", zipfile.ZIP_DEFLATED) as archive:
        for name, data in entries.items():
            archive.writestr(name, data)
    return stream.getvalue()


class CompanionOverlayTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.input = self.root / "input"
        self.input.mkdir()
        self.starter = self.root / "inventory_delivery_t6.lua"
        self.starter.write_bytes(b"-- packaged only, never executed\nKCD2VRThrowingAssets1={NativeReady=function()end}\n")
        self.supply = self.root / "native_supply_t2.lua"
        self.supply.write_bytes(b"-- new readiness labels; never executed\n")
        items = []
        stock = []
        for guid, (name, cls, model, amount) in builder.ITEMS.items():
            items.append(f'<MeleeWeapon Id="{guid}" Name="{name}" Class="{cls}" Model="{model}" UIName="ui_nm_{name}" UIInfo="ui_in_{name}"/>')
            stock.append(f'<PresetItem Name="{name}" Amount="{amount}"/>')
        self.members = {
            builder.STARTUP_MEMBER: b"-- previous exact startup\n",
            builder.SUPPLY_MEMBER: b"-- previous exact supply\n",
            "Scripts/Startup/zzkcd2vr_doors.lua": b"-- protocol1966/revision66 native door owner\n",
            "Scripts/G1/current_crafting.lua": b"-- unrelated latest combined G1 bytes\r\n",
            "Objects/Custom/new_combined_asset.cgf": b"\0latest combined binary asset\xff",
            "Libs/Tables/item/item__kcd2vr_throwing_assets1.xml": ('<database><ItemClasses version="8">' + ''.join(items) + '</ItemClasses></database>').encode(),
            "Libs/Tables/item/InventoryPreset__kcd2vr_throwing_assets1.xml": ('<database><InventoryPresets version="2"><InventoryPreset Name="inventory_kcd2vr_throwing_assets1">' + ''.join(stock) + '</InventoryPreset></InventoryPresets></database>').encode(),
        }
        for table, tag, group in (("perk", "perk", "perks"), ("perk_script", "perk_script", "perk_scripts")):
            self.members[f"Libs/Tables/rpg/{table}__kcd2vr_throwing_assets1.xml"] = ('<database><' + group + '>' + ''.join(f'<{tag} perk_id="{guid}"/>' for guid in builder.RECEIPTS) + '</' + group + '></database>').encode()
        self.files = {
            builder.MANIFEST: b'<kcd_mod><info><name>Latest Combined G1</name><modid>kcdvr</modid><description>Native C66 and unrelated G1.</description><version>66.178.66.G1</version></info></kcd_mod>',
            "README.txt": b"Latest combined instructions, preserved.\n",
            "install_unified_companion63.ps1": b"DO NOT EXECUTE THIS TEST INSTALLER\n",
        }
        labels = '<Table>' + ''.join('<Row><Cell>' + prefix + name + '</Cell><Cell>Visible item text</Cell></Row>' for name, _, _, _ in builder.ITEMS.values() for prefix in ('ui_nm_', 'ui_in_')) + '</Table>'
        languages = ('English','Czech','Chineses','Chineset','French','German','Italian','Japanese','Korean','Polish','Portuguese','Russian','Spanish','Turkish','Ukrainian','Vietnamese')
        for language in languages:
            self.files[builder.MOD + 'Localization/' + language + '_xml.pak'] = zipped({'text_ui__kcd2vr_throwing_assets1.xml': labels.encode(), 'text_ui_g1.xml': b'<Table/>'})
        self.flush()

    def flush(self):
        self.files[builder.PAK] = zipped(self.members)
        self.metadata = {'version': 63, 'candidate': 66, 'door_protocol': 1966, 'door_revision': 66, 'members': len(self.members), 'pak_sha256': builder.sha(self.files[builder.PAK]), 'unrelated_G1_metadata': {'preserve': True}}
        self.metadata['distribution_files'] = {n[len(builder.MOD):]: builder.sha(b) for n,b in self.files.items() if n.startswith(builder.MOD) and n not in (builder.PAK,builder.MARKER)}
        self.files[builder.MARKER] = (json.dumps(self.metadata) + '\n').encode()
        for name,data in self.files.items():
            p=self.input/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)

    def stage(self, name='out', **kwargs):
        return builder.build(self.input, self.root / name, starter_source=self.starter, supply_source=kwargs.pop('supply_source', self.supply), **kwargs)

    def test_only_two_owned_members_change_and_latest_combined_assets_survive(self):
        before = builder.read_tree(self.input)
        receipt = self.stage()
        out = self.root / 'out'
        after = builder.read_pak((out/builder.PAK).read_bytes())
        self.assertEqual(set(after),set(self.members))
        self.assertEqual(receipt['changed_members'],sorted([builder.STARTUP_MEMBER,builder.SUPPLY_MEMBER]))
        for name,raw in self.members.items():
            if name not in receipt['changed_members']:
                self.assertEqual(after[name][1],raw,name)
        for path,raw in before.items():
            if path not in (builder.PAK,builder.MARKER,builder.MANIFEST,'README.txt'):
                self.assertEqual((out/path).read_bytes(),raw,path)
        self.assertEqual(builder.read_tree(self.input),before)
        meta=json.loads((out/builder.MARKER).read_bytes())
        self.assertEqual(meta['unrelated_G1_metadata'],{'preserve':True})
        self.assertEqual((meta['candidate'],meta['door_protocol'],meta['door_revision']),(66,1966,66))
        self.assertEqual(meta['distribution_files']['mod.manifest'],builder.sha((out/builder.MANIFEST).read_bytes()))
        self.assertIn(b'<version>66.178.66.G1</version>',(out/builder.MANIFEST).read_bytes())
        self.assertEqual(receipt['validation']['localization_packages'],16)

    def test_supply_is_optional_and_unchanged_source_is_not_called_a_change(self):
        self.supply.write_bytes(self.members[builder.SUPPLY_MEMBER])
        self.assertEqual(self.stage()['changed_members'],[builder.STARTUP_MEMBER])
        self.assertEqual(self.stage('out_without_supply',supply_source=None)['changed_members'],[builder.STARTUP_MEMBER])

    def test_repeated_t6_staging_changes_no_archive_payload(self):
        self.stage()
        result=builder.build(self.root/'out',self.root/'again',starter_source=self.starter,supply_source=self.supply)
        self.assertEqual(result['changed_members'],[])
        self.assertEqual((self.root/'out'/builder.PAK).read_bytes(),(self.root/'again'/builder.PAK).read_bytes())
        self.assertEqual((self.root/'out/README.txt').read_bytes(),(self.root/'again/README.txt').read_bytes())

    def test_stale_distribution_hash_refused_before_output(self):
        (self.input/builder.MANIFEST).write_bytes(b'<changed/>')
        with self.assertRaisesRegex(builder.PackageError,'distribution inventory'):
            self.stage()
        self.assertFalse((self.root/'out').exists())

    def test_missing_native_guid_refused_before_output(self):
        path='Libs/Tables/item/item__kcd2vr_throwing_assets1.xml'
        self.members[path]=self.members[path].replace(next(iter(builder.ITEMS)).encode(),b'00000000-0000-0000-0000-000000000000')
        self.flush()
        with self.assertRaisesRegex(builder.PackageError,'native throwable'):
            self.stage()
        self.assertFalse((self.root/'out').exists())

    def test_missing_save_owned_receipt_refused_before_output(self):
        path='Libs/Tables/rpg/perk__kcd2vr_throwing_assets1.xml'
        self.members[path]=b'<database><perks/></database>';self.flush()
        with self.assertRaisesRegex(builder.PackageError,'family receipt'):
            self.stage()
        self.assertFalse((self.root/'out').exists())

    def test_fabricated_scale_or_bad_stock_refused(self):
        path='Libs/Tables/item/item__kcd2vr_throwing_assets1.xml'
        self.members[path]=self.members[path].replace(b'<MeleeWeapon ',b'<MeleeWeapon Scale="0.7" ',1);self.flush()
        with self.assertRaisesRegex(builder.PackageError,'Scale'):
            self.stage()
        self.members[path]=self.members[path].replace(b' Scale="0.7"',b'')
        path='Libs/Tables/item/InventoryPreset__kcd2vr_throwing_assets1.xml'
        self.members[path]=self.members[path].replace(b'Amount="5"',b'Amount="500"');self.flush()
        with self.assertRaisesRegex(builder.PackageError,'finite shop stock'):
            self.stage()

    def test_duplicate_case_alias_archive_path_refused(self):
        self.members[builder.STARTUP_MEMBER.lower()]=b'foreign alias';self.flush()
        with self.assertRaisesRegex(builder.PackageError,'duplicate/case-alias'):
            self.stage()

    def test_new_external_output_required_and_no_game_write(self):
        (self.root/'existing').mkdir()
        for out in (self.root/'existing',self.input/'nested',self.root/'steamapps/new'):
            with self.subTest(out=out):
                with self.assertRaises(builder.PackageError):
                    builder.build(self.input,out,starter_source=self.starter,supply_source=None)
        self.assertFalse((self.root/'steamapps').exists())

    def test_missing_fallback_api_or_localization_refused(self):
        self.starter.write_bytes(b'-- wrong unrelated Lua file\n')
        with self.assertRaisesRegex(builder.PackageError,'fallback API'):
            self.stage()
        self.starter.write_bytes(b'KCD2VRThrowingAssets1={NativeReady=function()end}\n')
        self.files[builder.MOD+'Localization/English_xml.pak']=zipped({'text_ui__kcd2vr_throwing_assets1.xml':b'<Table/>'});self.flush()
        with self.assertRaisesRegex(builder.PackageError,'visible inventory item text'):
            self.stage()


if __name__ == '__main__':
    unittest.main()
