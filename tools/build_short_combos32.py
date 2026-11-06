#!/usr/bin/env python3
"""Build an OFF-by-default native two-step recipe companion variant.

This shortens selected learned native recipes to their last two inputs. It does
not create a free pommel/kick/shield opener, grant perks, advance DIRECT combo
state, select reactions, bypass armor, or change any hit/damage implementation.
The native recipe table has no player-only field: its scope is NOT Henry-only.
"""
from __future__ import annotations
import argparse
import copy
import hashlib
import io
import json
from pathlib import Path
import re
import zipfile
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]
TABLES_SHA = "a217cd744ee3aeaa4aefe30598995915bc0db1b245b30b0a2eecd814d9c7da1e"
COMPANION_SHA = "24be84e89b5b37b8b75d63d54ea0b72fc5c9eebe08f322f49a73ecf130e37da4"
COMBOS = "Libs/Tables/combat/CombatCombos__generated.xml"
LINKS = "Libs/Tables/rpg/perk_combat_technique.xml"
PARAM = "Libs/Tables/rpg/rpg_param__kcdvr_short_combos32.xml"
PINS = {
    COMBOS: "048e138ee061ef2003c0ce050bc9f4aa42aed5b6740a97500ca92594603a13e6",
    LINKS: "378ca8370695e734f189d22f4283eb42e01929e493e26efade2c3d624e0ab078",
}
# IDs are equipment-qualified. Values pin the EXISTING technique, never a new
# technique, arbitrary AttackType, or an assumed anatomical hit-part number.
SELECTED = {
    ("C12", "longsword", "longsword"): ("037e3ff3-3939-6242-420f-efe1c8ee0ed4", "pommel finish"),
    ("C01", "shortSwords", "AnyExceptShield"): ("10931c0f-f611-4d5b-8b55-3eb4aa74e51c", "pommel finish"),
    ("C13", "shortSwords", "AnyExceptShield"): ("3c29e8b7-610f-4638-bc43-0f7fe334888f", "pommel finish"),
    ("C01", "blunt", "AnyExceptShield"): ("ddd376c0-db0d-498b-9568-953950ca3087", "handle finish"),
    ("C02", "swords", "shield"): ("abd0c33f-5cd7-43b7-8a12-75d6c4d6ba5f", "shield-edge finish"),
    ("C02", "blunt", "shield"): ("1a624a60-96b5-4cda-a92d-6053f39c5b2a", "shield-edge finish"),
    ("C16", "blunt", "shield"): ("ba6031fc-2401-40bf-95c1-9d3985631881", "shield finish"),
    # These native unarmed techniques contain kicks in their authored sequence;
    # final INPUT remains native hook/punch. They are not free-kick bindings.
    ("C01", "unarmed", "unarmed"): ("3bf8ed7e-9e6b-40ed-bc1a-aa4ddf1cc902", "native kick-containing sequence"),
    ("C02", "unarmed", "unarmed"): ("7653c443-59d6-468c-bd7c-d2881a30bbf7", "native kick-containing sequence"),
}
PARAM_BYTES = (b'<?xml version="1.0" encoding="utf-8"?>\n'
    b'<database><rpg_params version="1">\n'
    b'  <rpg_param rpg_param_key="ComboMinSteps" rpg_param_value="2" />\n'
    b'</rpg_params></database>\n')

def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()

def key(row):
    return tuple(row.get(k, "") for k in ("Id", "RightWeapon", "LeftWeapon"))

def parse(data):
    root = ET.fromstring(data)
    if root.tag != "database" or len(root.findall("CombatCombos")) != 1:
        raise ValueError("unexpected native recipe root/schema")
    rows = root.findall("CombatCombos/CombatCombo")
    if len(rows) != 51 or len({key(r) for r in rows}) != len(rows):
        raise ValueError("recipe row count or equipment-qualified duplicate key")
    for row in rows:
        if set(row.attrib) != {"Id", "RightWeapon", "LeftWeapon", "CombatTechniqueId", "UIGroup", "UIOrder"}:
            raise ValueError("unqualified recipe attribute/schema")
        steps = row.findall("CombatComboSteps/CombatComboStep")
        if len(row) != 1 or not 2 <= len(steps) <= 4:
            raise ValueError("unexpected native recipe steps")
        for s in steps:
            if set(s.attrib) - {"GuardZone", "AttackType", "UseSolver", "IsCharged"}:
                raise ValueError("unknown step schema")
            if s.get("GuardZone") not in {"0", "1", "2", "5"}:
                raise ValueError("unknown native guard zone")
    return rows

