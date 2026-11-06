from pathlib import Path
import sys,zipfile,xml.etree.ElementTree as ET,struct,zlib,re,json,hashlib
r=Path(__file__).resolve().parents[1]
g=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2');inspiration=g/'inspirations/mods'
sys.path.insert(0,str(r/'tools'));import decode_ui90_gfx as swf
ITEM='ad7f69c0-cb67-4808-83b2-21b691bafe49'
MODEL='objects/characters/assets/kcd2vr_heldmap/kcd2vr_heldmap'
def action(op,data=b''):return bytes([op])+(struct.pack('<H',len(data))+data if op>=128 else b'')
def push(value):
 data=b'\0'+value.encode()+b'\0' if isinstance(value,str) else b'\7'+struct.pack('<i',value)
 return action(0x96,data)
def var(name):return push(name)+action(0x1C)
def assign(name,data):return push(name)+data+action(0x1D)
def call(obj,method,args):return b''.join(reversed(args))+push(len(args))+var(obj)+push(method)+action(0x52)
def paper_actions():
 body=b''.join(var('target')+push(k)+push(512)+action(0x4F) for k in ['_width','_height'])+action(0)
 code=action(0x9B,b'_kcd2vrPaperLoaded\0'+struct.pack('<H',1)+b'target\0'+struct.pack('<H',len(body))+body)
 code+=assign('_kcd2vrPaper',var('_root')+push('_kcd2vrPaper')+action(0x4E))
 code+=assign('_kcd2vrPaperListener',push(0)+push('Object')+action(0x40))
 code+=var('_kcd2vrPaperListener')+push('onLoadInit')+var('_kcd2vrPaperLoaded')+action(0x4F)
 code+=assign('_kcd2vrPaperLoader',push(0)+push('MovieClipLoader')+action(0x40))
 code+=call('_kcd2vrPaperLoader','addListener',[var('_kcd2vrPaperListener')])+action(0x17)
 code+=call('_kcd2vrPaperLoader','loadClip',[push('libs/ui/textures/books/maps/kcd2vr_heldmap_paper_ui.dds'),var('_kcd2vrPaper')])+action(0x17)
 return code+action(0)
