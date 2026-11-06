"""Reproduce the F8=2 static contract and apple/script findings. No game execution."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse
import hashlib
import json
import re
import struct
import zipfile
from pathlib import Path
from native_research112 import PE

EXPECTED = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'

def inspect(game):
    root = Path(__file__).resolve().parents[1]
    binary = game/'Bin/Win64MasterMasterSteamPGO/WHGame.dll'
    pe = PE(binary)
    digest = hashlib.sha256(pe.data).hexdigest()
    assert digest == EXPECTED and pe.stamp == 0x6A350E20, 'Unsupported retail binary'
    src = read_source(root/'src/diagnostics/interaction_lifecycle_sites165.inl')
    sites = []
    for rva, numbers, callback in re.findall(r'\{(0x[0-9A-F]+),\{([^}]+)\},(\w+)\}', src):
        address = int(rva, 16)
        expected = bytes(int(x, 16) for x in numbers.split(','))
        actual = pe.read(address, len(expected))
        assert actual == expected, (rva, callback)
        sites.append(dict(rva=rva, callback=callback, bytes=actual.hex()))
    assert len(sites) == 23
    # RTTI COL refers to C_PickableItem at primary (offset 0) vtable.
    vft = 0x3EC85E0
    col = struct.unpack('<Q', pe.read(vft-8, 8))[0]-pe.base
    signature, offset, _, descriptor, _, self_rva = struct.unpack('<6I', pe.read(col, 24))
    assert signature == 1 and offset == 0 and self_rva == col
    name = pe.read(descriptor+16, 80).split(b'\0')[0].decode()
    assert name == '.?AVC_PickableItem@entitymodule@wh@@'
    methods = {}
    for slot, target in [(0x2B8, 0x2A8A838), (0x2E0, 0x2A8A7EC), (0x400, 0x1A7F820)]:
        assert struct.unpack('<Q', pe.read(vft+slot, 8))[0] == pe.base+target
        methods[hex(slot)] = hex(target)
    assert pe.read(0x1A7F820, 5) == bytes.fromhex('488b4158c3')
    assert struct.unpack('<Q', pe.read(0x4077048+0x98, 8))[0] == pe.base+0x47EC24
    assets = []
    for pak, suffix, pattern in [
        ('Tables.pak', '/item/item.xml', r'Name="apple"'),
        ('Scripts.pak', '/PickableItem.lua', r'OnUsed|OnSteal|OnUse|RemoveEntity'),
        ('Scripts.pak', '/AlchemyItem.lua', r'alch_use|bRigidBody|alchemy_type|OnUse|IsUsable'),
    ]:
        with zipfile.ZipFile(game/'Data'/pak) as z:
            names = [n for n in z.namelist() if n.lower().endswith(suffix.lower())]
            assert len(names) == 1, (pak, suffix, names)
            data = z.read(names[0])
            selected = [dict(line=i, text=s.strip()) for i, s in enumerate(data.decode('utf-8-sig',errors='replace').splitlines(), 1) if re.search(pattern, s)]
            assets.append(dict(pak=pak, entry=names[0], sha256=hashlib.sha256(data).hexdigest(), matches=selected))
    return dict(whgame_sha256=digest,timestamp=hex(pe.stamp),sites=sites,pickable_rtti=name,
                pickable_vft=hex(vft),methods=methods,assets=assets,
                caveat='Static identity/dispatch evidence; not a runtime constraint or successful grasp proof.')

if __name__ == '__main__':
    p=argparse.ArgumentParser();p.add_argument('--game',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
    a=p.parse_args();result=inspect(a.game);a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print('PASS exact retail SHA/timestamp, 23 observer sites, PickableItem RTTI/3 virtual targets, native RemoveEntity slot, apple and Lua assets')
