"""Bake native local-map definitions, all native POI positions and local art.
Source overlay only; never installs or opens game menus. Current PAK is merged
only by the coordinator, preserving unrelated members verbatim.
"""
from pathlib import Path
import argparse,zipfile,xml.etree.ElementTree as E,hashlib,json,math,io,struct
from PIL import Image
import build_map_navigation_t2 as renderer
import build_native_held_map32 as native
ROOT=Path(__file__).resolve().parents[1]
DATA=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Data')
def h(b):return hashlib.sha256(b).hexdigest()
def lua(x):
 if x is None:return 'nil'
 if isinstance(x,bool):return 'true' if x else 'false'
 if isinstance(x,(int,float)):return format(x,'.9g')
 if isinstance(x,str):return json.dumps(x,ensure_ascii=True)
 if isinstance(x,list):return '{'+','.join(lua(v) for v in x)+'}'
 return '{'+','.join('['+lua(k)+']='+lua(v) for k,v in x.items())+'}'
class Inputs:
 def __init__(self,data):self.data=data;self.z={};self.hashes={}
 def read(self,pak,n):
  if pak not in self.z:self.z[pak]=zipfile.ZipFile(self.data/pak)
  b=self.z[pak].read(n);self.hashes[pak+':'+n]=h(b);return b
def parse(b):return E.fromstring(b)
def pair(s):return [float(x) for x in s.split(',')]
def collect(r):
 levels={}
 for e in parse(r.read('Tables.pak','Libs/Tables/level.xml')).iter('LevelData'):
  if e.get('LevelName') not in ('trosecko','kutnohorsko','klaster'):continue
  a=e.attrib
  levels[a['LevelName']]={'id':int(a['LevelId']),'rotation':float(a['CompassOffset']),'world':pair(a['MapWorldOrig'])+pair(a['MapWorldSize']),
   'level':pair(a['MapLevelSize']),'tiles':pair(a['MapTilesSize']),'crop':pair(a['MapTilesCropOrig'])+pair(a['MapTilesCropSize'])}
 regions={v['id']:k for k,v in levels.items()}
 locations={e.get('location_id'):regions.get(int(e.get('level_id','-1'))) for e in parse(r.read('Tables.pak','Libs/Tables/rpg/location.xml')).iter('location')}
 types={e.get('poi_type_id'):int(e.get('mark_type','-1')) for e in parse(r.read('Tables.pak','Libs/Tables/rpg/poi_type.xml')).iter('poi_type')}
 maps=[]
 for e in parse(r.read('Tables.pak','Libs/Tables/ui/ui_local_maps.xml')).iter('ui_local_maps'):
  a=e.attrib;region=locations.get(a['location_id'])
  if region is None:raise ValueError('unknown local region '+a['ui_local_map_name'])
  maps.append({'id':a['ui_local_map_id'],'location':a['location_id'],'name':a['ui_local_map_name'],'region':region,
   'rect':[float(a[x]) for x in ('position_x','position_y','width','height')],
   'active':[float(a[x]) for x in ('active_x','active_y','active_width','active_height')],
   'ellipse':a['active_area_ellipse']=='true','inside':a['enabled_only_when_inside']=='true','default':a['enabled_by_default']=='true',
   'priority':int(a.get('priority','0')),'tiles':[int(a['tile_width']),int(a['tile_height'])]})
 pois={};polygons={}
 for level in levels:
  pak='Levels/'+level+'/level.pak';w=parse(r.read(pak,'whdata_0'));rows=[]
  for e in w.findall('./RPGLocationManager/MarkIdMap/Mark'):
   p=e.findtext('./Shape/Position');kind=int(e.findtext('MarkType','-1'))
   if kind<0:kind=types.get(e.findtext('POITypeId'),-1)
   # Native ordinary POIs only. Dynamic quest/objective markers come from
   # the authenticated live feed, not static quest-spoiler guesses.
   if p and kind>=4:
    pos=pair(p)
    if len(pos)==3 and all(math.isfinite(x) for x in pos):rows.append(pos+[kind,e.findtext('Id','')])
  pois[level]=rows;polygons[level]={}
  inside_ids={m['location'] for m in maps if m['region']==level and m['inside']}
  if inside_ids:
   objects=parse(r.read(pak,'objects_mission0.xml'))
   for e in objects:
    props=e.find('Properties');area=e.find('Area')
    if props is None or area is None or props.get('guidLocationId') not in inside_ids:continue
    origin=pair(e.get('Pos','0,0,0'));q=pair(e.get('Rotate','1,0,0,0'));scale=pair(e.get('Scale','1,1,1'))
    if len(scale)==1:scale*=3
    # Exact exported quaternion is wxyz; transform local area points to world.
    wq,xq,yq,zq=q
    poly=[]
    for pt in area.findall('./Points/Point'):
     v=[a*b for a,b in zip(pair(pt.get('Pos')),scale)];vx,vy,vz=v
     tx=2*(yq*vz-zq*vy);ty=2*(zq*vx-xq*vz);tz=2*(xq*vy-yq*vx)
     poly.extend([origin[0]+vx+wq*tx+yq*tz-zq*ty,origin[1]+vy+wq*ty+zq*tx-xq*tz])
    if len(poly)>=6:
     polygons[level].setdefault(props.get('guidLocationId'),[]).append({'xy':poly,'z':origin[2],'height':float(area.get('Height','0'))})
  for m in maps:
   if m['region']==level and m['inside'] and not polygons[level].get(m['location']):raise ValueError('missing native inside shape')
 return {'levels':levels,'maps':maps,'pois':pois,'inside':polygons}
