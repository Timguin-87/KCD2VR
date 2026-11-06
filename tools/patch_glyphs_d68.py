"""Pin retail UI assets, add PS decorations while preserving Xbox action lookup.
Offline AVM1 injection only: no input binding, hit area, placement or native ABI.
The D63 six-container notification reduction is regenerated first for HUD.
"""
from pathlib import Path
import argparse, hashlib, json, os, re, struct, zipfile, zlib
from patch_hud_notifications_d63 import patch as patch_hud
ROOT=Path(__file__).resolve().parents[1]
FLAG='KCD2VR_PS_GLYPHS_D68'
CALLBACK='fc_setKcd2vrGlyphStyleD68'
REGISTRY='KCD2VR_GlyphClipsD68'
REGISTER='KCD2VR_RegisterGlyphD68'
REFRESH='KCD2VR_RefreshGlyphsD68'
SEMANTIC='KCD2VR_SemanticGlyphD68'
SLOT='KCD2VR_GlyphSlotD68'
CAPACITY=512
SURFACES=('HUD','Menu','ItemSelection','ItemTransfer','ApseMap','ApseModalDialog','ApsePlayerInfo','GameOver')
MAPPING=dict(zip(
 ('xi_a','xi_b','xi_x','xi_y','xi_shoulderl','xi_shoulderr','xi_triggerl','xi_triggerr','xi_dpad_down','xi_dpad_right','xi_dpad_up','xi_dpad_left','xi_thumbl_move','xi_thumbl','xi_thumblx','xi_thumbly','xi_thumbl_l','xi_thumbl_r','xi_thumbr_move','xi_thumbr','xi_thumbrx','xi_thumbry','xi_start','xi_back'),
 ('pad_cross','pad_circle','pad_square','pad_triangle','pad_l1','pad_r1','pad_l2','pad_r2','pad_down','pad_right','pad_up','pad_left','pad_thumbl_move','pad_l3','pad_sticklx','pad_stickly','pad_stickl_left','pad_stickl_right','pad_thumbr_move','pad_r3','pad_stickrx','pad_stickry','pad_start','pad_touch')))
EXPECTED={'Libs/UI/hud.gfx': '85303baaeaa925ae051fe49626f15e638642908599886168682a16e402a065cd', 'Libs/UI/Menu.gfx': '7f37d24fc14b3961fb58b403340532a947239a4a0bdce01f059485572d841c1c', 'Libs/UI/ItemSelection.gfx': 'e0d8c2f9c2567116d4da11367b94a601fa885a6083690877569e719f03f22215', 'Libs/UI/ItemTransfer.gfx': 'b60c2aac4996c8bd0e1fd15c2ceb412c7254d9fb5455abfc22a8166029f955ba', 'Libs/UI/ApseMap.gfx': 'c886643f6afe323bdb12699f744c61b1bd540b3cd0d2e019afd492019286ac17', 'Libs/UI/ApseModalDialog.gfx': '7e3ec05a423e8c6f4f3ac9f1159dbe38d60638f8413b9c9b4d8b280dd8cffaa2', 'Libs/UI/ApsePlayerInfo.gfx': 'd091e8bf7d4476915b5c56e33db06000965b4fb9848f8e576edaf02c40c5652c', 'Libs/UI/GameOver.gfx': '5c8af55b088250a5f80a5b4d6fbdd4be75d36ca3b73a042cb5d553cad02c6fce', 'Libs/UI/buttons.gfx': '83b2867d305000918479ab8e8511573d1708e4342760ebaaa1ebb3fef993f24d', 'Libs/UI/UIElements/HUD.xml': '7dd34131a758f203e3f93c584fcda8839a574d8eb9c233323a1ca05999fa65d1', 'Libs/UI/UIElements/Menu.xml': '50a2d01827a700bdc291061dc796bf633876b90536a9295a1034f9ed0682d75b', 'Libs/UI/UIElements/ItemSelection.xml': '15184a150c50c5951b1351b7d1ee0bb9319014867499070ad6c499ed0f78d89b', 'Libs/UI/UIElements/ItemTransfer.xml': 'eb065db52bb41b91267e4a244012d766270d3cec763b6c436d27aaa63b6ff3f1', 'Libs/UI/UIElements/ApseMap.xml': '320131af7dddf4fda8a6c18dc481451b82710c47b5c62696571ec0ab85ef5443', 'Libs/UI/UIElements/ApseModalDialog.xml': 'cfc35498c71546f9264effd700f3f3817b9b3a8dad1380bb4fd3e205ea281507', 'Libs/UI/UIElements/ApsePlayerInfo.xml': 'c96d6eb83088ddff01e952f6c465a7431c9bf8e9ce4f92a04c932f408d377f3f', 'Libs/UI/UIElements/GameOver.xml': '76d69f48dc86de81a52a0e0bae54e1c96c60cddfa3eb0d55b432908ae98670e7'}
def sha(b):return hashlib.sha256(b).hexdigest()
def atomic(p,b):
 p.parent.mkdir(parents=True,exist_ok=True);t=p.with_name(p.name+'.d68tmp');t.write_bytes(b);os.replace(t,p)
