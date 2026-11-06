"""Exact-retail static audit and reproducible prediction hook byte pins."""
import argparse, hashlib, json, struct
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_64
from native_research112 import PE
from research_prediction119 import EXPECTED, CALLS

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--whgame',type=Path,required=True)
    ap.add_argument('--out',type=Path,required=True);ap.add_argument('--header',type=Path)
    a=ap.parse_args();pe=PE(a.whgame)
    assert hashlib.sha256(pe.data).hexdigest()==EXPECTED and pe.stamp==0x6A350E20
    cs=Cs(CS_ARCH_X86,CS_MODE_64)
    ins=list(cs.disasm(pe.read(0xA7C624,0x46C),pe.base+0xA7C624))
    # Every use of the borrowed RDI until the real actor replaces it, including
    # the terminal player read. No stores, escapes or calls through this pointer.
    borrowed=[(i.address-pe.base,i.mnemonic,i.op_str) for i in ins if 0xA7C65F<=i.address-pe.base<0xA7C8AE and 'rdi' in i.op_str]
    assert borrowed==[(0xA7C65F,'movsd','xmm0, qword ptr [rdi + 0x154]'),
        (0xA7C667,'lea','r13, [rdi + 0x74]'),(0xA7C671,'lea','r15, [rdi + 0x16c]'),
        (0xA7C68B,'mov','ebx, dword ptr [rdi + 0x15c]'),(0xA7C89F,'mov','rax, qword ptr [rdi]')]
    assert next(i.op_str for i in ins if i.address-pe.base==0xA7C8AE)=='rdi, qword ptr [rax + 0x278]'
    assert struct.unpack('<f',pe.read(0xA7C8AE+0x363BFBE,4))[0]>2.09
    for site,target in CALLS.items():
        raw=pe.read(site,5);assert raw[0]==0xE8 and site+5+struct.unpack('<i',raw[1:])[0]==target
    pins={0xA7C624:16,0xA7C65F:12,0xA7CA90:16,0xA7CDE9:5,0xA7C968:5,0xA7C990:5,0xA7C9BC:5}
    header=['// Generated from exact SHA by tools/research_prediction120.py. Do not hand-edit.',
        'static bool PredictionContract120(std::uintptr_t b) noexcept {',
        '    if(!b||!SupportedWhBuild(b)||!retail_build112::Ready(b))return false;']
    records=[]
    for n,(rva,size) in enumerate(pins.items()):
        raw=pe.read(rva,size);records.append(dict(rva=f'{rva:X}',bytes=raw.hex()))
        header += [f'    const unsigned char expected{n}[]{{'+','.join(f'0x{x:02X}' for x in raw)+'};',
            f'    unsigned char actual{n}[{size}]{{}};',
            f'    if(!Read(reinterpret_cast<void*>(b+0x{rva:X}),actual{n})||std::memcmp(actual{n},expected{n},{size}))return false;']
    header+=['    return true;','}']
    a.out.mkdir(parents=True,exist_ok=True)
    generated='\n'.join(header)+'\n'
    (a.out/'prediction_contract120.inl').write_text(generated,encoding='utf-8')
    if a.header:a.header.write_text(generated,encoding='utf-8')
    report=dict(result='PASS',sha256=EXPECTED,timestamp='6A350E20',borrowed_rdi_reads=borrowed,pins=records,
        limitations=['Static structure only; socket equality and scheduling require runtime correlation.',
        'No runtime injection, native hit acceptance or performance was measured.'])
    (a.out/'static-contract.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(dict(result='PASS',borrowed_reads=len(borrowed),pins=len(pins),call_edges=len(CALLS))))

if __name__=='__main__':main()
