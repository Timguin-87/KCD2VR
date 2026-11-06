#!/usr/bin/env python3
"""Run current navigation Lua5.1 against the packaged production AVM1 movie.

Compare every visible tile/glyph property with the immutable D55 Lua fixture.
Native UIAction, image decode, actor and clock are explicit host leaves. Counts
are actual bridge calls in this fixture, not engine duration/headset evidence.
"""
import argparse, collections, hashlib, json, math, sys, zipfile
import xml.etree.ElementTree as ET
from pathlib import Path
from lupa.lua51 import LuaRuntime
from test_map_renderer_t2 import VM
ROOT=Path(__file__).resolve().parents[1]
DRAW_NAMES={'TileRect':'fc_tile_rect','Tile':'fc_tile','Marker':'fc_marker','Clear':'fc_clear'}
NAMES={**DRAW_NAMES,'ReceiptD56':'fc_receipt_d56'}

class Scene:
 def __init__(self,source,gfx,catalog,assets,xml):
  self.vm=VM(gfx);assert set(NAMES.values()).issubset(self.vm.functions)
  self.functions={f.get('name'):f.get('funcname') for f in ET.fromstring(xml).findall('.//functions/function')}
  self.variables={v.get('name'):v.get('varname') for v in ET.fromstring(xml).findall('.//variables/variable')}
  assert self.functions==NAMES and self.variables.get('ReceiptD56')=='d56Receipt'
  assert set(self.vm.functions)-set(NAMES.values())<= {'fc_load_init_d57','fc_load_error_d57'}
  self.calls=[];self.fail=None;self.get_mode=None;self.movie_missing=False;self.dispatch_missing=False
  self.assets=assets;self.lua=LuaRuntime(unpack_returned_tuples=True)
  self.lua.globals().ui_call=self.ui
  self.lua.globals().ui_get=self.get_variable
  self.lua.execute('''
now=10;level='levels/kutnohorsko';pos={x=2048,y=2048,z=100};yaw=0;paperHidden=false
KCD2VRMap={world=42,awaitStart=false,entities={paper={entity={IsHidden=function()return paperHidden end}}}}
System={GetCurrTime=function()if clockMissing then error('missing clock')end;return now end,
 GetCVar=function(k)return k=='sv_map' and level or 0 end,LogAlways=function()end}
UIAction={CallFunction=function(...)return ui_call(...)end,GetVariable=function(...)return ui_get(...)end}
g_localActor={id=42,inventory={},GetWorldPos=function()return pos end,GetWorldAngles=function()return {z=yaw}end}
''')
  self.lua.execute(catalog);self.lua.execute(source)
 def ui(self,element,instance,name,*args):
  assert element=='KCD2VRMapT2' and instance==0
  self.calls.append((name,args))
  if self.movie_missing or self.dispatch_missing:return None
  if self.fail and name==self.fail['name']:
   self.fail['left']-=1
   if self.fail['left']==0:
    mode=self.fail['mode'];self.fail=None
    if mode=='raise':raise RuntimeError('synthetic UIAction failure')
    return None if mode=='nil' else False
  if name=='TileRect' and args[-1]:assert args[1].lower() in self.assets,('missing packaged image',args[1])
  self.vm.call(self.functions[name],*args)
  return None # successful void dispatch; current movie readback separately confirms reuse
 def get_variable(self,element,instance,name):
  assert element=='KCD2VRMapT2' and instance==0 and name=='ReceiptD56'
  self.calls.append(('GetVariable',(name,)))
  if self.movie_missing or self.get_mode=='nil':return None
  if self.get_mode=='false':return False
  if self.get_mode=='raise':raise RuntimeError('synthetic readback failure')
  value=self.vm.env[self.variables[name]]
  return str(value) if self.get_mode=='string' else value
 def execute(self,s):self.lua.execute(s)
 def feed(self,packet,advance=.6):
  self.calls=[];self.execute('now=now+'+str(advance));self.lua.globals().kcd2vr_map_t2_feed(packet)
  return dict(collections.Counter(n for n,_ in self.calls))
 def tick(self,advance=.6):
  self.calls=[];self.execute('now=now+'+str(advance));self.lua.globals().KCD2VRMap.NavigationTick()
  return dict(collections.Counter(n for n,_ in self.calls))
 def snapshot(self):
  out={'tiles':{},'markers':{}}
  for i in range(20):
   t=self.vm.env['map']['t'+str(i)]
   if t.get('_visible'):out['tiles'][i]={k:t.get(k) for k in ('_x','_y','_xscale','_yscale','source')}
  for i in range(257):
   m=self.vm.env['map']['m'+str(i)]
   if m.get('_visible'):out['markers'][i]={**{k:m.get(k) for k in ('_x','_y','_rotation')},
     'dot':m['dot'].get('_visible'),'dir':m['dir'].get('_visible'),'color':m['dot']['ink'].get('_rgb')}
  return out
 @property
 def state(self):return self.lua.globals().KCD2VRMap.navigation

