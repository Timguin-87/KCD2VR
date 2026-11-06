"""Reproduce C46's alchemy ABI/event audit against the exact supplied retail PE.

Read-only: never loads or executes the game DLL. Runtime pin bytes are parsed
from the production header; a fixture cannot silently stand in for retail proof.
"""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source, read_source_bytes
from pathlib import Path
import argparse, hashlib, json, re, struct
import xml.etree.ElementTree as ET
from smith_native_probe46 import PE, EXPECTED

SLOTS = [
    (0x3F60260, 0x110, 0x17BAD08, 'UseBellows OnTick'),
    (0x3A5BD40, 0xF8, 0xA95AEC, 'OpenBook native start'),
    (0x3A7D9D0, 8, 0x1AAF020, 'CAnimEventList count'),
    (0x3A7D9D0, 0x18, 0x1AAFB60, 'CAnimEventList entry'),
    (0x3A711E0, 0x170, 0x84351C, 'CSkeletonAnim event dispatcher'),
    (0x40519E8, 0xE0, 0x1A73130, 'CActionController GetTimeScale'),
]
FUNCTIONS = [
    ('controller_update', 0x4B3DF8, None),
    ('controller_queue', 0x684D8C, None),
    ('controller_timescale', 0x1A73130, 9),
    ('bellows_work_builder', 0x6C653A, None),
    ('bellows_completion', 0x6C4B5B, None),
    ('bellows_ready_tick', 0x17BAD08, None),
    ('mortar_work_builder', 0x6C38E0, None),
    ('mortar_completion', 0x6C3046, None),
    ('resource_builder', 0x701250, None),
    ('resource_callback', 0x6C29D8, None),
    ('native_event_scan', 0x842248, None),
    ('native_event_scan_caller', 0x84182C, None),
    ('native_event_dispatch', 0x84351C, None),
    ('event_list_count', 0x1AAF020, 8),
    ('event_list_entry', 0x1AAFB60, 16),
    ('human_params_ctor', 0xD7FA70, None),
    ('human_fragment_ids', 0xD81378, None),
    ('book_read_enable', 0x9F2380, None),
    ('book_start_reading', 0x1833484, 40),
    ('book_page_a', 0xA96158, None),
    ('book_page_b', 0x2E17BD8, None),
]

def dump(path, value):
    path.write_text(json.dumps(value, indent=2, sort_keys=True)+'\n')

def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('dll', type=Path)
    ap.add_argument('--out', required=True, type=Path)
    ap.add_argument('--tables', type=Path, help='Extracted Libs/Tables root (optional)')
    a = ap.parse_args(); a.out.mkdir(parents=True, exist_ok=True)
    pe = PE(a.dll)
    header = Path(__file__).resolve().parents[1] / 'src/core/alchemy_semi_pins46.h'
    rows = []
    pattern = r'\{(0x[0-9A-Fa-f]+),\s*"((?:\\x[0-9A-Fa-f]{2})+)",\s*(\d+)\},\s*//\s*([^\n]+)'
    for m in re.finditer(pattern, read_source(header)):
        rva, n = int(m[1],16), int(m[3]); expected = bytes.fromhex(m[2].replace('\\x',''))
        actual = pe.read(rva,n)
        assert len(expected)==n and actual==expected, (hex(rva),m[4],actual.hex(),expected.hex())
        bounds = pe.function(rva)
        instruction = None
        if bounds:
            instruction = next((i for i in pe.disasm(*bounds) if i['rva']==rva), None)
        rows.append(dict(rva=hex(rva), size=n, expected=expected.hex(), actual=actual.hex(),
                         verified=True, purpose=m[4], unwind_range=list(map(hex,bounds)) if bounds else None,
                         first_instruction=instruction))
    assert len(rows)==read_source(header).count('}, //'), 'Every production pin must be audited'
    pin_count=len(rows); slots=[]
    for table, offset, expected, purpose in SLOTS:
        actual=struct.unpack('<Q',pe.read(table+offset,8))[0]-pe.base
        assert actual==expected, (hex(table),hex(offset),hex(actual),hex(expected))
        slots.append(dict(vtable=hex(table),offset=hex(offset),target=hex(actual),verified=True,purpose=purpose))
    dump(a.out/'runtime_pins.json',dict(sha256=EXPECTED,timestamp=hex(pe.stamp),image_base=hex(pe.base),
        header_sha256=hashlib.sha256(read_source_bytes(header)).hexdigest(),pin_count=len(rows),pins=rows,
        vtable_count=len(slots),vtables=slots,method='Read-only PE bytes and unwind-bounded GNU objdump; no game execution'))
    functions=[]
    for name,rva,size in FUNCTIONS:
        bounds=(rva,rva+size) if size else pe.function(rva)
        assert bounds, (name,hex(rva),'Missing native boundary')
        (a.out/(name+'.asm')).write_text(pe.listing(*bounds))
        functions.append(dict(name=name,selected_rva=hex(rva),start=hex(bounds[0]),end=hex(bounds[1]),
                              boundary='Exact verified leaf span' if size else 'Native PE exception/unwind range'))
    dump(a.out/'functions.json',functions)
    table_counts={}
    if a.tables:
        tables={}; identities={}
        for relative in ('animation/anim_fragment_events.xml','animation/anim_fragment.xml','action/actor_anim_action.xml'):
            path=a.tables/relative; rows=[]
            for _,element in ET.iterparse(path,events=('end',)):
                if element.attrib and any('alchemy' in v.lower() for v in element.attrib.values()):
                    rows.append(dict(element.attrib))
                element.clear()
            tables[relative]=rows; table_counts[relative]=len(rows)
            identities[relative]=dict(sha256=hashlib.sha256(path.read_bytes()).hexdigest(),rows=len(rows))
        names=sorted({row.get('mn_fragment_id','') for rows in tables.values() for row in rows})
        dump(a.out/'animation_tables_alchemy.json',tables)
        dump(a.out/'animation_table_index.json',dict(inputs=identities,fragments=names,
            stirring_named_fragments=[n for n in names if re.search('stir|ladle',n,re.I)],
            boundary='Supplied table coverage only; absence does not invent a native verb or prove every possible mod/asset'))
    print(json.dumps(dict(result='PASS',pins=pin_count,
        vtable_slots=len(slots),native_function_listings=len(functions),table_rows=table_counts,
        sha256=EXPECTED,out=str(a.out)),sort_keys=True))

if __name__=='__main__': main()
