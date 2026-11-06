#!/usr/bin/env python3
"""Verify delivered files, independently of the compiler. Intentional edits require review."""
from pathlib import Path
import hashlib,json,sys
root=Path(__file__).resolve().parents[1]
manifest=root/'MANIFEST_SHA256.json'
if not manifest.is_file():
    print('FAIL: release manifest missing');sys.exit(1)
entries=json.loads(manifest.read_text())['files'];bad=[]
for name,wanted in entries.items():
    p=root/name
    if not p.is_file():bad.append((name,'missing'))
    elif hashlib.sha256(p.read_bytes()).hexdigest()!=wanted:bad.append((name,'changed'))
for name,status in bad:print(status.upper(),name)
print(f'{len(entries)-len(bad)}/{len(entries)} packaged files match. Extra build outputs are not checked.')
sys.exit(bool(bad))
