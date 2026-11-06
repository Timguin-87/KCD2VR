"""Exact-retail private door getter ABI proof. Read-only; no native execution."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse,hashlib,json,re,struct
from native_research112 import PE
ROOT=Path(__file__).resolve().parents[1]
SHA='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
def main():
 q=argparse.ArgumentParser();q.add_argument('dll',nargs='?',default='C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll');q.add_argument('--out');a=q.parse_args()
 p=PE(a.dll);source=read_source(ROOT/'src/door_signal19c4r4.cpp');checks=[]
 def ck(name,test):
  if not test:raise AssertionError(name)
  checks.append(name)
 def byte(name,rva,value):ck(name,p.read(rva,len(bytes.fromhex(value)))==bytes.fromhex(value))
 def call(rva,target):return p.read(rva,1)==b'\xe8' and rva+5+struct.unpack('<i',p.read(rva+1,4))[0]==target
 def rip(rva):return rva+7+struct.unpack('<i',p.read(rva+3,4))[0]
 ck('full retail SHA256',hashlib.sha256(p.data).hexdigest()==SHA);ck('timestamp',p.stamp==0x6a350e20)
 for n,rva in [('entry',0x41c388),('parser',0x41dc20),('end',0x2ab4c2c),('registration',0x144bfd2)]:
  raw=re.search(r'unsigned char '+n+r'\[\]\{([^}]+)',source).group(1);v=bytes(int(x,16) for x in re.findall('0x([0-9a-fA-F]+)',raw));ck('production pin '+n,p.read(rva,len(v))==v)
 byte('handler is native second argument; count virtual28',0x41c392,'488b02488bca488bfaff5028')
 byte('GetCVar requires exactly one param',0x41c39e,'488bcf83f801')
 byte('param1 output pointer and call',0x41c3aa,'4c8d44244848c7442448000000008bd0')
 ck('GetCVar calls ReadString41DC20',call(0x41c3ba,0x41dc20))
 byte('ReadString handler,param index,out pointer; string variant5',0x41dc2a,'488b01498bf84c8d442420c744242005000000ff5038')
 byte('ReadString type check and borrowed output copy',0x41dc40,'32db84c07411837c242005750a488b442428b301488907')
 ck('ReadString releases local variant through native destructor',call(0x41dc5c,0x714c64))
 byte('ReadString returns bool AL',0x41dc61,'8ac3')
 byte('EndUInt zero-extends uint32, variant4, float32 conversion',0x2ab4c32,'8b020f57c0488d542420c744242004000000f3480f2ac0')
 byte('EndUInt passes native float variant to handler EndFunction50',0x2ab4c49,'488b01f30f11442428ff5050')
 ck('EndUInt releases native local variant',call(0x2ab4c5c,0x714c64))
 byte('EndUInt returns native result count EAX',0x2ab4c61,'8bc3')
 byte('registration embeds function pointer in R8',0x144bfd2,'4c8d05af03fdfe');ck('registered target41C388',rip(0x144bfd2)==0x41c388)
 ck('registration packages exact callable',call(0x144bfe0,0x23d2a48))
 ck('GetCVar registration name',p.read(rip(0x144bfec),8)==b'GetCVar\0')
 ck('registration submits GetCVar descriptor',call(0x144c002,0xb9588c))
 for value in [0,1,2,1944,65535,999500,8388606,8388607]:ck('float32 exact protocol value '+str(value),struct.unpack('<f',struct.pack('<f',value))[0]==value)
 ranges=[(0x41c388,0x41c42d),(0x41dc20,0x41dc6e),(0x2ab4c2c,0x2ab4c69),(0x144bfd2,0x144c007)]
 result={'retail_sha256':SHA,'timestamp':hex(p.stamp),'checks':checks,'functions':[{'begin':hex(x),'end':hex(y),'sha256':hashlib.sha256(p.read(x,y-x)).hexdigest()} for x,y in ranges], 'native_execution':False}
 print('door signal ABI: %d/%d PASS'%(len(checks),len(checks)))
 if a.out:Path(a.out).write_text(json.dumps(result,indent=2)+'\n')
if __name__=='__main__':main()
