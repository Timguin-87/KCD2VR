#!/usr/bin/env python3
"""Extract the actual current readable-prompt owner for a fixture with explicit Flash leaves."""
from pathlib import Path
import hashlib, os
root=Path(__file__).resolve().parents[1]
p=root/'src/subsystems/hud_ui_controls/runtime/flashui_probe.cpp'
s=p.read_text();start=s.index('bool ApplyReadablePromptScale(');brace=s.index('{',start)
depth=1;i=brace+1
while depth:
    if s[i]=='{':depth+=1
    elif s[i]=='}':depth-=1
    i+=1
out=root/'tests/generated/item_prompt_owner_d69.inl';out.parent.mkdir(exist_ok=True)
body='// Generated from current flashui_probe.cpp SHA256 '+hashlib.sha256(p.read_bytes()).hexdigest()+'\n'+s[start:i]+'\n'
tmp=out.with_name(out.name+'.tmp');tmp.write_bytes(body.encode());os.replace(tmp,out)
print(out)
