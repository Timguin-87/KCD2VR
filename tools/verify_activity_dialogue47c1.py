"""Static asset/native pins verification; not native behavior-tree or game acceptance."""
from pathlib import Path
import argparse,zipfile,json,xml.etree.ElementTree as ET,hashlib,sys
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--game',type=Path,required=True);ap.add_argument('--out',type=Path,required=True);a=ap.parse_args()
 r=Path(__file__).resolve().parents[1];pak=r/'companion/Mods/KCD2VR/Data/KCD2VR.pak'
 key='AI/speech/getparametersfrompreset.xml'
 provenance=json.loads((r/'runtime/Mods/KCD2VRFirstPersonActivities/Data/KCD2VRFirstPersonActivities.json').read_text())
 with zipfile.ZipFile(pak) as new:
  assert set(new.namelist())=={key,'Libs/Config/defaultProfile.xml'}|{'Animations/Mannequin/ADB/'+name for name in provenance}
  for name,record in provenance.items():
   assert hashlib.sha256(new.read('Animations/Mannequin/ADB/'+name)).hexdigest()==record['output_sha256'],name
  patched=ET.fromstring(new.read(key))
 with zipfile.ZipFile(a.game/'Data/Scripts.pak') as z:
  original=ET.fromstring(z.read(key));caller=ET.fromstring(z.read('AI/speech/dialoginitiator_inner.xml'))
 originalTrees={n.get('name'):n for n in original.findall('BehaviorTree')}
 newTrees={n.get('name'):n for n in patched.findall('BehaviorTree')}
 assert set(originalTrees)==set(newTrees)
 for name,bt in originalTrees.items():
  if name!='speech_getParametersFromPreset':assert ET.tostring(bt)==ET.tostring(newTrees[name])
  else:
   seq=newTrees[name].find('Root/Behavior/Sequence')
   assert ET.tostring(bt.find('Root/Behavior')[0])==ET.tostring(seq[0])
   assert seq[1].tag=='ExecuteLua'
   assert seq[2].get('condition')=='$kcd2vrNoCameras and $preset==$enum:dialogPreset.fader'
 requests=list(caller.iter('RequestDialog'));assert requests
 assert any(n.get('UseTwins')=='$dialogParameters.useTwinsAsInitiator' for n in requests)
 binary=a.game/'Bin/Win64MasterMasterSteamPGO/WHGame.dll';digest=hashlib.sha256(binary.read_bytes()).hexdigest()
 assert digest=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
 # pefile is optional tooling; exact game inspection needs it.
 import pefile
 pe=pefile.PE(str(binary),fast_load=True)
 pins={0x602360:bytes.fromhex('488b4138c3'),0x59B0F0:bytes.fromhex('48895c24084889742410574883ec30'),0x9AF2D4:bytes.fromhex('48895c24185556574883ec50'),0x8DD068:bytes.fromhex('488bc448895808488970184889501057')}
 for rva,data in pins.items():assert pe.get_data(rva,len(data))==data,hex(rva)
 for vft in [0x3A399A8,0x3A6B9E8]:
  getter=int.from_bytes(pe.get_data(vft+0x18,8),'little')-pe.OPTIONAL_HEADER.ImageBase
  assert getter==0x602360,hex(vft)
 report={'activity_assets_byte_preserved':True,'original_preset_switch_preserved':True,'other_behavior_trees_preserved':True,'native_RequestDialog_useTwins_variable_verified':True,'WHGame_sha256':digest,'exact_pins':{hex(k):v.hex() for k,v in pins.items()},'puppet_getters_verified':['0x3A399A8','0x3A6B9E8'],'native_execution':False}
 a.out.write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':main()
