"""Read retail camera/sequence clues; do not extract or modify game archives."""
from pathlib import Path
import argparse,zipfile,zlib,struct,re,json,hashlib,collections
RX=re.compile(rb'first.?person|fp.?camera|camera.?bone|bone.?camera|player.?camera|camera.?type|camera.?mode',re.I)
def read(z,info):
    try:return z.read(info)
    except zipfile.BadZipFile:
        # Some retail level PAKs use '/' in central names and '\\' in local
        # names. Read the declared stored/deflated payload and verify its CRC.
        z.fp.seek(info.header_offset);h=z.fp.read(30)
        if h[:4]!=b'PK\x03\x04' or info.flag_bits&1:raise
        a,b=struct.unpack_from('<HH',h,26);z.fp.seek(a+b,1)
        d=z.fp.read(info.compress_size)
        if info.compress_type==8:d=zlib.decompress(d,-15)
        elif info.compress_type!=0:raise ValueError('Unsupported compression')
        if len(d)!=info.file_size or zlib.crc32(d)&0xffffffff!=info.CRC:raise ValueError('CRC mismatch')
        return d
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--data',type=Path,required=True);ap.add_argument('--out',type=Path,required=True);a=ap.parse_args()
    report=[]
    for p in [a.data/'Scripts.pak',a.data/'IPL_GameData.pak',a.data/'Tables.pak',*a.data.glob('Levels/*/level.pak')]:
        hits=[];scanned=0;errors=[]
        with zipfile.ZipFile(p) as z:
            for info in z.infolist():
                if Path(info.filename).suffix.lower() not in {'.xml','.lua','.ent','.txt'}:continue
                try:d=read(z,info)
                except Exception as e:errors.append({'path':info.filename,'error':str(e)});continue
                scanned+=1;matches=list(RX.finditer(d))
                if matches:hits.append({'path':info.filename,'bytes':len(d),'sha256':hashlib.sha256(d).hexdigest(),'hit_count':len(matches),
                    'snippets':[d[max(0,m.start()-160):m.end()+180].decode('utf-8','replace') for m in matches[:8]]})
        report.append({'pak':str(p),'members_scanned':scanned,'errors':errors,'files':hits})
        a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2)+'\n')
        print(p.parent.name,p.name,'scanned',scanned,'matched',len(hits),'errors',len(errors),flush=True)
if __name__=='__main__':main()
