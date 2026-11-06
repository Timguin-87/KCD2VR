"""Read-only exact-retail activity-eye / native ShowPage evidence.

This verifies production pins and actual RTTI/constructor writes. It is not a
headset test and makes no claim that a layout transaction produced visible pixels.
"""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse
import hashlib
import json
import re
import struct
from native_research112 import PE


def run(binary):
    pe=PE(binary)
    sha=hashlib.sha256(pe.data).hexdigest()
    assert sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
    assert pe.stamp==0x6A350E20
    root=Path(__file__).resolve().parents[1]
    source=read_source(root/'src/flashui_probe.cpp')
    source=source[source.index('bool PagePins19('):source.index('bool InstallPageObserver19() noexcept {')]
    rows=[]
    for name,rva in [('entry',0x1F8DB80),('invoke',0x1F8DBA2),('element',0x566BE2)]:
        data=re.search(r'unsigned char '+name+r'\[\]\{([^}]+)\}',source).group(1)
        expected=bytes(int(v.strip(),0) for v in data.split(','))
        actual=pe.read(rva,len(expected))
        rows.append(dict(name=name,rva=hex(rva),expected=expected.hex(),actual=actual.hex(),match=expected==actual))
    assert pe.read(0x3EF0240,9)==b'ShowPage\0'
    types=[]
    for vft,want,offset in [(0x3A81028,'.?AVC_Player@entitymodule@wh@@',0x40),
                            (0x3AA4B98,'.?AVC_Player@entitymodule@wh@@',0),
                            (0x3A80610,'.?AVC_CombatPlayer@combatmodule@wh@@',0)]:
        locator=struct.unpack('<Q',pe.read(vft-8,8))[0]-pe.base
        sig,off,_,desc,_,own=struct.unpack('<6I',pe.read(locator,24))
        name=pe.read(desc+16,150).split(b'\0')[0].decode('ascii')
        assert sig==1 and own==locator and off==offset and name==want
        types.append(dict(vtable=hex(vft),type=name,subobject=hex(off)))
    # Constructors establish that receiver-40 IS C_Player, not C_CombatPlayer.
    for rva,expected in [(0xBC7840,'488d05e197eb0248894340'),
                         (0xDA777F,'488d05a298cd0248894140'),
                         (0x7F2335,'488d4bc0488b01ff9040020000'),
                         (0x2EB3F7E,'f20f110ff30f7f470c894708'),
                         (0x2EB4653,'f20f110e44897608f30f7f460c')]:
        data=bytes.fromhex(expected);actual=pe.read(rva,len(data))
        rows.append(dict(name='layout_or_pose_writer',rva=hex(rva),expected=expected,actual=actual.hex(),match=data==actual))
    producer_chains=[]
    for file,receiver in [('activity_lens59c10r1.h','viewSelf'),('dialogue_state59c10r2.h','self')]:
        text=read_source(root/'src'/file)
        required=[f'player={receiver}-0x40', 'read(player+0x278,actor)',
                  'read(actor+0x1F8,back)', 'back!=player']
        assert all(term in text for term in required)
        assert f'const auto actor={receiver}-0x40' not in text
        producer_chains.append(dict(file=file,source_sha256=hashlib.sha256(text.encode()).hexdigest(),
                                    retail_receiver='C_Player+0x40',combat_backlink='player+278 -> actor+1F8 -> player'))
    # Reading/sharpening secondary camera-interface +10 dispatches these pose writers.
    for slot,target in [(0x3F74B78,0x2EB3DCC),(0x3F74D30,0x2EB3FAC)]:
        actual=struct.unpack('<Q',pe.read(slot,8))[0]-pe.base
        rows.append(dict(name='activity_camera_slot',rva=hex(slot),target=hex(target),actual=hex(actual),match=actual==target))
    # Incoming relative branches are audited against the actual entry overwrite.
    # First two instructions are 2-byte push rbx + 4-byte sub rsp, no RIP operand.
    assert pe.read(0x1F8DB80,6)==bytes.fromhex('40534883ec20')
    target=0x1F8DB80;incoming=[]
    _,va,size,raw=next(s for s in pe.sections if s[0]==b'.text')
    code=pe.data[raw:raw+size]
    opcodes=[b'\xe8',b'\xe9']+[bytes([0x0F,n]) for n in range(0x80,0x90)]
    for opcode in opcodes:
        at=-1;length=len(opcode)+4
        while True:
            at=code.find(opcode,at+1)
            if at<0 or at+length>len(code):break
            dest=va+at+length+struct.unpack_from('<i',code,at+len(opcode))[0]
            if target<=dest<target+16:
                incoming.append(dict(rva=hex(va+at),opcode=opcode.hex(),target=hex(dest)))
    assert incoming and all(int(b['target'],16)==target for b in incoming)
    result=dict(sha256=sha,timestamp=hex(pe.stamp),pins=rows,receiver_types=types,
        page_entry_incoming=incoming,show_page_function=[hex(v) for v in pe.function(target)],
        producer_chains=producer_chains,passed=all(r['match'] for r in rows),runtime_execution=False)
    assert result['passed']
    return result


if __name__=='__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument('binary',type=Path)
    parser.add_argument('--out',type=Path)
    args=parser.parse_args()
    result=run(args.binary)
    text=json.dumps(result,indent=2)+'\n'
    if args.out:args.out.write_text(text,encoding='utf-8')
    print(text,end='')
