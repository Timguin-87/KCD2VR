#!/usr/bin/env python3
"""Read-only exact-retail verification for the C10R13 native-grasp adapter.

No game function is executed. Checks the full source DLL identity, every new
compiled pin, selected interface slots, native call edges and event-hash names.
The matching assembly and the implementation contract explain the ABI reasoning.
"""
from __future__ import annotations
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse, hashlib, json, re, struct
from pathlib import Path

EXPECTED_SHA = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
EXPECTED_TS = 0x6A350E20
ROOT = Path(__file__).resolve().parents[1]
class PE:
    def __init__(self, path: Path):
        self.data=path.read_bytes()
        if self.data[:2]!=b'MZ': raise ValueError('not an MZ executable')
        p=self.u32(0x3c)
        if self.data[p:p+4]!=b'PE\0\0': raise ValueError('missing PE signature')
        if self.u16(p+4)!=0x8664 or self.u16(p+24)!=0x20b: raise ValueError('requires AMD64 PE32+')
        self.timestamp=self.u32(p+8); self.base=self.u64(p+48)
        start=p+24+self.u16(p+20); self.sections=[]
        for i in range(self.u16(p+6)):
            s=start+40*i; vs,va,rs,ro=struct.unpack_from('<IIII',self.data,s+8)
            self.sections.append((va,rs,ro))
    def u16(self,p): return struct.unpack_from('<H',self.data,p)[0]
    def u32(self,p): return struct.unpack_from('<I',self.data,p)[0]
    def u64(self,p): return struct.unpack_from('<Q',self.data,p)[0]
    def at(self,rva,n):
        for va,rs,ro in self.sections:
            if va<=rva and rva+n<=va+rs: return self.data[ro+rva-va:ro+rva-va+n]
        raise ValueError(f'RVA {rva:#x}+{n} not backed by one section')
    def q(self,rva): return struct.unpack('<Q',self.at(rva,8))[0]
    def cstr(self,rva): return self.at(rva,96).split(b'\0',1)[0].decode('ascii')

