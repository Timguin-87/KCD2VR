"""Build one gameplay PAK from C63 payloads plus scoped C63 map and opt-in throwing definitions; reject conflicting paths.
Legacy standalone map is intentionally NOT an input: its older duplicate startup
and item table conflict with the map already present in the current main PAK.
Language packs retain the engine-required Localization folder layout.
"""
from pathlib import Path
import argparse, hashlib, json, shutil, zipfile
import xml.etree.ElementTree as ET
ROOT=Path(__file__).resolve().parents[1]
def sha(data): return hashlib.sha256(data).hexdigest()
def collect(archives, loose=()):
    result={}; origins={}
    def add(name,data,origin):
        name=name.replace(chr(92),'/'); key=name.casefold()
        if name.startswith('/') or '..' in name.split('/'): raise ValueError('Unsafe member '+name)
        if key in result:
            if result[key][1]!=data: raise ValueError('Conflicting member '+name+' from '+origin)
            origins[key].append(origin);return
        result[key]=(name,data);origins[key]=[origin]
    for path in archives:
        with zipfile.ZipFile(path) as z:
            if z.testzip(): raise ValueError('Invalid CRC '+str(path))
            for info in z.infolist():
                if not info.is_dir(): add(info.filename,z.read(info),str(path))
    for name,path in loose: add(name,path.read_bytes(),str(path))
    return result,origins

