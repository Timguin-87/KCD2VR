# Settings

most tuning is in the VR menu (F7). the INI stores those choices. every key in KCD2VR.ini has a comment that says what
it does now, grep the key name there. the Experiments pages need Developer mode (setup window).

keep your INI and calibration when you update. an old key can have a number in its name (VrMotionTrialD67,
VrCrossbowReloadRouteR96 ...) because renaming it would throw away a saved choice. the number is the version that added
it, not something you need to know.

the INI is saved on exit. its values are the state at exit, not a timeline of what was active earlier. a feature i
tested and called working is ON when its key is missing, a saved OFF stays OFF. updating never replaces your INI.

values below marked "shipped" are what the KCD2VR.ini in the source root has.

## hands and weapons

**Motion timing** (`VrMotionTrialD67`, Home > VRIK EXPERIMENTS or EXPERIMENTS / LIVE SETTINGS > VRIK+ > Motion
timing). applies and saves at once, no restart.

| choice | value | what |
| --- | ---: | --- |
| BASELINE motion | 0 | the old baseline timing. |
| FRESH POSE timing | 2 | fresh-publication token pacing. the default when the key is missing. |
| ELBOW reach preference | 3 | the elbow comparison timing. |
| Fresh Pose Uncapped | 4 | no native-frame XR-token wait, one extra pose-only refresh before the native update. shipped. |

Uncapped samples head and grips at the current OpenXR time with the NOW camera source, with the predicted camera
source it uses the latest runtime target. the head in that sample is only the hands' reference, it never replaces the
camera head. a busy lock or a stale target falls back to the last completed sample with its real age. "uncapped" means
no token pacing from the mod - it does not override an FPS limiter, the headset refresh or the real CPU/GPU cost.
Shift+F7 records it without turning the token wait back on.

**Elbow preference** (Experiments > BODY): D86 DEFAULT, R89 HAND-GUIDED (`VrElbowSolverR89`) and FRIK TRIAL
(`VrFrikElbows`, shipped ON). hand-guided points the elbow from your hand: behind the wrist along the hand and on the
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

Experiments > Hands > Free-hand contact detail:
- NORMAL: five-point palm contact.
- PRECISE: palm/finger contact with finger fitting.
- BALANCED: five palm points plus thumb/index tips, 2 mm clearance, no finger fitting. same 18-ray / 1 ms query limit.

free controller motion is not filtered. station contact, hammer, tongs and the held workpiece have their own simpler
contact. none of these create rigid bodies.

## objects and corpses

- **Small props in the hand** (`VrStaticPropGrabR89`, Experiments > Objects page 2, shipped ON): grab small scenery
  props. two hands can't claim the same entity.
- **Loose scenery props (swap)** (`VrSceneryPropSwapR100`, Hands page 2, shipped ON): Grip on a mug/plate/candle that is
  level mesh makes it a loose prop you can hold. only where the mesh has a usable path.
- **Corpse dragging** (Experiments > NPCS "Physical corpse dragging"): needs a dead body from the game and an empty
  hand, no recorder or physical combat needed.
- **Corpse search** (`VrCorpseSearchSqueezeR89`, Hands page 2, only used while corpse dragging is ON). ALWAYS (shipped):
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

crafting prompt size: **Crafting UI size** slider on the VR HUD page (`VrMinigamePromptScale`, shipped 0.45). at a
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

NPC defence speed by skill (`VrDirectNpcBlockBySkill`, shipped ON): block raise, parry raise and dodge clip speed come from the
NPC's own melee skill.

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

the map uses the held paper. quest markers come from the native compass subset, not every objective in the world.
map profiling needs F2 or Shift+F2.

Xbox and PlayStation glyphs change the pictures, not the button layout.

## optional gore

the gore button cycles OFF, HEADS ONLY, HEADS + LIMBS. it needs the separate gore pak, which is never merged into the
main one. main gameplay works without it. cosmetic cutting doesn't replace the game's death or damage checks.

## FPS

`VrFpsCapEnabled=false`, `VrFpsCap=90` shipped. the cap value is only stored while the cap is off. for the smoothest
head motion cap the game at the headset refresh in the GPU driver (NVIDIA / AMD frame limiter).
