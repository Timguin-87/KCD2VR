"""Read-only exact-retail proof that R42's inferred registry is a stack frame.
Requires pefile and capstone. Never loads WHGame or writes game memory.
"""
from pathlib import Path
import argparse, hashlib, json
import pefile, capstone

def main():
    p=argparse.ArgumentParser();p.add_argument('--dll',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
    a=p.parse_args();raw=a.dll.read_bytes();sha=hashlib.sha256(raw).hexdigest()
    assert sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
    pe=pefile.PE(data=raw);assert pe.FILE_HEADER.TimeDateStamp==0x6A350E20
    d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_64)
    ranges={'registry_prologue':(0xCE3F40,0xCE3F5B),'window_registration':(0xCE4CD3,0xCE4D43),'registration_helper':(0x6DD3D0,0x6DD465)}
    report={'dll_sha256':sha,'timestamp':'0x6A350E20','native_execution':False,'ranges':{}}
    for name,(start,end) in ranges.items():
        data=pe.get_data(start,end-start);ins=list(d.disasm(data,start))
        assert sum(i.size for i in ins)==len(data)
        report['ranges'][name]={'start':hex(start),'end_exclusive':hex(end),'sha256':hashlib.sha256(data).hexdigest(),
            'instructions':[f'{i.address:X}: {i.mnemonic} {i.op_str}' for i in ins]}
    assert any(
        'lea rbp, [rsp - 0x13fa0]' in x for x in report['ranges']['registry_prologue']['instructions'])
    for text in ('lea rdx, [rbp + 0x12fe0]','mov r9d, 0x100','lea rdx, [rbp + 0x12fa0]','mov r9d, 0x104'):
        assert any(text in x for x in report['ranges']['window_registration']['instructions']),text
    report['conclusion']='node - (0x13FE0 - 0x10*index) equals this function\'s RBP stack anchor, not a persistent registry. No value array or per-player timing-write boundary is established.'
    a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2)+'\n')
    print('PASS exact retail: R42 inferred registry is the CE3F40 stack frame; runtime writes NONE')
if __name__=='__main__':main()
