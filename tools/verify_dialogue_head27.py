#!/usr/bin/env python3
"""Exact retail head-presentation observer boundaries; no native execution."""
import argparse,hashlib,json
from pathlib import Path
from verify_native_grasp13 import PE,EXPECTED_SHA,EXPECTED_TS
REGIONS=[('0x82ec48', 147, 'd118a3dd6fb845ae150a5b43516b7f9517f9a3344fc6dd3d5e567ff94b817d22'), ('0x1351fec', 210, '30e24117116c854f15383d1fa72c86a2e6883115efdd05f1d00774b152b2fe8f'), ('0x9e5977', 219, '458913440c27748cc6616c1e1b088775019be4ca8943b0ddf1f038e073d377ba')]
def main():
    p=argparse.ArgumentParser();p.add_argument('--dll',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
    pe=PE(a.dll);checks=[]
    def add(name,actual,expected):checks.append(dict(name=name,actual=actual,expected=expected,passed=actual==expected))
    add('SHA256',hashlib.sha256(pe.data).hexdigest(),EXPECTED_SHA);add('timestamp',pe.timestamp,EXPECTED_TS)
    if not all(c['passed'] for c in checks):
        a.out.write_text(json.dumps(dict(checks=checks,all_passed=False),indent=2));return 1
    for at,size,want in REGIONS:add('bounded native region '+at,hashlib.sha256(pe.at(int(at,16),size)).hexdigest(),want)
    add('FirstPerson virtual+20',hex(pe.q(0x3A5A0F8+0x20)-pe.base),'0x838ae0')
    add('Dialog virtual+20',hex(pe.q(0x3A5A210+0x20)-pe.base),'0x41a6a0')
    result=dict(dll=str(a.dll),native_execution=False,checks=checks,passed=sum(c['passed'] for c in checks),total=len(checks),all_passed=all(c['passed'] for c in checks))
    a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(result,indent=2)+'\n')
    print('dialogue head observer retail: %s/%s %s'%(result['passed'],result['total'],'PASS' if result['all_passed'] else 'FAIL'))
    return 0 if result['all_passed'] else 1
if __name__=='__main__':raise SystemExit(main())
