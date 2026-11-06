#!/usr/bin/env python3
"""Read exact retail PE only; validate T4 native held source/collider evidence."""
from pathlib import Path
import argparse,hashlib,json
from verify_native_grasp13 import PE,EXPECTED_SHA,EXPECTED_TS

def verify(dll):
    pe=PE(Path(dll));assert hashlib.sha256(pe.data).hexdigest()==EXPECTED_SHA and pe.timestamp==EXPECTED_TS
    checks=[]
    def check(rva,wanted,label):
        actual=pe.at(rva,len(bytes.fromhex(wanted))).hex();checks.append(dict(rva=hex(rva),expected=wanted,actual=actual,label=label,passed=actual==wanted))
    slots=[(0x4072C78,0x250,0x5D70C4),(0x4072C78,0x398,0xABDFF8),(0x4072C78,0x168,0x509810),
           (0x3AC3478,0xB0,0x602360),(0x3AC3478,0xC0,0xABE028),(0x3AC3478,0xC8,0x509804),
           (0x3EC85E0,0x168,0x1F1BF20),(0x3EC85E0,0xB0,0x2A8B978),(0x3EC85A8,8,0xA5619C),
           (0x3AA4B98,0x610,0x1A81DD0)]
    for table,slot,target in slots:
        got=pe.q(table+slot)-pe.base;checks.append(dict(rva=hex(table+slot),expected=hex(target),actual=hex(got),label='exact native vft slot',passed=got==target))
    for rva,wanted,label in [
        (0x41FC4F,'4c8b4930','CreatePhysicalEntity foreign pointer is proxy+30 CEntity'),
        (0x41FC63,'ba02000000','native foreign kind2'),
        (0x41FCCC,'89542420','foreign kind is fifth x64 argument'),
        (0x41FCD0,'8b17','second argument is native requested body type'),
        (0x41FCD2,'ff9088000000','native world CreatePhysicalEntity'),
        (0x8478EC,'660d0001','suspension saves state with bit100'),
        (0x8478F0,'c64647ff','native source simulation becomes suspended -1'),
        (0x8478F4,'6641890424','native source saved suspension class through retained body+7C address'),
        (0x529517,'8b4244c1e009c1f809','native physical ID is signed 23-bit body+44')]:
        check(rva,wanted,label)
    assert all(c['passed'] for c in checks),[c for c in checks if not c['passed']]
    return dict(sha256=EXPECTED_SHA,timestamp=hex(pe.timestamp),native_execution=False,passed=len(checks),total=len(checks),checks=checks)
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('dll');p.add_argument('--out');a=p.parse_args();r=verify(a.dll)
    if a.out:Path(a.out).parent.mkdir(parents=True,exist_ok=True);Path(a.out).write_text(json.dumps(r,indent=2)+'\n')
    print(f"T4 held source/collider pins {r['passed']}/{r['total']} PASS; native execution=NONE")
