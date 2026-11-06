"""Reject concrete delivery corruption of the real optional H15 package."""
from pathlib import Path
import importlib.util
import json
import shutil
import struct
import tempfile
import unittest
import zipfile

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('gore_verify_h15', ROOT/'tools/verify_optional_gore_h15.py')
verifier = importlib.util.module_from_spec(spec)
spec.loader.exec_module(verifier)


class OptionalGoreH15(unittest.TestCase):
    def copied(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        root = Path(temporary.name)
        shutil.copytree(ROOT/'companion/OptionalGoreH15', root/'companion/OptionalGoreH15')
        shutil.copytree(ROOT/'gore_pak', root/'gore_pak', ignore=shutil.ignore_patterns('Objects'))
        return root, root/'companion/OptionalGoreH15/Mods/kcd2vr_gore/Data/kcd2vr_gore.pak'

    def test_complete_reviewed_package(self):
        result = verifier.verify(ROOT)
        self.assertEqual(result['entries'], 3358)
        self.assertEqual(result['preserved_members'], 3357)
        self.assertTrue(result['exact_lua_pair'])

    def test_missing_pak_is_rejected(self):
        root, pak = self.copied(); pak.unlink()
        with self.assertRaisesRegex(ValueError, 'Missing paired'):
            verifier.verify(root)

    def test_changed_source_lua_is_rejected(self):
        root, _ = self.copied()
        script = root/'gore_pak/Scripts/Startup/zzkcd2vr_gore.lua'
        script.write_bytes(script.read_bytes() + b'\n-- unrelated edit\n')
        with self.assertRaisesRegex(ValueError, 'Lua/source pairing differs'):
            verifier.verify(root)

    def test_normalized_duplicate_is_rejected(self):
        root, pak = self.copied()
        with zipfile.ZipFile(pak, 'a') as archive:
            archive.writestr('scripts\\startup\\ZZKCD2VR_GORE.LUA', b'duplicate')
        with self.assertRaisesRegex(ValueError, 'Duplicate normalized'):
            verifier.verify(root)

    def test_unsafe_member_path_is_rejected(self):
        root, pak = self.copied()
        with zipfile.ZipFile(pak, 'a') as archive:
            archive.writestr('../escaped.cdf', b'bad')
        with self.assertRaisesRegex(ValueError, 'Unsafe PAK member'):
            verifier.verify(root)

    def test_corrupt_compressed_member_is_rejected(self):
        root, pak = self.copied()
        with zipfile.ZipFile(pak) as archive:
            info = next(i for i in archive.infolist() if i.compress_size > 20)
        data = bytearray(pak.read_bytes())
        name_size, extra_size = struct.unpack_from('<HH', data, info.header_offset + 26)
        start = info.header_offset + 30 + name_size + extra_size
        data[start + info.compress_size // 2] ^= 0x01
        pak.write_bytes(data)
        with self.assertRaisesRegex(ValueError, 'PAK CRC/data check failed'):
            verifier.verify(root)

    def test_wrong_asset_provenance_is_rejected(self):
        root, pak = self.copied()
        path = pak.with_name(pak.name + '.provenance.json')
        data = json.loads(path.read_text())
        data['preserved'][0]['sha256'] = '0' * 64
        path.write_text(json.dumps(data))
        with self.assertRaisesRegex(ValueError, 'Provenance member hash/size differs'):
            verifier.verify(root)


if __name__ == '__main__':
    unittest.main()
