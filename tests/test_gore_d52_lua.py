"""Execute shipped Lua 5.1 cleanup and actual addParts body; native leaves mocked."""
import unittest
from pathlib import Path
from lupa.lua51 import LuaRuntime
ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'mods/KCD2VRGore/r93r3/zzkcd2vr_gore.lua'

class GoreD52(unittest.TestCase):
    def runtime(self):
        l=LuaRuntime(unpack_returned_tuples=True)
        l.execute('''
          entities={}; logs={}; timers={};
          System={LogAlways=function(s)table.insert(logs,s)end,GetEntity=function(id)return entities[id]end,
            RemoveEntity=function()end}; Script={SetTimer=function(ms,fn)table.insert(timers,fn)end};
          victim={id=42,destroyCount=0,showCount=0};entities[42]=victim
          function victim:DestroyAttachment(slot,name)
            self.destroyCount=self.destroyCount+1
            if self.throwDestroy then error('destroy failure') end
            if self.failDestroy then return false end
          end
          function victim:HideAttachment(slot,name,hidden)
            assert(hidden==false);self.showCount=self.showCount+1
            if self.failShow then return false end
          end
        ''')
        l.globals().flyHeadActual=l.execute(SOURCE.read_text()+'\nreturn flyHead')
        l.execute("KCD2VR_Gore.limb41Leases[victim]={made={'kcd2vr_cut_body_arms'},orig={'body_arms'}}")
        return l

    def test_false_destroy_keeps_original_hidden_and_debt(self):
        l=self.runtime();l.execute('victim.failDestroy=true;KCD2VR_Gore.ReleaseAll()')
        self.assertEqual(l.globals().victim.showCount,0)
        self.assertTrue(l.eval('KCD2VR_Gore.limb41Leases[victim]~=nil'))
        l.execute('victim.failDestroy=false;KCD2VR_Gore.ReleaseAll()')
        self.assertEqual(l.globals().victim.showCount,1)
        self.assertTrue(l.eval('KCD2VR_Gore.limb41Leases[victim]==nil'))

    def test_throw_destroy_retains_retry(self):
        l=self.runtime();l.execute('victim.throwDestroy=true;KCD2VR_Gore.ReleaseAll()')
        self.assertEqual(l.globals().victim.showCount,0)
        self.assertTrue(l.eval('KCD2VR_Gore.limb41Leases[victim]~=nil'))

    def test_failed_restore_does_not_destroy_twice(self):
        l=self.runtime();l.execute('victim.failShow=true;KCD2VR_Gore.ReleaseAll()')
        self.assertEqual(l.globals().victim.destroyCount,1)
        l.execute('victim.failShow=false;KCD2VR_Gore.ReleaseAll()')
        self.assertEqual(l.globals().victim.destroyCount,1)
        self.assertEqual(l.globals().victim.showCount,2)
        self.assertTrue(l.eval('KCD2VR_Gore.limb41Leases[victim]==nil'))

    def test_entity_id_reuse_never_mutates_replacement(self):
        l=self.runtime();l.execute('entities[42]={id=42};KCD2VR_Gore.ReleaseAll()')
        self.assertEqual(l.globals().victim.destroyCount,0)
        self.assertEqual(l.globals().victim.showCount,0)

    def part_runtime(self):
        l=self.runtime()
        l.execute('''
          function vec(p)return p end;function helperProps()return {} end
          function log(s)table.insert(logs,s)end
          head={id=10};function head:GetWorldPos()return {x=0,y=0,z=0}end
          function head:AttachChild()end
          function spawnGore(p)
            kid={id=11};function kid:LoadCharacter(slot,path)self.path=path end
            function kid:IsSlotValid()return true end;function kid:IsSlotCharacter()return true end
            function kid:SetMaterial(m)self.material=m end
            function kid:SetLocalPos()end;function kid:SetLocalAngles()end;function kid:DrawSlot()end
            return kid
          end
          KCD2VR_Gore.PartMtl={['helmet.skin']='objects/characters/humans/male/clothing/head/helmet/helmet_m01.mtl'}
          KCD2VR_Gore.WrapperMtlD52={['objects/characters/humans/male/clothing/head/helmet/helmet.fly_m04.cdf']='objects/characters/humans/male/clothing/head/helmet/helmet_m04.mtl'}
        ''')
        s=SOURCE.read_text();a=s.index('local function variantOf(');b=s.index("local BLOOD = 'collisions.combat.blood_head'",a)
        add=l.execute('local G=KCD2VR_Gore\n'+s[a:b]+'\nreturn addParts')
        l.globals().addPartsActual=add
        return l

    def test_wrapper_variant_not_replaced_by_table_default(self):
        l=self.part_runtime()
        l.execute("addPartsActual(head,{target=42,att={{n='helmet_m04_test',s='objects/characters/humans/male/clothing/head/helmet/helmet.skin'}}},{x=0,y=0,z=0},{})")
        self.assertIsNone(l.globals().kid.material)  # D54: authored CA_SKIN material is retained, not overridden on the entity
        self.assertTrue(any('source=WRAPPER' in str(x) for x in l.globals().logs.values()))

    def test_explicit_native_material_has_priority(self):
        l=self.part_runtime()
        l.execute("addPartsActual(head,{target=42,att={{n='helmet_m04_test',s='objects/characters/humans/male/clothing/head/helmet/helmet.skin',m='objects/characters/humans/male/clothing/head/helmet/native.mtl'}}},{x=0,y=0,z=0},{})")
        self.assertEqual(l.globals().kid.material,'objects/characters/humans/male/clothing/head/helmet/native.mtl')

    def test_complete_head_material_branch_reaches_spawn_completion(self):
        l=self.runtime()
        l.execute('''
          KCD2VR_Gore.headPhysics=false;KCD2VR_Gore.neckCapCgf='';KCD2VR_Gore.headStumpCgf=''
          function System.SpawnEntity(p)
            h={id=101,slots={},pos=p.position};entities[101]=h
            function h:LoadCharacter(slot,path)self.slots[slot]=path end
            function h:IsSlotValid(slot)return self.slots[slot]~=nil end
            function h:IsSlotCharacter(slot)return self.slots[slot]~=nil end
            function h:DrawSlot()end;function h:SetSlotPos()end;function h:LoadParticleEffect()end
            function h:GetWorldPos()return self.pos end;function h:Physicalize()end
            function h:SetMaterial(m)self.material=m end
            return h
          end
          headResult=flyHeadActual({target=42,headSkin='objects/characters/humans/male/head/test.skin',
            headMtl='objects/characters/humans/male/head/test_m04.mtl',att={},p={x=0,y=0,z=1.7}},false)
        ''')
        self.assertTrue(l.globals().headResult,list(l.globals().logs.values()))
        self.assertTrue(any('FLY head spawned' in str(x) for x in l.globals().logs.values()))

if __name__=='__main__':unittest.main()
