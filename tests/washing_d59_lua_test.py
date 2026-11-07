"""Exercise the shipped D59 adapter against actual retail descriptor/trigger code.
Python 3.12 + lupa.lua51; no game process or production file mutations.
"""
import sys,zipfile,hashlib,json
from pathlib import Path
from lupa.lua51 import LuaRuntime
base=Path(__file__).resolve().parents[1]
script=Path(sys.argv[2]) if len(sys.argv)>2 else base/'companion/source/Scripts/Startup/zzkcd2vr_wash_d59.lua'
pak=Path(sys.argv[1]) if len(sys.argv)>1 else Path(r'C:/Steam/steamapps/common/KingdomComeDeliverance2/Data/Scripts.pak')
names=['Scripts/Utils/InteractorAction.lua','Scripts/Entities/WH/Triggers/TriggerBase.lua','Scripts/Entities/WH/Triggers/ActionTrigger.lua','Scripts/Entities/WH/Triggers/WaterTubeActionTrigger.lua']
with zipfile.ZipFile(pak) as z: native={n:z.read(n) for n in names}
expected=[
 '3a94541c07d3444408425c75979ca7da3aba81e92ca6a05c7f52ad76dd3d33ce',
 'e80b87d775efe287f3a1b09733f99a5ea8564fc628858fb80c001501f9c5da89',
 '48b749fb188cb60bd7291ec6446c65f5b340f4f35cf37b3de6e7142e5441023e',
 'c74956e6c60fefe45de061ca52ddae7c7e7f2cd877f56840143a32d3a7ee87c0']
for n,wanted in zip(names,expected):
 assert hashlib.sha256(native[n]).hexdigest()==wanted, 'unqualified native script: '+n
def runtime():
 l=LuaRuntime(unpack_returned_tuples=True)
 l.execute("""
 Script={ReloadScript=function()end}
 function table.Merge(a,b)
   for k,v in pairs(b)do
     if a[k]==nil then if type(v)=='table' then a[k]={};table.Merge(a[k],v) else a[k]=v end
     elseif type(a[k])=='table' and type(v)=='table' then table.Merge(a[k],v) end
   end
 end
 inr_scriptTrigger=81; AHT_HOLD=2; INVALID_WUID={} -- Distinct native sentinel; its numeric value is not guessed.
 XGenAIModule={IsOneshotAvailable=function()return true end,IsOneshotBlocked=function()return false end,GetMyWUID=function()return 99 end,
 SendMessageToEntity=function() messages=messages+1 end,SendMessageToEntityArray=function()end}
 Framework={WUIDToMsg=function(n)return tostring(n)end}
 """)
 for data in native.values():l.execute(data.decode('utf-8-sig'))
 l.execute("""
 dirty=true; nativeCalls=0; messages=0; pressed=0;throwNative=false;replaceAtBounds=false;linkPresent=true;lastWuid=nil
 player={id=42,this={id=42},actor={IsMoreDirty=function()return dirty end,IsBodyMoreDirty=function()return dirty end,IsCarryingCorpse=function()return false end},
 soul={GetDerivedStat=function()return 0 end,IsInCombatDanger=function()return false end,HasScriptContext=function()return false end,IsInTenseCircumstance=function()return false end},inventory={}}
 so={id=900,this={id=900},smartObject={__this=900}}
 tub={id=99,Runtime={ClickAvailable=true,ClickMessage='wash',HoldAvailable=false,HoldMessage=''},Properties={}}
 table.Merge(tub.Properties,WaterTubeActionTrigger.Properties)
 setmetatable(tub,{__index=WaterTubeActionTrigger})
 tub.Properties.Click.esActionType='Animation';tub.Properties.Click.sAction='washFace'
 tub.Properties.Click.sActionTags='washFaceTub'
 tub.GetClass=function()return 'WaterTubeActionTrigger'end
 tub.CountLinks=function()return linkPresent and 1 or 0 end;tub.GetLink=function()return so,'' end
 tub.GetName=function()return 'native_tub'end
 tub.interactionTrigger={SetPressed=function()pressed=pressed+1 end}
 tub.GetWorldBBox=function()
   if replaceAtBounds then entities[99]={id=99}end
   return {x=-.3,y=-.3,z=.5},{x=.3,y=.3,z=1.0}
 end
 entities={[42]=player,[99]=tub,[900]=so};System={GetEntity=function(id)return entities[id]end}
 PlayerStateHandler={PlayAnimationAction=function(wuid,action)lastWuid=wuid;assert(action=='washFace');nativeCalls=nativeCalls+1;if throwNative then error('native unknown')end end}
 output={sentinel=true};inst={sentinel=true}
 """)
 l.execute(script.read_text())
 return l,l.globals().kcd2vr_wash_d59
