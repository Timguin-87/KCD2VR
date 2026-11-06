"""Extract the actual pre-HMD dialogue eye callback and POD mailboxes for host tests."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source, read_source_bytes
from pathlib import Path
import hashlib,json

root=Path(__file__).resolve().parents[1]
source=root/'src/camera_hook.cpp'
text=read_source(source, encoding='utf-8')
start=text.index('core::SnapshotMailbox<ds::Policy> g_dialogue_policy59c10r2;')
end=text.index('// Keep mutex RAII outside functions containing MSVC __try blocks',start)
body=text[start:end].rstrip()+'\n'
target=root/'tests/generated_dialogue_camera19c2.inc'
target.write_text('// Generated from src/camera_hook.cpp; do not edit.\n'+body,encoding='utf-8')
(root/'tests/dialogue_camera19c2_extraction.json').write_text(json.dumps({
    'source':'src/camera_hook.cpp','source_sha256':hashlib.sha256(read_source_bytes(source)).hexdigest(),
    'start_line':text[:start].count('\n')+1,'extracted_sha256':hashlib.sha256(body.encode()).hexdigest(),
    'scope':'Actual DialogueEyeBase59c10r2 and its production POD mailboxes; native leaves explicitly mocked; no source camera pipeline replacement'},indent=2)+'\n',encoding='utf-8')
print('dialogue_camera19c2: extracted actual pre-HMD eye callback')
