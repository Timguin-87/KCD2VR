-- Current hand focus -> the game's localized AlchemyItem name and native HUD
-- entity-name notification. Read-only; no GetActions, Use or recipe mutation.
function kcd2vr_alchemy_name_d72(packet)
 if type(packet)~='string' or #packet>127 then return 'N72|BAD' end
 local sn,sp,se,st,sa=packet:match('^N72|(%d+)|(%d+)|(%d+)|(%d+)|([01])$')
 local n,pid,eid,slot=tonumber(sn),tonumber(sp),tonumber(se),tonumber(st)
 if not n or n<1 or n>9007199254740991 or not pid or pid>4294967295 or not eid or eid>4294967295 or not slot or slot>20 then return 'N72|BAD' end
 local function reply(s)return 'N72|'..sn..'|'..s end
 if not UIAction or type(UIAction.CallFunction)~='function' then return reply('NO_HUD') end
 local function clear()
  local ok,result=pcall(UIAction.CallFunction,'HUD',-1,'SetKcd2vrAlchemyNameD72','',false)
  return reply(ok and result~=false and 'CLEARED' or 'NO_HUD')
 end
 if sa=='0' then return clear() end
 if pid<1 or eid<1 or not System or type(System.GetEntity)~='function' or not player or System.GetEntity(pid)~=player then return clear() end
 local user=player
 local e=System.GetEntity(eid)
 if not e or not AlchemyItem or type(AlchemyItem.GetUsableName)~='function' or e.GetUsableName~=AlchemyItem.GetUsableName or
    not e.Properties or e.Properties.alchemy_type~=slot or e.nUserId~=user.id then return clear() end
 local ok,name=pcall(e.GetUsableName,e,0)
 if not ok or type(name)~='string' or #name<1 or #name>512 or name=='x' or name:find('%z') then return clear() end
 if player~=user or System.GetEntity(pid)~=user or System.GetEntity(eid)~=e or e.nUserId~=user.id or
    e.Properties.alchemy_type~=slot or e.GetUsableName~=AlchemyItem.GetUsableName then return clear() end
 local sent,result=pcall(UIAction.CallFunction,'HUD',-1,'SetKcd2vrAlchemyNameD72',name,true)
 -- Dispatch receipt only; headset/Scaleform pixels are not observed here.
 return reply(sent and result~=false and 'DISPLAYED' or 'NO_HUD')
end
