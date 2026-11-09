# Settings

Most tuning belongs in the VR menu. The INI stores those choices; it should not
be necessary to remember a release number to understand a switch.

Keep your existing INI and calibration when updating. The launcher preserves
them. An old key can have a number in its name because renaming it would discard
a saved preference. The comments describe what it does now.

The INI is saved on exit. Its values describe that saved state, not a timeline
of earlier attempts. Use timestamped runtime readbacks to establish which
options were active during a particular action.

## View and rendering

**Home > Alternate Eye (DLSS)** is a third view-mode toggle alongside Mono and
Stereo. It renders one camera eye each game frame and retains the previous
image for the other eye. It requires the game's DLSS upscaler. If DLSS's paired
contexts were not captured at startup, the Graphics page asks for a restart so
the game can recreate them.

Each eye has its own DLSS history, fed only by that eye's live frames, and DLSS
only ever evaluates live frames. The game's motion vectors map a frame to the
immediately previous frame, which belongs to the opposite eye. R149 rebuilds
them per pixel for the same eye's previous live frame (N to N-2) from the depth
buffer and the exact CRenderView camera banks. They are never composed through
the other eye's image, which used to make areas hidden from the other eye
shimmer. The eye of each live frame comes from the camera actually being
rendered, not from the main thread's newer publication counter. A built-in check
compares the game's own vectors with the camera model every frame. It selects
the depth convention and falls back to a per-frame DLSS reset while the model
does not hold. `VrAerDlssMotionR149` (INI only): 1 same-eye reprojection
(default), 0 reset every eye frame, 2 raw game vectors (diagnostic).

**Stale-eye warp (R154).** The eye that was not rendered this frame shows its
own last post-DLSS image, carried to the camera the live frame would have
rendered for it. **Home > WARP** (shown next to ALT EYE) cycles the tier, each
containing the one before it:

- **WARP ROT**: HMD and game camera rotation only (the R144 warp).
- **WARP 6DOF**: also the camera's translation (head lean, walking, riding, the
  eye separation itself), reprojected through that eye's own depth from its
  last live frame. Exact for the static world.
- **WARP MV** (default): also every moving surface (NPCs, animals, your own
  body, hands and weapon) moved by the motion the game measured for it. The
  live eye's motion vectors point at the previous render, which is the stale
  eye's image, so they say where each surface went since. Surfaces the live eye
  cannot see keep their own motion from the frame before.

Pixels nothing lands on (areas revealed behind a moving or nearer object) take
the background beside them at its rotation + 6DoF warp; wider gaps fall back to
a backward 6DoF warp, then rotation alone. The image is resampled once. Missing
inputs (first frames, a camera cut, menus, flat UI) fall back to WARP ROT for
that frame. Saved as `VrAerWarpModeR154` (0 rotation, 1 6DoF, 2 motion
vectors). `VrAerWarpDebugR154=true` (INI only) tints the stale eye by tier:
blue own motion, green measured motion, cyan crack fill, yellow background
fill, orange backward 6DoF, magenta rotation only. `KCD2VR.log` gets an
`R154 AER_WARP` line every 30 s (tier shares, fallbacks, GPU time) and an
`R154 AER_PERF` line (live eye frames per second, XR blits per second, CPU time
of the blit). The warp's GPU memory (about 180 MB at a 1536 render) is freed a
few seconds after ALT EYE stops.

