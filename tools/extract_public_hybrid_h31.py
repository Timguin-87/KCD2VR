#!/usr/bin/env python3
"""Extract actual public pose-policy wiring for its complete-TU host fixture."""
from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1]
source=root/"src/subsystems/base/pose_physical_executor.inl"
text=source.read_text()
prefixes=("        pi.coherentMelee119=", "        pi.physicalCombat177=", "        ph::select_forward_motion36(", "        pi.intent59c16=")
lines=[]
for prefix in prefixes:
    found=[line for line in text.splitlines() if line.startswith(prefix)]
    if len(found)!=1:raise RuntimeError(f"expected exactly one actual adapter statement: {prefix}, found {len(found)}")
    lines.extend(found)
output=root/"tests/generated_public_hybrid_h31.inc"
output.write_text("// Generated from the actual pose adapter; regenerate, do not edit.\n"+"\n".join(lines)+"\n")
(root/"tests/public_hybrid_h31_provenance.json").write_text(json.dumps({"source":str(source.relative_to(root)),"sourceSha256":hashlib.sha256(source.read_bytes()).hexdigest(),"extractedSha256":hashlib.sha256(output.read_bytes()).hexdigest(),"prefixes":prefixes},indent=2)+"\n")
