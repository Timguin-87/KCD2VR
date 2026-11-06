"""Read-only retail/table/log audit. No SDK ABI assumptions or native execution."""
import argparse
import collections
import hashlib
import json
from pathlib import Path
import re
import statistics
import struct
import zipfile
import xml.etree.ElementTree as ET
from native_research112 import PE

def sha(data):return hashlib.sha256(data).hexdigest()

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--game',type=Path,required=True);ap.add_argument('--out',type=Path,required=True)
    a=ap.parse_args();a.out.mkdir(parents=True,exist_ok=True)
    dll=a.game/'Bin/Win64MasterMasterSteamPGO/WHGame.dll';pe=PE(dll)
    assert sha(pe.data)=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
    assert pe.stamp==0x6A350E20
    report={'dll':str(dll),'sha256':sha(pe.data),'timestamp':hex(pe.stamp),'native_execution':False,'tables':{}}
    # Verify retained instruction listings against current on-disk image. This
    # checks bytes, not the truth of all historical semantic annotations.
    root=Path(__file__).resolve().parents[1];verified={}
    for directory in ('review/V66.176/contact/native','review/V66.176/contact/attack_lifecycle_native'):
        for path in sorted((root/directory).glob('*.asm')):
            count=0
            for line in path.read_text().splitlines():
                match=re.match(r'\s*([0-9a-fA-F]+):\s+((?:[0-9a-fA-F]{2} )*[0-9a-fA-F]{2})\s',line)
                if not match:continue
                address=int(match[1],16);encoded=bytes.fromhex(match[2])
                assert pe.read(address-pe.base,len(encoded))==encoded,f'Listing drift: {path}:{address:x}'
                count+=1
            if count:verified[str(path.relative_to(root))]=count
    report['verified_listing_byte_rows']=verified
    try:
        from capstone import Cs,CS_ARCH_X86,CS_MODE_64
        md=Cs(CS_ARCH_X86,CS_MODE_64);report['fresh_disassembly']={}
        for start,end in [(0xD53610,0xD53778),(0xD56ECE,0xD56FD0),(0xA7EBBC,0xA7ED54),
                          (0xA7CD4C,0xA7CE50),(0x91115C,0x91122D)]:
            text='\n'.join(f'{i.address-pe.base:08X} {i.bytes.hex()} {i.mnemonic} {i.op_str}' for i in md.disasm(pe.read(start,end-start),pe.base+start))+'\n'
            dest=a.out/f'{start:X}.asm';dest.write_text(text);report['fresh_disassembly'][dest.name]=sha(text.encode())
    except ImportError:report['fresh_disassembly']='Capstone unavailable; retained bytes still checked'
    table_path=a.game/'Data/Tables.pak'
    with zipfile.ZipFile(table_path) as z:
        for n in z.namelist():
            if not any(n.endswith('/'+x) for x in ('weapon_class.xml','combat_action_hit.xml','combat_action_attack.xml','combat_action_sync_hit.xml','combat_action_sync_attack.xml','CombatCombos.xml','body_material2subpart.xml')):continue
            data=z.read(n);rows=[x.attrib for x in ET.fromstring(data).iter() if len(x)==0 and x.attrib]
            item={'sha256':sha(data),'rows':len(rows)}
            if n.endswith('/weapon_class.xml'):item['classes']=[{k:x[k] for k in ('id','name','is_twohanded','primary_attack_type_id','secondary_attack_type_id') if k in x} for x in rows]
            if n.endswith('/body_material2subpart.xml'):item['human_rows']=[x for x in rows if x.get('race_id')=='0']
            if 'combat_action_' in n:
                item['ragdoll_by_fragment']=dict(collections.Counter(x.get('mn_fragment_id','') for x in rows if x.get('hasRagdoll')=='true'))
                item['charged_count']=sum(x.get('charged_attack')=='true' for x in rows)
                item['kick_tags']=sum('oppKick' in x.get('mn_tags','') for x in rows)
            if n.endswith('/combat_action_attack.xml'):
                selected=[x for x in rows if x.get('attack_type_id') in ('0','1') and x.get('mounted')=='false']
                item['unmounted_type01_rows']=len(selected)
                for field in ('attack_time_to_start','attack_time_to_hit','attack_time_to_end'):
                    v=[float(x[field]) for x in selected if field in x]
                    item[field]={'min':min(v),'median':statistics.median(v),'max':max(v)} if v else None
                item['player_field_counts']=dict(collections.Counter(x.get('player','') for x in rows))
                item['note']='Table times are authored metadata, not live release-to-window timing. player=false rows are NOT a proven Henry admission contract.'
            report['tables'][n]=item
    log=a.game/'Bin/Win64MasterMasterSteamPGO/KCD2VR-diagnostics.log'
    if log.exists():
        data=log.read_bytes();text=data.decode('utf-8','replace')
        report['available_log']={'path':str(log),'bytes':len(data),'sha256':sha(data),
            'builds':re.findall(r'KCD2VR BUILD ([^\r\n]+)',text),
            'tag_counts':{s:text.count(s) for s in ('F4_ATTACK_STAGE','F4_CONTACT','HIT_TEMPLATE','MELEE_COMMAND','PREDICTION','F4_PHYSICAL_IMPACT')},
            'note':'Marker determines provenance. Existing log is not a V176/V177 headset validation.'}
        templates=[bytes.fromhex(x) for x in re.findall(r'HIT_TEMPLATE[^\n]*detailsD8=([0-9A-Fa-f]+)',text)]
        templates=[x for x in templates if len(x)==0xD8]
        if templates:
            report['available_log']['template_audit']={
                'complete_D8_copies':len(templates),
                'target_ids_at24':sorted({struct.unpack_from('<I',x,0x24)[0] for x in templates}),
                'surface_values_at64':sorted({struct.unpack_from('<i',x,0x64)[0] for x in templates}),
                'body_material_values_at68':sorted({struct.unpack_from('<i',x,0x68)[0] for x in templates}),
                'dword_distinct_values':{f'{offset:02X}':len({x[offset:offset+4] for x in templates}) for offset in range(0,0xD8,4)},
                'note':'Forty templates already satisfy a count-only request for 20 examples. Variability does not identify ownership, pointers, vector lifetimes or field semantics; do not clone this blob.'}
        submits=[line for line in text.splitlines() if ' SUBMIT_HIT ' in line]
        player_ids=set(re.findall(r'(?:attacker|target)=(\d+)\(PLAYER\)',text))
        report['available_log']['submit_audit']={
            'records':len(submits),'player_ids_marked_in_log':sorted(player_ids),
            'outgoing_player':sum(any(f'attacker={p} ' in x or f'attacker={p}(PLAYER)' in x for p in player_ids) for x in submits),
            'redirected_1':sum('redirected=1' in x for x in submits),
            'note':'Submit records retain native outcomes; not all have a tracked-trajectory causal join.'}
    (a.out/'audit.json').write_text(json.dumps(report,indent=2)+'\n')
    print(f'PASS {sum(verified.values())} listing byte rows, {len(report["tables"])} tables, exact retail image; no native calls')
if __name__=='__main__':main()
