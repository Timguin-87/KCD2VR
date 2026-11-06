"""Read-only exact-retail door helper and production MID pin/branch audit.
Requires an already available capstone module; never downloads or executes game.
"""
import argparse,bisect,hashlib,json,re,struct,sys
from pathlib import Path
ap=argparse.ArgumentParser();ap.add_argument('--whgame',required=True,type=Path);ap.add_argument('--out',required=True,type=Path);ap.add_argument('--module-path',type=Path);args=ap.parse_args()
if args.module_path:sys.path.insert(0,str(args.module_path))
from capstone import Cs,CS_ARCH_X86,CS_MODE_64
root=Path(__file__).resolve().parents[1];data=args.whgame.read_bytes();sha=hashlib.sha256(data).hexdigest()
assert sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
pe=struct.unpack_from('<I',data,0x3c)[0];assert struct.unpack_from('<I',data,pe+8)[0]==0x6a350e20
opt=pe+24;base=struct.unpack_from('<Q',data,opt+24)[0];nsec=struct.unpack_from('<H',data,pe+6)[0];optsize=struct.unpack_from('<H',data,pe+20)[0];sections=[]
for n in range(nsec):
 o=opt+optsize+n*40;name=data[o:o+8].rstrip(b'\0').decode();vs,va,size,raw=struct.unpack_from('<IIII',data,o+8);sections.append((name,va,va+size,raw,size))
def offset(r):
 for _,a,b,raw,_ in sections:
  if a<=r<b:return raw+r-a
 raise ValueError(hex(r))
def raw(r,n):return data[offset(r):offset(r)+n]
pd,plen=struct.unpack_from('<II',data,opt+112+24);funcs=[struct.unpack_from('<III',data,o) for o in range(offset(pd),offset(pd)+plen,12)];starts=[f[0] for f in funcs]
md=Cs(CS_ARCH_X86,CS_MODE_64);md.detail=True
args.out.mkdir(parents=True,exist_ok=True)
def decode(r,n):return list(md.disasm(raw(r,n),base+r))
def body(r,length=None):
 i=bisect.bisect_right(starts,r)-1;f=funcs[i] if i>=0 else None
 if length is None:
  assert f and f[0]<=r<f[1],hex(r);a,b,u=f
 else:a,b,u=r,r+length,0
 ins=decode(a,b-a);path=args.out/f'{r:X}.asm';path.write_text('\n'.join([f'; exact retail RVA{a:X}..{b:X} unwind{u:X}']+[f'{i.address-base:08X} {i.bytes.hex():<32} {i.mnemonic:<10} {i.op_str}'.rstrip() for i in ins])+'\n')
 return dict(rva=hex(r),start=hex(a),end=hex(b),sha256=hashlib.sha256(raw(a,b-a)).hexdigest(),assembly=path.name),ins
specs={0x63f9ac:None,0x7f55fc:None,0xfbb860:None,0x4033ac:0x42,0xa867d0:None,0xa86fd8:None,0xa87478:None,0x1ed47b0:0x50,0x1ed4840:None,0x1ed5f50:None,0x1ed3fb0:None,0x1ecfa10:0x18c,0x1ed7270:0x82}
functions=[];selector=[]
for r,length in specs.items():
 f,ins=body(r,length);functions.append(f)
 if r==0x63f9ac:selector=ins
src=(root/'src/interaction_door_diagnostics_c54r3.inl').read_text();pins=[(int(a,16),bytes.fromhex(b),stage) for a,b,stage in re.findall(r'\{0x([0-9A-F]+),"([0-9a-f]+)",&DoorDiagMid3<dd3::(\w+)>\}',src)]
assert len(pins)==9
for r,p,_ in pins:assert raw(r,len(p))==p,hex(r)
existing=[0x6404e1,0x6404b9,0x640117] # other retained selector MID patches
sensitiveCalls={0x64033b:0x640340,0x64035e:0x640363} # existing wrappers qualify native return address
branchAudit=[]
for r,_,stage in pins:
 for minimum in [5,14]:
  decoded=decode(r,32);span=0;displaced=[]
  for ins in decoded:
   displaced.append(ins);span+=ins.size
   if span>=minimum:break
  end=r+span
  incoming=[]
  for ins in selector:
   if ins.mnemonic.startswith('j') and ins.operands and ins.operands[0].type==2:
    target=ins.operands[0].imm-base;addr=ins.address-base
    if r<target<end and not r<=addr<end:incoming.append(dict(source=hex(addr),target=hex(target),op=ins.mnemonic))
  overlaps=[]
  for other in existing+[p[0] for p in pins if p[0]!=r]:
   size=0
   for ins in decode(other,32):
    size+=ins.size
    if size>=minimum:break
   if max(r,other)<min(end,other+size):overlaps.append(hex(other))
  changedRA=[hex(i.address-base) for i in displaced if i.mnemonic=='call' and i.address-base in sensitiveCalls]
  assert not incoming and not overlaps and not changedRA,(hex(r),minimum,incoming,overlaps,changedRA)
  branchAudit.append(dict(site=hex(r),stage=stage,minJump=minimum,span=span,end=hex(end),inboundTail=incoming,overlaps=overlaps,relocatedReturnSensitiveCalls=changedRA,relocatedCalls=[hex(i.address-base) for i in displaced if i.mnemonic=='call']))
# Direct E8 call candidates are independently validated at instruction boundary
# within their actual runtime-function body, rather than treating byte hits as calls.
callers=[]
for name,a,b,off,size in sections:
 if name!='.text':continue
 region=data[off:off+size];at=region.find(b'\xe8')
 while at>=0 and at+5<=len(region):
  call=a+at
  if call+5+struct.unpack_from('<i',region,at+1)[0]==0x63f9ac:
   idx=bisect.bisect_right(starts,call)-1
   if idx>=0 and funcs[idx][0]<=call<funcs[idx][1]:
    start,end,_=funcs[idx]
    if any(i.address-base==call and i.mnemonic=='call' for i in decode(start,end-start)):callers.append(hex(call))
  at=region.find(b'\xe8',at+1)
assert callers==['0x7f5722'],callers
threshold=struct.unpack('<f',raw(0x409a4a8,4))[0];assert threshold==2**-23
report=dict(retailSha256=sha,timestamp='0x6A350E20',selectorDirectCallers=callers,callerReturn='0x7F5727',functions=functions,pins=[dict(rva=hex(r),stage=stage,hex=p.hex()) for r,p,stage in pins],branchAudit=branchAudit,comparatorEpsilon=threshold,nativeExecution=False,disassemblyIsStatic=True)
(args.out/'VERIFICATION.json').write_text(json.dumps(report,indent=2)+'\n');print(f'door_retail_c54r3={len(pins)} pins/{len(branchAudit)} branch+overlap checks PASS; game execution NONE')
