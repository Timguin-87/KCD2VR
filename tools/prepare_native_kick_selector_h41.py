from pathlib import Path
import sys, struct, hashlib, json, zipfile, xml.etree.ElementTree as ET,argparse
args=argparse.ArgumentParser()
args.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1])
args.add_argument('--output',type=Path,required=True)
args.add_argument('--pak',type=Path,required=True)
args.add_argument('--dll',type=Path)
args.add_argument('--tables',type=Path)
args.add_argument('--d72-current-source',action='store_true')
args.add_argument('--d85-free',action='store_true',help='D85: also copy the no-target prepare branch 12EDF54/12EE0B4 (free guard reader/request)')
arg=args.parse_args();S=arg.source.resolve();R=arg.output.resolve();R.mkdir(parents=True,exist_ok=True)
sys.path.insert(0,str(S/'tools'))
from native_research112 import PE
DLL=arg.dll or Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll')
sha=lambda b:hashlib.sha256(b).hexdigest()
assert sha(DLL.read_bytes())=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
p=PE(DLL)
ranges=[(0x27C08C8,0x27C09A8),(0x90F824,0x90F89C),(0x939310,0x9397C0),
 (0xD4A99C,0xD4ACC8),(0xD4B06C,0xD4B0E8),(0x93A28C,0x93A2D0),(0x939C84,0x939CD8),
 (0x6881C8,0x68827E),(0x687BBC,0x687CC4),(0x687D14,0x687F3C),
 (0x9CA0E4,0x9CA152),(0x9CA86C,0x9CA944),(0x9CB23C,0x9CB492),
 (0x9CB518,0x9CB6A8),(0x3F9358,0x3F964E),(0x21E5694,0x21E57F0),
 (0x2262628,0x2262670),(0x936744,0x936A28),(0x1E520C0,0x1E52158),(0x1E47640,0x1E47664),
 (0x9838C8,0x983D82),(0x9158C4,0x91590A),(0x90D1B0,0x90D1F4),
 (0x90C850,0x90C890),(0x9CC3D4,0x9CC5F0),(0x9CC5F0,0x9CC988),
 (0x9CCBF8,0x9CCC10),(0x9CCCF8,0x9CCE24)]
if arg.d72_current_source:
 ranges += [(0x12EDF78,0x12EE0B4),(0x1315CB4,0x1315D0F),(0x23478FA,0x2347922),(0x18A4B7C,0x18A4BFC),(0x1E3B330,0x1E3B3BA),(0x1A7DDF0,0x1A7DDF5)]
if arg.d85_free:
 # D85 kick: D4AAC4 no-opponent branch D4AC61 -> 12EDF78 (free row exists
 # under FreeGuard) -> 12EE0B4 (current guard is FreeGuard via 12EDF54, else
 # request it through 12EE110, which stays an explicit host leaf).
 ranges += [(0x12EDF54,0x12EDF78),(0x12EE0B4,0x12EE110)]
pieces=[]; headers=[]
for a,b in ranges:
 data=p.read(a,b-a);headers.append((a,len(data),sum(len(x) for x in pieces),sha(data)));pieces.append(data)
(R/'native_selection_ranges.bin').write_bytes(b''.join(pieces))
pak=arg.pak.resolve()
tables=arg.tables or DLL.parents[2]/'Data/Tables.pak'
with zipfile.ZipFile(tables) as z:
 xname=next(n for n in z.namelist() if n.lower().endswith('/combat_action_attack.xml'))
 tname=next(n for n in z.namelist() if n.lower().endswith('/combat_action_attack.tbl'))
 xml=z.read(xname);tbl=z.read(tname)
 retail=[e.attrib for e in ET.fromstring(xml).iter('combat_action_attack')]
 mapping=z.read('Libs/Tables/combat/combat_zone_mapping.tbl')
 group_xml=z.read('Libs/Tables/combat/combat_weapon_group_to_class.xml')
 groups=[e.attrib for e in ET.fromstring(group_xml).iter('combat_weapon_group_to_class')]
with zipfile.ZipFile(pak) as z:
 name='Libs/Tables/combat/combat_action_attack.xml';built=z.read(name)
 adb=z.read('Animations/Mannequin/ADB/kcd_male_combat_generated.adb')
