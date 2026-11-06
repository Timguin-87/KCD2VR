"""Compile the separate VR recipe XML to bounded allocation-free C++ tables.

This is not WHGame's CombatCombos XML. Unknown syntax is an error, not a silently
ignored gameplay requirement. There is no native-memory or damage mutation here.
"""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse, hashlib, json, re, xml.etree.ElementTree as ET
ROOT=Path(__file__).resolve().parents[1]
REGIONS={x:x.title() for x in ('any','head','face','neck','torso','chest','abdomen','arm','leg','knee','ribs')}
FEATURES={x:''.join(s.title() for s in x.split('_')) for x in ('any','blade','tip','pommel','shield','fist','shield_edge','handle','weapon_head')}
MOTIONS={x:''.join(s.title() for s in x.split('_')) for x in ('any','cut','thrust','strike','blunt_strike','cut_after_deflection','bash','trip')}
def attrs(e,allowed,required=()):
    if set(e.attrib)-set(allowed):raise ValueError(f'{e.tag}: unknown attributes {set(e.attrib)-set(allowed)}')
    if not set(required)<=set(e.attrib):raise ValueError(f'{e.tag}: missing attributes')
    if e.text and e.text.strip():raise ValueError('unexpected element text')
    if e.tail and e.tail.strip():raise ValueError('unexpected tail text')
def boolean(v):
    if v not in ('true','false'):raise ValueError('invalid Boolean')
    return v
def compile_xml(data):
    if len(data)>131072 or b'<!DOCTYPE' in data or b'<!ENTITY' in data:raise ValueError('XML size/DTD forbidden')
    root=ET.fromstring(data);attrs(root,('version',),('version',))
    if root.tag!='VRBodyCombos' or root.attrib['version']!='1':raise ValueError('unsupported schema')
    if not 1<=len(root)<=64:raise ValueError('recipe count outside 1..64')
    out=['#pragma once','#include "../../vrik/body_combo59c7g.h"','namespace kcd2vr::bodycombo59c7g {',
         f'inline constexpr const char* sourceSha256="{hashlib.sha256(data).hexdigest()}";',
         f'inline constexpr std::array<Recipe,{len(root)}> recipes{{{{']
    names=set()
    for r in root:
        attrs(r,('name','technique','equipment','gapMs'),('name','technique','equipment'))
        if r.tag!='Recipe' or not 1<=len(r)<=6:raise ValueError('expected Recipe with 1..6 steps')
        name=r.attrib['name'];technique=r.attrib['technique'];equipment=r.attrib['equipment']
        if not re.fullmatch(r'[A-Za-z0-9_]{1,63}',name) or name in names:raise ValueError('invalid/duplicate name')
        names.add(name)
        if not re.fullmatch(r'[0-9a-f]{8}(-[0-9a-f]{4}){3}-[0-9a-f]{12}',technique):raise ValueError('invalid technique UUID')
        if equipment not in ('Longsword','Halberd','Shortsword','ShortswordShield','Blunt','BluntShield'):raise ValueError('invalid equipment group')
        gap=int(r.get('gapMs','1500'))
        if not 100<=gap<=3000:raise ValueError('gapMs outside 100..3000')
        steps=[]
        for s in r:
            attrs(s,('region','feature','motion','origin','twoHands','allowBlock'),('region','feature','motion'))
            if s.tag!='Step' or len(s):raise ValueError('expected empty Step')
            region=REGIONS[s.attrib['region']];feature=FEATURES[s.attrib['feature']];motion=MOTIONS[s.attrib['motion']]
            origin=int(s.get('origin','-1'))
            if origin not in (-1,0,1,2,5):raise ValueError('invalid guard origin')
            two=boolean(s.get('twoHands','false'));block=boolean(s.get('allowBlock','false'))
            if block=='true' and region!='Any':raise ValueError('a blocked contact cannot prove a body region')
            steps.append(f'{{Region::{region},Feature::{feature},Motion::{motion},{origin},{two},{block}}}')
        out.append('  Recipe{'+','.join(map(json.dumps,(name,technique,equipment)))+',{{'+','.join(steps)+'}},'+str(len(steps))+','+str(gap)+'},')
    out+=['}};','} // namespace','']
    return '\n'.join(out)
def main():
    p=argparse.ArgumentParser();p.add_argument('--check',action='store_true');a=p.parse_args()
    path=ROOT/'data/VRBodyCombos.xml';dst=ROOT/'src/subsystems/masterstrike_combos/body_recipes.h'
    text=compile_xml(path.read_bytes())
    if a.check:
        if not dst.exists() or read_source(dst)!=text:raise SystemExit('body combo header is stale; regenerate with this tool')
    else:dst.write_text(text,encoding='utf-8')
    print('VRBodyCombos: schema and generated header OK')
if __name__=='__main__':main()