def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--pak',required=True,type=Path);ap.add_argument('--out',required=True,type=Path)
 ap.add_argument('--source-ui',action='store_true',help='Before packaging, use the two generated source UI assets with the supplied catalog/image inventory.')
 args=ap.parse_args()
 args.out.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(args.pak) as z:
  gfx=z.read('Libs/UI/KCD2VRMapT2.gfx');xml=z.read('Libs/UI/UIElements/KCD2VRMapT2.xml')
  packaged_source=z.read('Scripts/KCD2VR/map_navigation_t2.lua');catalog=z.read('Scripts/KCD2VR/map_catalog_m39.lua').decode()
  assets={n.lower() for n in z.namelist()}
 source_gfx=(ROOT/'mods/KCD2VRMap/KCD2VRMapT2.gfx').read_bytes();source_xml=(ROOT/'mods/KCD2VRMap/KCD2VRMapT2.xml').read_bytes()
 if args.source_ui:gfx,xml=source_gfx,source_xml
 else:
  assert gfx==source_gfx and xml==source_xml,'Packaged D56 UI assets do not match current generated source.'
  assert packaged_source==(ROOT/'mods/KCD2VRMap/map_navigation_t2.lua').read_bytes(),'Packaged navigation Lua does not match current source.'
 baseline=(ROOT/'tests/fixtures/map_navigation_d55.lua').read_bytes()
 assert hashlib.sha256(baseline).hexdigest()=='ee091252929fa04c1550bc7dc6715cd295cf417160235151e1618845046cd19e','D55 baseline fixture changed'
 original=baseline.decode()
 source=(ROOT/'mods/KCD2VRMap/map_navigation_t2.lua').read_text()
 old=Scene(original,gfx,catalog,assets,xml);new=Scene(source,gfx,catalog,assets,xml);checks=0;records=[]
 def check(ok,why):
  nonlocal checks;checks+=1
  if not ok:raise AssertionError(why)
 import build_map_navigation_t2 as renderer
 base_movie=VM(renderer.gfx(tile_count=20,wide=True,marker_count=257))
 check(all(base_movie.functions[n]==new.vm.functions[n] for n in DRAW_NAMES.values()),'four existing draw functions retain exact production bytecode and parameter ABI')
 check(new.vm.env['d56Receipt']==0,'new movie initializes an unconfirmed receipt')
 image_receipts_d57='LoadInitEventsD57' in new.variables
 check(gfx==renderer.gfx(tile_count=20,wide=True,marker_count=257,receipt_d56=True,image_receipts_d57=image_receipts_d57) and xml==renderer.ui_xml(receipt_d56=True,image_receipts_d57=image_receipts_d57),'supplied owned UI assets reproduce from the current generator')
 def both(s):old.execute(s);new.execute(s)
 def draws(counts):return {k:v for k,v in counts.items() if k in DRAW_NAMES}
 def feed(label,p,advance=.6,equal=True):
  a,b=old.feed(p,advance),new.feed(p,advance)
  if equal:check(old.snapshot()==new.snapshot(),label+' preserves every visible packaged-movie property')
  records.append({'case':label,'d55_calls':sum(a.values()),'d56_calls':sum(b.values()),'d55_by_function':a,'d56_by_function':b,
                  'd56_draw_calls':sum(draws(b).values()),'markers':new.state.drawn,'view':new.state.view39,
                  'cache_confirmed':bool(new.state.cacheConfirmedD56)})
  return draws(a),draws(b)
 # Use a native local-map active rectangle center, inverse of the production
 # transform. This only selects fixture input; render still runs actual Lua.
 c=new.lua.globals().KCD2VRMapCatalogM39
 target=next(m for _,m in c.maps.items() if m.region=='kutnohorsko' and not m.inside)
 px,py=target.active[1]+target.active[3]/2,target.active[2]+target.active[4]/2
 project=new.lua.globals().KCD2VRMap.MapPixelsM39
 q=project('kutnohorsko',0,0);a=project('kutnohorsko',1,0);b=project('kutnohorsko',0,1)
 ax,ay=a[0]-q[0],a[1]-q[1];bx,by=b[0]-q[0],b[1]-q[1];det=ax*by-ay*bx
 x=((px-q[0])*by-(py-q[1])*bx)/det;y=((py-q[1])*ax-(px-q[0])*ay)/det
 both(f'pos.x={x!r};pos.y={y!r}')
 p=f'T2|1|3||1,{x+5},{y+7},100,abc;4,{x+8},{y+5},100,def;'
 first=feed('actual_catalog_first',p)
 repeat=feed('actual_catalog_stationary',p)
 check(repeat[1]=={},'identical confirmed persistent view needs zero repeated draw writes')
 check(new.calls==[('GetVariable',('ReceiptD56',))] and new.state.cacheConfirmedD56,'stationary draw checks the actual current movie once and does not periodically repaint')
 both('yaw=.12');heading=feed('actual_catalog_heading_only',p)
 check(heading[1]=={'Marker':1},'heading-only change writes only the current player marker')
 # Keep the actual native local projection and production movie; a small
 # explicit POI set makes shrink/expansion assertions independent of saturation.
 both('KCD2VRMapCatalogM39.pois.kutnohorsko={};KCD2VRMap.NavigationRetire()')
 p0='T2|2|1||';p1=f'T2|2|3||1,{x+5},{y+7},100,abc;';p2=p1+f'1,{x+8},{y+9},100,def;'
 feed('small_first',p1);expanded=feed('expanded_markers',p2)
 check(expanded[1]=={'Marker':1},'expanded contiguous set writes only the new glyph and never clears existing glyphs')
 replacement=f'T2|2|3||1,{x+5},{y+7},100,abc;1,{x+11},{y+9},100,abc1;'
 replaced=feed('same_count_replaced_poi',replacement)
 check(replaced[1]=={'Marker':1},'same-count replacement updates exact changed slot without Clear')
 shrink=feed('removed_poi',p1)
 check(shrink[1]=={'Clear':1,'Marker':2},'shrinking set clears former tail and reissues current player plus one glyph')
 check(not new.vm.env['map']['m2']['_visible'],'removed marker cannot survive shrinking set')
 for kind in ('false','raise'):
  both('yaw=yaw+.1');new.fail={'name':'Marker','left':1,'mode':kind}
  feed('failed_marker_'+kind,p1,equal=False)
  check(new.state.surfaceRefused and not new.state.open,'failed/throwing surface is not marked successful')
  retry=feed('failed_marker_retry_'+kind,p1)
  check(retry[1].get('TileRect')==20 and retry[1].get('Clear')==1 and retry[1].get('Marker')==2,
        'failure invalidates all cached writes and retry replays complete current view')
 # Late failure after some successful writes cannot leave a partially trusted cache.
 new.fail={'name':'Marker','left':2,'mode':'false'}
 late=p2.replace('T2|2|','T2|3|');feed('partial_surface_failure',late,equal=False)
 check(not new.state.open,'late partial failure refuses the composed surface')
 retry=feed('partial_surface_retry',late)
 check(retry[1].get('Marker')==3 and retry[1].get('TileRect')==20,'late retry includes already successful earlier slots')
 both('paperHidden=true');a,b=old.tick(),new.tick()
 check(not new.snapshot()['tiles'] and not new.snapshot()['markers'] and not new.state.open,'real hidden paper clears the actual movie')
 check('GetVariable' not in b,'hidden helper cannot probe or create a movie through receipt readback')
 both('paperHidden=false');reveal=feed('real_hide_then_reveal',late)
 check(reveal[1].get('TileRect')==20 and reveal[1].get('Marker')==3,'hidden lifecycle never skips a required reappearance write')
 both('now=.1');rewind=feed('clock_rewind',late,advance=0)
 check(rewind[1].get('TileRect',0)>=20 and new.state.lastRender==.1,'time rewind invalidates cache and cannot throttle new current metadata')
 both('clockMissing=true');old.tick();new.tick()
 check(not new.state.open and not new.snapshot()['tiles'],'missing clock does not retain visible cached content')
 both('clockMissing=false');feed('clock_recovered',late)
 both('KCD2VRMap.NavigationRetire();g_localActor.id=43;KCD2VRMap.world=43')
 actor=feed('retired_actor_replaced',late)
 check(actor[1].get('Clear')==1 and actor[1].get('TileRect')==20,'actor/world retirement invalidates every slot')
 both("level='levels/trosecko';pos.x=10;pos.y=10;pos.z=100")
 transition=feed('region_transition',late)
 check(new.state.region=='trosecko' and old.snapshot()==new.snapshot(),'region transition uses current native projection and images')
 # Missing API must not masquerade as a success even with cached identical slots.
 new.execute('UIAction=nil');new.feed(late)
 check(new.state.surfaceRefused and not new.state.open,'missing private UIAction cannot use cached writes to claim surface readiness')
 new.execute('UIAction={CallFunction=function(...)return ui_call(...)end,GetVariable=function(...)return ui_get(...)end}')
 restored=feed('ui_api_recovered',late)
 check(restored[1].get('TileRect')==20 and restored[1].get('Clear')==1,'API recovery retries the complete current view')
 new.fail={'name':'TileRect','left':1,'mode':'false'}
 tile_packet=late.replace('T2|3|','T2|4|');feed('failed_tile_write',tile_packet,equal=False)
 check(new.state.surfaceRefused and not new.state.open,'failed tile write refuses the full surface')
 tile_retry=feed('failed_tile_retry',tile_packet)
 check(tile_retry[1].get('TileRect')==20,'failed tile is retried with every current slot')
 new.fail={'name':'Clear','left':1,'mode':'false'}
 feed('failed_shrink_clear','T2|4|1||',equal=False)
 check(new.state.surfaceRefused and not new.state.open,'failed Clear cannot mark a smaller marker set complete')
 clear_retry=feed('failed_shrink_clear_retry','T2|4|1||')
 check(clear_retry[1].get('Clear')==1 and list(new.snapshot()['markers'])==[0],'Clear failure retries and removes every obsolete marker')
 check(old.vm.created==new.vm.created and old.vm.draws==new.vm.draws,'no persistent clip creation or vector rebuilds added')
 # A fresh private movie may have the same actor, visible helper, activation,
 # clock and frame properties. Its reset receipt must force current repaint.
 new.vm=VM(gfx)
 recreated=feed('same_actor_movie_recreated','T2|4|1||')
 check(recreated[1].get('TileRect')==20 and recreated[1].get('Clear')==1 and new.state.cacheConfirmedD56,'actual new movie receipt invalidates identical old property cache')
 # Exercise the ambiguous nil transport case without pretending nil itself
 # is an acknowledgement: the entire unavailable dispatch does nothing.
 new.execute('KCD2VRMap.NavigationRetire()');new.vm=VM(gfx);new.movie_missing=True
 missing=feed('unrendered_nil_first_dispatch','T2|5|1||',equal=False)
 check(not new.state.cacheConfirmedD56 and not new.snapshot()['tiles'],'nil dispatch without an exact movie echo cannot confirm any cache')
 missing_again=feed('unrendered_nil_repeat','T2|5|1||',equal=False)
 check(missing_again[1].get('TileRect')==20 and missing_again[1].get('Clear')==1,'ambiguous nil dispatch is retried instead of suppressed forever')
 new.movie_missing=False
 recovered=feed('nil_dispatch_movie_recovered','T2|5|1||')
 check(recovered[1].get('TileRect')==20 and new.state.cacheConfirmedD56,'real movie recovery repaints every current slot and confirms only its own echo')
 # Reloading Lua resets its local counter while the movie can survive. A
 # discarded new commit must not accidentally match that movie's old receipt.
 new.vm.env['d56Receipt']=1;new.execute(source);new.dispatch_missing=True
 feed('reloaded_lua_silent_dispatch','T2|5|1||')
 check(not new.state.cacheConfirmedD56 and new.vm.env['d56Receipt']==1,'receipt nonce differs from the pre-draw movie value even after Lua reload')
 new.dispatch_missing=False
 reload_retry=feed('reloaded_lua_dispatch_recovered','T2|5|1||')
 check(reload_retry[1].get('TileRect')==20 and new.state.cacheConfirmedD56,'reloaded Lua retries every slot after silent dispatch recovery')
 for mode in ('nil','false','raise','string'):
  new.get_mode=mode
  fallback=feed('readback_'+mode,'T2|5|1||')
  check(not new.state.cacheConfirmedD56 and fallback[1].get('TileRect')==20,'unavailable or untyped receipt falls back to the uncached renderer: '+mode)
  new.get_mode=None
  recovery=feed('readback_recovered_'+mode,'T2|5|1||')
  check(recovery[1].get('TileRect')==20 and new.state.cacheConfirmedD56,'readback recovery verifies a full current draw: '+mode)
 new.execute('UIAction.GetVariable=nil')
 fallback=feed('readback_api_absent','T2|5|1||')
 check(not new.state.cacheConfirmedD56 and fallback[1].get('TileRect')==20,'missing GetVariable API preserves uncached rendering')
 new.execute('UIAction.GetVariable=function(...)return ui_get(...)end')
 feed('readback_api_recovered','T2|5|1||')
 for mode in ('nil','false','raise'):
  both('yaw=yaw+.1');new.fail={'name':'ReceiptD56','left':1,'mode':mode}
  feed('lost_commit_'+mode,'T2|5|1||')
  check(not new.state.cacheConfirmedD56,'lost receipt cannot acknowledge the previous nonce: '+mode)
  retry=feed('lost_commit_retry_'+mode,'T2|5|1||')
  check(retry[1].get('TileRect')==20 and new.state.cacheConfirmedD56,'lost receipt retries the complete current surface: '+mode)
 stationary=feed('confirmed_stationary_after_recovery','T2|5|1||')
 check(stationary[1]=={} and new.calls==[('GetVariable',('ReceiptD56',))],'recovered persistent movie returns to one readback and no draw writes')
 record={'passed':True,'checks':checks,'cases':records,'source_sha256':hashlib.sha256((ROOT/'mods/KCD2VRMap/map_navigation_t2.lua').read_bytes()).hexdigest(),
   'baseline_lua_sha256':hashlib.sha256(baseline).hexdigest(),
   'ui_asset_origin':'generated_source' if args.source_ui else 'supplied_package',
   'gfx_sha256':hashlib.sha256(gfx).hexdigest(),'xml_sha256':hashlib.sha256(xml).hexdigest(),'catalog_sha256':hashlib.sha256(catalog.encode()).hexdigest(),
   'limits':'Actual production Lua5.1 and supplied production AVM1; mocked UIAction/native actor/clock/image decode. Counts include real fixture readback/receipt calls, not engine timings. A dispatch receipt does not prove image decode, material/GPU readiness or headset execution.'}
 (args.out/'MAP_INCREMENTAL_D56.json').write_text(json.dumps(record,indent=2)+'\n')
 print(json.dumps(record,indent=2))
if __name__=='__main__':main()
