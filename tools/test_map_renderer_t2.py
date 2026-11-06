"""Execute ACTUAL generated AVM1 and Lua against explicit native/Flash leaves.

The tiny independent AVM1 reader implements only the opcodes this owned movie
uses. Function sub-block lengths follow SWF19/Ruffle, not builder internals.
This catches malformed actions, wrong stack order, clip churn and coordinate
errors. It is not a native Scaleform/image-decoder/headset acceptance test.
"""
from pathlib import Path
import argparse, json, math, struct, sys, zlib, zipfile
sys.path.insert(0,str(Path(__file__).resolve().parent))
sys.path.insert(0,"C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps")
import decode_ui90_gfx as swf
ROOT=Path(__file__).resolve().parents[1]

class Clip(dict):
    def __init__(self,vm):super().__init__();self.vm=vm
    def createEmptyMovieClip(self,name,depth):
        assert name not in self,"persistent glyph/tile unexpectedly rebuilt"
        c=Clip(self.vm);c["_depth"]=depth;self[name]=c;self.vm.created+=1;return c
    def beginFill(self,*a):self.vm.draws+=1
    def moveTo(self,*a):pass
    def lineTo(self,*a):pass
    def endFill(self):pass
    def setMask(self,m):self["_mask"]=m

class Loader:
    def __init__(self,vm):self.vm=vm;self.listeners=[]
    def addListener(self,listener):self.listeners.append(listener);return True
    def notify(self,name,*args):
        for listener in self.listeners:
            callback=listener.get(name)
            if callback: callback(*args)
    def loadClip(self,url,target):
        assert url.startswith("img://libs/ui/textures/kcd2vrmapt2/") and url.endswith(".dds")
        self.vm.loads.append(url);target["_source"]=url;return True
class Color:
    def __init__(self,target):self.target=target
    def setRGB(self,color):self.target["_rgb"]=color

def avm_string(x):return format(x,"g") if isinstance(x,(int,float)) and not isinstance(x,bool) else str(x)
class VM:
    def __init__(self,raw):
        self.created=self.draws=0;self.loads=[];self.functions={};self.stopped=False
        self.root=Clip(self);self.env={"_root":self.root,"Stage":{}}
        assert raw[:3]==b"CWS" and raw[3]==8
        body=zlib.decompress(raw[8:]);assert len(body)+8==struct.unpack_from('<I',raw,4)[0]
        bits=swf.Bits(body);n=bits.u(5);bounds=[bits.s(n) for _ in range(4)]
        assert bounds==[0,20480,0,20480]
        tags=list(swf.tags(body,bits.pos()+4));assert len([1 for t,b in tags if t==12])==1
        self.run(next(b for t,b in tags if t==12))
        assert self.stopped,"own timeline must stop after one initialization"
    def run(self,b,params=None):
        p=0;stack=[];scope={} if params is None else dict(params)
        def get(name):return scope[name] if name in scope else self.env.get(name)
        def member(o,k):return o.get(k) if isinstance(o,dict) else getattr(o,k)
        while p<len(b):
            code=b[p];p+=1
            if code==0:break
            n=struct.unpack_from('<H',b,p)[0] if code>=128 else 0
            if code>=128:p+=2
            data=b[p:p+n];assert len(data)==n;p+=n
            if code==0x9B:
                q=data.index(0);name=data[:q].decode();q+=1
                argc=struct.unpack_from('<H',data,q)[0];q+=2;args=[]
                for _ in range(argc):e=data.index(0,q);args.append(data[q:e].decode());q=e+1
                size=struct.unpack_from('<H',data,q)[0];q+=2;assert q==len(data)
                self.functions[name]=(args,b[p:p+size]);assert len(b[p:p+size])==size;p+=size
                self.env[name]=(lambda *a,_name=name:self.call(_name,*a))
            elif code==0x96:
                kind=data[0]
                if kind==0:assert data[-1]==0;value=data[1:-1].decode()
                elif kind==1:value=struct.unpack_from('<f',data,1)[0]
                elif kind==5:value=bool(data[1])
                elif kind==7:value=struct.unpack_from('<i',data,1)[0]
                else:raise AssertionError(('push kind',kind))
                stack.append(value)
            elif code==0x1C:stack.append(get(stack.pop()))
            elif code in (0x1D,0x3C):
                value,name=stack.pop(),stack.pop()
                if code==0x3C or name in scope:scope[name]=value
                else:self.env[name]=value
            elif code==0x4E:key,obj=stack.pop(),stack.pop();stack.append(member(obj,key))
            elif code==0x4F:value,key,obj=stack.pop(),stack.pop(),stack.pop();obj[key]=value
            elif code==0x47:
                c,a=stack.pop(),stack.pop();stack.append(avm_string(a)+avm_string(c) if isinstance(a,str) or isinstance(c,str) else a+c)
            elif code==0x49:c,a=stack.pop(),stack.pop();stack.append(a==c)
            elif code==0x48:c,a=stack.pop(),stack.pop();stack.append(a<c)
            elif code==0x12:stack.append(not stack.pop())
            elif code==0x9D:
                offset=struct.unpack('<h',data)[0]
                if stack.pop():p+=offset
                assert 0<=p<=len(b)
            elif code==0x52:
                name,obj,count=stack.pop(),stack.pop(),stack.pop()
                args=[stack.pop() for _ in range(count)];stack.append(getattr(obj,name)(*args))
            elif code==0x40:
                name,count=stack.pop(),stack.pop();args=[stack.pop() for _ in range(count)]
                stack.append(Loader(self) if name=="MovieClipLoader" else Color(*args) if name=="Color" else None)
            elif code==0x17:stack.pop()
            elif code==0x07:self.stopped=True
            else:raise AssertionError(('unknown actual opcode',hex(code),p))
        assert not stack,('unbalanced AVM1 stack',stack)
    def call(self,name,*args):
        params,code=self.functions[name];assert len(params)==len(args);self.run(code,dict(zip(params,args)))

