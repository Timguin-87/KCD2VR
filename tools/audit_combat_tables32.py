#!/usr/bin/env python3
"""Extract exact retail native recipe/face-coverage evidence; never installs changes."""
import argparse
import hashlib
import json
from pathlib import Path
import xml.etree.ElementTree as E
import zipfile

from build_short_combos32 import TABLES_SHA, COMBOS, LINKS, SELECTED, key

def digest(b):
    return hashlib.sha256(b).hexdigest()

def audit(path):
    data = path.read_bytes()
    if digest(data) != TABLES_SHA:
        raise ValueError("exact supplied retail Tables SHA mismatch")
    names = [COMBOS, LINKS, "Libs/Tables/item/body_material2subpart.xml",
             "Libs/Tables/item/body_subpart.xml", "Libs/Tables/item/armor_archetype.xml",
             "Libs/Tables/item/armor_archetype2body_subpart.xml",
             "Libs/Tables/item/body_part.xml", "Libs/Tables/combat/combat_attack_type.xml",
             "Libs/Tables/rpg/rpg_damage_type.xml", "Libs/Tables/combat/combat_zone_distance.xml",
             "Libs/Tables/rpg/rpg_param.xml"]
    with zipfile.ZipFile(path) as z:
        members = {n: z.read(n) for n in names}
    roots = {n: E.fromstring(b) for n, b in members.items()}
    def rows(name, tag):
        return [r.attrib for r in roots[name].iter(tag)]
    material = rows(names[2], "body_material2subpart")
    body = {r["body_subpart_id"]: r for r in rows(names[3], "body_subpart")}
    archetypes = {r["armor_archetype_id"]: r["armor_archetype_name"] for r in rows(names[4], "armor_archetype")}
    coverage = rows(names[5], "armor_archetype2body_subpart")
    head = []
    for row in material:
        if row["race_id"] == "0" and body[row["body_subpart_id"]]["body_part_id"] == "1":
            head.append({"original_material": row, "body_subpart": body[row["body_subpart_id"]]})
    helms = [{"id": i, "name": archetypes[i], "coverage": [r for r in coverage if r["armor_archetype_id"] == i],
              "includes_face27": any(r["armor_archetype_id"] == i and r["body_subpart_id"] == "27" for r in coverage)}
             for i in map(str, range(36, 45))]
    combos = [{"recipe": r.attrib, "native_steps": [s.attrib for s in r.findall("CombatComboSteps/CombatComboStep")]}
              for r in roots[COMBOS].iter("CombatCombo") if key(r) in SELECTED]
    params = [r for r in rows(names[-1], "rpg_param") if any(t in r["rpg_param_key"].lower()
              for t in ("armor", "staminadeficithealth", "combo"))]
    return {"status": "EXACT_RETAIL_DATA_NOT_RUNTIME_OUTCOME", "tables_sha256": TABLES_SHA,
            "members": [{"name": n, "sha256": digest(b), "bytes": len(b)} for n, b in members.items()],
            "human_head_original_material_domain": head, "helmet_archetype_coverage": helms,
            "selected_learned_recipes": combos,
            "native_attack_types": rows(names[7], "combat_attack_type"),
            "native_damage_types": rows(names[8], "rpg_damage_type"),
            "human_body_damage_coefficients": [r for r in rows(names[6], "body_part") if r.get("race_id") == "0"],
            "raw_armor_stamina_combo_parameters": params,
            "limits": ["Surface index, original material, physics part, body subpart and equipment part are different domains.",
                       "Original material3 maps human face27; an unknown material is not a face hit.",
                       "Open helmets41/42 omit face27; full helmets43/44 include it. Do not bypass every helmet for head hits.",
                       "No native armor penetration formula or guaranteed20-30% plate chip is proven by these rows.",
                       "combat_zone_distance is guard-zone scoring, not world clinch initiation distance.",
                       "Existing technique/perk and native combo advancement remain required; this audit grants none."]}

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("tables", type=Path)
    p.add_argument("--out", type=Path, required=True)
    a = p.parse_args()
    try:
        result = audit(a.tables)
    except (OSError, ValueError, KeyError, zipfile.BadZipFile, E.ParseError) as e:
        p.exit(1, str(e) + "\n")
    a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps(result, indent=2) + "\n")
    print("PASS exact retail combat audit:11 member hashes,9 selected recipes, human head materials,9 helmet archetypes")

if __name__ == "__main__":
    main()
