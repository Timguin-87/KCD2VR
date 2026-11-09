# Settings

most tuning is in the VR menu (F7). the INI stores those choices. every key in KCD2VR.ini has a comment that says what
it does now, grep the key name there. the Experiments pages need Developer mode (setup window).

keep your INI and calibration when you update. an old key can have a number in its name (VrMotionTrialD67,
VrCrossbowReloadRouteR96 ...) because renaming it would throw away a saved choice. the number is the version that added
it, not something you need to know.

the INI is saved on exit. its values are the state at exit, not a timeline of what was active earlier. a feature i
tested and called working is ON when its key is missing, a saved OFF stays OFF. updating never replaces your INI.

values below marked "shipped" are what the KCD2VR.ini in the source root has.

## view and rendering

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

## hands and weapons

**Motion timing** (`VrMotionTrialD67`, Home > VRIK EXPERIMENTS, or EXPERIMENTS / LIVE SETTINGS > VRIK > "Hand
presentation timing"). applies and saves at once, no restart.

| choice | value | what |
| --- | ---: | --- |
| BASELINE motion | 0 | the old baseline timing. |
| FRESH POSE timing | 2 | fresh-publication token pacing. |
| ELBOW reach preference | 3 | the elbow comparison timing. |
| Fresh Pose Uncapped | 4 | no native-frame XR-token wait, one extra pose-only refresh before the native update. shipped, and the default when the key is missing (since R124). |

Uncapped samples head and grips at the current OpenXR time with the NOW camera source, with the predicted camera
source it uses the latest runtime target. the head in that sample is only the hands' reference, it never replaces the
camera head. a busy lock or a stale target falls back to the last completed sample with its real age. "uncapped" means
no token pacing from the mod - it does not override an FPS limiter, the headset refresh or the real CPU/GPU cost.
Shift+F7 records it without turning the token wait back on.

**Elbow preference** (Experiments > VRIK, last row): D86 DEFAULT, R89 HAND-GUIDED (`VrElbowSolverR89`), FRIK TRIAL
(`VrFrikElbows`, shipped ON) and ELBOW REACH PREFERENCE (`VrElbowPreferenceD67`, the old D67 bottom elbow). when the INI
has more than one of them on, the solver takes them in this order: hand-guided, reach preference, FRIK. hand-guided points the elbow from your hand: behind the wrist along the hand and on the
little finger side, mixed with the body pole and pushed out of the chest. one solver for free arms and bow arms, it
keeps its history when a comparison changes so the elbow does not snap. none of them is "the natural one" yet.

keep the wrist on the controller. arm reach goes through the body and shoulders with fixed arm lengths. smoothing that
makes the hands trail behind is not a fix for timing.

two-hand primary bias (`VrTwoHandPrimaryBiasD65`) says how strongly the main hand leads a two-handed weapon (shipped 0.40). grip offsets are visual
alignment, not a way to get more attack reach.

**Two-hand weight feel** (`VrTwoHandWeightFeelR90`, Experiments > GEAR, OFF). ON lets the weapon weight slider
(`VrWeaponWeightFeel`, lower = heavier) also slow down how fast a two-handed weapon turns. the two-hand stop without
spring-back works either way.

## contact

Experiments > VRIK > Free-hand contact detail:
- NORMAL: five-point palm contact.
- PRECISE: palm/finger contact with finger fitting.
- BALANCED: five palm points plus thumb/index tips, 2 mm clearance, no finger fitting. same 18-ray / 1 ms query limit.

free controller motion is not filtered. station contact, hammer, tongs and the held workpiece have their own simpler
contact. none of these create rigid bodies.

## objects and corpses

- **Small props in the hand** (`VrStaticPropGrabR89`, Experiments > Objects page 2, shipped ON): grab small scenery
  props. two hands can't claim the same entity.
- **Loose scenery props (swap)** (`VrSceneryPropSwapR100`, Objects page 3 since R159, shipped ON): Grip on a mug/plate/candle that is
  level mesh makes it a loose prop you can hold. only where the mesh has a usable path.
- **Scenery props: size** (`VrSceneryBigPropsR158`, Experiments > Objects page 3, shipped OFF): SMALL = up to 65 cm
  across, same as before. UP TO 1 M also takes stools, baskets and small crates. mass stays capped (1.5 kg loose copy,
  3 kg scenery) so a big one carries like a light box, one hand only. reach to the box centre grows to max 55 cm
  so you can grab it at the edge
- **Corpse dragging** (Experiments > NPCS "Physical corpse dragging"): needs a dead body from the game and an empty
  hand, no recorder or physical combat needed.
- **Corpse search** (`VrCorpseSearchSqueezeR89`, VRIK page 2, only used while corpse dragging is ON). ALWAYS (shipped):
  every empty hand looks for a body 10 times a second, so a fast squeeze on a limb is caught. ONLY ON SQUEEZE: the
  search runs only for 700 ms after the grip passes 0.15, no cost with open hands, but a squeeze that closes inside one
  frame can miss (squeeze again).
- dog petting (`VrPetGestureD59`) and riding hands are ON. petting is on Objects, riding hands on Body page 2. native
  mounting finishes, then VRIK starts after one second.

## crossbow reload

**Crossbow reload route** (`VrCrossbowReloadRouteR96`, Experiments > INSTANT, last row, needs
`VrCrossbowReloadD65C=true`). HAND ONLY (shipped, the bow pattern): the game's reload request waits while your hands do
it.
- light crossbow: hold it in the left hand, grip the string with the right hand and pull it back (about 9 cm of
  string travel).
- goat's foot / cranequin: grip at the chest takes the game's real spanner into the right hand (native cocking action,
  driven by the hand). heavy and medium can also grip the string at the front and pull it back by hand (R111/R117
  switches in the INI).
- all of them: grip behind the right shoulder takes a bolt into the right hand (same native factory and same hand
  writer as the bow arrow), open the hand on the rail to load.
- if the hands stay idle for 1.5 s after the game asked for a reload, the game reloads by itself.
- an incomplete pull, regripping, or a bolt released away from the rail pauses the same reload, it does not finish it.

NATIVE-DRIVEN (false) is the older route where the hands ride the game's reload animation. the old key
`VrCrossbowHandOnlyR89` is not read anymore. Instant reloads changes only the speed of the native tail, not who does
the reload.

## crafting and instant actions

Native keeps the original interaction. Semi keeps selected tool phases. Free and 6DOF use tracked movement where they
connect. instant interactions, crafting, fast reload transitions and combat have separate switches.

the alchemy 8x setting is a ceiling for Henry's pickup/return and pre-work turning, not a multiplier for the whole
station. book and station changes, distillation, pouring, grinding, bellows and the native work/effects keep their own
timing. speed does not replace a missing ownership or replay an inventory effect.

head hover can show ingredient names while the hand targets something else. looking is presentation, gripping is the
action.

crafting prompt size: **Crafting / minigame icon size** slider on the VR HUD page (`VrMinigamePromptScale`, shipped 0.4). at a
station the size is sent again every second. Experiments > Immersion has CRAFT HINTS sizes and TIMED HINTS (hints
show for ten seconds at station entry or a new work step, look at an open palm to see them again). Immersion > SHOW
HINTS is the game's own wh_ui_ShowHints, hints off wins over the timer.

**Smith LT anvil assist** (`VrSmithSemiAnchorsD67`, shipped ON): hold the left Trigger at the anvil for the original
anvil placement of the workpiece, release = the left hand is tracked again. only at the anvil in 6DOF. no effect on
the right-hand hammer.

**Dice: point to choose** (`VrDiceHandsD85`, Experiments > Objects page 2, OFF). when it is your turn to pick dice,
point the right hand at a die and the game's dice cursor jumps to it, then A keeps/unkeeps it like on the gamepad. X, Y,
B and the stick work as before. the row says why it falls back to the gamepad. taking, shaking and putting down the cup
is still Henry's own animation.

## combat

stab follow-through, unarmed clinch and masterstrike reactions have their own switches on the second combat page in
Experiments.

punch travel is in metres. more travel = fewer tiny false punches, but short real punches can be rejected. walking and
pulling a fist back must not count as a strike. swing and thrust sensitivity fail in different ways, one energy setting
does not explain every missed hit.

combo and defence options leave eligibility, perks, stamina, armor and results to the game. a visible reaction is not
proof that a named combo was accepted.

NPC defence by skill (`VrDirectNpcBlockBySkill`, shipped ON): block raise, parry raise and dodge clip speed come from the
NPC's own melee skill. it also rolls once per swing if your opponent defends: 25 % at skill 0, 75 % at skill 30 and
more (the game's wh_cs_ForceBlock, only in a real fight: Henry in combat, no dialogue / cutscene / menu; R155). `VrDirectNpcBlock` 1 = 65 %, 2 = always,
3 = always perfect block. never with a practice / quest weapon in your hand (the wooden sword of the prologue sparring, R156): the sparring
partner is scripted to let you hit him, he is not forced to block. note: `VrDirectNpcBlock` 0 does not switch the roll off while BY SKILL is on.

**Tutorials (training fights)** (`VrTutorialNativeHitsR159`, Experiments > VRIK page 2 row 5, R161: 0 OFF / 1 NATIVE HITS / 2 GAME TIMING, shipped 0 since R162 (GAME TIMING gave 0 chained follow-ups in VR); old true = 1, false = 0, missing = 0). GAME TIMING: with the practice weapon the mod does not shorten the native time-to-hit (`VrNativeHitTime`) and the motion attack clock runs at the native rate, decided once per swing at its start, F2 row `R161 TUTORIAL_TIMING on/off mode=...`; the direct route stays as with OFF. NATIVE HITS (the old R159, with it 5 of 6 chained follow-ups got no hit in the sparring): while you hold a
practice / quest weapon (the wooden sword of the prologue sparring with Capon) every swing hits only through the game's own
attack in its strike window, the mod's direct hit (late contact, VR intent hit) is off for that swing. the game's tutorial
then sees the hits the way it expects them. a contact the native strike window doesn't reach in time is not a hit there, so
swing like the game wants it - wait until the sword is back in guard. decided once per swing, a change in the middle of a
swing doesn't drop or double it. real weapon in hand = nothing changes. F2 log: `R159 TUTORIAL_NATIVE on/off reason=...`.
only the weapon is used to see a training fight; the arranged fight flag of the quest recorder (Shift+F5) is not read for this

**Training fights: vanilla NPCs** (`VrTrainingVanillaNpcR163`, Experiments > VRIK page 2 row 4, R163, default ON). while you hold
a practice / quest weapon (the same detector as above, now also with native controls) no mod feature writes to any NPC: npc
defence speed (R146), force block, flinch, blade reaction zone (NR1 / stamina hits), lively npc + listening gestures (D73/D76),
npc look at the headset, unarmed spacing, npc difficulty, counter stab, stagger rule, directional block and defence timing
(C39), and the global npc cvars (H3 router windows, D70 dodge window, R52 shared rules). what a feature still holds on an npc is
given back at once (force block cvar, cvar leases, npc life cleanup, flinch pose). your own VR (swing, announced attack, body,
camera, input, kicks) stays. when the practice weapon is gone everything runs again (no latch). F2 log: `R163 TRAINING_VANILLA
on held=... restored=...` / `off heldMs=...`; always (few rows): `R163 TRAINING_VANILLA ... held-off writes (all/tutor): ...`.
OFF = all NPC features as before (a bisect switch: if the sparring scene still stops with it ON, the mod's NPC writes are not it).

## walking direction

Immersive left-stick movement follows the recentered headset heading by default:
turn your head toward a direction and push forward to walk that way. The VR
CONFIG > CONTROLS button cycles between `WALK: HMD` and `WALK: STICK`; the INI
key is `VrHmdWalkingDirectionR134`. Menus, dialogue navigation, authored
minigames and roomscale follow/dodge input keep their own directions.

## hand tracking

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

## room movement and dodge

physical walking uses the measured horizontal HMD movement together with the stick. the older offset-follow option is
a different thing and not one-to-one room movement. F1 changes body follow only.

lean height (`VrRoomscaleLeanHeightD70`, metres below the recentered height, 0..0.12, shipped 0.12). lowering the
head past it means leaning, not walking. a level lean and a step
can't be told apart perfectly with only a headset. native collision can shorten a step at a wall. recenter does not
replay old movement.

headset dodge only asks for side or backward dodges, with a speed threshold, quiet rearm and cooldown. it should not
dodge forward when you lean into a punch. the game decides if the dodge is legal. physical walking and headset dodge
are separate buttons.

## HUD, map and glyphs

palm HUD: look at your open **left** hand for the compass, open **right** hand for health/vitals/buffs. the hand has to
face the headset and be looked at. trigger, grip, buttons or a held item cancel it. one second of looking, then it
shows max three seconds. closing or turning the hand, losing tracking or looking away hides it. it uses head
direction, not eye tracking.

more HUD switches are on the VR HUD page and in the INI: Minimal HUD (`VrMinimalHudR141`), HUD at infinity, combat
crosshair size, weapon/potion slots hidden until thumbrest + stick. each has its INI comment.

R160: without developer mode the VR HUD page is one simple page: **Icon size** (`VrUiUnifiedScaleR160`, 100% = every
HUD element as big as its own size setting, the crafting icons are not touched), **Crafting / minigame icon size**
(`VrMinigamePromptScale`), HUD mode IMMERSIVE (palm glance, `VrContextualHud`) / MINIMAL IMMERSIVE (`VrMinimalHudR141`) /
NORMAL (both off), eleven [X]/[ ] ticks (block message, tutorials, compass, status icons, lower icons, quick slots
fade, combat crosshair, dot crosshair, cinematic bars, lockpick solid, slow HUD follow) and the HUD distance. developer mode keeps the
old pages, INTERACTION / PROMPTS has the tutorial box position too (`VrUiTutorialYR160`, + = down, 0 = game place).

the map uses the held paper. quest markers come from the native compass subset, not every objective in the world.
map profiling needs F2 or Shift+F2.

Xbox and PlayStation glyphs change the pictures, not the button layout.

## optional gore

the gore button cycles OFF, HEADS ONLY, HEADS + LIMBS. it needs the separate gore pak, which is never merged into the
main one. main gameplay works without it. cosmetic cutting doesn't replace the game's death or damage checks.
LIMBS means arms only (forearm at the elbow), legs are never cut (R153). the extra wound cap meshes from the gore pak are
off by default, `KCD2VR_Gore.limbCaps=true` in the console turns them on again.

## FPS

`VrFpsCapEnabled=false`, `VrFpsCap=90` shipped. the cap value is only stored while the cap is off. for the smoothest
head motion cap the game at the headset refresh in the GPU driver (NVIDIA / AMD frame limiter).
