#!/usr/bin/env python3
"""R59.16C10R10: build the companion pak variants for the kick/ragdoll bisect (CLAUDE_TO_CODEX_C10R9: the shipped kick rows
had lost z_spatial_grid; regenerated from vanilla they carry it). Each variant = the working 7 members + a named set.
Output: companion/bisect/<name>/Mods/KCD2VR/Data/KCD2VR.pak + mod.manifest, and BISECT_MANIFEST.json with SHA-256 per member."""
from pathlib import Path
import hashlib,json,os,tempfile,zipfile,xml.etree.ElementTree as E
r=Path(__file__).resolve().parents[1]; src=r/'companion/source'
working=json.loads((r/'companion/WORKING_BASE_MEMBERS59C4.json').read_text())['members']
KICK_TABLES=['Libs/Tables/combat/combat_action_attack__kcdvr.xml','Libs/Tables/combat/combat_zone_distance__kcdvr.xml','Libs/Tables/combat/combat_attack_config__kcdvr.xml']
ADB=['Animations/Mannequin/ADB/kcd_male_combat_generated.adb']
MASTER=['Libs/Tables/combat/combat_action_perfect_block__kcdvr.xml','Libs/Tables/combat/combat_action_sync_perfect_block_hit__kcdvr.xml']
RAGDOLL=['Libs/Tables/action/hit_reaction_type__kcdvr.xml']
RPG=['Libs/Tables/rpg/rpg_param__kcdvr.xml']
variants={
 'WORKING_7':[],
 'KICK_A_tables_only':KICK_TABLES,
 'KICK_B_tables_plus_adb':KICK_TABLES+ADB,
 'KICK_C_plus_masterstrikes':KICK_TABLES+ADB+MASTER,
 'RAGDOLL_D_reaction_coef':RAGDOLL,
 'FULL_E_everything':KICK_TABLES+ADB+MASTER+RAGDOLL+RPG,
}
manifest={}
adb=(src/ADB[0]).read_text(encoding='utf-8')
for name,extra in variants.items():
    members=list(working)+[m for m in extra if m not in working]
    data={}
    for m in members:
        p=src/m
        if not p.is_file(): raise SystemExit('missing member %s'%m)
        b=p.read_bytes()
        if p.suffix.lower() in ('.xml','.adb'): E.fromstring(b)
        data[m]=b
    for m,b in data.items():
        if m.startswith('Libs/Tables/combat/combat_action_') and ADB[0] in data:
            import re
            for g in re.findall(rb'mn_fragment_guid="([^"]+)"',b):
                if ('GUID="%s"'%g.decode()) not in adb: raise SystemExit('row without fragment: %s %s'%(m,g.decode()))
    if 'combat_action_attack__kcdvr.xml' in ' '.join(members):
        b=data['Libs/Tables/combat/combat_action_attack__kcdvr.xml']
        if b.count(b'z_spatial_grid=')!=b.count(b'<combat_action_attack '): raise SystemExit('kick rows without z_spatial_grid')
    out=r/'companion/bisect'/name/'Mods/KCD2VR/Data/KCD2VR.pak'; out.parent.mkdir(parents=True,exist_ok=True)
    fd,tmp=tempfile.mkstemp(suffix='.tmp',dir=out.parent);os.close(fd)
    with zipfile.ZipFile(tmp,'w',compression=zipfile.ZIP_DEFLATED,compresslevel=9) as z:
        for m,b in data.items():
            info=zipfile.ZipInfo(m,(2026,10,14,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED;info.external_attr=0o644<<16;z.writestr(info,b)
    with zipfile.ZipFile(tmp) as z:
        if z.testzip() is not None or set(z.namelist())!=set(data): raise SystemExit('archive validation failed')
    os.replace(tmp,out)
    (out.parent.parent/'mod.manifest').write_text((r/'companion/Mods/KCD2VR/mod.manifest').read_text(encoding='utf-8').replace('<description>','<description>BISECT %s. '%name),encoding='utf-8')
    manifest[name]={'pak_sha256':hashlib.sha256(out.read_bytes()).hexdigest(),'members':{m:hashlib.sha256(b).hexdigest()[:16] for m,b in data.items()}}
(r/'companion/bisect/BISECT_MANIFEST.json').write_text(json.dumps(manifest,indent=1))
for k,v in manifest.items(): print(k,v['pak_sha256'][:16],len(v['members']),'members')
