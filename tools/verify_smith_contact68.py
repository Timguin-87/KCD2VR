"""Read-only exact-retail pins for the optional native smith contact observer."""
from pathlib import Path
import argparse,hashlib,json,re,struct
from smith_native_probe46 import PE,EXPECTED

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--dll',type=Path,required=True);ap.add_argument('--out',type=Path,required=True);a=ap.parse_args()
    p=PE(a.dll);root=Path(__file__).resolve().parent.parent
    text=(root/'src/subsystems/smithing/native_contact68.inl').read_text()
    addresses={'entry':0xC51060,'pointCall':0x859A71,'eventCall':0x858C3E,'cursorGetter':0x9D3820,'pointSelector':0xC510A2}
    rows=[]
    for name,rva in addresses.items():
        m=re.search(r'constexpr unsigned char '+name+r'\[\]\{([^}]+)\}',text);assert m,name
        want=bytes(int(v,16) for v in re.findall(r'0x([0-9A-Fa-f]{2})',m[1]))
        got=p.read(rva,len(want));rows.append({'name':name,'rva':hex(rva),'bytes':got.hex(),'ok':got==want})
    slots=[]
    for table,offset,target in [(0x3A6A1E0,8,0x9D3820),(0x3A6A2C8,8,0x8589E8),
        (0x4072C78,0x100,0x1A74140),(0x4072C78,0x140,0x1A749E0),
        (0x4072C78,0x328,0x6411AC),(0x40602F8,0x58,0x8A49D0),
        (0x406B930,0xA0,0x641224),(0x40602F8,0x38,0x1AA6BC0),
        (0x3A71028,0xD8,0x641250),(0x3A71510,0x50,0x53D830),
        (0x3C16A90,0x240,0xC6500C)]:
        actual=struct.unpack('<Q',p.read(table+offset,8))[0]-p.base
        slots.append({'vft':hex(table),'offset':hex(offset),'expected':hex(target),'actual':hex(actual),'ok':actual==target})
    ok=all(r['ok'] for r in rows+slots)
    result={'ok':ok,'sha256':EXPECTED,'timestamp':hex(p.stamp),'pins':rows,'slots':slots,
        'nativeExecuted':False,'writes':False,'scope':'actual native StrokeHit point/cursor read-only observation, not physical hit admission'}
    a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(result,indent=2)+'\n')
    print(f"smith_contact68: {len(rows)} code pins + {len(slots)} exact slots {'PASS' if ok else 'FAIL'} (read-only)")
    return 0 if ok else 1
if __name__=='__main__':raise SystemExit(main())
