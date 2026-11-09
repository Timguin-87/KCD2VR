-- R158 quest progression recorder (Shift+F5 in the DLL, developer mode). read-only.
-- the DLL asks once per second while Shift+F5 records (native_script_t2 bridge, game thread) and writes the answer
-- to KCD2VR.log. asks the game only the questions its own BasicAIActions.lua asks before a chat or a dialogue
-- (soul:IsInCombatDanger / IsInTenseCircumstance / IsInCombatMode / HasScriptContext / IsDialogRestricted,
-- actor:IsDead / IsUnconscious / CanTalk / CanChat / HasChatRequest / HasAcceptedChat / IsWaitingForDialogueReply,
-- player:IsCombatChatTarget, actor:CanFollow / IsFollowing), for Henry and for every NPC within 25 m.
-- no write, no action, no timer, no listener. every engine call is in pcall, a missing method reads '?'.
KCD2VR_QuestDiagR158=KCD2VR_QuestDiagR158 or {}
local Q=KCD2VR_QuestDiagR158
Q.RADIUS=25
Q.MAXNPC=12
Q.SCAN=1024
local function method(obj,name)
  if obj==nil then return nil end
  local ok,f=pcall(function() return obj[name] end)
  if ok and type(f)=='function' then return f end
  return nil
end
local function call(obj,name,...)
  local f=method(obj,name)
  if not f then return nil end
  local ok,v=pcall(f,obj,...)
  if ok then return v end
  return nil
end
local function b(v)
  if v==true then return '1' elseif v==false then return '0' end
  return '?'
end
local function word(v,limit)
  if type(v)~='string' then v=tostring(v) end
  v=string.gsub(v,'^userdata: ','')
  v=string.gsub(v,'[^%w_%-%.]','_')
  if #v>limit then v=string.sub(v,1,limit) end
  if v=='' then v='?' end
  return v
end
local function pos(e)
  local p=call(e,'GetWorldPos')
  if type(p)=='table' and type(p.x)=='number' and type(p.y)=='number' and type(p.z)=='number' then return p end
  return nil
end
local function num(v)
  if type(v)=='number' and v==v and v>-1e9 and v<1e9 then return string.format('%.0f',v) end
  return '?'
end
local function npcRow(p,e,d)
  local s,a,id=e.soul,e.actor,e.id
  local fields={
    'd='..string.format('%.1f',d),
    'dead='..b(call(a,'IsDead')),
    'unconscious='..b(call(a,'IsUnconscious')),
    'hp='..num(call(a,'GetHealth')),
    'combatMode='..b(call(s,'IsInCombatMode')),
    'combatDanger='..b(call(s,'IsInCombatDanger')),
    'arrangedFight='..b(call(s,'HasScriptContext','combat_arrangedFight')),
    'suppressedDialogInFight='..b(call(s,'HasScriptContext','combat_suppressedDialogInArrangedFight')),
    'canTalk='..b(call(a,'CanTalk',p.id)),
    'canChat='..b(call(a,'CanChat',p.id)),
    'chatRequest='..b(call(a,'HasChatRequest')),
    'acceptedChat='..b(call(a,'HasAcceptedChat')),
    'waitingReply='..b(call(a,'IsWaitingForDialogueReply')),
    'dialogRestricted='..b(call(s,'IsDialogRestricted',p.id)),
    'combatChatTarget='..b(call(p.player,'IsCombatChatTarget',id)),
    'canFollow='..b(call(p.actor,'CanFollow',id)),
    'following='..b(call(p.actor,'IsFollowing',id)),
  }
  return 'N|'..word(id,30)..'|'..word(call(e,'GetName') or '?',40)..'|'..table.concat(fields,' ')
end
function Q.Build(sn)
  local rows={'Q158|'..sn}
  local p=player
  if type(p)~='table' or p.soul==nil or p.actor==nil then
    rows[2]='P|player=NONE'
    return table.concat(rows,'\n')
  end
  local pp=pos(p)
  local found,scanned,query={},0,'OK'
  if not pp then query='NO_PLAYER_POS'
  elseif not System or type(System.GetEntitiesInSphere)~='function' then query='NO_QUERY'
  else
    local ok,list=pcall(System.GetEntitiesInSphere,pp,Q.RADIUS)
    if not ok or type(list)~='table' then query='QUERY_FAILED'
    else
      for _,e in ipairs(list) do
        scanned=scanned+1
        if scanned>Q.SCAN then query='SCAN_LIMIT' break end
        if type(e)=='table' and e~=p and e.class=='NPC' and e.actor~=nil and e.soul~=nil then
          local ep=pos(e)
          if ep then
            local dx,dy,dz=ep.x-pp.x,ep.y-pp.y,ep.z-pp.z
            found[#found+1]={e=e,d=math.sqrt(dx*dx+dy*dy+dz*dz)}
          end
        end
      end
    end
  end
  table.sort(found,function(x,y) return x.d<y.d end)
  rows[#rows+1]='P|combatDanger='..b(call(p.soul,'IsInCombatDanger'))..
    ' tenseCircumstance='..b(call(p.soul,'IsInTenseCircumstance'))..
    ' combatMode='..b(call(p.soul,'IsInCombatMode'))..
    ' dead='..b(call(p.actor,'IsDead'))..
    ' npcs25m='..#found..' query='..query
  for i=1,math.min(#found,Q.MAXNPC) do
    local ok,row=pcall(npcRow,p,found[i].e,found[i].d)
    if ok and type(row)=='string' then rows[#rows+1]=row end
  end
  if #found>Q.MAXNPC then rows[#rows+1]='E|'..(#found-Q.MAXNPC)..'_more_npcs_beyond_the_nearest_'..Q.MAXNPC end
  return table.concat(rows,'\n')
end
function kcd2vr_quest_diag_r158(packet)
  if type(packet)~='string' or #packet>40 then return 'Q158|BAD' end
  local sn=string.match(packet,'^Q158|(%d+)$')
  if not sn or tonumber(sn)==nil or tonumber(sn)<1 then return 'Q158|BAD' end
  local ok,out=pcall(Q.Build,sn)
  if ok and type(out)=='string' then return out end
  return 'Q158|'..sn..'\nE|lua_error_'..word(out,80)
end
