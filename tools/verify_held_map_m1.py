from pathlib import Path
import sys,zipfile,zlib,struct,hashlib,json,subprocess,xml.etree.ElementTree as ET
r=Path.cwd();sys.path.insert(0,str(r/'tools'));import decode_ui90_gfx as swf
sys.path.insert(0,'C:/Users/turcs/.codex/worktrees/d514/KCD2VR/build-review178r19/analysis-deps');import pefile
exe=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll');pe=pefile.PE(str(exe),fast_load=True)
pins={0xA6D6F4:'48895c240848896c2418488954241056',0x87963C:'48895c24185556574881ecb000000048',0x509810:'48895c24084889742410574883ec2045',0x423C30:'0f10020f1181080a00000f104a100f11',0x6411AC:'40534883ec208bda33d2e8095ff9ff4c'}
for a,s in pins.items():assert pe.get_data(a,len(bytes.fromhex(s)))==bytes.fromhex(s)
assert pe.FILE_HEADER.TimeDateStamp==0x6A350E20
for v,s,fn in [(0x4077048,0x80,0xA6D6F4),(0x4072C78,0x168,0x509810),(0x4072C78,0x328,0x6411AC)]:assert struct.unpack('<Q',pe.get_data(v+s,8))[0]==pe.OPTIONAL_HEADER.ImageBase+fn
assert hashlib.sha256(exe.read_bytes()).hexdigest()=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
with zipfile.ZipFile(r/'companion/Mods/KCD2VRHeldMap/Data/KCD2VRHeldMap.pak') as z:
 assert z.testzip() is None
 for name in z.namelist():
  if name.lower().endswith(('.xml','.cdf','.mtl','.chrparams')):ET.fromstring(z.read(name))
 gfx=z.read('Libs/UI/KCD2VRHeldMap.gfx');raw=zlib.decompress(gfx[8:]);assert len(raw)+8==struct.unpack_from('<I',gfx,4)[0]
 bits=swf.Bits(raw);nb=bits.u(5);rect=[bits.s(nb) for _ in range(4)];assert rect==[0,10240,0,10240]
 tags=list(swf.tags(raw,bits.pos()+4));assert tags[-1][0]==0
 added=[(a,p) for a,p in tags if a==39 and struct.unpack_from('<H',p)[0]==65530];assert len(added)==1
 with zipfile.ZipFile('C:/Steam/steamapps/common/KingdomComeDeliverance2/inspirations/mods/minimap.pak') as orig:
  b=orig.read('Libs/UI/minimap.gfx');d=zlib.decompress(b[8:]);x=swf.Bits(d);nb=x.u(5);[x.s(nb) for _ in range(4)];old=list(swf.tags(d,x.pos()+4))
  i=next(i for i,t in enumerate(tags) if t==added[0]);assert tags[:i]+tags[i+3:]==old
 print('PAK CRC, XML and preserved GFX tags: PASS; retail SHA, entry pins and vtable consumer slots: PASS')
report={'retail_sha_and_timestamp':'PASS','entry_pins':'PASS','exact_vtable_consumers':'PASS','pak_CRC_and_XML':'PASS','GFX_stage_and_preserved_original_tags':'PASS','native_GFx_execution':'NOT_RUN','native_scene_pixels':'NOT_RUN'}
(r/'review/C7FM1/STATIC_VERIFICATION.json').write_text(json.dumps(report,indent=2))
