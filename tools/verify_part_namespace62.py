"""Verify the production read-only C62 namespace observer's exact retail pins."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source, read_source_bytes
import argparse, hashlib, json, re, struct
from pathlib import Path
from native_research112 import PE
ap=argparse.ArgumentParser();ap.add_argument("dll",type=Path);ap.add_argument("--output",type=Path)
a=ap.parse_args();p=PE(a.dll)
sha=hashlib.sha256(p.data).hexdigest()
assert sha=="bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7" and p.stamp==0x6A350E20,"Unsupported retail"
root=Path(__file__).resolve().parents[1]
header=root/"src/vrik/part_namespace62.h"
text=read_source(header, encoding="utf-8");checks=[]
for name,rva in [("characterEntry",0x6411AC),("poseGetter",0x1AA6BC0),("physicsGetter",0x1A77FD0),("countGetter",0x1AAF020)]:
    found=re.search(r"\b"+name+r"\{\{([^}]+)\}\}",text);assert found,name
    expected=bytes(int(v.strip(),0) for v in found.group(1).split(","))
    actual=p.read(rva,len(expected))
    checks.append(dict(name=name,rva=hex(rva),expected=expected.hex(),actual=actual.hex(),match=actual==expected))
for table,slot,target in [(0x4072C78,0x328,0x6411AC),(0x40602F8,0x38,0x1AA6BC0),(0x3A71028,0x30,0x1A77FD0),(0x406B930,8,0x1AAF020)]:
    actual=struct.unpack("<Q",p.read(table+slot,8))[0]-p.base
    checks.append(dict(table=hex(table),slot=hex(slot),expected=hex(target),actual=hex(actual),match=actual==target))
# Native AddImpact's embedded-physics -> character -> default skeleton chain.
for rva,expected in [(0x37C619B,"488b8110010000"),(0x37C61A2,"488bb8700a0000")]:
    actual=p.read(rva,len(bytes.fromhex(expected))).hex()
    checks.append(dict(rva=hex(rva),expected=expected,actual=actual,match=actual==expected))
result=dict(sha256=sha,timestamp=hex(p.stamp),header_sha256=hashlib.sha256(read_source_bytes(header)).hexdigest(),checks=checks,
    native_calls=0,native_writes=0,authority="NONE",all_match=all(v["match"] for v in checks))
if a.output:a.output.write_text(json.dumps(result,indent=2)+"\n",encoding="utf-8")
print(f"C62 production namespace observer retail: {sum(v['match'] for v in checks)}/{len(checks)} PASS")
raise SystemExit(0 if result["all_match"] else 1)
