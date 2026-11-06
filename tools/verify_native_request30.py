"""Exact-retail read-only native RequestDialog named argument/lifecycle proof."""
from pathlib import Path
import hashlib, json, struct, sys, zipfile
SOURCE = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(SOURCE/'tools'))
from native_research112 import PE

PIN = {
 0x11441F9:'41b888000000488d1582469802e895000000',
 0x169C7EE:'498d8e88000000e8bef6d2008a08888d95000000',
 0x4D1911:'488b034532ffba0400000044887d40488bcbff503083f80375134c8d4540488bcb8d5001e85e070000448a7d40',
 0x4D197F:'488bcf44887c2420ff9010050000',
 0x93AE58:'405553565741564157488d6c24c84881ec38010000',
 0x93AF14:'e85710a901488d542430488b8850010000488b7910e8c2100000488d54242844887dd4488d4d88e800bab2',
 0x93AF66:'8a85900000008845d584db74',
 0xD57979:'488b8358010000488b88b000000080b9a500000000',
 0xD579CF:'4885c074164c8b83b8020000488bc8488b9358010000e8866e1101488983e8020000',
 0x86FA42:'488bcfe86a52daff4883bfe8020000007417',
 0x86F68D:'488b8f68010000b201e8456adaff',
 0x6160E0:'488b89a8000000e93c000000',
 0x8DA2D4:'488b8140030000483b81480300007436488b00488b50408a82b000000084c0742c3c0374283c06742432d2488b89580100008b81bc00000084d2750b0fbaf0158981bc000000c30fbae815ebf3b201eb',
 0x3BDF7C:'488b9fe80200004885db740e0f28ce488bcbe82d938400894310',
}
def main():
 import argparse
 parser=argparse.ArgumentParser(description=__doc__)
 parser.add_argument('--binary',default='C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll')
 parser.add_argument('--scripts',default='C:/Steam/steamapps/common/KingdomComeDeliverance2/Data/Scripts.pak')
 parser.add_argument('--out',type=Path,required=True)
 args=parser.parse_args()
 p=PE(args.binary)
 sha=hashlib.sha256(p.data).hexdigest()
 assert p.stamp==0x6A350E20 and sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
 pins=[]
 for a,h in PIN.items():
  data=p.read(a,len(bytes.fromhex(h)));assert data.hex()==h,hex(a)
  pins.append({'rva':hex(a),'hex':h})
 # Distinct exact Actor/Human/Player primary dispatch entries share this body.
 entries=[]
 for vf in [0x3E74198,0x3A3B398,0x3AA4B98]:
  target=struct.unpack('<Q',p.read(vf+0x510,8))[0]-p.base
  assert target==0x93AE58
  entries.append({'vft':hex(vf),'byteSlot':'0x510','target':'0x93ae58'})
 # Named field calculation from two independent native producers.
 bt_base=-0x10;bt_store=0x95
 # 93AE58: 6 pushes, rbp=entry-0x30-0x38, finalrsp=entry-0x30-0x138.
 lua_rbp=-0x30-0x38;lua_rsp=-0x30-0x138
 lua_base=lua_rsp+0x30-lua_rbp
 assert bt_store-bt_base==0xA5 and -0x2B-lua_base==0xA5
 scripts=[]
 with zipfile.ZipFile(args.scripts) as z:
  for n in ['Scripts/Entities/AI/Shared/BasicAIActions.lua','Scripts/Entities/AI/Dog.lua']:
   data=z.read(n)
   assert data.count(b"self.actor:RequestDialog(user.id,'',false,true)")==1
   scripts.append({'entry':n,'sha256':hashlib.sha256(data).hexdigest(),'fourthArgument':True})
 result={'retailSHA256':sha,'timestamp':hex(p.stamp),'codePins':pins,'dispatch':entries,
  'namedField':{'UseTwins': 'S_DialogParams+0xA5','BTContextPropertyByteOffset':'0x88',
   'btParamsBase':'rbp-0x10','btStore':'rbp+0x95','luaParamsBase':'rbp-0xD0',
   'luaFourthArgumentStore':'rbp-0x2B','luaNativeFifthArgument':'[rbp+0x90]'},
  'scripts':scripts,'nativeExecuted':False,'writes':False}
 args.out.parent.mkdir(parents=True,exist_ok=True)
 args.out.write_text(json.dumps(result,indent=2)+'\n')
 print(f'native RequestDialog: exact SHA + {len(pins)} producer/consumer pins + Actor/Human/Player dispatch + both stock Lua sites PASS')
if __name__=='__main__':main()
