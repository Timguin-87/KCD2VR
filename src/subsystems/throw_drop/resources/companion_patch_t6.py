#!/usr/bin/env python3
"""Overlay T6 owned Lua into an existing unified companion; never install it.

The INPUT is the supplied/latest combined distribution, never historical donors.
All other archive payloads, tables, localization and top-level files are kept.
"""
import argparse
import copy
import hashlib
import json
from pathlib import Path, PurePosixPath
import re
import sys
import xml.etree.ElementTree as ET
from xml.sax.saxutils import escape
import zipfile

REPO = Path(__file__).resolve().parents[4]
STARTER = Path(__file__).with_name("inventory_delivery_t6.lua")
SUPPLY = REPO / "src/vrik/native_supply_t2.lua"
MOD = "Mods/KCD2VR/"
PAK = MOD + "Data/KCD2VR.pak"
MARKER = MOD + "KCD2VR_UNIFIED63.json"
MANIFEST = MOD + "mod.manifest"
STARTUP_MEMBER = "Scripts/Startup/zzkcd2vr_throwing_assets1.lua"
SUPPLY_MEMBER = "Scripts/KCD2VR/native_supply_t2.lua"
ITEMS = {
    "43286e47-cde0-50bd-ba4c-9747b777b16d": ("kcd2vr_throwing_dagger1", "0", "manmade/weapons/daggers/dagger_common.cgf", 5),
    "c0760688-f426-5341-95b9-482b87dbcb9b": ("kcd2vr_throwing_axe1", "3", "manmade/weapons/axes/workaxe01", 3),
}
RECEIPTS = {"70ceb681-04b8-5c5d-bb49-f80f03c9dc40", "86c6952f-0382-5a68-9435-aebcd31a4a39"}


class PackageError(ValueError):
    pass


def sha(data):
    return hashlib.sha256(data).hexdigest()


def require(condition, reason):
    if not condition:
        raise PackageError(reason)


def safe_name(name, directory=False):
    value = name[:-1] if directory and name.endswith("/") else name
    parts = value.split("/")
    require(value and "\\" not in value and ":" not in value and not PurePosixPath(value).is_absolute() and
            all(part not in ("", ".", "..") for part in parts), "unsafe member path: " + name)
    return name


def plain_path(path):
    for entry in (path, *path.parents):
        require(not entry.is_symlink(), "linked path refused: " + str(entry))


def read_tree(root):
    plain_path(root)
    require(root.is_dir(), "input unified directory does not exist")
    result = {}
    for path in sorted(root.rglob("*")):
        require(not path.is_symlink(), "linked distribution member refused")
        if path.is_file():
            name = safe_name(path.relative_to(root).as_posix())
            require(name.casefold() not in {x.casefold() for x in result}, "case-alias distribution member")
            require(path.suffix.casefold() not in (".asi", ".dll", ".exe", ".ini"), "not a companion-only input: " + name)
            result[name] = path.read_bytes()
    for name in (PAK, MARKER, MANIFEST):
        require(name in result, "missing unified companion member: " + name)
    return result


def read_pak(raw):
    import io
    with zipfile.ZipFile(io.BytesIO(raw)) as archive:
        require(archive.testzip() is None, "input gameplay PAK CRC failure")
        result, folded = {}, set()
        for info in archive.infolist():
            name = safe_name(info.filename, info.is_dir())
            require(name.casefold() not in folded, "duplicate/case-alias PAK member: " + name)
            require(not ((info.external_attr >> 16) & 0o170000) == 0o120000, "linked PAK member refused")
            require(not info.flag_bits & 1, "encrypted PAK member refused")
            folded.add(name.casefold())
            result[name] = (copy.copy(info), archive.read(info))
    require(STARTUP_MEMBER in result and SUPPLY_MEMBER in result, "input has no current throwing Lua API members")
    return result


def xml_member(members, name):
    require(name in members, "missing existing throwing definition: " + name)
    try:
        return ET.fromstring(members[name][1].decode("utf-8-sig"))
    except (ET.ParseError, UnicodeError) as error:
        raise PackageError("invalid existing XML " + name) from error


