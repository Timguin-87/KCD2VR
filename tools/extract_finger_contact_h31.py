#!/usr/bin/env python3
"""Copy the actual final-writer owner guard and pose commit boundary for the host fixture."""
from pathlib import Path
import hashlib
import sys
root=Path(__file__).resolve().parents[1]
s=(root/'src/vrik/coherent_pose.inl').read_text()
out=Path(sys.argv[1]);out.mkdir(parents=True,exist_ok=True)
a=s.index('        if(!nativeFingerCapture&&!objectFingers&&freeContact164R1[side]){')
end='        }else stagedContact[side].springH31={};'
b=s.index(end,a)+len(end)
dispatch=s[a:b]
a=s.index('    tx.committed=pc::commit(')
b=s.index('\n    if(!CoherentOwnerCurrent57(tx))',a)
commit=s[a:b]
needle='        if(collisionMode)s.contact[side]=stagedContact[side];'
assert s.count(needle)==1
for name,body in [('finger_contact_dispatch_h31.inc',dispatch),('finger_contact_commit_h31.inc',commit+'\n    for(int side=0;side<2;++side){\n'+needle+'\n    }\n')]:
 (out/name).write_text('// Actual production slice SHA256 '+hashlib.sha256(body.encode()).hexdigest()+'\n'+body+'\n')
