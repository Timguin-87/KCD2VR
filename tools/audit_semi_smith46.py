"""Read-only audit of C46 smith exact native owner/axis/clock pins.
Usage: python3 tools/audit_semi_smith46.py /path/WHGame.dll --out /tmp/audit
The supplied DLL is parsed and disassembled, never loaded or executed.
"""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse,json,re,struct
from smith_native_probe46 import PE,EXPECTED

def main():
    ap=argparse.ArgumentParser();ap.add_argument('dll');ap.add_argument('--out',type=Path,required=True);a=ap.parse_args()
    root=Path(__file__).resolve().parent.parent;p=PE(a.dll)
    header=read_source(root/'src/core/semi_smith_pins46.h')
    pins=[]
    for address,literal,size,label in re.findall(r'\{(0x[0-9A-F]+), "([^"]+)", (\d+)\}, // ([^\n]+)',header):
        rva=int(address,16);blob=bytes(int(x,16) for x in re.findall(r'\\x([0-9A-F]{2})',literal));assert len(blob)==int(size)
        actual=p.read(rva,len(blob));pins.append({'rva':hex(rva),'expected':blob.hex(),'actual':actual.hex(),'ok':blob==actual,'meaning':label})
    slots=[]
    for at,target in re.findall(r'\{(0x[0-9A-F]+),(0x[0-9A-F]+)\}',header):
        rva=int(at,16);want=int(target,16);actual=struct.unpack('<Q',p.read(rva,8))[0]-p.base
        slots.append({'rva':at,'expected':target,'actual':hex(actual),'ok':want==actual})
    old=json.loads((root/'tools/semi_smith_legacy_pins46.json').read_text())
    for rva,want,label in old['pins']:
        actual=p.read(rva,len(want)//2).hex();pins.append({'rva':hex(rva),'expected':want,'actual':actual,'ok':want==actual,'meaning':label})
    for rva,want,label in old['slots']:
        actual=struct.unpack('<Q',p.read(rva,8))[0]-p.base;slots.append({'rva':hex(rva),'expected':hex(want),'actual':hex(actual),'ok':want==actual,'meaning':label})
    constant=struct.unpack('<f',p.read(0x409A210,4))[0]
    result={'dll_sha256':EXPECTED,'timestamp':hex(p.stamp),'image_base':hex(p.base),'native_execution':False,
        'pins':pins,'slots':slots,'forge_repeat_seconds':constant,'ok':len(pins)==94 and len(slots)==12 and all(v['ok'] for v in pins+slots) and constant==.5}
    a.out.mkdir(parents=True,exist_ok=True);(a.out/'semi_smith46_audit.json').write_text(json.dumps(result,indent=2)+'\n')
    for f in [0x399AE0,0x399464,0x9CD584,0x8570C0,0x856B3C,0x8571F8,0x857764,0x856F98,0x682DA8,0x684D8C,0x684DEC,
              0x4B3DF8,0x4B3A20,0xA984B4,0xA98B64,0x858558,0x857D90,0xA98330,0x8595F0,0x8587B4,0x1538E60,0x2E5BD18,0x8599D8,0xC51060]:
        (a.out/f'{f:X}.asm').write_text(p.listing(f))
    for f,n in [(0x1A7E0E0,14),(0x1A73130,9),(0x684DC4,40),(0x3B6E80,3)]: (a.out/f'{f:X}.asm').write_text(p.listing(f,f+n))
    print(json.dumps({'ok':result['ok'],'pins':len(pins),'slots':len(slots),'native_execution':False,'result':str(a.out/'semi_smith46_audit.json')}))
    return 0 if result['ok'] else 1
if __name__=='__main__':raise SystemExit(main())
