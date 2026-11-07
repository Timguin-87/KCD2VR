from pathlib import Path
import sys,json
# Lupa is an optional isolated research dependency. This executes the actual embedded production Lua.
try:from lupa import LuaRuntime
except ImportError:
 if __name__=='__main__' and len(sys.argv)>1:
  sys.path.insert(0,sys.argv[1])
  from lupa import LuaRuntime
 else:
  import unittest
  raise unittest.SkipTest('optional Lua runtime is unavailable; run with the isolated research dependency')
repo=Path(__file__).resolve().parents[1]
s=(repo/'src/vrik/dagger_chest54_lua.inc').read_text().split('R"DAGGER54(\n',1)[1].rsplit(')DAGGER54"',1)[0]
L=LuaRuntime(unpack_returned_tuples=True)
L.execute(r"""
timers={} Script={SetTimer=function(ms,fn)table.insert(timers,fn)end}
logs={} entities={} items={} scans=0 binds=0 creates=0 destroys=0
System={GetEntity=function(id)return entities[id] end,LogAlways=function(s)table.insert(logs,s) end}
ItemManager={GetItem=function(id)return items[id] end}
function player(id)
 local v={id=id,atts={},inv={}}
 v.inventory={GetInventoryTable=function()scans=scans+1 return v.inv end,
  FindItem=function(_,cls)for _,w in pairs(v.inv)do if items[w] and items[w].class==cls then return w end end end}
 function v:GetAttachmentBone(slot,n)return self.atts[n] and self.atts[n].bone end
 function v:GetAttachmentCGF(slot,n)return self.atts[n] and self.atts[n].path end
 function v:CreateBoneAttachment(slot,b,n)creates=creates+1 self.atts[n]={bone=string.lower(b)} if self.afterCreate then self:afterCreate()end end
 function v:SetAttachmentCGF(slot,n,p)binds=binds+1 self.atts[n].path=p end
 function v:SetAttachmentPos(slot,n,p)self.atts[n].pos=p end
 function v:SetAttachmentAngles(slot,n,p)self.atts[n].angles=p end
 function v:DestroyAttachment(slot,n)destroys=destroys+1 self.atts[n]=nil end
 entities[id]=v return v
end
v=player(77) g_localActor=v
cls='3a640e5d-d8bd-4e8b-b61d-8cd5180e79e7'
items[101]={id=101,class=cls,amount=1,entity=0}
v.inv={101}
""")
L.execute(s)
L.execute(r"""
local S=KCD2VR_DaggerChest54
S.refresh(77,1,'kcd2vr_chest54_1','Spine3',true,true)
assert(binds==1 and scans==1 and v.atts.kcd2vr_chest54_1)
assert(S.key==101 and S.path=='objects/manmade/weapons/daggers/dagger_common.cgf')
for i=1,50 do S.refresh(77,1,'kcd2vr_chest54_1','Spine3',false,true)end
assert(scans==1 and binds==1)
-- Removing the actual item retires only our mesh, without another inventory scan.
v.inv={};S.refresh(77,1,'kcd2vr_chest54_1','Spine3',false,true)
assert(destroys==1 and not v.atts.kcd2vr_chest54_1 and not S.owner)
-- No fake prop when no carried dagger, unknown class or a real world/drawn entity exists.
v.inv={102};items[102]={id=102,class='unknown',entity=0};S.refresh(77,2,'n2','Spine3',true,true);assert(binds==1)
items[102]={id=102,class=cls,entity=123};S.refresh(77,2,'n2','Spine3',true,true);assert(binds==1)
-- Ambiguous different native WUIDs/classes are refused.
items[102]={id=102,class='36099b07-dae2-4b7b-bcb2-5b580e872ec3',entity=0};v.inv={101,102}
S.refresh(77,2,'n2','Spine3',true,true);assert(binds==1 and not S.owner)
-- A foreign pre-existing attachment is not replaced or deleted.
v.inv={101};v.atts.n2={bone='Spine3',path='foreign.cgf'}
S.refresh(77,2,'n2','Spine3',true,true);assert(binds==1 and destroys==1)
v.atts.n2=nil
S.refresh(77,2,'n2','Spine3',true,true);assert(binds==2)
-- A native/foreign rebind while held wins, even when the feature is disabled.
v.atts.n2.path='foreign.cgf';S.refresh(77,2,'n2','Spine3',false,true);assert(binds==2)
S.refresh(0,2,'n2','Spine3',false,false);assert(destroys==1 and v.atts.n2.path=='foreign.cgf')
-- ID reuse cannot delete an attachment on a new entity object.
v.atts.n2=nil;S.refresh(77,3,'n3','Spine3',true,true);assert(binds==3)
old=v;v=player(77);g_localActor=v;v.atts.n3={bone='Spine3',path='objects/manmade/weapons/daggers/dagger_common.cgf'}
S.refresh(0,3,'n3','Spine3',false,false);assert(destroys==1 and v.atts.n3)
-- Wrong script actor cannot borrow a valid-looking entity ID.
local proper=g_localActor;g_localActor=player(88);v.inv={101}
S.refresh(77,4,'wrong','Spine3',true,true);assert(binds==3 and not v.atts.wrong)
g_localActor=proper
-- Inventory can change during Create: remove our empty attachment; never bind a stale item.
v.inv={101};v.afterCreate=function(self)self.inv={}end
S.refresh(77,4,'n4','Spine3',true,true);assert(binds==3 and not v.atts.n4 and destroys==2)
-- Size cap prevents consuming an unbounded Lua table; it does not claim to cap native table creation.
v.afterCreate=nil;v.inv={}for i=1,513 do v.inv[i]=1000+i end
S.refresh(77,5,'n5','Spine3',true,true);assert(binds==3 and not S.owner)
-- A foreign removal of our bound CGF does not authorize deletion of the remaining attachment.
v.inv={101};S.refresh(77,6,'n6','Spine3',true,true);assert(binds==4)
v.atts.n6.path=nil;S.refresh(0,6,'n6','Spine3',false,false)
assert(destroys==2 and v.atts.n6)
-- Expired old lease cannot clear a renewed binding; latest lease expires only ours.
v.atts.n6=nil;S.refresh(77,7,'n7','Spine3',true,true);assert(binds==5)
local oldTimer=timers[#timers];S.refresh(77,7,'n7','Spine3',false,true)
oldTimer();assert(v.atts.n7 and destroys==2)
timers[#timers]();assert(not v.atts.n7 and destroys==3)
""")
print('dagger54 actual embedded Lua: carried WUID/model, no scan polling, no entity/physics/item duplication, revalidation, foreign rebind, id reuse, cap PASS')