def action(op,payload=b''):
 return bytes([op])+(struct.pack('<H',len(payload))+payload if op>=128 else b'')
def push(*values):
 p=b''
 for x in values:
  if x is None:p+=b'\x03'
  elif isinstance(x,str):p+=b'\0'+x.encode()+b'\0'
  else:p+=b'\x07'+struct.pack('<i',x)
 return action(0x96,p)
def tag(kind,payload):return struct.pack('<HI',kind*64+63,len(payload))+payload

class Code:
 """Tiny label assembler; AVM1 branches are relative to the next action."""
 def __init__(self):self.b=bytearray();self.labels={};self.fix=[]
 def emit(self,b):self.b.extend(b);return self
 def label(self,n):assert n not in self.labels;self.labels[n]=len(self.b);return self
 def jump(self,n,conditional=False):
  self.b.extend(action(0x9d if conditional else 0x99,b'\0\0'));self.fix.append((len(self.b)-2,n));return self
 def finish(self):
  for at,n in self.fix:struct.pack_into('<h',self.b,at,self.labels[n]-(at+2))
  return bytes(self.b)
def var(n):return push(n)+b'\x1c'
def member(obj,n):return var(obj)+push(n)+b'\x4e'
def local(n,value):return push(n)+value+b'\x3c'
def define(name,params,body):
 decl=name.encode()+b'\0'+struct.pack('<H',len(params))+b''.join(x.encode()+b'\0' for x in params)+struct.pack('<H',len(body))
 return action(0x9b,decl)+body

def callback():
 # The retail Observable refresh still runs, but some widgets skip equal input2.
 # Refresh retained semantic labels only when artwork changes; never toggle input.
 c=Code().emit(local('desired',var('style')+push(4)+b'\x49'))
 c.emit(local('previous',member('_global',FLAG)))
 c.emit(var('_global')+push(FLAG)+var('desired')+b'\x4f')
 c.emit(var('previous')+var('desired')+b'\x49').jump('native',True)
 c.emit(member('_global',REFRESH)+push(None)+b'\x49').jump('native',True)
 c.emit(push(0)+var('_global')+push(REFRESH)+b'\x52\x17')
 c.label('native').emit(push(2,1,'fc_setInputId')+b'\x3d\x17\0')
 return define(CALLBACK,('style',),c.finish())+b'\0'

