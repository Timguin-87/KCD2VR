"""Verify exact retail bytes behind H24's positive native block release timer policy."""
import argparse,hashlib,json,pathlib,struct
ap=argparse.ArgumentParser();ap.add_argument('binary',type=pathlib.Path);args=ap.parse_args()
proof=json.loads((pathlib.Path(__file__).resolve().parents[1]/'review/H24/NATIVE_RELEASE_TIMER_PROOF.json').read_text())
b=args.binary.read_bytes();assert hashlib.sha256(b).hexdigest()==proof['retailSha256'],'retail fingerprint mismatch'
pe=struct.unpack_from('<I',b,0x3c)[0];assert b[pe:pe+4]==b'PE\0\0'
count=struct.unpack_from('<H',b,pe+6)[0];optional=struct.unpack_from('<H',b,pe+20)[0];table=pe+24+optional
assert struct.unpack_from('<I',b,pe+8)[0]==int(proof['timestamp'],16)
def rva_bytes(rva,n):
 for i in range(count):
  at=table+40*i;virtual_size,virtual_address,raw_size,raw_offset=struct.unpack_from('<IIII',b,at+8)
  if virtual_address<=rva and rva+n<=virtual_address+raw_size:
   return b[raw_offset+rva-virtual_address:raw_offset+rva-virtual_address+n]
 raise AssertionError('unmapped exact RVA')
for pin in proof['pins']:
 assert rva_bytes(pin['rva'],pin['size']).hex()==pin['hex'],pin['purpose']
assert struct.unpack('<f',rva_bytes(0x27BCE21+0x18DD12B,4))[0]==0.0
assert struct.unpack('<f',rva_bytes(0x9D6D67+0x36C82ED,4))[0]==100000.0
print(json.dumps({'retailHash':'PASS','timestamp':'PASS','nativePins':len(proof['pins']),'zeroThreshold':'PASS','100000TicksPerSecond':'PASS'}))
