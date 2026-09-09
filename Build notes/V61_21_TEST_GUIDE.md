# V61.21 test guide — FlashUI + CPoseData + corrected stereo static chase

Source-only build. No Windows ASI compiled in this environment.

## One short run is enough
1. Use the old known-good main KCD2VR.ini. Do **not** press SAVE INI for this test.
2. Reach normal gameplay.
3. Open/close the HUD/menu and aim at an interactable or archery target for 10–20 s.
4. Draw sword or bow and move both hands for 10–20 s.
5. Send `KCD2VR-debug*.log`.

## FlashUI markers
Look for:
- `V61.21-FLASHUI static-xref confirmed global_rva=0x492D938`
- `V61.21-FLASHUI passive HUD lookup observer enabled`
- `V61.21-FLASHUI HUD-LOOKUP ... out0=... ret=...`

Static DLL evidence behind this probe:
- WHGame+0x89B6A6 and +0x188ABF3 load global `WHGame+0x492D938`.
- its vtable slot +0x70 is called with literal `hud` (RVA 0x3A55B7C).
- returned HUD element is queried via vslot +0x238 for `CombatCursorCross`, `CursorCrossPBSlot`, and `CursorCrossOpponentLock`.
This is substantially stronger than the failed broad RTTI global scan.

## FRIK / CPoseData markers
Look for:
- `V61.21-FRIK-PROBE ... gate414=2 ... poseRtti='...CPoseData...'`
- `V61.21-CPOSE-SCAN ...`

The ABI is corrected from static disassembly: `WeaponSolver::SetIKTargets` at WHGame+0xA7F9CC is a two-argument member call (`RCX=this`, `RDX=pose argument`). R8/R9 are not set at its sole direct caller and must not be treated as semantic parameters.
No skeleton writes are performed.

## Stereo
No runtime stereo detours in this build. Static conclusion is corrected:
- `g_stereoFrameworkEnable` is registered default 1 and was observed live.
- renderer `r_Stereo*` parameter storage is live.
- `$VolCloudLeftEye` at RVA 0x3E01CE8 is a volumetric-cloud **resource name** registered into owner+0xA0; it is not by itself an eye/pass selector.
- V36 pipeline offsets 0x818/0x828 alternate with the two Normal CRenderViews, but are not yet proven eye IDs.
Next stereo target remains the producer/consumer of a real eye/pass index or per-eye projection before CRenderView dispatch.
