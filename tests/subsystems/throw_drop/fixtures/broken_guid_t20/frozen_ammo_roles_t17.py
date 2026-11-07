"""Native projectile roles, separate from already saved held melee item GUIDs.

Data only: constructing these XML rows neither grants stock nor launches anything.
The release owner must bind one finite unit to one acknowledged native projectile.
"""
import copy
import uuid

NAMESPACE=uuid.UUID('a6ea5379-e253-5e52-a340-b0fb207f7e9d')
DAGGER='kcd2vr_throwing_dagger_ammo_t17'
AXE='kcd2vr_throwing_axe_ammo_t17'
LAUNCHER='kcd2vr_throwing_launcher_t17'

def guid(name):
    return str(uuid.uuid5(NAMESPACE,name))

def rows(native):
    result=[]
    for name,held in ((DAGGER,'daggerCommon'),(AXE,'axeWork01')):
        # A native Arrow, not an Ammo-looking melee definition. Keep its native
        # type/schema and projectile factory. Runtime presentation/rotation must
        # qualify the supplied weapon model's helpers before claiming alignment.
        ammo=copy.deepcopy(native.items['arrow_enh_cutting'])
        source=native.items[held]
        ammo.set('Name',name);ammo.set('Id',guid(name))
        ammo.set('UIName','ui_nm_kcd2vr_throwing_'+('axe1' if name==AXE else 'dagger1'))
        ammo.set('UIInfo','ui_in_kcd2vr_throwing_'+('axe1' if name==AXE else 'dagger1'))
        ammo.set('Model',source.get('Model'))
        ammo.set('IconId',source.get('IconId'))
        ammo.set('UiSound',source.get('UiSound','ui_inv_item_weapon_arrows'))
        ammo.set('Weight',source.get('Weight'))
        ammo.set('InquiverMaterial','')
        ammo.attrib.pop('InquiverModel',None)
        # Do not inherit the ordinary broken-arrow model. THIS OMISSION DOES
        # NOT MAKE THE ROLE UNBREAKABLE: native 2AA0138 can delete its payload
        # then attempt the absent/zero BrokenGUID. Post-impact disposition is
        # still unqualified; these data-only roles are not shipping-ready.
        ammo.attrib.pop('BrokenItemClassId',None)
        ammo.set('VisibilityCoef','1')
        ammo.set('AttackStab','0' if name==AXE else '160')
        ammo.set('AttackSlash','160' if name==AXE else '0')
        ammo.set('AttackSmash','60' if name==AXE else '0')
        ammo.set('DisplayInShop','false')
        result.append(ammo)
    # 289F48C dereferences missile-class metadata on the SOURCE weapon as well
    # as Ammo on the payload. Passing a dagger there is not a valid launcher.
    # This private role is never equipped, granted, sold or added to loadouts
    # by this data patch. Its WUID must survive while native causes reference it.
    source=copy.deepcopy(native.items['bow_b'])
    source.set('Name',LAUNCHER);source.set('Id',guid(LAUNCHER))
    source.set('UIName','ui_nm_kcd2vr_throwing_dagger1')
    source.set('UIInfo','ui_in_kcd2vr_throwing_dagger1')
    source.set('VisibilityCoef','1')
    source.set('IsBreakable','false')
    result.append(source)
    return result

def validate(native,result):
    import re
    import xml.etree.ElementTree as ET
    ns={'x':'http://www.w3.org/2001/XMLSchema'}
    xsd=native.get('Libs/Tables/item/item.xsd')
    assert len(result)==3
    assert len({e.get('Id') for e in result})==3
    assert not ({e.get('Id') for e in result}&native.all_guids)
    assert not ({e.get('Name') for e in result}&set(native.items))
    for e in result:
        schema=xsd.find("x:element[@name='%s']"%e.tag,ns)
        allowed={a.get('name'):a for a in schema.findall('.//x:attribute',ns)}
        # Retail's stale XSD omits these attributes, which the exact cloned
        # native rows do contain. No unproven new gameplay attribute is added.
        base=native.items['arrow_enh_cutting' if e.tag=='Ammo' else 'bow_b']
        assert set(e.attrib)<=set(allowed)|set(base.attrib)
        assert all(a.get('name') in e.attrib for a in allowed.values() if a.get('use')=='required')
        for k,v in e.attrib.items():
            typ=allowed[k].get('type','') if k in allowed else ''
            if typ=='xs:integer':assert re.fullmatch(r'-?\d+',v),(e.get('Name'),k,v)
            elif typ=='xs:decimal':assert re.fullmatch(r'-?\d+(\.\d+)?',v)
            elif typ=='xs:boolean':assert v in ('true','false','0','1')
            elif typ=='xs:NCName':assert re.fullmatch(r'[A-Za-z_][A-Za-z0-9_.-]*',v)
    assert all(e.tag=='Ammo' and e.get('Type')=='1' for e in result[:2])
    assert result[2].tag=='MissileWeapon' and result[2].get('Class')=='9'
    assert result[0].get('Model')==native.items['daggerCommon'].get('Model')
    assert result[1].get('Model')==native.items['axeWork01'].get('Model')
    return {'native_ammo_roles':2,'native_missile_source_roles':1,
            'existing_saved_guid_changes':0,'grants':0,'native_launch_executed':False,
            'ids':{e.get('Name'):e.get('Id') for e in result}}
