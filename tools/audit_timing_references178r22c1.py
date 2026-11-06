#!/usr/bin/env python3
"""Compare the two supplied timing mods with this installation; read-only, no dependencies."""
import argparse, collections, hashlib, json, math, zipfile
from pathlib import Path
from xml.etree import ElementTree as ET

def sha(b): return hashlib.sha256(b).hexdigest()
def equivalent(a,b):
    if a==b:return True
    try:return math.isclose(float(a),float(b),rel_tol=1e-5,abs_tol=1e-6)
    except (TypeError,ValueError):return False

def audit(root):
    mods=root/'inspirations/mods'
    fast=mods/'fast_cmb_anims.pak';better=mods/'yefreitor_bettercombat.pak'
    result={'input_sha256':{str(p):sha(p.read_bytes()) for p in (fast,better)},
            'numeric_comparison':{'relative_tolerance':1e-5,'absolute_tolerance':1e-6},'members':[]}
    with zipfile.ZipFile(root/'Data/Tables.pak') as tables, zipfile.ZipFile(root/'Data/Animations.pak') as anims, zipfile.ZipFile(fast) as mod:
        retail={n.lower():(z,n) for z in (tables,anims) for n in z.namelist()}
        for name in mod.namelist():
            z,n=retail[name.lower()];before=z.read(n);after=mod.read(name)
            x=ET.fromstring(before);y=ET.fromstring(after)
            raw=collections.Counter();semantic=collections.Counter();examples={}
            xx=list(x.iter());yy=list(y.iter())
            assert len(xx)==len(yy) and all(a.tag==b.tag for a,b in zip(xx,yy)),name
            for a,b in zip(xx,yy):
                for k in set(a.attrib)|set(b.attrib):
                    old,new=a.get(k),b.get(k)
                    if old==new:continue
                    key=a.tag+'@'+k;raw[key]+=1
                    if k=='timestamp':continue
                    if a.tag=='Animation' and k=='speed' and old is None:old='1'
                    if equivalent(old,new):continue
                    semantic[key]+=1;examples.setdefault(key,{'before':old,'after':new,'guid':a.get('GUID') or a.get('mn_fragment_guid')})
            row={'member':name,'retail_sha256':sha(before),'mod_sha256':sha(after),'same_tree_order':True,
                 'raw_attribute_changes':dict(sorted(raw.items())),
                 'numeric_or_text_changes_excluding_timestamp':dict(sorted(semantic.items())),
                 'examples':dict(sorted(examples.items()))}
            if name.endswith('.adb'):
                a={e.get('GUID'):e for e in x.iter('Fragment')};b={e.get('GUID'):e for e in y.iter('Fragment')}
                assert a.keys()==b.keys()
                row['fragment_count']=len(a);row['animation_count']=len(list(y.iter('Animation')))
                row['speed_values']=dict(collections.Counter(e.get('speed','1') for e in y.iter('Animation')))
            else:
                row['rows']=len(x[0]);row['player_attribute_values']=dict(collections.Counter(e.get('player','UNSPECIFIED') for e in y[0]))
                row['player_attribute_caveat']='A false selector is not proved to mean NPC-exclusive.'
            result['members'].append(row)
        rp=ET.fromstring(tables.read('Libs/Tables/rpg/rpg_param.xml'))
        defaults={e.get('rpg_param_key'):e.get('rpg_param_value') for e in rp.iter('rpg_param')}
        with zipfile.ZipFile(better) as z:
            result['ai_parameters']=[{'key':e.get('rpg_param_key'),'mod':e.get('rpg_param_value'),
                'base_rpg_param_xml':defaults.get(e.get('rpg_param_key'))} for e in ET.fromstring(z.read(z.namelist()[0])).iter('rpg_param')]
    result['limits']=['Missing base XML keys are unknown; author comments are not validated defaults.',
        'Shared ADB/table changes are not Henry-only or dynamically driven by a tracked strike.',
        'This verifies asset differences, not native runtime clock ownership or gameplay. No files installed.']
    return result

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('game_root',type=Path);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
    result=audit(a.game_root);a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('PASS: compared',len(result['members']),'timing assets and',len(result['ai_parameters']),'AI parameters; no installation or native execution')
