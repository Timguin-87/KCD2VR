"""Read-only exact retail ABI/category/consumer proof for H26 allowance adapter."""
from pathlib import Path
import argparse, hashlib, json, re, struct
from native_research112 import PE

parser=argparse.ArgumentParser()
parser.add_argument('dll')
parser.add_argument('--output')
args=parser.parse_args()
p=PE(args.dll)
expected='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
assert hashlib.sha256(p.data).hexdigest()==expected and p.stamp==0x6A350E20
source=Path(__file__).resolve().parents[1]/'src/vrik/player_window_h26.inl'
pins=[]
for address,escaped,size in re.findall(r'\{0x([A-Fa-f0-9]+),"((?:\\x[0-9a-fA-F]{2})+)",(\d+)\}',source.read_text()):
    want=bytes.fromhex(escaped.replace('\\x',''))
    assert len(want)==int(size) and p.read(int(address,16),len(want))==want,address
    pins.append(address)
assert len(pins)==12
categories={}
for vft,category,fn,name in [(0x3AECEA0,1,0x41A6A0,'C_CombatMultiTriggerPerfectBlock'),(0x3AECE20,5,0x1A78ED0,'C_CombatMultiTriggerMasterStrike')]:
    col=struct.unpack('<Q',p.read(vft-8,8))[0]-p.base
    sig,offset,cd,td,ch,selfr=struct.unpack('<IIIIII',p.read(col,24))
    assert sig==1 and offset==0 and selfr==col
    actual=p.read(td+16,180).split(bytes([0]))[0].decode()
    assert actual=='.?AV'+name+'@combatmodule@wh@@'
    assert struct.unpack('<Q',p.read(vft+0x68,8))[0]==p.base+fn
    assert p.read(fn,3)==bytes([0xb0,category,0xc3])
    categories[name]={'vft':hex(vft),'category':category,'getter':hex(fn),'rtti':actual}
# Original return XMM0; both native callers consume it as a float allowance.
assert p.read(0xB279F7,4)==bytes.fromhex('f30f58c6')
assert p.read(0x9142B4,10)==bytes.fromhex('e89f362100f30f114548')
assert p.read(0xB27654,5)==bytes.fromhex('e8ff020000')
assert p.read(0xB2765D,5)==bytes.fromhex('f30f114528')
assert sorted(p.calls(0xB27958))==[0x9142B4,0xB27654]
functions={}
for rva in [0xB27958,0xB27A18,0xB27AB8,0x914204,0xB275F8]:
    start,end=p.function(rva)
    assert start==rva
    functions[hex(rva)]={'end':hex(end),'sha256':hashlib.sha256(p.read(start,end-start)).hexdigest()}
result={'result':'PASS','dllSha256':expected,'timestamp':'6A350E20','instructionPins':pins,'categories':categories,'functions':functions,
        'nativeExecution':'NONE','scope':'Native return allowance only; original eligibility and impact cap remain native.'}
if args.output:Path(args.output).write_text(json.dumps(result,indent=2)+'\n')
print('PASS:12 instruction pins; two exact RTTI/vft category getters; float ABI/native consumers;5 function hashes; native execution NONE')
