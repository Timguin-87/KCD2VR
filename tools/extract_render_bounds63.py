"""Extract actual renderer bounds decision/detour; fixture supplies native leaves."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source_bytes
from pathlib import Path
import hashlib, json
root=Path(__file__).resolve().parents[1]
src=root/'src/vrik/render_bounds63.inl'
raw=read_source_bytes(src);s=raw.decode().replace('\r\n','\n')
lo=s.index('struct BoundsNote63 ')
hi=s.index('static void TraceHenryBounds63(',lo)
body=s[lo:hi]
(root/'tests/generated_render_bounds63.inc').write_text('// Generated verbatim production decision/detour; do not edit.\n'+body,encoding='utf-8')
life=s[s.index('static bool InstallHenryBounds63('):]
(root/'tests/generated_render_bounds63_lifecycle.inc').write_text('// Generated verbatim production install/shutdown; do not edit.\n'+life,encoding='utf-8')
(root/'tests/render_bounds63_provenance.json').write_text(json.dumps({
 'source':str(src.relative_to(root)), 'source_sha256':hashlib.sha256(raw).hexdigest(),
 'extracted_sha256':hashlib.sha256(body.encode()).hexdigest(), 'lifecycle_sha256':hashlib.sha256(life.encode()).hexdigest(),
 'scope':'Actual production SelectHenryBounds63 and original-first detour. WhBase/clock/return-address/native original and checked memory read are explicit fixture leaves; real mailbox/pure bounds logic.',
 'native_execution':False},indent=2)+'\n',encoding='utf-8')
print('render_bounds63: extracted actual renderer decision and original-first detour')
