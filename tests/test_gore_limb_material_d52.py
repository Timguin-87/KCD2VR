"""Production generator material resolution and CLI wrapper regression tests."""
import importlib.util
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest
import xml.etree.ElementTree as ET
import zipfile

TOOL = Path(__file__).resolve().parents[1] / 'tools/gore_limb_d41.py'
spec = importlib.util.spec_from_file_location('gore_limb_d41', TOOL)
gore = importlib.util.module_from_spec(spec)
spec.loader.exec_module(gore)
SKIN = 'objects/characters/humans/male/body/male_body_arms.skin'
MATERIAL = 'objects/characters/humans/male/body/male_body_mat.mtl'


def chunk_file(chunks):
    offset = 16 + 16 * len(chunks)
    table, payload = [], []
    for cid, (kind, version, data) in enumerate(chunks, 1):
        table.append(struct.pack('<HHIII', kind, version, cid, len(data), offset))
        payload.append(data)
        offset += len(data)
    return b'CrCh' + struct.pack('<III', 0x746, len(chunks), 16) + b''.join(table + payload)


def material_chunk(name=b'male_body_mat', version=0x802):
    return 0x1014, version, name.ljust(128, b'\0')


def source_skin(name=b'male_body_mat'):
    # Small valid input for the actual build() path. The test changes materials,
    # not geometry; twelve distal vertices satisfy the generator's MIN_VERTS.
    bones = bytearray(3 * 584)
    for i, bone in enumerate((b'LeftArm', b'LeftForeArm', b'LeftHand')):
        bones[i * 584 + 344:i * 584 + 344 + len(bone)] = bone
        struct.pack_into('<12f', bones, i * 584 + 296, 1, 0, 0, i * .1, 0, 1, 0, 0, 0, 0, 1, 0)
    def stream(kind, count, size, data):
        return 0x1016, 0x800, struct.pack('<IIIHH', 0, kind, count, size, 0) + b'\0' * 8 + data
    positions = b''.join(struct.pack('<3f', i * .01, .02, .03) for i in range(15))
    weights = b''.join(struct.pack('<4H4B', 1 if i < 12 else 0, 0, 0, 0, 255, 0, 0, 0) for i in range(15))
    indices = struct.pack('<15H', *range(15))
    return chunk_file([material_chunk(name), (0x2000, 0x800, bones), stream(0, 15, 12, positions), stream(9, 15, 12, weights), stream(5, 15, 2, indices)])


class MaterialTests(unittest.TestCase):
    def resolve(self, data, available=(MATERIAL,)):
        return gore.source_material_d52(data, SKIN, set(available))[0]

    def test_exact_embedded_beats_folder_first(self):
        self.assertEqual(self.resolve(source_skin(), (MATERIAL, MATERIAL.replace('male_body_mat', 'a_wrong'))), MATERIAL)

    def test_case_and_extension(self):
        self.assertEqual(self.resolve(source_skin(b'MALE_BODY_MAT.mtl')), MATERIAL)

    def test_exact_root_path(self):
        self.assertEqual(self.resolve(source_skin(MATERIAL.replace('/', '\\').encode())), MATERIAL)

    def test_no_guess_when_missing(self):
        self.assertEqual(self.resolve(source_skin(), (MATERIAL.replace('male_body_mat', 'capon_body'),)), '')

    def test_same_basename_elsewhere_is_not_a_match(self):
        self.assertEqual(self.resolve(source_skin(), ('objects/other/male_body_mat.mtl',)), '')

    def test_exporter_alias_is_not_stripped(self):
        self.assertEqual(self.resolve(source_skin(b'male_body_mat1')), '')

    def test_missing_material_chunk(self):
        self.assertEqual(self.resolve(chunk_file([])), '')

    def test_ambiguous_material_chunks(self):
        self.assertEqual(self.resolve(chunk_file([material_chunk(), material_chunk()])), '')

    def test_unsupported_material_version(self):
        self.assertEqual(self.resolve(chunk_file([material_chunk(version=0x800)])), '')

    def test_invalid_fields(self):
        for name in (b'', b'../male_body_mat', b'/male_body_mat', b'c:/male_body_mat', b'a//b', b'foo/../male_body_mat', b'male_body_mat"', b'\xff', b'a' * 128):
            with self.subTest(name=name):
                self.assertEqual(self.resolve(chunk_file([material_chunk(name)])), '')

    def test_unproven_relative_path(self):
        self.assertEqual(self.resolve(source_skin(b'body/male_body_mat')), '')

    def test_bounds_and_header(self):
        good = source_skin()
        for data in (b'', good[:15], b'junk' + good[4:], good[:16], good[:40]):
            with self.subTest(length=len(data)):
                self.assertEqual(self.resolve(data), '')
        bad = bytearray(good)
        struct.pack_into('<I', bad, 28, len(good))  # Material chunk past EOF.
        self.assertEqual(self.resolve(bad), '')

    def test_cli_default_and_named_variant(self):
        for include_exact in (True, False):
            with self.subTest(include_exact=include_exact), tempfile.TemporaryDirectory() as folder:
                root = Path(folder)
                pak = root / 'native.pak'
                named = MATERIAL.replace('male_body_mat', 'coat_m02')
                with zipfile.ZipFile(pak, 'w') as z:
                    z.writestr(SKIN, source_skin())
                    z.writestr(MATERIAL.replace('male_body_mat', 'a_wrong'), '<Material/>')
                    z.writestr(named, '<Material/>')
                    if include_exact:
                        z.writestr(MATERIAL, '<Material/>')
                run = subprocess.run([sys.executable, str(TOOL), '--pak', str(pak), '--out', str(root / 'out')], capture_output=True, text=True)
                self.assertEqual(run.returncode, 0, run.stdout + run.stderr)
                stem = root / 'out' / SKIN[:-5]
                default = ET.parse(str(stem) + '_kcdglap.fly.cdf').find('.//Attachment')
                self.assertEqual(default.get('Material'), MATERIAL if include_exact else None)
                variant = ET.parse(str(stem) + '_kcdglap.fly_m02.cdf').find('.//Attachment')
                self.assertEqual(variant.get('Material'), named)
                original = source_skin()
                built, _ = gore.build(original, 'la')
                self.assertEqual(Path(str(stem) + '_kcdglap.skin').read_bytes(), built['lap'][0])


if __name__ == '__main__':
    unittest.main()
