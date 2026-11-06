#!/usr/bin/env python3
"""R125: CEntity::Hide pin (src/subsystems/ranged_reload/entity_hide_pin_r125.h) against a real WHGame.dll.

Reads the 16 code bytes at kHideRva and the CEntity vtable slot kEntityVftRva+kHideSlot from the image,
then compiles tests/entity_hide_pin_r125_test.cpp with those bytes and runs it: original bytes accepted,
the D77 mid hook shapes (E9 rel32 over the first 5 byte instruction, FF 25 absolute) accepted, the old
R124 exact compare refuses the E9 shape (the R124 HIDE_PIN_REFUSED), anything else refused.
"""
from pathlib import Path
import argparse,re,struct,subprocess,sys,json
p=argparse.ArgumentParser();p.add_argument('--dll',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
root=Path(__file__).resolve().parents[1];o=a.out.resolve();o.mkdir(parents=True,exist_ok=True)
h=(root/'src/subsystems/ranged_reload/entity_hide_pin_r125.h').read_text()
k=lambda n:int(re.search(n+r'=0x([0-9A-Fa-f]+)',h).group(1),16)
hide_rva,vft_rva,slot=k('kHideRva'),k('kEntityVftRva'),k('kHideSlot')
d=a.dll.read_bytes();e=struct.unpack_from('<I',d,0x3c)[0];nsec=struct.unpack_from('<H',d,e+6)[0];opt=struct.unpack_from('<H',d,e+20)[0]
stamp=struct.unpack_from('<I',d,e+8)[0];ib=struct.unpack_from('<Q',d,e+24+24)[0]
secs=[struct.unpack_from('<8sIIII',d,e+24+opt+40*i) for i in range(nsec)]
def off(rva):
 for n,vs,va,rs,ro in secs:
  if va<=rva<va+rs:return ro+rva-va
 raise KeyError(hex(rva))
code=d[off(hide_rva):off(hide_rva)+16];slotv=struct.unpack_from('<Q',d,off(vft_rva+slot))[0]-ib
inc=o/'entity_hide_pin_r125_dll.inc'
inc.write_text('static const unsigned char kDllCode[16]{%s};\nstatic const unsigned long long kDllStamp=0x%X,kDllSlotRva=0x%X;\n'%(','.join('0x%02X'%b for b in code),stamp,slotv))
exe=o/'entity_hide_pin_r125_test'
c=subprocess.run(['g++','-std=c++20','-O1','-Wall','-Werror','-I'+str(root/'src'),'-I'+str(o),str(root/'tests/entity_hide_pin_r125_test.cpp'),'-o',str(exe)],capture_output=True,text=True)
if c.returncode:print(c.stdout+c.stderr);sys.exit(2)
r=subprocess.run([str(exe)],capture_output=True,text=True);print(r.stdout,end='');(o/'run.log').write_text(r.stdout+r.stderr)
(o/'RESULTS.json').write_text(json.dumps({'dll':str(a.dll),'stamp':hex(stamp),'code':code.hex(),'vftSlotRva':hex(slotv),'returncode':r.returncode},indent=2)+'\n')
sys.exit(r.returncode)