if arg.d72_current_source:
 built=(S/'companion/source/Libs/Tables/combat/combat_action_attack.xml').read_bytes()
 adb=(S/'companion/source/Animations/Mannequin/ADB/kcd_male_combat_generated.adb').read_bytes()
rows=[e.attrib for e in ET.fromstring(built).iter('combat_action_attack')]
assert len(rows)==(286 if arg.d72_current_source else 233) and len(retail)==221
lookup={x['mn_fragment_guid']:(i,x) for i,x in enumerate(retail)}
raw=[];lineage=[]
for x in rows:
 guid=x['mn_fragment_guid']
 if guid in lookup: i,base=lookup[guid]
 else:
  # Authored armed rows clone the native male/dog ordinary kick; D72 free
  # rows clone the exact native FreeAttack kick template instead.
  candidates=[(i,b) for i,b in enumerate(retail) if b['attack_type_id']=='4' and b['action_type_id']==x['action_type_id'] and (b['mn_fragment_guid']=='e942fff4-f601-4378-9500-7f6ac20a44ad' if x['action_type_id']=='26' else b.get('opp_actor_classes')==x.get('opp_actor_classes'))]
  assert len(candidates)==1
  i,base=candidates[0]
  allowed={'mn_fragment_guid','mn_tags','r_weapon_class_id','l_weapon_class_id','r_weapon_group_id','l_weapon_group_id','z_spatial_grid','input_class_id','mn_option_index'}
  assert {k for k in set(base)|set(x) if base.get(k)!=x.get(k)}<=allowed
 data=bytearray(tbl[28+i*0x170:28+(i+1)*0x170]);assert len(data)==0x170
 # Native raw offsets are established by the untouched 9838C8 hydrator.
 for off,k in [(0xCC,'action_type_id'),(0xD4,'attack_zone_id'),(0xD8,'attack_type_id'),(0xDC,'input_class_id'),(0x16C,'finishing_zone_id'),(0x128,'r_weapon_class_id'),(0x12C,'l_weapon_class_id'),(0x130,'r_weapon_group_id'),(0x134,'l_weapon_group_id')]:
  struct.pack_into('<i',data,off,int(x[k]))
 raw.append(bytes(data));lineage.append({'guid':guid,'retail_template_index':i,'tags':x['mn_tags'],'fragment':x['mn_fragment_id'],'attack_type':int(x['attack_type_id']),'input':int(x['input_class_id']),'zone':int(x['attack_zone_id']),'rclass':int(x['r_weapon_class_id']),'lclass':int(x['l_weapon_class_id']),'rgroup':int(x['r_weapon_group_id']),'lgroup':int(x['l_weapon_group_id']),'opp_classes':x.get('opp_actor_classes','')})
 # Authored metadata GUID must name its companion ADB fragment. The fixture
 # executes selection/admission only: raw template GUID/string storage is not
 # an executed animation-database lookup and must not be reported as one.
 if 'kick' in x['mn_tags'] and x['mn_fragment_id']=='CombatAttackGen':
  assert guid.encode() in adb
