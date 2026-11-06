from pathlib import Path
import sys,json,hashlib,difflib,xml.etree.ElementTree as ET,datetime
OUT=Path(sys.argv[1]).resolve();OUT.mkdir(parents=True,exist_ok=True);SRC=Path(__file__).resolve().parents[1];BASE=SRC/'review/H38_QUICK_TALK_HINT';OLD=BASE/'baseline'
sha=lambda b:hashlib.sha256(b).hexdigest()
sys.path.insert(0,'C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps')
from lupa.lua51 import LuaRuntime
sys.path.insert(0,str(SRC/'tools'));from native_research112 import PE
profile=SRC/'companion/source/Libs/Config/defaultProfile.xml';basic=SRC/'companion/source/Scripts/Entities/AI/Shared/BasicAIActions.lua'
pb,bb=(OLD/'before/companion/source/Libs/Config/defaultProfile.xml').read_bytes(),(OLD/'before/companion/source/Scripts/Entities/AI/Shared/BasicAIActions.lua').read_bytes();assert sha(pb)=='a9cff332eb765ee800ecc02f5387862d79fdd4ee09b909cfad834d30265bd010';assert sha(bb)=='6bed9a0fb429728dc01f7b296ddb53ed002a6fd2d15cf69e1b7bbc27436f669b'
pt,bt=pb.decode(),bb.decode()
needle='\t\t<action name="chat_init_interactor_press" onPress="1" keyboard="_keybinds_ref_" pspad="pad_r1">'
insert='\t\t<!-- Initial hidden Grip opener only: keep RB distinct from the visible focused X hint; the original X/RB action still owns native replies. -->\n\t\t<action name="kcd2vr_chat_init_gesture" onPress="1" xboxpad="xi_shoulderr" />\n'
assert pt.count(needle)==1;pa=pt.replace(needle,insert+needle)
old='Action():action("chat_init_interactor_press"):hintType(AHT_PRESS):uiOrder(5):func(BasicAIActions.OnChat):'
new='Action():action("kcd2vr_chat_init_gesture"):hintType(AHT_PRESS):uiOrder(5):func(BasicAIActions.OnChat):'
assert bt.count(old)==1;ba=bt.replace(old,new)
assert profile.read_bytes()==pa.encode() and basic.read_bytes()==ba.encode(), 'Actual source differs from reviewed candidate'
patch=''
for rel,before,after in [('companion/source/Libs/Config/defaultProfile.xml',pt,pa),('companion/source/Scripts/Entities/AI/Shared/BasicAIActions.lua',bt,ba)]:
 patch+=''.join(difflib.unified_diff(before.splitlines(keepends=True),after.splitlines(keepends=True),fromfile='a/'+rel,tofile='b/'+rel))
 for kind,s in [('before',before),('after',after)]:
  p=OUT/kind/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(s,newline='')
(OUT/'initial_hidden_rb_alias.patch').write_text(patch,newline='')
builder=(BASE/'RETAIL_InteractorAction.lua').read_bytes();(OUT/'RETAIL_InteractorAction.lua').write_bytes(builder)
fixture='''BasicAIActions={};AHT_PRESS=1;AHC_CHAT=3;AHC_FOLLOW=4;inr_chatFollow=42
DebugUtils={Log=function()end};EntityUtils={GetName=function(e)return tostring(e.id)end}
function make(danger,target,can,follow,request,accepted,waiting)
 local n={id=11,events={}};n.actor={CanChat=function()return can end,HasChatRequest=function()return request end,IsWaitingForDialogueReply=function()return waiting end,HasAcceptedChat=function()return accepted end,
 DoChat=function(_,id,focused)table.insert(n.events,{kind='DO_CHAT',id=id,focused=focused})end,
 AcceptChatRequest=function()table.insert(n.events,{kind='ACCEPT_REQUEST'})end,
 OpenChat=function(_,id)table.insert(n.events,{kind='OPEN_CHAT',id=id})end}
 local u={id=1,soul={IsInCombatDanger=function()return danger end},player={IsCombatChatTarget=function(_,id)assert(id==11);return target end},actor={CanFollow=function()return follow end,IsFollowing=function()return false end,StartFollow=function(_,id)table.insert(n.events,{kind='FOLLOW',id=id})end}}
 return n,u
end'''
def runtime(text):
 a=text.index('function BasicAIActions:GetChatActions(user)');z=text.index('function BasicAIActions:OnStealthKill(',a)
 lua=LuaRuntime(unpack_returned_tuples=True);lua.execute(builder.decode());lua.execute(fixture);lua.execute(text[a:z]);return lua,text[a:z]