**Alternate-eye frame dump.** Press **Ctrl+F12**, or use **Home > DUMP** next to
the mode button (capture starts 3 s later so the menu can be closed). The mod
captures the next 4 live eye frames, two per eye. For each frame it stores the
DLSS colour, depth and game motion vectors, the vectors DLSS received, and the
DLSS output. It also records both camera banks, the retained same-eye camera,
identities, jitter and parameters, plus the XR composition of those frames: the
cached post-DLSS image and both submitted eye images with their warp constants.
Files go to `KCD2VR_AER_DUMP\<date_time>\` beside the mod DLL (about 1 GB,
uncompressed DDS + `dump.json`). Expect a short stutter while capturing. A beep
confirms the finished write; `KCD2VR.log` names the folder. Analyse with
`python tools\analyze_aer_dump_r149.py <folder>`.

In direct controller-hand mode, controller movement is anchored from the
camera's published position, including the camera height, side and back offsets.
The anatomical palm-to-wrist correction remains applied to the hand pose.

**VR CONFIG > CONTROLS > Modifier** cycles between right thumbrest (the existing
layout), left thumbrest, and face touch. Left thumbrest moves the D-pad, Map and
VR menu chord to the right stick; holding it with right A requests Wait and with
right B opens Pause. The setting is saved as `VrQuickAccessLeftThumbrestR143`;
the older `VrQuickAccessFaceTouchR112` key still selects face touch.

## Hands and weapons

The accepted publication timing is part of ordinary VRIK and does not require
recording. The new **Fresh Pose Uncapped** choice is also persistent and does
not require recording. Shift+F7 (the VRIK recorder) cannot turn its XR-token wait back on. Extra hand
lead, same-frame weapon placement, rigid-grip behavior and the additional elbow
trial are separate experiments. Extra prediction does not remove a stale pose.
No historical elbow preset is established as a natural-motion solution; the
corrective review does not prescribe another settings sweep.

Experiments > Elbow preference has three choices: D86 DEFAULT (the D86 elbow
destination), R89 HAND-GUIDED (`VrElbowSolverR89=true`) and FRIK TRIAL
(`VrFrikElbows`). hand-guided points the elbow from your hand: behind the wrist
along the hand and on the little finger side, mixed with the body pole and
pushed out of the chest. one solver for free arms and bow arms, it keeps its
history when a comparison rung changes, so the elbow does not snap.

Keep the wrist on the controller. Arm reach is handled through the body and
shoulders within fixed arm lengths. Smoothing that makes the hands trail behind
is not an acceptable substitute for correct timing.

### Fresh Pose Uncapped

Choose **Home > VRIK EXPERIMENTS > Fresh Pose Uncapped**, or use
**Home > EXPERIMENTS / LIVE SETTINGS > VRIK+ > Motion timing**. Existing
developer-mode page visibility remains in effect. The button applies and saves
the choice immediately; it does not need a new game start.

| Choice | `VrMotionTrialD67` | Behavior |
| --- | ---: | --- |
| BASELINE motion | 0 | Existing baseline timing. Separate explicit legacy gates retain their old behavior. |
| FRESH POSE timing | 2 | Existing fresh-publication token pacing. Still the missing-key default. |
| Fresh Pose Uncapped | 4 | No native-frame XR-token wait; extra pose-only refresh before the ordinary native update. |
| ELBOW reach preference | 3 | Existing elbow comparison and timing behavior. |

With the ordinary NOW camera source, Uncapped samples the head and grips at the
current converted OpenXR time. With the predicted camera source it uses the
latest qualified runtime target. Existing optional extra hand lead is preserved;
the new mode does not enable or increase it. Cached-head and offset camera
sources retain their established semantics instead of combining an early head
with independently advanced hands. No camera-source setting is changed.

A busy pose/action-space lock, unavailable clock, obsolete predicted target or
rejected publication falls back to the completed snapshot with its original
age. Partial tracking loss retains existing per-hand validity and grace rules.
The mode does not generate extra action edges, run an extra native update, or
change stereo eye order. The later head refresh is kept; it is not forced to
reuse the earlier hand sample.

"Uncapped" means this mode removes the mod's native-frame token pacing. It does
not override a separately enabled FPS limiter, headset refresh, runtime wait,
driver policy or actual CPU/GPU cost. Final rendered hand continuity and the
resulting FPS still need a game/headset run.

Two-hand primary bias controls how strongly the main hand leads the supported
weapon. The supplied current profile uses 0.40; this patch preserves it. Grip offsets are visual alignment settings,
not a way to extend a weapon's native attack reach.

## Contact

Experiments > Hands > Free-hand contact detail has three choices:

- NORMAL: the existing five-point palm contact.
- PRECISE: the existing palm/finger contact and finger fitting.
- BALANCED: five palm points plus stable thumb/index tips, 2 mm clearance,
  without finger fitting. This replaces the old Performance choice, which
  skipped world contact. It keeps the same 18-ray / 1 ms query ceiling.

Balanced contact is a new comparison, not a headset-proven default. Existing
saved contact choices are kept. All modes use high-resolution timing when
releasing a measured contact correction. Free controller motion is not filtered.
The permissive five-point station path remains separate, as do hammer, tongs
and held-workpiece contact. None of these options creates arbitrary rigid bodies.

F9 records physics independently of the gameplay choices. Native hand capsules,
corpse dragging, existing small rigid objects and living-NPC experiments remain
separate features. The corpse choice now persists and runs without physical
combat or a recorder; it still requires a qualified native dead body and empty
hand. An enabled preference does not prove that the body moved.

## Crafting and instant actions

Native keeps the original interaction. Semi keeps selected authored tool phases.
Free and 6DOF use tracked movement at their connected boundaries. Smithing's
optional semi anchor is only for the left workpiece on the anvil; it should not
lock the right wrist to the bellows.

Instant interactions, crafting, fast reload transitions and combat have separate
controls. Physical crossbow reload is independent of fast reload transitions:
your hand earns string/tool travel and bolt placement even when faster native
trailing animation is OFF.

The saved alchemy 8x setting is a ceiling for Henry's qualified pickup/return
and pre-work turning segments. It is not a multiplier for the whole station.
Book and station changes, distillation, actual pouring, grinding, bellows and
native work/effects retain their existing native or gesture-owned timing. The
carafe turn may accelerate only with current positive gesture credit and before
the earliest qualified work/effect boundary. Stopping or losing that gesture
cannot authorize continued fast turning. No native work event is sought past or
synthetically fired. A delayed or unqualified attachment is still a real limit;
speed does not replace missing ownership or replay an inventory effect.

Head hover can show ingredient names while the hand targets something else.
Looking is presentation; gripping is an action. The two must not fight each other.
Station prompts use 90% of their native size by default, twice the tested 45% value. Experiments > Immersion
has CRAFT HINTS size choices and TIMED HINTS. With timed hints on, they appear for
ten seconds at station entry or a new native work step. Look at an open palm to
show them again. The size change keeps their original positions and leaves
full-screen effects alone. This source change still needs a game check.

Two-hand weight feel (R90, `VrTwoHandWeightFeelR90`, Experiments > GEAR, last row
"Two-hand weight feel", OFF by default). OFF keeps the two-hand heft as it is. ON lets
the weapon weight slider (`VrWeaponWeightFeel`, lower = heavier) also slow down how
fast a weapon held with both hands turns. the two-hand stop without spring-back works
with both settings.

Temporal stereo publishes only after the finished RIGHT image has been recorded
and submitted on its matching queue. A timeout does not authorize an earlier
main-target copy. Resource resize and per-eye reset recovery are automatic.
Startup failures produce a bounded file receipt even with F2 off; detailed
capture measurements still require F2. There is no startup-fallback switch.

Dice (D85 trial, `VrDiceHandsD85`, Experiments > Objects page 2 "Dice: point to
choose", OFF by default). when ON and it is your turn to pick dice, point the right
hand at a die and the game's own dice cursor jumps to it, then press A to keep or
unkeep it like on the gamepad. X, Y, B and the stick work the same as before. the
row shows why it falls back to the gamepad (no hand ray, other game version, game
refused the cursor). taking, shaking and putting down the cup is still Henry's own
animation: the throw is started by C_ThrowDiceAction and nothing proves yet which
clock fires its throw event, so we dont pause it.

Small props in the hand (`VrStaticPropGrabR89`, Experiments > Objects page 2)
is OFF by default. This is an unfinished scenery-prop experiment. It now prevents
two hands claiming the same entity and retains failed physics restoration for
retry. A script completing still cannot prove that a void physics call changed
the native body. Ordinary native pickable items retain their existing route.

Crossbow reload route (R96, `VrCrossbowReloadRouteR96`, Experiments > INSTANT page, last row;
needs `VrCrossbowReloadD65C=true`). HAND ONLY is the default (bow pattern): the game's
reload request is held back while your hands do the reload. Light crossbow: pull the
string back with the right hand. Goat's foot / cranequin: grip at the chest takes the
game's real spanner into the right hand (native cocking action, hand driven). All three:
grip behind the right shoulder takes a bolt into the right hand (same native factory and
same hand writer as the bow arrow), open the hand on the rail to load. Hands idle for
1.5 s after the game asked for a reload: the game reloads natively. NATIVE-DRIVEN (false)
is the older XR34 route where the hands ride the game's reload action. The old key
`VrCrossbowHandOnlyR89` is no longer read (R92-R95 always wrote it as false).

Corpse search (R90, `VrCorpseSearchSqueezeR89`, Experiments > Hands page 2 last row,
only used while corpse dragging is ON). ALWAYS (default): every empty hand looks for a
dead body 10 times a second, so a fast squeeze on a limb is caught. ONLY ON SQUEEZE: the
search runs only in the 700 ms after the grip passes 0.15, no cost while the hands are
open, but a squeeze that goes from open to closed inside one frame can miss the body
(squeeze again).

Immersion > SHOW HINTS controls the native wh_ui_ShowHints setting. It is separate
from the timer. Turning native hints off wins over a timed reveal. The old global
HUD sliders still control their original global layout; station-local prompt
size is no longer forced to 60% by that older path.

## Combat

Stab follow-through, unarmed clinch and masterstrike reactions have independent
switches on the second combat page in Experiments. The old combined preference
is only a fallback when those individual choices have not been saved.

Punch travel is measured in metres. Increasing it reduces tiny false gestures
but can also reject short real punches. Walking and retracting a fist must not
count as a new strike. Swing and thrust sensitivity have different failure modes;
do not use a single energy setting to explain every missed hit.

Combo and defensive-window options leave native eligibility, perks, stamina,
armor and outcomes in the game. A visible reaction is not proof that a complete
named combo was accepted.

## Walking direction

Immersive left-stick movement follows the recentered headset heading by default:
turn your head toward a direction and push forward to walk that way. The VR
CONFIG > CONTROLS button cycles between `WALK: HMD` and `WALK: STICK`; the INI
key is `VrHmdWalkingDirectionR134`. Menus, dialogue navigation, authored
minigames and roomscale follow/dodge input keep their own directions.

## Hand tracking

Home > VRIK includes **HANDS 1:1 ON / HANDS LEGACY**. The default LEGACY mode
keeps the existing F8-calibrated hand mapping. 1:1 mode maps each free hand's
palm to the tracked controller at 1:1 translation, while retaining Henry's
skeleton-based hand orientation and palm-to-wrist correction. It skips hand
smoothing, free-hand collision corrections, and presentation lead. Saved
standing-eye height is used to place Henry's head from headset height; the
session headset orientation supplies the tracking-to-game coordinate frame.
The direct mode runs without the F8 arm T-pose. Its Home height reset reuses the
saved standing-eye reference and recenters the headset. Native item and activity
owners still control their hand while those interactions are active. The saved
INI key is `VrHandsDirectTrackingR135`.

### Hand offsets

Home > **HAND OFFSETS** moves and rotates each rendered hand relative to its
controller, in the controller's own frame (X right, Y up, Z forward). Each hand
has three position sliders (-20 to +20 cm, 0.5 cm steps) and three rotation
sliders (pitch, yaw, roll, -180 to +180 degrees). **RESET RIGHT** / **RESET
LEFT** zero one hand. The offsets apply in both the 1:1 and the legacy hand
modes. INI keys are `VrHandOffsetR150_<R|L>_<PosX|PosY|PosZ|Pitch|Yaw|Roll>`,
all defaulting to 0.

## Room movement and dodge

The physical roomscale experiment uses measured horizontal HMD displacement
alongside ordinary joystick input. The older offset-follow option is a different
method and should not be called one-to-one room movement. D84 connects explicit
physical-walking controls to the finite D76 path and keeps their selections in
sync. An unrelated menu apply preserves a prior explicit choice. F1 changes
BODY only. Select physical walking ON once to replace a preserved legacy follow
choice; saved OFF stays OFF.

The fresh-install lean threshold is 10 cm; saved values are retained. Lowering the head beyond it reserves the movement
for leaning rather than walking. A level-height lean and a step cannot be
distinguished perfectly with only a headset. Native collision may shorten a step
at a wall. Recenter must not replay old room movement.

Headset dodge only requests side or backward dodges, with a speed threshold,
quiet rearm and cooldown. It should not dodge forward when leaning into a punch.
The game still decides whether the dodge is legal or perfect. Physical walking
and headset dodge now have separate buttons. Enabling BODY preserves both
choices: the dodge request path is independent of full 6DOF or physical walking.
R2 removes the one-time movement-option override from R1. It preserves current
movement choices, calibration, lean threshold and accepted hand timing. The
exit-saved profile cannot identify which transport was active during the failed
test, and changing a default is not a repair of directional movement or dodges.

## HUD, map and glyphs

The immersive open-palm HUD uses the **left hand for compass** and the **right
hand for health/vitals/buffs**. A current open hand must face the HMD and be
looked at. Known trigger contact, grip, buttons or a held physical item refuse
the open-hand condition. The original one-second deliberate-look dwell remains;
the reveal then lasts at most three seconds. Closing the hand, turning it away,
losing tracking or looking away hides it immediately. A continuous stare cannot
restart the timer; disengage before a new reveal. The check uses head direction,
not an eye-tracking sensor.

The uploaded profile retains the ordinary held-paper backend. D81 restores
simple dark POI marks and removes repeated lookup/memory-query work. The D80 run
was slow while the map was open; recovered performance is not yet measured.
D84 map profiling requires F2 or Shift+F2; map opening alone does not start it.
The alternate renderer remains experimental. Current quest markers come from
the native compass subset, not every objective in the world.

Xbox and PlayStation glyph choices change the pictures, not the button layout.
Empty symbols are a font/movie problem, not a reason to remap controls.
Full-field fades, blood and cinematic masks must stay full-field.

## Optional gore and backups

Gore requires its own optional package. Main gameplay works without it. Cosmetic
cutting does not replace native death or damage checks, and reliable limb removal
is still unfinished.

The old stowed-bow and obsolete migration backups are no longer created. One
last-good INI checkpoint remains to protect settings writes and calibration.
It is not read as a gameplay preference and does not enable an old bow behavior.
Do not replace the active profile with an old backup to fix a current feature.

## Accepted defaults and readable controls

Dog petting and riding hands default ON when the key is absent. A saved OFF
still means OFF; updating does not replace your INI. Petting is on Objects;
riding hands are on Body, page 2. Native mounting completes, then normal VRIK
starts after one second without requiring a weapon draw. Dismounting and
protected native actions use the existing action rules. Washing is separate.

The experiment rows use larger, high-contrast bold explanations. The layout and
saved key names are unchanged. The legacy bow arm-line switch is described as
inactive; the separate physical shot-direction option is the active aim path.

## Optional gore

The existing gore button cycles OFF, HEADS ONLY, HEADS + LIMBS. It preserves the
saved head style. Limbs stay off unless selected; an existing explicit saved
choice is retained. These modes require the separate optional gore package,
which is never merged into the main companion.

## Smithing anvil assistance

In Experiments, the crafting group has **Smith LT anvil assist**. Enable it to
hold the left Trigger for the original anvil placement; releasing immediately
returns the left hand to tracked control. It applies only at the anvil in 6DOF,
not while heating or moving between stations. It uses the existing
`VrSmithSemiAnchorsD67` setting and is ON in this repair baseline. It does not add
a smoothing delay or affect the right-hand hammer.

## Historical migrations and the current profile

D81 originally migrated prompts, smithing assistance and selected NPC/knife
preferences. Its marker prevents repeated application. Those historical defaults
must not be read as the user's current live choices. The current starter INI
preserves the latest uploaded exit profile and calibration; only the requested
station prompt scale of 0.90 and the R1 migration marker differ in value. The
retained R1 marker now controls only that prompt migration; it makes no movement
write and respects later prompt changes.

D85R5 restored the throwing-knife service at the left hip despite the legacy
VrLeftChestKnives key name. Earlier notes saying acquisition is always retired
are historical. Native eligible stock and a complete throw remain separate
requirements; the current review does not grant inventory or certify them.

The bundled profile has VrFpsCapEnabled=false and VrFpsCap=90. The latter is a
stored target while the cap is disabled. These saved values cannot establish
whether a cap was active during an earlier run. R2 added no new FPS limit or
stereo-pacing change. R3 adds the separate opt-in uncapped route described above;
it preserves the existing limiter settings and the mode-2 default.
