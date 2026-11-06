"""Offline, exact-retail icon conversion. No runtime texture rebakes.

Static POIs keep one mesh/material per sheet. Dynamic current objectives use
bounded helper slots with retail native icon DDS paths. Source builds use the
checked-in output; regeneration needs the exact input paths supplied explicitly.
"""
from pathlib import Path
import argparse, hashlib, io, json, math, struct, sys, zipfile
from PIL import Image
from build_map_geometry_d64 import atomic,chunks,pack,bounds,rename,plain
import xml.etree.ElementTree as E
ROOT=Path(__file__).resolve().parents[1]
def material(texture):
 m=E.Element('Material',MtlFlags='524544',vertModifType='0');ss=E.SubElement(m,'SubMaterials')
 E.SubElement(ss,'Material',Name='proxy',Shader='NoDraw',MtlFlags='525440')
 s=E.SubElement(ss,'Material',Name='native_map_ink',Shader='Illum',MtlFlags='524416',Diffuse='1,1,1',
  Specular='0,0,0',Opacity='1',AlphaTest='0.2',Shininess='1',GenMask='0',StringGenMask='')
 t=E.SubElement(s,'Textures');E.SubElement(t,'Texture',File=texture,Map='Diffuse');E.SubElement(s,'PublicParams')
 return E.tostring(m,encoding='utf-8',xml_declaration=True)
def mesh(template,main,quads,path):
 cs=chunks(template);vs=[];uv=[];ix=[]
 for x,y,z,r,u0,v0,u1,v1 in quads:
  n=len(vs);vs.extend([(x-r,y,z+r),(x+r,y,z+r),(x+r,y,z-r),(x-r,y,z-r)])
  uv.extend([(u0,v0),(u1,v0),(u1,v1),(u0,v1)])
  ix.extend((n,n+1,n+2,n,n+2,n+3,n+2,n+1,n,n+3,n+2,n))
 assert vs and len(vs)<65536
 for typ,ver,cid,b in cs:
  if typ!=0x1016:continue
  st=struct.unpack_from('<I',b,4)[0];h=bytearray(b[:24])
  if st==15:
   struct.pack_into('<I',h,8,len(vs));b[:]=h+b''.join(struct.pack('<4e4B2e',*p,1,128,128,0,127,*t)for p,t in zip(vs,uv))
  elif st==5:
   struct.pack_into('<I',h,8,len(ix));b[:]=h+struct.pack('<'+'H'*len(ix),*ix)
  elif st==6:
   tangent=bytes(b[24:40]);struct.pack_into('<I',h,8,len(vs));b[:]=h+tangent*len(vs)
 points=[struct.unpack('<3e',struct.pack('<3e',*p))for p in vs]
 cm=chunks(main)
 for c in(cs,cm):
  bounds(c,points,len(points),len(ix))
  for row in c:
   if row[0]==0x1014:
    assert len(path.encode())<128;row[3][:128]=path.encode().ljust(128,b'\0')
 return pack(cm),pack(cs)
