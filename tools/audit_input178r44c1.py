"""Read-only exact-retail input-selector audit; never loads or calls the game."""
from pathlib import Path
import argparse, hashlib, json
import pefile, capstone

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--dll',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
    a=p.parse_args();raw=a.dll.read_bytes();sha=hashlib.sha256(raw).hexdigest()
    assert sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
    pe=pefile.PE(data=raw);assert pe.FILE_HEADER.TimeDateStamp==0x6A350E20
    dis=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_64)
    report={'dll_sha256':sha,'timestamp':'0x6A350E20','native_execution':False,'functions':{}}
    for start in (0xD4AAC4,0xD4A99C):
        entry=next(x.struct for x in pe.DIRECTORY_ENTRY_EXCEPTION if x.struct.BeginAddress==start)
        data=pe.get_data(start,entry.EndAddress-start);ins=list(dis.disasm(data,start))
        assert sum(i.size for i in ins)==len(data)
        report['functions'][hex(start)]={'end_exclusive':hex(entry.EndAddress),'sha256':hashlib.sha256(data).hexdigest(),
            'instructions':[f'{i.address:X}: {i.mnemonic} {i.op_str}' for i in ins]}
    for start,call in ((0xD4AAC4,'call 0xd4aeb0'),(0xD4AAC4,'call 0x939310'),(0xD4A99C,'call 0xd4b06c')):
        assert any(call in x for x in report['functions'][hex(start)]['instructions'])
    report['interpretation']='The existing input path queries native action selection and eligibility. A recorded attack type is not itself proof that a new ordinary input cannot be attempted. This audit does not guarantee action acceptance or an NPC reaction.'
    a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2)+'\n')
    print('PASS exact retail input selector functions; native action acceptance remains unmeasured')

if __name__=='__main__':main()
