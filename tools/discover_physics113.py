"""Static physics discovery. Strings/exports are leads, never callable ABI proof."""
import argparse
import hashlib
import json
import re
import struct
import zipfile
from pathlib import Path
from native_research112 import PE

EXPECTED='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
WORDS=re.compile(r'CPhysicalWorld|CPhysicalEntity|CRigidEntity|CArticulatedEntity|CSoftEntity|CLivingEntity|Physicalize|SetPhysicParams|AddImpulse|OnCollision|PHYSICPARAM_(?:CONSTRAINT|ARTICULATED)|EventPhysCollision|AddConstraint|RemoveConstraint|ragdoll|PhysicsConstraint',re.I)

def run(game,output):
    output.mkdir(parents=True,exist_ok=True)
    binary=game/'Bin/Win64MasterMasterSteamPGO/WHGame.dll';pe=PE(binary)
    digest=hashlib.sha256(pe.data).hexdigest()
    if digest!=EXPECTED or pe.stamp!=0x6A350E20:raise ValueError('Retail fingerprint mismatch')
    strings=[]
    for section,va,size,raw in pe.sections:
        if section==b'.text':continue
        for m in re.finditer(rb'[\x20-\x7e]{5,}',pe.data[raw:raw+size]):
            text=m[0].decode('ascii')
            if WORDS.search(text):strings.append({'rva':hex(va+m.start()),'text':text[:700],'section':section.decode()})
    def cstring(rva):
        pos=pe.offset(rva);end=pe.data.index(b'\0',pos)
        return pe.data[pos:end].decode('ascii',errors='replace')
    p=struct.unpack_from('<I',pe.data,0x3c)[0]
    exportRva=struct.unpack_from('<I',pe.data,p+24+112)[0]
    exports=[]
    if exportRva:
        e=pe.offset(exportRva)
        count=struct.unpack_from('<I',pe.data,e+24)[0]
        funcs,names,ords=struct.unpack_from('<III',pe.data,e+28)
        for i in range(count):
            name=cstring(struct.unpack('<I',pe.read(names+4*i,4))[0])
            ordinal=struct.unpack('<H',pe.read(ords+2*i,2))[0]
            rva=struct.unpack('<I',pe.read(funcs+4*ordinal,4))[0]
            exports.append({'name':name,'rva':hex(rva)})
    archives=[];matches=[]
    for relative in ['Data/Scripts.pak','Data/Tables.pak','Engine/Engine.pak']:
        path=game/relative
        item={'path':relative,'sha256':hashlib.sha256(path.read_bytes()).hexdigest()}
        with zipfile.ZipFile(path) as z:
            item['entries']=len(z.infolist())
            for entry in z.infolist():
                if entry.is_dir() or Path(entry.filename).suffix.lower() not in ('.lua','.xml','.cfg','.ent','.json') or entry.file_size>16000000:continue
                data=z.read(entry);text=data.decode('utf-8',errors='replace')
                if not WORDS.search(text):continue
                lines=[{'line':n,'text':line[:600]} for n,line in enumerate(text.splitlines(),1) if WORDS.search(line)]
                matches.append({'archive':relative,'path':entry.filename,'sha256':hashlib.sha256(data).hexdigest(),'matches':lines})
        archives.append(item)
    result={'whgame_sha256':digest,'timestamp':hex(pe.stamp),'image_base':hex(pe.base),'exports':exports,
            'binary_strings':strings,'archives':archives,'script_matches':matches,
            'boundary':'Presence proves shipped names/content only. No new function signatures, vtable slots, struct layouts or thread ownership have been qualified.'}
    (output/'physics_leads.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({'exports':len(exports),'binary_physics_strings':len(strings),'script_files':len(matches)}))

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--game',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
    a=p.parse_args();run(a.game,a.out)
