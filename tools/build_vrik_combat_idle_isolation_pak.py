#!/usr/bin/env python3
"""Build an OPTIONAL VRIK A/B test patch that blanks only player-tagged CombatIdle clips.
No attacks, blocks, hit reactions, locomotion or procedural nodes are removed.
"""
from __future__ import annotations
import argparse, io, pathlib, xml.etree.ElementTree as ET, zipfile

def main() -> None:
    ap=argparse.ArgumentParser(); ap.add_argument('combat_database',type=pathlib.Path); ap.add_argument('output',type=pathlib.Path); a=ap.parse_args()
    tree=ET.parse(a.combat_database); root=tree.getroot(); group=root.find('./FragmentList/CombatIdle')
    if group is None: raise RuntimeError('CombatIdle not found')
    nf=na=0
    for frag in group.findall('Fragment'):
        tags=(frag.get('Tags') or '').split('+')
        if 'player' not in tags: continue
        touched=False
        for anim in frag.findall('.//Animation'):
            if anim.get('name'):
                anim.set('name',''); na+=1; touched=True
        if touched:nf+=1
    if nf!=25: raise RuntimeError(f'expected 25 current-retail player CombatIdle fragments, got {nf}')
    out=io.BytesIO(); tree.write(out,encoding='us-ascii',xml_declaration=True)
    a.output.parent.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(a.output,'w',compression=zipfile.ZIP_DEFLATED,compresslevel=9) as z:
        z.writestr('Animations/Mannequin/ADB/kcd_male_combat_database.adb',out.getvalue())
    print(f'wrote {a.output}: blanked {na} clips in {nf} player CombatIdle fragments only')
if __name__=='__main__': main()