def rewrite(combos: bytes, links: bytes):
    for name, data in ((COMBOS, combos), (LINKS, links)):
        if sha(data) != PINS[name]:
            raise ValueError("retail member SHA mismatch: " + name)
    rows = parse(combos)
    linked = {r.get("combat_technique_id"): r.get("perk_id")
              for r in ET.fromstring(links).iter("perk_combat_technique")}
    original = {key(r): copy.deepcopy(r) for r in rows}
    changes = []
    # Replace complete pinned blocks in original bytes: every unrelated row,
    # declaration, newline and attribute order stays byte-for-byte intact.
    block_rx = re.compile(rb'<CombatCombo\s[^>]*>.*?</CombatCombo>', re.S)
    seen = set()
    def replace(m):
        row = ET.fromstring(m.group())
        k = key(row)
        if k not in SELECTED:
            return m.group()
        technique, label = SELECTED[k]
        if k in seen or row.get("CombatTechniqueId") != technique or not linked.get(technique):
            raise ValueError("selected native technique/perk mismatch")
        seen.add(k)
        step_rx = re.compile(rb'\s*<CombatComboStep\s[^>]*/>')
        matches = list(step_rx.finditer(m.group()))
        if len(matches) != len(row.findall("CombatComboSteps/CombatComboStep")):
            raise ValueError("step block representation differs")
        result = m.group()
        for drop in reversed(matches[:-2]):
            result = result[:drop.start()] + result[drop.end():]
        after = ET.fromstring(result)
        before_steps = [dict(x.attrib) for x in row.findall("CombatComboSteps/CombatComboStep")]
        after_steps = [dict(x.attrib) for x in after.findall("CombatComboSteps/CombatComboStep")]
        if after.attrib != row.attrib or after_steps != before_steps[-2:]:
            raise ValueError("two-step rewrite changed native final input/technique")
        changes.append({"key": list(k), "native_technique": technique, "perk": linked[technique],
                        "kind": label, "before": before_steps, "after": after_steps})
        return result
    output = block_rx.sub(replace, combos)
    if seen != set(SELECTED):
        raise ValueError("selected recipe missing")
    after_rows = parse(output)
    for r in after_rows:
        k = key(r)
        if k not in SELECTED and ET.tostring(r) != ET.tostring(original[k]):
            raise ValueError("unselected native recipe changed")
    return output, changes

def archive_members(data: bytes):
    with zipfile.ZipFile(io.BytesIO(data)) as z:
        names = z.namelist()
        if len(names) != len(set(n.lower() for n in names)):
            raise ValueError("duplicate/case-aliased archive member")
        return [(n, z.read(n)) for n in names]

def build(tables: Path, companion: Path, out: Path):
    if out.suffix != ".disabled":
        raise ValueError("variant must end .disabled; installation is a separate explicit choice")
    if any(part.lower() == "mods" for part in out.parts):
        raise ValueError("refusing to write optional variant into installable Mods tree")
    tb, cb = tables.read_bytes(), companion.read_bytes()
    if sha(tb) != TABLES_SHA or sha(cb) != COMPANION_SHA:
        raise ValueError("exact retail Tables/C32 companion archive SHA mismatch")
    with zipfile.ZipFile(io.BytesIO(tb)) as z:
        changed, changes = rewrite(z.read(COMBOS), z.read(LINKS))
    members = archive_members(cb)
    if len(members) != 26 or any(n.lower() in {COMBOS.lower(), PARAM.lower()} for n, _ in members):
        raise ValueError("unexpected C32 companion members or competing combo rows")
    # Do not pair ComboMinSteps with another existing override of the same key.
    for n, data in members:
        if n.endswith(".xml") and b'ComboMinSteps' in data:
            raise ValueError("competing ComboMinSteps key")
    out.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(out, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as z:
        for n, data in members + [(COMBOS, changed), (PARAM, PARAM_BYTES)]:
            info = zipfile.ZipInfo(n, (2026, 10, 16, 0, 0, 0))
            info.compress_type = zipfile.ZIP_DEFLATED
            info.external_attr = 0o100644 << 16
            z.writestr(info, data)
    actual = dict(archive_members(out.read_bytes()))
    if any(actual[n] != data for n, data in members):
        raise ValueError("base companion member changed")
    manifest = {"schema": 1, "status": "OPTIONAL_NOT_HEADSET_ACCEPTED_NOT_INSTALLED",
        "scope": "NATIVE_SHARED_EQUIPMENT_RECIPES_NOT_PLAYER_ONLY",
        "semantics": "lead-in -> existing learned special finish, NOT free special opener",
        "retail_tables_sha256": TABLES_SHA, "base_companion_sha256": COMPANION_SHA,
        "output_sha256": sha(out.read_bytes()), "preserved_base_members": 26,
        "changed_recipe_count": len(changes), "recipe_count": 51, "changes": changes,
        "members": [{"name": n, "sha256": sha(b)} for n, b in actual.items()],
        "limits": ["Native perk, equipment, timing and sync admission remain required.",
          "No guarantee of DIRECT combo advancement; native receipt boundary remains separate.",
          "No forced guard opening, armor penetration, player-only scope or R3 binding is added.",
          "ComboMinSteps is a shared native RPG parameter; this optional variant can affect NPC combos.",
          "Do not load alongside another copy of KCD2VR or another combo recipe replacement."]}
    out.with_suffix(out.suffix + ".json").write_text(json.dumps(manifest, indent=2) + "\n")
    return manifest

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--tables", type=Path, required=True)
    p.add_argument("--companion", type=Path, default=ROOT / "companion/reference/C32C1/KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak")
    p.add_argument("--out", type=Path, default=ROOT / "companion/variants/native_short_combos32/KCD2VR_native_short_combos32.pak.disabled")
    a = p.parse_args()
    try:
        result = build(a.tables, a.companion, a.out)
    except (OSError, ValueError, KeyError, zipfile.BadZipFile, ET.ParseError) as e:
        p.exit(1, str(e) + "\n")
    print(json.dumps({k: result[k] for k in ("status", "output_sha256", "preserved_base_members", "changed_recipe_count")}, indent=2))

if __name__ == "__main__":
    main()
