#!/usr/bin/env python3
"""Read-only exact-retail own solver-body and full-volume boundary verification."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse,hashlib,json,re,struct
from verify_native_grasp13 import PE,EXPECTED_SHA,EXPECTED_TS
ROOT=Path(__file__).resolve().parents[1]
def verify(game):
    pe=PE(game);checks=[]
    def add(name,actual,expected):checks.append(dict(name=name,actual=actual,expected=expected,passed=actual==expected))
    add('full WHGame SHA256',hashlib.sha256(pe.data).hexdigest(),EXPECTED_SHA)
    add('timestamp',pe.timestamp,EXPECTED_TS)
    text=read_source(ROOT/'src/native_weapon_body_t4_pins.h')
    sites=re.findall(r'\{"(\w+)",0x([0-9A-F]+)ull,\{([^}]+)\},(\d+)\}',text)
    assert sites
    for name,rva,body,size in sites:
        expected=bytes(int(b,16) for b in re.findall(r'0x([0-9A-F]{2})',body))
        assert len(expected)==int(size)
        add(name,pe.at(int(rva,16),len(expected)).hex(),expected.hex())
    for name,vt,slot,target in [
        ('world volume query',0x407FC80,0x1C8,0x8115BC),
        ('world create',0x407FC80,0x88,0x8463B8),
        ('world destroy',0x407FC80,0x98,0x846E10),
        ('rigid AddGeometry',0x3A48DC0,0x40,0x526194),
        ('rigid RemoveGeometry void',0x3A48DC0,0x48,0x38FEE3C),
        ('capsule Release',0x3A565B0,0x18,0x97CA04),
        ('geometry primitive',0x407FFC0,0x28,0x8102E0),
        ('geometry Register',0x407FFC0,0x38,0x8100EC),
        ('geometry Unregister',0x407FFC0,0x48,0x84A954)]:
        add(name,pe.q(vt+slot)-pe.base,target)
    for name,rva,expected in [
        ('native request sizeE8',0xAD2479,'41b8e8000000'),
        ('native query dispatch',0xAD2500,'ff90c8010000'),
        ('native query lock release',0xAD2506,'8b4df8488b4500f7d9f00108'),
        ('default zero-sweep overlap',0x812944,'66c785420400000101'),
        ('capsule default initializer',0x812AD2,'e831caffff'),
        ('positive overlap result1',0x812A55,'48b8000000000000f03f'),
        ('params26 third AND field',0x850ADC,'418b46182187f4000000'),
        ('params26 third OR field',0x850AEC,'410b460c8987f4000000')]:
        add(name,pe.at(rva,len(bytes.fromhex(expected))).hex(),expected)
    add('living class name',pe.cstr(0x3C0BA30),'collision_class_living')
    # No guessed field omitted from the params26 payload: native reads+18.
    source=read_source(ROOT/'src/native_weapon_body_t4.cpp')
    add('production params26 payload28',bool(re.search(r'static_assert\(sizeof\(CollisionClass\)==28\)',source)),True)
    add('production private foreign kind fitsint16',int(re.search(r'foreignType=0x([0-9A-F]+)',source)[1],16)<32768,True)
    return dict(passed=all(c['passed'] for c in checks),checks=checks,
        scope='Static exact bytes/vtables/native payload schema only; no WHGame execution or headset claim')
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('game',type=Path);p.add_argument('--out',type=Path);a=p.parse_args()
    report=verify(a.game)
    if a.out:a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2)+'\n')
    print(('PASS' if report['passed'] else 'FAIL'),sum(c['passed'] for c in report['checks']),'/',len(report['checks']))
    for c in report['checks']:
        if not c['passed']:print(c)
    raise SystemExit(0 if report['passed'] else 1)
