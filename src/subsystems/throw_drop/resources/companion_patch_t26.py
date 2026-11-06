"""In-memory LEFT chest knife overlay. No archive writer or game installation."""
import copy
import hashlib
import xml.etree.ElementTree as ET
from pathlib import Path
import companion_patch_t22 as prior

STARTUP = 'Scripts/Startup/zzkcd2vr_throwing_assets1.lua'
PRESETS = 'Libs/Tables/item/InventoryPreset__kcd2vr_throwing_assets1.xml'
PERKS = 'Libs/Tables/rpg/perk__kcd2vr_throwing_assets1.xml'
SCRIPTS = 'Libs/Tables/rpg/perk_script__kcd2vr_throwing_assets1.xml'
RECEIPT = '969eb6ac-4837-58a0-98d1-2fc9ac06c5d5'
STOCK = 'inventory_kcd2vr_throwing_assets1'
KNIFE = 'kcd2vr_throwing_dagger1'  # saved GUID remains compatible
AXE = 'kcd2vr_throwing_axe1'


def parse(data):
    return ET.fromstring(data.decode('utf-8-sig'))


def encode(root):
    ET.indent(root, space='  ')
    return ET.tostring(root, encoding='utf-8', xml_declaration=True) + b'\n'


def weapon_shops(native):
    # Exact retail ShopData identities; horse armour merchants are not weaponsmiths.
    result = sorted(name for name, row in native.shops.items()
                    if any(k in row.get('shop_name', '').lower()
                           for k in ('weaponsmith', 'blacksmith'))
                    and 'horseblacksmith' not in row.get('shop_name', '').lower())
    prior.base.require(result and all(n in native.presets for n in result),
                       'weaponsmith stock source missing')
    return result


def plan_members(members, native):
    original = copy.deepcopy(members)
    result, _, _ = prior.plan_members(members, native)
    require = prior.base.require
    require(RECEIPT not in native.all_guids, "T26 receipt collides with retail GUID")
    for n in (STARTUP, PRESETS, PERKS, SCRIPTS):
        require(n in result, 'current combined knife asset missing: ' + n)
    old_delivery = Path(__file__).with_name('inventory_delivery_t6.lua').read_bytes()
    new_delivery = Path(__file__).with_name('inventory_delivery_t26.lua').read_bytes()
    require(result[STARTUP][1] in (old_delivery, new_delivery),
            'unknown current starter; preserve and review before overlay')

    def put(name, data):
        info = copy.copy(result[name][0])
        result[name] = (info, data)

    put(STARTUP, new_delivery)
    root = parse(result[PRESETS][1])
    section = root.find('InventoryPresets')
    rows = {r.get('Name'): r for r in section}
    require(STOCK in rows, 'finite stock preset missing')
    stock = rows[STOCK]
    # Retain knife and its old recipe; retire only our new axe stock offers.
    for child in list(stock):
        if child.get('Name') in (AXE, 'recipe_' + AXE):
            stock.remove(child)
    knives = [c for c in stock if c.get('Name') == KNIFE]
    require(len(knives) == 1, 'ambiguous knife stock')
    knives[0].set('Amount', '5')
    shops = weapon_shops(native)
    for name in shops:
        if name not in rows:
            rows[name] = copy.deepcopy(native.presets[name])
            section.append(rows[name])
        row = rows[name]
        refs = [c for c in row if c.tag == 'InventoryPresetRef' and c.get('Name') == STOCK]
        require(len(refs) <= 1, 'duplicate knife shop reference')
        if not refs:
            ET.SubElement(row, 'InventoryPresetRef', {'Name': STOCK})
    put(PRESETS, encode(root))
    for name, section_name, tag, attributes in (
        (PERKS, 'perks', 'perk', {'autolearnable': 'false', 'perk_id': RECEIPT,
            'perk_name': 'KCD2VR one-time LEFT chest knives T26', 'visibility': '0'}),
        (SCRIPTS, 'perk_scripts', 'perk_script', {'perk_id': RECEIPT,
            'perk_editor_name': 'KCD2VR one-time LEFT chest knives T26'})):
        root = parse(result[name][1]); section = root.find(section_name)
        require(section is not None, 'native perk table schema changed')
        found = [r for r in section if r.get('perk_id') == RECEIPT]
        require(len(found) <= 1, 'duplicate durable receipt')
        if not found:
            section.append(ET.Element(tag, attributes))
        else:
            require(found[0].attrib == attributes, 'foreign receipt definition')
        put(name, encode(root))
    changed = {n for n in result if n not in original or result[n][1] != original[n][1]}
    removed = set(original) - set(result)
    allowed = {STARTUP, PRESETS, PERKS, SCRIPTS, prior.STARTUP_MEMBER,
               prior.base.SUPPLY_MEMBER, prior.prior.TABLE, prior.RESERVATION}
    require(changed <= allowed and removed <= {prior.prior.LUA}, 'unrelated member changed')
    require(all(result[n][1] == d for n, (_, d) in original.items()
                if n not in changed and n not in removed), 'unrelated payload not preserved')
    return result, changed, removed, {'shop_presets': shops, 'receipt': RECEIPT,
        'new_knife_target': 5, 'new_axe_grants': 0, 'native_calls_executed': False}


def plan_localization(data):
    """Change only the owned knife name cells; caller preserves other ZIP members."""
    root = parse(data); changed = 0
    for row in root.findall('Row'):
        cells = list(row)
        if cells and cells[0].text == 'ui_nm_kcd2vr_throwing_dagger1':
            for cell in cells[1:]:
                cell.text = 'Throwing knife'
            changed += 1
    prior.base.require(changed == 1, 'knife localization key missing/duplicate')
    return encode(root)
