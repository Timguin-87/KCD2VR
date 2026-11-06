import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import sys
root=Path(__file__).resolve().parents[1]
s=read_source(root/"src/vrik/contact_fx178r12.inl")
a=s.index("struct EnvironmentFxEvent178r22c1 ")
body=s[a:]
assert "static void PublishEnvironmentFx178r22c1" in body and "static void ConsumeEnvironmentFx178r22c1" in body
out=Path(sys.argv[1]);out.parent.mkdir(parents=True,exist_ok=True);out.write_bytes(body.encode())
print("Extracted actual C58 completed-environment-event producer/consumer")
