"""Read-only exact retail correspondences for T2 supply/drop-completion.
Never imports/loads WHGame as executable code or touches installed data.
"""
from pathlib import Path
import hashlib,json,struct,sys
sys.path.insert(0,r'C:/Users/turcs/.codex/worktrees/d514/KCD2VR/build-review178r19/analysis-deps')
import pefile,capstone
ROOT=Path(__file__).resolve().parents[1];out=ROOT/'review/THROW2_SUPPLY';out.mkdir(parents=True,exist_ok=True)
DLL=Path(r'C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll')
raw=DLL.read_bytes();p=pefile.PE(data=raw,fast_load=True);base=p.OPTIONAL_HEADER.ImageBase
checks=[]
def ck(ok,label):
 assert ok,label;checks.append(label)
ck(hashlib.sha256(raw).hexdigest()=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7','exact retail SHA256')
ck(p.FILE_HEADER.TimeDateStamp==0x6A350E20,'exact retail timestamp')
def pin(a,h,label):ck(p.get_data(a,len(bytes.fromhex(h)))==bytes.fromhex(h),label)
def target(a,disp=3,size=7):return a+size+struct.unpack('<i',p.get_data(a+disp,4))[0]
def call(a,dst):ck(p.get_data(a,1)==b'\xe8' and target(a,1,5)==dst,f'call {a:X}->{dst:X}')
def name(a,want):ck(p.get_data(target(a),len(want)+1)==want.encode()+b'\0',f'current registration/field {want}@{a:X}')
ck(target(0x144B8F2)==0x82B440,'System.GetEntity actual registered callback')
name(0x144B90C,'GetEntity')
pin(0x82B46A,'83f802','System.GetEntity explicitly distinguishes numeric parameter type2')
ck(target(0x82B46D,2,6)==0x2249D86,'numeric entity parameter selects native number branch')
call(0x2249D8B,0xAE5C18)
pin(0x82B48D,'8b542438','GetEntity rejoins with native DWORD EntityID')
pin(0x82B494,'ff5070','GetEntity uses actual entity-system lookup')
call(0x82B481,0x712164)
pin(0x8CEA48,'4883ec284c8bd94889542438','explicit-hand WUID wrapper prologue')
call(0x2AB9CCC,0x8CEA48);pin(0x2AB9CD1,'488b07','exact registered Lua return address')
pin(0x2AB9CB5,'83fe017717','current script hand accepts only0/1')
pin(0x8D0122,'83fe02753a','native explicit0/1 bypass default-hand selection')
ck(target(0x12B3153)==0x2ABBFF8,'GetInventoryTable actual registered callback')
name(0x12B3161,'GetInventoryTable')
pin(0x2ABC055,'33f6','inventory output table starts index0')
pin(0x2ABC074,'488b4130','inventory output value is current item WUID+30')
call(0x2ABC082,0xAD75D0)
pin(0xAD75D4,'498b00','SetAt receives full uint64 WUID')
pin(0xAD75E4,'c744242003000000','SetAt uses script handle type3, no float conversion')
ck(target(0x12B2D72)==0x2ABC0BC,'ItemManager.GetItem actual registered callback')
name(0x12B2D8E,'GetItem')
for a,s in [(0x2ABC11B,'id'),(0x2ABC158,'class'),(0x2ABC179,'health'),(0x2ABC194,'amount'),(0x2ABC1AE,'entity')]:name(a,s)
pin(0x2ABC10F,'488b4f30','GetItem.id from actual WUID+30')
pin(0x2ABC18D,'8b4750','GetItem.amount from actual item+50')
ck(target(0x1450246)==0x37D3354,'GetAttachmentBone actual registered callback')
name(0x1450254,'GetAttachmentBone')
ck(target(0x145026F)==0x37D3414,'GetAttachmentCGF actual registered callback')
name(0x145027D,'GetAttachmentCGF')
pin(0x37D33CD,'488b91c8000000','GetAttachmentBone asks actual attachment bone id')
pin(0x37D33C6,'488b5930','GetAttachmentBone resolves actual skeleton name')
pin(0x37D3490,'488b9148010000','GetAttachmentCGF returns actual bound statobj path')
pin(0x1F228D0,'4c8bdc574881ec80000000488b4108','exact drop consumer prologue')
pin(0x1F229C0,'ff9068010000','drop consumer performs native physicalize/linear call before return')
call(0x1F21BF5,0x1F20FB0)
pin(0x1F20FCD,'0f10030f1140080f104b100f1148180f1043200f1140280f104b300f1148380f1043400f114048','queued closure copies all0x50 record bytes unchanged')
ck(struct.unpack('<Q',p.get_data(0x3EC8E38+0x10,8))[0]==base+0x1F24570,'queued drop closure execute vtable slot')
pin(0x1F24570,'4883c108e957e3ffff','queued closure adjusts record+8 then reaches exact consumer')
call(0x1F1F08C,0x1F228D0)
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_64)
for start,end in [(0x82B440,0x82B4F0),(0x2249D86,0x2249D96),(0x2ABBFF8,0x2ABC0BC),(0x2ABC0BC,0x2ABC1F7),(0x37D3354,0x37D3414),(0x37D3414,0x37D34C9),(0x1F20FB0,0x1F20FFA),(0x1F228D0,0x1F22A06)]:
 (out/f'RETAIL_{start:X}.asm').write_text('\n'.join(f'{i.address-base:08X} {i.bytes.hex():26} {i.mnemonic} {i.op_str}'.rstrip() for i in md.disasm(p.get_data(start,end-start),base+start))+'\n')
report={'result':'PASS','checks':len(checks),'details':checks,'sha256':hashlib.sha256(raw).hexdigest(),'execution':'NONE read-only PE data','runtime_acceptance':'NOT_RUN'}
(out/'RETAIL_PROOF.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
