"""Execute injected AVM1 instruction subset and prove all prior asset leaves survive.
This is an offline decoder/interpreter fixture, NOT Scaleform/headset execution.
"""
from pathlib import Path
import argparse, hashlib, json, re, struct, sys, zipfile, zlib
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import patch_glyphs_d68 as p
from patch_hud_notifications_d63 import patch as hud
checks=0
def ck(ok,why):
 global checks;checks+=1
 if not ok:raise AssertionError(why)
def flat(raw):
 h,b=p.unpack(raw);out=[]
 def visit(b,path=()):
  for k,v,r in p.tags(b):
   if k==39:
    ident=struct.unpack_from('<H',v)[0];out.append((path,k,v[:4]));visit(v[4:],path+(ident,))
   else:out.append((path,k,v))
 visit(b);return h,out

class Clip(dict):
 def __init__(self):super().__init__(_parent=object());self.frame=None;self.replays=0
class Function:
 def __init__(self,params,body):self.params=params;self.body=body
class VM:
 """Executes emitted AVM1, not a Python rewrite of registry policy.
 MovieClip.gotoAndStop is the only simulated native timeline operation.
 """
 def __init__(self):self.root={};self.glob={};self.calls=[];self.steps=0;self.array_reads=0
 def get(self,obj,key):
  if isinstance(obj,list):
   self.array_reads+=1
   if key=='length':return len(obj)
   return obj[key] if isinstance(key,int) and 0<=key<len(obj) else None
  if isinstance(obj,Function) and key=='apply':return lambda this,args:self.invoke(obj,args,this)
  return obj.get(key) if isinstance(obj,dict) else None
 def set(self,obj,key,val):
  if isinstance(obj,list):assert isinstance(key,int) and 0<=key<len(obj);obj[key]=val
  else:assert isinstance(obj,dict);obj[key]=val
 def invoke(self,fn,args,this=None):
  if callable(fn):return fn(*args)
  if isinstance(fn,Function):return self.run(fn.body,{**dict(zip(fn.params,args)),'arguments':args},this)[0]
  raise AssertionError(('not callable',fn))
 def run(self,code,scope=None,this=None):
  scope={} if scope is None else scope;scope=dict(scope);scope['this']=this
  stack=[];at=0;frame=None;budget=0
  while at<len(code):
   budget+=1;self.steps+=1
   assert budget<100000,'bounded emitted bytecode execution'
   op=code[at];at+=1
   if op==0:break
   n=struct.unpack_from('<H',code,at)[0] if op>=128 else 0;at+=2 if op>=128 else 0
   data=code[at:at+n];at+=n
   if op==0x96:
    q=0
    while q<len(data):
     typ=data[q];q+=1
     if typ==0:e=data.index(0,q);stack.append(data[q:e].decode());q=e+1
     elif typ==7:stack.append(struct.unpack_from('<i',data,q)[0]);q+=4
     elif typ==3:stack.append(None)
     else:raise AssertionError(('push',typ))
   elif op==0x1c:
    key=stack.pop();stack.append(self.glob if key=='_global' else self.root if key=='_root' else scope.get(key,self.root.get(key)))
   elif op in (0x1d,0x3c):val=stack.pop();scope[stack.pop()]=val
   elif op==0x4e:key=stack.pop();obj=stack.pop();stack.append(self.get(obj,key))
   elif op==0x4f:val=stack.pop();key=stack.pop();obj=stack.pop();self.set(obj,key,val)
   elif op==0x49:
    b=stack.pop();a=stack.pop();stack.append(a is b if isinstance(a,(dict,list,Function)) or isinstance(b,(dict,list,Function)) else a==b)
   elif op==0x48:b=stack.pop();a=stack.pop();stack.append(a<b)
   elif op==0x47:b=stack.pop();a=stack.pop();stack.append(a+b)
   elif op==0x0c:b=stack.pop();a=stack.pop();stack.append(a*b)
   elif op==0x0d:b=stack.pop();a=stack.pop();stack.append(a/b)
   elif op==0x10:b=stack.pop();a=stack.pop();stack.append(bool(a and b))
   elif op==0x11:b=stack.pop();a=stack.pop();stack.append(bool(a or b))
   elif op==0x12:stack.append(not stack.pop())
   elif op==0x99:at+=struct.unpack('<h',data)[0]
   elif op==0x9d:
    if stack.pop():at+=struct.unpack('<h',data)[0]
   elif op==0x8c:
    frame=data.rstrip(b'\0').decode()
    if isinstance(this,Clip):this.frame=frame
   elif op==0x07:pass
   elif op==0x3d:
    name=stack.pop();argc=stack.pop();args=[stack.pop() for _ in range(argc)];self.calls.append((name,args));stack.append(None)
   elif op==0x52:
    name=stack.pop();obj=stack.pop();argc=stack.pop();args=[stack.pop() for _ in range(argc)]
    if isinstance(obj,Clip) and name=='gotoAndStop':
     assert len(args)==1 and args[0] in p.MAPPING
     obj.frame=args[0];obj.replays+=1;self.run(p.redirect(args[0],p.MAPPING[args[0]]),this=obj);stack.append(None)
    else:stack.append(self.invoke(self.get(obj,name),args,obj))
   elif op==0x40:
    name=stack.pop();argc=stack.pop();args=[stack.pop() for _ in range(argc)];assert name=='Array' and args==[p.CAPACITY];stack.append([None]*args[0])
   elif op==0x9b:
    q=data.index(0);name=data[:q].decode();count=struct.unpack_from('<H',data,q+1)[0];q+=3;params=[]
    for _ in range(count):e=data.index(0,q);params.append(data[q:e].decode());q=e+1
    size=struct.unpack_from('<H',data,q)[0];assert q+2==len(data)
    fn=Function(params,code[at:at+size]);at+=size
    if name:
     scope[name]=fn
     if scope.get('__install__'):self.root[name]=fn
    else:stack.append(fn)
   elif op==0x3e:return stack.pop(),frame
   elif op==0x17:stack.pop()
   else:raise AssertionError(('unknown op',op))
  return None,frame
 def show(self,clip,label):
  clip.frame=label;self.run(p.redirect(label,p.MAPPING[label]),this=clip)
 def style(self,style):
  before=len(self.calls);self.run(function_body(p.callback()),{'style':style});return self.calls[before:]

