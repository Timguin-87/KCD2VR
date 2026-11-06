"""Extract the actual native-release preflight for the physical-pull fixture."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source, read_source_bytes
from pathlib import Path
import hashlib
import json
import sys

root = Path(__file__).resolve().parents[1]
source = root / "src/vrik/physical_native.inl"
text = read_source(source)
start = text.index("static bool PhysBowReleaseReady175(")
end = text.index("// R30C1 compact causal ledger.", start)
body = text[start:end]
out = Path(sys.argv[1])
out.parent.mkdir(parents=True, exist_ok=True)
out.write_bytes(body.encode())
out.with_suffix(".json").write_text(json.dumps({
    "source": str(source), "sourceSHA256": hashlib.sha256(read_source_bytes(source)).hexdigest(),
    "extractedSHA256": hashlib.sha256(body.encode()).hexdigest(),
    "nativeExecution": "NONE; native charge acknowledgement is an explicit fixture input"
}, indent=2) + "\n")
print("Extracted actual PhysBowReleaseReady175")
