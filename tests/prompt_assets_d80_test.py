"""Execute actual emitted AVM1 plus exact asset regeneration. No game/Scaleform."""
from pathlib import Path
import sys,subprocess,hashlib,json
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import patch_prompt_presentation_d80 as d
import patch_glyphs_d68 as p
from glyph_assets_d68_test import VM,Clip,Function,function_body,flat
checks=0
def ck(value,label):
 global checks;checks+=1
 if not value:raise AssertionError(label)
# Exact parent assets to current source, including every retained native leaf.
for name in ['hud','buttons','Menu','ItemSelection','ItemTransfer','ApseMap','ApseModalDialog','ApsePlayerInfo','GameOver']:
 rel='companion/source/Libs/UI/'+name+'.gfx'
 old=subprocess.check_output(['git','show','11e3eae:'+rel],cwd=ROOT)
 expected=d.patch_movie('Libs/UI/'+name+'.gfx',old);actual=(ROOT/rel).read_bytes()
 ck(expected==actual,'exact reproducible scoped patch '+name)
 oldHeader,oldLeaves=flat(old);newHeader,newLeaves=flat(actual)
 ck(oldHeader==newHeader,'stage/header unchanged '+name)
 # Remove only the exact injected/replaced actions; all original assets remain.
 restore={d.callback_with_bootstrap():p.callback()}
 restore.update({d.redirect_with_bootstrap(n):p.redirect(n,p.MAPPING[n]) for n in p.MAPPING})
 kept=[]
 for path,kind,data in newLeaves:
  if kind==12 and data==d.injection():continue
  kept.append((path,kind,restore.get(data,data) if kind==12 else data))
 ck(oldLeaves==kept,'all native code/textures/placements retained '+name)
rel='companion/source/Libs/UI/UIElements/HUD.xml'
ck(d.patch_xml(subprocess.check_output(['git','show','11e3eae:'+rel],cwd=ROOT))==(ROOT/rel).read_bytes(),'HUD exact callback declaration only')
# An imported sprite can execute before the library root timeline. This was the gap.
vm=VM();clip=Clip();vm.run(d.redirect_with_bootstrap('xi_a'),this=clip)
ck(clip.frame is None and p.REGISTER in vm.glob,'early imported Xbox sprite registers without root frame')
vm.run(function_body(d.callback_with_bootstrap()),{'style':4})
ck(clip.frame=='pad_cross','style callback alone switches already live sprite')
vm.run(function_body(d.callback_with_bootstrap()),{'style':2})
ck(clip.frame=='xi_a','switch back keeps Xbox input semantics')
vm2=VM();vm2.run(function_body(d.callback_with_bootstrap()),{'style':4});late=Clip()
vm2.run(d.redirect_with_bootstrap('xi_b'),this=late);ck(late.frame=='pad_circle','style before sprite')
ck(all(c==('fc_setInputId',[2]) for c in vm.calls),'only native input family2 called')
# Prompt children: placement stays native; scaling is reversible/idempotent.
vm=VM();children=[]
for parts in d.PATHS:
 parent=vm.root
 for part in parts[:-1]:parent=parent.setdefault(part,{})
 clip=Clip();clip.update(_xscale=100,_yscale=80,_alpha=75,_x=234,_y=456)
 parent[parts[-1]]=clip;children.append(clip)
for name in d.NATIVE:vm.root[name]=Function([],p.push(None)+b'\x3e\0')
vm.run(d.injection(),{'__install__':True},vm.root)
def apply(active,percent,visible):vm.invoke(vm.root[d.CALLBACK],[active,percent,visible],vm.root)
apply(True,30,True)
ck(all((c['_xscale'],c['_yscale'],c['_alpha'],c['_x'],c['_y'])==(30,24,75,234,456) for c in children),'30% scale preserves alpha/position')
apply(True,30,True);ck(all(c['_xscale']==30 for c in children),'not compounded on retries')
apply(True,30,False);ck(all(c['_alpha']==0 for c in children),'timer hide alpha only')
apply(True,30,True);ck(all(c['_alpha']==75 for c in children),'palm reveal restores owned alpha')
# Original native function runs once, with full argument list, then presentation.
vm.root['kcd2vrNative_fc_setActionHints_D80']=lambda *args:None
# Function.apply leaf is modelled by the VM, actual wrapper bytecode remains under test.
vm.root['kcd2vrNative_fc_setActionHints_D80']=Function([],p.push(9)+b'\x3e\0')
ck(vm.invoke(vm.root['fc_setActionHints'],['content',5],vm.root)==9,'native result preserved by wrapper')
children[0]['_xscale']=90;apply(False,100,True)
ck(children[0]['_xscale']==90,'foreign scale not overwritten on exit')
ck(all(c['_xscale']==100 and c['_yscale']==80 and c['_alpha']==75 for c in children[1:]),'owned dimensions restored on exit')
# Fresh child obtains current policy on a native content update, no tree poll.
apply(True,30,False);replacement=Clip();replacement.update(_xscale=100,_yscale=100,_alpha=100)
vm.root['br']['actionHints']=replacement
vm.invoke(vm.root['fc_setActionHints'],[],vm.root)
ck(replacement['_xscale']==30 and replacement['_alpha']==0,'recreated native child receives active policy')
print(json.dumps({'checks':checks,'PASS':True,'actual_AVM1':True,'native_Scaleform':False,'headset':False}))
