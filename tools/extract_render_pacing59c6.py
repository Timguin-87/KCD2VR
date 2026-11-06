#!/usr/bin/env python3
"""Extract entire production handoff decision and read-only pair identity peek.
D3D copy/runtime/thread leaves are replaced only in the test harness.
"""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import json,hashlib
r=Path(__file__).resolve().parents[1];parts=[];meta=[]
for name,start,end in [('src/device_hook.cpp','bool DeviceHook::ReadBridgePairIdentity59c6','void DeviceHook::MarkBridgePairConsumed'),('src/xr_manager.cpp','void XrManager::OnRenderThreadTick','bool XrManager::QueueRenderThreadDiagnosticWork')]:
 s=read_source(r/name);a=s.index(start);b=s.index(end,a);t=s[a:b];parts.append(t);meta.append({'source':name,'firstLine':s[:a].count('\n')+1,'sha256':hashlib.sha256(t.encode()).hexdigest()})
(r/'tests/generated_render_pacing59c6.inc').write_text('\n'.join(parts))
(r/'tests/render_pacing59c6_extraction.json').write_text(json.dumps(meta,indent=2)+'\n')
