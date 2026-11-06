"""Reproducible read-only archive survey. No native code or donor binaries run."""
import argparse, hashlib, json, re, sqlite3, struct, zipfile
from pathlib import Path
from collections import Counter
from xml.etree import ElementTree as ET
from native_research112 import PE
from query_log_discovery113 import rows

def sha(data): return hashlib.sha256(data).hexdigest()
def filehash(path):
    with path.open('rb') as f: return hashlib.file_digest(f,'sha256').hexdigest()
def excerpts(data,pattern,cap=30,context=2):
    lines=data.decode('utf-8-sig',errors='replace').splitlines(); out=[]
    for i,line in enumerate(lines):
        if re.search(pattern,line,re.I):
            out.append(dict(line=i+1,context=[f'{j+1}: {lines[j]}' for j in range(max(0,i-context),min(len(lines),i+context+1))]))
            if len(out)>=cap:break
    return out
def member(z,n,pattern=None):
    b=z.read(n);d=dict(member=n,sha256=sha(b),bytes=len(b))
    if pattern:d['excerpts']=excerpts(b,pattern)
    return d
def xmlrows(z,n):return [dict(tag=e.tag,**e.attrib) for e in ET.fromstring(z.read(n)).iter() if e.attrib]
def combos(z,n):
    return [dict(**e.attrib,steps=[s.attrib for s in e.findall('./CombatComboSteps/CombatComboStep')]) for e in ET.fromstring(z.read(n)).iter('CombatCombo')]

