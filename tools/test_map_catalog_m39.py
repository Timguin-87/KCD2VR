"""Execute owned Lua and generated AVM1; independently check native transforms."""
from pathlib import Path
import sys,json,math
sys.path.insert(0,str(Path(__file__).resolve().parent))
from test_map_renderer_t2 import VM
import build_map_navigation_t2 as renderer
import build_local_parchment_m39 as builder
from lupa.lua51 import LuaRuntime
root=Path(__file__).resolve().parents[1]
c=builder.collect(builder.Inputs(builder.DATA))
vm=VM(renderer.gfx(tile_count=20,wide=True,marker_count=257))
assert set(vm.functions)=={'fc_tile','fc_tile_rect','fc_marker','fc_clear'}
created,draws=vm.created,vm.draws
vm.call('fc_tile_rect',19,'libs/ui/textures/kcd2vrmapt2/local_trosky.dds',12,13,21,35,True)
assert vm.env['map']['t19']['_xscale']==21 and vm.env['map']['t19']['_yscale']==35
vm.call('fc_marker',256,100,100,23,0x123456,True)
assert vm.env['map']['m256']['_visible']
l=LuaRuntime(unpack_returned_tuples=True)
def ui(element,instance,name,*args):
 assert element=='KCD2VRMapT2' and instance==0
 
 try:vm.call({'TileRect':'fc_tile_rect','Tile':'fc_tile','Marker':'fc_marker','Clear':'fc_clear'}[name],*args)
 except Exception as e:print('UI_ERROR',name,args,repr(e));raise
l.globals().ui_call=ui
l.execute('''
now=10;level='levels/trosecko';pos={x=1822,y=2066,z=70};yaw=0;paperHidden=false
KCD2VRMap={world=42,awaitStart=false,entities={paper={entity={IsHidden=function()return paperHidden end}}}}
System={GetCurrTime=function()return now end,GetCVar=function(k)assert(k=='sv_map');return level end,LogAlways=function()end}
UIAction={CallFunction=function(...)return ui_call(...)end}
g_localActor={id=42,inventory={},GetWorldPos=function()return pos end,GetWorldAngles=function()return {z=yaw}end}
''')
l.execute('KCD2VRMapCatalogM39='+builder.lua(c))
l.execute((root/'mods/KCD2VRMap/map_navigation_t2.lua').read_text())
pixels=l.globals().KCD2VRMap.MapPixelsM39
view=l.globals().KCD2VRMap.MapViewM39
def native(r,x,y):
 d=c['levels'][r];a=-math.radians(d['rotation']);co,si=math.cos(a),math.sin(a)
 # Independent rotate of world point, followed by native rotated square bounds.
 qx=(x-d['world'][0])/d['world'][2];qy=(d['level'][1]-y-d['world'][1])/d['world'][3]
 corners=[(u*co-v*si,u*si+v*co) for u,v in ((0,0),(1,0),(0,1),(1,1))]
 lo=[min(v[k] for v in corners) for k in (0,1)];hi=[max(v[k] for v in corners) for k in (0,1)]
 return tuple(((qx*co-qy*si,qx*si+qy*co)[k]-lo[k])/(hi[k]-lo[k])*d['tiles'][k]*2048-d['crop'][k]*2048 for k in (0,1))
def unproject(r,x,y):
 p=native(r,0,0);a=native(r,1,0);b=native(r,0,1)
 ax,ay=a[0]-p[0],a[1]-p[1];bx,by=b[0]-p[0],b[1]-p[1];det=ax*by-ay*bx
 return ((x-p[0])*by-(y-p[1])*bx)/det,((y-p[1])*ax-(x-p[0])*ay)/det
for r in c['levels']:
 for x,y in ((0,0),(100,1700),(2048,2048),(4096,4096)):
  assert max(abs(a-b) for a,b in zip(pixels(r,x,y),native(r,x,y)))<1e-8
