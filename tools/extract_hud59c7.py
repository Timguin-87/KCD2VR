#!/usr/bin/env python3
"""Extract complete production HUD functions for explicit native/XR/GPU-spy tests."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import json,hashlib
r=Path(__file__).resolve().parents[1];meta=[]
for output,file,start,end in [
 ('generated_hud_copy59c7.inc','src/hud_layer_v6664.cpp','XrHudFrame XrPrepare(', 'ID3D12Resource* AcquirePublishedForBlit'),
 ('generated_hud_trace59c7.inc','src/flashui_probe.cpp','void TraceHudVisibility57c1(', 'void SetHudSemanticOwner54'),
 ('generated_hud_elements59c7.inc','src/flashui_probe.cpp','bool ApplyUniformHudElements171(', 'void UpdateHudVitals59c7'),
 ('generated_hud_parent59c7.inc','src/flashui_probe.cpp','bool ApplyWholeReadableHud44c2(', 'bool ApplyNativeCanvasLayout('),
]:
 s=read_source(r/file);a=s.index(start);b=s.index(end,a);t=s[a:b].rstrip()+"\n"
 (r/'tests'/output).write_text(t)
 meta.append(dict(source=file,output=output,line=s[:a].count('\n')+1,sha256=hashlib.sha256(t.encode()).hexdigest()))
(r/'tests/hud59c7_extraction.json').write_text(json.dumps(meta,indent=2)+'\n')
