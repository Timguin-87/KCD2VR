"""Read-only exact-retail check for G3's optional PlaceMortar observer."""
from pathlib import Path
import argparse,json,re,struct
from smith_native_probe46 import PE,EXPECTED
p=argparse.ArgumentParser();p.add_argument('--dll',required=True);p.add_argument('--out',required=True);a=p.parse_args();pe=PE(a.dll)
s=(Path(__file__).resolve().parents[1]/'src/alchemy_semi_native46.inl').read_text();rows=[]
for name,rva in [('entry70',0x6C4A34),('slot70',0x6C3B16)]:
 m=re.search(r'constexpr unsigned char '+name+r'\[\]\{([^}]+)\}',s);assert m
 b=bytes(int(x,16) for x in re.findall(r'0x([0-9a-fA-F]{2})',m[1]));actual=pe.read(rva,len(b));assert actual==b
 rows.append(dict(name=name,rva=hex(rva),bytes=actual.hex()))
assert struct.unpack('<Q',pe.read(0x3A5BB90+0xF8,8))[0]==pe.base+0x6C3AC0
# Observed event id from SAnimEvent+1C is compared with this exact parent's
# Detach hash+68 and Use hash+6C. The observer never synthesizes either event.
assert pe.read(0x6C4A45,3)==bytes.fromhex('3b4168')
assert pe.read(0x6C4A4A,3)==bytes.fromhex('3b416c')
assert pe.read(0x6C3BBE,7)==bytes.fromhex('488d056f0e0000') # real event callback registration ->6C4A34
result=dict(pass_=True,sha256=EXPECTED,timestamp=hex(pe.stamp),code_pins=rows,event_fields=dict(detach='68',use='6C',SAnimEvent='1C'),native_start=dict(vft='3A5BB90',slot='F8',function='6C3AC0'),writes=False,native_executed=False)
Path(a.out).write_text(json.dumps(result,indent=2)+'\n');print('G3 PlaceMortar retail code/slot/event-layout pins PASS (read-only)')
