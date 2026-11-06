"""Read-only game asset check for the bone both native and Lua supply require."""
from pathlib import Path
import hashlib, json, zipfile
ROOT=Path(__file__).resolve().parents[1]
pak=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Data/IPL_Characters-part3.pak')
member='Objects/characters/humans/male/skeleton/male.chr'
with zipfile.ZipFile(pak) as z:raw=z.read(member)
digest=hashlib.sha256(raw).hexdigest()
assert digest=='72d3ad022c34cab435cf9db0a617367b9b17ea682c938cab5ea0ece9aacc6aa6'
assert b'Hips\0' in raw and b'Pelvis\0' not in raw and b'pelvis\0' not in raw
cpp=(ROOT/'src/subsystems/throw_drop/native_supply_publication.inl').read_text()
lua=(ROOT/'src/vrik/native_supply_t2.lua').read_text()
assert 'ResolveJointByNameExactV6545(skeleton,"Hips",named)&&semantic==named' in cpp
assert "v:CreateBoneAttachment(0,'Hips',name)" in lua
assert "norm(v:GetAttachmentBone(0,b.name))=='hips'" in lua
assert "v:CreateBoneAttachment(0,'Pelvis'" not in lua
print(json.dumps({'result':'PASS','asset':member,'sha256':digest,'bone':'Hips',
    'native_semantic_equality_required':True,'lua_same_bone':True,'game_writes':0},indent=2))
