"""Derive Repair2's Data PAK from the exact tested Repair1 PAK.

Only the retired C8 RPG override is removed. All surviving payloads and ZIP
member metadata are checked after writing; this does not install anything.
"""
import argparse
import copy
import hashlib
import json
from pathlib import Path
import subprocess
import zipfile

BASE_SHA256 = "de3daadf3b5fd73c57ac5e352e5b9267efcbf78e5c2defe5a9090e4184696d13"
REMOVED = "Libs/Tables/rpg/rpg_param__kcdvr.xml"


def digest(data):
    return hashlib.sha256(data).hexdigest()


def metadata(info):
    return {key: getattr(info, key) for key in (
        "filename", "date_time", "compress_type", "comment", "extra",
        "create_system", "create_version", "extract_version", "internal_attr",
        "external_attr")}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--baseline-pak", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    source = Path(__file__).resolve().parents[1]
    if (source / "companion/source" / REMOVED).exists():
        raise SystemExit("Retired override is still present in source")
    raw = args.baseline_pak.read_bytes()
    if digest(raw) != BASE_SHA256:
        raise SystemExit("Baseline PAK fingerprint mismatch")
    receipt_path = args.output.with_suffix(args.output.suffix + ".receipt.json")
    if args.output.exists() or receipt_path.exists():
        raise SystemExit("Refusing to overwrite an output or receipt")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(args.baseline_pak) as old:
        members = old.infolist()
        names = [item.filename for item in members]
        if len(names) != len(set(names)) or names.count(REMOVED) != 1:
            raise SystemExit("Unexpected duplicate members or missing override")
        with args.output.open("xb") as stream:
            with zipfile.ZipFile(stream, "w") as new:
                new.comment = old.comment
                for item in members:
                    if item.filename != REMOVED:
                        # ZipFile mutates header offsets on write; never mutate
                        # the baseline reader's own directory objects.
                        new.writestr(copy.copy(item), old.read(item.filename))
        with zipfile.ZipFile(args.output) as new:
            assert new.namelist() == [name for name in names if name != REMOVED]
            assert new.comment == old.comment
            preserved = []
            for item in members:
                if item.filename == REMOVED:
                    continue
                payload = old.read(item.filename)
                assert new.read(item.filename) == payload, item.filename
                assert metadata(new.getinfo(item.filename)) == metadata(item), item.filename
                preserved.append({"name": item.filename, "sha256": digest(payload)})
            assert new.testzip() is None
        receipt = {
            "baseline": str(args.baseline_pak.resolve()), "baseline_sha256": BASE_SHA256,
            "source_commit": subprocess.check_output(
                ["git", "rev-parse", "HEAD"], cwd=source, text=True).strip(),
            "tool_sha256": digest(Path(__file__).read_bytes()),
            "output": str(args.output.resolve()), "output_sha256": digest(args.output.read_bytes()),
            "removed": {"name": REMOVED, "sha256": digest(old.read(REMOVED))},
            "preserved_members": preserved, "verification": "PASS",
            "scope": "Archive preservation only; native NPC behavior requires headset verification",
        }
    receipt_path.write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({key: receipt[key] for key in (
        "source_commit", "output_sha256", "removed", "verification")}))


if __name__ == "__main__":
    main()
