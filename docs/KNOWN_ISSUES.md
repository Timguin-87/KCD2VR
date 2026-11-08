# Known issues

what i know is not right yet, or not proven in the headset. the longer story for each is in
KCD2VR_HISTORY_AND_RESEARCH.md (section 10 and the area sections).

## runtime / view

- SteamVR as the OpenXR runtime is newer than VDXR and less tested. SteamVR refuses the whole frame when one overlay
  layer is bad, the mod repairs the layers and lets overlays sit out for a moment instead of losing the frame. the
  `R106 XR_LAYERS` rows in KCD2VR.log say what SteamVR refused. VDXR (Virtual Desktop) is the tested runtime.
- sometimes the view starts upside down or facing backwards (seen with SteamVR). a start/load that faces more than
  90 deg away gets recentered, the first head pose is in the log (`R105 XR_HEAD`). no root cause yet.
- black frames now and then: no cause found in any log. if it happens, F2 at game start and note the time. when the
  game stalls the last picture is shown again instead of black (`VrEyeHoldR141`).
- DLSS/FSR/XeSS per eye work, but image quality and frame rate depend on the GPU and driver. Native (no upscaling) is
  the safe choice. some render size changes need a game restart.
- the HUD reading plane and the palm glance HUD need a comfort check per person (distance, size). full-field effects
  (fades, blood, cinematic masks) stay full-field on purpose.

## body / hands

- elbows have never been perfect. there are three elbow choices in Experiments > BODY, none is "the natural one" yet.
- weapon flicker while walking was worked on in R98 (shared frame delta), not confirmed in the headset since.
- without a saved T-pose VRIK starts on a default arm profile from your eye height. do the calibration once
  (Shift+F8 or the VR menu) for your real arm length.

## combat

- kicks: the route and cooldown are fixed in source, kicks in every context are not confirmed in the headset.
- impale: the hang works, the body still plays its death animation first before it ragdolls. a nearly instant ragdoll is
  not possible without the game's own hand-over.
- masterstrike / finisher damage stays the game's own 1x. the tracked blade does not own the authored finisher event.
- free targeting (native auto-turn vetoed during the fight) is not confirmed in the headset.

## interactions

- held props or a dragged corpse could push Henry away. there is a self-collision fix (collision class bits), not
  confirmed in the headset.
- scenery props without a native selection/ownership (level brushes) work through the brush swap only where the mesh has
  a usable path. fixed world brushes and physics-less meshes are not covered.
- the physical dice cup (take, shake, place) is not done. pointing at a die to choose it works (`VrDiceHandsD85`), the
  throw is still Henry's animation.
- alchemy: the mortar "release as drop in" over the kettle and the plate held in the hand had problems in my R101 run,
  fixes are in source, not confirmed.

## reloads

- crossbow: the tool in the hand for the goat's foot / cranequin and the light crossbow string pull by hand are not
  fully confirmed in a log. the F11 row `XBOW_TOOL_GRIP` shows what happened. the bow works.
- rifle: never tested in VR.

## other

- the optional gore pak is separate. reliable limb removal is still unfinished.
- the INI is saved on exit. it does not tell what was active during an earlier action.
- the mod is pinned to the current retail WHGame.dll. after a game update features with a failed byte pin do not run,
  the log says which.
