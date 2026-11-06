#!/usr/bin/env python3
"""Read-only exact-retail extension for T1 weapon Drop and inline-only spin.

No process, native call, injection or installed-file write. The verifier proves
bytes/slots, not in-headset behavior or completion of a queued native consumer.
"""
import argparse,json,struct
from pathlib import Path
from verify_native_grasp13 import PE,verify

def run(dll):
    report=verify(dll);pe=PE(dll);checks=report['checks']
    def add(name,actual,expected):
        checks.append(dict(name='T1 '+name,actual=actual,expected=expected,passed=actual==expected))
    def call(name,site,target):
        b=pe.at(site,5)
        actual=site+5+struct.unpack('<i',b[1:])[0] if b[0]==0xe8 else -1
        add(name,hex(actual),hex(target))
    for name,vf,slot,target in [('world inventory holder type',0x3A83DC0,8,0x1A74A40),
        ('rigid status',0x3A48DC0,0x30,0x8A6C90),('rigid Action',0x3A48DC0,0x38,0xAC0D18)]:
        add(name,hex(pe.q(vf+slot)-pe.base),hex(target))
    col=pe.q(0x3A83DC0-8)-pe.base;td=struct.unpack('<I',pe.at(col+12,4))[0]
    add('world inventory exact RTTI',pe.cstr(td+16),'.?AVC_WorldInventory@entitymodule@wh@@')
    add('whole-item count and world inventory offset',pe.at(0x10859C8,10).hex(),'4533c0498d8f70001000')
    add('player no-holder predicate',pe.at(0x10868C3,7).hex(),'48399a98000000')
    add('native queued consumer physics call',pe.at(0x1F229C0,6).hex(),'ff9068010000')
    add('existing linked world entity shortcut',pe.at(0x7081A0,16).hex(),'498b86880000004885c00f854b020000')
    for name,site,target in [('drop inventory-route check',0x10859C3,0x10868B4),
        ('player cold inventory transfer',0x230E5D5,0x8D534C),
        ('native hand-release notice',0x10859FC,0x1F23660),
        ('consumer world-link resolve',0x1F22961,0x70814C),
        ('dispatcher inline attempt',0x1F1EB9D,0x1F1F040),
        ('inline consumer completion',0x1F1F08C,0x1F228D0)]:call(name,site,target)
    add('Action entry',pe.at(0xAC0D18,16).hex(),'488bc448895818555657415441554156')
    add('Dynamics entry',pe.at(0x8A6C90,16).hex(),'488bc4555356574156488d68c84881ec')
    report.update(schema=2,scope='T1 base Drop pins retained; T2 queued completion/spin verified separately by supply proof and actual owner fixture',
        passed=sum(c['passed'] for c in checks),total=len(checks),all_passed=all(c['passed'] for c in checks))
    return report

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--dll',required=True,type=Path);ap.add_argument('--out',required=True,type=Path);a=ap.parse_args()
    r=run(a.dll);a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_bytes((json.dumps(r,indent=2)+'\n').encode('utf-8'))
    print(json.dumps({k:v for k,v in r.items() if k!='checks'},indent=2))
    for c in r['checks']:
        if not c['passed']:print('FAIL',c)
    return 0 if r['all_passed'] else 1
if __name__=='__main__':raise SystemExit(main())
