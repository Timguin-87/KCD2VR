"""Exact retail ownership/receiver proof for the H7 corpse-contact route."""
from pathlib import Path
import argparse, hashlib, json
from verify_native_grasp13 import PE, EXPECTED_SHA, EXPECTED_TS

def verify(path):
    p=PE(Path(path));assert hashlib.sha256(p.data).hexdigest()==EXPECTED_SHA and p.timestamp==EXPECTED_TS
    rows=[]
    for rva, wanted, meaning in [
        (0xC35E60,'4881c100010000','pose relinquish wrapper adjusts RCX by100'),
        (0xC35EED,'488bf1','relinquish saves adjusted receiver in RSI'),
        (0xC37471,'498bd7488bcee81055e8ff804e1c20','new body passed to setter before receiver flag20'),
        (0xABC9A2,'48895108','current main physics published at receiver+8'),
        (0x1A77FD0,'488b8108010000c3','pose getter reads same current main at pose+108')]:
        actual=p.at(rva,len(bytes.fromhex(wanted))).hex()
        rows.append(dict(rva=hex(rva),expected=wanted,actual=actual,meaning=meaning,passed=actual==wanted))
    expected=0xC35E60;actual=p.q(0x3A71028+0x48)-p.base
    rows.append(dict(rva=hex(0x3A71028+0x48),expected=hex(expected),actual=hex(actual),meaning='exact current pose method table relinquish slot',passed=actual==expected))
    assert all(x['passed'] for x in rows), rows
    return dict(sha256=EXPECTED_SHA,timestamp=hex(EXPECTED_TS),checks=rows,native_execution=False,
                limitation='Same fresh post-death contact only; does NOT map killing-hit parts to successor bodies')
if __name__=='__main__':
    a=argparse.ArgumentParser();a.add_argument('dll');a.add_argument('--out');args=a.parse_args();r=verify(args.dll)
    if args.out:Path(args.out).write_text(json.dumps(r,indent=2)+'\n',encoding='utf-8')
    print('H7 corpse contact receiver/publication 6/6 PASS; engine execution=NONE')