l,f=runtime()
assert '|READY|99|900|' in f('PROBE|1|42|99')
assert l.eval('output.sentinel and inst.sentinel')
assert f('COMMIT|1|42|99')=='OK|1|DISPATCHED'
assert l.eval('nativeCalls==1 and messages==1 and pressed==1')
assert f('COMMIT|1|42|99')=='OK|1|SPENT' and l.eval('nativeCalls==1')
# Actual retail IsUsable/IsEnabled/GetActions allow Animation without an SO.
# The D62C adapter refused before any gesture could arm; this is its negative control.
l,f=runtime();l.execute('linkPresent=false')
assert l.eval('tub:IsUsable(player) and tub:IsEnabled(player)')
assert '|READY|99|0|' in f('PROBE|11|42|99'), 'D63: retail Animation trigger without link must be eligible'
assert f('COMMIT|11|42|99')=='OK|11|DISPATCHED'
assert l.eval('nativeCalls==1 and messages==0 and pressed==1 and lastWuid==INVALID_WUID')
assert f('COMMIT|11|42|99')=='OK|11|SPENT' and l.eval('nativeCalls==1')
l,f=runtime();l.execute('linkPresent=false');f('PROBE|12|42|99');l.execute('linkPresent=true')
assert f('COMMIT|12|42|99')=='OK|12|LEASE_REFUSED' and l.eval('nativeCalls==0')
l,f=runtime();f('PROBE|13|42|99');l.execute('linkPresent=false')
assert f('COMMIT|13|42|99')=='OK|13|LEASE_REFUSED' and l.eval('nativeCalls==0')
l,f=runtime();l.execute('linkPresent=false');f('PROBE|14|42|99');l.execute('dirty=false')
assert f('COMMIT|14|42|99')=='OK|14|NATIVE_REFUSED_IS_ENABLED' and l.eval('nativeCalls==0')
l,f=runtime();l.execute("linkPresent=false;tub.Runtime.ClickAvailable=false")
assert f('PROBE|15|42|99')=='OK|15|NATIVE_REFUSED_IS_USABLE' and l.eval('nativeCalls==0')
l,f=runtime();l.execute("so.smartObject=nil")
assert f('PROBE|16|42|99')=='OK|16|NATIVE_REFUSED_LINK' and l.eval('nativeCalls==0')
l,f=runtime();l.execute("linkPresent=false;tub.Properties.Click.bIsActiveInCombat=false;player.soul.IsInCombatDanger=function()return true end")
assert f('PROBE|17|42|99')=='OK|17|NATIVE_REFUSED_IS_ENABLED' and l.eval('nativeCalls==0')
l,f=runtime();l.execute('XGenAIModule.IsOneshotBlocked=function()return true end')
assert f('PROBE|18|42|99')=='OK|18|NATIVE_REFUSED_IS_ENABLED' and l.eval('nativeCalls==0')
l,f=runtime();assert '|READY|' in f('PROBE|2|42|99');l.execute('dirty=false')
assert f('COMMIT|2|42|99')=='OK|2|NATIVE_REFUSED_IS_ENABLED' and l.eval('nativeCalls==0')
l,f=runtime();f('PROBE|3|42|99');l.execute('so={id=901,smartObject={__this=901}}')
assert f('COMMIT|3|42|99')=='OK|3|LEASE_REFUSED' and l.eval('nativeCalls==0')
l,f=runtime();f('PROBE|4|42|99');l.execute('throwNative=true')
assert f('COMMIT|4|42|99')=='OK|4|NATIVE_UNKNOWN'
assert f('COMMIT|4|42|99')=='OK|4|SPENT' and l.eval('nativeCalls==1')
l,f=runtime();f('PROBE|5|42|99');assert f('CANCEL|5|42|99')=='OK|5|CANCELLED'
assert f('COMMIT|5|42|99')=='OK|5|LEASE_REFUSED' and l.eval('nativeCalls==0')
l,f=runtime();f('PROBE|6|42|99');l.execute('replaceAtBounds=true')
assert f('COMMIT|6|42|99')=='OK|6|IDENTITY_CHANGED' and l.eval('nativeCalls==0')
l,f=runtime();l.execute("tub.Properties.Click.sAction='sitting'")
assert f('PROBE|7|42|99')=='OK|7|NATIVE_REFUSED_ACTION'
l,f=runtime();l.execute("tub.Properties.Click.bIsActiveInCombat=false;player.soul.IsInCombatDanger=function()return true end")
assert f('PROBE|8|42|99')=='OK|8|NATIVE_REFUSED_IS_ENABLED'
l,f=runtime();l.execute('tub.Runtime.ClickAvailable=false')
assert f('PROBE|9|42|99')=='OK|9|NATIVE_REFUSED_IS_USABLE'
l,f=runtime();l.execute("tub.GetWorldBBox=function()return{x=0,y=0,z=0},{x=4,y=4,z=4}end")
assert f('PROBE|10|42|99')=='OK|10|VOLUME_UNAVAILABLE'
print(json.dumps({'result':'PASS','cases':18,'runtime':'Lua 5.1 actual retail descriptor/TriggerBase/ActionTrigger/WaterTubeActionTrigger','native_sha256':{n:hashlib.sha256(b).hexdigest() for n,b in native.items()}},indent=2))
