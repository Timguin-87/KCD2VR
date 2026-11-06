from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1]
s=(root/'src/subsystems/ranged_reload/crossbow_rifle_request_h21.inl').read_text()
types=(root/'src/frik_pose_probe.cpp').read_text()
types=types[types.index('struct RangedSocketXR34 {'):types.index('struct CommittedFrame6695 {')]
(root/'tests/crossbow_pose_types_r95_actual.inc').write_text(types)
s=s[s.index('// Calibration has no destination or timestamp.'):s.index('static bool RangedPinsH21(')]
(root/'tests/crossbow_pose_xr34_actual.inc').write_bytes(s.encode())
(root/'tests/crossbow_pose_xr34_provenance.json').write_bytes((json.dumps({'source':'src/subsystems/ranged_reload/crossbow_rifle_request_h21.inl','slice_sha256':hashlib.sha256(s.encode()).hexdigest(),'types_source':'src/frik_pose_probe.cpp','types_slice_sha256':hashlib.sha256(types.encode()).hexdigest(),'native_execution':False},indent=2)+'\n').encode())
