"""Exact-retail, read-only named dialog input-map evidence; no game calls."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse,hashlib,json,re,struct
from native_research112 import PE
def run(binary):
    p=PE(binary);sha=hashlib.sha256(p.data).hexdigest()
    assert p.stamp==0x6A350E20 and sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
    source=read_source(Path(__file__).resolve().parents[1]/'src/activity_lens59c10r1.cpp')
    source=source[source.index('bool Pins() noexcept'):source.index('void RequestDialogueFirstPerson19c2')]
    pins=[]
    for rva,literal,size in re.findall(r'\{(0x[0-9A-Fa-f]+),"((?:\\x[0-9A-Fa-f]{2})+)",(\d+)\}',source):
        expected=bytes.fromhex(literal.replace('\\x',' '));assert len(expected)==int(size)
        actual=p.read(int(rva,16),len(expected));assert actual==expected,(rva,expected.hex(),actual.hex())
        pins.append({'rva':rva,'size':len(expected),'match':True})
    table=[]
    for address,wanted in [(0x4048018+0x98,0xA10AF4),(0x4048530+0xD0,0x1A88910),
                            (0x4048530+0xC8,0x85ADF4),(0x4048018+0xE8,0x85AB44)]:
        actual=struct.unpack('<Q',p.read(address,8))[0]-p.base;assert actual==wanted
        table.append({'slot':hex(address),'target':hex(actual)})
    # Native owner616128 toggles dialog vs chat_interaction via actual
    # IActionMapManager::EnableActionMap. It does not inspect camera shots.
    for address,hexbytes in [(0x61613D,'381174'),(0x616141,'8811'),
                             (0x61616C,'448ac7480f44d1'),
                             (0x616173,'488b084c8b89e8000000488bc841ffd1')]:
        expected=bytes.fromhex(hexbytes);actual=p.read(address,len(expected));assert actual==expected,(hex(address),actual.hex())
        pins.append({'rva':hex(address),'size':len(expected),'match':True})
    for address,wanted in [(0x3A3E084,b'dialog\0'),(0x3A42518,b'chat_interaction\0')]:
        assert p.read(address,len(wanted))==wanted,(hex(address),p.read(address,len(wanted)))
    return {'sha256':sha,'timestamp':hex(p.stamp),'pins':pins,'tables':table,
            'semanticOwner':'616128 toggles dialog input map through native EnableActionMap85AB44',
            'runtimeExecution':False,'passed':True}
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('binary');parser.add_argument('--out',type=Path);a=parser.parse_args()
    result=run(a.binary);text=json.dumps(result,indent=2)+'\n'
    if a.out:a.out.write_text(text)
    print(text)
