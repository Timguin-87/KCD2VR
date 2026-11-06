"""Verify the retail original-material producer/consumer chain without game calls."""
import argparse, hashlib, json
from pathlib import Path
from verify_v66112_native import PE

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('dll');p.add_argument('--out',required=True);a=p.parse_args()
    pe=PE(a.dll);sha=hashlib.sha256(pe.data).hexdigest()
    if pe.stamp!=0x6A350E20 or sha!='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7':
        raise SystemExit('Unsupported retail fingerprint')
    pins={
        0x13F5811:'895368', # original material -> details+68
        0x13F5814:'8b4e38', # mapped effect material from contact+38
        0x13F5817:'894b64', # mapped material -> details+64
        0x13F58BA:'418b842420010000', # ATTACKER collision shape material
        0x13F58C2:'89436c', # -> details+6C (not victim mapped material)
        0x38C3BBF:'66894f1a', # native ray original material -> ray_hit+1A
        0x38C4788:'6641894718', # native ray mapped material -> ray_hit+18
        0x38C47B0:'6641894f1a', # native ray original material -> ray_hit+1A
        0xD56A30:'8b4168', # native hit resolver reads original material
    }
    rows=[]
    for rva,expected in pins.items():
        actual=pe.read(rva,len(bytes.fromhex(expected))).hex()
        if actual!=expected:raise SystemExit(f'Pin mismatch {rva:X}: {actual} != {expected}')
        rows.append({'rva':f'{rva:X}','bytes':actual})
    report={'sha256':sha,'timestamp':hex(pe.stamp),'pins':rows,
            'native_calls':False,'claim':'Static dataflow pins only; runtime damage/defense parity not established.'}
    Path(a.out).write_text(json.dumps(report,indent=2)+'\n')
    print(f'PASS {len(rows)} contact-material pins; exact retail hash and timestamp')
if __name__=='__main__':main()
