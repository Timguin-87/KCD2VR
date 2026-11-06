import sys,unittest
from pathlib import Path
try:
 from lupa.lua51 import LuaRuntime
except ImportError:
 LuaRuntime=None
ROOT=Path(__file__).resolve().parents[1]
@unittest.skipUnless(LuaRuntime,'optional Lupa Lua runtime required')
class Gore55Lua(unittest.TestCase):
 def runtime(self,legacy=False):
  l=LuaRuntime(unpack_returned_tuples=True)
  l.execute("""
   BasicEntity={}; EntityCommon={TempPhysParams={},TempPhysicsFlags={}};
   EntityCommon.ApplyCollisionFiltering=function()end; EntityCommon.GetCollisionFiltering=function()return {}end;
   PE_RIGID=3;PE_STATIC=0;PE_ARTICULATED=4;PHYSICPARAM_FLAGS=1;
   pef_pushable_by_players=1;pef_never_break=2;pef_fixed_damping=4;ref_use_simple_solver=8;pef_override_impulse_scale=16;
   CryAction={IsImmersivenessEnabled=function()return 1 end};
   entities={};spawned={};logs={};timers={};native={physicalizations=0};
   System={LogAlways=function(s)table.insert(logs,s)end,GetEntity=function(id)return entities[id]end,RemoveEntity=function(id)entities[id]=nil end};
   Script={SetTimer=function(ms,fn)table.insert(timers,{ms=ms,fn=fn})end};
  """)
  # Execute actual retail SetupModel/PhysicalizeThis/PhysicalizeRigid bodies.
  l.execute((ROOT/'tests/fixtures55/gore_native_contract55.lua').read_text())
  l.execute("""
   System.SpawnEntity=function(p)
    local h={id=100,Properties=p.properties,bRigidBodyActive=true,position=p.position};setmetatable(h,{__index=BasicEntity});
    h.LoadObject=function()end;h.Physicalize=function(self,slot,ty,pp)native.physicalizations=native.physicalizations+1;self.physicsType=ty;self.mass=pp.mass end;
    h.SetPhysicParams=function()end;h.AwakePhysics=function(self,on)self.awake=on end;
    h.LoadCharacter=function(self,slot,path)self.character=path end;h.HideAllAttachments=function(self,slot,hide)self.allHidden=hide end;
    h.CreateSkinAttachment=function(self,slot,name,path)self.skin=path;self.skinName=name;self.skinHidden=self.allHidden end;
    h.HideAttachment=function(self,slot,name,hidden)assert(slot==1 and name==self.skinName);self.skinHidden=hidden end;h.DrawSlot=function(self,slot,on)self.carrierVisible=on end;
    h.SetSlotPos=function(self,slot,pos)self.slotOffset=pos end;h.GetWorldPos=function(self)return self.position end;
    h.AddImpulse=function(self,part,p,d,k,apply)self.impulse=k;self.impulseAfterPhysicalize=native.physicalizations>0 end;
    h.IsSlotValid=function()return true end;h.IsSlotCharacter=function(self)return self.character~=nil end;h:SetupModel();entities[h.id]=h;table.insert(spawned,h);return h;
   end;
   local v={id=42};v.CreateBoneAttachment=function(self,slot,bone,name)self.bone=bone end;
   v.SetAttachmentCGF=function()end;v.SetAttachmentPos=function(self,slot,tag,pos)self.stumpOffset=pos end;
   v.SetAttachmentEffect=function()end;v.HideAttachment=function()end;entities[42]=v;
  """)
  src=(ROOT/'gore_pak/Scripts/Startup/zzkcd2vr_gore.lua').read_text()
  if legacy:src=src.replace('bPhysicalize = true, bRigidBody = true, bPushableByPlayers = true','bPhysicalize = 1, bRigidBody = 1, bPushableByPlayers = 1')
  l.execute(src);l.execute("KCD2VR_Gore.Cut{target=42,cut='head',headSkin='head.skin',p={x=1,y=2,z=3},d={x=1,y=0,z=1},energy=1,att={}}")
  return l
 def test_actual_native_boolean_contract(self):
  l=self.runtime();h=l.globals().spawned[1]
  self.assertEqual(l.globals().native.physicalizations,1);self.assertEqual(h.physicsType,3);self.assertEqual(h.mass,4)
  self.assertEqual(h.awake,1);self.assertEqual(h.carrierVisible,0);self.assertTrue(h.impulseAfterPhysicalize);self.assertEqual(h.impulse,28)
  self.assertTrue(str(h.character).endswith('kcd2vr_fly.cdf'));self.assertIsNone(h.skin)
  self.assertIsNone(l.globals().entities[42].stumpOffset)
  self.assertEqual(l.globals().entities[42].bone,'Neck')
  self.assertTrue(any('mode=effect ok=true mesh=false' in str(v) for v in l.globals().logs.values()))
 def test_numeric_one_reproduces_native_nonphysical(self):
  l=self.runtime(True);self.assertEqual(l.globals().native.physicalizations,0);self.assertFalse(l.globals().spawned[1].impulseAfterPhysicalize)
 def test_pak_copy_preserves_every_mesh_and_refuses_inplace(self):
  import tempfile,zipfile,importlib.util
  spec=importlib.util.spec_from_file_location('gorepatch55',ROOT/'tools/patch_gore55.py');mod=importlib.util.module_from_spec(spec);spec.loader.exec_module(mod)
  with tempfile.TemporaryDirectory() as td:
   base=Path(td)/'old.pak';new=Path(td)/'new.pak'
   with zipfile.ZipFile(base,'w') as z:z.writestr('Scripts'+chr(92)+'Startup'+chr(92)+'zzkcd2vr_gore.lua','old');z.writestr('Objects/body/head.skin',b'mesh\x00\xff')
   original=base.read_bytes();r=mod.patch(base,new,ROOT/'gore_pak/Scripts/Startup/zzkcd2vr_gore.lua')
   self.assertEqual(r['preserved_count'],1);self.assertEqual(base.read_bytes(),original)
   with zipfile.ZipFile(new) as z:self.assertEqual(z.read('Objects/body/head.skin'),b'mesh\x00\xff')
   with self.assertRaises(ValueError):mod.patch(base,base,ROOT/'gore_pak/Scripts/Startup/zzkcd2vr_gore.lua')
if __name__=='__main__':unittest.main()
