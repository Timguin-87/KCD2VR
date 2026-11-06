"""Extract actual map-only producer ownership and existing qualified-fist body."""
from pathlib import Path
import sys,json,hashlib
root=Path(__file__).resolve().parents[1];out=Path(sys.argv[1]);out.mkdir(parents=True,exist_ok=True)
pose=(root/'src/vrik/coherent_pose.inl').read_text()
finger=(root/'src/subsystems/vrik/pose_fingers.inl').read_text()
a=pose.index('        const auto& supportOwnerM34=');b=pose.index('        if(!Compose6689(s))',a)
(out/'map_owner_m34_producer.inc').write_text(pose[a:b])
a=finger.index('static bool QualifiedCombatFistH34(');b=finger.index('static bool ApplyFingerLocals6689(',a)
(out/'map_owner_m34_fist.inc').write_text(finger[a:b])
(root/'review/M34').mkdir(exist_ok=True)
(out/'map_owner_m34_provenance.json').write_text(json.dumps({'sha256':{str(x.relative_to(root)):hashlib.sha256(x.read_bytes()).hexdigest() for x in [root/'src/vrik/coherent_pose.inl',root/'src/subsystems/vrik/pose_fingers.inl',root/'src/subsystems/vrmap/map_hand_owner_m34.h',Path(__file__)]},'scope':'Actual producer assignment+retained-support join and existing actual qualified fist. Test supplies explicit read-only native owner/mode/context leaves; physical support comes from actual physical::step.'},indent=2)+'\n')
print('M34 actual ownership/fist clauses extracted')
