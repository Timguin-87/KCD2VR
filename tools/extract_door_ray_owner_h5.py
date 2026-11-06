"""Copy the two actual legacy HUD cleanup statements for owner arbitration tests."""
from pathlib import Path
import hashlib
import json
import re
import sys

root = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(root / "tools"))
from source_text import read_source

path = root / "src/subsystems/hud_ui_controls/host.inl"
source = read_source(path)
pattern = r"if\(frik_pose_probe::MiscInteractionDiagMode\(\)<2\) interaction_alignment::[^\n]+"
matches = list(re.finditer(pattern, source))
assert len(matches) == 2, "Audit both legacy cleanup branches when their source changes"
output = ["// Verbatim current production cleanup statements; surrounding HUD state is an explicit fixture."]
metadata = []
for index, match in enumerate(matches):
    text = match.group()
    output.append(f"static void LegacyCleanupH5_{index}() noexcept {{ {text} }}")
    metadata.append({"source": str(path.relative_to(root)).replace("\\", "/"),
                     "line": source[:match.start()].count("\n") + 1,
                     "sha256": hashlib.sha256(text.encode()).hexdigest(),
                     "statement": text})
(root / "tests/generated_door_ray_owner_h5.inc").write_text("\n".join(output) + "\n")
(root / "tests/door_ray_owner_h5_extraction.json").write_text(json.dumps(metadata, indent=2) + "\n")
