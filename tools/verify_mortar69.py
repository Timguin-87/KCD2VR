"""Read-only exact retail validation of G2 mortar pins; never loads native code."""
from pathlib import Path
import argparse,json,re
from smith_native_probe46 import PE,EXPECTED
p=argparse.ArgumentParser();p.add_argument('--dll',required=True);p.add_argument('--out',required=True);a=p.parse_args()
pe=PE(a.dll);root=Path(__file__).resolve().parents[1];rows=[]
for file,name,rva in [('src/vrik/alchemy_tool18.inl','mortarHeld69',0x8D237C),('src/vrik/alchemy_tool18.inl','mortarPhase69',0xA97A9A),('src/subsystems/alchemy/native_mortar69.inl','nativeDestination69',0x8D20DC)]:
 text=(root/file).read_text();m=re.search(r'constexpr unsigned char '+name+r'\[\]\{([^}]+)\}',text);assert m,name
 wanted=bytes(int(v,16) for v in re.findall(r'0x([0-9a-fA-F]{2})',m[1]));actual=pe.read(rva,len(wanted));assert actual==wanted,(name,actual.hex());rows.append(dict(name=name,rva=hex(rva),bytes=actual.hex(),ok=True))
result=dict(ok=True,sha256=EXPECTED,timestamp=hex(pe.stamp),pins=rows,native_executed=False,writes=False)
Path(a.out).write_text(json.dumps(result,indent=2)+'\n');print('G2 mortar:3 exact-retail pins PASS; read-only')
