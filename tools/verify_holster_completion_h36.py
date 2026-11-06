"""Read-only exact retail proof for H36R1 observed standard-cycle closure."""
from pathlib import Path
import sys,struct,hashlib,json,argparse
sys.path.insert(0,str(Path(__file__).resolve().parent));from native_research112 import PE
ap=argparse.ArgumentParser();ap.add_argument('--dll',type=Path,default=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll'));ap.add_argument('--output',type=Path,required=True);a=ap.parse_args()
p=PE(a.dll);sha=hashlib.sha256(p.data).hexdigest();assert p.stamp==0x6A350E20 and sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
ranges={'StowWrapper':(0x8CF158,0x8CF195),'StowBranch':(0x8CF198,0x8CF234),'DetachRequest':(0x8D02AC,0x8D0379),'SpecialStow':(0x1084A48,0x1084BC2),'OriginalDrawCallbackDispatch':(0x8D0848,0x8D08C7),'OriginalDrawCallback':(0x1F22AC0,0x1F22BA0),'SpecialDraw':(0x1084BDC,0x1084D95),'ActualDetach':(0x1E7FAF0,0x1E7FB1A),'ActualClearBinding':(0x9CF0A8,0x9CF14B),'BoneClearBinding':(0xAB9C64,0xAB9D3C),'FindAttachment':(0x1085F74,0x1085FD0),'StowedRegistry':(0x10850D0,0x10850FB),'StowedName':(0x10850B0,0x10850CD),'NativeAuthoredSlotEnumeration':(0x1084F38,0x10850AD),'ActorModelGetter':(0x1A81DD0,0x1A81DD8),'ActualAddBinding':(0x9DFAFC,0x9DFC5C)}
# Exact decoded RIP operands used by the reader; no virtual slot guesses.
def rip(at,length,disp):return at+length+struct.unpack('<i',p.read(at+disp,4))[0]
assert rip(0x10850D0,6,2)==0x5325660
assert rip(0x10850D6,7,3)==0x5325650
assert rip(0x10850E0,7,3)==0x5325648
assert rip(0x8D02CE,7,3)==0x40AC2A8
assert p.read(0xAB9CC8,8)==bytes.fromhex('48c7432000000000')
assert p.read(0x1A81DD0,8)==bytes.fromhex('488b81b0010000c3')
assert p.read(0x8CF193,2)==bytes.fromhex('ebf9') # lookup-null skips mov AL,1, contrary to donor claim
assert p.read(0x8CF18C,2)==bytes.fromhex('b001')
# Specific original callback has exactly one actual attach submission.
def direct_calls(start,end,target):
 return [x for x in p.calls(target) if start<=x<end]
assert direct_calls(0x1F22AC0,0x1F22BA0,0x1085178)==[0x1F22B5A]
assert direct_calls(0x1084BDC,0x1084D95,0x1085178)==[0x1084D41]
assert direct_calls(0x1084A48,0x1084BC2,0x1085178)==[0x1084B6C]
report={'retail':{'path':str(a.dll),'sha256':sha,'timestamp':hex(p.stamp)},'result':'PASS','ranges':{k:{'first':hex(x),'endExclusive':hex(y),'sha256':hashlib.sha256(p.read(x,y-x)).hexdigest()} for k,(x,y) in ranges.items()},'derivedData':{'stowedBegin':'0x5325648','stowedEnd':'0x5325650','stowedFirstId':'0x5325660','nativeHeldNames':'0x40AC2A8','leftHandNameEntry':'0x40AC2B0','boneBoundObjectOffset':'0x20','attachmentManagerOffset':'0x28','currentCharacterOffset':'0x70','entityAttachmentIdOffset':'0x8'},'permittedConclusion':'Original preexisting-world-link DRAW returns after submitting one original hand attachment. Actual new hand binding plus original stored binding gone closes that observed draw. Later original STOW return plus hand binding gone and real new stored binding (or ordinary world-link removal) closes the observed operation. No assumed scheduler ordering.','notProven':['Native async world-create first-draw callback closure','Arbitrary unobserved native successors','Native authored animation completion (existing body owner remains separate)','Retail native execution, headset physics or gameplay success']}
a.output.parent.mkdir(exist_ok=True,parents=True);a.output.write_text(json.dumps(report,indent=2),newline='\n');print(json.dumps({'result':'PASS','ranges':len(ranges),'output':str(a.output)}))
