"""Build an optional HUD override for the exact installed noletterbox 1.9 movie.

Never alters the installed game/mod. Preserves every noletterbox leaf except six
local message scales, then adds the same reviewed D68/D72 callback blocks as main.
The override must load after noletterbox and the matching KCD2VR companion.
"""
from pathlib import Path
import argparse,hashlib,io,json,os,struct,zipfile,zlib
import patch_hud_notifications_d63 as n
import patch_glyphs_d68 as g
import patch_alchemy_name_d72 as a
ROOT=Path(__file__).resolve().parents[1]
PIN='a82d2f71c88d9fabcaea68cfbd77359a3ac601008fd0fa4fec55dbd78f99544f'
MAIN_PIN='92957cc4e93556dbfdc7e78abbb1373f37cf6da9adbd0eda3e7c1937a893e440'
OUTPUT_PIN='bc8379d78a673b5309492c061f48b6e89ac261e2cd98c834537a5cee3478d530'
def sha(b):return hashlib.sha256(b).hexdigest()
def atomic(p,b):
 p.parent.mkdir(parents=True,exist_ok=True);t=p.with_suffix(p.suffix+'.tmp');t.write_bytes(b);os.replace(t,p)
def patch(raw,main):
 if sha(raw)!=PIN:raise ValueError('noletterbox HUD SHA256 mismatch; refuse to infer compatibility')
 if sha(main)!=MAIN_PIN:raise ValueError('Main HUD changed; review compatibility against its exact payload first')
 h,tags=n.unpack(raw);before=n.parse_tags(tags);edits={};changes={};parents={}
 for key,sprite,kind,payload,encoded in before:
  if kind not in (26,70):continue
  name,char,matrix,start,end=n.placement(payload,kind)
  if sprite==0 and name in n.PARENTS:parents[name]=char
  target=n.TARGETS.get((sprite,name))
  if not target:continue
  assert kind==26 and payload[0]==0x26 and char==152 and target not in changes
  assert matrix[:4]==(65536,0,0,65536)
  half=(32768,0,0,32768,*matrix[4:]);edits[key]=payload[:start]+n.encode_matrix(half)+payload[end:]
  changes[target]={'before':matrix,'after':half}
 assert parents==n.PARENTS and set(changes)==set(n.TARGETS.values())
 modified=n.rewrite(tags,edits);after=n.parse_tags(modified);unchanged=0;scripts=0
 assert len(before)==len(after)
 for l,r in zip(before,after):
  key,sprite,kind,payload,encoded=l
  assert l[:3]==r[:3]
  if kind==39:assert payload[:4]==r[3][:4]
  elif key in edits:assert r[3]==edits[key]
  else:assert encoded==r[4];unchanged+=1
  if kind in (12,59):assert payload==r[3];scripts+=1
 blocks=(g.callback(),a.injection())
 _,mainTags=g.unpack(main)
 for block in blocks:
  assert sum(kind==12 and payload==block for kind,payload,_ in g.tags(mainTags))==1,'matching main HUD callback required'
 for label in (g.CALLBACK,a.CALLBACK):assert label.encode() not in tags
 assert b'setNpcName\0' in tags and b'fc_setInputId\0' in tags
 output=b'';done=False
 for kind,payload,encoded in g.tags(modified):
  if kind==1 and not done:
   output+=b''.join(g.tag(12,block) for block in blocks);done=True
  output+=encoded
 assert done
 result=g.pack(raw,h,output)
 # Strip only the two additions: resulting tag stream must be byte-identical
 # to the six-matrix result, retaining noletterbox scripts and mask changes.
 _,resultTags=g.unpack(result)
 restored=b''.join(encoded for kind,payload,encoded in g.tags(resultTags) if not(kind==12 and payload in blocks))
 assert restored==modified
 return result,dict(input_sha256=sha(raw),main_hud_sha256=sha(main),output_sha256=sha(result),
  changes=changes,unchanged_leaf_tags=unchanged,preserved_script_blocks=scripts,
  added_callback_sha256=[sha(x) for x in blocks],native_or_headset_execution=False)