def registry_helpers():
 # Helpers run only in exact shared button movie. No timers, event polling,
 # onUnload replacement or UI-tree traversal. Fixed storage, no live eviction.
 r=Code().emit(var('mc')+push(SEMANTIC)+var('label')+b'\x4f')
 r.emit(local('a',member('_global',REGISTRY)))
 r.emit(var('a')+push(None)+b'\x49\x12').jump('array_ready',True)
 r.emit(local('a',push(CAPACITY,1,'Array')+b'\x40'))
 r.emit(var('_global')+push(REGISTRY)+var('a')+b'\x4f')
 r.label('array_ready').emit(local('slot',member('mc',SLOT)))
 # Repeated timeline activation is constant time; don't scan 512 entries.
 r.emit(var('slot')+push(None)+b'\x49').jump('scan',True)
 r.emit(var('a')+var('slot')+b'\x4e'+var('mc')+b'\x49').jump('done',True)
 r.label('scan').emit(local('free',push(-1))).emit(local('i',push(0)))
 r.label('loop').emit(var('i')+push(CAPACITY)+b'\x48\x12').jump('assign',True)
 r.emit(local('old',var('a')+var('i')+b'\x4e'))
 r.emit(var('old')+var('mc')+b'\x49').jump('existing',True)
 r.emit(var('old')+push(None)+b'\x49').jump('dead',True)
 r.emit(member('old','_parent')+push(None)+b'\x49').jump('dead',True)
 r.jump('next')
 r.label('dead').emit(var('a')+var('i')+push(None)+b'\x4f')
 r.emit(var('free')+push(-1)+b'\x49\x12').jump('next',True)
 r.emit(push('free')+var('i')+b'\x1d')
 r.label('next').emit(push('i')+var('i')+push(1)+b'\x47\x1d').jump('loop')
 r.label('existing').emit(var('mc')+push(SLOT)+var('i')+b'\x4f').jump('done')
 r.label('assign').emit(var('free')+push(-1)+b'\x49').jump('done',True)
 r.emit(var('a')+var('free')+var('mc')+b'\x4f')
 r.emit(var('mc')+push(SLOT)+var('free')+b'\x4f')
 r.label('done').emit(push(None)+b'\x3e\0')
 f=Code().emit(local('a',member('_global',REGISTRY)))
 f.emit(var('a')+push(None)+b'\x49').jump('done',True)
 f.emit(local('i',push(0)))
 f.label('loop').emit(var('i')+push(CAPACITY)+b'\x48\x12').jump('done',True)
 f.emit(local('mc',var('a')+var('i')+b'\x4e'))
 f.emit(var('mc')+push(None)+b'\x49').jump('next',True)
 f.emit(member('mc','_parent')+push(None)+b'\x49').jump('dead',True)
 f.emit(member('mc',SEMANTIC)+push(None)+b'\x49').jump('dead',True)
 f.emit(member('mc',SEMANTIC)+push(1)+var('mc')+push('gotoAndStop')+b'\x52\x17').jump('next')
 f.label('dead').emit(var('a')+var('i')+push(None)+b'\x4f')
 f.label('next').emit(push('i')+var('i')+push(1)+b'\x47\x1d').jump('loop')
 f.label('done').emit(push(None)+b'\x3e\0')
 # Anonymous DefineFunction pushes the callable onto the operand stack. Guard
 # each assignment so another loaded buttons movie cannot reset the registry.
 c=Code()
 for name,params,body in ((REGISTER,('mc','label'),r.finish()),(REFRESH,(),f.finish())):
  end='skip_'+name
  c.emit(member('_global',name)+push(None)+b'\x49\x12').jump(end,True)
  c.emit(var('_global')+push(name)+define('',params,body)+b'\x4f').label(end)
 return c.emit(b'\0').finish()

def redirect(original,target):
 # Exact mc_button instance retains Xbox semantics, including when currently
 # showing a PS frame. Replaying the Xbox label restores either art family.
 move=action(0x8c,target.encode()+b'\0')+b'\x07'
 register=push(original)+var('this')+push(2)+var('_global')+push(REGISTER)+b'\x52\x17'
 return register+member('_global',FLAG)+b'\x12'+action(0x9d,struct.pack('<h',len(move)))+move+b'\0'

def unpack(raw):
 if raw[:3]!=b'CFX':raise ValueError('Expected compressed retail GFx')
 b=zlib.decompress(raw[8:]);assert len(b)+8==struct.unpack_from('<I',raw,4)[0]
 off=(5+4*(b[0]>>3)+7)//8+4
 return b[:off],b[off:]
