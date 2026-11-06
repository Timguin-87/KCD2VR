"""Actual shipped Lua callbacks; engine slot leaves substituted, no game."""
from pathlib import Path
import runpy,sys,json
R=Path(__file__).resolve().parents[1]
# Preserve and run the previous marker/player/close fixture first.
base=runpy.run_path(str(R/'tools/test_map_markers_d64.py'));l=base['l']
l.execute(r'''
actor.GetWorldPos=function()return{x=1000,y=2000,z=0}end
check(kcd2vr_map_t3_ready('R3|3|7|42|100'):find('HELPER_READY',1,true)~=nil)
local e=entities[100];e:Hide(0);e.draw={}
e.DrawSlot=function(self,slot,shown)self.draw[slot]=shown end
local packet='Q75|3|7|42|100|1|1100,2000|2,2,4,1000,2000;86,0,5,1200,2000;'
check(kcd2vr_map_quests_d75(packet)=='Q75|LIVE|3')
check(e.slots[3]=='Objects/kcd2vr/mapquests75/quest_side_blue.cgf')
check(e.slots[4]=='Objects/kcd2vr/mapquests75/quest_racing_red.cgf')
check(e.slots[5]=='Objects/kcd2vr/mapquests75/checkpoint.cgf')
check(e.draw[3]==1 and e.draw[4]==1 and e.draw[5]==1)
check(math.abs(e.pos[3].x-((1000-496)/2652-.5)*.55)<1e-8)
local old=loads;local oldPositions=positions;local draws=0
local drawSlot=e.DrawSlot;e.DrawSlot=function(self,slot,on)draws=draws+1;drawSlot(self,slot,on)end
check(kcd2vr_map_quests_d75(packet)=='Q75|LIVE|3'and loads==old and positions==oldPositions and draws==0)
local reordered='Q75|3|7|42|100|1|1100,2000|86,0,5,1200,2000;2,2,4,1000,2000;'
check(kcd2vr_map_quests_d75(reordered)=='Q75|LIVE|3'and loads==old and positions==oldPositions and draws==0)
check(e.slots[3]=='Objects/kcd2vr/mapquests75/quest_side_blue.cgf')
check(kcd2vr_map_quests_d75(packet)=='Q75|LIVE|3'and loads==old and positions==oldPositions and draws==0)
local pos=positions
check(kcd2vr_map_quests_d75(packet:gsub('Q75|3|','Q75|2|'))=='Q75|STALE'and positions==pos)
check(kcd2vr_map_quests_d75(packet:gsub('|7|42|','|8|42|'))=='Q75|STALE'and positions==pos)
check(kcd2vr_map_quests_d75(packet:gsub('|42|100|','|43|100|'))=='Q75|OWNER'and positions==pos)
check(kcd2vr_map_quests_d75(packet:gsub('|42|100|','|42|999|'))=='Q75|OWNER'and positions==pos)
e:Hide(1);check(kcd2vr_map_quests_d75(packet)=='Q75|HIDDEN'and positions==pos);e:Hide(0)
check(kcd2vr_map_quests_d75('Q75|3|7|42|100|1||2,2,4,20000,20000;')=='Q75|LIVE|0')
check((e.draw[3]==0 and e.pos[3].z==0) and (e.draw[4]==0 and e.pos[4].z==0) and (e.draw[5]==0 and e.pos[5].z==0))
check(kcd2vr_map_quests_d75('Q75|3|7|42|100|1||2,999,4,1000,2000;')=='Q75|LIVE|0')
check(kcd2vr_map_quests_d75('Q75|3|7|42|100|1||2,2,4,nan,2000;')=='Q75|MALFORMED')
check(kcd2vr_map_quests_d75('Q75|3|7|42|100|1||2,2,4,1000,2000')=='Q75|MALFORMED')
check(kcd2vr_map_quests_d75('Q75|3|7|42|100|1||;')=='Q75|MALFORMED')
check(kcd2vr_map_quests_d75('Q75|3|7|42|100|1||'..string.rep('2,2,4,1000,2000;',17))=='Q75|LIMIT')
check(kcd2vr_map_quests_d75(packet)=='Q75|LIVE|3')
check(kcd2vr_map_quests_d75('Q75|3|7|42|100|0||')=='Q75|LIVE|0'and (e.draw[3]==0 and e.pos[3].z==0))
check(kcd2vr_map_quests_d75(packet)=='Q75|LIVE|3')
-- Close/reopen of same helper never reveals old objectives before new capture.
check(kcd2vr_map_m31_intent('I31|4|0')=='I31|4|CLOSED')
check(kcd2vr_map_t3_ready('R3|5|7|42|100'):find('HELPER_READY',1,true)~=nil)
check((e.draw[3]==0 and e.pos[3].z==0) and (e.draw[4]==0 and e.pos[4].z==0) and (e.draw[5]==0 and e.pos[5].z==0))
e:Hide(0);packet=packet:gsub('Q75|3|','Q75|5|')
check(kcd2vr_map_quests_d75(packet)=='Q75|LIVE|3')
local original=e.LoadObject;e.LoadObject=function(self,slot,path)original(self,slot,path);error('native load fault')end
check(kcd2vr_map_quests_d75('Q75|5|7|42|100|1||1,0,0,1000,2000;')=='Q75|FAULT')
check((e.draw[3]==0 and e.pos[3].z==0) and (e.draw[4]==0 and e.pos[4].z==0) and (e.draw[5]==0 and e.pos[5].z==0))
e.LoadObject=function(self,slot,path)original(self,slot,path);kcd2vr_map_m31_intent('I31|6|0')end
check(kcd2vr_map_quests_d75('Q75|5|7|42|100|1||1,1,0,1000,2000;')=='Q75|FAULT')
check(e.hidden and (e.draw[3]==0 and e.pos[3].z==0))
e.LoadObject=original
-- A failed load BEFORE native mutation must not poison the desired-art cache.
check(kcd2vr_map_t3_ready('R3|7|7|42|100'):find('HELPER_READY',1,true)~=nil)
e:Hide(0)
local retry='Q75|7|7|42|100|1||2,2,4,1000,2000;'
check(kcd2vr_map_quests_d75(retry)=='Q75|LIVE|1')
e.LoadObject=function()error('native load failed before write')end
local changed='Q75|7|7|42|100|1||1,0,4,1000,2000;'
check(kcd2vr_map_quests_d75(changed)=='Q75|FAULT'and (e.draw[3]==0 and e.pos[3].z==0))
e.LoadObject=original;local retryLoads=loads
check(kcd2vr_map_quests_d75(changed)=='Q75|LIVE|1'and loads==retryLoads+1)
check(e.slots[3]=='Objects/kcd2vr/mapquests75/quest_main_red.cgf'and e.pos[3].z~=-1000)
-- Native private attachment trial retains its three-slot contract.
check(kcd2vr_map_t3_ready('R3N44|8|7|42|100'):find('HELPER_READY',1,true)~=nil)
check(kcd2vr_map_quests_d75('Q75|8|7|42|100|1||')=='Q75|STALE')
''')
print(json.dumps({'result':'PASS','checks_including_retained':l.globals().n,'actual_lua':True,'engine_leaves':'SUBSTITUTED','rendering':'NOT_RUN'}))
