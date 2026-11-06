import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
s=read_source(r/'src/vrik/staged_reload596c2.inl')
a=s.index('static bool ManualItemsRead596(');b=s.index('#include "manual_reload_attachment19c4r4.inl"',a)
s=s[a:b]
(r/'tests/reload_layout59c9.inc').write_text(s,newline='\n')
(r/'tests/reload_layout59c9_provenance.json').write_text(json.dumps({'source':'src/vrik/staged_reload596c2.inl','sha256':hashlib.sha256(s.encode()).hexdigest(),'native_leaves':'equipment/component/visual lookup mocked; whole read function unchanged'},indent=2))
