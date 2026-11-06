"""Read-only exact-retail PE research. No target code is loaded or executed.

Uses Python stdlib and GNU objdump; addresses in output are module-relative RVAs.
Raw byte searches are candidates only. Function-boundary disassembly validates them.
"""
from pathlib import Path
import argparse, bisect, hashlib, json, re, struct, subprocess

EXPECTED = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'

class PE:
    def __init__(self, path):
        self.path = Path(path).resolve(); self.data = self.path.read_bytes()
        h = struct.unpack_from('<I', self.data, 0x3c)[0]
        assert self.data[h:h+4] == b'PE\0\0'
        self.stamp = struct.unpack_from('<I', self.data, h+8)[0]
        self.base = struct.unpack_from('<Q', self.data, h+48)[0]
        count, size = struct.unpack_from('<H', self.data, h+6)[0], struct.unpack_from('<H', self.data, h+20)[0]
        self.sections = []
        for i in range(count):
            q = h+24+size+i*40
            virtualsize, va, rawsize, raw = struct.unpack_from('<IIII', self.data, q+8)
            flags=struct.unpack_from('<I',self.data,q+36)[0]
            self.sections.append((self.data[q:q+8].rstrip(b'\0').decode(),va,rawsize,raw,flags))
        er, es = struct.unpack_from('<II',self.data,h+24+112+24)
        self.functions = [struct.unpack_from('<III', self.data, self.offset(er)+i)[:2] for i in range(0,es,12)]
        self.starts = [f[0] for f in self.functions]
        assert hashlib.sha256(self.data).hexdigest() == EXPECTED
        assert self.stamp == 0x6A350E20 and self.base == 0x180000000
        self.cache={}
    def offset(self,rva,size=1):
        for _,va,n,raw,_ in self.sections:
            if va <= rva and rva+size <= va+n: return raw+rva-va
        raise ValueError(hex(rva))
    def read(self,rva,size):
        o=self.offset(rva,size);return self.data[o:o+size]
    def function(self,rva):
        i=bisect.bisect_right(self.starts,rva)-1
        return self.functions[i] if i>=0 and rva<self.functions[i][1] else None
    def section_rva(self,offset):
        for _,va,n,raw,_ in self.sections:
            if raw<=offset<raw+n:return va+offset-raw
    def locations(self,needle,code_only=False):
        for _,va,n,raw,flags in self.sections:
            if code_only and not (flags&0x20000000):continue
            sec=self.data[raw:raw+n];at=-1
            while True:
                at=sec.find(needle,at+1)
                if at<0:break
                yield va+at
    def calls(self,target):
        for r in self.locations(b'\xe8',True):
            if r+5+struct.unpack('<i',self.read(r+1,4))[0]==target:yield r
    def pointers(self,target):return self.locations(struct.pack('<Q',self.base+target))
    def rip_xrefs(self,targets):
        """Common 64-bit LEA/MOV RIP-address candidates; boundary-validated."""
        out=[]
        for rex in (0x48,0x4c):
            for opcode in (0x8d,0x8b,0x89):
                for modrm in (0x05,0x0d,0x15,0x1d,0x25,0x2d,0x35,0x3d):
                    for at in self.locations(bytes((rex,opcode,modrm)),True):
                        target=at+7+struct.unpack('<i',self.read(at+3,4))[0]
                        if target not in targets:continue
                        f=self.function(at)
                        if not f:continue
                        ins=self.disasm(*f)
                        i=next((j for j,x in enumerate(ins) if x['rva']==at),None)
                        if i is not None:
                            out.append({'target':hex(target),'rva':hex(at),'function':list(map(hex,f)),
                                        'context':ins[max(0,i-6):i+9]})
        return out
    def disasm(self,a,b=None):
        if b is None:
            f=self.function(a);assert f,(hex(a),'no unwind function')
            a,b=f
        if (a,b) not in self.cache:
            s=subprocess.check_output(['objdump','-D','-w','-Mintel',f'--start-address={self.base+a}',f'--stop-address={self.base+b}',str(self.path)],text=True)
            lines=[]
            for line in s.splitlines():
                m=re.match(r'^\s*([0-9a-f]+):\s+((?:[0-9a-f]{2}\s+)+)\s*(.*)$',line)
                if m:
                    rva=int(m[1],16)-self.base
                    lines.append({'rva':rva,'bytes':''.join(m[2].split()),'asm':m[3].strip()})
            self.cache[a,b]=lines
        return self.cache[a,b]
    def listing(self,a,b=None):
        return '\n'.join(f'{x["rva"]:08X}  {x["bytes"]:<32} {x["asm"]}' for x in self.disasm(a,b))+'\n'
    def boundary_callers(self,target):
        result=[]
        for rva in self.calls(target):
            f=self.function(rva)
            if not f:continue
            if any(x['rva']==rva and x['asm'].startswith('call') for x in self.disasm(*f)):
                result.append({'rva':hex(rva),'function':list(map(hex,f))})
        return result
    def field_scan(self,field):
        needle=struct.pack('<I',field);out=[];unbounded=[]
        for rva in self.locations(needle,True):
            f=self.function(rva)
            if not f:unbounded.append(hex(rva));continue
            ins=self.disasm(*f)
            for i,x in enumerate(ins):
                if x['rva']<=rva<x['rva']+len(x['bytes'])//2 and re.search(r'\+0x'+format(field,'x')+r'(?:\]|\,)',x['asm']):
                    out.append({'displacement_rva':hex(rva),'function':list(map(hex,f)),
                                'instruction':x,'context':ins[max(0,i-15):i+7]})
        return {'field':hex(field),'validated':out,'no_unwind_range_candidates':unbounded}

def main():
    ap=argparse.ArgumentParser();ap.add_argument('dll');ap.add_argument('--out',type=Path,required=True)
    ap.add_argument('--field',type=lambda x:int(x,0));ap.add_argument('--function',type=lambda x:int(x,0),action='append',default=[])
    a=ap.parse_args();p=PE(a.dll);a.out.mkdir(parents=True,exist_ok=True)
    (a.out/'identity.json').write_text(json.dumps({'sha256':EXPECTED,'timestamp':hex(p.stamp),'image_base':hex(p.base),'method':'read-only PE/objdump disassembly, no native execution'},indent=2)+'\n')
    if a.field is not None:
        results=p.field_scan(a.field)
        (a.out/f'field_{a.field:X}.json').write_text(json.dumps(results,indent=2)+'\n')
        for h in results['validated']:print(h['function'],hex(h['instruction']['rva']),h['instruction']['asm'])
        print('Unbounded raw candidates:',results['no_unwind_range_candidates'])
    for rva in a.function:
        f=p.function(rva);(a.out/f'{rva:X}.asm').write_text(p.listing(rva))
        callers=list(p.calls(rva))
        qualified=p.boundary_callers(rva) if len(callers)<50 else {'raw_count':len(callers),'not_qualified':'common helper, bounded investigation only'}
        print(hex(rva),list(map(hex,f)),'callers',qualified,'pointers',list(map(hex,p.pointers(rva))))

if __name__=='__main__':main()
