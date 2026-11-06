-- Run with Lua 5.1: lua tests/quick_talk_target_d78_test.lua <source-root>
-- Executes the actual companion GetChatActions and read-only eligibility bridge.
-- Action builder and actor queries are explicit native leaves; no game runtime.
local root=KCD2VR_SOURCE_ROOT_D78 or (arg and arg[1]) or '.'
dofile(root..'/companion/source/Scripts/Entities/AI/Shared/BasicAIActions.lua')
dofile(root..'/companion/source/Scripts/Startup/zzkcd2vr_chat_target_d78.lua')
local methods={GetChatActions=BasicAIActions.GetChatActions,OnChat=BasicAIActions.OnChat}
local actions=BasicAIActions
local checks,nativeActions,descriptorQueries=0,0,0
local f,e,user,entities
local outputBefore,instBefore={sentinel='output'},{sentinel='inst'}
function Action()
 return setmetatable({}, {__index=function(t,key)
  return function(self,value) rawset(self,key=='interaction' and 'interactionName' or key,value);return self end
 end})
end
function AddInteractorAction(rows,first,row)
 if row.action=='kcd2vr_chat_init_gesture' then
  if f.wrongAction then row.action='chat_init_with_focus' end
  if f.wrongFunction then row.func=function()nativeActions=nativeActions+1 end end
  if f.disabled then row.enabled=false end
  if f.duplicate then rows[#rows+1]=row end
 end
 rows[#rows+1]=row
 return false
end
local function reset()
 f={canChat=true}
 BasicAIActions=actions;actions.GetChatActions=methods.GetChatActions;actions.OnChat=methods.OnChat
 e={id=77,class='NPC',human={}}
 user={id=33,human={},soul={},player={},actor={}}
 e.actor={}
 function e.actor:IsDead() if f.unknownDead then return nil end return f.dead==true end
 function e.actor:IsUnconscious() return f.unconscious==true end
 function e.actor:CanChat(id)
  descriptorQueries=descriptorQueries+1;assert(id==33)
  if f.error then error('explicit native query fault') end
  if f.targetChanged then entities[77]={id=77} end
  if f.playerChanged then player={id=33} end
  if f.actorChanged then e.actor={} end
  if f.componentChanged then e.dog={} end
  if f.classChanged then e.class='NPC';e.human={} end
  if f.methodChanged then e.GetChatActions=function()return {} end end
  if f.handlerChanged then actions.OnChat=function()nativeActions=nativeActions+1 end end
  return f.canChat
 end
 function e.actor:HasChatRequest() return f.reply==true end
 function e.actor:HasAcceptedChat() return false end
 function e.actor:IsWaitingForDialogueReply() return f.reply==true end
 function e.actor:DoChat() nativeActions=nativeActions+1 end
 function user.soul:IsInCombatDanger() return f.combat==true end
 function user.player:IsCombatChatTarget(id) assert(id==77);return f.combatTarget==true end
 function user.actor:CanFollow(id) assert(id==77);return f.follow==true end
 function user.actor:IsFollowing() return false end
 e.GetChatActions=methods.GetChatActions
 player=user;entities={[33]=user,[77]=e}
 System={GetEntity=function(id)return entities[id]end}
 output=outputBefore;inst=instBefore
end
local function check(expected,label,packet)
 checks=checks+1
 local reply=kcd2vr_chat_target_d78(packet or 'CHAT|1|33|77')
 assert(reply==expected,label..': '..tostring(reply))
 assert(output==outputBefore and inst==instBefore,label..': native descriptor globals leaked')
 assert(nativeActions==0,label..': eligibility called a native action')
end
reset();check('CHAT|1|77|1','current enabled native descriptor')
reset();f.follow=true;check('CHAT|1|77|1','follow alternative does not replace eligible chat')
reset();f.combat=true;f.combatTarget=true;check('CHAT|1|77|1','native combat target remains eligible')
for _,fault in ipairs({'dead','unknownDead','unconscious','wrongAction','wrongFunction','disabled','duplicate',
 'error','targetChanged','playerChanged','actorChanged','methodChanged','handlerChanged'}) do
 reset();f[fault]=true;check('CHAT|1|77|0',fault)
end
reset();f.canChat=false;check('CHAT|1|77|0','native CanChat false')
reset();f.canChat=false;f.reply=true;check('CHAT|1|77|0','reply descriptor is not init eligibility')
reset();f.combat=true;check('CHAT|1|77|0','native combat danger disables non-target descriptor')
reset();e.class='Dog';e.human=nil;e.dog={};check('CHAT|1|77|1','native Dog inherits exact enabled chat descriptor')
reset();e.class='Dog';e.human=nil;e.dog={};f.canChat=false;check('CHAT|1|77|0','Dog native CanChat refusal')
reset();e.class='Dog';e.human=nil;e.dog={};f.disabled=true;check('CHAT|1|77|0','Dog disabled native descriptor')
reset();e.class='Dog';e.human=nil;e.dog={};f.dead=true;check('CHAT|1|77|0','dead Dog')
reset();e.class='Dog';e.human=nil;e.dog={};f.componentChanged=true;check('CHAT|1|77|0','Dog component replaced during descriptor query')
reset();e.class='Dog';e.human=nil;e.dog={};f.classChanged=true;check('CHAT|1|77|0','Dog class replaced during descriptor query')
reset();e.class='Dog';check('CHAT|1|77|0','Dog lacks native dog component')
reset();e.class='Horse';check('CHAT|1|77|0','unsupported actor class')
reset();e.GetChatActions=function()error('foreign method must never run')end;check('CHAT|1|77|0','foreign descriptor method')
reset();e.actor.IsDead=nil;check('CHAT|1|77|0','missing native life query')
reset();entities[33]={id=33};check('CHAT|1|77|0','foreign current player')
reset();check('CHAT|BAD','malformed request','CHAT|1|33|77|extra')
reset();check('CHAT|BAD','non-exact serial range','CHAT|9007199254740992|33|77')
assert(descriptorQueries>0)
print('PASS actual Lua chat descriptor and eligibility: '..checks..' checks, native leaves substituted, native actions=0')
return checks