checks=0
for m in c['maps']:
 if m['inside']:continue # exact area test below
 a=m['active'];x,y=unproject(m['region'],a[0]+a[2]/2,a[1]+a[3]/2)
 p=l.table_from(dict(region=m['region'],x=x,y=y,z=100,yaw=0))
 v=view(p);assert v['kind']=='local'
 # Overlapping regions legitimately choose higher priority native map.
 assert v['m']['priority']>=m['priority'];checks+=1
 for k in range(4):assert v['m']['rect'][k+1]==next(vv for vv in c['maps'] if vv['id']==v['m']['id'])['rect'][k]
for m in c['maps']:
 if not m['inside']:continue
 for area in c['inside'][m['region']][m['location']]:
  xy=area['xy'];x=sum(xy[::2])/(len(xy)/2);y=sum(xy[1::2])/(len(xy)/2)
  # Only test an interior polygon centroid that lies within its map active shape.
  p=l.table_from(dict(region=m['region'],x=x,y=y,z=area['z']+min(1,area['height']/2),yaw=0))
  v=view(p);assert v and v['kind']=='local'
  p['z']=area['z']+area['height']+100
  assert not (view(p)['m'] and view(p)['m']['id']==m['id'])
# Render every map's native active center plus regional fallbacks through actual AVM1.
for i,m in enumerate(c['maps']):
 a=m['active'];x,y=unproject(m['region'],a[0]+a[2]/2,a[1]+a[3]/2)
 l.execute("now=now+1;level="+json.dumps('levels/'+m['region'])+f";pos.x={x};pos.y={y};pos.z=100")
 l.globals().kcd2vr_map_t2_feed(f'T2|{i+1}|3|{x+4000},{y+4000}|1,{x+2000},{y+2000},70,abc;')
 assert l.globals().KCD2VRMap.navigation.open, (i,m['name'],dict(l.globals().KCD2VRMap.navigation))
 assert 0<l.globals().KCD2VRMap.navigation.tileCount39<=20
 assert vm.env['map']['m0']['_visible']
 if l.globals().KCD2VRMap.navigation.viewKind39=='local':
  v=view(l.table_from(dict(region=m['region'],x=x,y=y,z=100,yaw=0)))
  tile=vm.env['map']['t0'];rect=v['m']['rect']
  assert abs(tile['_xscale']-rect[3]*v['scale']/2048*100)<1e-7
  assert abs(tile['_yscale']-rect[4]*v['scale']/2048*100)<1e-7
for r in ('trosecko','kutnohorsko'):
 p=l.table_from(dict(region=r,x=10,y=10,z=100,yaw=0));v=view(p)
 assert v['kind']=='region'
 assert abs(v['scale']-3*min(896/(c['levels'][r]['crop'][2]*2048),448/(c['levels'][r]['crop'][3]*2048)))<1e-10
l.execute('''
now=100;level='levels/trosecko';pos.x=1822;pos.y=2066
kcd2vr_map_t2_feed('T2|30|3|3000,3000|1,1900,2100,70,abc;')
assert(#KCD2VRMap.navigation.markers==1 and KCD2VRMap.navigation.waypoint)
now=.1;KCD2VRMap.NavigationTick()
assert(#KCD2VRMap.navigation.markers==0 and not KCD2VRMap.navigation.waypoint and KCD2VRMap.navigation.open)
paperHidden=true;KCD2VRMap.NavigationTick();assert(not KCD2VRMap.navigation.open)
''')
assert all(not vm.env['map']['t'+str(i)]['_visible'] for i in range(20))
assert (created,draws)==(vm.created,vm.draws)
print(json.dumps({'production_Lua51_and_generated_AVM1':'PASS','maps':len(c['maps']),'active_center_priority_checks':checks,'native_projection_regions':len(c['levels']),'persistent_movie_rebuilds':0,'native_Scaleform_and_headset':'NOT_RUN'},indent=2))
