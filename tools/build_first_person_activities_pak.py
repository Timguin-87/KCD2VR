#!/usr/bin/env python3
"""Build KCD2VR's current-retail first-person activity Mannequin patch.

The patch is intentionally narrow: it mirrors the blank-animation technique from
FirstPersonDialogsAndMercy, but does NOT blank ADLG_Cameras or special dialogue
camera fragments. Input must be the current retail Animations/Mannequin/ADB dir.
"""
from __future__ import annotations
import argparse, io, pathlib, xml.etree.ElementTree as ET, zipfile, json, hashlib

MALE_GROUPS = {
    'BathDialog','CarryItemPickup','CombatAttackMercy','CookingPanCamp','CookingPanKitchen',
    'Digging','Drying','FishingDialog','LaundryWash','Leaning_Back_Loop','Leaning_Left_Loop',
    'Leaning_Right_Loop','PickingHerbs','PickingRose','SharpeningDialog','SharpeningDialog_VAR',
    'ShortButchering','SmithForgingDialog','SmithForgingDialog_VAR','SmithHeatingDialog',
    'SmithHeatingDialog_VAR','SmokehouseSniff','StomachPainIdle','StomachPainIdleDialog',
    'SweepingFloorDialog'
}
FEMALE_GROUPS = {'HealerSittingTea','HealerSittingTeaDialog','MotionIdle','PickingHerbs','SweepingFloorDialog'}

def patch(path: pathlib.Path, groups: set[str]) -> tuple[bytes,int,int]:
    tree=ET.parse(path); root=tree.getroot(); fl=root.find('FragmentList')
    if fl is None: raise RuntimeError(f'FragmentList missing in {path}')
    fragments=animations=0
    for group in list(fl):
        if group.tag not in groups: continue
        for frag in group.findall('Fragment'):
            touched=False
            anims=frag.findall('.//Animation')
            # The reference mod leaves the first two female PickingHerbs layers intact
            # and blanks only its third authored clip; mirror that exact exception.
            indexes=range(len(anims))
            if path.name=='wh_female_dialog_cameras.adb' and group.tag=='PickingHerbs': indexes=[2] if len(anims)>2 else []
            for i in indexes:
                anim=anims[i]
                # Blank the authored clip but preserve fragment GUID/tags/blends/procedural layers.
                if anim.get('name'):
                    anim.set('name',''); animations+=1; touched=True
            if touched: fragments+=1
    out=io.BytesIO(); tree.write(out,encoding='us-ascii',xml_declaration=True)
    return out.getvalue(),fragments,animations

def patch_body_camera(path: pathlib.Path) -> tuple[bytes,int,int]:
    tree=ET.parse(path);root=tree.getroot();fl=root.find('FragmentList')
    if fl is None:raise RuntimeError(f'FragmentList missing in {path}')
    fragments=procedures=0
    for group in fl:
        if group.tag not in MALE_GROUPS:continue
        for frag in group.findall('Fragment'):
            touched=False
            for proc in frag.findall('.//Procedural'):
                if proc.get('type') in {'CameraBoneAnimated','AnimateCamera'}:
                    proc.set('type','');procedures+=1;touched=True
            fragments+=int(touched)
    out=io.BytesIO();tree.write(out,encoding='us-ascii',xml_declaration=True)
    return out.getvalue(),fragments,procedures

def verify_camera_only(before: bytes,after: bytes,body: bool=False) -> int:
    a=ET.fromstring(before);b=ET.fromstring(after);changes=0
    def walk(x,y,group=''):
        nonlocal changes
        if x.tag!=y.tag or len(x)!=len(y):raise RuntimeError('Structure changed')
        if x.tag=='FragmentList':group=''
        if x.tag in MALE_GROUPS|FEMALE_GROUPS:group=x.tag
        expected=dict(x.attrib)
        if body and x.tag=='Procedural' and group in MALE_GROUPS and x.get('type') in {'CameraBoneAnimated','AnimateCamera'}:expected['type']=''
        if not body and x.tag=='Animation' and x.get('name') and y.get('name')=='':
            if group not in MALE_GROUPS|FEMALE_GROUPS:raise RuntimeError('Camera change outside selected activities')
            expected['name']=''
        if y.attrib!=expected:raise RuntimeError(f'Unexpected attribute change {group}/{x.tag}')
        changes+=int(x.attrib!=y.attrib)
        for xc,yc in zip(x,y):walk(xc,yc,group)
    walk(a,b)
    return changes

def main() -> None:
    ap=argparse.ArgumentParser(); ap.add_argument('adb_dir',type=pathlib.Path); ap.add_argument('output',type=pathlib.Path); a=ap.parse_args()
    inputs=[('kcd_dialog_cameras.adb',MALE_GROUPS),('wh_female_dialog_cameras.adb',FEMALE_GROUPS)]
    payload=[]; totalf=totala=0;report={}
    for name,groups in inputs:
        data,nf,na=patch(a.adb_dir/name,groups); payload.append((name,data)); totalf+=nf; totala+=na
        report[name]={'input_sha256':hashlib.sha256((a.adb_dir/name).read_bytes()).hexdigest(),'output_sha256':hashlib.sha256(data).hexdigest(),'camera_clips_blanked':verify_camera_only((a.adb_dir/name).read_bytes(),data),'fragments':nf}
        print(f'{name}: blanked {na} Animation entries in {nf} fragments')
    body_name='kcd_male_database.adb'
    body,nf,na=patch_body_camera(a.adb_dir/body_name);payload.append((body_name,body))
    report[body_name]={'input_sha256':hashlib.sha256((a.adb_dir/body_name).read_bytes()).hexdigest(),'output_sha256':hashlib.sha256(body).hexdigest(),'camera_procedures_blanked':verify_camera_only((a.adb_dir/body_name).read_bytes(),body,True),'fragments':nf,'body_animations_preserved':True}
    a.output.parent.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(a.output,'w',compression=zipfile.ZIP_DEFLATED,compresslevel=9) as z:
        for name,data in payload: z.writestr(f'Animations/Mannequin/ADB/{name}',data)
    a.output.with_suffix('.json').write_text(json.dumps(report,indent=2)+'\n')
    print(f'wrote {a.output}: {totala} camera clips / {totalf} fragments; {na} camera procedures; ADLG_Cameras and combat body DB untouched')
if __name__=='__main__': main()
