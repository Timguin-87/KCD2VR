import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
s=read_source(r/'src/vrik/manual_reload_attachment19c4r4.inl')
body=s[s.index('static bool ManualBoundEntity19c4r4('):s.index('static bool ManualNockAttached19c4r4(')]
(r/'tests/manual_arrow_binding64.inc').write_text(body,encoding='utf-8')
(r/'tests/manual_arrow_binding64_provenance.json').write_text(json.dumps({'source':'src/vrik/manual_reload_attachment19c4r4.inl','sha256':hashlib.sha256(body.encode()).hexdigest(),'native_execution':False},indent=2)+'\n')
