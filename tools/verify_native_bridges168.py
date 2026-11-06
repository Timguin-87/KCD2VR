"""Read-only exact-retail verifier; never executes or modifies WHGame code.

Usage: python tools/verify_native_bridges168.py --dll PATH --out report.json
Python standard library only. Runtime callback tests are separate from this tool.
"""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse, hashlib, json, re, struct
from pathlib import Path
from native_research112 import PE

def verify(path: Path) -> dict:
    root=Path(__file__).resolve().parents[1]
    expected=json.loads((root/'review/V66.168/binary_contract168.json').read_text())
    pe=PE(path);digest=hashlib.sha256(pe.data).hexdigest()
    if digest!=expected['whgame_sha256'] or pe.stamp!=int(expected['timestamp'],16):
        raise ValueError('Unsupported retail DLL. No discovery/writes attempted.')
    for key,entry in expected['slices'].items():
        if hashlib.sha256(pe.read(int(entry['rva'],16),entry['size'])).hexdigest()!=entry['sha256']:
            raise ValueError(f'Changed code slice: {key}')
    def rel(at,target):
        b=pe.read(at,5)
        assert b[0] in (0xE8,0xE9) and at+5+struct.unpack('<i',b[1:])[0]==target,(hex(at),hex(target))
    rel(0x1483A8B,0x1483AF0);rel(0x1483A9D,0x1483B04);rel(0x1483AD8,0x4219F4)
    rel(0x1483AFE,0x9CB40C);rel(0x14836C2,0x1483A70);rel(0x7120A8,0xB35634)
    assert pe.read(0x1483A90,4)==bytes.fromhex('84c07410')
    assert pe.read(0x1483AD3,5)==bytes.fromhex('33d2488bcb')
    pins=[]
    source=read_source(root/'src/vrik/native_bridges168.inl')
    for rva,numbers in re.findall(r'\{(0x[0-9A-F]+),\{([^}]+)\}\}',source):
        wanted=bytes(int(v,16) for v in numbers.split(','))
        assert len(wanted)==16 and pe.read(int(rva,16),16)==wanted,rva
        pins.append(rva)
    assert len(pins)==10
    lifecycle=[]
    for rva,numbers,callback in re.findall(r'\{(0x[0-9A-F]+),\{([^}]+)\},(\w+)\}',read_source(root/'src/diagnostics/interaction_lifecycle_sites165.inl')):
        wanted=bytes(int(v,16) for v in numbers.split(','));assert pe.read(int(rva,16),len(wanted))==wanted,(rva,callback)
        lifecycle.append({'rva':rva,'callback':callback,'bytes':wanted.hex()})
    assert len(lifecycle)==21
    camera=[]
    for name,reg,storage,help_rva in [
       ('wh_cs_playerinputlockareawidth',0x10A9A7A,0x108,0x3E446D8),
       ('wh_cs_playerinputcombatunlockdelay',0x10A96FA,0x120,0x3E43FE0),
       ('wh_cs_playerinputlockingtolerance',0x10A9250,0xB4,0x3E43070),
       ('wh_cs_playerinputcursorunlockdistance',0x10A96C8,0x11C,0x3E44058)]:
        data=pe.read(reg,7);assert data[:3]==bytes.fromhex('488d0d'),hex(reg)
        addr=reg+7+struct.unpack('<i',data[3:])[0]
        actual_name=pe.read(addr,160).split(b'\0')[0].decode('ascii')
        assert actual_name.lower()==name,name
        help_text=pe.read(help_rva,512).split(b'\0')[0].decode('utf-8',errors='replace')
        camera.append({'name':name,'registration':hex(reg),'storage_offset':hex(storage),'help':help_text,'runtime_mutation':'NONE'})
    return {'verified':True,'whgame_sha256':digest,'timestamp':hex(pe.stamp),'master_runtime_pins':pins,
      'slices':expected['slices'],'lifecycle_sites':lifecycle,'camera_cvars':camera,
      'interpretation':'AL is action-group membership, not proof of final masterstrike admission. Filter is rejection-only at one known factory; retail timing and output construction remain native. No native code executed by this tool.'}

if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('--dll',type=Path,required=True);ap.add_argument('--out',type=Path,required=True)
    args=ap.parse_args()
    try: report=verify(args.dll)
    except (OSError,ValueError,AssertionError,struct.error) as exc: ap.exit(1,f'FAIL: {exc}\n')
    args.out.parent.mkdir(parents=True,exist_ok=True);args.out.write_text(json.dumps(report,indent=2)+'\n')
    print('PASS exact DLL SHA/timestamp, 10 master pins, native branch/call targets, 21 lifecycle pins, 4 KCD2 CVar registrations. Read-only.')
