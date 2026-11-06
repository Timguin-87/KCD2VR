"""Regenerate the actual production PhysCollisionDetour host fixture; no engine claims."""
from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1]
p=root/'src/diagnostics/native_hit_probe134.inl';s=p.read_text()
def extract(name):
 start=s.index(name);brace=s.index('{',start);depth=0
 for end in range(brace,len(s)):
  if s[end]=='{':depth+=1
  elif s[end]=='}':
   depth-=1
   if depth==0:return s[start:end+1]
 raise RuntimeError('Unterminated detour')
output=root/'tests/h24_collision';output.mkdir(exist_ok=True)
(output/'actual_detour_h24.inc').write_text(extract('static int __fastcall PhysCollisionProduceDetourH24(')+'\n'+extract('int PhysCollisionDetour(')+'\n')
(output/'PROVENANCE.json').write_text(json.dumps({'source':str(p.relative_to(root)),'source_sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'functions':['PhysCollisionProduceDetourH24','PhysCollisionDetour'],'native_leaves':'Original callbacks/retail event/clock doubled; exact production wrapper bodies exercised'},indent=2))
