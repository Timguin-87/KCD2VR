-- D59: controller caress -> current exact native Dog companion_bond descriptor.
-- Called only through the fingerprint-qualified original-first script context.
-- No animal physicalization, synthetic R3/crouch/lock, reward or pose mutation.
local high,terminal=0,nil
local function response(n,e,status) return 'PET|'..n..'|'..e..'|'..status end
function kcd2vr_pet_d59(packet)
 if type(packet)~='string' or #packet>112 then return 'PET|BAD' end
 local sn,sp,se=packet:match('^PET|(%d+)|(%d+)|(%d+)$')
 local n,pid,eid=tonumber(sn),tonumber(sp),tonumber(se)
 if not n or n<1 or n>9007199254740991 or not pid or pid<1 or pid>4294967295 or not eid or eid<1 or eid>4294967295 then return 'PET|BAD' end
 if terminal and n==terminal.n then
  if pid==terminal.pid and eid==terminal.eid then return terminal.reply end
  return response(sn,se,'SERIAL_IDENTITY_CHANGED')
 end
 if n<=high then return response(sn,se,'STALE') end
 high=n
 terminal={n=n,pid=pid,eid=eid,reply=response(sn,se,'UNKNOWN_NO_RETRY')}
 local function done(status) terminal.reply=response(sn,se,status);return terminal.reply end
 if not System or type(System.GetEntity)~='function' or not player or System.GetEntity(pid)~=player then return done('NO_CURRENT_PLAYER') end
 local user=player
 local e=System.GetEntity(eid)
 -- Require the actual Dog script table's descriptor producer and handler.
 -- Do not silently treat a human, horse, corpse or arbitrary use as petting.
 if not e or not Dog or not e.dog or not e.actor or e.GetActions~=Dog.GetActions or e.OnBonding~=Dog.OnBonding or type(Dog.GetActions)~='function' or type(Dog.OnBonding)~='function' then return done('NOT_NATIVE_DOG') end
 local actor,dog=e.actor,e.dog
 local priorOutput,priorInst=output,inst
 local ok,rows=pcall(e.GetActions,e,user,false)
 output,inst=priorOutput,priorInst
 if not ok or type(rows)~='table' then return done('DESCRIPTOR_UNAVAILABLE') end
 local selected,slot=nil,nil
 for i,row in ipairs(rows)do
  if type(row)=='table' and row.action=='companion_bond' and row.enabled==true and row.func==Dog.OnBonding then
   if selected then return done('AMBIGUOUS_DESCRIPTOR') end
   selected,slot=row,i
  end
 end
 if not selected then return done('NATIVE_INELIGIBLE') end
 -- GetActions performs CanInteractWith, IsPickpocketing, life/unconscious,
 -- IsBondable and CanBondNow checks. Rejoin exact entities after the getters.
 if player~=user or System.GetEntity(pid)~=user or System.GetEntity(eid)~=e or e.actor~=actor or e.dog~=dog or e.GetActions~=Dog.GetActions or selected.func~=e.OnBonding then return done('IDENTITY_CHANGED') end
 -- Spend before entry: synchronous exception or unknown return cannot repeat.
 terminal.reply=response(sn,se,'NATIVE_UNKNOWN_NO_RETRY')
 local entered=pcall(selected.func,e,user,slot)
 return done(entered and 'NATIVE_HANDLER_RETURNED' or 'NATIVE_UNKNOWN_NO_RETRY')
 -- OnBonding is a void handler: returned is not an accepted reward/action.
 -- KCD2 retains dog response, paired alignment, animation and all effects.
end