def build(out):
    out=Path(out).resolve()
    if out.exists(): raise ValueError('Output must be new')
    # Never overwrite source inputs or the installed game.
    if ROOT in out.parents or 'steamapps' in [x.casefold() for x in out.parts]: raise ValueError('Choose an external staging directory')
    main=ROOT/'companion/Mods/KCD2VR'
    members,origins=collect([main/'Data/KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak',ROOT/'companion/Mods/KCD2VRDoors/Data/KCD2VR_Doors.pak',ROOT/'companion/optional_throwing_assets1/Mods/KCD2VRThrowingAssets/Data/KCD2VR_ThrowingAssets1.pak'],[
      ('Scripts/Startup/zzkcd2vr_lab.lua',ROOT/'companion/Mods/kcd2vr_lab/Scripts/Startup/zzkcd2vr_lab.lua')])
    # Explicit, reviewed source replacements only. All other parent members stay
    # byte-identical; collect() still rejects every accidental conflicting donor.
    replacements={
      'Scripts/Startup/zzkcd2vr_map.lua':ROOT/'mods/KCD2VRMap/map_runtime.lua',
      'Scripts/Startup/zzkcd2vr_doors.lua':ROOT/'mods/KCD2VRDoors/door_runtime.lua',
      'Scripts/KCD2VR/native_supply_t2.lua':ROOT/'src/vrik/native_supply_t2.lua',
      'Scripts/Startup/zzkcd2vr_throwing_assets1.lua':ROOT/'mods/KCD2VRThrowingAssets/startup63.lua'}
    replaced=[]
    for name,path in replacements.items():
        key=name.casefold()
        if key not in members: raise ValueError('Missing exact replacement '+name)
        old=members[key][1]; data=path.read_bytes()
        replaced.append({'member':name,'before':sha(old),'after':sha(data)})
        members[key]=(name,data);origins[key].append(str(path))
    tables=[]
    for key,(name,data) in members.items():
        if key.startswith('libs/tables/') and key.endswith('.xml'):
            tree=ET.fromstring(data)
            # Detect duplicated row identities WITHIN a native table; do not invent schema keys.
            count=0
            for group in tree:
                seen=set()
                for row in group:
                    identity=row.attrib.get('Id',row.attrib.get('id'))
                    if identity is not None:
                        value=(row.tag,identity)
                        if value in seen: raise ValueError('Duplicate row identity '+name+' '+str(value))
                        seen.add(value)
                    count+=1
            tables.append({'member':name,'rows':count})
    target=out/'Mods/KCD2VR';(target/'Data').mkdir(parents=True)
    pak=target/'Data/KCD2VR.pak'
    with zipfile.ZipFile(pak,'w',zipfile.ZIP_DEFLATED,compresslevel=6) as z:
        for key,(name,data) in sorted(members.items()):
            info=zipfile.ZipInfo(name,(2026,10,19,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED;z.writestr(info,data)
    with zipfile.ZipFile(pak) as z:
        assert z.testzip() is None and len(z.namelist())==len(members)
        for key,(name,data) in members.items(): assert z.read(name)==data,name
    manifest=ET.parse(main/'mod.manifest');info=manifest.getroot().find('info')
    info.find('version').text='66.178.66'
    info.find('description').text='C66 unified companion: preserved native combat tables, map, controls/dialogue; hand-owned native door animation protocol1966/revision66; gated lab protocol1 and finite opt-in throwing prototypes. Requires matching C66 ASI. Replaces older separate KCD2VR, KCD2VRDoors, KCD2VRMap, kcd2vr_lab and KCD2VRThrowingAssets installations. Gore remains separate.'
    manifest.write(target/'mod.manifest',encoding='utf-8',xml_declaration=True)
    shutil.copytree(main/'Localization',target/'Localization')
    # Throwing labels are separate extension members, not replacements of saved
    # HUD/dialogue translations. Keep one language PAK in the required directory.
    for donor in (ROOT/'companion/optional_throwing_assets1/Mods/KCD2VRThrowingAssets/Localization').glob('*.pak'):
        dst=target/'Localization'/donor.name
        payload,origin=collect(([dst] if dst.exists() else [])+[donor])
        existing=ROOT/'companion/C63_UNIFIED/Mods/KCD2VR/Localization'/donor.name
        if existing.is_file():
            with zipfile.ZipFile(existing) as old:
                original={n.casefold():(n,old.read(n)) for n in old.namelist() if not n.endswith('/')}
            if original==payload:
                shutil.copyfile(existing,dst) # preserve unchanged localization bytes/timestamps
                continue
        with zipfile.ZipFile(dst,'w',zipfile.ZIP_DEFLATED,compresslevel=6)as z:
            for _,(name,data) in sorted(payload.items()):z.writestr(name,data)
    shutil.copy2(ROOT/'tools/install_unified_companion63.ps1',out/'install_unified_companion63.ps1')
    report={'version':63,'candidate':66,'door_protocol':1966,'door_revision':66,'pak_sha256':sha(pak.read_bytes()),'members':len(members),'installed':False,
      'preserved_current_main_members':67,'explicit_source_replacements':replaced,'throwing_starter':'EXPLICIT_RIGHT_HIP_SUPPLY_ONLY_5_DAGGERS_3_AXES_NO_AUTOMATIC_BOOT_GRANT','rejected_donor':'Older standalone KCD2VRMap has conflicting startup/item definitions',
      'gated_lab_included':True,'native_tables':tables,
      'members_provenance':[{'path':name,'sha256':sha(data),'sources':origins[key]}for key,(name,data)in sorted(members.items())]}
    report['distribution_files']={str(p.relative_to(target)).replace(chr(92),'/'):sha(p.read_bytes()) for p in sorted(target.rglob('*')) if p.is_file() and p!=pak}
    (target/'KCD2VR_UNIFIED63.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    (out/'README.txt').write_text('C66 companion staging in the existing C63 distribution layout. Requires C66 ASI for hand-owned native door animation (protocol1966/revision66). One gameplay PAK plus required language resources. Replace older KCD2VR folder; remove/disable standalone KCD2VRDoors, KCD2VRMap, kcd2vr_lab, KCD2VRThrowingAssets and older overlapping companion PAKs before installing. Do not stack them. Keep INI/calibration. This package does not contain the ASI. Other native gameplay tables and resources are preserved; finite throwing stock stays opt-in. Gore remains separate. Nothing installed automatically.\n',encoding='utf-8')
    return report
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',required=True);a=p.parse_args()
    report=build(a.out);print(json.dumps({k:v for k,v in report.items()if k!='members_provenance'},indent=2))
