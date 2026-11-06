"""Optional NPC-only ordinary defence retiming; never edits game files.
Builds on the exact main companion ADB, preserving every unselected fragment.
"""
from pathlib import Path
import argparse,copy,hashlib,json,re,zipfile,xml.etree.ElementTree as ET

def sha(data):return hashlib.sha256(data).hexdigest()
def main():
 p=argparse.ArgumentParser();p.add_argument('--base',type=Path,required=True);p.add_argument('--data',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
 adb_name='Animations/Mannequin/ADB/kcd_male_combat_generated.adb'
 with zipfile.ZipFile(a.base)as z:base={n:z.read(n)for n in z.namelist()}
 with zipfile.ZipFile(a.data/'Tables.pak')as z:native={n:z.read(n)for n in z.namelist()if n.endswith('.xml')}
 adb=base[adb_name];root=ET.fromstring(adb);fragments={f.get('GUID'):(g.tag,f)for g in root.find('FragmentList')for f in g.findall('Fragment')}
 selected={};outputs={};rows_receipt=[];excluded=[]
 timing={'animation_duration','block_time_to_end','block_time_to_hit','block_time_to_start','fade_in','sync_point'}
 groups={'block':'CombatBlockGen','perfect_block':'CombatBlockPerfectGen','dodge':'CombatDodgeGen'}
 for family,group in groups.items():
  name='Libs/Tables/combat/combat_action_'+family+'.xml';data=base.get(name,native[name]);table=ET.fromstring(data);container=list(table)[0];rows=list(container);picked=[]
  roles={}
  for row in rows:roles.setdefault(row.get('mn_fragment_guid'),set()).add(row.get('player'))
  for row in rows:
   guid=row.get('mn_fragment_guid');frag=fragments.get(guid)
   if row.get('player')!='false' or row.get('contains_attack')=='true' or not frag or frag[0]!=group or roles[guid]!={'false'}:
    excluded.append({'family':family,'guid':guid,'player':row.get('player'),'contains_attack':row.get('contains_attack'),'fragment':frag[0]if frag else None});continue
   assert 'player' not in ((frag[1].get('Tags','')+'+'+frag[1].get('FragTags','')).split('+'))
   selected[guid]=group
   changed=copy.deepcopy(row)
   for key,value in row.attrib.items():
    if key in timing and float(value)>0:changed.set(key,format(float(value)/1.5,'.9g'))
   assert all(changed.get(k)==v for k,v in row.attrib.items()if k not in timing)
   picked.append(changed);rows_receipt.append({'family':family,'guid':guid,'before':dict(row.attrib),'after':dict(changed.attrib)})
  patch=copy.deepcopy(table);dest=list(patch)[0];dest[:]=picked
  outputs[name.replace('.xml','__kcdvr_npc_defence15.xml')]=ET.tostring(patch,encoding='utf-8',xml_declaration=True)
 text=adb.decode('utf-8-sig');changed_fragments=[]
 def rewrite(match):
  block=match.group(0);guid=re.search(r'GUID="([^"]+)"',block).group(1)
  if guid not in selected:return block
  def tagfix(m):
   tag=m.group(0)
   if tag.startswith('<Animation '):
    speed=re.search(r'\bspeed="([^"]+)"',tag)
    if speed:tag=tag[:speed.start(1)]+format(float(speed.group(1))*1.5,'.9g')+tag[speed.end(1):]
    else:tag=tag.replace('/>',' speed="1.5" />')
   elif tag.startswith('<Blend ') or tag.startswith('<Fragment '):
    tag=re.sub(r'\b(ExitTime|StartTime|Duration|BlendOutDuration)="([^"]+)"',lambda x:x.group(1)+'="'+format(float(x.group(2))/1.5,'.9g')+'"',tag)
   return tag
  result=re.sub(r'<(?:Animation|Blend|Fragment)\s[^>]*>',tagfix,block)
  changed_fragments.append({'guid':guid,'before_sha256':sha(block.encode()),'after_sha256':sha(result.encode())})
  return result
 pattern=r'<Fragment\s[^>]*GUID="[^"]+"[^>]*>.*?</Fragment>'
 changed=re.sub(pattern,rewrite,text,flags=re.S);assert len(changed_fragments)==len(selected)
 # Strong preservation: erase only selected blocks and compare everything else.
 def erase(t):return re.sub(pattern,lambda m:''if re.search(r'GUID="([^"]+)"',m.group(0)).group(1)in selected else m.group(0),t,flags=re.S)
 assert erase(text)==erase(changed)
 parsed=ET.fromstring(changed);after={f.get('GUID'):f for g in parsed.find('FragmentList')for f in g.findall('Fragment')}
 for guid in selected:
  before_anims=list(fragments[guid][1].iter('Animation'));after_anims=list(after[guid].iter('Animation'));assert len(before_anims)==len(after_anims)>0
  for b,c in zip(before_anims,after_anims):assert abs(float(c.get('speed','1'))-float(b.get('speed','1'))*1.5)<1e-7
 outputs[adb_name]=changed.encode('utf-8');a.out.mkdir(parents=True,exist_ok=True);pak=a.out/'KCD2VR_NPC_DEFENCE_150.pak'
 with zipfile.ZipFile(pak,'w',compression=zipfile.ZIP_DEFLATED)as z:
  for name,data in outputs.items():z.writestr(zipfile.ZipInfo(name,(2026,10,23,0,0,0)),data)
 receipt={'main_pak_sha256':sha(a.base.read_bytes()),'optional_pak_sha256':sha(pak.read_bytes()),'selected_fragments':len(selected),'families':{f:sum(x['family']==f for x in rows_receipt)for f in groups},'fragments':changed_fragments,'rows':rows_receipt,'excluded':excluded,'unselected_adb_byte_preserved':True,'native_headset':'NOT_RUN','limits':['Requires exact main companion and loading after it; full ADB is an exclusive asset override.','Ordinary NPC block/perfect-parry/dodge only; broken-block, paired/sync defence, attacks and hit/death variants excluded.','Henry windows unchanged. Selected NPC block/parry effective times also become earlier; this is gameplay timing as well as playback. XML-over-TBL loading and headset quality are unverified. Procedural-layer Blend times scale with the fragment; non-time parameters remain unchanged.']}
 (a.out/'RECEIPT.json').write_text(json.dumps(receipt,indent=2)+'\n');print(json.dumps({k:v for k,v in receipt.items()if k not in ['fragments','rows','excluded']},indent=2))
if __name__=='__main__':main()
