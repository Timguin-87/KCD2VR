"""C27 read-only retail verification. Never executes a native function."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse,hashlib,json,re,sys
import pefile
root=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser()
parser.add_argument('--dll',default=r'C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll')
parser.add_argument('--output',type=Path)
args=parser.parse_args()
path=Path(args.dll);p=pefile.PE(str(path),fast_load=True);base=p.OPTIONAL_HEADER.ImageBase
checks=[]
def check(name,actual,expected):checks.append(dict(name=name,actual=actual,expected=expected,passed=actual==expected))
check('SHA256',hashlib.file_digest(path.open('rb'),'sha256').hexdigest(),'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7')
check('timestamp',hex(p.FILE_HEADER.TimeDateStamp),'0x6a350e20')
source=read_source(root/'src/diagnostics/native_defense_install27.inl')
pins=dict(entry=0xC55BB4,call=0x14837AF,evaluation=0xC55C8F,type=0xC5564E,managerLoad=0xC5588E,managerStore=0xC558D1,rowStore=0xC55D11)
for name,rva in pins.items():
    body=re.search(r'constexpr unsigned char '+name+r'\[\]\{([^}]+)\}',source).group(1)
    want=bytes(int(x,16) for x in re.findall(r'0x([0-9A-Fa-f]+)',body))
    check('compiled '+name,p.get_data(rva,len(want)).hex(),want.hex())
def bytes_at(name,rva,data):check(name,p.get_data(rva,len(bytes.fromhex(data))).hex(),data.lower())
bytes_at('explicit attacker R8 at candidate call',0x14837A8,'4d8bc6897c2420e800247dff')
bytes_at('candidate uses explicit attacker',0xC55BE2,'4d8be84c8bf1')
bytes_at('selected row copied to output',0xC55D15,'488bcee8ef0e74ff')
bytes_at('application zero native request',0x173BF26,'44886c2428')
bytes_at('evaluation call and return',0x173BF53,'e858a961ff84c0')
bytes_at('selected holder dispatch',0x1483917,'488d542430488bcee82c8f48ff')
check('NPC eligibility vslot5B0',hex(int.from_bytes(p.get_data(0x3E3FAC8+0x5B0,8),'little')-base),'0x9084d4')
result=dict(dll=str(path),native_execution=False,headset_test=False,passed=sum(c['passed'] for c in checks),total=len(checks),all_passed=all(c['passed'] for c in checks),checks=checks)
text=json.dumps(result,indent=2)+'\n'
if args.output:args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(text)
print(text);sys.exit(0 if result['all_passed'] else 1)
