-- Read-only native chat suitability for one already nominated person or dog. Called by
-- the existing original-first dispatcher, never a world search or action call.
function kcd2vr_chat_target_d78(packet)
 if type(packet)~='string' or #packet>112 then return 'CHAT|BAD' end
 local sn,sp,se=packet:match('^CHAT|(%d+)|(%d+)|(%d+)$')
 local n,pid,eid=tonumber(sn),tonumber(sp),tonumber(se)
 if not n or n<1 or n>9007199254740991 or not pid or pid<1 or pid>4294967295 or not eid or eid<1 or eid>4294967295 then return 'CHAT|BAD' end
 local no='CHAT|'..sn..'|'..se..'|0'
 if not System or type(System.GetEntity)~='function' or not player or System.GetEntity(pid)~=player then return no end
 local user=player
 local e=System.GetEntity(eid)
 local kind=e and e.class
 local component=e and ((kind=='NPC' and e.human) or (kind=='Dog' and e.dog))
 if not e or e==user or not component or not e.actor or not BasicAIActions or
    type(BasicAIActions.GetChatActions)~='function' or e.GetChatActions~=BasicAIActions.GetChatActions or
    type(BasicAIActions.OnChat)~='function' then return no end
 local actor,actions,method,onChat=e.actor,BasicAIActions,e.GetChatActions,BasicAIActions.OnChat
 local okDead,dead=pcall(actor.IsDead,actor)
 local okUnconscious,unconscious=pcall(actor.IsUnconscious,actor)
 if not okDead or dead~=false or not okUnconscious or unconscious~=false then return no end
 local previousOutput,previousInst=output,inst
 local ok,rows=pcall(method,e,user)
 output,inst=previousOutput,previousInst
 if not ok or type(rows)~='table' then return no end
 local count=0
 for _,row in ipairs(rows) do
  if type(row)=='table' and row.action=='kcd2vr_chat_init_gesture' and row.enabled==true and row.func==onChat then count=count+1 end
 end
 if count~=1 or player~=user or System.GetEntity(pid)~=user or System.GetEntity(eid)~=e or e.actor~=actor or
    e.class~=kind or ((kind=='NPC' and e.human) or (kind=='Dog' and e.dog))~=component or
    BasicAIActions~=actions or actions.OnChat~=onChat or
    e.GetChatActions~=method or actions.GetChatActions~=method then return no end
 return 'CHAT|'..sn..'|'..se..'|1'
end
