# V61.51 test guide

## A. Head sync first
1. Build/install the ASI.
2. Open F7 and keep **Fix head sync** ON.
3. Set the game FPS cap to the headset-proven 84 or 85 using the existing FPS control.
4. Compare head turning against Fix head sync OFF. Mouse-look is not the useful discriminator here.

## B. Live UI companion
1. Copy `OPTIONAL_UI_MODS/KCD2VR_UI_LIVE` to the game's `Mods` directory.
2. Disable old KCD2VR viewport HUD PAK experiments while testing this companion.
3. Start gameplay. The log should contain `[KCD2VR-UI] running` and, once the HUD is available, `semantic HUD transforms active`.
4. Check:
   - subtitles are smaller;
   - dialogue left/right content is pulled inward;
   - interaction/action hints are no longer at the far-right edge.
5. Tune live if needed:
   - `kcd2vr_ui_sub_scale 45` / `50` / `55`
   - `kcd2vr_ui_dialog_scale 60` / `67` / `75`
   - `kcd2vr_ui_dialog_inset 8` / `12` / `16`
   - `kcd2vr_ui_hints_inset 20` / `30` / `40`
6. `kcd2vr_ui_enable 0` restores captured native transforms. F7 `VR UI layout` sends the same switch.
7. If a HUD reload causes a bad baseline, run `kcd2vr_ui_recapture` during normal gameplay.

Letterbox is not part of this test. Do not use letterbox success/failure to judge the semantic layout path.

## C. VRIK ownership ladder
The supplied INI enables hook installation but VRIK mode begins OFF.

1. Press F9 once -> READ. No writes. Confirm the F7/status log shows a valid rig, plausible joint count and semantic right hand.
2. Only after READ is valid, F9 -> FINAL HAND_R. Observe only Henry's visible right hand/weapon.
3. If FINAL executes but is invisible, F9 -> PREBUILD HAND_R.
4. If both write modes execute with no visible hand motion, stop. Do not increase amplitude or return to OperatorQueue; the next target is downstream of `desc+0x20`.
5. If the wrong character/LOD moves or anything corrupts, cycle immediately back to OFF and keep the log.

VRIK status is intentionally sparse (about once per second) to avoid reintroducing logging-related head jitter.
