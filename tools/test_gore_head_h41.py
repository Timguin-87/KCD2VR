from pathlib import Path
import sys,argparse,json,hashlib
try:
 from lupa.lua51 import LuaRuntime
except ImportError:
 sys.path.insert(0,r'C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps')
 from lupa.lua51 import LuaRuntime
P=argparse.ArgumentParser();P.add_argument('--script',type=Path,default=Path(__file__).resolve().parents[1]/'mods/KCD2VRGore/zzkcd2vr_gore.lua');P.add_argument('--output',type=Path,required=True);a=P.parse_args();source=a.script.read_bytes()
leaves=r'''
logs={};timers={};entities={};removed={};spawned={};fail='';removeFails=false;removeNoEffect=false;reuseOnFailure=false
function entity(id)
 local e={id=id,slots={},particles=0,impulses=0,loads=0}
 function e:GetWorldPos() return {x=0,y=0,z=0} end
 function e:GetDirectionVector() return {x=0,y=1,z=0} end
 function e:LoadCharacter(slot,path) self.loads=self.loads+1;if fail=='load' then error('partial native load fault')end;if fail~='invalid' then self.slots[slot]=path end end
 function e:IsSlotValid(slot) if fail=='readback' then error('unreadable slot')end;return self.slots[slot]~=nil end
 function e:IsSlotCharacter(slot) return self.slots[slot]~=nil end
 function e:HideAttachment() end
 function e:DrawSlot(slot,on) self.carrier=on end
 function e:SetSlotPos() if fail=='position' then if reuseOnFailure then replacement=entity(self.id);entities[self.id]=replacement end;error('partial position fault')end end
 function e:LoadParticleEffect() self.particles=self.particles+1 end
 function e:AddImpulse() self.impulses=self.impulses+1;if fail=='impulse' then error('partial impulse fault')end end
 return e
end
player=entity(1);entities[1]=player
System={LogAlways=function(s)logs[#logs+1]=s end,GetEntity=function(id)return entities[id]end,
 RemoveEntity=function(id)if removeFails then error('remove threw')end;if not removeNoEffect then removed[#removed+1]=id;entities[id]=nil end end,
 SpawnEntity=function(p)local h=entity(100+#spawned);entities[h.id]=h;spawned[#spawned+1]=h;return h end}
Script={SetTimer=function(ms,fn)timers[#timers+1]={ms=ms,fn=fn}end}
function count(t)local n=0;for _ in pairs(t or {})do n=n+1 end;return n end
function fire(ms)local q={};for _,t in ipairs(timers)do if t.ms==ms then q[#q+1]=t.fn end end;for _,f in ipairs(q)do f()end end
'''
checks=[]
def run(name,setup,expr):
 l=LuaRuntime(unpack_returned_tuples=True);l.execute(leaves);l.execute(source.decode());l.execute(setup);ok=bool(l.eval(expr));checks.append({'case':name,'pass':ok});print(('PASS ' if ok else 'FAIL ')+name)
run('normal valid head remains active until exact LIFE timer',"KCD2VR_Gore.Test('head');before=#removed;fire(120000)","before==0 and #removed==1 and spawned[1].impulses==1 and count(KCD2VR_Gore.headCarriersH41)==0")
for f in ['invalid','readback','load','position','impulse']:
 run('partial '+f+' rollback exact own carrier',"fail='"+f+"';KCD2VR_Gore.Test('head')","#removed==1 and entities[100]==nil and count(KCD2VR_Gore.headCarriersH41)==0")
run('invalid slot refuses before effects or impulse',"fail='invalid';KCD2VR_Gore.Test('head')","spawned[1].impulses==0 and spawned[1].particles==0 and #timers==0")
run('failed removal retains debt and ReleaseAll retries actual own carrier',"fail='position';removeFails=true;KCD2VR_Gore.Test('head');retained=count(KCD2VR_Gore.headCarriersH41);removeFails=false;KCD2VR_Gore.ReleaseAll()","retained==1 and #removed==1 and count(KCD2VR_Gore.headCarriersH41)==0")
run('normal-return removal without readback effect is not success',"fail='position';removeNoEffect=true;KCD2VR_Gore.Test('head');retained=count(KCD2VR_Gore.headCarriersH41);removeNoEffect=false;KCD2VR_Gore.ReleaseAll()","retained==1 and #removed==1 and count(KCD2VR_Gore.headCarriersH41)==0")
run('failed rollback cancels already-queued effects while retaining cleanup',"fail='impulse';removeFails=true;KCD2VR_Gore.Test('head');before=spawned[1].particles;fire(500)","count(KCD2VR_Gore.headCarriersH41)==1 and spawned[1].particles==before")
run('ABA during partial failure never removes replacement',"fail='position';reuseOnFailure=true;KCD2VR_Gore.Test('head')","entities[100]==replacement and #removed==0 and count(KCD2VR_Gore.headCarriersH41)==0")
run('successful carrier ReleaseAll cancels old timers and reused ID effects',"KCD2VR_Gore.Test('head');KCD2VR_Gore.ReleaseAll();replacement=entity(100);entities[100]=replacement;fire(250);fire(500);fire(120000)","#removed==1 and entities[100]==replacement and replacement.impulses==0 and replacement.particles==0")
a.output.parent.mkdir(parents=True,exist_ok=True);receipt={'script':str(a.script.resolve()),'sha256':hashlib.sha256(source).hexdigest(),'runtime':'existing Lua5.1 through lupa.lua51','checks':checks,'passed':sum(x['pass']for x in checks),'total':len(checks),'native_engine':False,'headset':'NOT_RUN','scope':'Actual full Lua module, explicit entity/character/effect/timer leaves; carrier validity/cleanup only. Native C59 owns victim visibility. No complete gore/asset acceptance.'};a.output.write_text(json.dumps(receipt,indent=2)+'\n');raise SystemExit(0 if all(x['pass']for x in checks)else 1)
