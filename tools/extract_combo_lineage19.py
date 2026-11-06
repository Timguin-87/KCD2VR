import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
s=read_source(r/'src/subsystems/masterstrike_combos/native_combo_grace.inl')
a=s.index('namespace combo59c17 {');b=s.index('static void Summary()',a)
body=s[a:b]+'}\n'
old='reinterpret_cast<TimerStartFn>(g_base+kTimerStart)(static_cast<unsigned char*>(trigger)+0x28,wanted,0.f);'
assert body.count(old)==2 # F1 window open + CB2 real-step re-arm
body=body.replace(old,'HostTimerStart(static_cast<unsigned char*>(trigger)+0x28,wanted,0.f);')
(r/'tests/combo_lineage19.inc').write_text('// Generated from the actual complete grace consumer; only native timer-start leaf substituted.\n#include \"diagnostics/combo_snapshot.h\"\n#include \"vrik/combat_r9.h\"\n'+body)
(r/'tests/combo_lineage19.provenance.json').write_text(json.dumps({'sha256':hashlib.sha256(s[a:b].encode()).hexdigest(),'nativeLeaves':['TimerStart only; native manager/trigger originals are explicit spies']},indent=2))
mid=s.index('// Exact retail register seams.')
(r/'tests/combo_update_h10_sdk.inc').write_text('// Generated verbatim production SDK callbacks/installation. Host SDK context/patch creation leaves only.\nnamespace combo59c17 {\n'+s[mid:])
(r/'tests/combo_update_h10_sdk.provenance.json').write_text(json.dumps({'sha256':hashlib.sha256(s[mid:].encode()).hexdigest(),'substitutions':'None; actual production callback and install bodies'},indent=2)+'\n')

# The current public recognizer/intent adapter is composed with these same grace
# callbacks in the additional bridge. Extract its whole production predicate;
# frame publication, tracking and native-body read leaves stay explicit there.
cpp=read_source(r/'src/frik_pose_probe.cpp')
owner=''
for name in ('unsigned GeometryIntentOwnerStateH30(', 'bool GeometryIntentOwnerH30('):
 start=cpp.index(name);brace=cpp.index('{',start);depth=1;end=brace+1
 while depth:
  depth+=(cpp[end]=='{')-(cpp[end]=='}');end+=1
 owner+=cpp[start:end]+'\n'
 assert cpp.count(name)==1
(r/'tests/combo_intent_owner_current.inc').write_text('// Generated verbatim current production intent-owner predicate.\n'+owner)
(r/'tests/combo_intent_owner_current.provenance.json').write_text(json.dumps({'sha256':hashlib.sha256(owner.encode()).hexdigest(),'requestedSource':'src/frik_pose_probe.cpp','functions':['GeometryIntentOwnerStateH30','GeometryIntentOwnerH30'],'substitutions':'None in extracted functions; explicit host mailbox/tracking/native-body leaves only'},indent=2)+'\n')

# Native104 composes the literal early direct-route prefix with its actual
# admitted-release reader and the existing direct lineage/flag consumers.
# Keep returns/gotos intact; the host fixture supplies receiver discovery only.
if len(_m2_source_sys.argv)>1:
 direct=read_source(r/'src/subsystems/hybrid/drain_live_delivery.inl')
 marker='        if(action){\n            if(!p.action)p.action=action; // bind on first sight'
 assert direct.count(marker)==1
 prefix=direct[:direct.index(marker)]
 output=Path(_m2_source_sys.argv[1]);output.parent.mkdir(parents=True,exist_ok=True)
 output.write_text(prefix)
 output.with_suffix('.provenance.json').write_text(json.dumps({
  'source':'src/subsystems/hybrid/drain_live_delivery.inl',
  'normalized_source_sha256':hashlib.sha256(direct.encode()).hexdigest(),
  'prefix_sha256':hashlib.sha256(prefix.encode()).hexdigest(),
  'range':'Start through normal live-action branch; existing outer live-route brace closed by host fixture',
  'substitutions':'None inside prefix; receiver discovery/native execution are explicit host leaves'
 },indent=2)+'\n')

# CB2: exact complete native step callback. Only its native original/memory and
# diagnostic logging leaves are supplied by the connected host fixture.
observer=read_source(r/'src/subsystems/masterstrike_combos/native_combo_observer.inl')
start=observer.index('static void __fastcall ComboStepDetour6691(')
brace=observer.index('{',start);depth=1;end=brace+1
while depth:
 depth+=(observer[end]=='{')-(observer[end]=='}');end+=1
callback=observer[start:end]
(r/'tests/combo_cb2_step_observer.inc').write_text('// Generated verbatim actual native combo-step detour.\n'+callback+'\n')
(r/'tests/combo_cb2_step_observer.provenance.json').write_text(json.dumps({'source':'src/subsystems/masterstrike_combos/native_combo_observer.inl','sha256':hashlib.sha256(callback.encode()).hexdigest(),'substitutions':'None; fixture models native original, read memory and diagnostic leaves'},indent=2)+'\n')
