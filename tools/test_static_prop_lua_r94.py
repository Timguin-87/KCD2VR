#!/usr/bin/env python3
"""Execute actual R94 companion methods with explicit engine service spies.
Lua 5.4 host is not the retail Lua 5.1 VM; no engine physics is simulated.
"""
from pathlib import Path
from lua_host_r94 import LuaHost
ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'companion/Mods/kcd2vr_lab/Scripts/Startup/zzkcd2vr_lab.lua'
HARNESS=r'''
PE_RIGID=2
entity={id=7,class='BasicEntity',Properties={Physics={Mass=-1,bPhysicalize=true}},mass=0,physicalizations=0,awakes=0,impulses=0}
g_localActor={id=1}
local entities={[7]=entity,[1]=g_localActor}
System={GetCurrTime=function()return 100 end,GetEntity=function(id)return entities[id]end,
 GetCVar=function(name) if name:find('kcd2vr_small_contact_v1:',1,true)==1 then return admission or 0 end return 0 end}
function replaceEntity() entities[7]={id=7,class=entity.class} end
function entity:GetMass()return self.mass end
function entity:GetSlotCount()return self.slots or 1 end
function entity:IsSlotValid(slot)return slot==0 end
function entity:GetWorldPos()return {x=0,y=0,z=0}end
function entity:Physicalize(slot,kind,p)
 self.physicalizations=self.physicalizations+1;assert(slot==0 and kind==PE_RIGID and p.density==-1);self.mass=p.mass
end
function entity:AwakePhysics(on)assert(on==1);self.awakes=self.awakes+1 end
function entity:AddImpulse(part,point,direction,k,scale)
 self.impulses=self.impulses+1;assert(part==-1 and scale==1);self.lastPoint=point;self.lastImpulse=k
end
function prep(promote) return KCD2VR_Lab.PrepareSceneryR94(7,entity.class,.35,promote~=false)end
function push()return KCD2VR_Lab.PushPropR94(7,entity.class,{x=.05,y=0,z=0},{x=1,y=0,z=0},.12)end
function makeItem()
 entity.class='PickableItem'
 entity.item={CanUse=function()return true end,CanSteal=function()return false end,
 BelongsToDeadBody=function()return false end,GetId=function()return 55 end}
 Framework={WUIDToString=function(key)return '[Item-'..key..']'end}
 ItemManager={GetItem=function(key)return {id=key,class='not-a-hard-coded-contact-guid'}end}
end
'''
CASES={
 'static_promote_native_mass':"assert(prep());assert(entity.mass==.35 and entity.physicalizations==1 and entity.awakes==1)",
 'authored_small_mass_preserved':"entity.Properties.Physics.Mass=.75;assert(prep());assert(entity.mass==.75)",
 'authored_heavy_static_refused':"entity.Properties.Physics.Mass=100;assert(not prep());assert(entity.physicalizations==0)",
 'physics_explicitly_disabled':"entity.Properties.Physics.bPhysicalize=false;assert(not prep());assert(entity.physicalizations==0)",
 'existing_body_awake_only':"entity.mass=.4;assert(prep(false));assert(entity.physicalizations==0 and entity.mass==.4 and entity.awakes==1)",
 'rigid_recreation_refused':"entity.mass=.4;assert(not prep());assert(entity.physicalizations==0)",
 'static_without_promotion_refused':"assert(not prep(false));assert(entity.physicalizations==0 and entity.awakes==0)",
 'mass_unknown_refused':"entity.GetMass=nil;assert(not prep());assert(entity.physicalizations==0)",
 'mass_nan_refused':"entity.mass=0/0;assert(not prep());assert(entity.physicalizations==0)",
 'no_readback_no_hold':"entity.Physicalize=function(self)self.physicalizations=self.physicalizations+1 end;assert(not prep());assert(entity.awakes==0)",
 'explicit_native_refusal':"entity.Physicalize=function()return false end;assert(not prep());assert(entity.awakes==0)",
 'native_exception_refused':"entity.Physicalize=function()error('native')end;assert(not prep());assert(entity.awakes==0)",
 'multiple_slots_refused':"entity.slots=2;assert(not prep());assert(entity.physicalizations==0)",
 'invalid_slot_refused':"entity.IsSlotValid=function()return false end;assert(not prep());assert(entity.physicalizations==0)",
 'native_interaction_flag_retained':"entity.Properties.bUsable=true;assert(not prep());assert(entity.physicalizations==0)",
 'native_pickable_flag_retained':"entity.Properties.bPickable=1;assert(not prep());assert(entity.physicalizations==0)",
 'quest_native_retained':"entity.Properties.bQuestItem=true;assert(not prep());assert(entity.physicalizations==0)",
 'actor_refused':"entity.actor={};assert(not prep());assert(entity.physicalizations==0)",
 'native_item_refused_as_scenery':"entity.item={};assert(not prep());assert(entity.physicalizations==0)",
 'active_user_retained':"entity.user=g_localActor;assert(not prep());assert(entity.physicalizations==0)",
 'slot_callback_identity_change':"entity.GetSlotCount=function()replaceEntity();return 1 end;assert(not prep());assert(entity.physicalizations==0)",
 'physicalize_identity_change':"entity.Physicalize=function(self)self.mass=.35;replaceEntity()end;assert(not prep());assert(entity.awakes==0)",
 'awake_body_lost':"entity.AwakePhysics=function(self)self.mass=0 end;assert(not prep())",
 'fracture_static_not_recreated':"entity.class='BreakableObject';assert(not prep());assert(entity.physicalizations==0)",
 'fracture_existing_dynamic_usable':"entity.class='BreakableObject';entity.mass=.5;assert(prep(false));assert(entity.physicalizations==0)",
 'exact_rigidbody_light':"entity.class='RigidBodyLight';assert(prep());assert(entity.physicalizations==1)",
 'generic_light_protected':"entity.class='Light';assert(not prep());assert(entity.physicalizations==0)",
 'native_point_impulse':"entity.mass=.35;assert(push());assert(entity.impulses==1 and entity.lastPoint.x==.05 and entity.lastImpulse==.12)",
 'push_never_creates_body':"assert(not push());assert(entity.physicalizations==0 and entity.impulses==0)",
 'push_heavy_refused':"entity.mass=30;assert(not push());assert(entity.impulses==0)",
 'push_point_nan_refused':"entity.mass=.35;assert(not KCD2VR_Lab.PushPropR94(7,entity.class,{x=0/0,y=0,z=0},{x=1,y=0,z=0},.1));assert(entity.impulses==0)",
 'push_point_distant_refused':"entity.mass=.35;assert(not KCD2VR_Lab.PushPropR94(7,entity.class,{x=.66,y=0,z=0},{x=1,y=0,z=0},.1));assert(entity.impulses==0)",
 'push_explicit_refusal':"entity.mass=.35;entity.AddImpulse=function()return false end;assert(not push())",
 'push_wake_identity_change':"entity.mass=.35;entity.AwakePhysics=function()replaceEntity()end;assert(not push());assert(entity.impulses==0)",
 'native_pickable_push_only':"makeItem();entity.mass=.35;assert(push());assert(entity.impulses==1 and entity.physicalizations==0)",
 'stolen_item_native_retained':"makeItem();entity.mass=.35;entity.item.CanSteal=function()return true end;assert(not push());assert(entity.impulses==0)",
 'corpse_item_native_retained':"makeItem();entity.mass=.35;entity.item.BelongsToDeadBody=function()return true end;assert(not push());assert(entity.impulses==0)",
 'item_use_callback_identity_change':"makeItem();entity.mass=.35;entity.item.CanUse=function()replaceEntity();return true end;assert(not push());assert(entity.impulses==0)",
 'contact_native_weight_no_lua_list':"makeItem();admission=1250;assert(KCD2VR_Lab.TryPrepareContactR24(7));assert(entity.mass==1.25 and entity.physicalizations==1 and entity.impulses==0)",
 'contact_without_nomination_refused':"makeItem();admission=0;assert(not KCD2VR_Lab.TryPrepareContactR24(7));assert(entity.physicalizations==0)",
 'contact_native_weight_limit':"makeItem();admission=3001;assert(not KCD2VR_Lab.TryPrepareContactR24(7));assert(entity.physicalizations==0)",
 'contact_theft_native_retained':"makeItem();admission=300;entity.item.CanSteal=function()return true end;assert(not KCD2VR_Lab.TryPrepareContactR24(7));assert(entity.physicalizations==0)",
}
def main():
 source=SOURCE.read_text();passed=0
 for name,test in CASES.items():
  try:
   with LuaHost() as lua:lua.run(HARNESS+'\n'+source+'\n'+test,name)
   passed+=1;print('PASS '+name)
  except Exception as error:print('FAIL '+name+': '+str(error))
 print(f'{passed}/{len(CASES)} actual companion cases (Lua host; engine services substituted)')
 return 0 if passed==len(CASES) else 1
if __name__=='__main__':raise SystemExit(main())
