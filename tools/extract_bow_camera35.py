import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
s=read_source(r/'src/frik_pose_probe.cpp')
a=s.index('static bool BowCameraInput35(');b=s.index('void ObserveBowCamera126(',a)
body=s[a:b]
(r/'tests/generated_bow_camera35.inc').write_text('// Actual production bow camera producer and consumer; regenerate.\n'+body)
(r/'tests/bow_camera35_provenance.json').write_text(json.dumps({'source':'src/frik_pose_probe.cpp','sha256':hashlib.sha256(body.encode()).hexdigest(),'native_leaves':'None called; immutable frame mailbox and existing mode/epoch/ready atomics substituted'},indent=2)+'\n')