def validate_definitions(members, files):
    item = xml_member(members, "Libs/Tables/item/item__kcd2vr_throwing_assets1.xml")
    section = item.find("ItemClasses")
    require(section is not None and section.get("version") == "8", "wrong native item table version")
    for guid, (name, weapon_class, model, _) in ITEMS.items():
        rows = [row for row in item.iter("MeleeWeapon") if row.get("Id", "").lower() == guid]
        require(len(rows) == 1, "missing/duplicate ordinary native throwable: " + guid)
        row = rows[0]
        require(row.get("Name") == name and row.get("Class") == weapon_class and row.get("Model") == model,
                "throwable native identity/class/model mismatch: " + name)
        require(row.get("UIName") == "ui_nm_" + name and row.get("UIInfo") == "ui_in_" + name,
                "throwable visible localization keys missing: " + name)
        require("Scale" not in row.attrib, "unqualified item Scale attribute")
    for member, tag in (("perk", "perk"), ("perk_script", "perk_script")):
        root = xml_member(members, "Libs/Tables/rpg/" + member + "__kcd2vr_throwing_assets1.xml")
        for receipt in RECEIPTS:
            require(sum(row.get("perk_id", "").lower() == receipt for row in root.iter(tag)) == 1,
                    "missing/duplicate save-owned family receipt: " + receipt)
    stock = xml_member(members, "Libs/Tables/item/InventoryPreset__kcd2vr_throwing_assets1.xml")
    section = stock.find("InventoryPresets")
    require(section is not None and section.get("version") == "2", "wrong native stock table version")
    rows = [r for r in stock.iter("InventoryPreset") if r.get("Name") == "inventory_kcd2vr_throwing_assets1"]
    require(len(rows) == 1, "missing/duplicate own native stock preset")
    for _, (name, _, _, amount) in ITEMS.items():
        entries = [r for r in rows[0].iter("PresetItem") if r.get("Name") == name]
        require(len(entries) == 1 and entries[0].get("Amount") == str(amount), "wrong finite shop stock: " + name)
    locales = [n for n in files if n.startswith(MOD + "Localization/") and n.endswith("_xml.pak")]
    require(len(locales) >= 16, "input unified companion is missing required localization packages")
    import io
    for name in locales:
        with zipfile.ZipFile(io.BytesIO(files[name])) as archive:
            require(archive.testzip() is None, "localization PAK CRC failure: " + name)
    english = MOD + "Localization/English_xml.pak"
    require(english in files, "missing native inventory English localization")
    with zipfile.ZipFile(io.BytesIO(files[english])) as archive:
        require(archive.testzip() is None, "localization PAK CRC failure")
        labels = {}
        for name in archive.namelist():
            if name.lower().endswith(".xml") and "throwing" in name.lower():
                for row in ET.fromstring(archive.read(name).decode("utf-8-sig")).iter("Row"):
                    cells = [c.text or "" for c in row.findall("Cell")]
                    if len(cells) >= 2:
                        require(cells[0] not in labels, "duplicate inventory localization key")
                        labels[cells[0]] = cells[1]
        for _, (name, _, _, _) in ITEMS.items():
            for prefix in ("ui_nm_", "ui_in_"):
                require(labels.get(prefix + name, "").strip(), "missing visible inventory item text")
    return {"native_items": list(ITEMS), "family_receipts": sorted(RECEIPTS), "localization_packages": len(locales)}


def update_description(raw):
    root = ET.fromstring(raw.decode("utf-8-sig"))
    node = root.find("info/description")
    require(node is not None and not list(node), "manifest description must be a plain existing text node")
    description = node.text or ""
    note = "T6: native inventory starter once (5 daggers and 3 compact native axes), independent of right-hip visibility; save-owned claimed receipts are never reset or refilled. No ASI or native door protocol change."
    if note not in description:
        description += " " + note
    # Preserve every other manifest byte, including current combined version,
    # mod ID and unrelated additions. Do not turn this PAK patch into an ASI bump.
    result, count = re.subn(rb"<description>.*?</description>", ("<description>" + escape(description) + "</description>").encode("utf-8"), raw, count=1, flags=re.DOTALL)
    require(count == 1, "cannot patch exact existing manifest description")
    return result