def rows(text=ba,**args):
 vals=[args.get(k,False) for k in ['danger','target']]+[args.get('can',True),args.get('follow',False),args.get('request',False),args.get('accepted',False),args.get('waiting',False)]
 lua,actual=runtime(text);n,u=lua.globals().make(*vals);o=lua.globals().BasicAIActions.GetChatActions(n,u)
 return lua,n,u,[row for _,row in o.items()]
def bindings(root,mp,act):
 e=root.find(f".//actionmap[@name='{mp}']/action[@name='{act}']");assert e is not None,(mp,act)
 return ([e.attrib['xboxpad']] if 'xboxpad'in e.attrib else [])+[x.attrib['input'] for x in e.findall('./xboxpad/inputdata')]
oldroot,newroot=ET.fromstring(pb),ET.fromstring(pa)
def select(rs,root):
 # Static model only; exact native instruction pins independently supplied.
 result=[]
 for r in sorted(rs,key=lambda r:r['action']):
  keys=set(bindings(root,r['actionMap'],r['action']))
  if not any(r['hintType']==k['hintType'] and keys.intersection(bindings(root,k['actionMap'],k['action'])) for k in result):result.append(r)
 return [r for r in sorted(result,key=lambda r:r['uiOrder'],reverse=True) if r['uiVisible'] is not False]
cases=[]
def check(name,condition,detail):cases.append(dict(case=name,passed=bool(condition),detail=detail));assert condition,name
lua,n,u,rs=rows();oldrs=rows(bt)[3]
check('initial_actual_native_callbacks_preserved',rs[0]['action']=='chat_init_with_focus' and rs[1]['action']=='kcd2vr_chat_init_gesture' and rs[1]['uiVisible']==False,'Actual GetChatActions and native retail builder')
check('old_initial_static_binding_drops_x',not select(oldrs,oldroot),'Exact-retail static semantic witness only, not live native execution')
check('new_initial_static_binding_keeps_x',any(r['action']=='chat_init_with_focus' for r in select(rs,newroot)),'No same-key conflict with RB-only hidden initial alias')
check('visible_initial_focused_callback',rs[0]['enabled']==True and rs[0]['func'](n,u,0) is None and n.events[1]['kind']=='DO_CHAT' and n.events[1]['focused']==True and n.events[1]['id']==1,'Actual OnChatWithFocus calls original DoChat(user,true), explicit native API leaf')
rs[1]['func'](n,u,0)
check('gesture_initial_unfocused_callback',n.events[2]['kind']=='DO_CHAT' and n.events[2]['focused']==False and n.events[2]['id']==1,'Actual OnChat calls original DoChat(user,false), no outcome invented')
for request,accepted,name,event in [(True,False,'request','ACCEPT_REQUEST'),(False,True,'accepted','OPEN_CHAT'),(True,True,'request_precedes_accepted','ACCEPT_REQUEST')]:
 l,n2,u2,r2=rows(can=False,request=request,accepted=accepted,waiting=True)
 check('reply_'+name+'_x_binding',len(r2)==1 and r2[0]['action']=='chat_init_interactor_press' and r2[0]['hint']=='@ui_hud_chat_reply' and bindings(newroot,r2[0]['actionMap'],r2[0]['action'])==['xi_x','xi_shoulderr'],'Original reply action, labels, order5 and X/RB unchanged')
 r2[0]['func'](n2,u2,0);check('reply_'+name+'_callback',n2.events[1]['kind']==event and (event!='OPEN_CHAT' or n2.events[1]['id']==1),'Actual original native reply callback, explicit leaf only')
