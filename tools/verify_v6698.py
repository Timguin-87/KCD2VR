#!/usr/bin/env python3
"""Read-only pinned-retail fingerprint verification; not runtime ABI validation."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse, hashlib, json, re, struct
from pathlib import Path

def run(dll: Path, root: Path) -> dict:
    d=dll.read_bytes()
    if len(d)<256 or d[:2]!=b'MZ':raise ValueError('Not a PE image')
    pe=struct.unpack_from('<I',d,0x3c)[0]
    if d[pe:pe+4]!=b'PE\0\0':raise ValueError('Missing PE signature')
    machine,n=struct.unpack_from('<HH',d,pe+4)
    timestamp=struct.unpack_from('<I',d,pe+8)[0]
    size=struct.unpack_from('<H',d,pe+20)[0]
    base=struct.unpack_from('<Q',d,pe+24+24)[0]
    if machine!=0x8664 or not 0<n<96:raise ValueError('Expected AMD64 image with bounded sections')
    sections=[]
    for k in range(n):
        o=pe+24+size+k*40
        vs,va,rs,rp=struct.unpack_from('<IIII',d,o+8)
        if rp+rs>len(d):raise ValueError('Section outside file')
        sections.append((va,rs,rp))
    def read(rva,count):
        for va,rs,rp in sections:
            if va<=rva and rva+count<=va+rs:return d[rp+rva-va:rp+rva-va+count]
        raise ValueError(f'RVA {rva:x} outside raw sections')
    checks=[]
    def check(name,valid,**extra):checks.append(dict(name=name,passed=bool(valid),**extra))
    sha=hashlib.sha256(d).hexdigest()
    check('exact retail SHA-256',sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7',actual=sha)
    check('PE timestamp',timestamp==0x6A350E20,actual=hex(timestamp))
    check('preferred image base',base==0x180000000,actual=hex(base))
    text=read_source(root/'src/vrik/physical_native_contract.h')
    rows=re.findall(r'\{(0x[\dA-Fa-f]+), \{([^}]+)\}, "([^"]+)"\}',text)
    check('fingerprint coverage',len(rows)==15,actual=len(rows))
    for rva,raw,purpose in rows:
        expected=bytes(int(v,16) for v in re.findall(r'0x[\dA-Fa-f]+',raw))
        got=read(int(rva,16),16)
        check(purpose,got==expected,rva=rva,actual=got.hex(),expected=expected.hex())
    # Confirm exactly the interface/subobject vslots used, not similarly named root slots.
    for name,vt,slot,fn in [('render interface local bbox',0x3A4A268,0x90,0x63F884),('render ROOT same slot is NOT bbox',0x3A49FD8,0x90,0x5DD810),('player input OnAction',0x3EC99A8,8,0x8E5324)]:
        got=struct.unpack('<Q',read(vt+slot,8))[0]-base
        check(name,got==fn,vtable=hex(vt),slot=hex(slot),actual=hex(got),expected=hex(fn))
    return dict(build='V66.98 PHYSICAL WEAPONS',scope='Static bytes/vslots only. Native execution, runtime owner values, Windows SEH and headset behavior NOT tested.',checks=checks,passed=all(x['passed'] for x in checks))

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--dll',type=Path,required=True);p.add_argument('--report',type=Path)
    a=p.parse_args()
    try:report=run(a.dll,Path(__file__).resolve().parents[1])
    except (OSError,ValueError,struct.error) as exc:p.exit(2,f'Verification failed: {exc}\n')
    text=json.dumps(report,indent=2)+'\n'
    if a.report:a.report.parent.mkdir(parents=True,exist_ok=True);a.report.write_text(text)
    print(text);return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
