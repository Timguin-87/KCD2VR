"""Pure proof-generator unit tests; full composition requires explicit freeze SHA."""
import importlib.util
from pathlib import Path
import sys
import unittest

spec = importlib.util.spec_from_file_location("m2_composition", Path(__file__).resolve().parents[1] / "tools/test_subsystem_composition_m2.py")
proof = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = proof
spec.loader.exec_module(proof)


class CompositionGeneratorTests(unittest.TestCase):
    def test_actual_descriptor_source_edit_preserves_prefix_and_newlines(self):
        for raw in (b"original\n", b"original\r\n"):
            changed = proof.descriptor_edit(raw, "vrmap")
            self.assertTrue(changed.startswith(raw))
            self.assertIn(b'static_assert(sizeof(char) == 1, "M2_COMPOSITION_vrmap");', changed)

    def test_host_marker_is_inside_the_selected_body(self):
        raw = b"#if KCD2VR_M2_HOST_SLOT == 33\r\n// M2_SLOT_BEGIN 33\r\nreal_body();\r\n// M2_SLOT_END 33\r\n#endif\r\n"
        changed, slot, marker = proof.host_edit(raw, "vrmap")
        self.assertEqual(slot, 33)
        self.assertLess(changed.index(marker.encode()), changed.index(b"// M2_SLOT_END 33"))
        self.assertEqual(changed.replace(b"// " + marker.encode() + b"\r\n", b""), raw)

    def test_missing_host_slot_refused(self):
        with self.assertRaisesRegex(ValueError, "no lexical slot"):
            proof.host_edit(b"// reserved module\n", "alchemy")

    def test_external_helper_reference_preserves_version_contract(self):
        raw = b"ModuleDescriptor d{}; d.id=13; return d;\n"
        changed = proof.helper_edit(raw)
        self.assertTrue(changed.startswith(b'#include "composition_helper_m2.h"\n'))
        self.assertIn(b"d.module_version=CompositionHelperM2();", changed)
        self.assertIn(b"d.id=13; return d;", changed)
        with self.assertRaisesRegex(ValueError, "qualified initializer"):
            proof.helper_edit(raw + raw)

    def test_lane_identifier_cannot_inject_source(self):
        for lane in ("../base", 'x";bad();', "Base", "x-y"):
            with self.assertRaises(ValueError):
                proof.marker_for(lane)


if __name__ == "__main__":
    unittest.main()
