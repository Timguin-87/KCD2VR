"""Read-only exact-retail defense intent, timing, zone and activation proof."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse,hashlib,json,re,struct
from pathlib import Path
from verify_native_grasp13 import PE,EXPECTED_SHA,EXPECTED_TS
ROOT=Path(__file__).resolve().parents[1]
def verify(path):
    pe=PE(path);checks=[]
    def add(name,a,e):checks.append(dict(name=name,actual=a,expected=e,passed=a==e))
    add('SHA256',hashlib.sha256(pe.data).hexdigest(),EXPECTED_SHA);add('timestamp',hex(pe.timestamp),hex(EXPECTED_TS))
    # Henry and NPC combat actors are different concrete retail types. A
    # fixture accepting only the NPC vtable concealed an always-refused Henry
    # intent transaction in C4/C4R1.
    for vf,expected in [(0x3A80610,'.?AVC_CombatPlayer@combatmodule@wh@@'),
                        (0x3E3FAC8,'.?AVC_CombatActor@combatmodule@wh@@'),
                        (0x3AA4B98,'.?AVC_Player@entitymodule@wh@@')]:
      col=pe.q(vf-8)-pe.base
      sig,offset,_,desc,_,selfrva=struct.unpack('<6I',pe.at(col,24))
      add('RTTI identity '+hex(vf),pe.cstr(desc+16),expected)
      add('RTTI primary object '+hex(vf),(sig,offset,selfrva),(1,0,col))
    source=read_source(ROOT/'src/diagnostics/native_defense_offer19c4.inl')
    arrays={n:bytes(int(v,16) for v in re.findall(r'0x([a-fA-F0-9]+)',x)) for n,x in re.findall(r'constexpr unsigned char (\w+)\[\]\{([^}]+)\}',source)}
    locations=dict(kLeadEntry=0xA73A04,kLeadCall=0x9142DF,kOfferPostEntry=0x91432E,kIntentProducer=0xB27446,kIntentDefault=0x915867,
      kIntentReader=0x909C08,kReachCtor=0x10F15AB,kExtendedCtor=0x10F15D7,kIntentCtor=0x10F168E,kZoneSchema=0xB0BF1,
      kZoneParse=0x932641,kZoneNativePublish=0x939E21,kOfferRpgClamp=0x9142A8)
    assert locations.keys()==arrays.keys()
    for name,at in locations.items():add('compiled pin '+name,pe.at(at,len(arrays[name])).hex(),arrays[name].hex())
    add('plain bool virtual getter',hex(pe.q(0x3B0A6A8)-pe.base),'0x1a7de10')
    add('bool getter reads one byte',pe.at(0x1A7DE10,5).hex(),'0fb64108c3')
    add('attack_zone_id registration name',pe.cstr(0x3E4F0F8),'attack_zone_id')
    add('zone registration raw offset',pe.at(0xB0C05,7).hex(),'c7400cd4000000')
    add('guard reads resolved attack-zone ID',pe.at(0x939E21,24).hex(),'488b8338010000488d8e500100008b68088bd5e89738fdff')
    add('trigger activation virtual predicate',hex(pe.q(0x3B813F8+0x60)-pe.base),'0x909bc8')
    add('trigger OnOpen virtual',hex(pe.q(0x3B813F8+0x70)-pe.base),'0x908a9c')
    calls=[('ordinary admitted-intent setter',0xB2745B,0x72FA34),('ordinary phase4 setter',0xB27476,0x93A0C8),
      ('VR offer authored-lead call',0x9142DF,0xA73A04),('native RPG allowance',0x9142B4,0xB27958),
      ('native zone lookup',0x932648,0x933F90),('native defense eligibility unchanged',0x909BDB,0x908444)]
    for name,site,expected in calls:
      b=pe.at(site,5);actual=site+5+struct.unpack('<i',b[1:])[0] if b[0]==0xE8 else -1;add(name,hex(actual),hex(expected))
    return dict(dll=str(path),native_execution=False,headset_test=False,passed=sum(c['passed'] for c in checks),total=len(checks),all_passed=all(c['passed'] for c in checks),checks=checks)
def main():
 p=argparse.ArgumentParser();p.add_argument('--dll',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
 result=verify(a.dll);a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(result,indent=2)+'\n')
 print({k:v for k,v in result.items() if k!='checks'})
 for c in result['checks']:
  if not c['passed']:print('FAIL',c)
 return 0 if result['all_passed'] else 1
if __name__=='__main__':raise SystemExit(main())