def build(input_unified, out, *, starter_source=STARTER, supply_source=SUPPLY):
    input_unified, out = Path(input_unified).absolute(), Path(out).absolute()
    plain_path(input_unified)
    plain_path(out)
    input_unified, out = input_unified.resolve(strict=True), out.resolve()
    require(not out.exists(), "output must be a NEW external staging directory")
    require(not out.is_relative_to(input_unified) and not input_unified.is_relative_to(out), "input/output directories overlap")
    require(not out.is_relative_to(REPO) and "steamapps" not in {p.casefold() for p in out.parts}, "output cannot be source/game directory")
    files = read_tree(input_unified)
    input_hashes = {n: sha(data) for n, data in files.items()}
    # Our previous receipt is audit output, not an input asset to recursively
    # inventory/hash inside its replacement. Input bytes still remain recorded.
    files.pop("T6_PACKAGE_RECEIPT.json", None)
    metadata = json.loads(files[MARKER])
    require(metadata.get("version") == 63 and metadata.get("pak_sha256") == sha(files[PAK]), "stale/incompatible input PAK metadata")
    distribution = {n[len(MOD):]: sha(data) for n, data in files.items() if n.startswith(MOD) and n not in (PAK, MARKER)}
    require(metadata.get("distribution_files") == distribution, "stale/incomplete input distribution inventory")
    members = read_pak(files[PAK])
    checks = validate_definitions(members, files)
    replacements = {STARTUP_MEMBER: Path(starter_source)}
    if supply_source is not None:
        replacements[SUPPLY_MEMBER] = Path(supply_source)
    changes, actual = [], {}
    for name, path in replacements.items():
        plain_path(path)
        data = path.read_bytes()
        require(data and len(data) <= 128 * 1024, "invalid owned Lua source size")
        data.decode("utf-8-sig")  # Validate only; never execute/import a mod script.
        if name == STARTUP_MEMBER:
            require(b"KCD2VRThrowingAssets1" in data and b"NativeReady" in data, "starter must preserve current native fallback API")
        if data != members[name][1]:
            changes.append({"member": name, "before": sha(members[name][1]), "after": sha(data), "source": str(path.resolve())})
            actual[name] = data  # Freeze the exact bytes validated above.
    import io
    stream = io.BytesIO()
    with zipfile.ZipFile(stream, "w") as archive:
        for name, (info, data) in members.items():
            archive.writestr(copy.copy(info), actual.get(name, data))
    patched = stream.getvalue()
    after = read_pak(patched)
    require(set(after) == set(members), "T6 changed the PAK member set")
    changed = {n for n in members if members[n][1] != after[n][1]}
    require(changed == set(actual) <= {STARTUP_MEMBER, SUPPLY_MEMBER}, "T6 touched unrelated PAK assets")
    for name in set(members) - changed:
        require(after[name][1] == members[name][1], "unrelated PAK payload changed")
    files[PAK] = patched
    files[MANIFEST] = update_description(files[MANIFEST])
    metadata["t6_parent"] = {"pak_sha256": metadata["pak_sha256"], "members": len(members), "input": str(input_unified)}
    metadata.update(pak_sha256=sha(patched), members=len(after), installed=False,
                    throwing_starter="NATIVE_INVENTORY_ONCE_5_DAGGERS_3_AXES_INDEPENDENT_RIGHT_HIP",
                    t6_replacements=changes)
    for row in metadata.get("explicit_source_replacements", []):
        if row.get("member") in actual:
            row["before"] = sha(members[row["member"]][1])
            row["after"] = sha(actual[row["member"]])
    provenance = {row["path"]: row for row in metadata.get("members_provenance", [])}
    metadata["members_provenance"] = []
    for name, (_, data) in after.items():
        row = copy.deepcopy(provenance.get(name, {"path": name, "sources": [str(input_unified / PAK)]}))
        row["sha256"] = sha(data)
        if name in actual:
            row.setdefault("sources", []).append(str(replacements[name].resolve()))
        metadata["members_provenance"].append(row)
    metadata["distribution_files"] = {n[len(MOD):]: sha(data) for n, data in files.items() if n.startswith(MOD) and n not in (PAK, MARKER)}
    files[MARKER] = (json.dumps(metadata, indent=2) + "\n").encode("utf-8")
    note = b"\nT6 overlay: the finite 5-dagger/3-axe native inventory starter is independent of hip visibility. Existing save-owned claimed receipts stay terminal; no automatic replenishment. This staging tool did not install anything or change the ASI, INI, native door protocol, item tables or localization.\n"
    if "README.txt" in files:
        if note not in files["README.txt"]:
            files["README.txt"] += note
    receipt = {"schema": 1, "stage": "T6_PAK_ONLY", "installed": False, "input": str(input_unified),
               "input_pak_sha256": input_hashes[PAK], "output_pak_sha256": sha(patched),
               "members_before": len(members), "members_after": len(after), "changed_members": sorted(changed),
               "preserved_other_members": len(members) - len(changed), "replacements": changes,
               "input_files": input_hashes, "validation": checks,
               "door_protocol": metadata.get("door_protocol"), "door_revision": metadata.get("door_revision"),
               "status": "PASS; packaged bytes only, no game/save acceptance claim"}
    # No output is created until every input/schema/archive/hash check succeeds.
    out.mkdir(parents=True)
    for name, data in files.items():
        target = out / name
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(data)
    require(read_tree(input_unified) == {n: (input_unified / n).read_bytes() for n in input_hashes}, "input distribution changed while staging")
    require({n: sha((input_unified / n).read_bytes()) for n in input_hashes} == input_hashes, "input distribution bytes changed while staging")
    require(read_pak((out / PAK).read_bytes()).keys() == members.keys(), "staged PAK verification failed")
    output_files = read_tree(out)
    require(output_files == files, "staged distribution byte verification failed")
    receipt["output_files"] = {n: sha(data) for n, data in files.items()}
    (out / "T6_PACKAGE_RECEIPT.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8", newline="\n")
    return receipt


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input-unified", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--supply-source", type=Path, default=SUPPLY, help="exact owned readiness Lua; unchanged bytes stay untouched")
    args = parser.parse_args()
    try:
        report = build(args.input_unified, args.out, supply_source=args.supply_source)
    except (PackageError, OSError, ValueError, KeyError, ET.ParseError, zipfile.BadZipFile) as error:
        print("REFUSED:", error, file=sys.stderr)
        return 1
    print(json.dumps({k: report[k] for k in ("stage", "members_after", "changed_members", "preserved_other_members", "output_pak_sha256", "status")}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
