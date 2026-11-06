"""C37 read-only check: every kReloadCallerPins37/kReloadSlotPins37 entry in the
source equals the exact retail WHGame bytes. Stdlib only; never loads the DLL."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse,hashlib,re,struct
SHA='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--dll',type=Path,required=True);a=ap.parse_args()
    d=a.dll.read_bytes();assert hashlib.sha256(d).hexdigest()==SHA,'Wrong retail DLL'
    e=struct.unpack_from('<I',d,0x3C)[0];n=struct.unpack_from('<H',d,e+6)[0];o=struct.unpack_from('<H',d,e+20)[0]
    base=struct.unpack_from('<Q',d,e+48)[0];secs=[]
    for i in range(n):
        s=e+24+o+40*i;vs,va,rs,ra=struct.unpack_from('<IIII',d,s+8);secs.append((va,max(vs,rs),ra))
    def off(rva):return next(ra+rva-va for va,sz,ra in secs if va<=rva<va+sz)
    src=read_source(Path(__file__).resolve().parents[1]/'src/diagnostics/reload_lifecycle178r36c2.inl')
    table=src[src.index('kReloadCallerPins37[]{'):src.index('};',src.index('kReloadCallerPins37[]{'))]
    pins=re.findall(r'\{0x([0-9A-F]+),"([0-9a-f]+)"\}',table);assert len(pins)==14
    for rva,hx in pins:
        r=int(rva,16);assert d[off(r):off(r)+len(hx)//2].hex()==hx,rva
    slots=src[src.index('kReloadSlotPins37[]{'):src.index('};',src.index('kReloadSlotPins37[]{'))]
    pairs=re.findall(r'\{0x([0-9A-F]+),0x([0-9A-F]+)\}',slots);assert len(pairs)==2
    for s,t in pairs:assert struct.unpack_from('<Q',d,off(int(s,16)))[0]-base==int(t,16),s
    print(f'PASS: {len(pins)} C37 caller-context pins and {len(pairs)} std::function _Do_call slots match retail WHGame.')
if __name__=='__main__':main()
