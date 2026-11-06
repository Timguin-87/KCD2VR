"""Read-only proof of the retail missile receiver and ammunition boundary.

This does not qualify or execute a thrown-melee damage adapter. In particular,
knowing the correct service receiver is not proof it accepts a melee item as ammo.
"""
from pathlib import Path
import argparse, hashlib, json
from verify_native_grasp13 import PE, EXPECTED_SHA, EXPECTED_TS

def verify(path):
    pe=PE(path); checks=[]
    def add(name, actual, expected):
        checks.append(dict(name=name,actual=actual,expected=expected,passed=actual==expected))
    def pin(name,rva,expected):
        add(name,pe.at(rva,len(bytes.fromhex(expected))).hex(),expected)
    add('retail SHA256',hashlib.sha256(pe.data).hexdigest(),EXPECTED_SHA)
    add('retail timestamp',pe.timestamp,EXPECTED_TS)
    add('C_CombatSoul missile service slot158',hex(pe.q(0x3F45670+0x158)-pe.base),'0x2d291d0')
    add('C_Soul combat getter',hex(pe.q(0x3F44F28+0x1C8)-pe.base),'0x1a75200')
    # Native source item is resolved by WUID, not a current equipped-hand query.
    pin('source item lookup',0xC66832,'488b4b20488d54243048894c2430')
    pin('retained ammo item lookup',0xC6686E,'488b4b30488d54243048894c2430')
    # Both the stats helper and concrete ammo item accessor require native kind1B.
    pin('ammo native-kind check',0x7280DF,'ba1b000000488bc8e8a414d4ff84c0741f')
    pin('ammo accessor native-kind check',0xC6689E,'ba1b000000488bc8e8e52c80fff6d8')
    pin('missile cause allocation',0x2D226AC,'48895c2410574883ec20')
    return dict(schema=1,dll=str(path),native_execution=False,
        damage_adapter_cleared=False,checks=checks,
        passed=sum(c['passed'] for c in checks),total=len(checks),
        all_passed=all(c['passed'] for c in checks))

if __name__=='__main__':
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--dll',required=True,type=Path);ap.add_argument('--out',required=True,type=Path)
    args=ap.parse_args();report=verify(args.dll)
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(report,indent=2)+'\n')
    print(f"{report['passed']}/{report['total']} boundary checks; native damage not cleared")
    raise SystemExit(0 if report['all_passed'] else 1)
