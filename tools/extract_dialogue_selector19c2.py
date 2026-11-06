"""Extract the current native dialogue-candidate detour into a host fixture."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import sys

root = Path(__file__).resolve().parents[1]
source = read_source(root / "src/activity_lens59c10r1.cpp", encoding="utf-8")
signature = "bool __fastcall DialogueCameraCandidate19c2(void* self) noexcept"
start = source.index(signature)
brace = source.index("{", start)
depth = 0
for end in range(brace, len(source)):
    if source[end] == "{":
        depth += 1
    elif source[end] == "}":
        depth -= 1
        if not depth:
            break
else:
    raise ValueError("Unclosed production dialogue selector")
out = Path(sys.argv[1])
out.parent.mkdir(parents=True, exist_ok=True)
out.write_text("// Generated from current production; never edit.\n" + source[start:end + 1] + "\n", encoding="utf-8")
