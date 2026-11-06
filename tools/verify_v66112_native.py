"""Record exact retail native landmarks without loading or executing WHGame."""
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

EXPECTED_SHA = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
FUNCTIONS = {
    'animation_start_name': 0xAA7FA0, 'animation_start_id': 0xAA8514,
    'native_zone_setter': 0x90F824, 'native_request_attack': 0x93A28C,
    'native_damage_observer': 0x72704C, 'weapon_sweep': 0xA7A124,
    'sweep_authored_subdivision': 0xA7C624, 'sweep_world_bone_caller': 0xA7EAB4,
    'sweep_other_subdivision': 0x27C1204, 'world_bone_pose': 0x905560,
    'absolute_joint_pose': 0x9057B0,
    'combined_factory': 0x120F438, 'position_factory': 0x120F550,
    'rotation_factory': 0x120F588, 'combined_create': 0x915AF4,
    'position_create': 0x152853C, 'rotation_create': 0x915DF8,
    'position_combined_enter_candidate': 0xC4E524,
    'rotation_enter_candidate': 0xC4E5E8,
    'position_combined_registration': 0xC4FB68,
    'rotation_registration': 0xC4E720,
    'alignment_late_query': 0x9E37A8, 'alignment_apply': 0x9E2D08,
    'alignment_queue_consumer': 0xA70F20, 'alignment_active_record': 0x14098D8,
}


def verify(dll):
    pe = PE(dll)
    digest = hashlib.sha256(pe.data).hexdigest()
    assert digest == EXPECTED_SHA and pe.stamp == 0x6A350E20, 'Unsupported WHGame'
    root = Path(__file__).resolve().parents[1]
    text = read_source(root / 'src/vrik/physical_native_contract.h')
    checks = []
    for rva, values, name in re.findall(r'\{(0x[0-9A-Fa-f]+),\s*\{([^}]+)\},\s*"([^"]+)"', text):
        expected = bytes(int(x.strip(), 16) for x in values.split(','))
        checks.append({'purpose': name, 'rva': rva, 'passed': pe.read(int(rva, 16), len(expected)) == expected})
    # V114 adds two per-item wrappers alongside the unchanged 15 legacy entries.
    assert len(checks) == 17 and all(c['passed'] for c in checks), checks
    motion = read_source(root / 'src/vrik/native_motion112.inl')
    motion_checks = []
    for name, rva in [('query', 0x9E37A8), ('alignmentCall', 0x9E2D50), ('sweepCall', 0xA7EB53)]:
        values = re.search(r'const unsigned char '+name+r'\[\d+\]\{([^}]+)\}', motion).group(1)
        expected = bytes(int(x.strip(), 0) for x in values.split(','))
        assert pe.read(rva, len(expected)) == expected, name
        motion_checks.append({'name': name, 'rva': hex(rva), 'passed': True})
    assert struct.unpack('<Q', pe.read(0x3A633B0+7*8, 8))[0] == pe.base+0x9E2D00
    assert pe.read(0x9E2E99, 9) == bytes.fromhex('488b03488bcbff5010'), 'native action release'
    for primary, method, kind in [(0x3BAD328,0x1A74A40,2),(0x3A622C8,0x1A78EB0,3),(0x3A62098,0x41A6A0,1)]:
        assert struct.unpack('<Q',pe.read(primary+0x68,8))[0] == pe.base+method
        assert pe.read(method,3) == bytes([0xB0,kind,0xC3]), 'alignment type is an enum'
    records = {}
    for name, rva in FUNCTIONS.items():
        bounds = pe.function(rva)
        assert bounds and bounds[0] == rva, (name, bounds)
        records[name] = {'rva': hex(rva), 'end_rva': hex(bounds[1]),
                         'first16': pe.read(rva, 16).hex(),
                         'function_sha256': hashlib.sha256(pe.read(rva, bounds[1]-rva)).hexdigest()}
    calls = []
    for rva in (0xA7C9BC, 0xA7EB53, 0x27C1690):
        instruction = pe.read(rva, 5)
        assert instruction[0] == 0xE8
        assert rva+5+struct.unpack_from('<i', instruction, 1)[0] == 0xA7A124
        calls.append({'call_rva': hex(rva), 'return_rva': hex(rva+5)})
    tables = {}
    for name, rva in [('position', 0x3BAD328), ('rotation', 0x3A622C8), ('combined', 0x3A62098)]:
        tables[name] = {'rva': hex(rva), 'entries_rva': [hex(struct.unpack('<Q', pe.read(rva+i*8, 8))[0]-pe.base) for i in range(14)]}
    return {'passed': True, 'dll_sha256': digest, 'pe_timestamp': hex(pe.stamp),
            'preferred_image_base': hex(pe.base), 'physical_fingerprints': checks,
            'functions': records, 'sweep_calls': calls, 'alignment_vtables': tables,
            'motion_fingerprints': motion_checks,
            'limitations': 'Static bytes and control flow only. Rung 3 live timing, active record ownership and bone/socket correlation still require runtime validation. No hit context construction or enemy contact ABI is certified.'}


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('--whgame', required=True, type=Path)
    p.add_argument('--out', required=True, type=Path)
    a = p.parse_args()
    report = verify(a.whgame)
    a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps(report, indent=2) + '\n')
    print(f"PASS {len(report['physical_fingerprints'])} physical fingerprints, {len(report['functions'])} function landmarks, 3 sweep calls; no native execution")
