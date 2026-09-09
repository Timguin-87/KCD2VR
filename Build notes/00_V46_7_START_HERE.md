# Medieval Immersion Simulator — V46.7

V46.7 combines three jobs in one launch:

1. preserve the working V46.5 mouse/menu path and V46.6 mono projection guard;
2. run an extended F8 ladder with mono calibration followed by safe sequential/native renderer observers;
3. finish with a 10-second read-only VRIK idle baseline.

## F8 order — 55 seconds total

1. MONO_FOV112_FRAME097 — 5 s
2. MONO_FOV115_FRAME097 — 5 s
3. MONO_FOV118_FRAME097 — 5 s
4. MONO_FOV115_FRAME094 — 5 s
5. MONO_FOV115_FRAME100 — 5 s
6. SEQUENTIAL_OBSERVER_FRAME097 — 5 s
7. NATIVE_OBSERVER_FRAME094 — 5 s
8. NATIVE_OBSERVER_FRAME097 — 5 s
9. NATIVE_OBSERVER_FRAME100 — 5 s
10. VRIK_IDLE_BASELINE — 10 s

Modes 2/3 are still mono compositor fallbacks. They are renderer observers only and do not call an unproven second-render entry point.

## VRIK idle phase

Stay still with controllers naturally held. The log samples once per second:
- left/right OpenXR grip pose + quaternion;
- trigger, grip and thumbstick values;
- player_NoIK;
- player_DrawIK;
- wh_pl_DebugIKLimbs;
- ca_useADIKTargets;
- wh_pl_ShowHead.

No skeleton/joint writes occur in V46.7.

## Menu styling

The runtime menu retains the illuminated-manuscript parchment treatment and requests `Augusta` by installed font-family name, with normal Windows fallback if unavailable. No font file is bundled. The previously generated MIS illuminated-book concept is included as `MIS_MENU_ART_REFERENCE.png` for the visual direction of the permanent menu.
