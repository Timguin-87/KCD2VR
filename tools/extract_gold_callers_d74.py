"""Extract actual diagnostic display branches; native logic is not substituted."""
from pathlib import Path
import hashlib,os
root=Path(__file__).resolve().parents[1]
def read(rel):
 p=root/rel;return p.read_text(),hashlib.sha256(p.read_bytes()).hexdigest()
def block(s,start):
 i=s.index('{',start)+1;depth=1
 while depth:
  if s[i]=='{':depth+=1
  elif s[i]=='}':depth-=1
  i+=1
 return s[start:i]
s,h=read('src/vrik/native_input117.inl');at=s.index('        char text[96]{};std::snprintf(text,sizeof(text),"combo ');start=s.rfind('    if(',0,at)
out='// Actual combo display branch SHA256 '+h+'\nstatic void ComboGoldD74(std::uint64_t now){\n'+block(s,start)+'\n}\n'
s,h=read('src/subsystems/throw_drop/holster_gestures.inl');start=s.index('        if(reachedZoneR9&&')
out+='// Actual holster display branch SHA256 '+h+'\nstatic void HolsterGoldD74(bool reachedZoneR9,std::uint64_t now){\n'+block(s,start)+'\n}\n'
s,h=read('src/vrik/semi_workstations46.h');start=s.index('    void emitAttemptW4(');end=s.index('    template<class Pad>',start);body=s[start:end]
first=next(x for x in body.splitlines() if 'if(!diagnostics::capture168::active(' in x)
last=next(x for x in body.splitlines() if 'publishDiagnosticD74(' in x)
out+='// Actual F5 producer gate + display publication SHA256 '+h+'\nstatic void CraftGoldD74(bool visible,std::uint64_t now){\n'+first+'\n'+last+'\n}\n'
p=root/'tests/generated/gold_callers_d74.inl';p.parent.mkdir(exist_ok=True);t=p.with_suffix('.tmp');t.write_bytes(out.encode());os.replace(t,p)
