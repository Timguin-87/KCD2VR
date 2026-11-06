"""Offline exact-retail visor wrapper evidence; never loads or executes WHGame."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse,hashlib,json,re,struct
from pathlib import Path
from audit_ui175 import PE,EXPECTED_SHA256

def audit(dll):
 pe=PE(dll);digest=hashlib.sha256(pe.data).hexdigest()
 assert digest==EXPECTED_SHA256 and pe.timestamp==0x6A350E20
 header=read_source(Path(__file__).resolve().parents[1]/'src/vrik/utility_gestures54.h')
 checks=[]
 def pin(name,rva,expected):
  ok=pe.read(rva,len(expected))==expected;checks.append({'name':name,'rva':hex(rva),'passed':ok});assert ok,name
 for kind,rva,slot in [('Open',0x9F0D74,0x5B0),('Close',0x2AA1358,0x5B8)]:
  raw=re.search(r'kVisor'+kind+r'Entry54\[\]\{([^}]+)',header).group(1)
  code=bytes(int(x.strip(),16) for x in raw.split(','))
  pin(kind+' full pinned entry',rva,code)
  pin(kind+' actual C_Player vtable',0x3AA4B98+slot,struct.pack('<Q',pe.image_base+rva))
 pin('closeVisorOn actual callback actor dispatch',0x29DFD21,bytes.fromhex('488b91b8050000'))
 pin('raiseVisorOn actual callback actor dispatch',0x29E0551,bytes.fromhex('488b91b0050000'))
 for name,site,target,opcode in [('close command registration',0x100E151,0x29DFC98,0x48),('raise command registration',0x100E16E,0x29E04C8,0x48)]:
  pin(name,site,bytes.fromhex('488d15')+struct.pack('<i',target-site-7))
 return {'sha256':digest,'timestamp':'0x6A350E20','checks':checks,'passed':all(x['passed'] for x in checks),'limit':'Static call/identity evidence only. No native execution or visible visor completion is claimed.'}
if __name__=='__main__':
 ap=argparse.ArgumentParser();ap.add_argument('--dll',required=True);ap.add_argument('--out',required=True);a=ap.parse_args();r=audit(a.dll);Path(a.out).write_text(json.dumps(r,indent=2)+'\n');print('PASS visor exact identity + %d pins'%len(r['checks']))
