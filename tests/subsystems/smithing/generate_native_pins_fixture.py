"""Generate fake byte leaves for the actual production wrapper fixture.

This is not retail verification. Run verify_smith_tool36.py against WHGame.dll
separately. Byte leaves merely allow the fixture to exercise the real wrapper's
player, phase, minigame and representation lifetime logic.
"""
from pathlib import Path
import argparse,re

ROOT=Path(__file__).resolve().parents[3]

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--out',type=Path,required=True);a=ap.parse_args()
    source=(ROOT/'src/vrik/smith_tool36.inl').read_text()
    arrays={n:bytes(int(v,16) for v in re.findall(r'0x([0-9a-fA-F]+)',b))
        for n,b in re.findall(r'constexpr unsigned char (\w+)\[\]\{([^}]+)\}',source)}
    lines=['// Generated fake native byte leaves; not a native execution/retail test.','static void seedNativePinsG1(){']
    for at,name in re.findall(r'match\(base\+0x([0-9A-Fa-f]+),(\w+)\)',source):
        lines.append('blob(base+0x'+at+', {'+','.join(hex(b) for b in arrays[name])+'});')
    for at,target in re.findall(r'callTo\(base\+0x([0-9A-Fa-f]+),base\+0x([0-9A-Fa-f]+)\)',source):
        lines.append(f'put(base+0x{at},std::uint8_t(0xE8));put(base+0x{at}+1,std::int32_t(0x{target}-0x{at}-5));')
    for at,index,target in re.findall(r'slot\(base\+0x([0-9A-Fa-f]+),(0x[0-9A-Fa-f]+|\d+),base\+0x([0-9A-Fa-f]+)\)',source):
        lines.append(f'put(base+0x{at}+8*{index},base+0x{target});')
    lines.append('}')
    a.out.mkdir(parents=True,exist_ok=True)
    (a.out/'native_pins_fixture_g1.h').write_text('\n'.join(lines)+'\n')

if __name__=='__main__':main()
