#!/usr/bin/env python3
"""Verify D49/D50 prompt native identities against the supplied retail DLL bytes.

Read-only PE/file audit; no native function is loaded or invoked.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--dll',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
a=p.parse_args();root=Path(__file__).resolve().parents[1];b=a.dll.read_bytes();rows=[]
sha=hashlib.sha256(b).hexdigest()
assert sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7','retail DLL SHA mismatch'
pe=struct.unpack_from('<I',b,0x3c)[0];assert b[pe:pe+4]==b'PE\0\0'
machine,count,timestamp=struct.unpack_from('<HHI',b,pe+4);optional=struct.unpack_from('<H',b,pe+20)[0]
assert machine==0x8664 and timestamp==0x6A350E20
imagebase=struct.unpack_from('<Q',b,pe+24+24)[0]
sections=[struct.unpack_from('<IIII',b,pe+24+optional+40*i+8) for i in range(count)]
def at(rva,size):
    for virtual_size,virtual_address,raw_size,raw_pointer in sections:
        offset=rva-virtual_address
        if 0<=offset and offset+size<=raw_size:return b[raw_pointer+offset:raw_pointer+offset+size]
    raise ValueError('RVA outside file-backed section: '+hex(rva))
source=(root/'src/subsystems/hud_ui_controls/runtime/prompt_device_lock.cpp').read_text()
pins=source[source.index('bool CVarPinsD49('):source.index('void* InputDeviceCVarD49(')]
for name,rva in [('get',0x9D8D38),('setInt',0x4D857C),('setRef',0xC05DB8),('getInt',0x1A727A0),('getRef',0x4A48E0)]:
    content=re.search(r'\b'+name+r'\[\]\s*=\s*\{([^}]+)\}',pins).group(1)
    expected=bytes(int(x.strip(),0) for x in content.split(',') if x.strip())
    actual=at(rva,len(expected));rows.append({'name':name,'rva':hex(rva),'expected':expected.hex(),'actual':actual.hex(),'match':actual==expected})
for name,vt,slot,target in [('console GetCVar',0x3DCF850,0xB8,0x9D8D38),('int GetIVal',0x3A7F0B8,0x10,0x1A727A0),('int SetIVal',0x3A7F0B8,0x38,0x4D857C),('int-ref GetIVal',0x3DD0020,0x10,0x4A48E0),('int-ref SetIVal',0x3DD0020,0x38,0xC05DB8)]:
    actual=struct.unpack('<Q',at(vt+slot,8))[0]-imagebase
    rows.append({'name':name,'vtable':hex(vt),'slot':hex(slot),'target':hex(target),'actual':hex(actual),'match':actual==target})
hint=source[source.index('bool ActionHintPinsD53('):source.index('void ObserveHintFamilyD53(')]
content=re.search(r'caller\[\]\s*(?:=\s*)?\{([^}]+)\}',hint).group(1)
expected=bytes(int(x.strip(),0) for x in content.split(',') if x.strip())
rows.append({'name':'D53 exact action-hint GetCVar call and return site','rva':'0x8e6b05','return_rva':'0x8e6b1c','expected':expected.hex(),'actual':at(0x8E6B05,len(expected)).hex(),'match':at(0x8E6B05,len(expected))==expected})
rows.append({'name':'D53 exact action-hint CVar name','rva':'0x3bc4228','match':at(0x3BC4228,18)==b'wh_ui_InputDevice\0'})
callback=at(0x2BBA132,5);target=0x2BBA137+struct.unpack_from('<i',callback,1)[0]
rows.append({'name':'CVar callback tail jump to manager','rva':'0x2bba132','target':hex(target),'match':callback[0]==0xE9 and target==0xC3C034})
result={'dll_sha256':sha,'PE_timestamp':hex(timestamp),'image_base':hex(imagebase),'source_sha256':hashlib.sha256((root/'src/subsystems/hud_ui_controls/runtime/prompt_device_lock.cpp').read_bytes()).hexdigest(),'checks':rows,'passed':all(x['match'] for x in rows),'native_execution':False}
a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps({'checks':len(rows),'passed':result['passed'],'native_execution':False}))
raise SystemExit(0 if result['passed'] else 1)
