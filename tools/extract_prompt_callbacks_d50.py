#!/usr/bin/env python3
"""Extract actual prompt callbacks; tests replace native IO, not their ordering."""
import argparse
import hashlib
import json
from pathlib import Path

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1])
p.add_argument('--out',type=Path,required=True)
a=p.parse_args();root=a.source.resolve();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
relative='src/subsystems/hud_ui_controls/runtime/prompt_device_lock.cpp'
raw=(root/relative).read_bytes();text=raw.decode().replace('\r\n','\n')
blocks=[]
for signature in ['bool PromptCaptureActiveD50() noexcept {','void ReassertPromptFamilyD49(int forced) noexcept {','void __fastcall Detour(void* self,int requested) {']:
    assert text.count(signature)==1,signature
    start=text.index(signature);opening=text.index('{',start);depth=1;end=opening+1
    while depth:
        if text[end]=='{':depth+=1
        elif text[end]=='}':depth-=1
        end+=1
    blocks.append(text[start:end])
snippet='\n\n'.join(blocks)+'\n'
(out/'prompt_callbacks_d50.inc').write_text(snippet)
(out/'PROVENANCE.json').write_text(json.dumps({'source':relative,'sha256':hashlib.sha256(raw).hexdigest(),'extracted_sha256':hashlib.sha256(snippet.encode()).hexdigest(),'native_io':'explicit fixture leaves; no native execution'},indent=2)+'\n')
