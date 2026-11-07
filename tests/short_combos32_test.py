#!/usr/bin/env python3
"""Actual pinned retail recipes -> optional native two-step variant, no fake outcomes."""
import importlib.util
from pathlib import Path
import unittest
import tempfile
import zipfile
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("short_combos32", ROOT / "tools/build_short_combos32.py")
m = importlib.util.module_from_spec(spec)
spec.loader.exec_module(m)
FIX = ROOT / "tests/fixtures/native_short_combos32"

class NativeShortCombos32(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.combos = (FIX / "CombatCombos__generated.xml").read_bytes()
        cls.links = (FIX / "perk_combat_technique.xml").read_bytes()

    def test_actual_native_rows_only(self):
        out, changes = m.rewrite(self.combos, self.links)
        old = {m.key(r): r for r in m.parse(self.combos)}
        new = {m.key(r): r for r in m.parse(out)}
        self.assertEqual(set(old), set(new))
        self.assertEqual(len(changes), 9)
        for key, row in new.items():
            self.assertEqual(row.attrib, old[key].attrib)
            before = old[key].findall("CombatComboSteps/CombatComboStep")
            after = row.findall("CombatComboSteps/CombatComboStep")
            if key in m.SELECTED:
                self.assertEqual([s.attrib for s in after], [s.attrib for s in before[-2:]])
                self.assertEqual(ET.tostring(after[-1]), ET.tostring(before[-1]))
            else:
                self.assertEqual(ET.tostring(row), ET.tostring(old[key]))

    def test_actual_shipped_c32_variant_keeps_all_map_dialogue_members(self):
        tables=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Data/Tables.pak')
        base=ROOT/'companion/reference/C32C1/KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak' # C34: pinned C32C1 base
        shipped=ROOT/'companion/variants/native_short_combos32/KCD2VR_native_short_combos32.pak.disabled'
        with tempfile.TemporaryDirectory() as td:
            out=Path(td)/'candidate.pak.disabled'
            manifest=m.build(tables,base,out)
            self.assertEqual(out.read_bytes(),shipped.read_bytes())
            self.assertEqual(manifest['preserved_base_members'],26)
            old=dict(m.archive_members(base.read_bytes()));new=dict(m.archive_members(out.read_bytes()))
            self.assertEqual(len(new),28)
            for name,data in old.items():self.assertEqual(new[name],data,name)

    def test_wrong_retail_combo_rejected(self):
        with self.assertRaisesRegex(ValueError, "SHA mismatch"):
            m.rewrite(self.combos.replace(b'GuardZone="1"', b'GuardZone="2"', 1), self.links)

    def test_wrong_perk_mapping_rejected(self):
        with self.assertRaisesRegex(ValueError, "SHA mismatch"):
            m.rewrite(self.combos, self.links + b" ")

    def test_duplicate_recipe_key_rejected(self):
        root = ET.fromstring(self.combos)
        root.find("CombatCombos").append(root.find("CombatCombos/CombatCombo"))
        with self.assertRaisesRegex(ValueError, "duplicate key"):
            m.parse(ET.tostring(root))

    def test_unknown_anatomical_recipe_attribute_rejected(self):
        root = ET.fromstring(self.combos)
        root.find(".//CombatComboStep").set("BodyPart", "face")
        with self.assertRaisesRegex(ValueError, "unknown step schema"):
            m.parse(ET.tostring(root))

    def test_equipment_key_not_id_alone(self):
        pommels = [k for k in m.SELECTED if k[0] == "C01"]
        self.assertEqual(len(pommels), 3)
        self.assertEqual(len({m.SELECTED[k][0] for k in pommels}), 3)

    def test_parameter_is_one_shared_native_key(self):
        rows = ET.fromstring(m.PARAM_BYTES).findall("rpg_params/rpg_param")
        self.assertEqual([r.attrib for r in rows], [{"rpg_param_key": "ComboMinSteps", "rpg_param_value": "2"}])

    def test_no_fake_kick_pommel_attack_types(self):
        out, _ = m.rewrite(self.combos, self.links)
        before = {r.get("AttackType") for r in ET.fromstring(self.combos).iter("CombatComboStep")}
        self.assertEqual({r.get("AttackType") for r in ET.fromstring(out).iter("CombatComboStep")}, before)
        self.assertNotIn("pommel", before)
        self.assertNotIn("shield_bash", before)

    def test_enabled_path_refused_before_input_read(self):
        with self.assertRaisesRegex(ValueError, "end .disabled"):
            m.build(Path("missing"), Path("missing"), Path("unsafe.pak"))

    def test_installable_mod_path_refused(self):
        with self.assertRaisesRegex(ValueError, "installable Mods"):
            m.build(Path("missing"), Path("missing"), Path("companion/Mods/KCD2VR/unsafe.pak.disabled"))
        with self.assertRaisesRegex(ValueError, "installable Mods"):
            m.build(Path("missing"), Path("missing"), Path("companion/mods/KCD2VR/unsafe.pak.disabled"))

if __name__ == "__main__":
    unittest.main()