def main():
    p=argparse.ArgumentParser();p.add_argument('--pak',type=Path,required=True);args=p.parse_args()
    with zipfile.ZipFile(args.pak) as z:
        raw=z.read('Libs/UI/KCD2VRMapT2.gfx');lua_source=z.read('Scripts/KCD2VR/map_navigation_t2.lua').decode()
        assert z.read('Scripts/KCD2VR/map_navigation_t2.lua')==(ROOT/'mods/KCD2VRMap/map_navigation_t2.lua').read_bytes()
        import xml.etree.ElementTree as ET
        xml=ET.fromstring(z.read('Libs/UI/UIElements/KCD2VRMapT2.xml'))
        el=xml.find('UIElement');assert el.attrib['mouseevents']=='0' and el.attrib['cursor']=='0'
        assert el.find('GFx/Constraints/Align').get('mode')=='fixdyntexsize'
        for r in ('trosky','kuttenberg'):
            mat=ET.fromstring(z.read('Objects/kcd2vr/map32/'+r+'.mtl'))
            tex=mat.find(".//Texture[@Map='Opacity']");assert tex.get('File')=='KCD2VRMapT2@0.ui' and tex.get('TexType')=='5'
            scroll=mat.find('SubMaterials')[1]
            assert scroll.get('Shininess')=='10' and scroll.get('Specular')=='0.005,0.005,0.005'
            assert scroll.find('PublicParams').get('DetailGlossScale')=='0'
            assert '%SNDUVS' in scroll.get('StringGenMask') and '%NORMAL_MAP' in scroll.get('StringGenMask')
            assert scroll.find("Textures/Texture[@Map='Diffuse']").get('File').endswith('scroll_diff.tif')
            assert scroll.find("Textures/Texture[@Map='Bumpmap']").get('File').endswith('scroll_ddna.tif')
        vm=VM(raw)
        assert set(vm.functions)=={'fc_tile','fc_marker','fc_clear'}
        created,draws=vm.created,vm.draws
        vm.call('fc_tile',0,'libs/ui/textures/kcd2vrmapt2/trosky_1.dds',256,0,25.,True)
        vm.call('fc_tile',0,'libs/ui/textures/kcd2vrmapt2/trosky_1.dds',257,1,26.,True)
        assert len(vm.loads)==1 and vm.env['map']['t0']['_x']==257
        vm.call('fc_marker',96,700,20,45,0x123456,True)
        assert vm.env['map']['m96']['dir']['_visible'] and not vm.env['map']['m96']['dot']['_visible']
        vm.call('fc_clear');assert all(not vm.env['map']['m'+str(i)]['_visible'] for i in range(97))
        assert (vm.created,vm.draws)==(created,draws)
    from lupa.lua51 import LuaRuntime
    lua=LuaRuntime(unpack_returned_tuples=True)
    def ui_call(element,instance,name,*values):
        assert element=='KCD2VRMapT2' and instance==0
        vm.call({'Tile':'fc_tile','Marker':'fc_marker','Clear':'fc_clear'}[name],*values)
    lua.globals().ui_call=ui_call
    lua.execute('''
now=10;level='levels/trosecko';pos={x=1822,y=2066,z=70};yaw=0;paperHidden=false
KCD2VRMap={world=42,awaitStart=false,entities={paper={entity={IsHidden=function()return paperHidden end}}}}
System={GetCurrTime=function()if clockMissing then error('clock missing')end;return now end,
 GetCVar=function(name)assert(name=='sv_map');return level end,LogAlways=function()end}
UIAction={CallFunction=function(...)return ui_call(...)end,ShowElement=function()error('screen UI forbidden')end}
g_localActor={id=42,inventory={},GetWorldPos=function()return pos end,GetWorldAngles=function()return {z=yaw}end}
''')
    lua.execute(lua_source)
    lua.execute('''
local meta=kcd2vr_map_t2_feed('T2|1|3|2500,2400|1,1850,2070,70,abc;4,1824,2066,70,def')
assert(string.find(meta,'T2|42|trosecko|',1,true) and string.sub(meta,-2)=='|1')
assert(KCD2VRMap.navigation.drawn==3 and KCD2VRMap.navigation.status==3)
local u,v=KCD2VRMap.NavigationProject('trosecko',1822,2066);assert(math.abs(u-.5)<1e-10 and math.abs(v-.5)<1e-10)
now=10.6;kcd2vr_map_t2_feed('T2|1|1|2500,2400|');assert(KCD2VRMap.navigation.drawn==1) -- APSE waypoint without compass
now=11.2;kcd2vr_map_t2_feed('T2|1|0||');assert(KCD2VRMap.navigation.drawn==0) -- native data unavailable clears only markers
now=11.8;kcd2vr_map_t2_feed('T2|1|3||1,1850,2070,70,abc');assert(KCD2VRMap.navigation.drawn==1)
now=13.4;KCD2VRMap.NavigationTick();assert(KCD2VRMap.navigation.drawn==0) -- stale clear, player remains
now=14;kcd2vr_map_t2_feed('T2|2|3||1,NaN,2,3,abc');assert(#KCD2VRMap.navigation.markers==0)
now=15;kcd2vr_map_t2_feed('T2|2|3||1,1850,2070,70,abc');assert(KCD2VRMap.navigation.drawn==1)
now=15.6;kcd2vr_map_t2_feed('T2|999|3||malformed');assert(KCD2VRMap.navigation.activation==2)
now=16.2;kcd2vr_map_t2_feed('T2|3|3||1,1850,2070,70,abc');assert(KCD2VRMap.navigation.drawn==1)
clockMissing=true;KCD2VRMap.NavigationTick();assert(not KCD2VRMap.navigation.open);clockMissing=false
''')
    assert all(not vm.env['map']['m'+str(i)]['_visible'] for i in range(97))
    assert all(not vm.env['map']['t'+str(i)]['_visible'] for i in range(4))
    lua.execute('''
paperHidden=true;KCD2VRMap.NavigationTick();assert(not KCD2VRMap.navigation.open)
paperHidden=false;now=17;level='levels/kutnohorsko';pos.x=2048;pos.y=2048
local meta=kcd2vr_map_t2_feed('T2|4|1|2100,2200|');assert(string.find(meta,'|kutnohorsko|',1,true))
local u,v=KCD2VRMap.NavigationProject('kutnohorsko',2048,2048);assert(math.abs(u-.5)<1e-10 and math.abs(v-.4)<1e-10)
KCD2VRMap.NavigationRetire();assert(KCD2VRMap.feedGeneration==2)
level='unknown';assert(string.find(kcd2vr_map_t2_feed('T2|4|0||'),'|unknown|',1,true))
''')
    assert all(not vm.env['map']['t'+str(i)]['_visible'] for i in range(4))
    # A valid native/game clock may reset across save/load while the actor/VM
    # and map module survive. Exercise the ACTUAL packaged Lua and bytecode:
    # no stale artwork or marks may remain throttled against the former clock.
    lua.execute('''
level='levels/trosecko';now=100;pos.x=1200;pos.y=2400
kcd2vr_map_t2_feed('T2|5|3||1,1300,2400,20,abc;')
assert(KCD2VRMap.navigation.lastRender==100 and KCD2VRMap.navigation.drawn==1)
local generation=KCD2VRMap.feedGeneration
now=.1;pos.x=1800
local meta=kcd2vr_map_t2_feed('T2|6|3||1,1900,2400,20,def;')
assert(KCD2VRMap.navigation.lastRender==.1 and KCD2VRMap.navigation.lastFeed==.1)
assert(KCD2VRMap.navigation.drawn==1 and KCD2VRMap.navigation.open)
assert(KCD2VRMap.feedGeneration==generation+1)
assert(string.sub(meta,-1)==tostring(KCD2VRMap.feedGeneration))
-- Rewind first observed by the timer must retire old objective/waypoint data;
-- only the live player/artwork may redraw before the next fresh native feed.
now=10;kcd2vr_map_t2_feed('T2|7|3|1950,2400|1,1900,2400,20,def;')
generation=KCD2VRMap.feedGeneration
now=.2;KCD2VRMap.NavigationTick()
assert(KCD2VRMap.navigation.status==0 and #KCD2VRMap.navigation.markers==0 and KCD2VRMap.navigation.waypoint==nil)
assert(KCD2VRMap.navigation.lastRender==.2 and KCD2VRMap.feedGeneration==generation+1)
now=.8;kcd2vr_map_t2_feed('T2|7|3||1,1900,2400,20,def;')
assert(KCD2VRMap.navigation.lastRender==.8 and KCD2VRMap.navigation.drawn==1)
-- Missing-clock samples cannot erase the previous valid sample and disguise
-- another rewind. Recovery still needs fresh finite player/native packet data.
clockMissing=true;KCD2VRMap.NavigationTick();clockMissing=false
now=.05;generation=KCD2VRMap.feedGeneration
kcd2vr_map_t2_feed('T2|8|1||')
assert(KCD2VRMap.navigation.lastRender==.05 and KCD2VRMap.feedGeneration==generation+1)
KCD2VRMap.NavigationRetire()
''')
    assert all(not vm.env['map']['t'+str(i)]['_visible'] for i in range(4))
    assert vm.created==created and vm.draws==draws
    # A missing/refused private movie cannot be marked as a successful surface
    # just because pcall itself did not throw.
    lua.execute("UIAction.CallFunction=function(...)return false end;now=20;kcd2vr_map_t2_feed('T2|9|1||');assert(not KCD2VRMap.navigation.open and KCD2VRMap.navigation.surfaceRefused)")
    print(json.dumps({'actual_generated_AVM1':'PASS','actual_packaged_navigation_Lua51':'PASS',
      'private_material_only':True,'persistent_clips':created,'rebuilds_after_updates':0,
      'native_image_decode_and_Scaleform':'NOT_RUN','headset':'NOT_RUN'},indent=2))
if __name__=='__main__':main()
