import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source, read_source_bytes
from pathlib import Path
import json,hashlib
r=Path(__file__).resolve().parents[1]
def block(text,needle):
 s=text.index(needle);b=text.index('{',s);n=1;i=b+1
 while n:
  if text[i]=='{':n+=1
  if text[i]=='}':n-=1
  i+=1
 return text[s:i]
h=read_source(r/'src/frik_pose_probe.h');f=read_source(r/'src/frik_pose_probe.cpp');m=read_source(r/'src/mod.cpp')
ctx=block(h,'struct GameStateContext')+';'
publish=block(f,'void PublishGameStateContext(')
s=m.index('            gs.native_reading=');e=m.index('// Presentation only, never a native Reading owner.',s)
e=m.index('\n',e)
producer=m[s:e]
seam=read_source(r/'src/vrik/workstation_pose_context19c4r2_publish.inl')
publish=publish.replace('#include "vrik/workstation_pose_context19c4r2_publish.inl"',seam)
out=ctx+'\nvoid ProduceContext57(GameStateContext& gs,bool hud_hidden57){\n'+producer+'\n}\n'+publish+'\n'
(r/'tests/pose_policy57.inc').write_text(out)
(r/'tests/pose_policy57_provenance.json').write_text(json.dumps({'source_files':{p:hashlib.sha256(read_source_bytes(r/p)).hexdigest() for p in ['src/mod.cpp','src/frik_pose_probe.h','src/frik_pose_probe.cpp','src/vrik/workstation_pose_context19c4r2_publish.inl']},'extracted_sha256':hashlib.sha256(out.encode()).hexdigest()},indent=2)+'\n')
n=read_source(r/'src/diagnostics/native_hit_probe134.inl')
a=n.index('#define Allow(');b=n.index('// V66.146R1 PERFORMANCE:',a)
(r/'tests/receipt_capture57.inc').write_text(n[a:b])
