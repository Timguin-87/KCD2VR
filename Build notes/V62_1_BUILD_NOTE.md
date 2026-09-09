# KCD2VR V62.1 — UI sliders + F6 AUTO/override + first live right-arm VRIK

Base: V62.0 FIX1. Corrected V61.12 gameplay-CView culling, V60.1/V61.36 dialogue/inventory source-FOV guard, GOOD MONO presentation, head-sync 0/2 checkbox, and V61.40 projection throttle are preserved.

## 1. UI tuning is exposed immediately

F7 opens directly on the existing native UI tuning page so the previously hidden controls can actually be tested:

- subtitle scale / vertical position;
- dialogue scale / inward position;
- interaction-hint scale / inward position / vertical position.

BACK returns to the unchanged legacy menu. This is not the planned pre-beta menu redesign.

The backend remains direct ASI -> CryEngine `UIAction.SetScale/SetPos` against the vanilla HUD.xml MovieClip names. No Lua, Dynamic HUD, Dynamic Detector, PAK, hud.gfx or HUD.xml replacement is required.

## 2. F6 virtual-screen state machine

F6 now cycles:

`AUTO -> FORCE SCREEN -> FORCE IMMERSIVE -> AUTO`

AUTO owns the startup/title comfort screen, Touch inventory/pause comfort state, default-key I/M/Escape fallback, and the vanilla cutscene context `e_SkipInFrustumOptimizationWH=1`. Returning F6 to AUTO always returns ownership to this state machine instead of getting stuck in a manual mode.

Startup/title begins on the LOCAL virtual screen and auto-exits after the proven stable gameplay camera condition (published gameplay VFOV >= 80 degrees for 100 ms). The sample flat-screen scale is raised from 1.58 to 1.70 to make the screen slightly smaller / less zoomed without touching game FOV. Right Grip + Right Stick calibration remains available while the virtual screen is active.

Keyboard I/M/Escape detection is explicitly a default-key fallback and does not infer custom key remaps.

## 3. VRIK ownership proof retired; live right arm starts

The V62.0 headset result established a valid semantic rig (`jointCount=718`, `HAND_R=408`) and visible right-arm deformation from the late semantic path. V62.1 therefore removes the FINAL/PREBUILD proof ladder.

F9 now toggles one state only:

- OFF
- LIVE RIGHT ARM

The skinning hook uses the proven buffer-1 `QuatT` getter/setter immediately before `WHGame+0x5FE2F0` consumes the pose. It reads semantic RIGHT upperarm/forearm/hand joints, derives Henry's actual upper/forearm lengths, and solves a two-bone elbow/hand target. Only upperarm and forearm rotations are temporarily modified; translations and hand transform are left native in this first live test. KCD2 builds its normal final hierarchy/skinning output, then the original QuatTs are restored immediately.

The XR/control thread publishes HMD + right-controller tracking through a lock-free atomic snapshot. The skinning hook performs no OpenXR calls and no per-call logging.

### First-step calibration

The exact character-world transform is not yet recovered. To avoid guessing a world origin or causing a startup jump, the first valid F9 frame captures:

- native current right-hand position in the arm's local parent space;
- right-controller position relative to the HMD, converted with the proven camera axis rule `Cry local = { XR.x, -XR.z, XR.y }`.

After calibration, controller *delta* drives the hand target. This is intentionally a first live-arm bridge; it proves controller direction, scale, elbow solve and late pose ownership before adding wrist orientation / exact character-world anchoring.

Calibration resets whenever F9 is enabled, the correlated rig owner changes, controller tracking drops, or F6/menu/cutscene suspension occurs.

## 4. Authored-action safety

Automatic native action-state identification for smithing/alchemy/pickpocket is not yet implemented in C++. The reference Dynamic Detector Lua identifies scripted scenes by querying `ActionMapManager.IsFilterEnabled`, but no native C++ ABI for that manager is yet proven.

V62.1 automatically suspends live arm writes for:

- F6 virtual screen;
- KCD2VR F7 menu;
- startup/menu comfort screen;
- vanilla cutscene context;
- inactive right-controller tracking.

For this first live-arm test, press F9 OFF before smithing, alchemy, pickpocketing or another authored hand minigame. Do not replace this with guessed native offsets. After ordinary right-arm tracking is stable, the next VRIK layer is a proven action-state classifier and native/blended animation policy.

## 5. Performance rules

- no OpenXR calls in the skinning hook;
- no heap allocation in the hot solve;
- no per-call file logging;
- sparse one-second status only while F9 LIVE is enabled;
- old OperatorQueue / WeaponSolver-joint writes / `+0x53D7C4` misuse remain retired.
