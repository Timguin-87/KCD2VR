"""Reproduce the selected read-only R59C4 native-services audit (requires capstone).
These are inspected seams, NOT certified callable contracts or runtime patches.
"""
from pathlib import Path
import argparse, hashlib, json, struct
from native_research112 import PE
from capstone import Cs, CS_ARCH_X86, CS_MODE_64

SHA = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
FUNCTIONS = {
    0x9089AC:'block-trigger attacker-state callback',
    0x908A9C:'block-trigger open', 0x908B50:'block-trigger close',
    0x908CF4:'block-trigger bound attacker resolver',
    0x90A100:'notifying bool property with actor payload',
    0x90A128:'bool/actor signal emission',
    0x909FC4:'defensive-slot acquire', 0x909FF4:'defensive-slot release',
    0x93A0C8:'notifying state-id property setter',
    0x1482DA0:'block action state-signal callback',
    0x1483250:'block action automation request',
    0x148345C:'block automation state-signal callback',
    0x1483480:'block automation type-6 request',
    0x1699500:'block-trigger action factory',
    0x1699558:'block-trigger action allocation',
    0x1410F70:'C_CombatTriggerBlock factory',
    0x914950:'trigger close lifecycle',
    0x9E3B80:'coordinator; defender reads opponent time-to-hit',
    0x9E43AC:'notifying float property setter',
    0x687130:'native attack-cost calculation',
    0x6871FC:'weapon/stat attack-cost calculation',
    0xC5BB14:'RPG preparation transaction candidate',
    0xC64CE4:'RPG strike-cost transaction candidate',
    0xC66334:'native stamina effect constructor',
    0xC5C9D8:'combat-stamina buff parameter allocation',
    0x169F74C:'combat-stamina buff factory',
    0x169F7F0:'combat-stamina buff constructor',
    0x1647B8C:'combat-stamina buff update',
    0xD568B0:'melee RPG build/evaluate/apply',
    0xD49AB4:'target reaction-manager selector',
    0x90C850:'recipient action dispatch',
    0xD542E0:'hit listeners/component notification',
    0xD53B24:'attack-helper callback; can dispatch native FailedAttack (not a generic completion)',
    0xD4CB30:'normal action collision registration reference',
}
# Inspected leaf extents take precedence over broad/shared pdata ranges. The
# attack-helper interface leaf ends at its tail jump; adjacent code is separate.
LEAVES = {0x908CF4:0x908D11, 0xD53B24:0xD53B2E}
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--whgame',required=True)
parser.add_argument('--out',type=Path,required=True)
args=parser.parse_args()
pe=PE(args.whgame)
digest=hashlib.sha256(pe.data).hexdigest()
off=struct.unpack_from('<I',pe.data,0x3C)[0]
stamp=struct.unpack_from('<I',pe.data,off+8)[0]
if digest!=SHA or stamp!=0x6A350E20: raise SystemExit('Exact retail fingerprint mismatch; no audit emitted')
args.out.mkdir(parents=True,exist_ok=True)
md=Cs(CS_ARCH_X86,CS_MODE_64);md.detail=True
report={'schema':1,'whgame_sha256':digest,'timestamp':hex(stamp),
        'status':'STATIC_SELECTED_RANGES_NOT_CALL_AUTHORIZATION','functions':[]}
for rva,label in FUNCTIONS.items():
    bound=pe.function(rva)
    if rva in LEAVES:
        start,end=rva,LEAVES[rva];extent='explicit inspected leaf extent'
    elif bound:
        start,end=bound
        if start!=rva: raise ValueError(f'{rva:X} is an interior address ({start:X})')
        extent='pdata principal range; external cold branches not included'
    else:raise ValueError(f'{rva:X} has no qualified extent')
    data=pe.read(start,end-start)
    ins=list(md.disasm(data,pe.base+start))
    text='\n'.join(f'{i.address-pe.base:08X} {i.bytes.hex():<32} {i.mnemonic} {i.op_str}'.rstrip() for i in ins)+'\n'
    (args.out/f'{rva:X}.asm').write_text(text,encoding='utf-8')
    report['functions'].append({'rva':hex(rva),'end':hex(end),'label':label,'extent':extent,
        'sha256':hashlib.sha256(data).hexdigest(),'entry16':data[:16].hex(),
        'direct_calls':list(dict.fromkeys(hex(i.operands[0].imm-pe.base) for i in ins if i.mnemonic=='call' and i.operands[0].type==2))})
(args.out/'audit.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps({'functions':len(report['functions']),'sha256':digest,'output':str(args.out)}))
