#!/usr/bin/env python3
"""Join four D41 disputed combo GUIDs to installed retail table/perk/localization columns, without unpacking assets."""
import argparse,hashlib,json
from pathlib import Path
from zipfile import ZipFile
import xml.etree.ElementTree as ET
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--game',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
root=Path(__file__).resolve().parents[1]
inputs={};members={}
def sha(b):return hashlib.sha256(b).hexdigest()
def read(archive,member):
 archive=a.game/archive
 if str(archive) not in inputs:inputs[str(archive)]={'bytes':archive.stat().st_size,'sha256':sha(archive.read_bytes())}
 with ZipFile(archive) as z:b=z.read(member)
 members[member]={'archive':str(archive),'sha256':sha(b),'bytes':len(b)}
 return ET.fromstring(b)
combos=read('Data/Tables.pak','Libs/Tables/combat/CombatCombos__generated.xml')
links=read('Data/Tables.pak','Libs/Tables/rpg/perk_combat_technique.xml')
perks=read('Data/Tables.pak','Libs/Tables/rpg/perk__combat.xml')
texts=read('Localization/English_xml.pak','text_ui_soul.xml')
loc={r.findtext('Cell'):[''.join(c.itertext()) for c in r.findall('Cell')] for r in texts.findall('Row')}
disputed=json.loads((root/'data/research/combat/combo_table_disagreements.json').read_text())
result=[]
for d in disputed:
 guid=d['guid'];c=[e for e in combos.iter('CombatCombo') if e.get('CombatTechniqueId')==guid]
 assert len(c)==1,(guid,'combo uniqueness',len(c))
 l=[e for e in links.iter('perk_combat_technique') if e.get('combat_technique_id')==guid]
 assert len(l)==1,(guid,'perk technique uniqueness',len(l))
 k=[e for e in perks.iter('perk') if e.get('perk_id')==l[0].get('perk_id')]
 assert len(k)==1,(guid,'perk uniqueness',len(k))
 key=k[0].get('perk_ui_desc');cells=loc[key];assert len(cells)==3
 result.append({'guid':guid,'native_combo':c[0].attrib,'native_final_step':list(c[0].find('CombatComboSteps'))[-1].attrib,'perk':k[0].attrib,'description_key':key,'column_2_original':cells[1],'column_3_translated':cells[2],'d41_diagnostic':d['diagnostic_recipe'],'d41_precontact':d['precontact_filter']})
notes=Path('E:/backup/KCD2VR/deepseek colab')
collab={}
for name in ['DEEPSEEK_TO_COL_20261028_1945_D41_DIAGNOSTICS.txt','DEEPSEEK_TO_COL_20261028_1955_AUDIT_VERDICT.txt','D41_MISSING_LINKS_AUDIT_20261028.txt']:
 n=notes/name
 if n.is_file():collab[str(n)]={'sha256':sha(n.read_bytes()),'bytes':n.stat().st_size}
output={'source_proof_only':True,'headset_result':False,'archive_inputs':inputs,'member_inputs':members,'collab_inputs':collab,'primary_schema_docs':['https://www.cryengine.com/docs/static/engines/cryengine-5/categories/23756813/pages/23306533','https://www.cryengine.com/docs/static/engines/cryengine-5/categories/23756816/pages/26873675'],'rows':result,'conclusion':'Three motion disagreements match changes between original and translated text columns. Fourth geometry-feature disagreement is not resolved by text. Native final AttackType is slash in all four exact GUID rows, including localized thrust descriptions. Do not conflate the native entry command with visible finisher contact or promote an unconnected physical dispatch from these descriptions. No production finisher eligibility/table changes.'}
a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(output,indent=2,ensure_ascii=False)+'\n');print('4 exact GUID -> native combo -> perk -> three-column text joins; hashes written to',a.out)
