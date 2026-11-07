"""Execute the shipped Lua module; mock only engine entity/slot/material leaves."""
from pathlib import Path
import unittest
import math
import zipfile
import xml.etree.ElementTree as ET
from lupa.lua51 import LuaRuntime
ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'mods/KCD2VRGore/limbs41/zzkcd2vr_gore_v6232.lua'
SKIN = 'objects/characters/humans/male/head/m_head_034/beards/m_beard_08.skin'
BASE = SKIN[:-5]

class GorePresentationD54(unittest.TestCase):
    def runtime(self):
        lua = LuaRuntime(unpack_returned_tuples=True)
        lua.execute(r'''
          entities={};logs={};spawned={};removed={};timers={};fail={};nextId=100
          function entity(id)
            local e={id=id,slots={},draw={},slotPositions={},slotAngles={},slotScales={},position={x=0,y=0,z=1.7}}
            function e:GetWorldPos()return self.position end
            function e:LoadCharacter(slot,path)
              if fail.loadThrow then error('load fault')end
              if fail.loadFalse then return false end
              self.slots[slot]=path;return slot
            end
            function e:IsSlotValid(slot)return self.slots[slot]~=nil end
            function e:IsSlotCharacter(slot)return self.slots[slot]~=nil end
            function e:SetMaterial(m)self.material=m end
            function e:AttachChild(child)
              if fail.attachThrow then error('attach fault')end
              if fail.attachFalse then return false end
              entities[child].parent=self
            end
            function e:SetLocalPos(p)
              if fail.positionThrow then error('position fault')end
              if fail.positionFalse then return false end
              self.localPosition=p
            end
            function e:SetLocalAngles(a)self.localAngles=a end
            function e:SetSlotPos(slot,p)
              if fail.slotPosition then return false end
              if slot==12 and self.slots[slot]=='custom.cgf' and fail.capPosition then return false end
              self.slotPosition=p;self.slotPositions[slot]=p
            end
            function e:SetSlotAngles(slot,a)
              if slot==12 and self.slots[slot]=='custom.cgf' and fail.capAngles then return false end
              self.slotAngles[slot]=a
            end
            function e:SetSlotScale(slot,s)
              if slot==12 and self.slots[slot]=='custom.cgf' then
                if fail.capScaleThrow then error('scale fault') end
                if fail.capScale then return false end
              end
              self.slotScales[slot]=s
            end
            function e:DrawSlot(slot,on)
              if slot==12 and self.slots[slot]=='custom.cgf' and on==1 and fail.capDraw then return false end
              self.draw[slot]=on
            end
            function e:LoadParticleEffect()end
            function e:Physicalize()end
            return e
          end
          System={LogAlways=function(s)logs[#logs+1]=s end,GetEntity=function(id)return entities[id]end,
            RemoveEntity=function(id)
              if not fail.remove then entities[id]=nil;removed[#removed+1]=id end
            end,
            SpawnEntity=function(p)
              local e=entity(nextId);nextId=nextId+1;e.name=p.name;e.position=p.position
              entities[e.id]=e;spawned[#spawned+1]=e;return e
            end}
          Script={SetTimer=function(ms,fn)timers[#timers+1]={ms,fn}end}
          head=entity(10);entities[10]=head
        ''')
        add, fly = lua.execute(SOURCE.read_text() + '\nreturn addParts,flyHead')
        lua.globals().addActual = add
        lua.globals().flyActual = fly
        lua.globals().skin = SKIN
        lua.globals().base = BASE
        lua.execute(r'''
          event={target=42,headSkin='objects/characters/humans/male/head/m_head_034/m_head_034.skin',
            materialTokenD54='0000000000000001',att={{n='beard_beardm_head_034_b00',s=skin,iD54=3}}}
          centre={x=0,y=0,z=1.65};kids={}
          KCD2VR_Gore.headPhysics=false;KCD2VR_Gore.neckCapCgf='';KCD2VR_Gore.headStumpCgf=''
        ''')
        return lua

    def run_add(self, lua):
        lua.execute('count=addActual(head,event,centre,kids)')
        return lua.globals().count

    def test_custom_head_cap_requires_loaded_slot_and_falls_back(self):
        for fault in ('false','empty','throw','none'):
            with self.subTest(fault=fault):
                l=self.runtime();l.globals().capFault=fault
                l.execute(r"""
                  local old=entity
                  function entity(id)
                    local e=old(id)
                    function e:LoadObject(slot,path)
                      if path=='custom.cgf' then
                        if capFault=='throw' then error('custom load fault') end
                        if capFault=='false' then return false end
                        if capFault=='empty' then return nil end
                      end
                      self.slots[slot]=path;return slot
                    end
                    return e
                  end
                  KCD2VR_Gore.headStumpCgf='custom.cgf';KCD2VR_Gore.neckCapCgf='fallback.cgf'
                  event.att={};event.p={x=0,y=0,z=1.7};result=flyActual(event,false)
                """)
                self.assertEqual(l.globals().spawned[1].slots[12], 'custom.cgf' if fault=='none' else 'fallback.cgf')
                self.assertTrue(l.globals().result)

    def cap_runtime(self, options=''):
        l=self.runtime()
        l.execute(r"""
          local old=entity
          function entity(id)
            local e=old(id)
            function e:LoadObject(slot,path)self.slots[slot]=path;return slot end
            return e
          end
          KCD2VR_Gore.headStumpCgf='custom.cgf';KCD2VR_Gore.neckCapCgf='fallback.cgf'
          event.att={};event.p={x=0,y=0,z=1.7}
        """)
        l.execute(options+'\nresult=flyActual(event,false)')
        self.assertTrue(l.globals().result)
        return l

    @staticmethod
    def rotated(v,a):
        x,y,z=v;cx,sx=math.cos(a[0]),math.sin(a[0]);cy,sy=math.cos(a[1]),math.sin(a[1]);cz,sz=math.cos(a[2]),math.sin(a[2])
        y,z=y*cx-z*sx,y*sx+z*cx  # x-axis rotation
        x,z=x*cy+z*sy,-x*sy+z*cy
        return (x*cz-y*sz,x*sz+y*cz,z)

    def test_custom_cap_default_centre_reaches_neck_target(self):
        l=self.cap_runtime();h=l.globals().spawned[1]
        self.assertEqual(h.slots[12],'custom.cgf');self.assertEqual(h.slotScales[12],1.3)
        C=(-.031076,.043906,1.576277);T=(0,.0025,1.382);head=(0,0,1.65)
        for i,k in enumerate('xyz'):
            self.assertAlmostEqual(h.slotPositions[12][k]+1.3*C[i],T[i]-head[i],places=10)
        self.assertTrue(any('FLY head stump placed' in str(x) and 'rendered=UNOBSERVED' in str(x) for x in l.globals().logs.values()))

    def test_custom_cap_angle_scale_and_offset_keep_target_centre(self):
        l=self.cap_runtime('KCD2VR_Gore.headStumpAngles={x=.3,y=-.4,z=1.2};KCD2VR_Gore.headStumpScale=2;KCD2VR_Gore.headStumpPos={x=.01,y=-.02,z=.03};KCD2VR_Gore.headStumpTarget={x=.02,y=.03,z=1.4}')
        h=l.globals().spawned[1];v=self.rotated(tuple(2*x for x in (-.031076,.043906,1.576277)),(.3,-.4,1.2))
        expected=(.03,.01,1.4-1.65+.03)
        for i,k in enumerate('xyz'):self.assertAlmostEqual(h.slotPositions[12][k]+v[i],expected[i],places=9)

    def test_custom_cap_transform_failure_uses_existing_fallback(self):
        for failure in ['capAngles','capScale','capScaleThrow','capPosition','capDraw']:
            with self.subTest(failure=failure):
                l=self.cap_runtime('fail.'+failure+'=true');h=l.globals().spawned[1]
                self.assertEqual(h.slots[12],'fallback.cgf');self.assertEqual(h.draw[12],1)
                self.assertFalse(any('FLY head stump placed' in str(x) for x in l.globals().logs.values()))

    def test_failed_custom_cap_without_fallback_is_hidden(self):
        l=self.cap_runtime("fail.capScale=true;KCD2VR_Gore.neckCapCgf=''")
        self.assertEqual(l.globals().spawned[1].draw[12],0)

    def test_nonfinite_tuning_does_not_reach_slot_transform(self):
        l=self.cap_runtime('KCD2VR_Gore.headStumpScale=0/0;KCD2VR_Gore.headStumpTarget={x=math.huge,y=0/0,z=-math.huge}')
        h=l.globals().spawned[1];self.assertEqual(h.slotScales[12],1.3)
        for k in 'xyz':self.assertTrue(math.isfinite(h.slotPositions[12][k]))

    def test_invalid_scale_or_overflow_uses_fallback(self):
        for options in ['KCD2VR_Gore.headStumpScale=0','KCD2VR_Gore.headStumpScale=-1','KCD2VR_Gore.headStumpScale=1e308;KCD2VR_Gore.headStumpCenter={x=1e308,y=1,z=1}']:
            with self.subTest(options=options):
                l=self.cap_runtime(options)
                self.assertEqual(l.globals().spawned[1].slots[12],'fallback.cgf')
                self.assertFalse(any('FLY head stump placed' in str(x) for x in l.globals().logs.values()))

    def test_female_head_retains_original_flesh_cap(self):
        l=self.cap_runtime('event.female=true');self.assertEqual(l.globals().spawned[1].slots[12],'fallback.cgf')

    def test_active_script_advertises_exact_material_protocol(self):
        l=self.runtime()
        self.assertIs(l.globals().KCD2VR_Gore.nativeMaterialD54,True)

    def load_generated_maps(self, lua):
        path=ROOT/'mods/KCD2VRGore/d54/kcd2vr_gore_d54_material_wrappers.pak'
        with zipfile.ZipFile(path) as pak:
            lua.execute(pak.read('Scripts/Startup/zzkcd2vr_gore_material_wrappers_d54.lua').decode())
        return path

    def test_generated_beard08_map_reaches_attachment_material(self):
        l=self.runtime();pakpath=self.load_generated_maps(l)
        self.assertEqual(self.run_add(l),1)
        kid=l.globals().kids[1]
        with zipfile.ZipFile(pakpath) as pak:
            cdf=ET.fromstring(pak.read(kid.slots[0]))
        material=cdf.find('.//Attachment').get('Material')
        self.assertEqual(material,'objects/characters/humans/kcd2vr_tint/m_beard_default_t2.mtl')
        self.assertIsNone(kid.material)

    def test_generated_native_beard08_material_bypasses_tint_without_copy_token(self):
        l=self.runtime();pakpath=self.load_generated_maps(l);l.execute('event.materialTokenD54=nil')
        native=l.globals().KCD2VR_Gore.PartMaterialD54[SKIN]
        l.globals().event.att[1].m=native
        self.assertEqual(self.run_add(l),1)
        kid=l.globals().kids[1]
        with zipfile.ZipFile(pakpath) as pak:
            cdf=ET.fromstring(pak.read(kid.slots[0]))
        self.assertEqual(cdf.find('.//Attachment').get('Material'),native)
        self.assertNotIn('kcd2vr_tint',native)

    def test_generated_runtime_copy_destination_keeps_coloured_fallback(self):
        l=self.runtime();pakpath=self.load_generated_maps(l)
        l.globals().event.att[1].m=l.globals().KCD2VR_Gore.PartMaterialD54[SKIN]
        self.assertEqual(self.run_add(l),1)
        kid=l.globals().kids[1]
        with zipfile.ZipFile(pakpath) as pak:
            cdf=ET.fromstring(pak.read(kid.slots[0]))
        self.assertEqual(cdf.find('.//Attachment').get('Material'),'objects/characters/humans/kcd2vr_tint/m_beard_default_t2.mtl')
        self.assertIsNone(kid.material)

    def test_actual_overlay_does_not_override_unknown_explicit_native_material(self):
        l=self.runtime();self.load_generated_maps(l)
        expected='objects/characters/humans/male/hair/other_existing_native.mtl'
        l.globals().event.materialTokenD54=None
        l.globals().event.att[1].m=expected
        self.assertEqual(self.run_add(l),1)
        self.assertEqual(l.globals().kids[1].material,expected)
        self.assertTrue(any('source=NATIVE_EVENT_NAME' in str(x) for x in l.globals().logs.values()))

    def test_generated_beard08_material_with_tint_disabled_is_bound_in_cdf(self):
        l=self.runtime();pakpath=self.load_generated_maps(l);l.execute('KCD2VR_Gore.hairTint=false')
        self.assertEqual(self.run_add(l),1)
        kid=l.globals().kids[1]
        with zipfile.ZipFile(pakpath) as pak:
            cdf=ET.fromstring(pak.read(kid.slots[0]))
        self.assertEqual(cdf.find('.//Attachment').get('Material'),l.globals().KCD2VR_Gore.PartMaterialD54[SKIN])
        self.assertIsNone(kid.material)

    def test_copy_token_keeps_coloured_fallback_until_native_transfer(self):
        l=self.runtime();l.execute('''
          event.att[1].m='native.mtl'
          KCD2VR_Gore.MaterialWrapperD54={[skin]={['native.mtl']=base..'.fly_native.cdf'}}
          KCD2VR_Gore.WrapperMtlD54={[base..'.fly_native_t2.cdf']='native_t2.mtl'}
          KCD2VR_Gore.TintWrapperD54={[base..'.fly_native.cdf']={base..'.fly_native_t0.cdf',base..'.fly_native_t1.cdf',base..'.fly_native_t2.cdf',base..'.fly_native_t3.cdf'}}
        ''');self.assertEqual(self.run_add(l),1)
        self.assertEqual(l.globals().kids[1].slots[0],BASE+'.fly_native_t2.cdf')
        self.assertIsNone(l.globals().kids[1].material)

    def test_token_missing_material_still_creates_native_destination(self):
        l=self.runtime();self.assertEqual(self.run_add(l),1)
        kid=l.globals().kids[1]
        self.assertEqual(kid.name,'kcd2vr_gore_d54_0000000000000001_part_3')
        self.assertEqual(kid.slots[0],BASE+'.fly.cdf')
        self.assertEqual(kid.parent.id,10)
        self.assertEqual(kid.localPosition.z,-1.65)
        self.assertEqual(kid.draw[0],1)
        self.assertIsNone(kid.material)

    def test_asset_tint_cdf_owns_skin_material(self):
        l=self.runtime();l.execute('''
          KCD2VR_Gore.TintWrapperD54={[base..'.fly.cdf']={base..'.fly_t0.cdf',base..'.fly_t1.cdf',base..'.fly_t2.cdf',base..'.fly_t3.cdf'}}
          KCD2VR_Gore.WrapperMtlD52={[base..'.fly_t2.cdf']='objects/characters/humans/kcd2vr_tint/m_beard_default_t2.mtl'}
        ''');self.assertEqual(self.run_add(l),1)
        self.assertEqual(l.globals().kids[1].slots[0],BASE+'.fly_t2.cdf')
        self.assertIsNone(l.globals().kids[1].material)

    def test_new_wrapper_map_survives_old_startup_table_assignment(self):
        l=self.runtime();l.execute("KCD2VR_Gore.WrapperMtlD54={[base..'.fly.cdf']='inherited.mtl'};KCD2VR_Gore.WrapperMtlD52={}")
        self.assertEqual(self.run_add(l),1)
        self.assertIsNone(l.globals().kids[1].material)
        self.assertTrue(any('material=inherited.mtl source=WRAPPER' in str(x) for x in l.globals().logs.values()))

    def test_exact_native_material_wrapper_wins_over_approximation(self):
        l=self.runtime();l.execute('''
          event.att[1].m='native.mtl'
          KCD2VR_Gore.MaterialWrapperD54={[skin]={['native.mtl']=base..'.fly_native.cdf'}}
          KCD2VR_Gore.WrapperMtlD52={[base..'.fly_native.cdf']='native.mtl'}
          KCD2VR_Gore.TintWrapperD54={[base..'.fly.cdf']={base..'.fly_t0.cdf'}}
        ''');self.assertEqual(self.run_add(l),1)
        self.assertEqual(l.globals().kids[1].slots[0],BASE+'.fly_native.cdf')
        self.assertIsNone(l.globals().kids[1].material)

    def test_no_exact_cdf_keeps_explicit_native_material(self):
        l=self.runtime();l.execute("event.att[1].m='native.mtl';KCD2VR_Gore.WrapperMtlD52={[base..'.fly.cdf']='default.mtl'}")
        self.assertEqual(self.run_add(l),1)
        self.assertEqual(l.globals().kids[1].material,'native.mtl')

    def test_complete_skin_map_supplies_missing_beard_entry(self):
        l=self.runtime();l.execute("event.materialTokenD54=nil;KCD2VR_Gore.PartMaterialD54={[skin]='beard_default.mtl'}")
        self.assertEqual(self.run_add(l),1)
        self.assertEqual(l.globals().kids[1].material,'beard_default.mtl')

    def test_explicit_part_material_off_retains_carrier_slot_mode(self):
        l=self.runtime();l.execute('KCD2VR_Gore.partMaterial=false')
        self.assertEqual(self.run_add(l),1)
        self.assertEqual(len(l.globals().spawned),0)
        self.assertEqual(l.globals().head.slots[2],BASE+'.fly.cdf')

    def test_tint_disabled_keeps_authored_wrapper(self):
        l=self.runtime();l.execute("KCD2VR_Gore.hairTint=false;KCD2VR_Gore.TintWrapperD54={[base..'.fly.cdf']={'bad.cdf','bad.cdf','bad.cdf','bad.cdf'}}")
        self.assertEqual(self.run_add(l),1)
        self.assertEqual(l.globals().kids[1].slots[0],BASE+'.fly.cdf')

    def test_forced_tint_index_uses_requested_asset(self):
        l=self.runtime();l.execute("KCD2VR_Gore.hairTintIndex=1;KCD2VR_Gore.TintWrapperD54={[base..'.fly.cdf']={'t0.cdf','t1.cdf','t2.cdf','t3.cdf'}}")
        self.assertEqual(self.run_add(l),1)
        self.assertEqual(l.globals().kids[1].slots[0],'t1.cdf')

    def test_bind_failures_use_existing_carrier_slot(self):
        for fault in ('attachThrow','attachFalse','positionThrow','positionFalse'):
            with self.subTest(fault=fault):
                l=self.runtime();l.globals().fail[fault]=True
                self.assertEqual(self.run_add(l),1)
                self.assertEqual(len(l.globals().kids),0)
                self.assertEqual(l.globals().head.slots[2],BASE+'.fly.cdf')
                self.assertEqual(l.globals().head.draw[2],1)
                self.assertEqual(l.globals().spawned[1].draw[0],0)
                self.assertEqual(len(l.globals().removed),1)

    def test_failed_cleanup_remains_registered_for_retry(self):
        l=self.runtime();l.execute('fail.attachFalse=true;fail.remove=true')
        self.assertEqual(self.run_add(l),1)
        self.assertTrue(l.eval('KCD2VR_Gore.spawned[spawned[1]]==false'))
        l.execute('fail.remove=false;KCD2VR_Gore.ReleaseAll()')
        self.assertTrue(l.eval('KCD2VR_Gore.spawned[spawned[1]]==nil'))

    def test_failed_fallback_position_is_not_reported_visible(self):
        l=self.runtime();l.execute('fail.attachFalse=true;fail.slotPosition=true')
        self.assertEqual(self.run_add(l),0)
        self.assertEqual(l.globals().head.draw[2],0)

    def test_failed_load_does_not_accept_stale_valid_slot(self):
        l=self.runtime();l.execute('fail.loadFalse=true')
        self.assertEqual(self.run_add(l),0)
        self.assertEqual(len(l.globals().kids),0)
        self.assertEqual(len(l.globals().removed),1)

    def test_invalid_token_has_no_native_named_receipt(self):
        for t in ('0000000000000000','1','000000000000000A','not_a_token_0001',42):
            with self.subTest(token=t):
                l=self.runtime();l.globals().event.materialTokenD54=t
                self.assertEqual(self.run_add(l),1)
                self.assertEqual(len(l.globals().spawned),0)
                self.assertEqual(l.globals().head.slots[2],BASE+'.fly.cdf')

    def test_invalid_index_has_no_native_named_receipt(self):
        for i in (0,25,1.5,'3'):
            with self.subTest(index=i):
                l=self.runtime();l.globals().event.att[1].iD54=i
                self.assertEqual(self.run_add(l),1)
                self.assertEqual(len(l.globals().spawned),0)

    def test_same_normalized_skin_is_not_duplicated(self):
        l=self.runtime();l.execute('event.att[2]={n="duplicate",s=string.upper(skin),iD54=4}')
        self.assertEqual(self.run_add(l),1)
        self.assertEqual(len(l.globals().spawned),1)

    def test_torso_cannot_join_head(self):
        l=self.runtime();l.execute('event.att[1].s="objects/characters/humans/male/torso/coat.skin"')
        self.assertEqual(self.run_add(l),0)
        self.assertEqual(len(l.globals().spawned),0)

    def test_full_head_has_matching_native_receipt_name_and_no_entity_override(self):
        l=self.runtime();l.execute("event.att={};event.p={x=0,y=0,z=1.7};event.headMtl='native_head.mtl';result=flyActual(event,false)")
        self.assertTrue(l.globals().result,list(l.globals().logs.values()))
        head=l.globals().spawned[1]
        self.assertEqual(head.name,'kcd2vr_gore_d54_0000000000000001_head')
        self.assertIsNone(head.material)
        self.assertIsNotNone(head.slots[1])

if __name__ == '__main__': unittest.main()