def verify(path: Path):
    pe=PE(path); checks=[]
    def add(name,actual,expected):
        checks.append(dict(name=name,actual=actual,expected=expected,passed=actual==expected))
    add('full DLL SHA-256',hashlib.sha256(pe.data).hexdigest(),EXPECTED_SHA)
    add('TimeDateStamp',hex(pe.timestamp),hex(EXPECTED_TS));add('image base',hex(pe.base),'0x180000000')
    text=read_source(ROOT/'src/core/native_grasp13_pins.h')
    rvas={name:int(v,16) for name,v in re.findall(r'k(\w+)=0x([0-9A-Fa-f]+)ull',text)}
    pins={name:bytes(int(x,16) for x in re.findall(r'0x([0-9a-fA-F]+)',body))
          for name,body in re.findall(r'p(\w+)\[\]\{([^}]+)\}',text)}
    if rvas.keys()!=pins.keys(): raise ValueError('pin declarations are inconsistent')
    for name in rvas:add('pin '+name,pe.at(rvas[name],len(pins[name])).hex(),pins[name].hex())
    slots=[('pickup End lifetime boundary',0x3A5B670,0x20,0x40B4B8),
           ('player actor-model',0x3AA4B98,0x610,0x1A81DD0),
           ('hand attachment world QuatTS',0x3A657C0,0x70,0x423974),
           ('actual CEntityAttachment placement',0x3A68F68,0x10,0x4238B8),
           ('current hand attachment object',0x3A657C0,0xE0,0x19A2A90),
           ('entity world Matrix34',0x4072C78,0x100,0x1A74140),
           ('entity-system lookup',0x4077048,0x70,0x4B4F98),
           ('Mannequin Queue',0x40519E8,19*8,0x684D8C),
           ('Mannequin Update',0x40519E8,21*8,0x4B3DF8),
           ('fragment CRC identity lookup (not slot18 text)',0x40519E8,15*8,0x140E294)]
    for name,vf,slot,target in slots:add('vtable '+name,hex(pe.q(vf+slot)-pe.base),hex(target))
    calls=[('End -> cleanup (followed by native manager continuation)',0x40B4E9,0x40B628),
           ('Attach event -> combined transfer',0xA7F959,0x8CF6FC),
           ('transfer -> native pickup notice',0x8CF7D0,0x8CF234),
           ('transfer -> persistent container transfer',0x8CF80C,0x8D534C),
           ('transfer -> attach-only',0x8CF899,0x8D0404),
           ('transfer -> attach-only last-argument source (C10R18)',0x8CF866,0x44179C),
           ('drop -> detach hand',0x10859A1,0x8D02AC),
           ('drop -> queue wrapper',0x1085A62,0x1F1EB60)]
    for name,site,target in calls:
        b=pe.at(site,5);actual=site+5+struct.unpack('<i',b[1:])[0] if b[0]==0xE8 else -1
        add('call '+name,hex(actual),hex(target))
    # C19C4 fragment identity and pause propagation, exact instructions rather
    # than copying public SDK virtual indices. Slot18 returns a sub-definition.
    add('fragment sub-definition return (not char pointer)',pe.at(0x4AC921,13).hex(),'4863c248c1e005488b440808c3')
    add('controller CRC definition chain and tail',pe.at(0x140E294,16).hex(),'488b4120488b08488b4918e948da37ff')
    add('native action update scales dt by action speed',pe.at(0x4B3F12,14).hex(),'0f28cff30f594b5c488bcbff5030')
    add('native scope reads action speed and cached speed',pe.at(0x4B3A76,13).hex(),'f30f107d5cf30f1043580f2ec7')
    add('native scope publishes changed speed to layer setter',pe.at(0x4B3B43,21).hex(),'8b53300f28d7488b064103d6488bceff90b8000000')
    # H23 clip-only pickup clock: this is the exact retail layer-scale setter,
    # distinct from IAction+5C (which remains 1x) and normalized-time seeking.
    clock=read_source(ROOT/'src/native_pickup_clock_h23.inl')
    clock_pin=re.search(r'kLayerScalePinH23\[\]\{([^}]+)\}',clock)
    if not clock_pin:raise ValueError('missing H23 compiled layer-scale pin')
    pin=bytes(int(v,16) for v in re.findall(r'0x([0-9A-Fa-f]+)',clock_pin.group(1)))
    add('H23 exact compiled layer-scale setter',pe.at(0xAA8448,len(pin)).hex(),pin.hex())
    add('H23 actual skeleton layer-scale virtual target',hex(pe.q(0x3A711E0+0xB8)-pe.base),'0xaa8448')
    add('H23 layer-scale store is stride64 +50',pe.at(0xAA8465,7).hex(),'f30f11440850c3')

    col=pe.q(0x3A68F68-8)-pe.base;td=struct.unpack('<I',pe.at(col+12,4))[0]
    add('CEntityAttachment RTTI',pe.cstr(td+16),'.?AUCEntityAttachment@@')
    add('actual attached entity id member',pe.at(0x4238D4,3).hex(),'8b5708')
    # The caller substitutes the return pointer AFTER native GetAttWorldAbsolute.
    add('drop pose preceding virtual call',pe.at(0x1085837,3).hex(),'41ffd0')
    # C10R18: the pose is read in a 3-pass loop whose back-edge jumps to 0x108583C, two bytes after the return site. A mid hook
    # at 0x108583A would put that jump target inside its own patch; the substitution is a getter-entry hook filtered by caller.
    add('drop pose loop back-edge (no mid hook at the return site)',pe.at(0x1085853,2).hex(),'7ce7')
    add('drop pose return site starts the loop',pe.at(0x108583A,2).hex(),'8bcb')
    add('native entity matrix getter',pe.at(0x1A74140,5).hex(),'488d4158c3')
    # Constructor hashes these exact strings, then stores their CRCs in the
    # two globals consumed by PickupEvent. Verify both RIP-relative boundaries.
    for name,lea,store,global_rva in [('Attach',0x8D2B38,0x8D2B8C,0x548A788),('Detach',0x8D2B82,0x8D2BD6,0x548A78C)]:
        b=pe.at(lea,7);ptr=lea+7+struct.unpack('<i',b[3:])[0]
        add('event string '+name,pe.cstr(ptr),name)
        b=pe.at(store,7);target=store+7+struct.unpack('<i',b[3:])[0]
        add('event hash slot '+name,hex(target),hex(global_rva))
    for side in range(2):
        add('native hand attachment name '+str(side),pe.cstr(pe.q(0x40AC2A8+8*side)-pe.base),('hand_right','hand_left')[side])
    return dict(schema=1,dll=str(path),native_execution=False,headset_test=False,
                compiled_pins=len(pins),checks=checks,passed=sum(x['passed'] for x in checks),
                total=len(checks),all_passed=all(x['passed'] for x in checks))

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--dll',required=True,type=Path);ap.add_argument('--out',required=True,type=Path)
    a=ap.parse_args()
    try: report=verify(a.dll)
    except (OSError,ValueError,struct.error) as e: report=dict(all_passed=False,error=str(e),native_execution=False)
    a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k!='checks'},indent=2))
    if not report['all_passed']:
        for c in report.get('checks',[]):
            if not c['passed']:print('FAIL',c)
    return 0 if report['all_passed'] else 1
if __name__=='__main__':raise SystemExit(main())
