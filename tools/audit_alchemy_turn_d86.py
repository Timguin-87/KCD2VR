#!/usr/bin/env python3
"""Read-only provenance for the native procedural boundary used by D86.

Does not execute the retail DLL. Validates the production byte pins, vtable,
RTTI identities, and records the supplied ADB's vial procedural timelines.
The runtime policy uses actual pending times, never these authored constants.
"""
from pathlib import Path
import argparse,hashlib,json,re,struct,zipfile,xml.etree.ElementTree as ET
from smith_native_probe46 import PE,EXPECTED
from test_crossbow_mechanism_d78 import function
ROOT=Path(__file__).resolve().parents[1]
ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--dll',required=True,type=Path);ap.add_argument('--animations',required=True,type=Path);ap.add_argument('--out',required=True,type=Path);a=ap.parse_args();a.out.mkdir(parents=True,exist_ok=True)
pe=PE(a.dll);source=(ROOT/'src/alchemy_resource46.inl').read_text();fn=function(source,'static bool SemiResourceTurnPinsD86(')
arrays={name:bytes(int(x,16) for x in re.findall(r'0x([0-9A-Fa-f]{2})',body)) for name,body in re.findall(r'constexpr unsigned char (\w+)\[\]\{([^}]+)\}',fn)}
pins=[]
for rva,name in re.findall(r'MatchBytes\(b\+(0x[0-9A-Fa-f]+),(\w+),sizeof\(\2\)\)',fn):
 expected=arrays[name];actual=pe.read(int(rva,16),len(expected));assert expected==actual,(name,rva)
 pins.append({'name':name,'rva':rva,'bytes':actual.hex(),'passed':True})
assert len(pins)==3
assert struct.unpack('<Q',pe.read(0x3AEA100+0x128,8))[0]-pe.base==0x4B4638
rtti=[]
for vft,col,td,name in [(0x3AB5680,0x4159508,0x4B438A0,b'.?AVC_ProceduralClipGroundAlignment@animationmodule@wh@@'),(0x3AB60A8,0x40C0EC0,0x5032BC0,b'.?AVCProceduralClipPositionAdjustAlignBone@@')]:
 assert struct.unpack('<Q',pe.read(vft-8,8))[0]-pe.base==col
 fields=struct.unpack('<6I',pe.read(col,24));assert fields[0]==1 and fields[1]==0 and fields[3]==td and fields[5]==col
 assert pe.read(td+16,len(name)+1)==name+b'\0'
 assert ('0x%X'%vft) in fn and ('0x%X'%td) in fn
 rtti.append({'vft':hex(vft),'locator':hex(col),'type_descriptor':hex(td),'name':name.decode(),'passed':True})
with zipfile.ZipFile(a.animations) as z:
 member='Mannequin/ADB/male_alchemy.adb';b=z.read(member)
r=ET.fromstring(b);profiles=[]
for f in r.find('FragmentList').find('AlchemyUse'):
 names=[e.get('name') for e in f.findall('./AnimLayer/Animation')]
 if not any(n and n.startswith('alchemy_pick_vial_') for n in names):continue
 procs=[]
 for layer in f.findall('ProcLayer'):
  t=0
  for e in layer:
   if e.tag=='Blend':t+=float(e.get('ExitTime','0'))
   elif e.tag=='Procedural':
    effect=e.find('./ProceduralParams/EffectName');joint=e.find('./ProceduralParams/JointName')
    procs.append({'time_seconds':t,'type':e.get('type'),'effect':effect.get('value') if effect is not None else None,'joint':joint.get('value') if joint is not None else None})
 assert all(p['type'] in ('','ParticleEffect','PositionAdjustAlignBone','GroundAlignment') for p in procs)
 assert sum(p['type']=='ParticleEffect' for p in procs)==1
 profiles.append({'guid':f.get('GUID'),'tags':f.get('Tags'),'fragment_tags':f.get('FragTags'),'animation_aliases':names,'procedural_layers':len(f.findall('ProcLayer')),'procedurals':procs})
assert len(profiles)==8
result={'passed':True,'dll_sha256':EXPECTED,'native_executed':False,'pins':pins,'scope_vtable':{'vft':'0x3AEA100','slot':'0x128','target':'0x4B4638'},'active_henry_clip_rtti':rtti,'asset':{'archive':a.animations.name,'member':member,'sha256':hashlib.sha256(b).hexdigest(),'profiles':profiles},'runtime_qualification':'Actual AlchemyUse native owner/verb/tags; genuine Attach and event-list membership; same CAF/header; current two/three procedural layers; exactly one pending unstarted layer; every active layer one of two pinned Henry alignment classes; dynamic pending time and first CAF event budget. No fixed ADB timing granted to unsupported clips.','limits':'Does not execute game/headset or establish physical emitter mouth, emission-stop lifetime, native recipe result or all modded asset arrangements.'}
(a.out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n')
for rva,name in [(0x4B4638,'procedural_layers'),(0x4B3A20,'scope_update')]: (a.out/(name+'.asm.txt')).write_text(pe.listing(rva))
print('Alchemy D86 retail audit: 3 byte pins, native updater slot, 2 RTTI identities and 8 authored vial profiles PASS (no native execution)')
