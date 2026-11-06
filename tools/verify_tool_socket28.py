#!/usr/bin/env python3
"""Exact-retail socket pending/current transfer and C28 observation-only seams.
Read-only; no native code execution or SDK slot transplantation.
"""
import argparse, hashlib, json
from pathlib import Path
from verify_native_grasp13 import PE, EXPECTED_SHA, EXPECTED_TS

def verify(path):
    p=PE(Path(path));checks=[]
    def check(name, actual, expected):
        checks.append(dict(name=name,actual=actual,expected=expected,passed=actual==expected))
    check('DLL SHA256',hashlib.sha256(p.data).hexdigest(),EXPECTED_SHA)
    check('DLL timestamp',p.timestamp,EXPECTED_TS)
    for name,at,hexbytes in [
        ('pending relative setter',0xA9F904,'0f10020f1181f4000000'),
        ('relative setter dirty',0xA9F924,'b8010000008881f200000086417ac3'),
        ('pending absolute setter',0xAFFC18,'0f10020f118110010000'),
        ('absolute setter dirty',0xAFFC38,'b801000000c681f20000000086417ac3'),
        ('native dirty test',0x20E82A7,'0fb6487a9084c9745f'),
        ('load pending relative',0x20E82CB,'0f1080f4000000'),
        ('write CURRENT relative',0x20E82E0,'0f11809c000000'),
        ('clear pending dirty after copies',0x20E830C,'86487a'),
        ('current relative getter',0x1A72860,'488d819c000000c3'),
        ('current joint getter',0x1AAF3C0,'0fb74178c3'),
        ('native object update obtains current world',0x4238E5,'488b16488bce4c8b4270488d54244041ffd0'),
        ('native world getter uses cached absolute',0x4239DC,'f3410f59448940'),
    ]:
        b=bytes.fromhex(hexbytes);check(name,p.at(at,len(b)).hex(),b.hex())
    for name,slot,dest in [('relative slot',0x60,0x1A72860),('joint slot',0xC8,0x1AAF3C0),('world slot',0x70,0x423974)]:
        check(name,p.q(0x3A657C0+slot)-p.base,dest)
    return dict(passed=all(c['passed']for c in checks),checks=checks,scope='static only; original-joint proof and runtime lifetime tested separately')
if __name__=='__main__':
    a=argparse.ArgumentParser();a.add_argument('--dll',required=True);a.add_argument('--out',required=True);args=a.parse_args()
    result=verify(args.dll);Path(args.out).parent.mkdir(parents=True,exist_ok=True);Path(args.out).write_text(json.dumps(result,indent=2))
    print(str(sum(c['passed']for c in result['checks']))+'/'+str(len(result['checks']))+' exact-retail tool socket checks '+('PASS'if result['passed']else'FAIL'))
    raise SystemExit(0 if result['passed'] else 1)