def run(game,out,db):
    out.mkdir(parents=True,exist_ok=True);result={'scope':'Read-only installed retail assets, supplied mods, historical indexed logs; not live execution.'}
    def save(name,data): (out/name).write_text(json.dumps(data,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
    dll=game/'Bin/Win64MasterMasterSteamPGO/WHGame.dll';pe=PE(dll)
    assert pe.stamp==0x6A350E20 and sha(pe.data)=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
    contracts={}
    for rva,data in {0x7F4520:'488bc44889581048897018',0x7F4784:'488bc448895818555657',0xA7CF9E:'f30f100486',0xA7D125:'f20f11774c'}.items():
        actual=pe.read(rva,len(bytes.fromhex(data))).hex();assert actual==data,(hex(rva),actual)
        contracts[hex(rva)]=actual
    for slot,target in [(0x3A55BA0+0xC8,0x7F4520),(0x3A55BA0+0xD0,0x7F4784)]:
        assert struct.unpack('<Q',pe.read(slot,8))[0]==pe.base+target
    save('retail166.json',dict(path=str(dll),sha256=sha(pe.data),timestamp=hex(pe.stamp),contracts=contracts,
        inverse_producer='A7CF9E-A7D12A: conjugate orientation, optimized cross-product inverse translation. Validator now checks this pair directly, <=2 mm/0.002 rad; no native tolerance expansion.'))
    tz=zipfile.ZipFile(game/'Data/Tables.pak');sz=zipfile.ZipFile(game/'Data/Scripts.pak');az=zipfile.ZipFile(game/'Data/Animations.pak');gz=zipfile.ZipFile(game/'Data/IPL_GameData.pak')
    recipe='Libs/Tables/combat/CombatCombos__generated.xml';native=combos(tz,recipe)
    tables={}
    wanted=['combat_action_type.xml','combat_action_type_group.xml','combat_native_guard_zone.xml','combat_zone.xml','combat_attack_type.xml','combat_hit_type.xml','combat_damage_type_mapping.xml','weapon_class.xml','equipment_slot.xml','body_part.xml','body_subpart.xml','perk_combat_technique.xml']
    for n in tz.namelist():
        if n.split('/')[-1] in wanted:tables[n]=dict(**member(tz,n),rows=xmlrows(tz,n))
    save('combat_equipment166.json',dict(recipe=member(tz,recipe),combos=native,tables=tables))
    mods={}
    for name in ['simple_combos.pak','zweaponsbelt.pak','daggers_extended.pak']:
        p=game/'inspirations/mods'/name
        if not p.exists():p=game.parent/name
        if not p.exists():continue
        mz=zipfile.ZipFile(p);entry=dict(path=str(p),sha256=filehash(p),members=[])
        for n in mz.namelist():
            if not n.lower().endswith('.xml'):continue
            m=member(mz,n)
            if name=='simple_combos.pak' and n==recipe:
                changed=combos(mz,n);identity=lambda c:(c['Id'],c.get('RightWeapon'),c.get('LeftWeapon'))
                old={identity(c):c for c in native}
                m['changed_recipes']=[dict(id=c['Id'],before=old.get(identity(c)),after=c) for c in changed if old.get(identity(c))!=c]
            elif name=='zweaponsbelt.pak':
                original={x['id']:x for x in xmlrows(tz,'Libs/Tables/item/weapon_class.xml') if 'id' in x}
                m['changes']=[dict(id=r['id'],fields={k:dict(before=original.get(r['id'],{}).get(k),after=v) for k,v in r.items() if original.get(r['id'],{}).get(k)!=v}) for r in xmlrows(mz,n) if 'id' in r]
            elif name=='simple_combos.pak':m['rows']=xmlrows(mz,n)
            else:m['row_count']=len(xmlrows(mz,n))
            entry['members'].append(m)
        mods[name]=entry
    save('mod_deltas166.json',mods)
    scripts={}
    pattern=r'Physicalize|bRigidBody|OnUsed|OnSteal|OnUse|DoPlayAnimation|StartMinigame|SetPhysicParams|idConstr|OnDestroy|GetUsableFor|IsCrossCentering'
    for n in ['Scripts/Entities/Doors/AnimDoor.lua','Scripts/Entities/Items/PickableItem.lua','Scripts/Entities/Physics/Constraint.lua','Scripts/Entities/WH/Minigames/AlchemyItem.lua','Scripts/Entities/WH/Minigames/AlchemyTable.lua','Scripts/Entities/WH/Minigames/Smithery.lua']:
        scripts[n]=member(sz,n,pattern)
    save('interaction_scripts166.json',scripts)
    effects={}
    for n in ['Libs/MaterialEffects/SurfaceTypes.xml','Libs/MaterialEffects/MaterialEffects.xml','Libs/MaterialEffects/FXLibs/collisions.xml']:
        effects[n]=member(gz,n,r'hit_stopped_sword|sword.*slash|wood|stone|metal|spark|dust')
    save('material_effects166.json',effects)
    animation={}
    for n in az.namelist():
        if not n.lower().endswith(('.adb','.xml','.chrparams')):continue
        if any(w in n.lower() for w in ['wh_bow','kcd_bow','kcd_crossbow','wh_rifle','wh_ranged']):
            animation[n]=member(az,n,r'attach|arrow|bolt|reload|load|charge|grab|uncharge')
    objects={}
    for p in sorted((game/'Data').glob('IPL_Objects*.pak')):
        z=zipfile.ZipFile(p)
        for n in z.namelist():
            if n.lower().endswith(('bow_a.cdf','bow_rig.chrparams','bow_rig.chr')):
                b=z.read(n);m=member(z,n,r'Bow_|Arrow|BoneName|Asset_Bow') if not n.endswith('.chr') else member(z,n)
                if n.endswith('.chr'):m['joint_name_strings']=[s.decode() for s in re.findall(rb'[A-Za-z_][A-Za-z_0-9]{3,60}',b) if s.startswith((b'Bow_',b'Asset_',b'slt_',b'pck_',b'arw_'))]
                m['archive']=str(p);objects[n]=m
    save('bow_reload_assets166.json',dict(animation=animation,objects=objects))
    donor={}
    sources={'activeragdoll-master.zip':['src/main.cpp'], 'higgs-master.zip':['src/hand.cpp','src/constraint.cpp'],
        'DI2_UEVR-main.zip':['scripts/helpers/melee.lua','scripts/libs/gestures.lua'],
        'SH2_UEVR-main.zip':['scripts/melee.lua'],'crysis_vrmod-main.zip':['Code/VR/OpenXRInput.cpp','Code/Melee.cpp'],
        'Re9vr1.2.4-1017-1-2-4-1780522393.zip':['re9_vr_motion.lua','re9_vr_weapons.lua']}
    for name,ends in sources.items():
        p=game/'inspirations'/name
        if not p.exists():continue
        z=zipfile.ZipFile(p);items=[]
        for n in z.namelist():
            if any(n.lower().endswith(e.lower()) for e in ends):items.append(member(z,n,r'getPointVelocity|relativeVelocity|roomspace|roomSpace|cooldown|SetLinkedWeapon|RequestBeginStandardAttack|PerformMelee|requestDamage|ActivateSweep|constraint|Remove.*World|minimum.*speed|snap.turn'))
        donor[name]=dict(path=str(p),sha256=filehash(p),members=items)
    save('donor_sources166.json',donor)
    if db and db.exists():
        con=sqlite3.connect(db.resolve().as_uri()+'?mode=ro',uri=True)
        inventory=[dict(id=r[0],path=r[1],sha256=r[2],first=r[3],last=r[4],markers=json.loads(r[5])) for r in con.execute('SELECT id,path,sha256,first_stamp,last_stamp,build_markers FROM logs WHERE last_stamp>=? ORDER BY first_stamp',('2026-09-23',))]
        recent={r['path']:r for r in inventory}; findings={}
        for family in ['V66.98 GRIP','V66.98 BOW','V66.98 PHYSICAL','V66.92R1-COLLISION STATUS','V66.85-F3-COLLISION']:
            counts=Counter();examples=[]
            for log,line,text in rows(con,family):
                if log not in recent:continue
                counts[log]+=1
                if len(examples)<12:examples.append(dict(path=log,line=line,sha256=recent[log]['sha256'],text=text))
            findings[family]=dict(count_by_log=dict(counts),examples=examples)
        save('history_index166.json',dict(database=str(db),read_only=True,date_from='2026-09-23',indexed_logs=inventory,selected_families=findings,
            limitation='Indexed corpus ends at V109 on Oct 3. Later runs are covered by review/V66.115R1 onward and R4 audit; no claim that this database contains Oct 7. Family prefixes are not necessarily build markers.'))
        con.close()
    result.update(native_combos=len(native),script_files=len(scripts),donor_archives=len(donor),bow_assets=len(objects),animation_databases=len(animation))
    save('summary166.json',result);print(json.dumps(result))

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--game',type=Path,required=True);p.add_argument('--out',type=Path,required=True);p.add_argument('--db',type=Path)
    a=p.parse_args();run(a.game,a.out,a.db)
