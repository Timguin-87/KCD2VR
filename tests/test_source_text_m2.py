import hashlib
import importlib.util
from pathlib import Path
import tempfile
import unittest

REPO = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("source_text", REPO / "tools/source_text.py")
# Normal module loading is required by dataclasses; no global Path monkeypatch.
import sys
reader = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = reader
spec.loader.exec_module(reader)


class SourceTextTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        (self.root / "src/feature").mkdir(parents=True)

    def put(self, name, content):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(content.encode())
        return path

    def test_nested_relative_and_src_root(self):
        entry = self.put("src/old.cpp", '#include "feature/body.cpp" // M2_OWNED_SOURCE\n')
        self.put("src/feature/body.cpp", 'first\r\n#include "piece.inl" // M2_OWNED_SOURCE\r\n#include "root.inl" // M2_OWNED_SOURCE\r\nlast\r\n')
        self.put("src/feature/piece.inl", "relative\r\n")
        self.put("src/root.inl", "root\n")
        result = reader.expand_source(entry, root=self.root)
        self.assertEqual(result.text, "first\nrelative\nroot\nlast\n")
        self.assertEqual(len(result.origins), 4)
        self.assertEqual(len(result.edges), 3)
        self.assertEqual(reader.read_source_bytes(entry, root=self.root), result.text.encode())
        self.assertEqual(result.provenance()["expanded_sha256"], hashlib.sha256(result.text.encode()).hexdigest())

    def test_canonical_edit_is_immediate(self):
        entry = self.put("src/old.h", '#include "feature/body.h" // M2_OWNED_SOURCE\n')
        canonical = self.put("src/feature/body.h", "before\n")
        self.assertEqual(reader.read_source(entry, root=self.root), "before\n")
        canonical.write_bytes(b"after\n")
        self.assertEqual(reader.read_source(entry, root=self.root), "after\n")

    def test_unmarked_and_similar_comments_untouched(self):
        text = '#include "feature/body.h"\n#include "other.h" // M2_OWNED_SOURCE extra\nconst char* s="M2_OWNED_SOURCE";\n'
        entry = self.put("src/old.cpp", text)
        self.assertEqual(reader.read_source(entry, root=self.root), text)

    def test_exact_splice_no_added_newline(self):
        entry = self.put("src/old.cpp", 'prefix\n#include "part.inl" // M2_OWNED_SOURCE\nend')
        self.put("src/part.inl", "body")
        self.assertEqual(reader.read_source(entry, root=self.root), "prefix\nbodyend")

    def test_cycle_refused(self):
        a = self.put("src/a.h", '#include "b.h" // M2_OWNED_SOURCE\n')
        self.put("src/b.h", '#include "a.h" // M2_OWNED_SOURCE\n')
        with self.assertRaisesRegex(reader.SourceError, "cycle"):
            reader.read_source(a, root=self.root)

    def test_outside_refused(self):
        a = self.put("src/a.h", '#include "../../outside.h" // M2_OWNED_SOURCE\n')
        # Use a separately owned temporary target rather than any ambient file.
        with tempfile.TemporaryDirectory() as external:
            target = Path(external) / "outside.h"
            target.write_text("outside")
            a.write_text('#include "' + str(target).replace("\\", "/") + '" // M2_OWNED_SOURCE\n')
            with self.assertRaises(reader.SourceError):
                reader.read_source(a, root=self.root)
            with self.assertRaises(reader.SourceError):
                reader.read_source(target, root=self.root)

    def test_relative_escape_existing_target_refused(self):
        with tempfile.TemporaryDirectory() as external:
            outside = Path(external) / "outside.h"
            outside.write_text("outside")
            import os
            relative = os.path.relpath(outside, self.root / "src").replace("\\", "/")
            a = self.put("src/a.h", '#include "' + relative + '" // M2_OWNED_SOURCE\n')
            with self.assertRaises(reader.SourceError):
                reader.read_source(a, root=self.root)

    def test_repeated_include_is_not_cycle(self):
        a = self.put("src/a.h", '#include "b.h" // M2_OWNED_SOURCE\n#include "b.h" // M2_OWNED_SOURCE\n')
        self.put("src/b.h", "twice\n")
        result = reader.expand_source(a, root=self.root)
        self.assertEqual(result.text, "twice\ntwice\n")
        self.assertEqual(len(result.origins), 2)

    def test_missing_marked_target_refused(self):
        a = self.put("src/a.h", '#include "missing.h" // M2_OWNED_SOURCE\n')
        with self.assertRaisesRegex(reader.SourceError, "not found"):
            reader.read_source(a, root=self.root)

    def test_encoding_and_newlines(self):
        a = self.put("src/a.h", "first\rsecond\r\nthird\n")
        self.assertEqual(reader.read_source(a, root=self.root), "first\nsecond\nthird\n")
        a.write_bytes(b"\xef\xbb\xbfhello\n")
        self.assertEqual(reader.read_source(a, encoding="utf-8-sig", root=self.root), "hello\n")

    def selector(self, slot):
        return '#define KCD2VR_M2_HOST_SLOT ' + str(slot) + ' // M2_SOURCE_SELECTOR\n#include "feature/host.inl" // M2_OWNED_SOURCE slot=' + str(slot) + '\n#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR\n'

    def test_explicit_host_selector_with_recursive_source(self):
        a = self.put("src/a.cpp", "before\n" + self.selector(17) + "after\n" + self.selector(18))
        self.put("src/feature/host.inl", '#if KCD2VR_M2_HOST_SLOT == 17\n// M2_SLOT_BEGIN 17\nfirst\n#include "piece.inl" // M2_OWNED_SOURCE\n// M2_SLOT_END 17\n#elif KCD2VR_M2_HOST_SLOT == 18\n// M2_SLOT_BEGIN 18\nsecond\n// M2_SLOT_END 18\n#endif\n')
        self.put("src/feature/piece.inl", "nested\n")
        result = reader.expand_source(a, root=self.root)
        self.assertEqual(result.text, "before\nfirst\nnested\nafter\nsecond\n")
        self.assertEqual([edge["slot"] for edge in result.provenance()["edges"] if "slot" in edge], [17, 18])

    def test_bad_selector_or_duplicate_slot_refused(self):
        a = self.put("src/a.cpp", self.selector(17))
        canonical = self.put("src/feature/host.inl", "// M2_SLOT_BEGIN 18\nother\n// M2_SLOT_END 18\n")
        with self.assertRaisesRegex(reader.SourceError, "slot"):
            reader.read_source(a, root=self.root)
        canonical.write_text("// M2_SLOT_BEGIN 17\none\n// M2_SLOT_END 17\n// M2_SLOT_BEGIN 17\ntwo\n// M2_SLOT_END 17\n")
        with self.assertRaisesRegex(reader.SourceError, "duplicate"):
            reader.read_source(a, root=self.root)
        canonical.write_text("// M2_SLOT_BEGIN 17\none\n// M2_SLOT_END 17\n")
        a.write_text(self.selector(17).replace("slot=17", "slot=18"))
        with self.assertRaisesRegex(reader.SourceError, "mismatch"):
            reader.read_source(a, root=self.root)
        a.write_text('#include "feature/host.inl" // M2_OWNED_SOURCE slot=17\n')
        with self.assertRaisesRegex(reader.SourceError, "malformed"):
            reader.read_source(a, root=self.root)
        a.write_text(self.selector("017"))
        with self.assertRaisesRegex(reader.SourceError, "decimal"):
            reader.read_source(a, root=self.root)  # C++ would otherwise treat 017 as octal.

    def test_selected_comment_requires_actual_compiler_guard(self):
        a = self.put("src/a.cpp", self.selector(17))
        bodies = [
            '#if KCD2VR_M2_HOST_SLOT == 18\n// M2_SLOT_BEGIN 17\nwrong\n// M2_SLOT_END 17\n#endif\n',
            '#if 0\n// M2_SLOT_BEGIN 17\nwrong\n// M2_SLOT_END 17\n#endif\n',
            '#if KCD2VR_M2_HOST_SLOT == 17\n#if 0\n// M2_SLOT_BEGIN 17\nwrong\n// M2_SLOT_END 17\n#endif\n#endif\n',
            '#if KCD2VR_M2_HOST_SLOT == 17\nskipped\n#elif KCD2VR_M2_HOST_SLOT == 17\n// M2_SLOT_BEGIN 17\nwrong\n// M2_SLOT_END 17\n#endif\n',
            '#if KCD2VR_M2_HOST_SLOT == 017\n// M2_SLOT_BEGIN 17\nwrong\n// M2_SLOT_END 17\n#endif\n',
            '// M2_SLOT_BEGIN 17\nwrong\n// M2_SLOT_END 17\n',
        ]
        for body in bodies:
            with self.subTest(body=body):
                self.put("src/feature/host.inl", body)
                with self.assertRaisesRegex(reader.SourceError, "guard"):
                    reader.read_source(a, root=self.root)

    def test_balanced_internal_conditionals_preserved_but_selector_change_refused(self):
        a = self.put("src/a.cpp", self.selector(17))
        header = '#if KCD2VR_M2_HOST_SLOT == 17\n// M2_SLOT_BEGIN 17\n'
        trailer = '// M2_SLOT_END 17\n#endif\n'
        body = '#if FEATURE\nfirst\n#else\nsecond\n#endif\n'
        self.put("src/feature/host.inl", header + body + trailer)
        self.assertEqual(reader.read_source(a, root=self.root), body)
        for bad in ['#else\nwrong\n', '#define KCD2VR_M2_HOST_SLOT 18\n', '#if FEATURE\nwrong\n']:
            self.put("src/feature/host.inl", header + bad + trailer)
            with self.assertRaisesRegex(reader.SourceError, "guard|selector"):
                reader.read_source(a, root=self.root)

if __name__ == "__main__":
    unittest.main()