(R/'actual_rows.bin').write_bytes(b''.join(raw))
(R/'actual_zone_mapping.bin').write_bytes(mapping[28:])
quote=lambda s:json.dumps(s)
h=['#pragma once',f'static constexpr unsigned nativeRowCountFixture = {len(rows)};','struct NativeRangeFixture { unsigned rva,size,offset; };','static const NativeRangeFixture nativeRangesFixture[] = {']
h += [f'{{0x{a:X},0x{n:X},0x{o:X}}},' for a,n,o,_ in headers];h+=['};','struct RowMetaFixture { const char* guid; const char* tags; int atk,inp,zone,rc,lc,rg,lg; const char* opp; };','static const RowMetaFixture rowMetaFixture[]={']
h += ['{'+','.join([quote(x['guid']),quote(x['tags'])]+[str(x[k]) for k in ['attack_type','input','zone','rclass','lclass','rgroup','lgroup']]+[quote(x['opp_classes'])])+'},' for x in lineage];h+=['};']
h += ['struct GroupMemberFixture {int group,cls; bool onehand;};','static const GroupMemberFixture groupMemberFixture[]={']
h += ['{'+g['combat_weapon_group_id']+','+g['weapon_class_id']+','+str(g.get('only_for_one_hand')=='true').lower()+'},' for g in groups];h+=['};']
(R/'native_selection_inputs.h').write_text('\n'.join(h)+'\n')
result={'retail':{'path':str(DLL),'sha256':sha(DLL.read_bytes())},'native_ranges':[{'rva':hex(a),'size':n,'offset':o,'sha256':v} for a,n,o,v in headers], 'pak':{'path':str(pak),'sha256':sha(pak.read_bytes()),'attack_member_sha256':sha(built),'adb_member_sha256':sha(adb)},'population_source':'canonical companion/source' if arg.d72_current_source else 'supplied donor PAK','raw_tbl_sha256':sha(tbl),'zone_mapping_sha256':sha(mapping),'weapon_group_member_sha256':sha(group_xml),'rows':lineage,'limits':['Native enum/string/group allocation helpers are explicit host leaves. Original hydrate, query, actor/mounted/weapon/input filters, zone selection, selected-row wrapper and queue consumer execute retail machine instructions. Native RPG/animation objects are not a live engine. Row population comes from the stated offline source, not a captured live database. Companion rows use exact native kick binary templates with the listed authored selector-field substitutions; spatial score/grid allocation is an explicit host leaf. Raw template GUID/string storage remains retail-template data: this harness does not execute an ADB GUID/CAF lookup or animation, collision, damage or stamina service. The separate asset receipt validates authored GUID/ADB lineage.']}
assert p.stamp==0x6A350E20
def target(at,size):
 b=p.read(at,size);return at+size+struct.unpack_from('<i',b,size-4)[0]
bindings=[]
for name,string,at,field,ctor,handler in [('prepare_to_attack',0x3E9FB40,0x1738F74,0x100,0xA4CAF4,0x27C08C8),('attack_primary',0x3E9FB88,0x1738F87,0x108,0xA4CB1F,0x27C08F8),('attack_special',0x3BB5DC8,0x1738FAD,0x118,0xA4CB75,0x27C0928)]:
 assert p.read(string,len(name)+1)==name.encode()+b'\0'
 assert target(at,7)==string and p.read(at+7,3)==bytes.fromhex('488d8b') and struct.unpack('<I',p.read(at+10,4))[0]==field
 assert target(at+14,5)==0x8E5AEC
 assert target(ctor,7)==handler and p.read(ctor+11,3)==bytes.fromhex('488d93') and struct.unpack('<I',p.read(ctor+14,4))[0]==field
 assert target(ctor+34,5)==0xA4CD60
 bindings.append({'name':name,'action_field':hex(field),'name_registration_rva':hex(at),'handler_registration_rva':hex(ctor),'handler_rva':hex(handler),'name_bytes_sha256':sha(p.read(at,19)),'handler_bytes_sha256':sha(p.read(ctor,39))})
result['action_bindings']=bindings
result['ingress_limit']='Target map names/fields/handlers are exact static pins. The host composition starts at the original-handler leaf of PhysCallAction6698; outer OnAction8E5324, C_Player virtual4A0 and +AC0 full-edge queue are NOT executed.'
adbtree=ET.fromstring(adb)
result['selected_clip_proof']=[{'guid':f.get('GUID'),'tags':f.get('Tags'),'fragment_tags':f.get('FragTags'),'animations':[e.get('name') for e in f.iter('Animation')],'normalized_fragment_sha256':sha(ET.tostring(f))} for f in adbtree.iter('Fragment') if f.get('GUID') in ['9c22a61c-fb64-3ac1-b0b8-1d325a2cced7','8b79de33-b0b1-5caa-8b5a-3b58afe71a62']]
assert len(result['selected_clip_proof'])==2 and all(x['animations']==['1d-combat_rg_sz5_az5_natk_kick_nwpn'] for x in result['selected_clip_proof'])
(R/'SELECTION_INPUTS.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps({'ranges':len(ranges),'rows':len(rows),'native_bytes':sum(len(x) for x in pieces),'pak_sha256':result['pak']['sha256']}))
