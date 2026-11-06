import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
s=read_source(r/'src/vrik/manual_reload_attachment19c4r4.inl')
s=s[s.index('static bool ManualBoundEntity19c4r4('):]
(r/'tests/manual_reload_attachment19c4r4.inc').write_text(s)
(r/'tests/manual_reload_attachment19c4r4_provenance.json').write_text(json.dumps({'source':'src/vrik/manual_reload_attachment19c4r4.inl','sha256':hashlib.sha256(s.encode()).hexdigest(),'boundaries':'Complete attachment/body/model/nock readers unchanged; only two native attachment lookup leaves plus safe memory/entity providers doubled'},indent=2)+'\n')