def tag(kind,data):return struct.pack('<H',(kind<<6)|63)+struct.pack('<I',len(data))+data
def patch_gfx(raw):
 d=zlib.decompress(raw[8:]) if raw[:1]==b'C' else raw[8:]
 bits=swf.Bits(d);n=bits.u(5);[bits.s(n) for _ in range(4)];rectend=bits.pos();start=rectend+4
 fields=[(15,5),(0,15),(10240,15),(0,15),(10240,15)]
 s=''.join(format(v, '0'+str(size)+'b') for v,size in fields);s+='0'*((-len(s))%8)
 rect=int(s,2).to_bytes(len(s)//8,'big');new=rect+d[rectend:start];added=False
 original=list(swf.tags(d,start))
 rootdepths={p['depth'] for kind,v in original for p in [swf.place(kind,v)] if p}
 depth=next(i for i in range(1,16384) if i not in rootdepths)
 assert all(not (kind==39 and struct.unpack_from('<H',v)[0]==65530) for kind,v in original)
 # A timeline clip is behind the minimap's dynamically created clips; no
 # unsupported negative createEmptyMovieClip depth or replacement of its map root.
 for kind,payload in original:
  if kind==1 and not added:
   new+=tag(39,struct.pack('<HH',65530,1)+tag(1,b'')+tag(0,b''))
   new+=tag(26,b'\x22'+struct.pack('<HH',depth,65530)+b'_kcd2vrPaper\0')
   new+=tag(12,paper_actions());added=True
  new+=tag(kind,payload)
 assert added
 return b'CFX'+raw[3:4]+struct.pack('<I',len(new)+8)+zlib.compress(new,9)
def build():
 entries={};origin={}
 def put(path,data,source):entries[path]=data;origin[path]={'source':source,'sha256':hashlib.sha256(data).hexdigest()}
 with zipfile.ZipFile(inspiration/'treasure_maps_of_bohemia.pak') as z:
  for ext in ['chr','chrparams']:
   name=f'Objects/characters/assets/parchment_folded/parchment_folded__treasure_maps_of_bohemia.{ext}'
   put(MODEL+'.'+ext,z.read(name),'treasure_maps_of_bohemia.pak:'+name)
  name='Objects/characters/assets/parchment_folded/parchment_folded__treasureHunter_map1.mtl'
  mat=ET.fromstring(z.read(name));materials=mat.find('SubMaterials')
  materials[1].find("Textures/Texture[@Map='Diffuse']").set('File','KCD2VRHeldMap@0.ui')
  materials[1].find("Textures/Texture[@Map='Diffuse']").set('TexType','5')
  for m in materials[1:]:
   for t in m.find('Textures'):
    path=t.get('File','')
    if path.lower().endswith('_ddna.dds'):t.set('File','libs/ui/textures/books/maps/kcd2vr_heldmap_paper_ddna.dds')
    elif m is materials[2] and t.get('Map')=='Diffuse':t.set('File','libs/ui/textures/books/maps/kcd2vr_heldmap_paper_diff.dds')
  put(MODEL+'.mtl',ET.tostring(mat,encoding='us-ascii',xml_declaration=True),'clone treasureHunter_map1 material; unique UI surface')
  for old,new in [('treasureHunter_map1_back_diff.dds','kcd2vr_heldmap_paper_diff.dds'),('treasureHunter_map1_ddna.dds','kcd2vr_heldmap_paper_ddna.dds')]:
   name='Libs/UI/Textures/Books/Maps/'+old;put('libs/ui/textures/books/maps/'+new,z.read(name),'treasure_maps_of_bohemia.pak:'+name)
  paper=bytearray(z.read('Libs/UI/Textures/Books/Maps/treasureHunter_map1_back_diff.dds'))
  assert paper[84:88]==b'DX10' and struct.unpack_from('<I',paper,128)[0] in {77,78}
  paper[84:88]=b'DXT5';paper=paper[:128]+paper[148:]
  put('libs/ui/textures/books/maps/kcd2vr_heldmap_paper_ui.dds',bytes(paper),'same BC3 paper blocks, legacy DDS header for Flash image decoder; physical back/normal unchanged')
 cdf=f'<CharacterDefinition><Model File="{MODEL}.chr" Material="{MODEL}.mtl"/><AttachmentList/></CharacterDefinition>'
 put(MODEL+'.cdf',cdf.encode(),'unique CDF; original animated parchment mesh')
 with zipfile.ZipFile(inspiration/'minimap.pak') as z:
  # Reuse the matched stitched-art/grid/projection assets. Never mix native unpadded local crops with these tiles.
  for name in z.namelist():
   if name.startswith('Libs/UI/Textures/Maps/') and name.lower().endswith('.dds'):put(name,z.read(name),'minimap.pak:'+name)
  put('Libs/UI/KCD2VRHeldMap.gfx',patch_gfx(z.read('Libs/UI/minimap.gfx')),'minimap.gfx + own paper background + square stage; no global native UI override')
  xml=ET.fromstring(z.read('Libs/UI/UIElements/Minimap.xml'));el=xml.find('UIElement');el.set('name','KCD2VRHeldMap')
  gfx=el.find('GFx');gfx.set('file','KCD2VRHeldMap.gfx');align=gfx.find('Constraints/Align');align.attrib={'mode':'fixdyntexsize','valign':'center','halign':'center'}
  ET.SubElement(gfx.find('Constraints'),'Position',{'width':'512','height':'512'})
  put('Libs/UI/UIElements/KCD2VRHeldMap.xml',ET.tostring(xml,encoding='utf-8',xml_declaration=True),'own material-bound UI instance; original minimap function schema')
  # Constants use distinct globals; this addon never starts the original screen minimap.
  for file in ['minimap_data.lua','minimap_loc.lua','minimap_localmaps.lua','minimap_mapgrid.lua','minimap_markicons.lua','minimap_poidata.lua']:
   data=z.read('Scripts/Startup/'+file).decode();data=data.replace('Minimap','HeldMapData')
   put('Scripts/KCD2VRHeldMap/'+file,data.encode(),'minimap.pak:'+file)
  lua=z.read('Scripts/Startup/minimap.lua').decode().replace('\r\n','\n');assert '-- boot\n' in lua;lua=lua.split('-- boot\n-- ---------------------------------------------------------------------------')[0]
  lua=lua.replace('MinimapMapGrid','HeldMapDataMapGrid').replace('MinimapLocalMaps','HeldMapDataLocalMaps').replace('MinimapPOIData','HeldMapDataPOIData').replace('MinimapMarkIcons','HeldMapDataMarkIcons').replace('MinimapData','HeldMapDataData').replace('MinimapLoc','HeldMapDataLoc')
  # Rename only the module global, not exported Flash function names (MinimapConfig, etc.).
  lua=re.sub(r'\bMinimap\b','KCD2VRHeldMap',lua)
  lua=lua.replace('local maxN = 160','local maxN = 96')
  lua=re.sub(r'M\.ELEMENT\s*=\s*"minimap"','M.ELEMENT = "KCD2VRHeldMap"',lua)
  assert 'M.ELEMENT = "KCD2VRHeldMap"' in lua and not re.search(r'^M.RegisterCommands\(\)',lua,re.M)
  for name in re.findall(r'^function (minimap_\w+)\(',lua,re.M):lua=lua.replace('function '+name+'(','function kcd2vr_held_'+name+'(')
  prefix='\n'.join('Script.ReloadScript("Scripts/KCD2VRHeldMap/'+f+'")' for f in ['minimap_data.lua','minimap_loc.lua','minimap_localmaps.lua','minimap_mapgrid.lua','minimap_markicons.lua','minimap_poidata.lua'])+'\n'
  addon=(r/'mods/KCD2VRHeldMap/held_map_runtime.lua').read_text().replace('@ITEM_GUID@',ITEM).replace('@MODEL@',MODEL)
  put('Scripts/Startup/zzKCD2VRHeldMap.lua',(prefix+lua+'\n'+addon).encode(),'namespaced reference renderer/data feeds + bounded own-map controller')
 ET.register_namespace('xsi','http://www.w3.org/2001/XMLSchema-instance')
 db=ET.Element('database',{'name':'barbora','{http://www.w3.org/2001/XMLSchema-instance}noNamespaceSchemaLocation':'item.xsd'});items=ET.SubElement(db,'ItemClasses',{'version':'8'})
 ET.SubElement(items,'Document',{'Type':'4','IconId':'map_book','UIInfo':'ui_in_kcd2vr_heldmap','UIName':'ui_nm_kcd2vr_heldmap','Model':MODEL.removeprefix('objects/')+'.cdf','EntityScript':'Book','IsQuestItem':'true','Weight':'0.1','Price':'0','FadeCoef':'1','VisibilityCoef':'1','Id':ITEM,'Name':'KCD2VR_Map'})
 put('Libs/Tables/item/item__kcd2vr_heldmap.xml',ET.tostring(db,encoding='utf-8',xml_declaration=True),'new additive document row; does not replace treasure maps')
 out=r/'companion/Mods/KCD2VRHeldMap/Data/KCD2VRHeldMap.pak';out.parent.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(out,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
  for name,data in entries.items():z.writestr(name,data)
 (out.parent.parent/'mod.manifest').write_text('<kcd_mod><info><name>KCD2VR Held Map experiment</name><modid>KCD2VRHeldMap</modid><description>C7FM1 F2 rung 4: original animated parchment with close local map, native marker feed.</description><author>KCD2VR / local reference assets</author><version>59.16.7.1</version><created_on>2026-10-13</created_on></info><supports><kcd_version>1.*</kcd_version></supports></kcd_mod>')
 # Additive localization tables; retain the normal full language packs.
 for locale in (g/'Localization').glob('*_xml.pak'):
  outloc=out.parent.parent/'Localization'/locale.name;outloc.parent.mkdir(exist_ok=True)
  text='<Table><Row><Cell>ui_nm_kcd2vr_heldmap</Cell><Cell>Map</Cell><Cell>Map</Cell></Row><Row><Cell>ui_in_kcd2vr_heldmap</Cell><Cell>Held map</Cell><Cell>Held map</Cell></Row></Table>'
  with zipfile.ZipFile(outloc,'w',zipfile.ZIP_DEFLATED) as z:z.writestr('text_ui__kcd2vr_heldmap.xml',text)
 report={'item_guid':ITEM,'pak_sha256':hashlib.sha256(out.read_bytes()).hexdigest(),'entries':origin,'native_scene_and_GFx_acceptance':'NOT_RUN'}
 (r/'review/C7FM1/ASSET_PROVENANCE.json').write_text(json.dumps(report,indent=2));print('asset entries',len(entries),'pak bytes',out.stat().st_size)
if __name__=='__main__':build()
