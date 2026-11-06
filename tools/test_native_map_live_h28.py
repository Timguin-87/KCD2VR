"""Execute shipped own AVM1 + production Lua51; native engine leaves substituted."""
from pathlib import Path
import sys,struct,zlib,zipfile,xml.etree.ElementTree as ET,json
sys.path.insert(0,'C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps')
from lupa.lua51 import LuaRuntime
import build_native_map_live_h28 as prod
import decode_ui90_gfx as swf
ROOT=Path(__file__).resolve().parents[1]
class Clip(dict):
 def __init__(self,vm):super().__init__();self.vm=vm;self['_visible']=True
 def __bool__(self):return True
 def getNextHighestDepth(self):return max([v.get('_depth',0) for v in self.values() if isinstance(v,Clip)]+[0])+1
 def createEmptyMovieClip(self,name,depth):
  assert name not in self,'persistent clip rebuilt'
  c=Clip(self.vm);c['_depth']=depth;self[name]=c;self.vm.created+=1;return c
 def beginFill(self,*a):self.vm.draws+=1
 def moveTo(self,*a):pass
 def lineTo(self,*a):pass
 def endFill(self):pass
 def setMask(self,m):self['_mask']=m
class Color:
 def __init__(self,c):self.c=c
 def setRGB(self,v):self.c['_rgb']=v
class Loader:
 def __init__(self,v):self.v=v
 def loadClip(self,url,c):
  assert url.startswith('img://libs/ui/textures/kcd2vrmapt2/') and url.endswith('.dds')
  self.v.loads.append(url);c['_source']=url;return True
class VM:
 def __init__(self):
  self.created=self.draws=self.now=0;self.loads=[];self.functions={};self.root=Clip(self);self.env={'_root':self.root};self.native=[];self.events=[];self.deliver=lambda *a:None
  for n in prod.ANCHORS:self.root[n]=Clip(self)
  def original(*args):self.native.append(args);self.env['g_ImagesA']=[]
  self.root['fc_setFullBook']=original
  self.env['flash']={'external':{'ExternalInterface':{'call':self.emit}}}
  self.run(prod.methods())
 def emit(self,*args):self.events.append(args);self.deliver(*args)
 def method(self,obj,name,args):
  if isinstance(obj,list) and name=='join':return args[0].join(str(x) for x in obj)
  if isinstance(obj,str) and name=='indexOf':return obj.find(args[0])
  f=obj.get(name) if isinstance(obj,dict) and name in obj else getattr(obj,name)
  return f(*args)
 def run(self,b,params=None,this=None):
  p=0;stack=[];scope=dict(params or {});scope['this']=this or self.root
  def get(n):return scope[n] if n in scope else self.env.get(n,self.root.get(n))
  while p<len(b):
   c=b[p];p+=1;n=struct.unpack_from('<H',b,p)[0] if c>=128 else 0;p+=2 if c>=128 else 0;d=b[p:p+n];p+=n
   if c==0:break
   if c==0x9b:
    at=d.index(0);name=d[:at].decode();count=struct.unpack_from('<H',d,at+1)[0];q=at+3;names=[]
    for _ in range(count):e=d.index(0,q);names.append(d[q:e].decode());q=e+1
    size=struct.unpack_from('<H',d,q)[0];body=b[p:p+size];p+=size
    def f(*args,names=names,body=body):assert len(args)==len(names);self.run(body,dict(zip(names,args)))
    self.functions[name]=f;self.root[name]=f
   elif c==0x96:
    q=0
    while q<len(d):
     t=d[q];q+=1
     if t==0:e=d.index(0,q);stack.append(d[q:e].decode());q=e+1
     elif t==5:stack.append(bool(d[q]));q+=1
     elif t==7:stack.append(struct.unpack_from('<i',d,q)[0]);q+=4
     elif t==1:stack.append(struct.unpack_from('<f',d,q)[0]);q+=4
     else:raise AssertionError(t)
   elif c==0x1c:stack.append(get(stack.pop()))
   elif c==0x1d:v=stack.pop();name=stack.pop();self.env[name]=v;self.root[name]=v
   elif c==0x3c:v=stack.pop();scope[stack.pop()]=v
   elif c==0x4e:k=stack.pop();obj=stack.pop();stack.append(obj.get(k) if isinstance(obj,dict) else getattr(obj,k))
   elif c==0x4f:v=stack.pop();k=stack.pop();obj=stack.pop();obj[k]=v
   elif c==0x47:
    v=stack.pop();u=stack.pop();stack.append(str(u)+str(v) if isinstance(u,str) or isinstance(v,str) else u+v)
   elif c==0x0b:v=stack.pop();u=stack.pop();stack.append(u-v)
   elif c==0x67:v=stack.pop();u=stack.pop();stack.append(u>v)
   elif c==0x34:stack.append(self.now)
   elif c==0x49:v=stack.pop();u=stack.pop();stack.append(u==v)
   elif c==0x12:stack.append(not stack.pop())
   elif c==0x9d:
    offset=struct.unpack('<h',d)[0]
    if stack.pop():p+=offset
   elif c==0x52:
    name=stack.pop();obj=stack.pop();n=stack.pop();args=[stack.pop() for _ in range(n)];stack.append(self.method(obj,name,args))
   elif c==0x40:
    name=stack.pop();n=stack.pop();args=[stack.pop() for _ in range(n)];stack.append(Loader(self) if name=='MovieClipLoader' else Color(*args))
   elif c==0x17:stack.pop()
   else:raise AssertionError(hex(c))
  assert not stack,stack
 def call(self,n,*args):return self.method(self.root,n,args)
 def content(self,images):self.env['g_ImagesA']=images;self.call('fc_setFullBook',1.,4,0)
 def watch(self):
  for n in prod.ANCHORS:
   c=self.root[n].get(prod.NAME)
   if c:self.run(self.watch_code,{},c)
