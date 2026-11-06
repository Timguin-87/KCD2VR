"""Add owned live-map methods to native GeneralBook; preserve all native tags.
No minimap.dll, native animation/mesh edits, material rebinding or GPU hook.
"""
from pathlib import Path
import argparse,hashlib,json,struct,zlib,zipfile,xml.etree.ElementTree as ET
import build_map_navigation_t2 as a
import build_companion32 as archive
import decode_ui90_gfx as swf
ROOT=Path(__file__).resolve().parents[1]
ANCHORS=('image0','image2','image3','image1')
NAME='__kcd2vr_live_h28'
FUN={'Begin':'kcd2vr_h28_begin','Tile':'kcd2vr_h28_tile','Marker':'kcd2vr_h28_marker','Clear':'kcd2vr_h28_clear','End':'kcd2vr_h28_end'}
def tag(t,b):return struct.pack('<HI',t<<6|63,len(b))+b
def methods():
 root=a.var('_root');ext=a.member(a.member(a.var('flash'),'external'),'ExternalInterface')
 # Only our child clips are hidden. Native content/actions are untouched.
 end=b''
 for name in ANCHORS:
  c=a.member(a.member(root,name),NAME)
  end+=a.iff(c,a.setm(c,'_visible',a.push(False)))
 end+=a.iff(a.member(root,'__kcd2vr_notice_h28'),a.setm(a.member(root,'__kcd2vr_notice_h28'),'attempt',a.push(6)))
 glyph=a.local('m',a.call(a.var('map'),'createEmptyMovieClip',[a.push('m')+a.var('index')+a.op(0x47),a.var('index')+a.push(20)+a.op(0x47)]))
 for clip,points in [('dot',[(0,-8),(7,0),(0,8),(-7,0)]),('dir',[(0,-12),(8,8),(0,4),(-8,8)])]:
  glyph+=a.local('d',a.create(a.var('m'),clip,1 if clip=='dot' else 2))
  glyph+=a.polygon(a.var('d'),points,0x231B13)
  glyph+=a.local('ink',a.create(a.var('d'),'ink',1))
  glyph+=a.polygon(a.var('ink'),[(0,-5),(4,0),(0,5),(-4,0)] if clip=='dot' else [(0,-8),(5,5),(0,2),(-5,5)],0x6F4D0C)
  glyph+=a.setm(a.var('m'),'tintD' if clip=='dot' else 'tintA',a.new('Color',[a.var('ink')]))
 glyph+=a.setm(a.var('m'),'_visible',a.push(False))
 # A page is 1024 square, while the established local-map coordinate system
 # has its square at256,0..768,512. This exact transform maps it to0..1024.
 make=a.local('c',a.call(a.var('parent'),'createEmptyMovieClip',[a.push(NAME),a.call(a.var('parent'),'getNextHighestDepth')]))
 for k,v in [('_x',-512),('_y',0),('_xscale',200),('_yscale',200)]:make+=a.setm(a.var('c'),k,a.push(v))
 make+=a.local('back',a.create(a.var('c'),'back',1))+a.polygon(a.var('back'),[(256,0),(768,0),(768,512),(256,512)],0x808080)
 make+=a.local('map',a.create(a.var('c'),'map',10))+a.local('mask',a.create(a.var('c'),'mask',11))
 make+=a.polygon(a.var('mask'),[(256,0),(768,0),(768,512),(256,512)],0xFFFFFF)+a.stmt(a.var('map'),'setMask',[a.var('mask')])
 for i in range(4):
  make+=a.local('h',a.create(a.var('map'),'t'+str(i),i+1))
  make+=a.setm(a.var('h'),'image',a.create(a.var('h'),'image',1))+a.setm(a.var('h'),'loader',a.new('MovieClipLoader'))
  make+=a.setm(a.var('h'),'source',a.push(''))+a.setm(a.var('h'),'_visible',a.push(False))
 for i in range(a.MARKERS):make+=a.stmt(root,'kcd2vr_h28_glyph',[a.var('map'),a.push(i)])
 make+=a.setm(a.var('c'),'onEnterFrame',a.var('kcd2vr_h28_watch'))
 beginpage=a.local('c',a.member(a.var('parent'),NAME))+a.iff(a.var('c')+a.op(0x12),make)
 beginpage+=a.setm(a.var('c'),'_visible',a.push(True))+a.setm(a.var('c'),'last',a.op(0x34))
 begin=b''.join(a.iff(a.member(root,n),a.stmt(root,'kcd2vr_h28_page',[a.member(root,n)])) for n in ANCHORS)
 begin+=a.iff(a.member(root,'__kcd2vr_notice_h28'),a.setm(a.member(root,'__kcd2vr_notice_h28'),'attempt',a.push(6)))
 watch=a.iff(a.member(a.var('this'),'_visible'),a.iff(a.op(0x34)+a.member(a.var('this'),'last')+a.op(0x0B)+a.push(1500)+a.op(0x67),a.setm(a.var('this'),'_visible',a.push(False))))
 tile=b'';marker=b'';clear=b''
 for n in ANCHORS:
  c=a.member(a.member(root,n),NAME);map=a.member(c,'map')
  h=a.member(map,a.push('t')+a.var('index')+a.op(0x47))
  b=a.local('h',h)+a.setm(a.var('h'),'_visible',a.var('visible'))
  transform=b''.join(a.setm(a.var('h'),k,a.var(v)) for k,v in [('_x','x'),('_y','y'),('_xscale','scale'),('_yscale','scale')])
  load=a.setm(a.var('h'),'source',a.var('file'))+a.stmt(a.member(a.var('h'),'loader'),'loadClip',[a.push('img://')+a.var('file')+a.op(0x47),a.member(a.var('h'),'image')])
  different=a.member(a.var('h'),'source')+a.var('file')+a.op(0x49)+a.op(0x12)
  b+=a.iff(a.var('visible'),transform+a.iff(different,load))
  tile+=a.iff(c,b)
  m=a.member(map,a.push('m')+a.var('index')+a.op(0x47));b=a.local('m',m)
  for k,v in [('_x',a.var('x')),('_y',a.var('y')),('_rotation',a.var('angle')),('_visible',a.push(True))]:b+=a.setm(a.var('m'),k,v)
  b+=a.setm(a.member(a.var('m'),'dot'),'_visible',a.var('arrow')+a.op(0x12))+a.setm(a.member(a.var('m'),'dir'),'_visible',a.var('arrow'))
  b+=a.stmt(a.member(a.var('m'),'tintD'),'setRGB',[a.var('color')])+a.stmt(a.member(a.var('m'),'tintA'),'setRGB',[a.var('color')])
  marker+=a.iff(c,b)
  clear+=a.iff(c,b''.join(a.setm(a.member(map,'m'+str(i)),'_visible',a.push(False)) for i in range(a.MARKERS)))
 # Native GeneralBook already uses ExternalInterface.call for OnBookPosition.
 # Capture Images before fc_setFullBook clears them, then call native ONCE.
 # The engine event gives Lua the ACTUAL UI instance, avoiding guessed0/GetArray.
 payload=a.call(a.var('g_ImagesA'),'join',[a.push('|')])
 pulse=a.iff(a.push(6)+a.member(a.var('this'),'attempt')+a.op(0x67),a.iff(a.op(0x34)+a.member(a.var('this'),'next')+a.op(0x67),a.setm(a.var('this'),'next',a.op(0x34)+a.push(500)+a.op(0x47))+a.setm(a.var('this'),'attempt',a.member(a.var('this'),'attempt')+a.push(1)+a.op(0x47))+a.stmt(ext,'call',[a.push('OnKCD2VRMapContent'),a.var('__kcd2vr_payload_h28')])) )
 notice=a.local('n',a.member(root,'__kcd2vr_notice_h28'))
 notice+=a.iff(a.var('n')+a.op(0x12),a.local('n',a.call(root,'createEmptyMovieClip',[a.push('__kcd2vr_notice_h28'),a.call(root,'getNextHighestDepth')])))
 notice+=a.setm(a.var('n'),'_visible',a.push(False))+a.setm(a.var('n'),'attempt',a.push(0))+a.setm(a.var('n'),'next',a.push(-1))+a.setm(a.var('n'),'onEnterFrame',a.var('kcd2vr_h28_notice'))
 wrap=a.local('payload',payload)+a.stmt(root,'kcd2vr_h28_end')+a.stmt(root,'__kcd2vr_native_set_h28',[a.var('dyslexiaLevel'),a.var('type'),a.var('visual')])
 wrap+=a.assign('__kcd2vr_epoch_h28',a.var('__kcd2vr_epoch_h28')+a.push(1)+a.op(0x47))
 wrap+=a.assign('__kcd2vr_payload_h28',a.var('__kcd2vr_epoch_h28')+a.push('|')+a.op(0x47)+a.var('payload')+a.op(0x47))
 wrap+=a.stmt(ext,'call',[a.push('OnKCD2VRMapContent'),a.var('__kcd2vr_payload_h28')])
 wrap+=a.iff(a.call(a.var('payload'),'indexOf',[a.push('kcd2vr_map_')])+a.push(-1)+a.op(0x67),notice)
 code=a.assign('__kcd2vr_native_set_h28',a.member(root,'fc_setFullBook'))+a.assign('__kcd2vr_epoch_h28',a.push(0))
 for n,args,b in [('kcd2vr_h28_glyph',['map','index'],glyph),('kcd2vr_h28_page',['parent'],beginpage),('kcd2vr_h28_watch',[],watch),('kcd2vr_h28_notice',[],pulse),('kcd2vr_h28_end',[],end),(FUN['Begin'],[],begin),(FUN['Tile'],['index','file','x','y','scale','visible'],tile),(FUN['Marker'],['index','x','y','angle','color','arrow'],marker),(FUN['Clear'],[],clear),('fc_setFullBook',['dyslexiaLevel','type','visual'],wrap)]:code+=a.fn(n,args,b)
 return code+a.op(0)
