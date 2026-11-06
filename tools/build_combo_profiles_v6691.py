#!/usr/bin/env python3
"""Compare the uploaded Simple Combos PAK to retained supplied-retail recipes.
Does not install a mod or infer effective game load order. Standard library only.
"""
from __future__ import annotations
from pathlib import Path
import argparse,collections,hashlib,json,sys,zipfile,xml.etree.ElementTree as ET
ROOT=Path(__file__).resolve().parents[1]
MEMBER='Libs/Tables/combat/CombatCombos__generated.xml'
PARAM='Libs/Tables/rpg/rpg_param__simple_combos.xml'
def digest(b):return hashlib.sha256(b).hexdigest()
def key(row):
    a=row['recipe'];return '|'.join(a.get(k,'') for k in ('RightWeapon','LeftWeapon','Id','CombatTechniqueId'))
def parse(data):
    parser=ET.XMLParser(target=ET.TreeBuilder(insert_comments=True));root=ET.fromstring(data,parser=parser)
    rows=[];comment=None
    for el in root.find('CombatCombos'):
        if el.tag is ET.Comment:comment=(el.text or '').strip();continue
        if el.tag!='CombatCombo':continue
        rows.append({'recipe':dict(el.attrib),'steps':[dict(s.attrib) for s in el.findall('CombatComboSteps/CombatComboStep')],'source_comment':comment,'physical_sequence':None,'physical_sequence_status':'NOT_DECODED_FROM_DIRECTION_RECIPE'});comment=None
    if len({key(x) for x in rows})!=len(rows):raise ValueError('duplicate weapon/technique-qualified recipe')
    return rows

def build(pak:Path,retail_path:Path,tables:Path|None=None):
    retail=json.loads(retail_path.read_text());base={key(x):x for x in retail}
    with zipfile.ZipFile(pak) as z:
        members=z.namelist();data=z.read(MEMBER);simple=parse(data);params=ET.fromstring(z.read(PARAM))
        manifest={'archive_name':pak.name,'archive_sha256':digest(pak.read_bytes()),'members':[{'name':n,'sha256':digest(z.read(n)),'bytes':len(z.read(n))} for n in members]}
    mod={key(x):x for x in simple};diffs=[]
    for k in sorted(set(base)|set(mod)):
        a,b=base.get(k),mod.get(k)
        if not a or not b:diffs.append({'key':k,'status':'ADDED' if b else 'REMOVED','retail':a,'simple':b});continue
        if a['recipe']!=b['recipe'] or a['steps']!=b['steps']:
            diffs.append({'key':k,'status':'CHANGED','recipe_before':a['recipe'],'recipe_after':b['recipe'],'steps_before':a['steps'],'steps_after':b['steps'],'step_count_before':len(a['steps']),'step_count_after':len(b['steps']),'final_step_identical':a['steps'][-1]==b['steps'][-1]})
        b['technique_perk_link']=a.get('technique_perk_link');b['perk']=a.get('perk')
    overrides=[dict(x.attrib) for x in params.findall('.//rpg_param')]
    base_params=[]
    if tables:
        with zipfile.ZipFile(tables) as z:
            data=z.read('Libs/Tables/rpg/rpg_param.xml')
            base_params=[dict(x.attrib) for x in ET.fromstring(data).findall('.//rpg_param') if x.get('rpg_param_key') in {p['rpg_param_key'] for p in overrides}]
    summary={'retail_recipes':len(retail),'simple_recipes':len(simple),'changed':sum(d['status']=='CHANGED' for d in diffs),'added':sum(d['status']=='ADDED' for d in diffs),'removed':sum(d['status']=='REMOVED' for d in diffs),'unchanged':len(set(base)&set(mod))-sum(d['status']=='CHANGED' for d in diffs),'step_lengths_retail':dict(sorted(collections.Counter(len(x['steps']) for x in retail).items())),'step_lengths_simple':dict(sorted(collections.Counter(len(x['steps']) for x in simple).items()))}
    return {'schema':1,'status':'OFFLINE_SUPPLIED_DATA_NOT_EFFECTIVE_RUNTIME_LOAD_ORDER','profile_default':'unknown','source':manifest,'retail_recipe_index_sha256':digest(retail_path.read_bytes()),'summary':summary,'parameters':{'supplied_retail':base_params,'simple_overrides':overrides,'native_effect_not_inferred':True},'profiles':{'supplied-retail':retail,'simple-combos':simple},'differences':diffs,'limits':['No PAK is installed by this tool.','Recipe step count is not proof of successful physical hits.','ComboMinSteps is retained as a raw mod setting, not all recipes become two-step.','C12 or another name is ambiguous without weapon and technique.','Native raw selector and GUID encodings remain unresolved.']}

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('simple_pak',type=Path);p.add_argument('--retail-index',type=Path,default=ROOT/'continuity/semantic_index/combo/combo_recipes.json');p.add_argument('--tables',type=Path);p.add_argument('--out',type=Path,default=ROOT/'continuity/semantic_index/combo/recipe_profiles.json');a=p.parse_args()
    try:
        result=build(a.simple_pak,a.retail_index,a.tables);a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result['summary'],indent=2));print(json.dumps(result['parameters'],indent=2))
    except (OSError,ValueError,KeyError,zipfile.BadZipFile,ET.ParseError) as e:print('Profile build failed:',e,file=sys.stderr);return 1
    return 0
if __name__=='__main__':raise SystemExit(main())
