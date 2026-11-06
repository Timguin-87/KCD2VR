"""Add a scoped native entity-name focus to the pinned D71 HUD.

Native setNpcName(idNum,type,name) owns insertion/removal and localization;
setActionHintsPosition owns placement. This adds no alternate text field,
timer, UI-tree search, visibility override or native action invocation.
Regenerate glyphs/notification assets first, then apply this patch.
"""
from pathlib import Path
import argparse, hashlib, json
import patch_glyphs_d68 as p

BASE_SHA256='534f51c6da339d9afd346f93d9c954d098fa50e1c5db310dbe23a1adc9e12e13'
ORIGINAL='kcd2vrNativeSetNpcNameD72'
ACTIVE='kcd2vrAlchemyNameActiveD72'
NAME='kcd2vrAlchemyNameD72'
LAST_NAME='kcd2vrNativeEntityNameD72'
LAST_TYPE='kcd2vrNativeEntityTypeD72'
CALLBACK='fc_setKcd2vrAlchemyNameD72'

def prop(name):return p.member('_root',name)
def put(name,value):return p.var('_root')+p.push(name)+value+b'\x4f'
def entity():return p.member('Enum','NPC_NAME_TYPE_ENTITY')
def invoke(args):
    return b''.join(reversed(args))+p.push(len(args))+p.var('_root')+p.push(ORIGINAL)+b'\x52\x17'

def wrapper_body():
    c=p.Code().emit(p.var('idNum')+entity()+b'\x49\x12').jump('native',True)
    c.emit(put(LAST_NAME,p.var('name'))+put(LAST_TYPE,p.var('type')))
    c.emit(prop(ACTIVE)+b'\x12').jump('native',True)
    c.emit(p.push('name')+prop(NAME)+b'\x1d')
    c.label('native').emit(invoke([p.var('idNum'),p.var('type'),p.var('name')]))
    return c.emit(b'\0').finish()

def callback_body():
    c=p.Code().emit(prop(ORIGINAL)+p.push(None)+b'\x49').jump('done',True)
    c.emit(p.var('active')+prop(ACTIVE)+b'\x49\x12').jump('change',True)
    c.emit(p.var('name')+prop(NAME)+b'\x49').jump('done',True)
    c.label('change').emit(put(ACTIVE,p.var('active'))+put(NAME,p.var('name')))
    c.emit(p.var('active')+b'\x12').jump('restore',True)
    c.emit(invoke([entity(),entity(),p.var('name')])).jump('position')
    c.label('restore').emit(p.local('nativeName',prop(LAST_NAME)))
    c.emit(p.var('nativeName')+p.push(None)+b'\x49\x12').jump('have_name',True)
    c.emit(p.push('nativeName','')+b'\x1d')
    c.label('have_name').emit(p.local('nativeType',prop(LAST_TYPE)))
    c.emit(p.var('nativeType')+p.push(None)+b'\x49\x12').jump('have_type',True)
    c.emit(p.push('nativeType')+entity()+b'\x1d')
    c.label('have_type').emit(invoke([entity(),p.var('nativeType'),p.var('nativeName')]))
    c.label('position').emit(p.push(0,'setActionHintsPosition')+b'\x3d\x17')
    c.label('done').emit(b'\0')
    return c.finish()

def injection():
    c=p.Code().emit(prop(ORIGINAL)+p.push(None)+b'\x49\x12').jump('saved',True)
    c.emit(put(ORIGINAL,p.var('setNpcName'))).label('saved')
    c.emit(p.define('setNpcName',('idNum','type','name'),wrapper_body()))
    c.emit(p.define(CALLBACK,('name','active'),callback_body()))
    return c.emit(b'\0').finish()

def patch(raw):
    if hashlib.sha256(raw).hexdigest()!=BASE_SHA256:raise ValueError('Exact D71 HUD asset mismatch')
    head,body=p.unpack(raw)
    if not all(x in body for x in (b'setNpcName\0',b'NPC_NAME_TYPE_ENTITY\0',b'setActionHintsPosition\0')):
        raise ValueError('Native entity-name route missing')
    out=[];done=False
    for kind,payload,original in p.tags(body):
        if kind==1 and not done:out.append(p.tag(12,injection()));done=True
        out.append(original)
    if not done:raise ValueError('First native frame missing')
    return p.pack(raw,head,b''.join(out))

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--input',type=Path,required=True);ap.add_argument('--output',type=Path,required=True);ap.add_argument('--report',type=Path,required=True);a=ap.parse_args()
    original=a.input.read_bytes();result=patch(original)
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_bytes(result)
    a.report.parent.mkdir(parents=True,exist_ok=True);a.report.write_text(json.dumps({'input_sha256':hashlib.sha256(original).hexdigest(),'output_sha256':hashlib.sha256(result).hexdigest(),'native_function':'setNpcName(idNum,type,name)','native_entity_name_only':True,'native_positions_preserved':True,'retail_native_execution':False,'headset_test':'NOT_RUN'},indent=2)+'\n')
    print('D72 native entity-name bridge injected; original HUD tags preserved')
if __name__=='__main__':main()