def patch_gfx(raw):
 if raw[:3]!=b'CFX' or raw[3]!=8:raise ValueError('unexpected native book bytecode')
 body=zlib.decompress(raw[8:]);bits=swf.Bits(body);bits.u(bits.u(5)*4);start=bits.pos()+4
 p=start;insert=None
 while p<len(body):
  at=p;h=struct.unpack_from('<H',body,p)[0];p+=2;n=h&63;t=h>>6
  if n==63:n=struct.unpack_from('<I',body,p)[0];p+=4
  if t==1:insert=at;break
  p+=n
 if insert is None:raise ValueError('native first frame missing')
 addition=tag(12,methods());out=body[:insert]+addition+body[insert:]
 assert out[:insert]+out[insert+len(addition):]==body
 return raw[:4]+struct.pack('<I',len(out)+8)+zlib.compress(out,9),{'insert':insert,'added_bytes':len(addition),'native_body_sha256':archive.sha(body)}
def patch_xml(raw):
 root=ET.fromstring(raw);e=root.find("UIElement[@name='GeneralBook']");f=e.find('functions')
 specs=[('Begin',[]),('Tile',[('index','int'),('file','string'),('x','float'),('y','float'),('scale','float'),('visible','bool')]),('Marker',[('index','int'),('x','float'),('y','float'),('angle','float'),('color','int'),('arrow','bool')]),('Clear',[]),('End',[])]
 for n,params in specs:
  el=ET.SubElement(f,'function',name='KCD2VRMap'+n,funcname=FUN[n])
  for name,t in params:ET.SubElement(el,'param',name=name,type=t)
 ev=ET.SubElement(e.find('events'),'event',name='OnKCD2VRMapContent',fscommand='OnKCD2VRMapContent')
 ET.SubElement(ev,'param',name='Payload',type='string')
 return ET.tostring(root,encoding='utf-8',xml_declaration=True)
