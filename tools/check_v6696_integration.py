import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib, json, sys
root=Path(__file__).resolve().parents[1]
base=Path('/mnt/data/v6695_src/KCD2VR_V66.95_CONTINUITY_CONTACT_SOURCE')
checks=[]
def ck(name,cond): checks.append((name,bool(cond))); print(('PASS' if cond else 'FAIL')+': '+name)
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
# Preserve every modern VRIK implementation file from V66.95 byte-for-byte.
for rel in sorted([p.relative_to(base) for p in (base/'src/vrik').rglob('*') if p.is_file()]):
    ck('preserve '+str(rel), (root/rel).exists() and sha(base/rel)==sha(root/rel))
for rel in ['src/frik_pose_probe.cpp','src/frik_pose_probe.h','src/combat_diag_probe.cpp','src/combat_diag_probe.h','src/core/snapshot_mailbox.h']:
    ck('preserve '+rel,(root/rel).exists() and sha(base/rel)==sha(root/rel))
mod=read_source(root/'src/mod.cpp', errors='ignore'); pre=read_source(root/'src/native_vr_pad_premap.cpp', errors='ignore'); ui=read_source(root/'src/ui_menu_input.cpp', errors='ignore'); tb=read_source(root/'src/touch_bindings.cpp', errors='ignore')
ck('V66.96 marker','KCD2VR-V66.96-VRIK-NATIVE-PAD-UI-RECOVERY' in mod)
ck('native XI preferred','SetGamepadMode(3)' in mod and 'NativeGamepadFullReady' in mod and 'PublishGamepadState' in mod)
ck('bounded XI discovery','native_xi_discovery_attempts<2' in mod and 'native_xi_next_discovery_ms' in mod)
ck('no production XI off','V65.19-INPUT native XI transport OFF' not in mod)
ck('no companion package',not (root/'companion').exists() and 'kcd2vr_ui_actions' not in mod)
ck('right grip reserves RS','if(!rg)' in pre and 'Right Grip preserves' in pre)
ck('RS jump crouch','if(r.thumb_y> kDigital)p.b=true' in pre and 'if(r.thumb_y<-kDigital)p.r3=true' in pre and 'p.ry=0.0f' in pre)
ck('gameplay B 1s','>=1000ull' in pre and 'p.start=true' in pre)
ck('APSE native tabs sort','p.lb=true' in pre and 'p.rb=true' in pre and 'p.dpad_left=true' in pre and 'p.dpad_right=true' in pre)
ck('grip pointer keeps native pad','native_gamepad' in ui and '!f.native_gamepad?captured' in ui)
ck('no keyboard nav in native mode','if(!blocked&&!f.native_gamepad)Keyboard' in ui)
ck('touch right grip aim retained','directional_mode' in tb and 'MouseMoveRelative(dx,dy)' in tb)
ck('native mode suppresses duplicate RS keyboard actions','!native_gamepad_actions_ && vertical_action_dir==Dir::Up' in tb and '!native_gamepad_actions_ && vertical_action_dir==Dir::Down' in tb)
ck('item transfer immersive router','Presentation::Immersive' in read_source(root/'src/ui_menu_router.h', errors='ignore'))
ck('HUD canvas settings','VrHudNativeCanvasFit' in (root/'KCD2VR.ini').read_text(errors='ignore'))
ck('fullscreen effects excluded','fader' in read_source(root/'src/flashui_probe.cpp', errors='ignore').lower() and 'vignette' in read_source(root/'src/flashui_probe.cpp', errors='ignore').lower())
failed=[n for n,v in checks if not v]
out={'passed':len(checks)-len(failed),'total':len(checks),'failed':failed}
(root/'V66.96_SOURCE_GUARDS.json').write_text(json.dumps(out,indent=2))
print(json.dumps(out,indent=2));sys.exit(bool(failed))
