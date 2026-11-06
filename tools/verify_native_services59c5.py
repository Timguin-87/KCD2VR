"""Read-only validation of the exact retail service evidence and installed pins."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse, hashlib, json, re
from pathlib import Path
from native_research112 import PE

def main():
    ap=argparse.ArgumentParser();ap.add_argument('dll',type=Path);args=ap.parse_args()
    root=Path(__file__).resolve().parents[1]
    evidence=json.loads((root/'review/V66.178R59C5/native/manifest.json').read_text())
    pe=PE(str(args.dll));assert hashlib.sha256(pe.data).hexdigest()==evidence['retail_sha256'], 'retail fingerprint mismatch'
    for fn in evidence['functions']:
        rva=int(fn['rva'],16);assert pe.function(rva)==(rva,rva+fn['size'])
        assert hashlib.sha256(pe.read(rva,fn['size'])).hexdigest()==fn['sha256']
    source=read_source(root/'src/diagnostics/native_services_install59c5.inl')
    for name,rva in [('raised',0xC5BA24),('commit',0xC64B18),('effect',0xC66334),('counter',0x1483580)]:
        body=re.search(name+r'Entry\[\]\{([^}]+)\}',source).group(1)
        pin=bytes(int(x.strip(),16) for x in body.split(','));assert pe.read(rva,len(pin))==pin
    print('PASS: 9 function bodies, 4 hook pins and exact retail SHA-256; no native code executed')
if __name__=='__main__':main()
