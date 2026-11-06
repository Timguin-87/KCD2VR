"""Source-only, pinned AVM1 patch. No PAK or installation is produced.

The exact current movies remain the base: their notification, name, glyph and
dialogue patches are retained. Only imported-glyph initialization and explicit
station prompt children are changed. Native input/action functions still run.
"""
from pathlib import Path
import argparse, hashlib, json, re
import patch_glyphs_d68 as p

ROOT=Path(__file__).resolve().parents[1]
CALLBACK='fc_setKcd2vrPromptStyleD80'
APPLY='kcd2vrApplyPromptsD80'
ADJUST='kcd2vrAdjustPromptD80'
ACTIVE='kcd2vrPromptActiveD80'
PERCENT='kcd2vrPromptPercentD80'
VISIBLE='kcd2vrPromptVisibleD80'
NATIVE=('setActionHintsPosition','fc_setActionHints','fc_setProgress')
PATHS=(('br','actionHints'),('tl','tutorial'),
       ('cccursor','actioncursor','ActionCursorOutline'),
       ('cccursor','actioncursor','ActionCursorCross'))

def prop(obj,key):return obj+p.push(key)+b'\x4e'
def put(obj,key,value):return obj+p.push(key)+value+b'\x4f'
def root(key):return prop(p.var('_root'),key)
def rootput(key,value):return put(p.var('_root'),key,value)
def path(parts):
    value=p.var('_root')
    for part in parts:value=prop(value,part)
    return value
def call(obj,name,args):
    return b''.join(reversed(args))+p.push(len(args))+obj+p.push(name)+b'\x52'

def adjust_body():
    c=p.Code().emit(p.var('clip')+p.push(None)+b'\x49').jump('done',True)
    c.emit(prop(p.var('clip'),'_parent')+p.push(None)+b'\x49').jump('done',True)
    for key,suffix in (('_xscale','X'),('_yscale','Y'),('_alpha','Alpha')):
        own='kcd2vrPromptOwn'+suffix+'D80';base='kcd2vrPromptBase'+suffix+'D80';last='kcd2vrPromptLast'+suffix+'D80'
        c.emit(p.local('current',prop(p.var('clip'),key)))
        wanted=p.var('active') if key!='_alpha' else p.var('active')+p.var('visible')+b'\x12\x10'
        c.emit(wanted+b'\x12').jump('restore'+suffix,True)
        c.emit(prop(p.var('clip'),own)+b'\x12').jump('capture'+suffix,True)
        c.emit(p.var('current')+prop(p.var('clip'),last)+b'\x49').jump('have'+suffix,True)
        c.label('capture'+suffix).emit(put(p.var('clip'),base,p.var('current')))
        c.label('have'+suffix)
        target=p.push(0) if key=='_alpha' else prop(p.var('clip'),base)+p.var('percent')+b'\x0c'+p.push(100)+b'\x0d'
        c.emit(p.local('target',target))
        c.emit(put(p.var('clip'),key,p.var('target'))+put(p.var('clip'),last,p.var('target'))+put(p.var('clip'),own,p.push(1))).jump('next'+suffix)
        c.label('restore'+suffix).emit(prop(p.var('clip'),own)+b'\x12').jump('next'+suffix,True)
        c.emit(p.var('current')+prop(p.var('clip'),last)+b'\x49\x12').jump('release'+suffix,True)
        c.emit(put(p.var('clip'),key,prop(p.var('clip'),base)))
        c.label('release'+suffix).emit(put(p.var('clip'),own,p.push(0)))
        c.label('next'+suffix)
    c.label('done').emit(p.push(None)+b'\x3e\0')
    return c.finish()

def apply_body():
    c=p.Code()
    # Native events outside a station incur only this one predicate. An explicit
    # disable callback passes force=1 once so our own scale/alpha can restore.
    c.emit(root(ACTIVE)+p.var('force')+b'\x11\x12').jump('done',True)
    for parts in PATHS:
        c.emit(call(p.var('_root'),ADJUST,[path(parts),root(ACTIVE),root(PERCENT),root(VISIBLE)])+b'\x17')
    c.label('done').emit(p.push(None)+b'\x3e\0')
    return c.finish()

def callback_body():
    c=p.Code().emit(rootput(ACTIVE,p.var('active'))+rootput(PERCENT,p.var('percent'))+rootput(VISIBLE,p.var('visible')))
    c.emit(call(p.var('_root'),APPLY,[p.push(1)])+b'\x17\0')
    return c.finish()

