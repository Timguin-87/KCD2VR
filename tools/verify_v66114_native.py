"""Static retail evidence for the comparison branch. Never loads/calls WHGame."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
from native_research112 import PE
from verify_v66112_native import verify

ROOT = Path(__file__).resolve().parents[1]
FUNCTIONS = {
    'draw_item_key': 0x8CEA48, 'draw_item': 0x8D00C4,
    'holster_item_key': 0x8CF158, 'holster_item': 0x8CF198,
    'bow_primary': 0x1F24800, 'bow_prepare': 0x2AAA4C0,
    'bow_charge': 0x28A1044, 'bow_release': 0x2AA2BD8,
    'contact_callback': 0xD51854, 'contact_enrichment': 0xD51B38,
    'contact_filter1': 0xD51ACC, 'contact_filter2': 0xD52340,
    'contact_listeners': 0xD53024,
}

def inspect(binary):
    baseline = verify(binary)  # full SHA, timestamp, all 17 entry fingerprints, V112 motion gates
    pe = PE(binary)
    checks = []
    def exact(rva, value, purpose):
        expected = bytes.fromhex(value)
        assert pe.read(rva, len(expected)) == expected, purpose
        checks.append({'rva':hex(rva), 'bytes':value, 'purpose':purpose})
    def branch(rva, target, opcode=0xE8):
        raw = pe.read(rva, 5)
        assert raw[0] == opcode and rva+5+struct.unpack_from('<i',raw,1)[0] == target
        checks.append({'rva':hex(rva), 'target':hex(target), 'kind':'call' if opcode==0xE8 else 'tailcall'})
    branch(0x2AA24BC,0x8CEA48); branch(0x2AA24E1,0x8CEA48,0xE9)
    exact(0x2AA24A9,'41b802000000','native draw wrapper supplies mode 2')
    branch(0x8CEA6A,0x1E3E090); branch(0x8CEA86,0x8D00C4,0xE9)
    branch(0x8EE9E0,0x8CF158); branch(0x8EE9FF,0x8CF158,0xE9)
    branch(0x8CF177,0x1E3E090); branch(0x8CF187,0x8CF198)
    exact(0x8CF18C,'b001','holster wrapper return is request dispatch, not attachment acknowledgement')
    exact(0x1F24825,'c6402101','bow input latch is set before eligibility')
    branch(0x1F24831,0x2AAA4C0)
    exact(0x28A1157,'0f2f05e68d7f01','native charge compares progress to constant')
    constant = 0x28A1157+7+struct.unpack('<i',pe.read(0x28A115A,4))[0]
    assert struct.unpack('<f',pe.read(constant,4))[0] == 1.0
    exact(0x28A115E,'0f83a6000000','native charge completes at >=1.0')
    # V115R1 consolidates this into the existing single hook owner. Verify its
    # full 17-byte production contract rather than the retired duplicate hook.
    source = read_source(ROOT/'src/combat_diag_probe.cpp')
    values = re.search(r'kEntry\[\]=\{([^}]+)\}',source).group(1)
    exact(0xD51854,bytes(int(v,0) for v in values.split(',')).hex(),'shared production contact hook fingerprint')
    assert struct.unpack('<Q',pe.read(0x3B912F0,8))[0] == pe.base+0xD51854
    assert struct.unpack('<I',pe.read(0x415DE54,4))[0] == 0x10  # secondary RTTI COL subobject offset
    exact(0xD51B63,'b980000000','raw contact copy length prefix 0x80')
    exact(0xD51BF3,'498b04084889040a','native copies final raw contact qword at +0x80')
    exact(0xD51C00,'418b5624','target actor ID is read from raw contact +0x24')
    branch(0xD5190B,0xD51B38); branch(0xD51920,0xD51ACC)
    branch(0xD51939,0xD52340); branch(0xD51958,0xD53024)
    functions = {}
    for name,rva in FUNCTIONS.items():
        bounds = pe.function(rva)
        assert bounds and bounds[0] == rva
        functions[name] = {'rva':hex(rva),'end_rva':hex(bounds[1]),
            'sha256':hashlib.sha256(pe.read(rva,bounds[1]-rva)).hexdigest()}
    return {'passed':True,'dll_sha256':baseline['dll_sha256'],'timestamp':baseline['pe_timestamp'],
        'baseline':baseline,'checks':checks,'functions':functions,
        'contact_vtable':{'primary':'0x3B91300','secondary':'0x3B912E8','secondary_offset':'0x10','slot':'0x3B912F0'},
        'limitations':'Static call/data flow only. No native execution. Raw contacts precede native filters; accepted hit, weapon parry geometry, active-ragdoll/constraint ownership and reload lifecycle remain unqualified.'}

if __name__ == '__main__':
    p=argparse.ArgumentParser();p.add_argument('--whgame',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
    a=p.parse_args();report=inspect(a.whgame);a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    print(f"PASS V114: {len(report['checks'])} byte/control-flow checks, {len(report['functions'])} functions, all V112 gates; no native execution")
