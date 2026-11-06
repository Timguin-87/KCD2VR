from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1]
parts=[];proof=[]
def take(name,start,end):
    s=(root/name).read_text();a=s.index(start);b=s.index(end,a);text=s[a:b]
    parts.append(text);proof.append({'path':name,'start':start,'end':end,'sha256':hashlib.sha256(text.encode()).hexdigest()})
take('src/vrik/weapon_tracking.inl','static bool NativeHandKey6697(','// Cheap exclusion')
take('src/subsystems/physics/native_equipment_snapshot.inl','static bool PhysReadItemWorld54(','static bool PhysDescribeItem54(')
take('src/subsystems/physics/native_equipment_snapshot.inl','static bool PhysCachedItemMatches54(','// Metadata-refresh')
take('src/subsystems/physics/native_equipment_snapshot.inl','struct PhysLive103 {','static bool PhysSameLive103(')
data='\n'.join(parts).encode()
(root/'tests/reload_native_identity_xr34_actual.inc').write_bytes(data)
(root/'tests/reload_native_identity_xr34_provenance.json').write_bytes((json.dumps({'sha256':hashlib.sha256(data).hexdigest(),'slices':proof,'native_execution':False},indent=2)+'\n').encode())

s=(root/'src/subsystems/ranged_reload/reload_passenger_xr34.inl').read_text()
a=s.index('static xcXR34::Source CompletedReloadPoseSourceXR34(');b=s.index('static bool PrepareReloadPoseExactR93R3(',a)
(root/'tests/reload_completed_source_xr34_actual.inc').write_bytes(s[a:b].encode())

s=(root/'src/subsystems/ranged_reload/crossbow_rifle_request_h21.inl').read_text()
a=s.index('static bool RangedRegistrationReadyXR34()');b=s.index('// XR34 native reload source',a)
c=s.index('static void RangedPublishXR34()');d=s.index('// The existing CEntityAttachment callback',c)
(root/'tests/reload_registration_xr34_actual.inc').write_bytes((s[a:b]+s[c:d]).encode())

registration=[]
for name in ['reload_completed_source_xr34_actual.inc','reload_registration_xr34_actual.inc']:
 data=(root/'tests'/name).read_bytes();registration.append({'path':'tests/'+name,'sha256':hashlib.sha256(data).hexdigest()})
(root/'tests/reload_registration_xr34_provenance.json').write_bytes((json.dumps({'generated':registration,'native_execution':False},indent=2)+'\n').encode())
