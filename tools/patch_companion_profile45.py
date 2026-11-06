"""C45: put companion/source/Libs/Config/defaultProfile.xml into the shipped companion pak.
The shipped KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak still carried the pre-C22b profile (chat_init_with_focus =
xi_shoulderl only), so the HUD "Quick talk" prompt showed LB while the VR mod sends X. Every other member is kept byte
for byte (same recipe as build_dialogue_companion43.py: replace members, verify, write a manifest). Offline; nothing is
installed. Run from anywhere: python tools/patch_companion_profile45.py [--pak <pak>] [--check]
"""
from pathlib import Path
import argparse,hashlib,json,os,tempfile,zipfile,xml.etree.ElementTree as E
R=Path(__file__).resolve().parents[1]
PROFILE='Libs/Config/defaultProfile.xml'
def validate(profile):
    root=E.fromstring(profile)
    maps={m.get('name'):m for m in root.iter('actionmap')}
    focus=maps['chat_init_interactor'].find("action[@name='chat_init_with_focus']")
    if focus is None or focus.get('xboxpad')!='xi_x' or focus.find('xboxpad') is not None:raise ValueError('chat_init_with_focus must be xi_x only (the shown Quick talk glyph)')
    for m,a in (('chat_init_interactor','chat_init_interactor_press'),('chat_init','chat_init_press'),('chat_init','chat_init_release')):
        inputs=[i.get('input') for i in maps[m].find("action[@name='%s']"%a).find('xboxpad').iter('inputdata')]
        if inputs!=['xi_x','xi_shoulderr']:raise ValueError('%s: expected xi_x first then xi_shoulderr, got %s'%(a,inputs))
    base=maps['reading_base']
    if base.find("action[@name='read_prev']").get('xboxpad')!='xi_shoulderl' or base.find("action[@name='read_next']").get('xboxpad')!='xi_shoulderr':
        raise ValueError('reading_base LB/RB must stay native (the mod pulses them)')
def main():
    a=argparse.ArgumentParser(description=__doc__)
    a.add_argument('--pak',type=Path,default=R/'companion/Mods/KCD2VR/Data/KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak')
    a.add_argument('--check',action='store_true',help='only verify that the pak already carries the source profile')
    p=a.parse_args()
    profile=(R/'companion/source'/PROFILE).read_bytes();validate(profile)
    with zipfile.ZipFile(p.pak) as z:
        if z.testzip():raise ValueError('base CRC')
        names=z.namelist()
        if len(names)!=len(set(names)):raise ValueError('duplicate member')
        members={n:z.read(n) for n in names}
    if PROFILE not in members:raise ValueError('pak has no '+PROFILE)
    before=hashlib.sha256(members[PROFILE]).hexdigest();after=hashlib.sha256(profile).hexdigest()
    if p.check:
        if before!=after:raise SystemExit('pak profile %s != source profile %s'%(before[:12],after[:12]))
        print(json.dumps({'pak':str(p.pak),'profile_sha256':after,'in_sync':True}));return
    members[PROFILE]=profile
    fd,tmp=tempfile.mkstemp(prefix=p.pak.name,suffix='.tmp',dir=p.pak.parent);os.close(fd)
    try:
        with zipfile.ZipFile(tmp,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
            for n in names:
                info=zipfile.ZipInfo(n,(2026,10,17,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED;info.external_attr=0o644<<16
                z.writestr(info,members[n])
        with zipfile.ZipFile(tmp) as z:
            if z.testzip() is not None or z.namelist()!=names:raise ValueError('archive validation failed')
            for n in names:
                if z.read(n)!=members[n]:raise ValueError('member mismatch '+n)
        os.replace(tmp,p.pak)
    finally:
        if os.path.exists(tmp):os.unlink(tmp)
    report={'pak_sha256':hashlib.sha256(p.pak.read_bytes()).hexdigest(),'members':len(names),
            'changed':{PROFILE:{'before':before,'after':after}},
            'scope':'C45: defaultProfile.xml only (Quick Speak prompt = X; chat_init_with_focus xi_x only). All other members byte-identical.',
            'headset_acceptance':'NOT_RUN','installed':False}
    p.pak.with_suffix('.C45.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))
if __name__=='__main__':main()