def streamed(r,name):
 b=r.read('IPL_GameData.pak',name)
 codec=b[84:88];factor=.5 if codec==b'DXT1' else 1 if codec==b'DXT5' else 0
 if not factor:raise ValueError('unexpected codec '+name)
 height,width=struct.unpack_from('<2I',b,12)
 if width!=height or width not in (512,2048) or len(b)!=128+int(1360*factor):raise ValueError('DDS base '+name)
 if struct.unpack_from('<I',b,28)[0]!=int(math.log2(width))-1:raise ValueError('DDS mips')
 parts=[]
 for i in range(int(math.log2(width))-5,0,-1):
  q=r.read('IPL_GameData.pak',name+'.'+str(i));side=32<<i
  if len(q)!=int(side*side*factor):raise ValueError('DDS stream '+name)
  parts.append(q)
 return b[:128]+b''.join(parts)+b[128:]
def global_art(r,level,definition):
 if not hasattr(r,'global_cache'):r.global_cache={}
 if level in r.global_cache:return r.global_cache[level]
 cols,rows=[int(x) for x in definition['crop'][2:]]
 prefix='Libs/UI/Textures/Maps/global_map_'+level+'_'
 overview=Image.open(io.BytesIO(streamed(r,prefix+'0.dds'))).convert('RGB')
 image=overview.resize((cols*2048,rows*2048),Image.Resampling.LANCZOS)
 # These are the native cropped row-major detail IDs (not the uncropped grid).
 for n in range(1,cols*rows+1):
  path=prefix+str(n)+'.dds'
  if path not in r.z['IPL_GameData.pak'].namelist():continue
  tile=Image.open(io.BytesIO(streamed(r,path))).convert('RGB')
  image.paste(tile,(((n-1)%cols)*2048,((n-1)//cols)*2048))
 r.global_cache[level]=image
 return image
def local_image(r,m,levels):
 cols,rows=m['tiles'];size=2048
 global_map=global_art(r,m['region'],levels[m['region']])
 x,y,w,h=m['rect']
 art=global_map.crop((round(x),round(y),round(x+w),round(y+h))).resize((cols*size,rows*size),Image.Resampling.LANCZOS)
 for n in range(1,cols*rows+1):
  b=streamed(r,'Libs/UI/Textures/Maps/'+m['name']+'_'+str(n)+'.dds')
  tile=Image.open(io.BytesIO(b)).convert('RGBA')
  art.paste(tile,(((n-1)%cols)*size,((n-1)//cols)*size),tile)
 # Native LocalMap.OnStreamed independently scales X/Y to the table rectangle.
 # Store a uniform2048square; TileRect supplies those SAME independent scales.
 art=art.resize((2048,2048),Image.Resampling.LANCZOS)
 return renderer.legacy_pigment(art)
def build(data,out):
 r=Inputs(data);catalog=collect(r);entries={}
 for m in catalog['maps']:
  name='Libs/UI/Textures/KCD2VRMapT2/local_'+m['name']+'.dds'
  if name not in entries:entries[name]=local_image(r,m,catalog["levels"])
 entries['Scripts/KCD2VR/map_catalog_m39.lua']=('KCD2VRMapCatalogM39='+lua(catalog)+'\n').encode()
 entries['Libs/UI/KCD2VRMapT2.gfx']=renderer.gfx(tile_count=20,wide=True,marker_count=257,receipt_d56=True,image_receipts_d57=True)
 entries['Libs/UI/UIElements/KCD2VRMapT2.xml']=renderer.ui_xml(receipt_d56=True,image_receipts_d57=True)
 for source,target in [('mods/KCD2VRMap/map_runtime.lua','Scripts/Startup/zzkcd2vr_map.lua'),('mods/KCD2VRMap/map_navigation_t2.lua','Scripts/KCD2VR/map_navigation_t2.lua'),('mods/KCD2VRMap/map_presentation_m31.lua','Scripts/KCD2VR/map_presentation_m31.lua')]:
  entries[target]=(ROOT/source).read_bytes()
 out.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(out/'MAP_M39_RESOURCE_OVERLAY.zip','w',compression=zipfile.ZIP_DEFLATED) as z:
  for n,b in entries.items():z.writestr(n,b)
 report={'maps':len(catalog['maps']),'artworks':len({m['name'] for m in catalog['maps']}),'pois':{k:len(v) for k,v in catalog['pois'].items()},
 'inputs':r.hashes,'outputs':{n:h(b) for n,b in entries.items()},'base':'453f7e5d5a3cc62a9c7904a5a1be0b5aaf05a6ac','installed':False}
 (out/'RESOURCE_MANIFEST.json').write_text(json.dumps(report,indent=2))
 (out/'CATALOG.json').write_text(json.dumps(catalog,indent=2))
 return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--data',type=Path,default=DATA);p.add_argument('--out',type=Path,required=True);a=p.parse_args();print(json.dumps(build(a.data,a.out),indent=2))
