# V61.45 test order

## 1. Baseline / tracking
Start with **no optional UI PAK enabled** and keep:

```ini
[Settings]
VrPoseSourceMode = 2
VrPosePredictionOffsetMs = 0.0

[Experimental]
EnableVrikSemanticOwnershipTest = false
```

Expected debug evidence during head movement:

`V53-ANGULAR ... source=2 ... sync=yes ...`

If mode 2 is worse or `SYNC-POSE-FAIL` repeats, switch only this one key to `VrPoseSourceMode=0` for the A/B comparison.

V61.12 culling synchronization remains enabled. V61.45 only prevents the temporary culling pose from retagging the rendered PRESENT identity and reuses the recent exact culling-pose matrix at PublishViewCamera when possible.

## 2. Dialogue / inventory zoom
There is no transient dialogue/inventory `cl_fov` writer in V61.45. The compositor keeps the user-selected source FOV (V60.1/V61.36 behavior). Keep dialogue mode `ForcePOV=1`, `NoCameras=0`.

## 3. UI layout test first
Install only:

`OPTIONAL_UI_MODS/KCD2VR_UI200_CENTERED-INSTALLABLE.zip`

Before testing, remove/disable `Mods/KCD2VR_HUD_Runtime` and any other mod that replaces `Libs/UI/hud.gfx`.

Check:
- dialogue/subtitle size;
- dialogue left/right choice visibility;
- `br.actionHints` / interaction icons;
- inventory/menu centering;
- whether any element is clipped.

## 4. Letterbox test separately
Disable UI200_CENTERED, then install only:

`OPTIONAL_UI_MODS/KCD2VR_DYNAMICHUD_VR_NOLETTERBOX-INSTALLABLE.zip`

The derivative defaults Dynamic HUD's semantic no-letterbox option to ON. If an existing Dynamic HUD saved setting overrides it, run:

`dh_no_letterbox 1`

Do not stack this with UI200_CENTERED because both own `Libs/UI/hud.gfx`.

## 5. VRIK later
Only after tracking/UI regression testing is clean, enable the existing semantic VRIK ownership test. It remains OFF by default.
