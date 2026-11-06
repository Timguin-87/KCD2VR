"""Extract the actual production drag adapter and unchanged native part picker."""
from pathlib import Path
import argparse
import hashlib
import json

parser = argparse.ArgumentParser()
parser.add_argument("output", type=Path)
parser.add_argument("--source-root", type=Path, default=Path(__file__).resolve().parents[1])
args = parser.parse_args()
args.output.mkdir(parents=True, exist_ok=True)
specs = (
    ("src/vrik/lab58.inl", "// ------------------------------------------------------------ C. corpse limb drag", "// D51: F9 selects the physics mode", "corpse_drag_native_d40.inc"),
    ("src/subsystems/gore/native_part_anchor.inl", "struct AnchorPart47 {", "// H18 successor recontact:", "corpse_drag_part_picker_d40.inc"),
)
receipts = []
for name, start, end, target in specs:
    source = (args.source_root / name).read_text(encoding="utf-8-sig")
    begin = source.index(start)
    actual = source[begin:source.index(end, begin)]
    (args.output / target).write_text(actual, encoding="utf-8")
    receipts.append({"source": name, "sha256": hashlib.sha256(actual.encode()).hexdigest(), "output": target})
(args.output / "corpse_drag_native_d40_sources.json").write_text(json.dumps({
    "source_root": str(args.source_root.resolve()),
    "scope": "Verbatim production native CG1/W5 adapter and native part-selection algorithm; engine/status/velocity leaves mocked by fixture.",
    "extractions": receipts,
}, indent=2) + "\n", encoding="utf-8")