def native_wrapper(name):
    # Function.apply preserves the entire native argument list and return value.
    # Native state/content is changed only by that original function, once.
    c=p.Code().emit(p.local('result',call(root('kcd2vrNative_'+name+'_D80'),'apply',[p.var('this'),p.var('arguments')])))
    c.emit(call(p.var('_root'),APPLY,[p.push(0)])+b'\x17')
    return c.emit(p.var('result')+b'\x3e\0').finish()

def injection():
    c=p.Code().emit(p.define(ADJUST,('clip','active','percent','visible'),adjust_body()))
    c.emit(p.define(APPLY,('force',),apply_body()))
    c.emit(p.define(CALLBACK,('active','percent','visible'),callback_body()))
    for name in NATIVE:
        saved='kcd2vrNative_'+name+'_D80';skip='skip_'+name
        c.emit(p.var(name)+p.push(None)+b'\x49').jump(skip,True)
        c.emit(root(saved)+p.push(None)+b'\x49\x12').jump(skip,True)
        c.emit(rootput(saved,p.var(name)))
        c.emit(p.define(name,(),native_wrapper(name))).label(skip)
    return c.emit(b'\0').finish()

def callback_with_bootstrap():
    old=p.callback();length=int.from_bytes(old[1:3],'little');decl=old[3:3+length]
    body_size=int.from_bytes(decl[-2:],'little');body=old[3+length:3+length+body_size]
    # Imported library root frames need not execute before exported sprites.
    # These idempotent definitions also make a style-change callback sufficient.
    return p.define(p.CALLBACK,('style',),p.registry_helpers()[:-1]+body)+b'\0'

def redirect_with_bootstrap(label):
    return p.registry_helpers()[:-1]+p.redirect(label,p.MAPPING[label])

def patch_movie(name,raw):
    head,body=p.unpack(raw);out=[];changed=0;inserted=False
    for kind,payload,original in p.tags(body):
        if name=='Libs/UI/buttons.gfx' and kind==39 and int.from_bytes(payload[:2],'little')==108:
            sprite=payload[:4];count=0
            for sk,sv,st in p.tags(payload[4:]):
                match=next((label for label in p.MAPPING if sk==12 and sv==p.redirect(label,p.MAPPING[label])),None)
                if match is not None:sprite+=p.tag(12,redirect_with_bootstrap(match));count+=1
                else:sprite+=st
            if count!=24:raise ValueError('Expected all 24 exact existing glyph redirects')
            out.append(p.tag(kind,sprite));changed+=count;continue
        if kind==12 and payload==p.callback():
            out.append(p.tag(kind,callback_with_bootstrap()));changed+=1;continue
        if name=='Libs/UI/hud.gfx' and kind==1 and not inserted:
            if CALLBACK.encode() in body:raise ValueError('Already patched HUD')
            if not all((name+'\0').encode() in body for name in NATIVE):raise ValueError('Native prompt functions missing')
            out.append(p.tag(12,injection()));inserted=True
        out.append(original)
    expected=24 if name.endswith('/buttons.gfx') else 1
    if changed!=expected or (name.endswith('/hud.gfx') and not inserted):raise ValueError('Unexpected source movie shape '+name)
    return p.pack(raw,head,b''.join(out))

def patch_xml(raw):
    text=raw.decode('utf8')
    declaration='''      <function name="SetKcd2vrPromptStyleD80" funcname="fc_setKcd2vrPromptStyleD80">
        <param name="active" type="bool" />
        <param name="percent" type="int" />
        <param name="visible" type="bool" />
      </function>
'''
    marker='      <!-- Reset all -->'
    if text.count(marker)!=1 or 'SetKcd2vrPromptStyleD80' in text:raise ValueError('Unexpected HUD declarations')
    return text.replace(marker,declaration+'\n'+marker).encode('utf8')

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--pins',type=Path,required=True);ap.add_argument('--report',type=Path,required=True);a=ap.parse_args()
    pins=json.loads(a.pins.read_text(encoding='utf-8-sig'));output={};report={}
    for name,pin in pins.items():
        path=ROOT/'companion/source'/name;raw=path.read_bytes()
        if p.sha(raw)!=pin:raise ValueError('Source input mismatch '+name)
        new=patch_xml(raw) if name.endswith('.xml') else patch_movie(name,raw)
        output[path]=new;report[name]={'input_sha256':pin,'output_sha256':p.sha(new),'input_bytes':len(raw),'output_bytes':len(new)}
    # All inputs and transformations qualify before any source asset is replaced.
    for path,data in output.items():p.atomic(path,data)
    p.atomic(a.report,(json.dumps({'files':report,'native_execution':False,'headset_test':'NOT_RUN','pak_generated':False},indent=2)+'\n').encode())
    print('Updated source movie assets only:',len(output))
if __name__=='__main__':main()
