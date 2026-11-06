"""Read-only exact-retail proof for Lua's same-VM TValue-stack relocation."""
from pathlib import Path
import argparse
import hashlib
import json
import re
import struct
import sys

sys.path.insert(0, 'C:/Users/turcs/.codex/worktrees/d514/KCD2VR/build-review178r19/analysis-deps')
import pefile
import capstone

ROOT = Path(__file__).resolve().parents[1]
SHA = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('output', type=Path)
    ap.add_argument('--binary', type=Path, default=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll'))
    args = ap.parse_args()
    raw = args.binary.read_bytes()
    assert hashlib.sha256(raw).hexdigest() == SHA
    pe = pefile.PE(data=raw, fast_load=True)
    assert pe.FILE_HEADER.TimeDateStamp == 0x6A350E20
    checks = []

    def pin(rva, data, label):
        assert pe.get_data(rva, len(bytes.fromhex(data))) == bytes.fromhex(data), label
        checks.append(label)

    def branch(rva, size, dst, label):
        assert rva + size + struct.unpack('<i', pe.get_data(rva + size - 4, 4))[0] == dst, label
        checks.append(label)

    pin(0x7156AE, '488b4e388bc3482b4e10c1e004', 'interpreter compares stack-last+38 minus top+10 against TValue16 bytes')
    pin(0x7156C4, '0f8e', 'capacity exhaustion takes native growth branch')
    branch(0x7156C4, 6, 0x2229EEE, 'interpreter growth cold branch')
    pin(0x2229EEE, '8bd3488bce', 'growth receives same Lua state and requested count')
    branch(0x2229EF3, 5, 0x39974B0, 'growth enters actual capacity helper')
    pin(0x39974B0, '448b4158448bca', 'capacity helper reads native stack capacity+58')
    pin(0x39974E0, '488b6940', 'native allocator retains original stack base+40')
    branch(0x3997514, 5, 0x7175C8, 'actual allocator receives original stack and old/new TValue16 byte sizes')
    pin(0x3997523, '48894340', 'native stores returned stack base at state+40')
    branch(0x3997552, 5, 0x3997318, 'allocator tail-calls native cursor rebase')
    pin(0x3997318, '488b41104c8bc2482bc2', 'rebase subtracts old stack base from top+10')
    pin(0x3997329, '4883e0f04803414048894110', 'rebase restores aligned TValue top from new base+40')
    pin(0x3997399, '488b4118492bc04883e0f04803414048894118', 'same native rebase updates frame+18 without replacing VM')
    pin(0x713964, '4c2b4b40', 'native protected call itself stores stack-relative function index')
    pin(0x717AF6, '488b51184c8b49104a8d04c2', 'SetTop uses current frame+18 plus count times16')
    text = (ROOT / 'src/native_script_t2.cpp').read_text()
    for name, rva in [('stackBase', 0x3997523), ('rebase', 0x3997399)]:
        m = re.search(r'const unsigned char ' + name + r'\[\]\{([^}]+)\}', text)
        assert m
        expected = bytes(int(v, 16) for v in re.findall(r'0x([0-9a-fA-F]+)', m[1]))
        assert pe.get_data(rva, len(expected)) == expected
        assert f'Pin(0x{rva:X},{name},sizeof({name}))' in text
        checks.append(f'current production {name} install pin equals retail')
    args.output.mkdir(parents=True, exist_ok=True)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
    ranges = [(0x7156AE, 0x7156E5), (0x2229EEE, 0x2229F05), (0x39974B0, 0x3997557),
              (0x3997318, 0x39973AD), (0x713914, 0x713982), (0x717AEC, 0x717B30)]
    for a, b in ranges:
        lines = [f'{i.address-pe.OPTIONAL_HEADER.ImageBase:08X} {i.bytes.hex()} {i.mnemonic} {i.op_str}'
                 for i in md.disasm(pe.get_data(a, b-a), pe.OPTIONAL_HEADER.ImageBase+a)]
        (args.output / f'{a:X}.asm').write_text('\n'.join(lines) + '\n')
    report = dict(result='PASS', checks=checks, count=len(checks), retail_sha256=SHA,
                  timestamp='0x6A350E20', source_sha256=hashlib.sha256((ROOT/'src/native_script_t2.cpp').read_bytes()).hexdigest(),
                  execution='Read-only PE data; no native function executed.',
                  limit='Proves valid same-VM stack movement is native behavior; does not establish that a particular headset callback relocated.')
    (args.output / 'RESULTS.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
