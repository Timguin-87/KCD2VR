"""Verify exact retail AddImpact AL-return evidence; no engine execution."""
import argparse
import hashlib
import json
from pathlib import Path
from verify_native_grasp13 import PE, EXPECTED_SHA, EXPECTED_TS

def verify(path):
    pe = PE(Path(path))
    assert hashlib.sha256(pe.data).hexdigest() == EXPECTED_SHA and pe.timestamp == EXPECTED_TS
    checks = []
    for rva, wanted, meaning in [
        (0x37C66DF, 'e874faffff4883c448c3', 'wrapper preserves body AL return'),
        (0x37C65FC, 'ff5038b001', 'issued native Action sets AL=1'),
        (0x37C6674, '32c0', 'native refusal sets AL=0'),
    ]:
        actual = pe.at(rva, len(bytes.fromhex(wanted))).hex()
        checks.append(dict(rva=hex(rva), expected=wanted, actual=actual, meaning=meaning, passed=actual==wanted))
    assert all(x['passed'] for x in checks), checks
    return dict(sha256=EXPECTED_SHA, timestamp=hex(EXPECTED_TS), checks=checks, native_execution=False)

if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('dll')
    p.add_argument('--out')
    a = p.parse_args()
    result = verify(a.dll)
    if a.out:
        Path(a.out).write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    print('H6 AddImpact retail Boolean contract: 3/3 PASS; engine execution=NONE')
