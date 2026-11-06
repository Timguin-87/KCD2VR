"""Exact retail native torch callback/receiver/filter proof; no native execution."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse, hashlib, json, re, struct
from pathlib import Path
from audit_ui175 import PE,EXPECTED_SHA256

def audit(dll):
 pe=PE(dll);sha=hashlib.sha256(pe.data).hexdigest();assert sha==EXPECTED_SHA256 and pe.timestamp==0x6A350E20
 header=read_source(Path(__file__).resolve().parents[1]/'src/vrik/native_torch62.h');checks=[]
 def pin(name,rva,data):
  ok=pe.read(rva,len(data))==data;checks.append({'name':name,'rva':hex(rva),'size':len(data),'passed':ok});assert ok,name
 def rip(name,at,target,opcode=b'\x48\x8d\x15'):
  pin(name,at,opcode+struct.pack('<i',target-at-7))
 for name,rva in [('Handler',0x2AADFA8),('Thunk',0x1F248B0),('Filter',0x35742A4),('GetCharacter',0x6411AC)]:
  values=re.search(r'k'+name+r'Entry\[\]\{([^}]+)',header).group(1)
  pin('production '+name,rva,bytes(int(x.strip(),16) for x in values.split(',')))
 for name,rva in [('keyboard closure',0x3EC9258),('XI closure',0x3EC9290)]:
  pin(name+' execute same this-only thunk',rva+0x10,struct.pack('<Q',pe.image_base+0x1F248B0))
 pin('exact CEntity GetCharacter slot328',0x4072C78+0x328,struct.pack('<Q',pe.image_base+0x6411AC))
 pin('exact filter getter slot108',0x4048018+0x108,struct.pack('<Q',pe.image_base+0x35742A4))
 rip('toggle_torch registration',0x100E828,0x3ECA6A0)
 rip('keyboard closure registration',0x100E836,0x3EC9258,b'\x48\x8d\x05')
 rip('xi_toggle_torch registration',0x100E8A7,0x3ECA770)
 rip('XI closure constructor',0x1F1E738,0x3EC9290,b'\x48\x8d\x05')
 pin('XI closure captures registered input receiver',0x1F1E742,bytes.fromhex('488b0248894108'))
 pin('C_PlayerInput ctor stores player at10',0x799EB9,bytes.fromhex('48895110'))
 rip('C_PlayerInput exact primary vft',0x799EBD,0x3EC99A8,b'\x48\x8d\x05')
 pin('callback mode eligibility 0',0x2AADFB9,bytes.fromhex('33d2488b9890090000488bcb488b03ff9098020000'))
 pin('callback native stow call',0x2AAE02A,b'\xe8'+struct.pack('<i',0x8EEA24-0x2AAE02F))
 pin('callback native draw flag and full wrapper',0x2AAE031,b'\xb2\x01\xe8'+struct.pack('<i',0x2AA2330-0x2AAE038))
 pin('draw checks native item manager',0x2AA2330,bytes.fromhex('48895c2408488974241048897c241855')) # exact entry; remaining native branches retain full-image qualification
 return {'sha256':sha,'timestamp':hex(pe.timestamp),'passed':all(c['passed'] for c in checks),'checks':checks,
  'signature':'void C_PlayerInput::registeredToggleTorch() ; std::function<void()> closure thunk, RCX only',
  'native_calls_by_audit':0,'runtime_calls':'current GetCharacter(0), IsFilterEnabled(no_torch), then full registered callback once if qualified',
  'limits':'Native method return is not a draw/stow or inventory success acknowledgment; no torch melee/block eligibility is inferred.'}
if __name__=='__main__':
 ap=argparse.ArgumentParser();ap.add_argument('--dll',required=True);ap.add_argument('--out',required=True);a=ap.parse_args()
 r=audit(a.dll);Path(a.out).parent.mkdir(parents=True,exist_ok=True);Path(a.out).write_text(json.dumps(r,indent=2)+'\n');print('PASS torch exact-retail %d checks'%len(r['checks']))
