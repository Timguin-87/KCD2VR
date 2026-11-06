"""Verify M2 combat source moves without compiling or executing game code.

Canonicalization only restores the recorded spelling of quoted include paths;
the resolved include target must stay identical. All C++ declarations,
statements, comments and ordering must otherwise expand byte-for-byte (LF).
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
from source_text import expand_source

ROOT = Path(__file__).resolve().parents[1]
MARKED = re.compile(r'^[ \t]*#\s*include\s*"([^"\n]+)"\s*//\s*M2_OWNED_SOURCE\s*\n', re.M)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--report", type=Path)
    args = parser.parse_args()
    data = json.loads((ROOT / "review/M2_COMBAT/EXTRACTION.json").read_text(encoding="utf-8-sig"))
    rewrites = data["relative_include_rewrites"]

    def resolve(path, include):
        for option in (path.parent / include, ROOT / "src" / include):
            if option.is_file():
                result = option.resolve()
                assert result.is_relative_to(ROOT), (path, include)
                return result
        raise AssertionError((path, include))

    include_checks = []
    for new, pairs in rewrites.items():
        old = data["moved"][new]
        for after, before in pairs:
            # A parent extraction may contain another marked child. Its new
            # path is resolved before canonical expansion, just like C++.
            a = re.search(r'#\s*include\s*"([^"]+)"', after)[1]
            b = re.search(r'#\s*include\s*"([^"]+)"', before)[1]
            left, right = resolve(ROOT / new, a), resolve(ROOT / old, b)
            assert left == right, (new, after, before, left, right)
            include_checks.append({"source": new, "target": left.relative_to(ROOT).as_posix()})

    def visit(path, stack=()):
        assert path not in stack, path
        name = path.relative_to(ROOT).as_posix()
        text = path.read_text(encoding="utf-8-sig")
        text = MARKED.sub(lambda m: visit(resolve(path, m[1]), (*stack, path)), text)
        # Marked directives are gone. Only ordinary include spelling changes
        # remain; their exact resolved target was checked independently above.
        for after, before in rewrites.get(name, []):
            if "M2_OWNED_SOURCE" not in after:
                text = text.replace(after, before)
        return text

    results = []
    for name, expected in data["originals"].items():
        expanded = expand_source(ROOT / name)
        canonical = visit(ROOT / name)
        actual = hashlib.sha256(canonical.encode()).hexdigest()
        assert actual == expected["sha256_lf"], (name, expected["sha256_lf"], actual)
        assert canonical.count("\n") == expected["lines"], name
        results.append({"path": name, "canonical_sha256": actual, "expanded_lines": expected["lines"],
                        "central_reader_origins": len(expanded.origins), "equivalent": True})
    report = {"result": "PASS", "meaning": "exact expanded source modulo LF and same-target quoted include spelling; no runtime claim",
              "files": results, "resolved_include_checks": include_checks,
              "owned_implementation_files": len(data["moved"])}
    if args.report:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f'PASS {len(results)} source expansion hashes, {len(include_checks)} same-target includes, {len(data["moved"])} owned implementation files')


if __name__ == "__main__":
    main()
