#!/usr/bin/env python3
"""Build a static clip->fragment association catalogue from supplied ADBs.
No animation names are evidence of accepted damage, actor role or kill eligibility.
"""
from pathlib import Path
import argparse,hashlib,json,xml.etree.ElementTree as ET

def build(paths):
    clips={};sources=[];variants=0
    for path in paths:
        data=path.read_bytes();root=ET.fromstring(data);source=path.name
        sources.append({'name':source,'sha256':hashlib.sha256(data).hexdigest()})
        fragments=root.find('FragmentList')
        if fragments is None:continue
        for kind in fragments:
            for variant in kind.findall('Fragment'):
                variants+=1
                proc=[p.attrib.get('type','') for p in variant.findall('./ProcLayer/Procedural') if p.attrib.get('type')]
                for layer_index,layer in enumerate(variant.findall('AnimLayer')):
                    for animation in layer.findall('Animation'):
                        name=animation.get('name','').strip()
                        if not name:continue
                        row={'source':source,'fragment':kind.tag,'guid':variant.get('GUID',''),'tags':variant.get('Tags',''),'fragment_tags':variant.get('FragTags',''),'relative_anim_layer':layer_index,'animation_flags':animation.get('flags',''),'procedural_types':proc}
                        bucket=clips.setdefault(name,[])
                        if row not in bucket:bucket.append(row)
    return {'schema':'KCD2VR_STATIC_CLIP_ASSOCIATIONS_1','status':'STATIC_CANDIDATES_NOT_RUNTIME_ACTION_IDENTITY','sources':sources,'variant_count':variants,'unique_clips':len(clips),'clips':dict(sorted(clips.items()))}
def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('adb',type=Path,nargs='+');ap.add_argument('--out',type=Path,required=True);args=ap.parse_args()
    result=build(args.adb);args.out.parent.mkdir(parents=True,exist_ok=True);args.out.write_text(json.dumps(result,separators=(',',':'))+'\n',encoding='utf8');print(result['unique_clips'],'unique names;',result['variant_count'],'fragment variants (not live states)')
if __name__=='__main__':main()