def build(data,base,output,report):
 if output.resolve()==base.resolve():raise ValueError('input must remain frozen')
 before=archive.unpack(base.read_bytes());after=dict(before)
 with zipfile.ZipFile(data/'IPL_GameData.pak') as z:gfx=z.read('Libs/UI/GeneralBook.gfx');xml=z.read('Libs/UI/UIElements/GeneralBook.xml')
 if archive.sha(gfx)!='8bf92b419c522efcbae8dd229465c833839f0e147462b43f5b2748f5cf8d27df' or archive.sha(xml)!='2ebf782f74ada05db55e747efab3f9590938149bf8b1d0f3f21c0ac67a610563':raise ValueError('unsupported native GeneralBook source')
 for k in ['Libs/UI/GeneralBook.gfx','Libs/UI/UIElements/GeneralBook.xml']:
  if k in before:raise ValueError('existing book override needs reconciliation: '+k)
 after['Libs/UI/GeneralBook.gfx'],evidence=patch_gfx(gfx)
 after['Libs/UI/UIElements/GeneralBook.xml']=patch_xml(xml)
 for k,p in [('Scripts/Startup/zzkcd2vr_map.lua','map_runtime.lua'),('Scripts/KCD2VR/map_navigation_t2.lua','map_navigation_t2.lua'),('Scripts/KCD2VR/map_native_read_h25.lua','map_native_read_h25.lua'),('Scripts/KCD2VR/map_native_live_h28.lua','map_native_live_h28.lua')]:after[k]=(ROOT/'mods/KCD2VRMap'/p).read_bytes()
 changed=sorted(k for k in after if after[k]!=before.get(k));archive.atomic(output,archive.packed(after))
 result={'base_sha256':archive.sha(base.read_bytes()),'output_sha256':archive.sha(output.read_bytes()),'changed':changed,'preserved_members':len(before)-len(set(changed)&set(before)),'native_gfx_sha256':archive.sha(gfx),'native_xml_sha256':archive.sha(xml),'native_tag_preservation':evidence,'minimap_dll':False,'native_animations_mesh_material':'UNCHANGED','native_game_headset':'NOT_RUN'}
 archive.atomic(report,(json.dumps(result,indent=2)+'\n').encode());return result
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--data',type=Path,required=True);p.add_argument('--base',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--report',type=Path,required=True);a0=p.parse_args();print(json.dumps(build(a0.data,a0.base,a0.output,a0.report),indent=2))
