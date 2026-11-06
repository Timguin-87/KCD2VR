"""Extract the actual Door intent/drive adapter for native-leaf host fixtures."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source_bytes
from pathlib import Path
import hashlib
import json
import sys

root = Path(__file__).resolve().parents[1]
output = Path(sys.argv[1])
source = root / "src/vrik/door_hand_native66.inl"
tracking = root / "src/vrik/weapon_tracking.inl"
raw = read_source_bytes(source)
text = raw.decode("utf-8").replace("\r\n", "\n")
tracking_raw = read_source_bytes(tracking)
tracking_text = tracking_raw.decode("utf-8").replace("\r\n", "\n")

def function(s, start):
    lo = s.index(start)
    brace = s.index("{", lo)
    depth = 1
    i = brace + 1
    while depth and i < len(s):
        if s[i] == "{":
            depth += 1
        elif s[i] == "}":
            depth -= 1
        i += 1
    if depth:
        raise ValueError("unbalanced production function: " + start)
    return s[lo:i] + "\n"

definitions = text[text.index("namespace dp66="):text.index("// Synchronous native OnEnter")]
pieces = [function(tracking_text, "static QuatT28 HandWorld6694("), definitions]
for name in ("static bool DoorIntent66(", "static int DriveDoor66(", "static void DetachDoor66(", "static void RetireDoor66("):
    pieces.append(function(text, name))
body = "\n".join(pieces)
output.parent.mkdir(parents=True, exist_ok=True)
output.write_text("// Generated actual production Door intent/drive; do not edit.\n" + body, encoding="utf-8")
output.with_suffix(".json").write_text(json.dumps({
    "source": str(source.relative_to(root)),
    "source_sha256": hashlib.sha256(raw).hexdigest(),
    "tracking_source": str(tracking.relative_to(root)),
    "tracking_sha256": hashlib.sha256(tracking_raw).hexdigest(),
    "extracted_sha256": hashlib.sha256(body.encode("utf-8")).hexdigest(),
    "scope": "Actual HandWorld6694, DoorIntent66, DriveDoor66, DetachDoor66, RetireDoor66 and their POD/native-grip/mailbox definitions. Native completed packet read and clock are fixture leaves; pure path, pose and physical compose execute current production headers. The native grip is inert copied geometry, not a tracking lease.",
    "native_execution": False,
}, indent=2) + "\n", encoding="utf-8")
print("door_drive66: extracted actual production intent/drive")