check('not_waiting_no_reply',not rows(can=False,request=True,waiting=False)[3] and not rows(can=False,accepted=True,waiting=False)[3],'No fabricated response hint')
check('can_chat_precedence',len(rows(can=True,request=True,accepted=True,waiting=True)[3])==2,'Original initial branch precedes pending reply branches')
check('native_combat_eligibility_unchanged',all(r['enabled']==False for r in rows(danger=True,target=False)[3]) and all(r['enabled']==True for r in rows(danger=True,target=True)[3]),'No forced enabled hint or CanChat override')
check('no_chat_request_no_fake_hint',not rows(can=False)[3],'Native no-chat case remains no action')
l,n2,u2,r2=rows(follow=True);follow=[r for r in r2 if r['hintClass']==4][0];follow['func'](n2,u2,0)
check('follow_callback_and_hint_preserved',n2.events[1]['kind']=='FOLLOW' and n2.events[1]['id']==11 and follow['action']=='focus_follow_init_interactor_press' and any(r['hintClass']==4 for r in select(r2,newroot)),'Actual original Follow callback and LB hint retained')
check('nil_user_no_actions',len(lua.globals().BasicAIActions.GetChatActions(n,None))==0,'Original nil guard')
check('hidden_alias_rb_only',bindings(newroot,'chat_init_interactor','kcd2vr_chat_init_gesture')==['xi_shoulderr'],'Existing generic interactor route, original OnChat callback')
check('unfocused_x_press_release_preserved',bindings(newroot,'chat_init','chat_init_press')==['xi_x','xi_shoulderr'] and bindings(newroot,'chat_init','chat_init_release')==['xi_x','xi_shoulderr'],'Native unfocused short-X preserved')
check('native_chooser_choices_and_release_preserved',[bindings(newroot,'chat_interaction','chat_'+n+'_controller') for n in ['first','second','third','fourth']]==[['xi_a'],['xi_b'],['xi_x'],['xi_y']] and bindings(newroot,'chat_interaction','chat_interaction_release')==['xi_shoulderr'],'A/B/X/Y choices and RB release unchanged')
oldactions={(m.attrib['name'],a.attrib['name']):ET.tostring(a,encoding='unicode') for m in oldroot.findall('actionmap') for a in m.findall('action')}
newactions={(m.attrib['name'],a.attrib['name']):ET.tostring(a,encoding='unicode') for m in newroot.findall('actionmap') for a in m.findall('action')}
check('all_existing_profile_actions_identical',all(newactions[k]==v for k,v in oldactions.items()) and set(newactions)-set(oldactions)=={('chat_init_interactor','kcd2vr_chat_init_gesture')},'One additional RB-only action; all original actions byte-serialized equal')
receipt=json.loads((OLD/'RECEIPT.json').read_text());pe=PE('C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll');assert sha(pe.data)==receipt['native_fingerprint']['sha256']
extra=[('generic_listener_vft',0x3F60DD8,0x3F60DF0),('listener_ctor',0x1748D0B,0x1748D28),('parsed_func_and_action',0xA2D234,0xA2D260),('stored_callback_target',0x8E8AB9,0x8E8AD4),('generic_input_action_name',0x8E609F,0x8E60F2),('generic_lookup_invoke',0x12D82D2,0x12D835C),('generic_callback_invoke',0x12D8388,0x12D838F),('function_operator',0x39AE30,0x39AE48),('script_functor_vft',0x3EB0CB8,0x3EB0CD0),('script_functor_invoke',0x1ED9A80,0x1ED9A90),('original_script_call',0xB29258,0xB292E0),('original_lua_function_call',0xB2950C,0xB29595)]
pins=receipt['native_pins']+[dict(name=n,rva=hex(a),end=hex(z),bytes=pe.read(a,z-a).hex(),sha256=sha(pe.read(a,z-a))) for n,a,z in extra]
_,actual=runtime(ba);(OUT/'ACTUAL_GETCHAT_AND_CALLBACKS.lua').write_text(actual,newline='')
result=dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),status='SOURCE_ONLY_PENDING_INDEPENDENT_REVIEW',kind='ACTUAL_LUA_BUILDER_CALLBACKS_XML_PLUS_EXACT_RETAIL_STATIC_GENERIC_DISPATCH_PROOF',cases=cases,native_execution=False,headset='NOT_RUN',native_fingerprint=receipt['native_fingerprint'],native_pins=pins,inputs=dict(profile_sha256=sha(pb),basic_sha256=sha(bb),builder_sha256=sha(builder)),candidate=dict(profile_sha256=sha(pa.encode()),basic_sha256=sha(ba.encode()),patch_sha256=sha(patch.encode())),limitations=['Static native binding conflict witness and generic-dispatch proof are not live engine execution.','Actual original Lua callbacks run with explicit native API leaves; no native outcomes fabricated.','Generic XML loading/action listener registration and UI rendering have not been headset verified.','No existing action/profile row removed; no native ABI, hook, timer or HUD writes.'])
(OUT/'RECEIPT.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(dict(passed=len(cases),candidate=result['candidate']),indent=2))
