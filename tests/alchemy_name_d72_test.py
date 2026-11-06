"""Actual emitted AVM1 and shipped Lua name bridge; game UI calls are spies."""
from pathlib import Path
import argparse, hashlib, json, sys
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import patch_alchemy_name_d72 as p
import glyph_assets_d68_test as g
checks=0
def ck(ok,why):
 global checks;checks+=1
 if not ok:raise AssertionError(why)

class NameVM(g.VM):
 def __init__(self):
  super().__init__();self.native=[];self.root={p.ORIGINAL:self.native.append}
  self.enum={'NPC_NAME_TYPE_ENTITY':2}
 def run(self,code,scope=None,this=None):
  values={'_root':self.root,'Enum':self.enum};values.update(scope or {})
  return super().run(code,values,this)
 def invoke(self,fn,args,this=None):
  if callable(fn):fn(args);return None
  return super().invoke(fn,args,this)
 def native_name(self,idNum,typ,name):
  self.run(p.wrapper_body(),{'idNum':idNum,'type':typ,'name':name})
 def focus(self,name,active):self.run(p.callback_body(),{'name':name,'active':active})

def main():
 ap=argparse.ArgumentParser();ap.add_argument('--baseline',type=Path,required=True);ap.add_argument('--output',type=Path,required=True);a=ap.parse_args()
 before=a.baseline.read_bytes();after=(ROOT/'companion/source/Libs/UI/hud.gfx').read_bytes()
 ck(p.patch(before)==after,'pinned baseline exactly regenerates shipped movie')
 bh,bb=p.p.unpack(before);ah,ab=p.p.unpack(after)
 kept=[];count=0
 for kind,payload,raw in p.p.tags(ab):
  if kind==12 and payload==p.injection():count+=1
  else:kept.append(raw)
 ck(count==1 and bh==ah and b''.join(kept)==bb,'every preexisting HUD tag and stage preserved exactly')
 vm=NameVM()
 vm.native_name(0,9,'chat');vm.native_name(1,7,'follow');vm.native_name(2,2,'head')
 ck(vm.native==[[0,9,'chat'],[1,7,'follow'],[2,2,'head']],'inactive wrapper preserves all native calls/arguments')
 vm.focus('@herb_name',True)
 ck(vm.native[-1]==[2,2,'@herb_name'],'hand focus calls actual entity-name route only')
 ck(vm.calls[-1]==('setActionHintsPosition',[]),'native layout positions the inserted label')
 n=len(vm.native);vm.focus('@herb_name',True)
 ck(len(vm.native)==n,'unchanged refresh does not remove/reinsert label')
 vm.native_name(0,9,'other NPC');ck(vm.native[-1]==[0,9,'other NPC'],'chat unaffected while alchemy focus active')
 vm.native_name(2,2,'new head')
 ck(vm.native[-1]==[2,2,'@herb_name'] and vm.root[p.LAST_NAME]=='new head','native entity update preserves current override and newest restoration value')
 vm.focus('@other_herb',True);ck(vm.native[-1]==[2,2,'@other_herb'],'changed current hand target replaces name')
 vm.focus('',False);ck(vm.native[-1]==[2,2,'new head'],'exit restores latest actual native entity name')
 n=len(vm.native);vm.focus('',False);ck(len(vm.native)==n,'duplicate exit is inert')
 fresh=NameVM();fresh.focus('@fresh',True);ck(fresh.native[-1]==[2,2,'@fresh'],'new movie instance never inherits old override')
 fresh.focus('',False);ck(fresh.native[-1]==[2,2,''],'unknown prior native name clears only entity label')
 from lupa.lua51 import LuaRuntime
 lua=LuaRuntime(unpack_returned_tuples=True)
 lua.execute((ROOT/'companion/source/Scripts/Startup/zzkcd2vr_alchemy_name_d72.lua').read_text())
 lua.execute(r'''
 checks=0;calls={};gets=0;uses=0
 function ck(x)checks=checks+1;assert(x)end
 player={id=1};item={nUserId=1,Properties={alchemy_type=7}}
 AlchemyItem={GetUsableName=function(self,slot)gets=gets+1;ck(self==item and slot==0);return '@native_localized_herb' end}
 item.GetUsableName=AlchemyItem.GetUsableName
 System={GetEntity=function(id)if id==1 then return player elseif id==42 then return item end end}
 UIAction={CallFunction=function(...)calls[#calls+1]={...};return true end}
 ck(kcd2vr_alchemy_name_d72('N72|1|1|42|7|1')=='N72|1|DISPLAYED')
 ck(gets==1 and #calls==1 and calls[1][1]=='HUD' and calls[1][2]==-1 and calls[1][3]=='SetKcd2vrAlchemyNameD72' and calls[1][4]=='@native_localized_herb' and calls[1][5]==true)
 ck(kcd2vr_alchemy_name_d72('N72|2|0|0|0|0')=='N72|2|CLEARED');ck(calls[2][5]==false and gets==1)
 for _,p in ipairs({'','N72|0|1|42|7|1','N72|1|1|42|21|1','N72|1|1|42|7|2','N72|1|1|42|7|1junk','N72|9007199254740992|1|42|7|1',string.rep('x',128)})do
  local n=#calls;ck(kcd2vr_alchemy_name_d72(p)=='N72|BAD');ck(#calls==n)
 end
 item.nUserId=2;ck(kcd2vr_alchemy_name_d72('N72|3|1|42|7|1')=='N72|3|CLEARED');ck(gets==1);item.nUserId=1
 item.Properties.alchemy_type=8;ck(kcd2vr_alchemy_name_d72('N72|4|1|42|7|1')=='N72|4|CLEARED');ck(gets==1);item.Properties.alchemy_type=7
 item.GetUsableName=function()error('foreign function')end;ck(kcd2vr_alchemy_name_d72('N72|5|1|42|7|1')=='N72|5|CLEARED');ck(gets==1)
 item.GetUsableName=AlchemyItem.GetUsableName
 local old=AlchemyItem.GetUsableName
 AlchemyItem.GetUsableName=function()item.nUserId=2;return '@stale' end;item.GetUsableName=AlchemyItem.GetUsableName
 ck(kcd2vr_alchemy_name_d72('N72|6|1|42|7|1')=='N72|6|CLEARED');ck(calls[#calls][5]==false)
 AlchemyItem.GetUsableName=old;item.GetUsableName=old;item.nUserId=1
 UIAction.CallFunction=function()return false end;ck(kcd2vr_alchemy_name_d72('N72|7|1|42|7|1')=='N72|7|NO_HUD')
 UIAction.CallFunction=function()error('movie gone')end;ck(kcd2vr_alchemy_name_d72('N72|8|0|0|0|0')=='N72|8|NO_HUD')
 ck(uses==0)
 ''')
 report={'asset_avm1_checks':checks,'lua51_checks':lua.globals().checks,'baseline_sha256':hashlib.sha256(before).hexdigest(),'candidate_sha256':hashlib.sha256(after).hexdigest(),'native_ui_calls':'SPY','retail_runtime':'NOT_RUN','headset':'NOT_RUN'}
 a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
if __name__=='__main__':main()
