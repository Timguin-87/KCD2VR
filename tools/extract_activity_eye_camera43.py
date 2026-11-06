"""Extract the actual activity-eye adapter, including its cache transaction."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source_bytes
from pathlib import Path
import hashlib
import json

root=Path(__file__).resolve().parents[1]
source=root/'src/camera_hook.cpp'
raw=read_source_bytes(source)
text=raw.decode('utf-8').replace('\r\n','\n')
start=text.index('core::SnapshotMailbox<ae19::Neutral> g_activity_neutral59c19;')
end=text.index('\nnamespace dsm38=dialogue_smooth38;',start)
body=text[start:end].rstrip()+'\n'
assert body.count('bool ActivityEyeBase59c19(')==1
(root/'tests/generated_activity_eye_camera43.inc').write_text(
    '// Generated from actual production camera adapter; do not edit.\n'+body,encoding='utf-8')
(root/'tests/activity_eye_camera43_extraction.json').write_text(json.dumps({
    'source':'src/camera_hook.cpp',
    'source_sha256':hashlib.sha256(raw).hexdigest(),
    'first_line':text[:start].count('\n')+1,
    'extracted_sha256':hashlib.sha256(body.encode()).hexdigest(),
    'scope':'Verbatim activity eye/cache function; clock, current state/body and diagnostic sink mocked; real SnapshotMailbox',
    'native_execution':False,
},indent=2)+'\n',encoding='utf-8')
print('activity_eye_camera43: extracted actual pre-HMD adapter and transaction guard')