def function_body(action):
 ck(action[0]==0x9b,'DefineFunction opcode');n=struct.unpack_from('<H',action,1)[0];header=action[3:3+n]
 fields=header.split(b'\0');ck(header.startswith(p.CALLBACK.encode()+b'\0'),'named native-registered UI callback')
 size=struct.unpack_from('<H',header,len(header)-2)[0];return action[3+n:3+n+size]

def main():
 ap=argparse.ArgumentParser();ap.add_argument('--pak',type=Path,required=True);ap.add_argument('--out',type=Path,required=True);a=ap.parse_args();generated,pins=p.generate(a.pak)
 with zipfile.ZipFile(a.pak) as z:
  for n,new in generated.items():
   actual=(p.ROOT/'companion/source'/n).read_bytes()
   expected=new
   # D72 has its own native entity-name callback; the D68 bytecode and
   # placements below still compare against their exact original source.
   if n=='Libs/UI/hud.gfx':
    import patch_alchemy_name_d72 as names_d72
    expected=names_d72.patch(new)
   elif n=='Libs/UI/UIElements/HUD.xml':
    actual=re.sub(rb'\n      <function name="SetKcd2vrAlchemyNameD72".*?</function>',b'',actual,flags=re.S)
   import patch_prompt_presentation_d80 as prompts_d80
   if n.endswith('.gfx'):expected=prompts_d80.patch_movie(n,expected)
   elif n=='Libs/UI/UIElements/HUD.xml':actual=re.sub(rb'\n      <function name="SetKcd2vrPromptStyleD80".*?</function>\n',b'',actual,flags=re.S)
   if n=='Libs/UI/UIElements/HUD.xml':
    # The exact parent->D80 byte proof is in prompt_assets_d80_test. Compare the
    # retained XML tree here; the parent acquired mixed newlines/EOF whitespace.
    import xml.etree.ElementTree as ET
    def tree_shape(raw):
     def shape(e):return (e.tag,dict(e.attrib),(e.text or '').strip(),tuple(shape(c) for c in e))
     return shape(ET.fromstring(raw))
    ck(tree_shape(expected)==tree_shape(actual),'retained HUD declarations/content')
   else: ck(expected==actual,'canonical matches exact regeneration plus scoped D72 name bridge '+n)
   orig=z.read(n);base=hud(orig)[0] if n=='Libs/UI/hud.gfx' else orig
   if n.endswith('.gfx'):
    bh,bl=flat(base);ah,al=flat(new);ck(bh==ah,'stage/header preserved '+n)
    injected=[];kept=[]
    for row in al:
     path,k,v=row
     if k==12 and (v==p.callback() or v==p.registry_helpers() or v in [p.redirect(k,t) for k,t in p.MAPPING.items()]):injected.append(row)
     else:kept.append(row)
    ck(bl==kept,'every preexisting script/placement/texture/shape/frame leaf exact '+n)
    ck(len(injected)==(25 if n.endswith('/buttons.gfx') else 1),'bounded injection count '+n)
   else:
    strip=re.sub(rb'\n      <function name="SetKcd2vrGlyphStyleD68".*?</function>',b'',new,flags=re.S)
    ck(strip==base,'only new function declarations '+n)
    import xml.etree.ElementTree as ET
    tree=ET.fromstring(new);nnew=len(tree.findall('.//function[@name="SetKcd2vrGlyphStyleD68"]'));nold=len(tree.findall('.//function[@name="SetInputId"]'))
    ck(nnew==nold and nnew in (1,2),'UI instance coverage '+n)
 # Real emitted global functions, callback and frame actions, not mirrored policy.
 vm=VM();vm.run(p.registry_helpers());first_register=vm.glob[p.REGISTER]
 vm.run(p.registry_helpers());ck(vm.glob[p.REGISTER] is first_register,'multiple loaded button movies preserve existing global helpers')
 clips={label:Clip() for label in p.MAPPING}
 for style in (2,4,2,4,999):
  ck(vm.style(style)==[('fc_setInputId',[2])],'same Xbox family for every decoration selection')
  for old,new in p.MAPPING.items():
   vm.show(clips[old],old)
   ck(clips[old].frame==(new if style==4 else old),'glyph art '+old+' style'+str(style))
 ck(len(vm.glob[p.REGISTRY])==p.CAPACITY,'fixed registry extent')
 ck(sum(x is not None for x in vm.glob[p.REGISTRY])==len(clips),'duplicate frame activations never duplicate entries')
 # Hot switch without a native widget redraw: callback alone must replay art.
 for style in (4,2,4,2):
  oldreplays=sum(x.replays for x in clips.values());vm.style(style)
  ck(all(c.frame==(p.MAPPING[n] if style==4 else n) for n,c in clips.items()),'hot switch refreshes already live glyphs in both directions')
  ck(sum(x.replays for x in clips.values())-oldreplays==len(clips),'changed event replays each registered instance once')
 oldreplays=sum(x.replays for x in clips.values());vm.style(2)
 ck(sum(x.replays for x in clips.values())==oldreplays,'equal art flag does not scan/replay')
 previous_reads=vm.array_reads;vm.show(clips['xi_a'],'xi_a')
 ck(vm.array_reads-previous_reads==1,'already registered timeline action is constant-time single slot lookup')
 # Retained original Xbox label updates if the native widget changes its button.
 vm.show(clips['xi_a'],'xi_y');vm.style(4);ck(clips['xi_a'].frame=='pad_triangle','latest semantic button retained')
 # Destroyed/recreated clips may share native paths; identity must not alias them.
 dead=clips['xi_b'];slot=dead[p.SLOT];dead['_parent']=None;replays=dead.replays
 fresh=Clip();vm.show(fresh,'xi_b');ck(vm.glob[p.REGISTRY][slot] is fresh,'first registration reuses removed slot by identity')
 vm.style(2);ck(dead.replays==replays and fresh.frame=='xi_b','removed clip is never replayed')
 no_label=clips['xi_x'];slot=no_label[p.SLOT];del no_label[p.SEMANTIC];vm.style(4)
 ck(vm.glob[p.REGISTRY][slot] is None,'lost marker pruned during changed event')
 # A stale/corrupt stored slot cannot duplicate or steal a live clip's slot.
 live=clips['xi_y'];real=live[p.SLOT];live[p.SLOT]=p.CAPACITY+9;vm.show(live,'xi_y')
 ck(live[p.SLOT]==real and sum(x is live for x in vm.glob[p.REGISTRY])==1,'stale slot falls back to identity-deduplicating scan')
 # Fill via real emitted registration. Overflow never evicts live entries and
 # still renders correct new artwork; its hot switching is explicitly bounded.
 full=VM();full.run(p.registry_helpers());full.style(4);many=[Clip() for _ in range(p.CAPACITY)]
 for c in many:full.show(c,'xi_a')
 ck(len(full.glob[p.REGISTRY])==p.CAPACITY and all(a is b for a,b in zip(full.glob[p.REGISTRY],many)),'full capacity uses exactly fixed storage')
 extra=Clip();full.show(extra,'xi_b')
 ck(extra.frame=='pad_circle' and p.SLOT not in extra,'overflow keeps new glyph art but does not allocate/evict')
 ck(all(a is b for a,b in zip(full.glob[p.REGISTRY],many)),'overflow retains all live registrants')
 full.style(2);ck(all(c.frame=='xi_a' for c in many) and extra.frame=='pad_circle','bounded overflow hot-refresh limitation is explicit')
 many[17]['_parent']=None;full.style(4);ck(full.glob[p.REGISTRY][17] is None,'destroyed clips pruned on art event')
 full.show(extra,'xi_b');ck(full.glob[p.REGISTRY][17] is extra,'later frame activation reclaims available slot')
 # No registry/helper yet is valid at surface startup: keep native family2.
 early=VM();ck(early.style(4)==[('fc_setInputId',[2])],'surface can initialize before buttons movie')
 early.run(p.registry_helpers());new=Clip();early.show(new,'xi_start');ck(new.frame=='pad_start','late button movie sees existing art preference')
 # A direct native Xbox refresh has no code that clears the decoration flag;
 # the original fc_setInputId function remains byte-for-byte unchanged above.
 ck(all('PS' not in k for k in p.MAPPING),'mapping contains only Xbox source labels')
 # Input/action profile is deliberately not generated or overridden.
 ck(not any('Config/' in n for n in generated),'no profile remapping')
 from companion_sources_d39 import SOURCE_PAIRS,ALLOWED_ADDITIONS
 for n in generated:ck(SOURCE_PAIRS['companion/source/'+n]==n and n in ALLOWED_ADDITIONS,'packager closure '+n)
 a.out.parent.mkdir(parents=True,exist_ok=True);result=json.dumps({'checks':checks,'passed':True,'asset_count':len(generated),'native_scaleform_execution':False,'headset_observed':False,'scope':'all native input/placement/action bytecode leaves unchanged; art-only callback, bounded event registry +24frame redirects','hot_refresh_capacity':p.CAPACITY,'overflow_limit':'above512simultaneously registered liveglyphs no live eviction; overflow art updates only on normal native redraw'},indent=2)+'\n';p.atomic(a.out,result.encode())
 print('D68 glyph asset/bytecode fixture',checks,'/',checks,'PASS (offline, no Scaleform/headset)')
if __name__=='__main__':main()