def main(pak):
 vm=VM();assert vm.created==0,'normal book initialization allocated map clips'
 lua=LuaRuntime(unpack_returned_tuples=True)
 lua.execute("""t=1;worldMap='trosecko';pos={x=1000,y=1000,z=10};yaw=0;KCD2VRMap={world=nil,awaitStart=true,entities={}};g_localActor={id=42,inventory={}}
 function g_localActor:GetWorldPos()return pos end
 function g_localActor:GetWorldAngles()return {z=yaw}end
 System={GetEntity=function(id)return id==42 and g_localActor or nil end,GetCurrTime=function()return t end,GetCVar=function(k)assert(k=='sv_map');return worldMap end}
 UIAction={GetArray=function()error('no instance probing')end}
 """)
 registrations=[];calls=[];listener={}
 def reg(table,element,instance,event,callback):
  assert element=='GeneralBook' and instance==-1 and event=='OnKCD2VRMapContent';registrations.append(instance);listener['f']=table[callback];return True
 def ui(element,instance,name,*args):
  assert element=='GeneralBook' and instance==17;calls.append(name);vm.call(prod.FUN[name.removeprefix('KCD2VRMap')],*args);return True
 lua.globals().pyreg=reg;lua.globals().pyui=ui
 lua.execute('UIAction.RegisterElementListener=function(...)return pyreg(...)end;UIAction.CallFunction=function(...)return pyui(...)end')
 with zipfile.ZipFile(pak) as z:
  for name in ['map_navigation_t2.lua','map_native_live_h28.lua']:
   data=z.read('Scripts/KCD2VR/'+name);assert data==(ROOT/'mods/KCD2VRMap'/name).read_bytes();lua.execute(data.decode())
  gfx=z.read('Libs/UI/GeneralBook.gfx');xml=ET.fromstring(z.read('Libs/UI/UIElements/GeneralBook.xml'))
  event=xml.find("UIElement/events/event[@name='OnKCD2VRMapContent']");assert event is not None
  native=zlib.decompress(gfx[8:]);addition=prod.tag(12,prod.methods());at=native.index(addition)
  import hashlib
  assert hashlib.sha256(native[:at]+native[at+len(addition):]).hexdigest()=='65ad7f52ebe5ee955eecfc5998bcb253acdaa72d5e93b5b17a388514dd333069'
  bits=swf.Bits(native);bits.u(bits.u(5)*4);tags=list(swf.tags(native,bits.pos()+4));assert any(t==12 and b==prod.methods() for t,b in tags)
 def deliver(event,payload):
  if 'f' in listener:listener['f']('GeneralBook',17,event,lua.table_from([payload]))
 vm.deliver=deliver
 assert lua.eval('KCD2VRMap.NativeMapArmH28()');assert registrations==[-1]
 vm.content([0,'kcd2vr_map_trosky',1,'kcd2vr_map_kuttenberg']);assert vm.native==[(1.,4,0)] and vm.created==1 # bounded metadata notice only
 feed=lua.globals().kcd2vr_map_native_live_h28
 reply=feed('T2|99|3|1100,1000|1,1050,1000,10,abc;69,1000,1050,10,def;');assert reply.startswith('T2|42|trosecko|')
 assert 0<len(vm.loads)<=16 # bounded visible tile set, reused on all page quads
 loads=len(vm.loads)
 created,draws=vm.created,vm.draws
 for name in prod.ANCHORS:
  c=vm.root[name][prod.NAME];assert (c['_x'],c['_y'],c['_xscale'],c['_yscale'])==(-512,0,200,200)
  assert c['map']['m0']['_x']==512 and c['map']['m0']['_y']==256
  assert abs(c['map']['m1']['_x']-576)<.001 and abs(c['map']['m2']['_x']-544)<.001
 for i in range(100):lua.execute('t=t+.5');feed('T2|99|3|1100,1000|1,1050,1000,10,abc;')
 assert vm.created==created and vm.draws==draws and len(vm.loads)==loads,'persistent pages/markers recreated or DDS reloaded'
 vm.now=2000
 watch=vm.functions['kcd2vr_h28_watch'].__kwdefaults__['body']
 for n in prod.ANCHORS:vm.run(watch,{},vm.root[n][prod.NAME]);assert not vm.root[n][prod.NAME]['_visible']
 lua.execute('t=t+.5;pos.x=1050;yaw=math.pi/2');feed('T2|99|3|15000,1000|1,1100,1000,10,abc;')
 c=vm.root['image0'][prod.NAME]['map'];assert abs(c['m0']['_rotation']+90)<.01 and abs(c['m2']['_x']-544)<.01
 assert c['m1']['dir']['_visible'] and c['m1']['_x']==750
 lua.execute("t=t+.5;worldMap='unsupported_world'");assert 'unknown' in feed('T2|99|1||');assert not vm.root['image0'][prod.NAME]['_visible']
 lua.execute("t=t+.5;worldMap='trosecko'");assert feed('T2|99|1||').startswith('T2|42|trosecko|');assert vm.root['image0'][prod.NAME]['_visible']
 old=len(calls);assert feed('OFF')=='L8|OFF' and len(calls)==old
 assert feed('T2|100|1||')=='L8|READING_OWNER_CHANGED'
 vm.content([0,'ordinary_book_image']);assert not vm.root['image0'][prod.NAME]['_visible'];assert feed('T2|100|1||')=='L8|NO_CURRENT_SURFACE_EVENT'
 assert len(vm.native)==2 and vm.native[-1]==(1.,4,0),'native content handler not forwarded exactly once'
 assert lua.eval('KCD2VRMap.NativeMapArmH28()');listener.clear();vm.content([0,'kcd2vr_map_trosky'])
 assert feed('T2|101|1||')=='L8|NO_CURRENT_SURFACE_EVENT' and 'f' in listener
 vm.now=3000;notice=vm.root['__kcd2vr_notice_h28'];vm.run(vm.functions['kcd2vr_h28_notice'].__kwdefaults__['body'],{},notice)
 assert feed('T2|101|1||').startswith('T2|42|trosecko|')

 print(json.dumps({'result':'PASS','actual_packaged_Lua51_and_AVM1':True,'dynamic_instance':17,'no_UI_instance_probing':True,'native_content_calls_preserved':True,'persistent_map_updates':100,'idle_or_OFF_UI_calls':0,'native_engine_headset':'NOT_RUN'},indent=2))
if __name__=='__main__':main(Path(sys.argv[1]))
