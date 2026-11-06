"""Actual retail generator + in-memory companion overlay; never builds a PAK."""
import argparse
import copy
import hashlib
import importlib.util
import json
from pathlib import Path
import sys
import xml.etree.ElementTree as ET

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'src/subsystems/throw_drop/resources'))
import companion_patch_t18 as p
from build_asset_pack import tables_for, validate, localization, parse, xml


def run(xml_out=None):
    game=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2')
    checks=[]
    def ck(ok,label):
        assert ok,label
        checks.append(label)
    def refused(members,label):
        try:p.plan_members(members,native)
        except p.base.PackageError as error:
            checks.append(label)
            return str(error)
        raise AssertionError(label+' accepted')
    native=p.Native(game)
    unified=ROOT/'companion/C63_UNIFIED'
    files=p.base.read_tree(unified)
    members=p.base.read_pak(files[p.base.PAK])
    immutable={n:hashlib.sha256(data).hexdigest() for n,(_,data) in members.items()}
    definition_proof=p.base.validate_definitions(members,files)
    ck(definition_proof['localization_packages']>=16,'actual unified legacy models/localization/finite receipts resolve')
    ck(len(members)==84,'actual current parent has84 members')
    ck(not any('native_ammo_t18' in n for n in members),'actual current parent has no projectile-role table')
    generated=tables_for(native,('kcd2vr_throwing_dagger1','kcd2vr_throwing_axe1'))
    native_checks=validate(native,generated,('kcd2vr_throwing_dagger1','kcd2vr_throwing_axe1'))
    ck(bool(native_checks),'actual generated final tables pass retail schema and native target references')
    labels={row.find('Cell').text for row in parse(localization()).iter('Row')}
    roles=p.roles.rows(native)
    for row in roles[:2]:
        legacy,broken,base,cls=p.roles.BROKEN_ROLES[row.get('Name')]
        ck(row.tag=='Ammo' and row.get('Type')=='1','native Arrow1 schema retained: '+row.get('Name'))
        ck(row.get('BrokenItemClassId')==broken,'exact existing legacy BrokenGUID: '+row.get('Name'))
        ck(broken!=row.get('Id') and broken not in native.all_guids,'not self GUID or retail target: '+row.get('Name'))
        ck(row.get('Model')==native.items[base].get('Model'),'native model unchanged: '+row.get('Name'))
        model='Objects/'+row.get('Model')
        if not model.lower().endswith(('.cgf','.cdf','.chr')):model+='.cgf'
        ck(native.exists(model),'actual retail CGF exists: '+row.get('Name'))
        ck(row.get('UIName') in labels and row.get('UIInfo') in labels,'actual generator labels resolve: '+row.get('Name'))

    # Frozen RED: run the actual prior generator against the SAME retail input.
    frozen=ROOT/'tests/subsystems/throw_drop/fixtures/broken_guid_t20/frozen_ammo_roles_t17.py'
    ck(hashlib.sha256(frozen.read_bytes()).hexdigest()=='f9931f9b3582fddb59063d2c3c08eddf1819ee0cb21c1ebf04a6cc3a04016732','frozen old generator bytes unchanged')
    old_receipt=frozen.with_name('POST_IMPACT_BROKEN_GUID_OLD.json')
    ck(hashlib.sha256(old_receipt.read_bytes()).hexdigest()=='9ce22c6588da1b06f0119fcf5743fde59d56f2358de6ac465b7e3b8d7f308cd2','frozen old eight-pin receipt bytes unchanged')
    spec=importlib.util.spec_from_file_location('frozen_broken_guid_t20',frozen)
    old=importlib.util.module_from_spec(spec);spec.loader.exec_module(old)
    prior=old.rows(native)
    ck(all(r.get('BrokenItemClassId') is None for r in prior[:2]),'RED old actual generator omits both BrokenGUIDs')
    try:p.roles.validate(native,prior)
    except AssertionError as error:
        ck('missing/wrong finite BrokenGUID' in str(error),'RED old rows rejected specifically for missing finite BrokenGUID')
        red_reason=str(error)
    else:raise AssertionError('frozen old broken roles unexpectedly satisfy current finite policy')
    stripped=copy.deepcopy(roles)
    for row in stripped[:2]:row.attrib.pop('BrokenItemClassId')
    ck([p.canonical(r) for r in stripped]==[p.canonical(r) for r in prior],'all old attributes/children unchanged except two BrokenGUIDs')

    after,changed=p.plan_members(members,native)
    ck(changed=={p.base.SUPPLY_MEMBER,p.LUA,p.TABLE},'initial overlay changes only three declared owned paths')
    preserved=[n for n in members if n not in changed]
    ck(len(preserved)==83,'all83 unrelated actual parent payloads preserved')
    ck(all(after[n][1]==members[n][1] for n in preserved),'all83 unrelated payloads byte-identical')
    ck(len(after)==86,'overlay adds only supply module and role table')
    p.base.validate_definitions(after,files)
    ck(True,'actual final overlay preserves saved definitions and external localization')
    again,changes=p.plan_members(after,native)
    ck(not changes and {n:d for n,(_,d) in again.items()}=={n:d for n,(_,d) in after.items()},'initial overlay idempotent')

    def mutation(source,member,fn):
        result=copy.deepcopy(source);root=ET.fromstring(result[member][1]);fn(root)
        result[member]=(result[member][0],xml(root))
        return result
    def ammo(root,index=0):return list(root.iter('Ammo'))[index]
    def missing_both(root):
        for row in root.iter('Ammo'):row.attrib.pop('BrokenItemClassId')
    old_overlay=mutation(after,p.TABLE,missing_both)
    upgraded,upgrade_changes=p.plan_members(old_overlay,native)
    ck(upgrade_changes=={p.TABLE},'known frozen old set upgrades only owned role table')
    ck(all(upgraded[n][1]==old_overlay[n][1] for n in old_overlay if n!=p.TABLE),'upgrade preserves all85 other current payloads')
    ck({n:d for n,(_,d) in upgraded.items()}=={n:d for n,(_,d) in after.items()},'old-set upgrade produces exact fresh expected table')
    ck(not p.plan_members(upgraded,native)[1],'old-set upgrade idempotent')
    refused(mutation(after,p.TABLE,lambda r:r.find('ItemClasses').remove(r.find('ItemClasses')[0])),'partial native role set refused')
    refused(mutation(after,p.TABLE,lambda r:ammo(r).set('AttackStab','9999')),'changed damage refused')
    refused(mutation(old_overlay,p.TABLE,lambda r:ammo(r).set('AttackStab','9999')),'old missing GUID plus unrelated attribute change refused')
    refused(mutation(old_overlay,p.TABLE,lambda r:next(r.iter('MissileWeapon')).set('Model','other/model')),'old missing GUID plus changed launcher refused')
    refused(mutation(after,p.TABLE,lambda r:ammo(r).attrib.pop('BrokenItemClassId')),'unknown mixed current/old set refused')
    for index in (0,1):
        for label,value in [('self',roles[index].get('Id')),('retail',native.items['arrow_enh_cutting'].get('BrokenItemClassId')),('zero','00000000-0000-0000-0000-000000000000'),('other family',roles[1-index].get('BrokenItemClassId'))]:
            refused(mutation(after,p.TABLE,lambda r,i=index,v=value:ammo(r,i).set('BrokenItemClassId',v)),label+' BrokenGUID refused family'+str(index))
    duplicate=copy.deepcopy(after);duplicate['Libs/Tables/item/item__duplicate.xml']=duplicate[p.TABLE]
    refused(duplicate,'duplicate native role table refused')
    displaced=copy.deepcopy(old_overlay);displaced['Libs/Tables/item/item__other.xml']=displaced.pop(p.TABLE)
    refused(displaced,'old roles outside exact owned table refused')
    refused(mutation(after,p.TABLE,lambda r:r.find('ItemClasses').set('version','9')),'wrong current table version refused')
    refused(mutation(old_overlay,p.TABLE,lambda r:r.find('ItemClasses').set('version','9')),'unknown old table version refused')
    refused(mutation(old_overlay,p.TABLE,lambda r:r.find('ItemClasses').append(ET.Element('Ammo',{'Name':'foreign_role'}))),'unknown extra old table row refused')
    refused(mutation(old_overlay,p.TABLE,lambda r:ammo(r).append(ET.Element('Unknown'))),'unknown child in frozen old row refused')
    target_member='Libs/Tables/item/item__kcd2vr_throwing_assets1.xml'
    for legacy,broken,base,cls in p.roles.BROKEN_ROLES.values():
        def find(root):return next(r for r in root.iter('MeleeWeapon') if r.get('Name')==legacy)
        for label,fn in [
            ('missing',lambda r:r.find('ItemClasses').remove(find(r))),
            ('wrong model',lambda r:find(r).set('Model','other/model')),
            ('wrong class',lambda r:find(r).set('Class','9')),
            ('missing localization',lambda r:find(r).attrib.pop('UIName')),
            ('wrong GUID',lambda r:find(r).set('Id','00000000-0000-0000-0000-000000000000')),
            ('duplicate',lambda r:r.find('ItemClasses').append(copy.deepcopy(find(r)))),
        ]:
            refused(mutation(after,target_member,fn),label+' legacy target refused '+legacy)
    refused(mutation(after,target_member,lambda r:next(r.iter('MeleeWeapon')).set('UIInfo','ui_absent')),'wrong visible localization key refused')
    ck({n:hashlib.sha256(data).hexdigest() for n,(_,data) in members.items()}==immutable,'input archive member bytes unchanged')
    ck((unified/p.base.PAK).read_bytes()==files[p.base.PAK],'input archive file bytes unchanged')
    if xml_out:
        xml_out.mkdir(parents=True,exist_ok=True)
        for name,data in generated.items():
            dst=xml_out/name;dst.parent.mkdir(parents=True,exist_ok=True);dst.write_bytes(data)
        dst=xml_out/p.TABLE;dst.parent.mkdir(parents=True,exist_ok=True);dst.write_bytes(after[p.TABLE][1])
    return {'suite':'T20 native BrokenGUID source candidate','passed':True,'checks':len(checks),'cases':checks,
        'native_execution':False,'archive_built':False,'installed':False,'retail_schema_checks':len(native_checks),
        'input_pak_sha256':hashlib.sha256(files[p.base.PAK]).hexdigest(),'input_members':len(members),'final_members':len(after),
        'initial_changed':sorted(changed),'unrelated_preserved':len(preserved),'upgrade_changed':sorted(upgrade_changes),
        'upgrade_other_payloads_preserved':len(old_overlay)-1,'old_red':{'expected_failure':True,'reason':red_reason,'generator_sha256':hashlib.sha256(frozen.read_bytes()).hexdigest()},
        'final_xml_sha256':hashlib.sha256(after[p.TABLE][1]).hexdigest(),
        'generated_xml':{n:hashlib.sha256(d).hexdigest() for n,d in generated.items()},
        'source_sha256':{str(path.relative_to(ROOT)):hashlib.sha256(path.read_bytes()).hexdigest() for path in [Path(p.roles.__file__),Path(p.__file__),ROOT/'companion/optional_throwing_assets1/build_asset_pack.py']}}


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--report',type=Path)
    parser.add_argument('--xml-out',type=Path)
    args=parser.parse_args()
    report=run(args.xml_out)
    if args.report:
        args.report.parent.mkdir(parents=True,exist_ok=True)
        args.report.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({k:v for k,v in report.items() if k not in ('cases','generated_xml','source_sha256')},indent=2))