def tags(b):
 p=0
 while p<len(b):
  start=p;h=struct.unpack_from('<H',b,p)[0];p+=2;k,n=h>>6,h&63
  if n==63:n=struct.unpack_from('<I',b,p)[0];p+=4
  assert p+n<=len(b);v=b[p:p+n];p+=n
  yield k,v,b[start:p]
  if k==0:assert p==len(b);break

def pack(raw,head,body):
 b=head+body;return raw[:4]+struct.pack('<I',len(b)+8)+zlib.compress(b,9)
def patch_movie(raw):
 h,b=unpack(raw);out=b'';done=False
 assert CALLBACK.encode() not in zlib.decompress(raw[8:])
 assert b'fc_setInputId\0' in b
 for k,v,t in tags(b):
  if k==1 and not done:out+=tag(12,callback());done=True
  out+=t
 assert done
 return pack(raw,h,out)
def patch_buttons(raw):
 h,b=unpack(raw);out=b'';changed=[];all_labels=[];helpers_added=False
 for k,v,t in tags(b):
  if k==39 and struct.unpack_from('<H',v)[0]==108:
   assert struct.unpack_from('<H',v,2)[0]==60
   sprite=v[:4];label=None
   for sk,sv,st in tags(v[4:]):
    if sk==43:label=sv.rstrip(b'\0').decode();all_labels.append(label)
    if sk==1 and label in MAPPING:sprite+=tag(12,redirect(label,MAPPING[label]));changed.append(label)
    sprite+=st
   out+=tag(39,sprite)
  else:
   if k==1 and not helpers_added:out+=tag(12,registry_helpers());helpers_added=True
   out+=t
 assert helpers_added
 assert set(changed)==set(MAPPING) and len(changed)==24
 assert set(MAPPING.values())<=set(all_labels)
 return pack(raw,h,out)
def patch_xml(raw):
 # Add alongside each native SetInputId declaration (some surfaces expose two UI instances).
 text=raw.decode('utf8');pattern=r'(<function name="SetInputId" funcname="fc_setInputId">.*?</function>)'
 new='\n      <function name="SetKcd2vrGlyphStyleD68" funcname="'+CALLBACK+'">\n        <param name="style" desc="2 Xbox art; 4 PS art; Xbox action bindings retained" type="int" />\n      </function>'
 text,n=re.subn(pattern,lambda m:m[1]+new,text,flags=re.S)
 assert n in (1,2),n
 return text.encode('utf8')

def generate(pak):
 result={};provenance={}
 with zipfile.ZipFile(pak) as z:
  for n,pin in EXPECTED.items():
   raw=z.read(n)
   if sha(raw)!=pin:raise ValueError('Exact retail member mismatch: '+n)
   baseline=patch_hud(raw)[0] if n=='Libs/UI/hud.gfx' else raw
   new=(patch_buttons(baseline) if n.endswith('/buttons.gfx') else patch_movie(baseline)) if n.endswith('.gfx') else patch_xml(baseline)
   result[n]=new;provenance[n]={'retail_sha256':sha(raw),'preserved_base_sha256':sha(baseline),'output_sha256':sha(new),'bytes':len(new)}
 return result,provenance

def main():
 ap=argparse.ArgumentParser();ap.add_argument('--pak',type=Path,required=True);ap.add_argument('--out',type=Path,default=ROOT/'companion/source');ap.add_argument('--report',type=Path,required=True);a=ap.parse_args()
 output,pins=generate(a.pak)
 for n,b in output.items():atomic(a.out/n,b)
 report={'ownership':'Xbox native semantic family2, glyph-only PS decoration selected by saved4','retail_member_pins':pins,'xbox_to_ps_frame_map':MAPPING,'native_execution':False,'headset_observed':False,'hot_refresh':{'registry_capacity':CAPACITY,'event_only':True,'registered_replay':'constant-time slot identity check','overflow':'new glyph art correct; live hot-refresh registration omitted, no eviction'}}
 atomic(a.report,(json.dumps(report,indent=2)+'\n').encode());print('D68 glyph assets generated:',len(output))
if __name__=='__main__':main()
