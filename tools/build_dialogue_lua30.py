"""Exact-retail ordinary OnTalk argument repair proposal; no installation.
Only the proven user-player immersive path asks native RequestDialog for originals.
"""
from pathlib import Path
import argparse,hashlib
INPUTS={
 'Scripts/Entities/AI/Dog.lua':'a1f56bb3dd31047bb4bea9e4004c72be6098e08fcf5f0342a33ed8e5a7f453cb',
 'Scripts/Entities/AI/Shared/BasicAIActions.lua':'8faba8d08f3d1019c9a71c7bf198aaa5a16bf15ab2df82936d3aec8c5daaec0d'}
CALL="self.actor:RequestDialog(user.id,'',false,true)"
BODY="""local kcd2vrUseTwins30 = true
{indent}if user and user.actor and user.actor.IsPlayer and user.actor:IsPlayer()
{indent}    and System.GetCVar('wh_dlg_NoCameras') == 1
{indent}    and System.GetCVar('wh_dlg_ForcePOV') == 1 then
{indent}    kcd2vrUseTwins30 = false
{indent}end
{indent}self.actor:RequestDialog(user.id,'',false,kcd2vrUseTwins30)"""

def patch(name:str,data:bytes)->bytes:
 if name not in INPUTS or hashlib.sha256(data).hexdigest()!=INPUTS[name]:raise ValueError('exact retail OnTalk source fingerprint mismatch')
 text=data.decode('utf-8');assert text.count(CALL)==1
 at=text.index(CALL);start=text.rfind('\n',0,at)+1;indent=text[start:at];assert not indent.strip()
 # Preserve every other byte, including the original newline convention.
 newline='\r\n' if b'\r\n' in data else '\n'
 insert=BODY.format(indent=indent).replace('\n',newline)
 return text.replace(CALL,insert).encode('utf-8')

def main():
 import zipfile
 a=argparse.ArgumentParser(description=__doc__);a.add_argument('scripts',type=Path);a.add_argument('out',type=Path);p=a.parse_args()
 with zipfile.ZipFile(p.scripts) as z:
  for name in INPUTS:
   target=p.out/name;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(patch(name,z.read(name)));print(target)
if __name__=='__main__':main()
