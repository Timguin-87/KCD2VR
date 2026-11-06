# Read-only exact-retail initial DRAW token/outer-callback proof. No native execution.
from pathlib import Path
import argparse,struct,hashlib,json
ap=argparse.ArgumentParser();ap.add_argument('--binary',type=Path,default=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll'));ap.add_argument('--output',type=Path,required=True);a=ap.parse_args()
b=a.binary.read_bytes();pe=struct.unpack_from('<I',b,0x3c)[0];n=struct.unpack_from('<H',b,pe+6)[0];ts=struct.unpack_from('<I',b,pe+8)[0];op=pe+24;image=struct.unpack_from('<Q',b,op+24)[0];tab=op+struct.unpack_from('<H',b,pe+20)[0]
sha=hashlib.sha256(b).hexdigest();assert sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7' and ts==0x6A350E20
sections=[struct.unpack_from('<IIII',b,tab+40*i+8) for i in range(n)]
def at(rva,size):
 for _,va,raw,offset in sections:
  if va<=rva and rva+size<=va+raw:return b[offset+rva-va:offset+rva-va+size]
 raise ValueError(hex(rva))
pins={0x1F0D230:'40574883ec204c8b19488bf94d8b9330905b00',0x1F10AF0:'4883c108e937c7ffff',0x8D08F0:'48ff8140905b00498bf8488b9940905b00',0x8D0901:'4c8d459f4c8bf1488955a74881c130905b0048895d9f',0xABEE24:'48895c2410574883ec2033db488bf9',0x8D0926:'4c8975bf',0x8D092E:'48895dc7',0x1F0D27F:'498b5828',0x1F0D28A:'e8b96d37ff',0x1F0D29A:'e8adae7ffe',0x1F0D2C4:'48ff6010'}
for r,h in pins.items():assert at(r,len(bytes.fromhex(h)))==bytes.fromhex(h),hex(r)
assert struct.unpack('<Q',at(0x3EBC600+0x10,8))[0]==image+0x1F10AF0
col=struct.unpack('<Q',at(0x3EBC600-8,8))[0]-image;fields=struct.unpack('<6I',at(col,24));name=at(fields[3]+16,1000).split(b'\0')[0].decode();assert '<lambda_93d5487e02e78f713e5a16c0a27f419d>' in name and '@@X$$V' in name
ranges={'originalWorldJobSubmission':(0x8D08C8,0x8D098C),'originalOuterCompletionIncludingTailcall':(0x1F0D230,0x1F0D2CE),'originalPendingInsert':(0x8D098C,0x8D0A97),'originalPendingPredicate':(0xABEE24,0xABEE8E),'nativeOriginalDraw':(0x8CEA48,0x8CEA8B),'originalHandRequest':(0x8D0404,0x8D0528),'originalHandMembership':(0x8D06F0,0x8D07E9)}
out={'binary':str(a.binary),'sha256':sha,'timestamp':hex(ts),'status':'PASS','pins':{hex(r):h for r,h in pins.items()},'ABI':{'outer':'void(void* originalClosurePayload)','outerVft':'0x3EBC600','nativeRTTI':name,'payloadWorldManager':0,'payloadOriginalJobToken':8,'mid':'0x8D0901 RCX originalWorldManager, RDX originalC_Item, RBX allocatedJobToken'},'ranges':{k:{'start':hex(x),'end':hex(y),'sha256':hashlib.sha256(at(x,y-x)).hexdigest()} for k,(x,y) in ranges.items()},'limits':'Static exact-retail proof; no native callback/attachment was executed by this verifier.'}
a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(out,indent=2),newline='\n');print('H36R3 exact retail token and complete callback ABI PASS')
