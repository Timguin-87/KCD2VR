"""Narrow in-memory H41 knife repair; never writes/installs a PAK."""
from pathlib import Path
import copy,hashlib
ROWS={
 'Scripts/Startup/zzkcd2vr_throwing_assets1.lua':('inventory_delivery_t26.lua','2d29dc4ac25e3b9248db9a69fd52a7ec5e870149923873ac2dabf216df73a8a6'),
 'Scripts/KCD2VR/native_throw_reservation_t20.lua':('native_throw_reservation_t20.lua','b81e545cd463761e1f9f1a8814ec0333cec69fe73edc72cbcafd0b0b05acd25b'),
}
def plan_members(members):
 result=copy.deepcopy(members)
 for member,(file,prior) in ROWS.items():
  if member not in members:raise ValueError('H41 knife member missing: '+member)
  updated=Path(__file__).with_name(file).read_bytes()
  old=members[member][1]
  if hashlib.sha256(old).hexdigest()!=prior and old!=updated:
   raise ValueError('Unknown knife revision, review before overlay: '+member)
  result[member]=(copy.copy(members[member][0]),updated)
 changed={n for n in result if result[n][1]!=members[n][1]}
 assert changed<=set(ROWS) and set(result)==set(members)
 assert all(result[n][1]==d for n,(_,d) in members.items() if n not in ROWS)
 return result,changed
