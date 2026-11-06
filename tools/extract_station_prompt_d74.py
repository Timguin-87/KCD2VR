from pathlib import Path
import hashlib,os
root=Path(__file__).resolve().parents[1]
p=root/'src/subsystems/hud_ui_controls/runtime/flashui_probe.cpp'
s=p.read_text();start=s.index('void UpdateReadablePromptsD74(');i=s.index('{',start)+1;depth=1
while depth:
 if s[i]=='{':depth+=1
 elif s[i]=='}':depth-=1
 i+=1
out=root/'tests/generated/station_prompt_d74.inl';out.parent.mkdir(exist_ok=True)
t=out.with_suffix('.tmp');t.write_text('// Actual production owner SHA256 '+hashlib.sha256(p.read_bytes()).hexdigest()+'\n'+s[start:i]+'\n');os.replace(t,out)
