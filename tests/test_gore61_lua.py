"""Execute the shipped T3/H15 Lua against explicit entity/attachment/timer leaves."""
import unittest
from pathlib import Path
try:
    from lupa.lua51 import LuaRuntime
except ImportError:
    LuaRuntime = None
ROOT = Path(__file__).resolve().parents[1]

@unittest.skipUnless(LuaRuntime, 'optional Lupa Lua 5.1 runtime required')
class Gore61Lua(unittest.TestCase):
    def runtime(self):
        lua = LuaRuntime(unpack_returned_tuples=True)
        lua.execute("""
          logs={}; timers={}; entities={}; removed={}; spawned={};
          function entity(id)
            local e={id=id,slots={},destroys=0,particles=0,cgfs=0,effects=0}
            function e:GetWorldPos() return {x=0,y=0,z=0} end
            function e:GetBonePos() return {x=0,y=0,z=1.4} end
            function e:GetDirectionVector() return {x=0,y=1,z=0} end
            function e:CreateBoneAttachment(slot,bone,tag) self.bone=bone self.tag=tag end
            function e:SetAttachmentCGF() self.cgfs=self.cgfs+1 end
            function e:SetAttachmentEffect() self.effects=self.effects+1 end
            function e:DestroyAttachment() self.destroys=self.destroys+1 end
            function e:LoadCharacter(slot,path) self.slots[slot]=path end
            function e:IsSlotValid(slot) return self.slots[slot]~=nil end
            function e:IsSlotCharacter(slot) return self.slots[slot]~=nil end
            function e:SetSlotPos() end
            function e:DrawSlot(slot,on) self.carrier=on end
            function e:AddImpulse() self.impulses=(self.impulses or 0)+1 end
            function e:LoadParticleEffect() self.particles=self.particles+1 end
            function e:HideAttachment() error('victim head visibility stays native-owned') end
            return e
          end
          player=entity(1); victim=entity(42); entities[1]=player; entities[42]=victim
          System={LogAlways=function(s)table.insert(logs,s)end,
            GetEntity=function(id)return entities[id]end,
            GetEntitiesInSphere=function()return {victim}end,
            RemoveEntity=function(id)table.insert(removed,id);entities[id]=nil end,
            SpawnEntity=function(p)local h=entity(100+#spawned);entities[h.id]=h;table.insert(spawned,h);return h end}
          Script={SetTimer=function(ms,fn)table.insert(timers,{ms=ms,fn=fn})end}
          function cut(skin,parts)
            KCD2VR_Gore.Cut{target=42,cut='head',headSkin=skin or '',att=parts or {},p={x=0,y=0,z=1.4}}
          end
          function fire(ms)
            local pending={}
            for _,t in ipairs(timers)do if t.ms==ms then table.insert(pending,t.fn)end end
            for _,f in ipairs(pending)do f()end
          end
        """)
        lua.execute((ROOT/'gore_pak/Scripts/Startup/zzkcd2vr_gore.lua').read_text())
        return lua

    def test_default_stump_has_blood_and_no_mesh(self):
        l = self.runtime(); l.execute('cut()')
        v = l.globals().victim
        self.assertEqual(v.bone, 'Neck'); self.assertEqual(v.effects, 1)
        self.assertEqual(v.cgfs, 0)
        l.execute('fire(20000)'); self.assertEqual(v.destroys, 1)

    def test_reused_victim_id_receives_no_old_stump_cleanup(self):
        l = self.runtime(); l.execute('cut(); replacement=entity(42);entities[42]=replacement;fire(300);fire(3000);fire(20000)')
        self.assertEqual(l.globals().replacement.destroys, 0)
        self.assertEqual(l.globals().victim.destroys, 0)

    def test_old_same_entity_timer_cannot_remove_newer_stump(self):
        l = self.runtime(); l.execute("""
          cut(); cut(); cleanups={}
          for _,t in ipairs(timers)do if t.ms==20000 then table.insert(cleanups,t.fn)end end
          cleanups[1]()
        """)
        self.assertEqual(l.globals().victim.destroys, 0)
        l.execute('cleanups[2]()'); self.assertEqual(l.globals().victim.destroys, 1)
        l.execute('cleanups[2]()'); self.assertEqual(l.globals().victim.destroys, 1)

    def test_clear_retires_pending_stump_timer(self):
        l = self.runtime(); l.execute('cut();KCD2VR_Gore.Clear();fire(20000)')
        self.assertEqual(l.globals().victim.destroys, 1)

    def test_flying_head_and_helmet_use_wrappers(self):
        l = self.runtime(); l.execute("cut('objects/head/h.skin',{{n='helmet_m01_test',s='objects/hat/hat.skin'}})")
        h = l.globals().spawned[1]
        self.assertEqual(h.slots[1], 'objects/head/kcd2vr_fly.cdf')
        self.assertEqual(h.slots[2], 'objects/hat/hat.fly_m01.cdf')
        self.assertEqual(h.carrier, 0); self.assertEqual(h.impulses, 1)

    def test_reused_flying_head_id_receives_no_old_effect_or_removal(self):
        l = self.runtime(); l.execute("cut('objects/head/h.skin');replacement=entity(100);entities[100]=replacement;fire(500);fire(120000)")
        self.assertEqual(l.globals().replacement.particles, 0)
        self.assertEqual(len(l.globals().removed), 0)

    def test_limb_remains_observation_only(self):
        l = self.runtime(); l.execute("KCD2VR_Gore.Cut{target=42,cut='lh',energy=1,att={}}")
        self.assertEqual(l.globals().victim.effects, 0)
        self.assertEqual(len(l.globals().spawned), 0)
        self.assertTrue(any('NOT_SUPPORTED_YET' in str(s) for s in l.globals().logs.values()))

if __name__ == '__main__':
    unittest.main()
