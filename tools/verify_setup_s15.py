"""Read-only launcher/PE export wiring check; does not execute the DLL or game."""
import argparse,json,hashlib
from pathlib import Path
from verify_single_binary_b1 import Binary
p=argparse.ArgumentParser();p.add_argument('--binary',type=Path,required=True);p.add_argument('--launcher',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
b=Binary(a.binary);exports=b.exports();assert 'KCD2VRSetupW' in exports, 'Actual artifact has no setup export'
if a.binary.name.lower()=='dinput8.dll': assert exports['KCD2VRSetupW']==7
script=a.launcher.read_bytes();assert b'%SystemRoot%\\System32\\rundll32.exe' in script and ('%~dp0'+a.binary.name).encode() in script and b',KCD2VRSetupW' in script
r={'result':'PASS','dll_sha256':hashlib.sha256(b.raw).hexdigest(),'launcher_sha256':hashlib.sha256(script).hexdigest(),'setup_export':exports['KCD2VRSetupW'],'binary_name':a.binary.name,'native_execution':False,'game_launched':False}
a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(r,indent=2)+'\n');print(json.dumps(r,indent=2))
