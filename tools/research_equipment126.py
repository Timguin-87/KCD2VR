"""Read-only retail equipment class/slot evidence; no native execution."""
import argparse
import hashlib
import json
from pathlib import Path
import xml.etree.ElementTree as ET
import zipfile
import capstone
from native_research112 import PE


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('--whgame', type=Path, required=True)
    p.add_argument('--tables', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    a = p.parse_args()
    pe = PE(a.whgame)
    digest = hashlib.sha256(pe.data).hexdigest()
    assert pe.stamp == 0x6A350E20 and digest == 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
    a.out.mkdir(parents=True, exist_ok=True)
    member = 'Libs/Tables/item/weapon_class.xml'
    with zipfile.ZipFile(a.tables) as z:
        raw = z.read(member)
    rows = [dict(kind=e.tag, **e.attrib) for e in ET.fromstring(raw)[0]]
    lookup = {int(r['id']): r for r in rows}
    for cls in (9, 10, 14, 15):
        assert lookup[cls]['equip_slot'] == 'SecondaryMainHand' and lookup[cls]['in_right_hand'] == 'false'
    assert lookup[13]['equip_slot'] == 'SecondaryMainHand' and lookup[13]['in_right_hand'] == 'true'
    (a.out/'weapon-class-table.json').write_text(json.dumps(dict(archive=str(a.tables), member=member,
        member_sha256=hashlib.sha256(raw).hexdigest(), rows=rows), indent=2)+'\n', encoding='utf-8')
    cs = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
    functions = []
    for rva in (0x8CF158, 0x8CF198, 0x8CEA48, 0x8D00C4):
        begin, end = pe.function(rva)
        assert begin == rva
        data = pe.read(begin, end-begin)
        text = '\n'.join(f'{i.address:08X} {i.mnemonic} {i.op_str}' for i in cs.disasm(data, rva))
        (a.out/f'equipment_{rva:X}.asm').write_text(text+'\n', encoding='utf-8')
        functions.append(dict(rva=hex(rva), end=hex(end), sha256=hashlib.sha256(data).hexdigest()))
    (a.out/'equipment-contract126.json').write_text(json.dumps(dict(sha256=digest, timestamp=hex(pe.stamp),
        functions=functions, limitation='Equipment-column identity differs from anatomical hand. Native wrapper return is not rendered holster proof. No new function address is called by V126.'), indent=2)+'\n', encoding='utf-8')
    print('PASS exact retail equipment functions; left-held ranged classes occupy SecondaryMainHand; rifle primary remains right')