def zbytes(entries):
 out=io.BytesIO()
 with zipfile.ZipFile(out,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
  for name,data in sorted(entries.items()):
   info=zipfile.ZipInfo(name,(2026,11,1,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED;z.writestr(info,data)
 return out.getvalue()
def main():
 p=argparse.ArgumentParser();p.add_argument('--noletterbox-pak',help='Optional exact original for full regeneration; default rebuilds from vendored qualified output');p.add_argument('--output',required=True);args=p.parse_args()
 mainMovie=(ROOT/'companion/source/Libs/UI/hud.gfx').read_bytes()
 if args.noletterbox_pak:
  with zipfile.ZipFile(args.noletterbox_pak) as z:
   matches=[x for x in z.namelist() if x.lower().replace('\\','/')=='libs/ui/hud.gfx'];assert len(matches)==1
   raw=z.read(matches[0])
  movie,report=patch(raw,mainMovie)
 else:
  vendor=ROOT/'companion/D74_NOLETTERBOX_COMPAT'
  movie=(vendor/'source/Libs/UI/hud.gfx').read_bytes()
  report=json.loads((vendor/'HUD_COMPAT_MANIFEST.json').read_text())
  if sha(movie)!=OUTPUT_PIN or sha(mainMovie)!=MAIN_PIN:raise ValueError('Vendored compatible HUD or matching main changed')
  assert report['input_sha256']==PIN and report['output_sha256']==OUTPUT_PIN
  _,body=g.unpack(movie)
  for block in (g.callback(),a.injection()):assert sum(kind==12 and payload==block for kind,payload,_ in g.tags(body))==1
  report.pop('archive_sha256',None)
 out=Path(args.output);out.mkdir(parents=True,exist_ok=True)
 pak=zbytes({'Libs/UI/hud.gfx':movie});report['pak_sha256']=sha(pak)
 manifest=b'''<?xml version="1.0" encoding="utf-8"?>
<kcd_mod><info><name>KCD2VR HUD noletterbox compatibility D74</name>
<modid>zz_kcd2vr_hud_compat</modid><description>Exact noletterbox 1.9 HUD with KCD2VR compact messages and matching glyph/alchemy-name callbacks. Load after noletterbox and KCD2VR.</description>
<author>KCD2VR</author><version>74.1</version><created_on>2026-11-01</created_on><modifies_level>false</modifies_level></info>
<supports><kcd_version>1.*</kcd_version></supports></kcd_mod>
'''
 readme='''OPTIONAL COMPATIBILITY ADDON -- ONLY IF noletterbox 1.9 IS INSTALLED
Requires the matching D74 main companion. Main already has compact notifications
and callbacks; this addon preserves the noletterbox movie which otherwise replaces
them. It does not contain gore. Do not install it for other HUD replacements.

Extract Mods/zz_kcd2vr_hud_compat beside your existing Mods/KCD2VR and Mods/noletterbox.
The captured game run has no mod_order.txt and loads mods alphabetically; this
addon name follows noletterbox. If you maintain Mods/mod_order.txt, preserve your
other entries and list zz_kcd2vr_hud_compat after both KCD2VR and noletterbox.
Verify the next native kcd.log says this addon mounts after both. No installer here
changes your mod_order.txt, user settings, installed game or other mod.

Only the HUD movie is overridden: six notification child scales are 50%; all
noletterbox movie code and all other leaves are retained, with the existing D68
glyph callback and D72 native ingredient-name callback added unchanged. D74 DLL
owns the station-local prompt60% service; this PAK is not a substitute for DLL.
Fader/ratio-strip/theater geometry is unchanged from your noletterbox movie.
Native/headset execution NOT_RUN. Remove this addon directory to undo it.
'''
 archive=zbytes({'Mods/zz_kcd2vr_hud_compat/mod.manifest':manifest,'Mods/zz_kcd2vr_hud_compat/Data/KCD2VR_HUD_Compat.pak':pak,
  'README.txt':readme.encode(),'HUD_COMPAT_MANIFEST.json':json.dumps(report,indent=2).encode()})
 atomic(out/'KCD2VR_D74_NOLETTERBOX_HUD_COMPAT_20261101.zip',archive)
 report['archive_sha256']=sha(archive);atomic(out/'HUD_COMPAT_VERIFICATION.json',json.dumps(report,indent=2).encode())
 print(json.dumps(report,indent=2))
if __name__=='__main__':main()
