"""Read-only verification of a single native mod binary; never LoadLibrary.

Windows/VC runtime and the installed OpenXR runtime remain system prerequisites.
This checks link imports, six proxy exports, identity and embedded menu art;
it does not execute DllMain, game code or a VR runtime.
"""
from pathlib import Path
import argparse
import hashlib
import json
import struct

SYSTEM_DLLS = {
    'kernel32.dll','kernelbase.dll','ntdll.dll','user32.dll','gdi32.dll',
    'advapi32.dll','bcrypt.dll','crypt32.dll','secur32.dll','shell32.dll',
    'shlwapi.dll','ole32.dll','oleaut32.dll','version.dll','winmm.dll',
    'comctl32.dll','comdlg32.dll','imm32.dll','setupapi.dll','cfgmgr32.dll',
    'd3d11.dll','d3d12.dll','dxgi.dll','dxguid.dll','d3dcompiler_47.dll',
    'xinput1_4.dll','xinput9_1_0.dll','psapi.dll','ws2_32.dll','wintrust.dll',
    'ucrtbase.dll','msvcp140.dll','msvcp140_1.dll','msvcp140_2.dll',
    'msvcp140_atomic_wait.dll','vcruntime140.dll','vcruntime140_1.dll',
}
EXPORTS = {'DirectInput8Create':1,'DllCanUnloadNow':2,'DllGetClassObject':3,
           'DllRegisterServer':4,'DllUnregisterServer':5,'GetdfDIJoystick':6}

class Binary:
    def __init__(self,path):
        self.raw=Path(path).read_bytes()
        assert self.raw[:2]==b'MZ','Not a PE file'
        pe=self.u32(0x3c)
        assert self.raw[pe:pe+4]==b'PE\0\0'
        assert self.u16(pe+4)==0x8664,'Not x64'
        assert self.u16(pe+22)&0x2000,'Not a DLL'
        opt=pe+24
        assert self.u16(opt)==0x20b,'Not PE32+'
        self.base=struct.unpack_from('<Q',self.raw,opt+24)[0]
        self.dirs=[struct.unpack_from('<II',self.raw,opt+112+8*i) for i in range(min(self.u32(opt+108),16))]
        self.sections=[]
        for i in range(self.u16(pe+6)):
            at=opt+self.u16(pe+20)+40*i
            _,va,size,offset=struct.unpack_from('<IIII',self.raw,at+8)
            self.sections.append((va,size,offset))
    def u16(self,at): return struct.unpack_from('<H',self.raw,at)[0]
    def u32(self,at): return struct.unpack_from('<I',self.raw,at)[0]
    def at(self,rva,size=1):
        for va,n,off in self.sections:
            if va<=rva and rva+size<=va+n:return off+rva-va
        raise ValueError('Unmapped RVA '+hex(rva))
    def text(self,rva):
        at=self.at(rva)
        end=self.raw.index(b'\0',at,min(at+1024,len(self.raw)))
        return self.raw[at:end].decode('ascii')
    def imports(self):
        result=[]
        for index,width in [(1,20),(13,32)]:
            if index>=len(self.dirs):continue
            rva,size=self.dirs[index]
            if not rva:continue
            assert size>=width
            for i in range(min(size//width,4096)):
                at=self.at(rva+i*width,width)
                fields=struct.unpack_from('<'+'I'*(width//4),self.raw,at)
                if not any(fields):break
                name=fields[3] if index==1 else fields[1]
                if index==13 and not(fields[0]&1):name-=self.base
                result.append(self.text(name).lower())
            else: raise ValueError('Unterminated import table')
        return sorted(set(result))
    def exports(self):
        rva,size=self.dirs[0]
        if not rva:return {}
        at=self.at(rva,40)
        base,nfunc,nname,funcs,names,ordinals=struct.unpack_from('<IIIIII',self.raw,at+16)
        assert nname<=nfunc and nfunc<65536
        result={}
        for i in range(nname):
            name=self.text(self.u32(self.at(names+4*i,4)))
            slot=self.u16(self.at(ordinals+2*i,2))
            assert slot<nfunc and name not in result
            addr=self.u32(self.at(funcs+4*slot,4))
            assert addr and not(rva<=addr<rva+size),'Unexpected forwarded export '+name
            result[name]=base+slot
        return result
    def bitmap(self):
        root,size=self.dirs[2]
        assert root and size,'Resources missing'
        def entry(directory,wanted):
            at=self.at(root+directory,16)
            count=self.u16(at+12)+self.u16(at+14)
            assert count<=4096
            for i in range(count):
                name,target=struct.unpack_from('<II',self.raw,self.at(root+directory+16+8*i,8))
                if name==wanted:return target
            raise ValueError('Required resource not found: '+str(wanted))
        kind=entry(0,2)
        assert kind&0x80000000
        item=entry(kind&0x7fffffff,3101)
        assert item&0x80000000
        at=self.at(root+(item&0x7fffffff),24)
        assert self.u16(at+12)+self.u16(at+14)>0
        _,leaf=struct.unpack_from('<II',self.raw,at+16)
        assert not(leaf&0x80000000)
        data,n=struct.unpack_from('<II',self.raw,self.at(root+leaf,16))
        return self.raw[self.at(data,n):self.at(data,n)+n]

def verify(binary,marker,bitmap,asi=False):
    image=Binary(binary)
    assert marker and marker.encode('ascii') in image.raw,'Build marker missing'
    imports=image.imports()
    unknown=[n for n in imports if n not in SYSTEM_DLLS and not n.startswith(('api-ms-win-','ext-ms-win-'))]
    assert not unknown,'Non-system DLL dependency: '+', '.join(unknown)
    exported=image.exports()
    if not asi:
        assert all(exported.get(n)==v for n,v in EXPORTS.items()),'Proxy exports/ordinals missing or wrong'
    art=Path(bitmap).read_bytes()
    assert art[:2]==b'BM' and image.bitmap()==art[14:],'Embedded menu bitmap differs'
    return {'result':'PASS','binary':str(binary),'bytes':len(image.raw),
            'sha256':hashlib.sha256(image.raw).hexdigest(),'marker':marker,
            'type':'ASI' if asi else 'dinput8 proxy','x64':True,
            'link_imports':imports,'non_system_link_dependencies':unknown,
            'required_proxy_exports':{} if asi else EXPORTS,
            'embedded_menu_bitmap_sha256':hashlib.sha256(art).hexdigest(),
            'native_binary_executed':False,'game_or_headset_accepted':False,
            'companion_game_content':'separate existing/matching PAK required'}

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--binary',required=True,type=Path)
    ap.add_argument('--marker',required=True)
    ap.add_argument('--bitmap',type=Path,default=Path(__file__).resolve().parents[1]/'assets/MIS_MENU_BOOK.bmp')
    ap.add_argument('--report',required=True,type=Path)
    ap.add_argument('--asi',action='store_true')
    a=ap.parse_args()
    result=verify(a.binary,a.marker,a.bitmap,a.asi)
    a.report.parent.mkdir(parents=True,exist_ok=True)
    a.report.write_text(json.dumps(result,indent=2)+'\n')
    print('PASS: x64 '+result['type']+', no non-system DLL imports, embedded menu art, marker '+a.marker)

if __name__=='__main__':main()