def main():
 a=argparse.ArgumentParser();a.add_argument('--retail-data',type=Path,required=True);a.add_argument('--types',type=Path,required=True)
 a.add_argument('--baseline',type=Path,required=True);a.add_argument('--lua-path');args=a.parse_args()
 if args.lua_path:sys.path.insert(0,args.lua_path)
 from lupa.lua51 import LuaRuntime
 lua=LuaRuntime();lua.execute((ROOT/'mods/KCD2VRMap/map_catalog_static_m41.lua').read_text());cat=plain(lua.globals().KCD2VRMapCatalogM39)
 types={x['type']:x['label'] for x in json.loads(args.types.read_text())}
 native=zipfile.ZipFile(args.retail_data/'IPL_GameData.pak');names={n.casefold():n for n in native.namelist()}
 hashes={};icons={}
 for k in sorted({int(p[3])for pp in cat['pois'].values()for p in pp}):
  key='Libs/UI/Textures/Icons/Map/'+types[k]+'_icon.dds';actual=names.get(key.casefold())
  if not actual:raise ValueError('Missing native POI art '+key)
  b=native.read(actual);hashes[actual]=hashlib.sha256(b).hexdigest();im=Image.open(io.BytesIO(b)).convert('RGBA')
  assert im.size==(64,64);icons[k]=im
 atlas=Image.new('RGBA',(1024,1024));uv={}
 for i,(kind,im) in enumerate(icons.items()):
  x,y=(i%12)*80+8,(i//12)*80+8;assert y+64<=1024;atlas.paste(im,(x,y))
  uv[kind]=(x/1024,y/1024,(x+64)/1024,(y+64)/1024)
 b=io.BytesIO();atlas.save(b,format='DDS');atomic(ROOT/'companion/source/Objects/kcd2vr/mapmarkers64/native_icons75.dds',b.getvalue())
 base=args.baseline/'companion/source/Objects/kcd2vr/mapmarkers64'
 target=ROOT/'companion/source/Objects/kcd2vr/mapmarkers64'
 from build_map_geometry_d64 import pixels
 sheets={m['name']:m for m in cat['maps']}
 for region in ('trosecko','kutnohorsko'):
  l=cat['levels'][region];sheets[region]={'name':region,'region':region,'rect':[0,0,l['crop'][2]*2048,l['crop'][3]*2048]}
 for name,m in sorted(sheets.items()):
  cs=chunks((base/(name+'.cgfm')).read_bytes());old=[]
  for typ,ver,cid,data in cs:
   if typ==0x1016 and struct.unpack_from('<I',data,4)[0]==15:
    n=struct.unpack_from('<I',data,8)[0];old=[struct.unpack_from('<3e',data,24+i*16) for i in range(n)]
  kinds=[];r=m['rect']
  for p in cat['pois'][m['region']]:
   x,y=pixels(cat['levels'][m['region']],p[0],p[1]);u,v=(x-r[0])/r[2],(y-r[1])/r[3]
   if 0<=u<=1 and 0<=v<=1:kinds.append(int(p[3]))
  assert len(old)==4*len(kinds),(name,len(old),len(kinds))
  quads=[]
  for i,k in enumerate(kinds):
   p=old[i*4:i*4+4];x,y,z=[sum(q[a]for q in p)/4 for a in range(3)]
   quads.append((x,y,z,.0048,*uv[k]))
  gc,gm=mesh((base/(name+'.cgfm')).read_bytes(),(base/(name+'.cgf')).read_bytes(),quads,'Objects/kcd2vr/mapmarkers64/'+name)
  atomic(target/(name+'.cgf'),gc);atomic(target/(name+'.cgfm'),gm);atomic(target/(name+'.mtl'),material('Objects/kcd2vr/mapmarkers64/native_icons75.dds'))
 # Dynamic quest art uses native DDS resources directly; no duplicate textures.
 quest=['checkpoint']
 for fam,colors in [('main','none red yellow blue'),('side','none red yellow blue'),('activity','none red yellow blue orange pink violet'),('racing','none red')]:
  quest +=['quest_'+fam+'_'+c for c in colors.split()]
 for q in quest:
  texture='Libs/UI/Textures/Icons/Map/'+q+'_icon.dds';actual=names.get(texture.casefold());assert actual,texture
  hashes[actual]=hashlib.sha256(native.read(actual)).hexdigest()
  path='Objects/kcd2vr/mapquests75/'+q
  gc,gm=mesh((base/'player.cgfm').read_bytes(),(base/'player.cgf').read_bytes(),[(0,-.019,0,.008,0,0,1,1)],path)
  t=ROOT/'companion/source'/path
  atomic(t.with_suffix('.cgf'),gc);atomic(t.with_suffix('.cgfm'),gm);atomic(t.with_suffix('.mtl'),material(actual))
 report={'type_mapping_sha256':hashlib.sha256(args.types.read_bytes()).hexdigest(),'native_icon_hashes':hashes,
  'static_poi_icon_types':len(icons),'sheets':len(sheets),'quest_models':len(quest),'runtime_texture_rebake':False,
  'static_poi_discovery':'CATALOG_AS_IN_D74_NOT_CURRENT_DISCOVERY','quest_source':'NATIVE_CURRENT_COMPASS_ONLY','headset_verified':False}
 atomic(ROOT/'repair_evidence/d75/map/ASSET_INPUTS.json',(json.dumps(report,indent=2)+'\n').encode())
 print('D75 static single-atlas POIs and',len(quest),'native quest icon models generated.')
if __name__=='__main__':main()
