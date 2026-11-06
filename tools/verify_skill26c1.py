"""Read-only exact-retail skill getter and actual weapon-class table verification."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse
import hashlib
import json
import re
import struct
import xml.etree.ElementTree as ET
import zipfile

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--dll', required=True)
p.add_argument('--tables', required=True)
p.add_argument('--out', required=True)
a = p.parse_args()
b = Path(a.dll).read_bytes()
assert hashlib.sha256(b).hexdigest() == 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
pe = struct.unpack_from('<I', b, 0x3c)[0]
assert b[pe:pe+4] == b'PE\0\0' and struct.unpack_from('<I', b, pe+8)[0] == 0x6A350E20
n = struct.unpack_from('<H', b, pe+6)[0]
sections = pe+24+struct.unpack_from('<H', b, pe+20)[0]

def at(rva, count):
    for i in range(n):
        _, va, size, offset = struct.unpack_from('<IIII', b, sections+i*40+8)
        if va <= rva and rva+count <= va+size:
            return b[offset+rva-va:offset+rva-va+count]
    raise ValueError(hex(rva))

checks = []
def check(name, ok):
    checks.append({'name': name, 'passed': bool(ok)})

check('native unsigned id bound and soul+0x5E8+8*id DWORD read',
      at(0x1123C4B, 14) == bytes.fromhex('83fa23730b8bc28b84c1e8050000'))
check('native stats range precheck', at(0x1123C38, 8) == bytes.fromhex('8d42dd83f80a730b'))
check('native skill path returns integer through eax', at(0x1123C59, 2) == bytes.fromhex('eb1a'))
check('native common epilogue', at(0x1123C75, 5) == bytes.fromhex('4883c428c3'))
root = Path(__file__).resolve().parents[1]
producer = read_source(root/'src/diagnostics/live_attributes5912.inl')
ids = [int(x) for x in re.search(r'ids\[5\]\{([^}]+)\}', producer)[1].split(',')]
check('producer ids all inside native unsigned skill range', all(0 <= i < 0x23 for i in ids))
names = ['weapon_sword', 'heavy_weapons', 'weapon_large', 'weapon_dagger', 'weapon_unarmed']
expected_classes = {0:3, 1:0, 2:0, 3:1, 4:0, 5:1, 6:2, 7:2, 11:3, 12:4, 16:0}
handling = read_source(root/'src/vrik/weapon_dynamics177.h')
body = handling.split('inline int skillSlot26', 1)[1].split('inline double skillFactor26', 1)[0]
actual_classes = {}
for cases, slot in re.findall(r'((?:case\s+\d+:)+)return\s+(\d+);', body):
    for cls in re.findall(r'case\s+(\d+):', cases):
        actual_classes[int(cls)] = int(slot)
for cls, slot in expected_classes.items():
    check('production handling class slot '+str(cls), actual_classes.get(cls) == slot)
tables = {}
with zipfile.ZipFile(a.tables) as z:
    skill_name = next(x for x in z.namelist() if x.lower() == 'libs/tables/rpg/skill.xml')
    class_name = next(x for x in z.namelist() if x.lower() == 'libs/tables/item/weapon_class.xml')
    for name in (skill_name, class_name):
        data = z.read(name)
        tables[name] = {'sha256': hashlib.sha256(data).hexdigest(), 'bytes': len(data)}
    skills = {e.get('skill_name'):int(e.get('skill_id')) for e in ET.fromstring(z.read(skill_name)).iter() if e.get('skill_name')}
    rows = {int(e.get('id')):e.attrib for e in ET.fromstring(z.read(class_name)).iter() if e.tag == 'MeleeWeaponClass'}
    for i, name in enumerate(names):
        check('table skill id '+name, skills[name] == ids[i])
    for cls, slot in expected_classes.items():
        check('table class '+str(cls)+' skill '+names[slot], rows[cls].get('skill') == names[slot])
result = {'dll': a.dll, 'dll_sha256': hashlib.sha256(b).hexdigest(),
          'timestamp': '0x6A350E20', 'tables': tables, 'checks': checks,
          'native_executed': False, 'native_written': False,
          'limitation': 'Base integer level only; no claim of effective buff-adjusted skill or headset feel.',
          'passed': all(x['passed'] for x in checks)}
out = Path(a.out); out.parent.mkdir(parents=True, exist_ok=True)
out.write_text(json.dumps(result, indent=2)+'\n')
print(f"skill getter/table verification: {sum(x['passed'] for x in checks)}/{len(checks)} PASS")
raise SystemExit(0 if result['passed'] else 1)
