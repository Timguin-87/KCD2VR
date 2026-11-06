"""Read-only verification of V164 retail pins and shipped door policy. Never loads the DLL."""
import argparse
import hashlib
import json
import struct
import zipfile
from pathlib import Path
from native_research112 import PE
from verify_v66114_native import inspect


def verify(binary, scripts):
    parent = inspect(binary)
    pe = PE(binary)
    checks = []
    for rva, raw, purpose in (
        (0x687C54, '488b4c24284833cc', 'completed native close-action query observer'),
        (0x687BF0, 'e82f020000', 'builder call to 687E24'),
        (0x1AAFBC0, '8b41082500000100c3', 'retail attachment hidden-bit reader; not called'),
    ):
        assert pe.read(rva, len(bytes.fromhex(raw))) == bytes.fromhex(raw), purpose
        checks.append(dict(rva=hex(rva), bytes=raw, purpose=purpose))
    assert pe.function(0x687C54)[0] == 0x687BBC
    assert 0x687BF0 + 5 + struct.unpack('<i', pe.read(0x687BF1, 4))[0] == 0x687E24
    rtti = []
    for vft, expected in ((0x3A578C0, '.?AVCPhysicalEntity@@'),
                          (0x3A657C0, '.?AVCAttachmentBONE@@'),
                          (0x40602F8, '.?AVCCharInstance@@')):
        col = struct.unpack('<Q', pe.read(vft - 8, 8))[0] - pe.base
        desc = struct.unpack('<I', pe.read(col + 12, 4))[0]
        name = pe.read(desc + 16, 100).split(b'\0')[0].decode('ascii')
        assert name == expected
        rtti.append(dict(vft=hex(vft), col=hex(col), type_descriptor=hex(desc), name=name))
    functions = {}
    for start in (0x687BBC, 0x687E24, 0x8A49FC):
        a, b = pe.function(start)
        assert a == start
        functions[hex(start)] = dict(end=hex(b), sha256=hashlib.sha256(pe.read(a, b-a)).hexdigest())
    member = 'Scripts/Entities/Doors/AnimDoor.lua'
    with zipfile.ZipFile(scripts) as z:
        data = z.read(member)
    lines = data.decode('utf-8-sig').splitlines()
    selected = [dict(line=i, text=s.strip()) for i, s in enumerate(lines, 1)
                if any(x in s for x in ('bRigidBody', 'bPushableByPlayers', 'PhysicalizeRigid',
                                        'collisionClass =', 'AddUseAction', 'IsLocked'))]
    assert any('Physics.bRigidBody = false' in s for s in lines)
    assert any('EntityCommon.PhysicalizeRigid(self, 0, Physics, true)' in s for s in lines)
    return dict(passed=True, parent=parent, checks=checks, rtti=rtti, functions=functions,
                door=dict(archive=str(scripts), archive_sha256=hashlib.sha256(scripts.read_bytes()).hexdigest(),
                          member=member, member_sha256=hashlib.sha256(data).hexdigest(), excerpts=selected),
                limits='Static pins/RTTI/Lua policy, not execution or a verified constraint ABI. No SDK vtable slots imported.')


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('--whgame', type=Path, required=True)
    p.add_argument('--scripts', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    a = p.parse_args()
    report = verify(a.whgame, a.scripts)
    a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print('PASS V164: close-query pins, three RTTI identities, door static policy, and inherited V114/V112 checks')
