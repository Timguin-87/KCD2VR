"""Read-only exact-retail check of F11 manual ammunition attachment boundaries."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse,hashlib,json,re,struct
from pathlib import Path
from verify_native_grasp13 import PE,EXPECTED_SHA,EXPECTED_TS
p=argparse.ArgumentParser();p.add_argument('--dll',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
r=Path(__file__).resolve().parents[1];pe=PE(a.dll);checks=[]
def add(n,got,want):checks.append(dict(name=n,actual=got,expected=want,passed=got==want))
add('exact DLL SHA256',hashlib.sha256(pe.data).hexdigest(),EXPECTED_SHA);add('timestamp',pe.timestamp,EXPECTED_TS)
s=read_source(r/'src/vrik/staged_leaf_pins596c2.inl').split('kReloadLeaves596[]{',1)[1].split('};',1)[0]
for at,raw in re.findall(r'\{0x([\dA-Fa-f]+),\{([^}]+)\}\}',s):
 b=bytes(int(x,16) for x in re.findall(r'0x([\dA-Fa-f]+)',raw));add('compiled reload leaf '+at,pe.at(int(at,16),len(b)).hex(),b.hex())
for n,vf,slot,target in [('C_Player actor-model',0x3AA4B98,0x610,0x1A81DD0),('attachment current object',0x3A657C0,0xE0,0x19A2A90)]:add(n,pe.q(vf+slot)-pe.base,target)
for n,at,want in [
 ('actor-model exact member',0x1A81DD0,'488b81b0010000c3'),('attachment helper model owner',0x1085F7A,'488b4920'),
 ('attachment current object field',0x19A2A90,'488b4120c3'),('CEntityAttachment current entity ID',0x4238D4,'8b5708'),
 ('visual weapon passed to ammo factory',0x2A8A067,'488b5358'),('created nock entity field',0x2A8A0A7,'8983dc000000'),
 ('native model hand slot only after submission',0x10852F2,'44897cb72c'),
 ('ammo attachment bound through native attachment',0x1F19DEC,'ff90d0000000'),
]:add(n,pe.at(at,len(bytes.fromhex(want))).hex(),want)
for n,at,target in [('hand creates attachment operation',0x10852ED,0x1085178),('find native arrow attachment',0x2A8A055,0x2A89B1C),('create native nock visual',0x2A8A07E,0x289F48C),('native nock attachment wrapper',0x2A8A09F,0x1F17100),('native entity attachment ctor',0x1F19DD3,0x9DFD44)]:
 b=pe.at(at,5);got=at+5+struct.unpack('<i',b[1:])[0] if b[0]==0xE8 else -1;add(n,got,target)
for at,want in [(0x40AC298,'entity_right'),(0x40AC2A0,'entity_left')]:add('native ammunition helper '+hex(at),pe.cstr(pe.q(at)-pe.base),want)
for vf,name in [(0x3A68F68,'.?AUCEntityAttachment@@'),(0x3A657C0,'.?AVCAttachmentBONE@@')]:
 col=pe.q(vf-8)-pe.base;td=struct.unpack('<I',pe.at(col+12,4))[0];add('retail RTTI '+hex(vf),pe.cstr(td+16),name)
out=dict(all_passed=all(c['passed'] for c in checks),passed=sum(c['passed'] for c in checks),total=len(checks),nativeExecution=False,checks=checks)
a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(out,indent=2)+'\n');print({k:v for k,v in out.items() if k!='checks'})
for c in checks:
 if not c['passed']:print('FAIL',c)
raise SystemExit(0 if out['all_passed'] else 1)
