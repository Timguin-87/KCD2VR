# KCD2VR - history and research (how i got here, what worked, what didn't)

this is not a readme and not a tutorial. it is the memory of the project, so whoever wants to fix something (me in
3 months, or you) doesn't walk into the same wall twice. i burned WEEKS on paths that were dead from the start and
the only reason was that nobody wrote down why it was dropped the last time. so now it is written down.

how to read it:
 - every area has: what i wanted / the history in order (version tag, what was tried, what the headset said) /
   DEAD END notes (don't do this again, here is why) / WHAT WORKS NOW (file, function, constant, INI key, menu
   row) / "if you want to fix X look here".
 - "static" = checked in the code or in the WHGame.dll bytes, without the headset. "headset" / "i tested" = it ran on
   my pc and i saw it. if a thing says "not headset proven" then it is NOT proven, whatever the readme of that build
   said. half of the bugs in this project came from believing a static check.
 - for the static searches in WHGame.dll and for bug hunting in my logs i used AI help (mostly claude). where a
   finding came from that it says so in a few places, everything about the headset is me.
 - version tags in the code comments tell you WHEN something was written: V38..V64 (sep), V65.x (sep),
   V66.1..V66.179 (22 sep - 8 oct, native stereo, body, combat), then the letter rounds in october: B1-B4,
   C1..C66, H3..H50, D30..D87, and R88..R149 in november. an R number in a tag like V66.178R45 or R59.16C10R11
   is a repair on that base. yes it is a mess. the timeline below sorts it.
 - WHGame.dll = the game's cryengine dll. offsets in this doc are for the current retail build
   (TimeDateStamp 0x6A350E20, SHA-256 bdf8f9e4a11257a7...). a game update moves everything - the code has pins (byte
   patterns) for most of them and refuses to run the feature when a pin fails. that is on purpose.
 - paths like src/vrik/physical_weapon.h are relative to the source root.

the rules i work by (where they came from is in 1.3) - if you change the mod, keep them:
 1. a patch must change BEHAVIOUR. logging-only patches and "diagnostic toggles" are not fixes.
 2. full replacement files, not .patch files.
 3. every new INI key = a switch in the Experiments tab, read AND saved, and ON means the feature RUNS (no
    "refused" state behind an ON switch). features i tested and called working are ON by default.
 4. no extra files in the game folder. one log: KCD2VR.log. no .bak of the ini, no finger caches.
 5. no text in the headset unless the F2 recorder runs. reasons go to the log.
 6. NO permanent latches. a refusal retries on the next attempt or after a save load. "disabled until restart"
    is a bug.
 7. say precisely what changed, where (file:line) and why. no vague summaries.
 8. source only. i build it with build_release.bat, no prebuilt dlls from anywhere.
 9. docs in plain programmer language: facts, file names, what to check, no marketing.

sections:
 1. TIMELINE AND THE WORKING RULES
 2. STEREO AND RENDERING
 3. VRIK, BODY, HANDS
 4. PHYSICS, MOTION, TRACKING, DIAGNOSTICS AND TESTING
 5. MELEE COMBAT
 6. INTERACTIONS (pickups, props, doors, holsters, corpses, throwing, petting)
 7. RANGED WEAPONS AND RELOADS (bow, crossbow)
 8. CRAFTING, DICE AND MINIGAMES
 9. UI, HUD, MENUS, INPUT
 10. WHERE THINGS STAND

it is long. use the headings. grep for a version tag or a file name, that is how it is meant to be used.

## 1. TIMELINE AND THE WORKING RULES

this is the "how did i get here" part. not a feature doc - the other sections have the per feature history.

### 1.1 timeline

#### january 2026: crysis vr, to see for myself

it started with the Crysis VR mod (fholger/crysis_vrmod). same engine family as KCD2, 6DOF, hands. i wanted to
see for myself how it is done, so i took it apart and reverse engineered it - how it gets the camera, how it
renders the eyes, how it hands the picture to the headset. nothing for KCD2 yet, just learning how a mod like that
works from the inside.

#### march - august 2026: my own groundwork in KCD2

from march i slowly built towards KCD2, on my own, step by step:
 - mono first. one picture in the headset, sitting right.
 - the HUD in VR. i looked at the Central HUD mod to see how it moves the HUD elements, went through it until
   i found the spot where it repositions them - "aha, here it is". that was the start of the HUD work that later
   became the HUD layer and our own HUD.xml (section 9).
 - the camera through KCD2Tools by Quang Trinh (tkhquang) - the TPVCamera camera hook is where the camera AOB came from,
   and DetourModKit is the same hook library.
 - static investigation of the game with libKCD2.
mono and HUD worked by the end of august. the dated entries from here on are the build line this source comes from.

#### end of august - sep 1: the D3D12 build line

- aug 30: the build line this source comes from. cmake + DetourModKit 4.2.0 (vcpkg back then, retired later).
  first log: "scan::resolve: 'kcd2vr_camera_hook' matched no candidate" = placeholder AOB, i took the camera AOB from
  TPVCamera. then the frustum builder resolved via 'Frustum_P1_PrologueMatrixRead' - first hook that worked on this
  line. then xrCreateInstance -4 (VDXR), black screen + empty log, xrCreateSwapchain ovrResult -7000 on D3D11 ->
  switched to D3D12. steamvr crashed the game ("steamvr is dead route, right now"). in the evening the
  device-capture hook gave a purple view in the headset and a crash before the main menu.
- sep 1: stayed on d3d12. read the crysis vr mod source side by side with mine again. from here every build is a zip
  that gets a bug hunt pass before i test it. there is no way to test in a virtual headset - i am the only headset.
  that never changed.

#### V38..V64 (sep 2 - sep 12): device capture, AFR, the choppy head

- early sep: D3D11 -> D3D12 -> D3D11-on-12 backend changes, GPU device-removed crashes debugged with Nvidia Aftermath
  dumps, device capture via D3D12CreateDevice -> CreateCommandQueue -> ExecuteCommandLists hook chain,
  WaitForFrame/SubmitFrame split. then "20+ hours, zero errors" stability. the PRESENT-capture -> copy into OpenXR
  swapchains mono bridge is from here and is still the fallback path today.
- V10-V17: "native selected camera pair" experiment behind EnableNativeEyeWrite=false; V13 caught an ABI bug in V10
  (6 args not 4) that never fired because the gate was off - first time fail-closed paid off. V40.2 (approx sep 4): the
  .ini had NEVER parsed ("Loaded 0 items", everything on hardcoded defaults) - raw GetPrivateProfileStringA fixed it.
  V41: eye identity from frame epochs, not PRESENT transitions. V45: the "mode 2" i was looking at all the time was
  the SHADOW pass, not stereo. V46.1 FOV via ICVar, V46.2 in-game menu via SetWindowSubclass.
- V47-V53: bad FOV, F6 double image, head jitter, menus too close. V53: sampling the pose "now" made jitter WORSE
  (V48/V50), so the ladder tested rotation order and predictedDisplayTime instead.
- V56.2 "looking through binoculars, nearly there, but not really". V58.7: crysis-style crop + quad F6. V59.7:
  r_Width=1920 r_Height=1800 cl_fov=95 in user.cfg gives a "nearly perfect" view. V60.1: cl_fov=110 only gave 81.69
  deg of real camera FOV -> deferred one-time calibration 500ms after Apply.
- V61.x (sep 5-8): went through the REFramework / crysis / cyberpunk sources again. REFramework is plain AFR
  (m_frame_count % 2), the cyberpunk vr port moved AWAY from a DXGI proxy to device/queue capture like i did. V61.12
  cull-sync: inject HMD matrix only into the gameplay CView frustum builder, restore right after, FOV untouched.
  V61.21.1 MSVC SEH fix. first VRIK: CPoseData writes, V63.4 proved i can move the rendered arm but not the
  weapon/fist authority.
- V64.0/V64.1 (sep 10): F4 UI auto, F5 interaction center, F8 outer native stereo ladder (7 rungs, unaudited RVAs
  left at zero), F9 grip/weapon ladder, F10 read-only. my notes from that run: "F9: no movement of the true
  hand/fingers/fist or held weapon. Elbows/wrists/arm mesh moved/deformed."
- sep 12, V64.36/37: THE head breakthrough. "the choppy juggery head movement is fixed, after 2 weeks". root cause:
  mouse look and HMD rotation entered the frame at different phases. fix: feed HMD yaw/pitch into
  CameraInputDispatch (eKI_MouseX 0x10A / eKI_MouseY 0x10B, delta at +0x18) so the game builds the camera itself.

#### V64.x / V65.x (sep 12 - sep 22): the zip build line

- V64.35 GOLDEN .. V65.01: xinput/interact lab, theater lock, dupframe/late pose, native pad glyphs, native FPS,
  theater camera owner, then the "RELEASE" line (left grip consumables, priority autorelease, game menu state, native
  menu lifetime).
- sep 14-15 (V65.03-V65.04a): corrected mono had a narrow warped FOV, VR laser broken, debug logs compiled to no-op,
  performance bad. my call: don't retire corrected vision, keep normal mono 97/97/0.95 as start, corrected
  vision as a fixed 105 experiment, auto-correct HUD as its own checkbox, FPS limiter OFF by default. F5 dialogue
  camera removal "works and feels good". F2 pose ownership experiment.
- V65.05-V65.77: F9 native IKTarget, projection V2, F10 attachment order, boundary FOV remap, VRIK weapon diag (crash),
  system RenderBegin bridge. V65.55a (sep 19, tested): even correct camera restore + system RenderBegin eventually hits
  the asynchronous CRenderView reset failure -> GUARDRAIL: never two top-level CSystem::Render calls, never
  call CD3D9Renderer::BeginFrame, never clone CRenderView, retail has no r_StereoMode.
- V65.81 (sep 22) = last headset-tested VRIK lineage before the stereo merge: fingers good, F10 upper body moves
  Henry's shoulder/armor, HUD roughly centered. V65.83 integrated debug = load regression (loading stall), logs
  20260923-173153/183553/191655 are the failure boundary. V65.85 VRIK fixes. V65.91/92 F5 inline copy CRASHED,
  V65.93 replaced it with the post-submit scene-color bridge.

#### V66.1 .. V66.95 (sep 22 - sep 30): NATIVE STEREO, then the body

- V66.01-V66.14: single run, read-only preflight, resource prep, zero separation, command list order probe, pair frame
  trace. V66.14 proved the GPU order "LEFT main-writer batch -> RIGHT main-writer batch -> PRESENT". V66.15 (approx sep
  23) = FIRST STEREO: copy LEFT before the RIGHT batch, copy RIGHT after it, publish the pair, OpenXR mode 2. frozen pair
  on purpose. V66.16/17 continuous, V66.19R1 stability, V66.20/21 (sep 24, headset) native menu plane + ownership fixes
  (800ms exit debounce). from here native stereo was not a search problem anymore.
- V66.44-V66.54 (sep 26): the FLICKER hunt. V66.53 headset-confirmed: shadow/specular/stair/water flicker = SVOGI.
  e_svoTI_Apply=0 live -> flicker gone (e_svoTI_Active does nothing, earlier "SVO off" tests were invalid). main SVO
  work runs LEFT pass only (site A 0x180579D64 counters L:8314 R:0). V66.49 SVO per-eye prev, V66.54 SVO per-eye
  history + cheap per-eye fixes.
- V66.66-V66.79: integrated profile UI, frozen owners weapon placement, donor rebuild. V66.80 "TODAY INTEGRATION" (sep
  28) WITHDRAWN: F12 brought back retired weapon candidates (generic slt_0, CEntity transform writes), VRIK was not
  on the V65.81 lineage. rule from that day: every change is RUNTIME-PROVEN / SOURCE-PROVEN / NEW-EXPERIMENT /
  RETIRED-DO-NOT-TEST before a build is called integrated.
- V66.90 native canvas HUD, V66.94 controller core, V66.95 (sep 30, headset): "Full connected upper body finally
  works well... Sword is controllable." first sustained controller-driven weapon. elbows still springy, hands
  penetrate.

#### V66.96 .. V66.179 (oct 1 - oct 8): hands, HUD, native combat funnel

- V66.96-V66.120: native pad UI, semantic HUD, calibration finger capture (V66.100), V66.115R1 (oct 4): "head position
  is good now", "open hands looks quite good" but also the first "elbow on the right side is corkscrew rotating".
  V66.120: "the two handed handling was now nearly perfect".
- V66.136R3 (oct 5, my biggest run then): weapon/fists flickered while turning, hits sometimes worked, bow holster
  worked, sword holster not, strike haptics finally worked, hands stop 1-2cm before environment but clip through my
  own body, head placement good, reflections doubled in one eye, "INPUT DEGRADED transport=SCAN_CODE_FALLBACK". hard
  log finding: localSweeps=559 trajectoryWrites=0 = the tracked blade still did not reach the native hit chain.
- V66.147 = native combat ADMISSION foundation (ProcessCollisionHit -> BuildAndSubmitMeleeHit -> native damage is real,
  the mod feeds it). V66.148 historical HUD, V66.149 immersion options, V66.150 camera smoothness, V66.152/153
  holster layout / native belt holsters, V66.154 single-owner HUD, V66.156 hand hover/use, V66.159 (oct 6) pinned as
  the base, 217/217 tests, no second controller weapon writer. V66.161 = the donor (the top README said V152 - don't
  trust a top README for lineage). V66.162R1 contextual HUD: "the old HUD was working good", "nearly good holsters,
  yeah! (probably)".
- V66.169R1 contact feedback, R2 bow pose hilt, R3 gated perf recovery, V66.170 manual HUD, V66.173 HUD native safety,
  V66.174R1/R2 alchemy camera diagnostic, V66.177 F4 contact damage effects + interruption recovery, V66.178 (oct 7/8)
  "F4 rung 2 VR combat core: strike quality + native rhythm".

#### V66.178R1 .. R59.16 (oct 8 - oct 12): repairs on V66.178

- R1 hand re-arm, R8 XI attack transport, R9 two-hand socket / bow string glue, R17 (oct 9) F4 rung 3 + native kick
  request + NPC flinch, R36C1-C5 bow grip, R44-R51 gesture/guard/draw ("C" suffix = fixes on top of an R), R59
  "kicks for every weapon, master strikes for every weapon, F9 ragdoll switch".
- R59.6 (oct 11): first build without a native swing: VR state driver + contact impulses. "bow in F11 rung 2
  is PERFECT! keep it as a default option". R59.13 dialogue camera stable, R59.16 live stats / swing stamina / weapon
  energy (HENRY_STATS line, physical_weapon.h).

#### R59.16C1 .. C66 (oct 12 - oct 19)

- R59.16C1 CP03 = checkpoint with a manifest + per-file SHA-256, C2 prop gravity/kick/workstation, C6/C7 HUD vital
  bars (oct 13). C7E-DECAP1 (oct 13): "THE PERFORMANCE IS GREAT!!!! like butter smooth, man, it was never like this
  before" - remembered later as the build with the better VRIK too (section 4, 2.1). "immersive (looking at palms)
  was not working". C9A: "interactions are still not working".
- C10R1..R20 (oct 13-15) one build at a time. C10R13 native grasp / controller wrist / native Drop / Stow.
  C10R18BC1/BC2 camera/grasp/native, C10R19 = BC2 + "native guard timing". C10R19C4R1 (oct 16) = "this is the best
  of them all, by far - best blocking (but never perfect ones, never masterstrikes)" and "I HAD AN APPLE for the first
  time" (then henry locked in place). C19C1 headset regression, C19C2/C3 native dialogue + clock.
- C21-C30 (oct 16): one-block combat core, reading/dialogue, native dialogue checkpoint.
  C31: "HUD: BAD, like really bad - black square in the middle started showing again... that is OLD bug" -> HUD revert;
  alchemy "some progress". C32: interactions "a lot better", objects clip through the palm. C34-C37 kick/ragdoll/gore,
  motion, interactions. C41 (oct 17, on C40): "the swings come as they should, enemy is also blocking... nearly good",
  decapitations "after every strike to the head", NPCs don't stop for dialogue. C46 (oct 18): "physics was working
  quite ok, I could move limbs of a dead man", quick talk worked 0.5s, bow camera sway "very nauseating" ("the HEAD
  SHOULD NEVER MOVE by the arrow pull"). C51R1: "THE FRICKING SHOP MENU WAS AGAIN NOT IN THEATER MODE!" C54 (oct 18):
  "the VRIK body correction made the movement worse - elbows are sometimes rotating like around themself" (C51
  shared head/body frame + C54 pelvis ownership, calibrated reach mapping removed); halberd holster "1 win today".
  C55-C59 recovery / lifecycle / wall-shield-map / T3-T4 physical weapon. C62 (oct 19): "alchemy - BAD - the camera is
  not ON henry's head", holsters working. C63 map + stock, C64-C66 DOORS (dev-console breakthrough, never in the
  headset).

#### B1-B4, H3..H50 (oct 19 - oct 24): the regression wave

- B1 combined prerelease (oct 19), B2 combat/ranged, B3 combined delivery, B4 static contract
  "KCD2VR-B4-C66-STATIC-6" is still the base_contract in subsystems.json. G1/G2/G3 crafting/door/mortar freezes.
  H3 (oct 19): "ok, ok... not bad... I did all the weapon types" = the reference for holsters/weapons from then on.
  H4 visor/crouch/boxing, H6 physics "working" (dev console), H7 mapping.
- H8/H9 (oct 20): "like 20% of hits were not landing, could not manage to do any combos... no masterstrike sound at
  all", "hand to hand combat is not working... where did it go", "interactions now are not working, could not pickup
  anything". that day i said it straight: "I DON'T CARE IF IT IS NOT LOGGING ONLY. I don't want any more damn logs, we
  have already 500 logs too much. If it doesn't work, it should write WHY, not log only." H15: 0.6mm wrist -> 39cm
  elbow jump. H21/H22: "after second punch from me - crash", "physics? what, where? no npc reactions".
- H26-H29 (oct 21): "THE VRIK is so bad, my hands are flailing behind me when I move forward, what? WTF?", "THE HITS
  ARE like 80% not registering" (H28 log: 617 requests, 569 rejected, 8 submitted). H26->H28/H29 removed the C54 pelvis
  foundation then restored it - only for the legacy path, so neither VRIK existed. H29R2 native parchment map
  (untested), first test "there is no map showing - THE INVENTORY OPENING, that is what is happening". H29R3
  HEADHELD_FIX (oct 22): "better, still some misses". HEADHELD_REPAIR1 d0ec8c4b: "A lot better... if enemy
  is in combo, I can maybe interrupt him mostly only with stabs". "apples: I could not hold any (floating), only the
  milk jug".
- H39/H40 (oct 23) hand-only door route. H41 (oct 23, STEP28 1ff56ce): "everything is not better, but worse". swings
  60-80% not registered "ALSO IN THE FUCKING DIRECT MODE", "none of the kicks are showing", "THE FUCKING DOORS ARE
  STILL NOT WORKING!", grabbing worse (item attached then our own cleanup code detached it 99 ms later - even though
  F7 still recorded Grip fully squeezed), wall collisions worse, VRIK "elbows are now little corkscrewy". the lesson:
  too many changes combined before proving they kept what worked, and too much trust in compiling, isolated tests and
  reading source. H42-H49 host-tested patches (grab retention decoupled from finger capture, three duplicate speed
  vetoes refusing 82 samples at 41-65 m/s, face-only fist block was never face-only, windowed laser cursor stretched,
  tip-travel slider missing from save, per-frame flood gated). H50 craft chain, A34 passenger coherence (oct 23),
  H41_STEP30 F9 independent grip (oct 24, host-validated, HEADSET NOT RUN).

#### R14..R28, J27, knives K33-K36 (oct 25 - oct 27)

- oct 25: one line of builds from now on (M1 plan on H50): one final zip of full replacement files on THIS base, i
  merge and build it. no new baseline, no INI reset, no game install. R4-R11 overlays (R8B/R8C frik elbows, R8D
  holster diag, R9, R10 fists, R11, R12 gore phys).
- R14 FAST_VISUAL (oct 25/26): the jitter report found the real problem: the VRIK pose re-roots from the LIVE NATIVE
  ANIMATED skeleton every frame, with no temporal filter on that path - it is not in the controller input at all ->
  F2 rung 2 no longer falls back to raw above 1 m/s. headset-tested oct 26 00:28-01:02: R14 = FROZEN BASE ("a good one,
  keep and upgrade"), fights "very good now". from then the work was split per area: gore + combat, VRIK body/
  smoothness, alchemy, smithing, holsters, history/research.
- R15 station/holster, R16 contact/smith, R17 gesture recovery (forge-bag pump: grip within 20cm, 3cm downstroke),
  R18 ownership (437.28mm recovery jump class), R19 resource transfer, R20 owner coherence, R21-R23 alchemy mode/attach/
  grip continuity, R24 camera family private, K33-K36 knives (left chest pouch 12cm left of sternum, replenish to 5),
  R25 gated delivery (first paired DLL/PAK, headset-tested live comparisons), R26 station visibility, R27 regression
  (one MAIN weapon rule, torch + knives are the left-hand exceptions), R28. all R15-R24 and R26-R28: "HEADSET
  NOT RUN". J27 = the headset-tested overlay on R26 (oct 26): "made the holsters worse - I can't pull my bow out"
  (R6 overlay), holsters were fed the reach-compressed VRIK hand, 52-70cm off.

#### C29 / D30 .. D87 (oct 27 - nov 2): headset runs every day

- C29 on R28 (A3 real anvil via CPhysicalWorld::GetEntitiesInBox vtable slot 34, HS holsters, impale, NPC look
  HMD). D30 (oct 27): R28 + C29 + repeating logs gated behind F binds. one critical bug found before link (combo
  carrier reads the slots pointer -> Drop on every window end) -> D31. M49 (oct 27): WRONG DLL INSTALLED - the game
  ran J27 (dinput8 marker KCD2VR-R26-PLUS-CLAUDE-J27) not D35, the whole run proved nothing. D36-D54 (oct 28-29): dead
  code, README, limbs, DS22, activity evidence, crossbow arrow, D49 FINAL TEST BUILD, D52 gore, D53 black frame
  findings.
- D55 headset run (oct 29, DLL c43edb12...): holsters were good, don't reopen/rewrite that subsystem. hand-to-hand
  worked (elbows unnatural), fights quite good, alchemy book improved, bellows worked. map: input arrived but
  presentation lost fresh hand data (80ms freshness boundary, 143-150ms stale, 156ms loop stalls). VRIK: wrist
  7.12mm -> elbow 74.62mm, shoulder 65.10mm in 10.53ms = the wrist is not the only moving input.
- D56-D62 (oct 30): startup repair, knife static map, gestures, hand presentation/offhand, D62C failed run. D63R1 (oct
  31, marker R28-PLUS-D63R1): rear weapon coming forward "probably fixed", collisions improved, pickup contact bad.
  VRIK: requested goal 2.3mm, final collision-adjusted goal 124.4mm in 12ms, 90 of 93 big jumps PUBLISHED->PUBLISHED
  = the elbow solver follows an already jumping endpoint (pose_free_hand_contact.inl -> pose_arm_solver.inl,
  ProjectPalmPlanes6697 in frik_pose_probe.cpp:2478-2545). D64-D73 (oct 31): integrated repair, heavy crossbow, D66
  headset + knives, free hand trace, hand pose, D70 headset repair + D70G gore, D71 contact transfer, D72 recorded
  repairs, D73 combined + map backend.
- D74-D81 (nov 1): headset run + repairs, native links, roomscale activities, draw diagnostics, D79 headset run: "Bow
  aim/orientation now fixed. Mortar emptying now works with particles. Dialogue finger Trigger works." failures:
  riding hands 3m above, alchemy hands stick/elongate, icons too big, dice zoom too near, map flicker + cost, 85-95 FPS
  vs remembered 100-110, crossbow gestures start a native animation instead of driving the reload. two things that
  became rules: no generated diagnostic/backup files in the game folder, no mod status/debug popups during quiet play.
  D80 NPC idle, D81 collision fingers / dice seated / map snapshot. D82 temporal stereo + DLSS/FSR stereo, D83R1
  merge, D84R1 (nov 2, D86 kick fallback NOT imported), D85R1-R6 smith/dice, draw/reload, two-hand, knives, fresh pose
  uncapped, D87 kicks + D87R1 DLSS live.

#### R88 .. R106 (nov 3 - nov 5): git worktrees, the merges

- R88/R89 (nov 3) on D85R5: corpse/dice/elbow/pickup/xbow. R89 pickup found WHY nothing reacted to small props: 144
  contact rows bodyClass=BASE owner=0 = brushes (no entity, can't move), ~190 BASE with entity = PE_STATIC dressing
  props, and lab_prop58.h:51 refuses STATIC_BODY by design. new switch VrStaticPropGrabR89 = Experiments > Objects p2
  "Small props in the hand".
- R90 (nov 3) = D85R5R2 + my fixes as options. R2 fixed the elbow flicker too (D85R5 112 jumps >15 deg, R2 0, R89 0)
  so R2 stays default. screen messages only with F2 - fix, no option (test screen_f2_gate_r90 35/35, R2 code fails
  17/35). the crossbow hand route still did not move the visible mechanism live, plus a scenery grab two-hand
  conflict.
- R92R1-R4 / R93 (nov 4): complete source zip again (export manifest with commit + hashes), reload transfers,
  crouch modes, pointing quick talk. R92R1 did not compile (presentationOwns71 five vs four args). R93R3/R4 build +
  repair. R94R1. R95/R95R1 crossbow bolt.
- R96 (nov 4-5) on R95R1, from my 17:44/17:50/18:05 nov 4 logs: crossbow flicker = two writers taking turns
  (R95 held 80ms then handed back), HAND ONLY route was dead (CrossbowHandOnlyR90() returned false), new key
  VrCrossbowReloadRouteR96 because R92-R95 wrote the old key false on every exit. horse 2m above = right-side mount
  clip unknown (R95 OK). "body follows native" killed VRIK for good (uncapped hand stream stayed owner) -> fixed
  tracking_sample59c6.h. .bak files removed. 23 "off until restart" latches now re-arm on load (load_rearm_r96.h).
  R96R1 = second pass on R96.
- R97 (nov 5) = the 3-WAY MERGE: R96R1 + R96, both from R95R1, git merge, 8 conflicts, 17 both-sides files read line
  by line. kept from R96R1: 500ms spanner join limit, seat foundation per mounted visit, pose-restore debt, door VM
  clear. kept from R96: HAND ONLY route, wrist hold, getup native, hand stream latch fix, no .bak, kicks (12 SPECIAL
  melee rows), combo HOLD. removed R96R1's 1s settle after mount (camera error 0.046m 0.1s after the clip). impale
  rule merged: BLADE contact impales only if the tip is >= 2cm under the surface. ONE load signal (R96R1's observed
  load episode, R96's rearm).
- R98 (nov 5): body height 3 modes VrBodyHeightModeR98 (0 HMD / 1 NATIVE / 2 HMD+NATIVE ride/crouch) because in
  native mode the shared frame moved ~8cm up/down 3x/s with the walk bob and the weapon chased it ("comes from behind").
  R99 kicks fix, R100 loose scenery props brush swap (VrSceneryPropSwapR100 ON), R101 corpse follow no stretching,
  R102 SteamVR controller input fix (3 files: xr_input.cpp/.h, xr_manager.cpp), R103 held props/corpse don't push you,
  crossbow lever in hand, free targeting, mortar. R104 SteamVR overlays: SteamVR refuses the WHOLE xrEndFrame if one
  quad layer is bad (VDXR draws it anyway) = "no HUD, no laser, menu only on the slowdown" -> xr_layer_guard_r104.h
  repairs every layer, resubmits the frame without overlays, sit-out 1s/10s/20s/max 60s, nothing off for the session.
  R105: session lifecycle (STOPPING/READY), startup waits for READY, input neutral per xrSyncActions. R106
  (nov 5): kept the lifecycle, +8ms pause while stopped (worker spun 100% one core with the dashboard up), put back
  the second xrEndFrame + sit-out. marker KCD2VR-R106-STEAMVR-SESSION-LAYERS-20261105. most of the feature sections
  below describe the tree at this point.

#### R107 .. R149 (nov 5 - nov 8): small rounds, one thing at a time

the KCD2VR.ini comments carry the details for each of these (grep the R number):
- R109 the empty left hand also draws a knife at the chest pouch. R111 with a saved T-pose (F8) VRIK switches on at
  every game start and save load, start/load/recenter put the view at Henry's eyes. R111/R117 heavy and medium
  crossbow: grip the string at the front and pull it back by hand. R112 directional combat crosshair size.
  R113 quick-access modifier, R114 sRGB swapchains (VDXR colours like SteamVR). R115 weapon/potion slots hidden until
  thumbrest + stick. R116 held small props stay locked in the hand, weapon whoosh on release, alchemy picks what you
  look at or reach. R118 controller layout. R120 shield block box, master strike gate. R121 the look/selection ray
  starts at the eye you see from. R123 one-handed weapons sit at their own handle socket. R124 plain F7 = VR menu,
  Shift+F7 = VRIK recorder, dropped mugs/plates play the game's own sounds. R125 HUD at infinity, combat cross arms
  toggle. R130/R131 the setup window opens by itself at the first game start. R133 free targeting mode. R137 headset
  centre selects objects in VRIK too. R141 Minimal HUD, tutorial messages hidden, eye hold (no black frame when the
  game stalls).
- R143 (nov 7): the cheat-flagged cvars the mod writes were refused without the game's -devmode - fixed with a direct
  write helper in mod.cpp, so -devmode is not needed. R144 VR2 GSM cascades kept in STEREO after preset/spec rewrites,
  VrCullFrustumScale reaches the frustum hook. R145 setup writes e_GsmSizesWH into user.cfg, crafting UI size slider.
  R146 NPC block/parry/dodge speed from the NPC's own melee skill (VrDirectNpcBlockBySkill), recorder fast exit in
  diagnostic_trace::Write (less cost with recording off). R147 on the horse a short left X tap sends the game's Follow.
  R148 SteamVR start/load facing behind you -> recenter when the head faces >90 deg away, intro screen re-placed.
  R149 (nov 8) no saved T-pose -> VRIK still starts on a default arm profile from the eye height.
  R153 NPC defence by skill: R146 only sped up a defence the NPC already chose, in the nov 8 run (F4 press 1) weak
  enemies never chose one. with BY SKILL on, one roll per Henry swing against his reciprocal opponent, 25 % at
  skill 0 .. 75 % at skill 30, a won roll leases the game's wh_cs_ForceBlock=1 for that swing (C40 had retired the
  cvar because it is global). VrDirectNpcBlock 1/2/3 (65 %/always/always perfect) now work in every F4 press.

### 1.2 lessons (the pattern that made every repair worse)

from the oct 23 look back over H8..H41, still valid:
1. OVER-GATING: the mod re-implements permission checks the game already makes and those vetoes silently cancel accepted
   actions (reach veto switching the weapon to display-only "exactly when you swing wide", three duplicate speed vetoes).
   "WE ARE THE ANIMATION... IF THE GAME ALLOWS US TO ATTACK - THEN IT HAS THE BLOCKS ALREADY IN THERE!"
2. CROSS-SUBSYSTEM COUPLING: one shared pose/input owner changed for X breaks Y and Z (holsters fed the reach-compressed
   hand, pickup retention coupled to finger capture, body alignment changes the wrist that drives weapons and holsters).
3. FIXES-NEVER-SHIPPED: partial restorations called repairs (face-only fist block, H26->H29 pelvis, H17 reach
   mapping only for the legacy frame, C8 palm alignment tested with the option OFF while gameplay had it ON).
4. SPECULATIVE PROOFS: host tests / console demos / "ok=true" from a pcall taken as headset proof.

static check vs headset: static validators (82/82 PASS, 21/21 exact WHGame audit), host suites (138/138, 1235/1235
ASAN), Windows link EXIT0 - none of it ever predicted the headset. so: a static check = the build compiles and the
tests of the changed thing pass. headset = me, with F2/F3/F5/F7/F9/F11 recording, then i read the log.

### 1.3 the working rules and where each one came from

these are MY rules. every one has a date and an incident. if you work on this mod, these apply.

1. BEHAVIOUR, NOT LOGS. a patch must change behaviour so the feature works. no log-only / receipt-only patches, no
   test toggles whose only job is to log. if a feature can't run it says WHY in one short line. origin: oct 20
   after H8/H9 ("I don't want any more damn logs, we have already 500 logs too much"). the logs were 800k-1M lines per
   session by then (D55: 813,493 lines; D79: 1,032,763 lines).
2. NO HEADSET TEXT UNLESS F2 RECORDS. the "why" line from rule 1 goes to the headset only while the F2 recorder is on;
   otherwise only to KCD2VR.log ("VR status: ..."). origin: D79 run nov 1 (no popups during quiet play), made a
   fix-no-option in R90 nov 3 (vrik/visible_failure.h screenAllowedR90, test screen_f2_gate_r90 35/35). the gore pak's
   say() lines are the known exception, separate pak.
3. FULL REPLACEMENT FILES, NOT .PATCH. i can't apply patch files. full files in the same folders. origin: oct 25, and
   the R92R1 episode where a patched tree did not compile and i went back to complete source zips (R93).
4. EVERY INI KEY IS A SWITCH IN THE EXPERIMENTS TAB, AND A SWITCH IS NEVER REFUSED. ON means the feature runs. no
   code path may say "switch is on but i refuse because X" silently. origin: oct 20 ("If a toggle is needed, it must
   switch the real feature") plus the C8 incident (tests ran with the option OFF while gameplay had it ON) plus H45
   (wall response gated out of ordinary hybrid until the gate was removed). examples: VrStaticPropGrabR89 =
   Experiments > Objects p2 "Small props in the hand", VrCrossbowReloadRouteR96 = "Crossbow reload route",
   VrElbowSolverR89 = Experiments > BODY "Elbow preference", VrSceneryPropSwapR100 = Hands p2 "Loose scenery props
   (swap)".
5. PROVEN FEATURES ON BY DEFAULT, NO SILENT FALLBACK THAT SWITCHES A WORKING FEATURE OFF, NOT TOO MANY OPTIONS. if i
   tested it and said it works (6DOF interactions, dog petting, bow F11 rung 2 "keep it as a default option"), it ships
   ON. origin: oct 11 (bow), oct 20 ("on by default if it works, no hidden test toggles"), and the R96 "body follows
   native killed VRIK for good" case where a fallback owner never gave the hands back.
6. NO EXTRA FILES IN THE GAME FOLDER, ONE LOG = KCD2VR.log. no finger caches, no .ini .bak backups, no DetourModKit_Log,
   no separate perf/startup/diagnostics logs. origin: D79 run nov 1; R96 removed the writers:
   config_file_guard_d62.h:55-84 checkpoint() wrote KCD2VR.ini.last-good.d62.bak on EVERY INI lease,
   startup_setup_s15.inl:177/:267 wrote KCD2VR.ini.pre-setup.bak and user.cfg.pre-setup.bak. nothing ever read them
   back; last-good INI is in memory now.
7. NO PERMANENT LATCHES. a refusal retries on the next attempt or after loading a save; nothing stays off until restart.
   origin: R96 ("something stops working once - impale hang, corpse drag, VRIK hands, holsters, combos, VR hits - and
   never comes back, even after loading the save"). R96 found 23 latches (gore32n::g_drive_fault33,
   lab58n::g_dragFault, touch43::g_latched, g_prop_fault5916c1 ...), all "until restart" before, all re-armed by
   src/core/load_rearm_r96.h on the native LoadingScreen edge (mod.cpp:6586, RearmAfterLoadR96 in frik_pose_probe.cpp,
   RearmCombatAfterLoadR96 in native_hit_probe134.inl). R97 merged it with the R96R1 load-scoped Fault type = ONE load
   signal.
8. PRECISE REPORTS. i hate short vague summaries. each change: where (file:line), what, why, which log line proves it,
   what is still open, what i should test in 1 minute. origin: the whole sept-oct "it is fixed" / "nothing changed"
   loop.
9. SOURCE ONLY, I BUILD. origin: M49 oct 27 where the game ran a J27 dinput8.dll while i thought it was D35 - a whole
   headset session wasted. the build marker in the first KCD2VR.log lines is how you check (KCD2VR_BUILD_MARKER in
   src/build_identity.h).
10. DON'T EDIT A TESTED BASE IN PLACE. a tested base (R14 FROZEN BASE, C40, D83R1, R95R1 ...) is never modified in
    place, the change goes on a copy and gets merged. also: never reset my INI or calibration.
11. PRAYDOG-STYLE DOCS. plain programmer language, short lines, facts only, never ";" in a sentence. credits: libKCD2 +
    KCD2Tools research, Crysis VR / PLANCK / HIGGS / FRIK inspiration. origin: mid sep ("Write it like a human would")
    and oct 16 (C26 README is the example).
12. NATIVE SEMANTICS, NO FAKE DAMAGE. the game owns locomotion, AI, combat eligibility, damage, stamina, armor, crime.
    no health writes, no duplicate damage, no second controller weapon writer to hide flicker, no giant grace windows
    to hide bugs. headset observation outranks a source assumption. the latest version number does not automatically
    own a subsystem.
13. ONE BUILD AT A TIME, FREEZE, TEST, CONTINUE. "Avoid giant combined builds" (oct 6) - the H41 and V66.80 disasters
    are the proof. and "DO NOT LOSE PROGRESS. Do not rediscover previously disproved candidates" (sep 28).

### 1.4 build markers

KCD2VR_BUILD_MARKER in src/build_identity.h, e.g. "KCD2VR-R106-STEAMVR-SESSION-LAYERS-20261105". the first lines of
KCD2VR.log print it. ALWAYS check it before believing a headset run (M49). older markers are in the code comments too
(V66.178R45, R59.16C10R11, D40, H18, C44, R93...) - grep the marker to find who owns a line. the install is
Bin\Win64MasterMasterSteamPGO\dinput8.dll + Mods\KCD2VR\Data\KCD2VR.pak (one combined pak; gore is a separate
optional pak).

### 1.5 where is what in the source tree

top: CMakeLists.txt, build_release.bat (the build), VERIFY_BUILD.cmd, KCD2VR.ini (shipped defaults, every key commented),
subsystems.json (module registry: base_contract KCD2VR-B4-C66-STATIC-6, shared_host_bridges list, per-module
owned_files), README.md, CONTRIBUTING.md, "Start KCD2VR.cmd.in", assets/, cmake/, companion/, data/, deps/, docs/,
mods/, src/, tests/, tools/.

#### src/ top files (the suffix numbers are the version that introduced the file)
- entry/runtime: dllmain.cpp, proxy_dinput8_29.cpp/.def (we ARE dinput8.dll), mod.cpp/.h (host: startup, INI lease, menu
  save, load edge mod.cpp:6586, the big Tick), build_identity.h (marker), retail_build112.cpp (exact WHGame check).
- render/XR: device_hook.cpp (D3D12 device/queue capture), xr_manager.cpp (OpenXR session, layers, xrEndFrame),
  xr_layer_guard_r104.h, xr_input.cpp (R102 steamvr), frame_sync.cpp, camera_hook.cpp (frustum/CView injection,
  CameraInputDispatch), native_stereo_*.inc/.cpp (V66.00-08 stereo path + signatures), native_svo_*/taa_*/postaa/cloud/
  water_reflection_right (per-eye fixes), native_dlss/fsr/xess/upscaler_stereo_d82/d85, temporal_capture_*_d85.
- camera/dialogue: dialogue_camera_override.cpp, dialogue_*19c2..59c10r2, npc_lookat_hmd.cpp, head_aim120r3.h,
  immersive_fov59c7g.cpp, focus_zoom58c2.h, comfort_vignette51.h.
- VRIK host: frik_pose_probe.cpp/.h (the big one: ProjectPalmPlanes6697 ~2478, NativeRhythmTick178, RearmAfterLoadR96
  ~3945), vrik_bridge.cpp, tracking_sample59c6.h, animation_filter.cpp (clip aliases, mount/dismount/getup lists),
  ragdoll_lease59.h, native_weapon_body_t4.cpp, native_weapon_clearance_t4.cpp, native_physics_phase_t4.cpp.
- combat host: combat_diag_probe.cpp/.h, action_time_owner59c11.cpp, combat_menu49.h, melee_tuning_ui_c54.h.
- interactions host: native_grasp59c13.inl, native_pickup_clock_h23.inl, pickup_controller_scope_d44.inl,
  interaction_*.inl/.cpp (doors, native selection), door_signal19c4r4.cpp, alchemy_*.inl, semi_smith_clock46.inl,
  native_supply_t2.inl, native_weapon_drop_t1.inl, native_script_t2.cpp (Lua bridge), companion_loader59c4.cpp.
- UI host: vr_menu.cpp/.h (the VR menu incl. Experiments tab), ui_*.h (palm HUD ui_palm171.h, contextual HUD
  ui_contextual_hud162r1.h, ui_menu_router.h, pause/death overlays, prompt scale), hud_layer_v6664.cpp (the OpenXR HUD
  quad), hud_xml_mod.cpp + hud_xml_base.inc (our HUD.xml), flashui_probe.cpp, action_map_probe.cpp, touch_bindings.cpp,
  native_vr_pad_premap.cpp, prompt_device_lock.cpp, activity_lens59c10r1.cpp, held_map_*(m1/t2) (the VR map),
  theater_exit113.h, graphics_menu_options.h.
- diagnostics host: diagnostic_trace.cpp, debug_trace.h, diagnostics/ (see below).

#### src/subsystems/<module>/ (17 modules, each has module.cpp/.h, host.inl, menu.inl, sources.json)
base (startup_setup_s15, combat mode/guard/melee policy controls, pose_physical_executor, native_motion_owner,
startup_graphics_d80), hybrid (contact telegraph, drains, native cost/answer, route state - the HYBRID combat mode),
direct (DIRECT combat mode), masterstrike_combos, physics (world contact, corpse, constraints), interactions,
alchemy, smithing, crafting, doors, dialogues, dice, hud_ui_controls (+runtime/: xr_input, hud_layer, hud_xml_mod,
ui_menu_*, quick_talk_native_h19, pad_dispatch_d84, prompt_*), vrik (46 files: calibration_*, pose_arm_solver.inl,
pose_leg_solver.inl, pose_fingers.inl, pose_torso_stage.inl, pose_frame_foundation.inl, native_crouch_r93.inl,
tracking_producer/reader.inl, rig_resolver.inl, finger_reference.inl), vrmap, gore, throw_drop, ranged_reload (40 files:
crossbow_hand_d89.inl, crossbow_rifle_request_h21.inl, bow/arrow), roomscale, experimental (the Experiments tab
backend: features_menu_h5.h, claude_options_h8.h, deepseek_options_h29.h, live_settings_d39.h, free_targeting_d42).
src/subsystems/catalog.cpp + subsystem.cpp = the registry.

#### src/vrik/ (391 files, the VRIK + weapon + holster + alchemy "inl" library)
coherent_pose.inl (THE pose composer, shared host bridge), physical_native.inl, native_motion112.inl, physical_weapon.h,
tracked_frame51.h (shared_frame_delta_r98), elbow_r89.h, visible_failure.h/.inl (screen line gate), lab58.h/.inl (F9
corpse lab), staged_pickup596c2.inl, alchemy_*, bow_*, belt_layout164.h, calibration_*, clinch_*, combo_*, constraint_*,
riding_vrik_d76.h, riding_aliases_r95.h, user_modes175.h, brush_swap_r100.h.

#### src/core/ (shared primitives)
config_file_guard_d62.h (INI lease, in-memory last-good), load_rearm_r96.h + observed_load_recovery_r96.h (the one
load signal), haptics.h, snapshot_mailbox.h, native_grasp13.h (+pins), grasp_transaction59c12c.h, instant_pickup_d72.h,
door_* (19c4/65/66 pins, hold, motion, path, script), book_*/reading_pages42.h, player_identity37.h,
trigger_ownership.h, render_pacing59c6.h, strike_power_rule59c10r6.h.

#### src/diagnostics/
capture168.h + capture_control168.h (the F-key recorder leases), geometry_hit178r27.inl (geom27::Drain, VR hits),
native_hit_probe134.inl (native hit funnel observer), native_defense_*, native_services59c5, contact_path122.inl,
combo_observe.h, block_indicator_h24.h, animation_alias_catalog.h. docs/DIAGNOSTICS.md is the key table (F2 bugs/timing/
elbow, Shift+F2 perf, F3 combat, F5 interactions, F7 VRIK, F9 physics, F11 reload).

#### companion/, mods/, tools/, tests/, docs/
companion/source/ (AI, Animations, Libs, Objects, Scripts, gore = what goes into KCD2VR.pak), companion/Mods/kcd2vr_lab.
mods/: KCD2VRDoors (hand_only_h40.lua), KCD2VRGore (separate pak), KCD2VRMap (ours, shipped), KCD2VRStow,
KCD2VRThrowingAssets, kcd2vr_tests. tools/: analyze_*/audit_* python per build, export_source.py (manifest),
Apply-Gore-D56.ps1. tests/: *_test.cpp per feature (load_rearm_r97, screen_f2_gate_r90,
config_file_guard_d62, static_grab_r89...), README_D87_RUNNER.md. docs/: BUILDING, DEPENDENCIES, DEVELOPMENT, SETTINGS,
KNOWN_ISSUES, DIAGNOSTICS, LAUNCHER_OPTIONS, TESTING and this file.

### 1.6 if you want to fix X, look here

- "who owns this line of code?" -> grep the version suffix (59c13 = R59.16C13 grasp, 19c4 = C19C4, h21 = H21,
  d82 = D82, r96 = R96), then this doc and the KCD2VR.ini comment for that number. subsystems.json owned_files =
  module; shared_host_bridges = the files everybody touches (mod.cpp, frik_pose_probe.cpp, coherent_pose.inl,
  geometry_hit178r27.inl, native_hit_probe134.inl, action_time_owner59c11).
- "what was tried for stereo and must not be repeated?" -> section 2, the DEAD END notes.
- "why does this rule exist?" -> 1.3 above.
- "did this ever run in the headset?" -> if this doc says NOT headset tested / HEADSET NOT RUN, it didn't, no
  matter how many host tests passed.

---

## 2. STEREO AND RENDERING

this is the story of how the game got a picture into the headset at all, how it got two DIFFERENT pictures (real stereo, same game frame), and all the shit that came with it: shadows flickering in one eye, clouds missing in the right eye, TAA smear, blown out sky, black frames, stale frames, 45 fps "mono" when stereo silently died. If you touch anything in `xr_manager.cpp`, `device_hook.cpp`, `camera_hook.cpp`, `frame_sync.cpp` or the `native_stereo_*` / `native_*_right_*` files, read this first. a LOT of dead ends are documented here so nobody repeats them.

short version of what runs today (R106): KCD2 renders LEFT eye as its normal frame with the camera shifted -IPD/2, then the mod re-enters the native graphics pipeline a second time in the SAME game frame with selector=1 and the camera shifted +IPD/2 (RIGHT). The engine reuses one main target, so the LEFT image is copied out of the D3D12 queue between the last LEFT batch and the first RIGHT batch, RIGHT is copied after its batch, the pair goes to OpenXR as a projection layer (2 views). HUD, menus, lasers are separate OpenXR quad layers. per-eye temporal histories (TAA, clouds, SVO, water reflections) are repaired by the mod because retail only allocates them for eye 0.

#### 0. timeline in one table

| when | tag | what |
|---|---|---|
| Jan - Aug 2026 | - | my own groundwork: crysis vr reverse engineering, mono, HUD in VR (Central HUD study), camera via KCD2Tools, libKCD2 |
| 30 Aug 2026 | V1..V17 | the build line of this source: ASI + OpenXR bring-up, D3D11 -> D3D12, device capture, mono again on the new line |
| Sep 2026 | V38..V64 | mono / corrected mono / pseudostereo / AFR, FOV + projection center, head jitter + pacing experiments |
| 10-19 Sep | V64.1..V65.55a | native stereo hunt part 1: two CSystem::Render calls (all crash) |
| 19-23 Sep | V65.56..V66.12 | COUNT2 native N-view, MRT sweeps, selector=1 eye pipeline, command-list order |
| 23-24 Sep | V66.15 / V66.19R1 | FIRST REAL STEREO in the headset, then stable continuous pairs |
| 24-28 Sep | V66.22..V66.80 | making it look right: SVO, clouds, TAA, water, GSM, sky, HUD |
| Oct 2026 | V66.1xx, C/H/D | pacing, fresh pose, render pacing identity, vignette |
| 1-3 Nov | D81..D85, R88 | DLSS/FSR/XeSS per-eye, live switch, start flicker |
| 5 Nov | R102..R106 | SteamVR runtime: input profile, layer validation, session lifecycle |

#### 1. getting the picture into the headset on the new build line (30 Aug - 5 Sep 2026)

what i wanted: a KCD2 VR mod like fholger's crysis_vrmod / the cyberpunk port, OpenXR, 6DOF, hands. mono and the HUD i already had from my own work before (section 1), this is the build line that became the source. Quest 3 over Virtual Desktop, so the OpenXR runtime is VDXR.

history:
1. the first build on this line was an ASI (winmm.dll loader, DetourModKit) with a D3D11 OpenXR session. `xrCreateSwapchain failed for eye 0, code=-2`, VD log `ovr_CreateTextureSwapChainDX ... -7000`. everything matched hello_xr. DEAD END: D3D11 under VDXR. "ok, switch do D3D12".
2. D3D12: the "unsupported graphics card, D3D feature level 12.0 required" error on game start was NOT our code, it was SteamVR running in the background ("ok, steamvr was running in the background. I killed it").
3. device capture: hooking `IDXGIFactory::CreateSwapChainForHwnd` timed out (another module's hook sat there already). i moved to `D3D12CreateDevice -> CreateCommandQueue -> ExecuteCommandLists` detours, still the chain today: `DeviceHook::DetourD3D12CreateDevice`, `DetourCreateCommandQueue`, `DetourExecuteCommandLists` in `device_hook.cpp`. the cyberpunk VR port's changelog says they moved AWAY from a full DXGI proxy for the same reasons.
4. camera: `CCamera::UpdateFrustumPlanes` found by AOB from tkhquang/KCD2Tools TPVCamera (3 signatures P1/P2/P3, `CameraHook::Install`, log "frustum builder resolved via candidate 'Frustum_P1_PrologueMatrixRead'"). HMD pose applied there (culling) and at `CSystem::SetViewCamera` publish (`FrameSync::PublishViewCameraDetour`, V45/V51).
5. GPU device-removal crashes (Aftermath dumps) came from the CustomRes experiment with TYPELESS formats (cyberpunk had the same bug). made inert, 20+ hours zero errors after.
6. the .ini never parsed until V40.2 ("Loaded 0 items" = every setting on defaults for weeks). raw `GetPrivateProfileStringA` fixed it.

DEAD END: D3D11 and D3D11-on-12 backends. DEAD END: DXGI swapchain proxy. DEAD END (then): SteamVR - "steamvr is dead route, right now". it came back in Nov (section 9).

#### 2. mono, corrected mono, pseudostereo, AFR (Sep 2026, V38-V65)

until the native stereo in late sep the headset got ONE image. the work was about making that one image sit right.

##### 2.1 mono FOV / "binoculars" / corrected mono
- V47.0: the projection layer must describe the pose that RENDERED the pixels, not the newest pose (`present_pose_pairing_mode_`, `CopyRenderFrameStateByIndex`, xr_manager.cpp ~line 4080). still there.
- V56.2: "looking through binoculars". `proj_views[eye].fov` was the headset's native fov but the blitted content covered less. fix = tangent-space remap of the source image into the real per-eye OpenXR tangents (`xrTan`, `sourceTan` in the PresentParams cbuffer, shader map mode 5).
- V58.7: crysis-style crop of an oversized symmetric frustum. test showed 0.0% clamp activity = mathematically identical at the tested FOV, so no visible change.
- V59.7: i found `r_Width=1920 r_Height=1800 cl_fov=95` in user.cfg looks nearly perfect. i computed the needed symmetric vertical fov ~110 deg. `GetRequiredMonoSourceVFovDeg` had a hardcoded 2560x1410 aspect bug, fixed.
- V60.1: raising the fov slider made the zoom WORSE: `cl_fov` and the mod's remap-fov were two separate values the Apply never synced. and `cl_fov=110` confirmed-applied gave only 81.69 deg measured camera fov. so a deferred one-shot calibration reads the real published fov ~500 ms after Apply (`FrameSync::LatestPublishedFovRadians`).
- V65.07..V65.10: "corrected mono" = source rendered at 120 deg, fitted VR framing 98 deg (my choice: `fittedRay = xrRay * tan(98/2)/tan(actual/2)` then sample the 120 source). V65.07 cancelled the geometry algebraically (DEAD END, too zoomed). V65.08 one-shot Y/pitch centering removed the black bottom bar - keep that mechanism.
- V65.27: CORRECT FOV is a BUTTON not a preset: user picks Game FOV + VR FOV, APPLY commits cl_fov, CORRECT recalculates the map-5 projection from that pair and live per-eye tangents. RESET VIEW = 97 game / 97 VR / 0.95 framing. don't hardcode 95/105 (V66.30 did, rejected). INI: `VrGameFov`, `VrFov`, `VrSourceVerticalFovDegrees`, `VrSourceWidth/Height` (restart, "V63.1-SOURCE-RES restartRequired=true"), `VrRenderScale` (live XR swapchain rebuild 0.5..2.0 on the XR worker, `RunFrame` "V58-RESOLUTION", desktop untouched).
- V64.42 / V64.47: projection center. the Y offset was a UV shift and could not fix an angular mismatch; i added a tangent-space pitch correction in the blit shader (`colorAdjust.w`), `VrAutoProjectionCenter` (auto = -0.5*(up+down) of the eye fov) + `VrPitchCorrectionDeg` trim, PageUp/PageDown. today `VrAutoProjectionCenter` default false and `v6622_default_center_applied` derives the vertical optical center once from the live XR eye fov.

##### 2.2 AFR (alternate frame rendering) - DEAD END for production
- V41 FRAME-LOCKED-STEREO: eye identity was taken from D3D12 PRESENT transitions, unreliable because one game frame can trigger several PRESENT-like transitions. fix = frame-epoch eye identity via two read-only hooks. V45: "mode 2" CRenderView we had been staring at since the start was the SHADOW pass (EViewType resolved), and the "Recursive View" was water reflection render-to-texture, not stereo. `BlitAfrStereoToEyes` never consulted `swap_eyes_` (then the fix was a double-swap and got reverted, swap already existed at submission).
- V65.33: AFR per-eye history (each cached eye described with the XR frame that rendered it, `afr_per_eye_historical_pose_`). geometric stereo yes, but left/right from different sim times - moving objects tear. REFramework is also plain AFR (`m_frame_count % 2`), so the field leader has the same problem.
- V61.39 retired AFR/DUAL from the INI: `vr_stereo_experiment_.store(0) // stale AFR/DUAL INI values ignored`, `VrAfrIpdScale` forced 0. `kStereoAfrRect` is declared and never drawn. `BlitAfrStereoToEyes` and `EnsureAfrCaches` still exist in xr_manager.cpp as dead code.

##### 2.3 pseudostereo - DEAD END
V64.22C depth-conditioned eye-FOV asymmetry (near 0.70..0.55, far 1.00), V64.28 requested `r_DepthOfField=2` for it. my verdict: "pseudo stereo mode have no parallax". the depth-push shader path is still in the blit (`VrDepthStereoStrength` default 0.30, `DepthPushMod`, weight==0 = no-op) but the menu selector is gone since V66.69 (VIEW MODES = MONO / STEREO only).

##### 2.4 head jitter, late pose, "duplicate frames", pacing (V48-V65.10)
this one ate weeks. the complaint: mouse look smooth as butter, head look choppy / double image.
1. V48/V50: sampling the pose "now" at the camera made jitter WORSE. V53 ladder tested rotation order and predictedDisplayTime timing (`V53-ANGULAR-WORST`).
2. 8 ms exponential rotation filter (mode 5) hardcoded on the raw pose, adaptive mode 7 existed but was never wired -> made a menu toggle. `SetLatePoseEnabled` (resample right before the frustum hook consumes it) was built, hardcoded off, then enabled.
3. V64.39 FRESH PUBLISH: `PublishViewCamera` reused the culling pose if <=24 ms old; at 90 Hz that is >2 display frames stale. F3=1 samples at the real publish boundary (`CameraHook::GetFreshPublishPose`, still the production path). V64.40 F3 ladder: early-persistent vs late-pose variants.
4. V64.48 XR-GATED RENDER (the "DUPFRAME-CHECK-LATEPOSE" package): PublishViewCamera waits for a NEW xr frame token like crysis_vrmod (`WaitForNextRenderFrameToken`, 3 ms burst guard so auxiliary cameras don't eat tokens). verdict on sep 13: "ran near headset cadence but did not materially cure the problem" - DO NOT revive as the jitter cure. the token wait code is still there (`xr_render_gate_mode_` default 0) and came back later as D67 fresh pose (section 6).
5. V64.60-63 mod-owned frame pacer (refresh from consecutive predictedDisplayTime, 72/80/90/120 exact, sleep + spin). my verdict: "ok, external lock fps was never working. I am retiring it. We need to instruct players to lock the framerate externally" -> NVIDIA + AMD (FRTC / Chill) readme instructions. the key finding: head motion is nearly perfect when KCD2 is externally capped AT the headset cadence, going above 90 on a 90 Hz headset was worse. `VrFpsCapEnabled` / `VrFpsCap` (60..120) write only `sys_MaxFPS` (V65.03), default off.
6. V65.03/V65.04 F2 "render ownership": OFF = fixed 8 ms filter + "adaptive35" PRESENT pickup; ON = a fresh renderer-matched pose (the published-camera ring matched against the real CRenderView camera matrix) owns the PRESENT/OpenXR frame pairing. result "repeatedly stable: pub/render matching, zero misses" -> V65.10 always on, toggle removed. this is `FrameSync::LatestRendererMatchedPoseFrame` + `f2_head_probe_level=3` in mod.cpp.

DEAD ENDs here: quaternion/head smoothing filters as the fix, V64.48 gating as the fix, mod-side fps pacer, "sample now" without renderer matching.

#### 3. the native stereo hunt (10-24 Sep 2026)

what i wanted: two eyes from the SAME game frame, not AFR, and if possible using the engine's own leftover stereo code instead of rendering the whole game twice.

##### 3.1 static archaeology (V61.3) - clues, not a switch
a static search of WHGame.dll showed: `r_StereoStrength/EyeDist/ScreenDist/...` cvars are live (storage 0x50DBDxx), `CFlowNode_StereoParameters` / `ReadStereoParameters` flownodes exist and really write the cvars, `g_stereoFrameworkEnable` (int, default 1), `Stereo.cfx` with StereoL t0 / StereoR t1. but `r_StereoMode`, `r_StereoDevice`, `r_StereoOutput` are ABSENT in retail. DEAD END: hunting a one-line stereo switch.

##### 3.2 rendering the frame twice (V64.1 - V65.55a) - DEAD END, documented in detail so nobody does it again
- V64.1 F8 ladder replaced the old inner `CStandardGraphicsPipeline::Execute` re-entry (crashy, DEAD END) with the crysis idea: duplicate the OUTER `ISystem::Render` with a `RenderBegin` reset between eyes. rungs 4-6 fail closed until the exact dll was audited.
- V65.37: single eye LEFT or RIGHT with live OpenXR IPD (0.06988 m). i saw shadows and geometry MOVE with the selected eye. so the camera write reaches real scene state. (this is the "shadows in one eye" proof, good news at the time)
- V65.38b/V65.39: same-frame LEFT `CSystem::Render` -> ordered LEFT D3D12 capture -> RIGHT `CSystem::Render` -> one Present. one-shot stable, continuous survived ~75 ms then CPU crash `WHGame!00427cbd` (CRenderView reset writes zero to `[rsi+0x8DB0]`, `WHGame+0x427C1C` is a high-frequency reset/reinit, not a destructor). Device Removed: No.
- V65.43a: replay `CD3D9Renderer::BeginFrame` (`+0x52EF84`) between eyes -> crash `WHGame!0254686f`. retired for good.
- V65.44 SAFE-RIGHT: right render with `r_ShadowMapsUpdate`, `r_ssdo`, `r_SSReflections`, `r_MotionBlur`, cloud reprojection, `r_Reflections`, `r_WaterReflections` off. V65.53 immediate second Render -> 00427cbd again. V65.54a: zero-IPD with the SYSTEM RenderBegin wrapper (`+0x52EA10`) between eyes survived; V65.54b with real IPD crashed 0254686f. V65.55a reproduced the exact crysis order (set LEFT, Render, restore, system RenderBegin, set RIGHT, Render, restore) - every step returned OK and the NEXT normal frame died at 00427cbd.
- conclusion: a second top-level `CSystem::Render` publication is incompatible with KCD2's asynchronous single-owner CRenderView lifecycle. the fps "~60 after startup" during these tests was `sys_MaxFPS`/vsync confounding, V65.44 added a once-per-second watchdog.
DEAD END list from that era: two CSystem::Render in any order, naked BeginFrame, manual CRenderView alloc/AddRef/clone, sleeps as certification, `%TEMP_EYES` (eyeball shader, not stereo), `_RT_SECONDARY_VIEW` = generic aux view, `PostStereo` = screen-space pseudo.

##### 3.3 COUNT2: KCD2 already has an N-view loop (V65.56 - V65.87)
static find: camera build `WHGame+0x865550` inits `[CRenderView+0xA330]=1` and loops `+0x866F6C` over built-view records (`+0x9B30`, stride 0x400) selecting source bank `(selector+i)&1` from banks `+0x8EE8` (CURRENT) / `+0x9508` (PREVIOUS, stride 0x310). `CStandardGraphicsPipeline` slot 5 `+0x57278C` forwards count to `+0x653940` which transforms N x 0x400 into N x 0x470 and submits once. V65.56 set the count to 2 inside the loop: `pairCalls=2 records=2 nativeSecondIteration=1 oneCSystemRender=1 onePipelineExecute=1`, no crash, native mode2 / `_RT_SECONDARY_VIEW` shader lane lit up. but ONE scene path still ran and the final target (1920x1800 Texture2D, 1 slice) stayed LEFT-like. V65.85a sweep found 4 full-size candidates (3x fmt27 MRT slots + fmt34 UAV). V65.87 froze MRT0/MRT1 from the same OMSetRenderTargets into the two eyes: i saw "one strange purple raw-looking image, the other flat texture-only, missing lighting" - those are G-buffer channels, not eyes. DEAD END: COUNT2 as the output solution. KEEP: COUNT2 as the oracle for how native prepares 2 views in one frame (ShadowMask is count-aware, etc).

##### 3.4 selector=1: the eye pipeline that works (V66.01 - V66.19R1)
- V66.02/03 preflight: the RIGHT velocity texture `$VelocityObjects_R` existed as an object with no backing -> provisioned with the game's own helper `WHGame+0x73CEF4` to 1920x1800 (`native_stereo_allocation_step_v6603.inc`, one attempt per process - this is why a render-size change needs a restart, see section 7).
- V66.04-V66.08: scoped selector write (`CRenderView+0x8EE0`=1), camera banks shifted +-IPD/2 in BOTH banks (current and previous, `MakeStereoDesign` in `stereo_eye_control_model_v6604.h`, eye tag at camera +0x20C), native camera builder `+0x865550` called once more, Standard Update/Execute `+0x57278C`/`+0x57362C`, GBuffer, opaque all return. 30-frame burst then hundreds of continuous RIGHT passes without killing the desktop game. F11 = CPU witness tone.
- V66.08: copied the backbuffer at CPU `PIPELINE-RETURN` after L and after R -> both eyes BLACK while desktop was fine. the resource was right, the BOUNDARY was wrong (GPU work not ordered yet). V66.09 proved final PRESENT snapshot -> both eyes is non-black. V66.10/11: `CRenderView+0x8D78 -> CRenderOutput -> CTexture+0x40` = `$swapchainbackbuffer 0:current`, sameBacking=1 for L and R: ONE engine output reused by both eyes. `+0x2543A6C` (final compose helper) did not fire in the normal route (came back in D82).
- V66.12 COMMAND-LIST ORDER PROBE: tagged command list generations per eye. result: separate DIRECT-queue batches, last LEFT main writer -> first RIGHT main writer -> PRESENT (case A, the best case).
- V66.15 FIRST STEREO PRESERVATION: copy LEFT in `DetourExecuteCommandLists` BEFORE submitting the first RIGHT main-writer batch (`V6615IncomingLockedRightMainWriter`, `BridgeCaptureCurrentMainTargetToLeft`, "queueOrder=BETWEEN-BATCHES"), copy RIGHT after its batch (`BridgeCaptureCurrentMainTargetToRight`). FIRST VISIBLE REAL STEREO in the headset.
- V66.19R1 MERGED STEREO STABILITY: allocator ring for the copies, non-blocking (skip a pair rather than stall the render thread), XR consumption gates publication, auto-arm in gameplay, after a flat menu wait for a fresh pair. log rows `V66.19R1-LIVE-COPY eye=L ... result=QUEUED` / `V66.19R1-LIVE-PUBLISH ... pairPublished=1 XR=NORMAL-LR-LIVE`.

the frozen architecture (do not move the copy boundary without new evidence):
```
game frame N (one CSystem::Render, one CRenderView)
  camera banks CURRENT/PREVIOUS shifted -IPD/2, eye tag 0  -> native build -> LEFT pipeline (normal)
  [ExecuteCommandLists: first RIGHT main-writer batch incoming -> CopyResource(main -> LeftCache)]
  selector=1, banks shifted +IPD/2, eye tag 1 -> native build -> RIGHT pipeline (extra, same frame)
  [after RIGHT main-writer batch -> CopyResource(main -> RightCache)]
  restore banks/selector/built count, publish pair serial
  PRESENT (desktop shows RIGHT-ish / whatever is last)
XR worker: TryGetBridgeCompletePair -> BlitBridgePairStableToEyes -> xrEndFrame projection layer
```
IPD comes from the OpenXR views (`ActiveEyeSeparation`, ~0.0699 m on Quest 3), `MakeStereoDesign` refuses ipd outside 0.03..0.10 except exactly 0 (zero-IPD diagnostic). the pair is one game frame = one pair (pair 16080 -> frame 17041, +360/+360), so this is NOT "120 game fps = 60 VR fps".

#### 4. making stereo look right (24-28 Sep, V66.22 - V66.80)

V66.22 was the first checkpoint where i actually played in stereo: true stereo yes, but TAA/SMAA smear, volumetrics missing in the RIGHT eye, and when a menu opened stereo sometimes died and stayed "mono" (= LEFT duplicated). careful with the word mono: OLD GOOD MONO = center camera, one render, stable shadows, smooth head. "mono" after a failed stereo = LEFT eye in both, a different thing, don't use it as the control.

##### 4.1 the "VR-safe post-fx policy" - DEAD END that made things worse
V66.23/24 forced SMAA1X, motion blur off, `r_VolumetricCloudsTemporalReprojection=0`, fog reprojection blend 0. that is the window where the unbearable blown-out LEFT sky and strong shadow flicker became explicit. V66.29 disabled eye adaptation globally -> sky still blown (auto exposure is natively primary-eye-only anyway, `CAutoExposureStage` selector rule). V66.50 stopped writing the three cloud/fog cvars at startup; the V66.38 policy had been writing the captured SAME values back every ~2 s and even same-value writes invalidate shader/history state. V66.79a "skyfix" = the mod never writes `r_VolumetricCloudsTemporalReprojection`, `r_VolumetricCloudsStereoReprojection`, `r_VolumetricFogReprojectionBlendFactor` anywhere (reads ok). that is an absolute invariant now, a source validator should fail on it. also: don't force the player's AA/upscaler on startup or on mono/stereo switch; stereo changes only the temporal WIRING (pair-epoch).

##### 4.2 "shadow flicker" was four different bugs
i described it as: dark interiors, stairs, shiny surfaces, water streams, spider-web geometry flickers, RIGHT eye worse up close, equal beyond ~50 cm, visible depth disparity, independent of mouse vs HMD movement. i tried every shadow/AO/light cvar ladder reddit has, nothing. zero-IPD (SHIFT+F8) removed it in both eyes. i split it:

1. SVO voxel GI (the dominant one). V66.53 F8 rung 1 set `e_svoTI_Apply=0` live -> flicker GONE (image flat without GI but the artifact is fixed). `e_svoTI_Active` does NOT switch the stage off, earlier "SVO off" tests were invalid. static: SVO sites A `0x180579D64` (runs once per frame, LEFT only, `A={L:8314 R:0}`) and B `0x18057B764` (both passes) read the eye-0 camera block at `CRenderView+0x8EE8` hard-wired instead of `+0x91F8` for eye 1, and the reprojection block is guarded by "already done this frame" so RIGHT reused LEFT's result. `CSvoGiStage` vtable `0x3C26198`, Execute `0xFF33EC`, SVO object 0x2CF8 bytes at `stage+0x16B0`, `SVO_ViewProjPrev` from `stage+0xA0`. FIX = `native_svo_right_v6654.cpp`: RIGHT-only set of the SVO_*/SV1_* history textures (made with the game's helper `0x180578644`, 30 slots), swapped in for the RIGHT pass, own reprojection, per-eye previous view-proj (needs V66.49 per-eye previous camera). default ON. no extra rendering.
2. GSM sun-shadow cascade square: a small square around Henry has close shadows, outside it none, visible because the VR view is so wide. V66.60 F3 changed `e_GsmSizesWH`; my captured original was `2,8,32,200` not the assumed 5-value string. VR1 `4,12,40,120,600` hides the square with less detail loss, VR2 `6,20,50,150,600` also ok, bigger rungs bring it back. today: STEREO = VR2 automatic, MONO = original (V66.69). `r_ShadowsScreenSpace` was 1 all along.
3. AA/TAA instability (4.4).
4. water reflection temporal ownership (4.5).

DEAD ENDs: RIGHT regenerating shadow maps (static gate `WHGame+0xF7516C` reads `+0x8EE0` and returns false for selector 1 - retail WANTS primary-only shadow maps, the 2048x2048 LEFT-write/RIGHT-read atlas is by design), V66.25 RIGHT shadow/particle freeze (no change, accidentally revived in V66.29), center-camera ShadowMap prep (valid negative), `+0x7C2D40` RIGHT-prep replay, V66.27 stale XR ring cache (worse head jitter), V66.44/45 stage-skip bisect (CRASHES, never skip stages), V66.47 history-leak census (observer flaw: didn't count DSV binds as writes, V66.48 fixed it and the suspicious R24G8 "leak" vanished), V66.49 PREVIOUS-bank centering (looked mono, smeared head motion), old F4 pose ladder (all rungs used the same pose token, not a timing test).

##### 4.3 clouds missing in the RIGHT eye
root cause: retail cloud resize `0x1809B94E8` passes `allowCreate=(eye==0)` to `0x181E142E0`, so the `$VolCloudScaledMax0_R/Max1_R/Min0_R/Min1_R/$VolCloudPrevDepthScaled_R` objects exist but are never backed. V66.41 backed them with the retail allocator -> direct RIGHT clouds appeared. V66.41 ran at 4 fps - that was hot `VirtualQuery` diagnostics in the path, not the fix; V66.42 removed it (~0.05 ms, 100+ pairs/s). `r_VolumetricCloudsStereoReprojection` is inert in KCD2 (params+0x32 hard-coded 0 at `0x1809BA22D`). F4 was a RIGHT-cloud A/B and several test runs accidentally left it OFF ("the fix stopped working") -> V66.63 F4 = ON-LOCKED. code: `native_cloud_right_v6641.cpp`.

##### 4.4 TAA
- V66.32/33: `CPostAAStage` (vtable `0x3C3FE80`) Resize `0xC1AF80` creates two LEFT ping-pong histories (`stage+0x40/+0x48`) and leaves the RIGHT slots `+0x50/+0x58` empty. hook lets Resize run then provisions `$PrevBackBuffer0_R/1_R` into the dormant slots (`native_postaa_repair_v6632.cpp`). necessary, not sufficient - TAA still looked awful: extremely soft, shimmery.
- V66.57/58 probe: the retail history getter `WHGame+0xFFDCAC` picks history index from eye + parity of a GLOBAL render-pass counter. two passes per game frame -> LEFT always sees even, RIGHT always odd -> each eye reads its own history from TWO frames ago / ping-pong never flips the way TAA expects.
- V66.61F2 replaced the tiny getter with a normal C++ detour -> F2 crashed the game at once. proven: the optimized native caller keeps `CRenderView*` in RDX across two consecutive getter calls (the leaf getter doesn't touch RDX), the detour clobbered it, second call read `[0+0x8EE0]`. V66.62 = a register-preserving native stub, F2 flips one data byte: OFF = retail global parity, ON = CRenderView pair-frame parity (`native_taa_pair_epoch_v6661.cpp`). since V66.68 STEREO = pair-epoch ON automatically, MONO = off. honest status: the V66.68 test guide says "confirm TAA becomes stable pair-epoch", i never wrote a clean A/B result down, and in Oct my setup ran `r_AntialiasingMode=0` / upscalers for a while. TAA softness at 1920x1800 vs the Quest's 2496x2688 recommended size is a separate thing (section 6).

##### 4.5 water reflections
`$WaterVolumeRefl` / `$WaterVolumeReflPrev` are two global textures at stage slots `+0x288/+0x290`, alternated by game-frame parity in `CWaterStage::Execute` (`WHGame+0x7BBE70`), no `_R` variants. same frame -> both eyes pick the same A/B -> RIGHT overwrites, LEFT reads wrong-eye history next frame (reflected clouds/sky asymmetric). V66.63: private `$WaterVolumeRefl_R/_Prev_R`, swap the two pointers in during the RIGHT water stage only, execute once, restore (`native_water_reflection_right_v6663.cpp`). no extra render. only water, not other polished materials.

##### 4.6 head follow in stereo (V66.46)
the F2 renderer-pose matcher required <=4 mm camera match, but in stereo the eye cameras are +-IPD/2 off -> never matched -> every pair fell back to the newest pose -> jitter. V66.46 ignores the offset along the camera right axis and removed the unjustified "`(latest-matched)<=2`" rule in the LEFT pose capture (`device_hook.cpp` comment "V66.46: a fresh (<50 ms) renderer match IS the rendered pose, however far the renderer lags"). result ~13,800 matched pairs, 4 fallbacks, 99.97% - i said head follow became nearly perfect. the stricter V66.46R1 variant was kept only as a milestone.

##### 4.7 HUD in stereo
the eye copies happen BEFORE KCD2's late Flash/UI composition, so the HUD vanished in stereo.
1. V66.63: hold the pair PENDING, replay only the proven late UI draws (DrawIndexed indexCount=30 / DrawInstanced vertexCount=60) into both caches, publish after PRESENT submit. worked but fragile (V66.83 widened it to 12/18/24/30/219/576, fullscreen <=6 draws excluded = bands/fades).
2. V66.64/V66.65: redirect ALL late UI draws into a private texture and show it as one head-locked OpenXR quad. i confirmed it working in V66.65.
3. V66.78 found the redirect had killed Inventory/Shop in Theater since V66.64: in Theater the stereo producer keeps running, the late window still opens, and those late draws ARE the menu - it never reached the backbuffer ("image looked frozen, nothing reacted to the laser"). fix: redirect only in immersive STEREO, and the HUD was composited in the eye blit shader at the exact source uv (t2 = HUD, root constants 30->32).
4. V66.80: back to ONE OpenXR quad (no double vision, no per-eye replay), the blit slot 2 is kept bound but `g_v6678_hud_active_for_blit=false` always. today: `subsystems/hud_ui_controls/runtime/hud_layer_v6664.cpp` (late window capture, 2 caches, published to the XR thread) + `hud_quad` in `RunFrame` at `hudDistance=1.55f`, VIEW space, size = eye fov, optional rotation lag (`hud_lag_q_`), fader/vignette pixels cover both frusta. R105 moved it to 3 m and i put it back to 1.55 in R106 (i never asked for the "minimal HUD" package).

##### 4.8 menu / defaults
V66.68 GOLDEN RECOVERY: only MONO and STEREO exist. INI `VrStereoExperiment=3` means STEREO (`vr_production_stereo_mode_`, default 3 because i want the game to start in STEREO not Mono). STEREO switches on: stage4 L/R, pair-epoch TAA, RIGHT cloud/SVO/water histories, GSM VR2, HUD layer. the purple startup square (debug placeholder) was retired for black.

#### 5. presentation: projection layer vs theater quad vs cylinder

- gameplay = `XrCompositionLayerProjection` in LOCAL space, 2 views (V58.7 - before that F6 tried to counter-warp the projection, DEAD END).
- F6 "Theater" / flat UI (menus, dialogue native cameras, frontend video) = `flat_game_quad` LOCAL-space world quad showing the game backbuffer. `XR_KHR_composition_layer_cylinder` is used for the curved theater (`flat_ui_curved_`, shallow radius, centralAngle clamp 0.28..0.70) and the physical map proof (V66.69, more curved 0.34..0.58). V61.13: interactive menus must be a FLAT quad, the pointer UV and the injected UI hit-test UV must share one coordinate system (`use_menu_cylinder=false` hardcoded).
- V66.118R1: one visual owner at a time. projection-under-Theater gave a second wrongly head-related background image in the headset. Theater REPLACES the projection layer, gameplay restores it (see the `if (!flat_game_requested)` block before xrEndFrame).
- V66.33/34 theater drain: while theater owns XR the stereo producer keeps running and its pairs are consumed without presenting (`DrainBridgePairForTheater`), so the producer cannot wedge on `published != consumed`, and leaving theater goes straight back to live L/R (no more MONO-WAIT-FRESH re-arm).
- layer list today (max 13, capped by runtime `maxLayers`): projection OR flat game, then flat laser, flat cursor, dice marker, test laser, hand laser, HUD, menu, menu laser. `VrFlatScreenScale` / F6 grip+stick calibration sets theater size.

#### 6. pacing, latency, performance (Oct - Nov 2026)

facts first: Quest 3 at 90 Hz. source 1920x1800 per eye (`r_Width/r_Height`), runtime recommends 2496x2688 (1.0x) / 2270x2429 at Oculus 1.1x - the Oculus setting changes the swapchain, NOT the game render size, so everything is upscaled twice and soft unless `VrSourceWidth/Height` match (restart). C71 era ran ~110 fps; D63R1 recordings: native 74.6-111.9/s (median 98), XR 90/s, image reuse 0-4%, `XR_WAIT` 8.5 ms median is runtime wait not IK cost.

1. V66.2x "previous-pair-not-yet-consumed": `if published!=0 && consumed!=published -> skip producing` couples the renderer to XR consumption = repeated frames / judder. still conceptually there (`MarkBridgePairConsumed`, `ReleaseBridgePairClaim`). the proposed proper fix (multiple fence-owned completed-pair slots, XR takes newest, pose token travels with the pair) was never built. open.
2. V66.36 cadence owner / V66.38 pair pose mode 3 (per-eye pose frames) / V47 present pose pairing: the projection layer uses the pose that rendered the pixels (`CopyRenderFrameStateByIndex(camera_frame)`), per-eye frames when `V6638PairPoseMode()==3`.
3. R59.16C6 (13 Oct): the render-thread pickup compared a packed pair token `(pairFrame<<32)^executeSerial` against the DESKTOP present counter (`latestDesktopPresent > last_submitted_present_serial_`) - different domains, so new certified pairs looked "not ready" and the XR loop plateaued at ~64/s while stereo made 100+ pairs/s. fix = `core/render_pacing59c6.h` `Decide()`: native modes 2/6/7 use pair identity (inequality), desktop/theater uses the present counter, grace 2-9 ms on a steady clock instead of GetTickCount64 (which rounded to 0 or 16 ms).
4. F2 rung 4 / I10 (26 Oct): wait at `PublishViewCamera` for a NEW XR token, <=20 ms (`WaitForNextRenderFrameToken(token,20,true,false)`), adaptive hand damping OFF, HJ1 OFF. i said: visibly smoother, except fast motion. avg wait 3.97-5.27 ms, max 15 ms, pairs 88.6/s, reuse 0.85-1.47% (baseline 0.10%). the log showed it did NOT reduce repeated frames, the improvement is scheduling/phase. this became D67 "FRESH POSE timing" = `VrMotionTrialD67=2`, the default.
5. D71 (31 Oct): all observed timeout chains had a newer unconsumed XR token at gate entry while the producer was already waiting 27-88 ms for native render pickup; the gate then spent ~20 ms more and 5 s watchdog suspensions followed. fresh images carry 1-3 token old camera identity (2-3 tokens = 22-33 ms). not tracking loss.
6. D85R5R3 (3 Nov): "Fresh Pose Uncapped" `VrMotionTrialD67=4` = no native-frame token wait at all + extra pose refresh before the native update (`motion_trials_d67::UncappedSelectedAt`). menu Home > VRIK EXPERIMENTS. recorded token waits in mode 2: median 5.97 ms, p95 7.4, max 10.5, 45 frames per half second = 90 cadence coupling. mode 4 removes that wait, it does not promise the old 110 fps back. untested by me at R106 as far as the docs say.
7. head pose source today: `VrPoseSourceMode` 0 = cached worker pose, 1 = `SampleHeadPoseAtTime(predictedDisplayTime)`, 2 = `SampleHeadPoseNow` (DEFAULT, "C26C1: preserve fresh sampling default"), 3 = predicted + `VrPosePredictionOffsetMs` (+-50). `CameraHook::ApplyCurrentPoseToPublishedCamera` does the sampling, with a 20 ms cached fallback (`tracking59c6::ReadHead`) and `V53-SYNC-POSE-FAIL` rows. `VrSynchronousHands=1` locates head + both grips at the same XrTime.
8. comfort vignette (C51): `VrComfortVignette` 0 off / 1 low / 2 medium / 3 strong, darkens edges while moving (left stick) or turning (right stick X), attack 0.12 s, release 0.35 s, off in theater/flat/menu. done in the present shader (`comfortVignette` constant, `present[31]`), `comfort_vignette51.h` `Envelope::Step`, CONTROLS page. default 0.
9. fps cap: `VrFpsCapEnabled=false`, `VrFpsCap=90` writes `sys_MaxFPS` only. external driver cap at headset Hz is still the recommendation.

#### 7. upscalers: DLSS / FSR / XeSS per eye (D81 - D85, R88)

- D81R2: STEREO refused every temporal upscaler (Native only) because one DLSS/FSR instance shared history between the two sequential eyes = garbage.
- D82 (2 Nov): separate vendor instance per eye. DLSS, static search: WHGame exports `NVSDK_NGX_D3D12_CreateFeature/EvaluateFeature_C/ReleaseFeature`; the game's evaluate helper tail-calls `EvaluateFeature_C` so hooking only `EvaluateFeature` misses it; RIGHT feature made from the same live parameter dictionary. FSR: public ffx loader API (native create helper `0x1DFEC30`, dispatch `0x1DFDFF0`, descriptor 0x1B0 bytes, jitter +0x168, reset +0x198); native velocity banks `pipeline+0x858/+0x860` are already per eye. histories reset together (a RIGHT-only reset refuses the pair), receipts per (epoch, frame). RIGHT publication needs the FINAL COMPOSE certificate: helper `+0x2543A6C` called from `+0x864654`, a real draw inside that scope with the main RTV bound, same command-list recording generation submitted (`Reset` = COM slot 10 invalidates, `ClearState` = slot 11), both SDK receipts for that epoch/frame. files: `native_dlss_stereo_d82.cpp/_runtime.inl`, `native_fsr_stereo_d82.cpp` + `fsr_stereo_runtime_d82.h`, `native_upscaler_stereo_d82.cpp` (aggregate owner), `temporal_compose_publication_d82.h`. Graphics page saves Native/DLSS/FSR for the next restart; a saved STEREO request stays saved even without twins so the next launch can capture creation (the first draft discarded it = restart loop).
- D83R1: a 24-file upscaler overlay was NOT merged - a fixture compiled it and showed RIGHT falling back to the LEFT context after a failed twin, asymmetric resets `1,0`, no frame continuity, and the header broke the D82 callers. fair review, keep the D82 bridge.
- D85: XeSS as a third backend (libxess IAT `0x183A052C0`, one call site each, execute wrapper `0x181DFFC40`), live switching (worker stops stereo, drains, writes quality then mode, holds autostart until a pair is captured), `temporal_pair_watchdog_d85.h` because with DLSS the bootstrap census locked the pair frame to the first RIGHT frame and nothing ever published -> black flickering stereo; the watchdog falls back to the eye-boundary copy. `upscaler_live_switch_d85.h`.
- R88 (3 Nov), my D85R5 logs, claude found the problems in them: (1) "SWITCHED LIVE" but the headset shows flat 45 fps: the RIGHT eye targets are created ONCE per process at that moment's render size (`one-attempt-per-process`, `right-already-backed noResize=1`), the game resizes only eye 0 on quality change, admission then refuses `velocity-size` (`stereo_eye_control_model_v6604.h:188`) every 2 s with a stale flash. fix = the switch watches the first stereo start, on size refusal shows `RENDER SIZE CHANGED: RESTART THE GAME` (`RenderSizeStaleR88`, `ObserveStereoStartR88`). real fix (re-provision eye1 targets at a GPU-idle boundary) not done. (2) one `right-reset-differs-from-left code=14` latched a global fault -> mono for the rest of the session; now it refuses that pair only, like FSR. (3) ~2 s start flicker: XR mode 2 re-presented the previous run's last pair (`s_bridge_complete_pair_serial` only cleared at shutdown) and the watchdog waited 1 s/45 attempts; now the serial is cleared at autostart arm (`SetV6610OutputCensus(true)`) and the watchdog latches after 100 ms / 8 attempts. (4) "unsharp DLSS": Quality at 1920x1800 renders 1280x1200 per eye then stretched to 2270x2430 = two upscales. choose source size + upscaler + quality BEFORE launch, don't change quality live.
- the `D82 DLSS_STEREO status` row with reset counts per eye is never written, so occasional native resets can't be excluded from the logs.

#### 8. known open issues (rendering)

- pair producer/consumer: single published-pair slot, "not-yet-consumed" skip; multi-slot fence-owned design never built.
- RIGHT eye targets fixed at first-creation size -> upscaler quality / render size change needs a restart.
- the temporal (DLSS/FSR) compose certificate never formed in my logs (`temporalPairs=0`), every DLSS pair came through the eye-boundary fallback; GPU content of that copy unverified.
- TAA pair-epoch: no clean written A/B from me; reflected clouds on non-water materials; SSR shared backing (V66.39R1) exact writer open.
- intermittent upside-down start (R105 adds `R105 XR_HEAD` first-pose rows, no guessed axis flip).
- SteamVR: R104/R106 layer guard untested in the headset at the time of writing.
- HUD plane distance / size is a comfort question (1.55 m now).

#### 9. OpenXR runtime differences: VDXR vs SteamVR (R102 - R106, 5 Nov)

everything up to R101 was tested only on VDXR (Virtual Desktop). on SteamVR/OpenXR 2.18.4: no controls, no laser, no HUD, menu only visible "on the slowdown".
- R102: `xrSuggestInteractionProfileBindings` is all-or-nothing per profile - one path SteamVR doesn't know (Touch thumbrest/touch etc) and the WHOLE oculus/touch profile is refused. now a core set first, optional paths separately, fallbacks (`khr/simple_controller`), and `R102 XR_INPUT` / `R102 XR_RUNTIME ... maxLayers= maxSwapchain=` rows always in KCD2VR.log (`subsystems/hud_ui_controls/runtime/xr_input.cpp`).
- R104: SteamVR validates every layer in `xrEndFrame` (unit quaternion, finite, size>0, rect inside the swapchain); one bad layer = the whole frame refused and the compositor keeps the last accepted frame - that was the "no HUD, slowdown". VDXR just draws it. `xr_layer_guard_r104.h`: renormalize quaternions (double precision), clamp rects, zero size -> 0.5 mm, drop only unrepairable; HUD swapchain rebuilt after a failed D3D12/OpenXR call instead of dying for the session (`x_failed` latch gone), image wait 1 ms not 0. R105 removed my second xrEndFrame (right, not in the spec) AND the sit-out (wrong) -> R106: one xrEndFrame per frame; on a LAYER error code (-1, -39, -42, -43, -44) the overlays of that frame sit out 1 s and come back one by one 300 ms apart, a kind that makes a frame fail again sits out 10/20/40/60 s, never off for the session (`layer_guard_r104_.admit`, `NoteEndFrameR105`). rows `R106 XR_LAYERS xrEndFrame refused code= layers= blamed=`.
- R105 (kept in R106): session lifecycle - STOPPING (dashboard, headset off) ends the session and READY begins it again with the same action set, events polled while stopped, recenter + UI anchors recaptured after a reference-space change (`PollSessionEventsR105`, `pending_reference_changes125_`); startup waits for READY instead of "500 ms or VR off forever"; R106 adds `Sleep(8)` per loop while stopped (the worker was spinning one core at 100% with the dashboard up).

#### 10. WHAT WORKS NOW - file map (R106)

| what | where |
|---|---|
| OpenXR instance/session/swapchains, blit shader, layers, xrEndFrame | `src/xr_manager.cpp`: `XrManager::CreateInstance/CreateSession/CreateSwapchains`, `CreateBlitPipeline` (PresentParams cbuffer: uvScale, xrTan, sourceTan, sharpen, poseDeltaQ, colorAdjust, hudEnable, comfortVignette), `RunFrame` (xrWaitFrame -> pose sample -> `QueueRenderThreadDiagnosticWork` -> `proj_views` -> layer guard -> xrEndFrame), `BlitBridgePairStableToEyes` (stereo pair), `BlitGameImageToEyes` (mono) |
| D3D12 capture, queue detour, LEFT/RIGHT copies, temporal certificate | `src/device_hook.cpp`: `DetourD3D12CreateDevice`, `DetourCreateCommandQueue`, `DetourExecuteCommandLists` (LEFT copy before first RIGHT main writer, `V66.19R1-LIVE-COPY`), `BridgeCaptureCurrentMainTargetToLeft/Right`, `TryGetBridgeCompletePair`, `MarkBridgePairConsumed`, `DetourResourceBarrier` (PRESENT tagging), `DetourResetCommandListD82/ClearState` |
| native eye pipeline (stage4) | `src/native_stereo_eye_control_v6604.inc` (`BuilderDetour`, `V66.12-DESIGN SELECT` / `V66.12-EYE SELECT` / `EYE BLOCKED reason=`), `src/stereo_eye_control_model_v6604.h` (`MakeStereoDesign`, `ShiftCamera`, admission `velocity-size`, `native-phase-or-mode`), `src/native_stereo_preflight_v6602.cpp` (`V66.12-PREFLIGHT`), `src/native_stereo_allocation_step_v6603.inc` (`$VelocityObjects_R`), `src/native_stereo_outer_lab.cpp` (single-run state machine, `PollSingleRun`, `CSystemRenderDetour`, cloud/secondary census hooks), autostart in `mod.cpp` (`v6618_auto_stereo_enabled`, `V66.18-CONTINUOUS COMPLETE`, 750/2000 ms rearm) |
| per-eye repairs | `native_svo_right_v6654.cpp` (ON), `native_cloud_right_v6641.cpp` (ON-LOCKED), `native_postaa_repair_v6632.cpp` (RIGHT PostAA histories), `native_taa_pair_epoch_v6661.cpp` (stub, STEREO=ON), `native_water_reflection_right_v6663.cpp`, `native_svo_prev_per_eye_v6649.cpp` |
| camera: frustum hook, pose apply, pose source modes, fresh publish | `src/camera_hook.cpp`: `CameraHook::Install` (AOB ladder), `FrustumDetour`, `ApplyCurrentPoseToPublishedCamera`, `SetPoseSourceMode`, `SetFreshPublishPose`, `RequestRecenter` |
| publish boundary, renderer-matched pose, AFR eye (dead) | `src/frame_sync.cpp`: `FrameSync::Install` (`kPublishViewCameraRva`), `PublishViewCameraDetour` (token wait `V64.48-XR-GATE`, backup/restore of the camera matrix), `LatestRendererMatchedPoseFrame`, `GraphicsPipelineExecuteDetour` (probe) |
| render-thread pickup pacing | `src/core/render_pacing59c6.h` `Decide()`, used in `XrManager::OnRenderThreadTick` |
| HUD layer | `src/subsystems/hud_ui_controls/runtime/hud_layer_v6664.cpp`, `hud_quad` in `RunFrame` (1.55 m) |
| upscalers | `native_upscaler_stereo_d82.cpp` (owner), `native_dlss_stereo_d82*.`, `native_fsr_stereo_d82.cpp` + `fsr_stereo_runtime_d82.h`, `native_xess_stereo_d85.cpp` + `xess_stereo_runtime_d85.h`, `temporal_compose_publication_d82.h`, `temporal_pair_watchdog_d85.h`, `temporal_capture_transaction_d85.h`, `upscaler_live_switch_d85.h` |
| SteamVR layer guard / session | `src/xr_layer_guard_r104.h`, `XrManager::PollSessionEventsR105`, `NoteEndFrameR105` |
| vignette | `src/comfort_vignette51.h`, `present[31]` in the three blit paths |
| INI keys (Settings) | `VrStereoExperiment` (3=STEREO), `VrSwapEyes`, `VrRenderScale`, `VrSourceWidth/Height`, `VrSourceVerticalFovDegrees`, `VrGameFov`, `VrFov`, `VrAutoProjectionCenter`, `VrPitchCorrectionDeg`, `VrPoseSourceMode` (2), `VrPosePredictionOffsetMs`, `VrMotionTrialD67` (2 / 4), `VrSynchronousHands`, `VrFpsCapEnabled`, `VrFpsCap`, `VrComfortVignette`, `VrFlatScreenScale`, `VrDepthStereoStrength` (legacy), `VrAfrIpdScale` (retired, forced 0), `EnableNativeEyeWrite`, `EnableLegacyStereoProbes` (dev) |

#### 11. if you want to fix X, look here

- stereo silently off / 45 fps flat image: `V66.12-EYE BLOCKED reason=` in the log (needs F2 capture). `velocity-size` = render size changed since the RIGHT targets were created -> restart, or implement eye1 re-provisioning (`native_stereo_allocation_step_v6603.inc`, helper `0x73CEF4`, free old textures at a GPU-idle point). `native-phase-or-mode` = pass scheduler / defer on (`r_GraphicsPipelinePassScheduler` must stay 0; the admission reads `scheduleEnabled`/`deferSetting`/`dispatchMode` in `stereo_eye_control_model_v6604.h`).
- stale / repeated frames, judder: `render_pacing59c6::Decide` + `OnRenderThreadTick` pickup, the single pair slot in `device_hook.cpp` (`s_bridge_complete_pair_serial`), `VrMotionTrialD67` 2 vs 4, F2 `freshPerS / reusePct` rows.
- head feels floaty: `VrPoseSourceMode`, `FrameSync::PublishViewCameraDetour` (fresh publish), `LatestRendererMatchedPoseFrame`, `present_pose_pairing_mode_`. do NOT add smoothing filters again.
- one eye looks different (lighting, clouds, reflections): a shared temporal history. pattern = find the retail texture/stage slot, back a `_R` twin with the game's allocator, swap pointers around the RIGHT pass only. examples: `native_svo_right_v6654`, `native_cloud_right_v6641`, `native_water_reflection_right_v6663`, `native_postaa_repair_v6632`. never skip stages, never regenerate shadow maps for RIGHT.
- sky blown out: check nobody writes the three cloud/fog reprojection cvars (grep `VolumetricClouds`), check `ApplyV6638RetailPostFxPolicy` lineage.
- flicker on stairs / close geometry with depth: SVO. `e_svoTI_Apply=0` as the test, `native_svo_right_v6654` ON.
- shadow square around Henry: `e_GsmSizesWH` policy in mod.cpp (VR2 `6,20,50,150,600`).
- HUD missing / doubled / wrong distance: `hud_layer_v6664.cpp` (late window redirect only in immersive STEREO), `hud_quad` in `RunFrame`, `hudDistance`.
- menus frozen / laser does nothing in Theater: the HUD redirect must be off in Theater (V66.78 lesson), `flat_game_quad` must REPLACE the projection (V66.118R1).
- SteamVR shows no overlays: `R106 XR_LAYERS` rows say which kind and code; `xr_layer_guard_r104.h`.
- DLSS/FSR/XeSS: `D82 DLSS_STEREO` / `D85 TEMPORAL_PAIR` / `D85 UPSCALER_LIVE_SWITCH` rows, `temporal_compose_publication_d82.h` for the certificate, watchdog in `temporal_pair_watchdog_d85.h`.
- image soft: source res vs headset res (`VrSourceWidth/Height` restart), `r_Sharpening` is clamped to >=0.24 by the mod, two upscales when DLSS quality < native.
- want to go back to the research: F8/F9/F10/F11 ladders in `native_stereo_outer_lab.cpp` are gated by `EnableLegacyStereoProbes` + developer mode; the COUNT2 / MRT / BeginFrame paths are documented dead above, don't rerun them for fun.


---

## 3. VRIK, BODY, HANDS

this is the whole story of the body: how we found henry's bones, how the hands first moved, the FRIK style upper body, the elbows (the damn elbows), calibration, fingers, who owns the body when (native vs VRIK, "passengers"), tracking timing, the body frame and height modes, crouch, roomscale, the horse, and the latches. it is long because it was the longest fight in the project. if you only want to fix something, jump to the last part "if you want to fix X".

what VRIK means here: the game keeps animating henry (legs, pelvis, combat state, weapon attachment, all native). at the very last moment before skinning the mod overwrites the upper body bones (arms, hands, fingers, spine/head to a degree) with a pose solved from the HMD + two touch controllers, lets the game skin it, then puts the native pose back. the game never knows. that is the crysis vr / cyberpunk vr pattern and it was decided on 8 sep 2026, before we even had a visible hand.

#### where it lives today (R106)

- `src/frik_pose_probe.cpp` (6.5k lines): the native hooks, henry identity, the skinning transaction (apply -> original skinning -> restore), calibration glue, all the F-key ladders. it includes `src/vrik/coherent_pose.inl` inside its private namespace.
- `src/vrik/coherent_pose.inl` (2k lines): `BeginCoherent6689()` = the ONE production pose writer since V66.89: one native model-space pose + one XR snapshot -> a staged length preserving pose. stages are `src/subsystems/vrik/pose_*.inl` #included in order: reference_stage, tracking_goals, frame_foundation, follow_input, torso_stage, reach_input, arm_solver, anatomy, fingers, final_wrist_d52, hand_display_d62, leg_solver. `PublishCommitted6689()` publishes the committed frame for camera/weapons/contacts.
- `src/vrik/pose_math.h`: two bone solve, `continuous_elbow`, quaternion helpers. `src/vrik/upper_body.h` (V66.103 shoulder girdle), `src/vrik/elbow_r89.h`, `src/vrik/elbow_destination_d86.h`, `src/vrik/frik_elbow_lab.h`, `src/vrik/elbow_shoulder_d45.h`, `src/vrik/elbow_hysteresis_d69.h`.
- `src/vrik/tracked_frame51.h` + `pose_frame_foundation.inl`: the shared body frame (camera, head, hands all built from one anchor).
- `src/animation_filter.cpp` `InspectNativeBody112()` / `NativeBodyRequired112()`: decides per scan if the native body owns (returns `BodyDecision115`).
- `src/tracking_sample59c6.h` + `subsystems/vrik/tracking_producer.inl` / `tracking_reader.inl`: the head+grip pose families.
- `src/vrik/calibration_profile.h`, `subsystems/vrik/calibration_*.inl`: F8 T-pose calibration, saved in `[VRIK.*]` INI sections.
- `src/vrik/riding_vrik_d76.h` + `riding_aliases_r95.h`: horse. `src/vrik/body_follow_r38.h`, `roomscale58.h`, `native_crouch_r93.h`, `pose_restore_debt_r96.h`, `core/load_rearm_r96.h`.
- menu: Experiments > Body / Hands / Elbow preference; Home > VRIK EXPERIMENTS. INI keys listed per topic below.

#### 1. how we found the body (V49 .. V65.28, aug-sep 2026)

1. **V49 plan** (early sep): "first prove controller poses and game input. then experimental arm IK: native shoulder roots + analytic two-bone solve to OpenXR grip targets, keep native weapon attachment and WeaponSolver, native legs stay". that plan is basically what we have today.
2. **V61.9** read-only "FRIK recording": `V61.9-FRIK-SAMPLE` logged controller pose + an estimated shoulder/elbow/hand. no writes. the `VrikBridge::Update()` scaffold from that time is only an observer (vrik_bridge.cpp still exists).
3. **V61.23 .. V61.35 - the CPoseData pulse era, all DEAD ENDS**: V61.23 never armed; V61.23.1 used `WHGame+0x53D7C4` as a "full QuatT getter" - it is NOT, it returns `buffer0 + joint*0x1C + 0x10` (translation only). real getters/setters: buffer-0 `+0x53F550`/`+0x53F510`, buffer-1 `+0x53D830`/`+0x53F4D0`, raw selector `+0x53F580`. V61.25 wrote buffer 1 AFTER `COperatorQueue::Execute` - nothing moved (not the visible owner, don't write harder). V61.30 APPLY/RESTORE around the skinning consumer on the `WeaponSolver::SetIKTargets` joints (`solver+0xC/+0xE` = 417/452) - nothing moved: 417/452 are the weapon helper pair (ITEM_R child of HAND_R, ITEM_L), NOT hands. V61.35 wrote the 0x20-byte DQ output of `WHGame+0x5FE2F0` (final transform builder, reads pose buffer 1) - never proven.
4. **V62.6 / V62.9 (sep 9): the rig.** right arm UPPERARM_R 403 (parent 395), FOREARM_R 405, ELBOW_R 406 (a sibling HELPER, not the hinge!), HAND_R 408, lengths 0.266 / 0.262 m; left 438/440/443; 718 joints, HEAD 472, NECK 393. semantic table `owner+0x17C+2*BoneID` (pelvis 2, spine 3..6, neck 9, head 10, hand L/R 18/19, clavicle 32/33, upperarm 36/37, forearm 40/41, elbow 42/43). selector-1 PREBUILD setter `WHGame+0x53F4D0` (RCX CPoseData, EDX joint, R8 QuatT*), write right before `+0x5FE2F0`, restore after. `Cry local = {XR.x, -XR.z, XR.y}`.
5. **V63.3 F10 lab** (wrist rotation / right arm / both arms / upper body overlay / weapon-aware). V63.4-V63.6 tried `WeaponSolver::SetIKTargets` inputs - DEAD END ("did not track the controller or own the gameplay weapon endpoint", V63_8 notes).
6. **V64.9 the vtable crash lesson.** called `CDefaultSkeleton` slot 0 with "hand_r" hoping for GetJointIDByName: callOk=1, garbage, crash 3 s later in a sword swing. SEH can't save you from corruption. RULE ("V64.11 safety rule" in source): no blind vtable calls, disassemble first.
7. **V64.13**: chain `WeaponSolver -> C_Player@entitymodule@wh -> +0x38 -> CEntity` stable. F11 whole-rig recorder: joints 417/419/420 moved exactly 2.1692 m max in a 191 s combat recording = rigid attached body = the weapon slot.
8. **V64.49 (12 sep) - THE PROOF.** +25 cm on joint 408 inside the skinning-builder window: i saw the hand stretch visibly to the right. V64.51 live XR in the lab, V64.53 `CHAR-MATCH ... WRITE-ELIGIBLE` identity barrier (other characters `CHAR-MISS`, fail closed - keep it). the first two-bone attempt was "visible but warped": `ControllerHeadAnchoredArmGoal()` added `HeadPhysicalDeltaCry()` on top of an already head-relative target = HMD translation twice, and it translated helper 406 as part of the chain.
9. **V65.02-V65.05** read-only F9 ladders. V65.05 found the native IKTarget machinery statically (`C_ProceduralClipIKTargetContext` vtable `WHGame+0x3BD6180`, update `+0x145DCBC`, target writers `+0xA7F6D0/+0xA80150`, `RgtArm01`/`LftArm01` crc `0xFE062C68`/`0x314004C5`) but no live owner at runtime. DEAD END (for now): native IKTarget / RgtArm01 writes stayed LOCKED in every ladder; the ADBs tie RgtArm01 to authored interaction clips (doors), not combat hands.
10. **V65.09**: one corrected arm (403->405->408 + tracked wrist) at 5 injection lifetimes (late skinning / pre-SetIK / post-SetIK / both / hold past skinning) to see if `hand_right + slt_0` follows. also F5 rung 2 NPC LookAt hook `WHGame+0x59B0F0` (see §10).
11. **V65.16**: hooks installed read-only at startup so henry is learned BEFORE F10. V65.20-V65.24: ITEM_R=417 / ITEM_L=452 proven; "arm + ITEM_R subtree" rung - the sword still did not follow. DEAD END: ITEM_R alone is not the weapon owner (V65.28).
12. **V65.27 (sep 17): first GOOD arm.** `ApplyWholeRightArmOwnership` "visually good in V65.27". V65.28: rung 4 crysis-style analog grip fingers (five chains found from the HAND_R hierarchy, curled from the grip value), rung 5 clavicle + HMD-relative upper body/head (rotation only).
13. **V65.46-V65.57 (parallel VRIK line, 18-22 sep)**: both arms, both-hand fingers, exact item attach/event hooks (V65.47), cached arm topology (V65.48, the O(N^2) per-frame ancestry scan killed), WUID resolver `WHGame+0x47C1F8`. **V65.49**: `V65.49-VRIK-CAL CAPTURED PALMS-UP TPOSE` - F8 became the T-pose calibration; V65.54/57a guided 2 s capture solved from the mean of the samples. V65.74: the OPEN hand reference (900 ms with grip/trigger released, straighten the four long fingers, store as OPEN). V65.80 tried the native idle open pose - not trusted (V66.78 went back to V65.74).
    note: the V65.4x-V65.9x numbers exist twice - one line was the stereo work, the other the VRIK work (same numbers, other lineage).

#### 2. the coherent pose kernel (V66.89, 29 sep) and the V66.1xx body work

- **V66.71**: henry was only found after the first punch (SetIK callback); the "player prime" chain `CCryAction -> ActionGame+0x88 -> PlayerModule+0xA40 -> CEntity+0x38 -> GetCharacter` (`V66.71-VRIK PLAYER-PRIME step=N`) finds him without a punch. **V66.78**: finger roots R 412..416 / L 447..451 (thumb..pinky). **V66.88**: V66.86 free-look pitch compensation RETIRED after headset-proven oscillation (frik_pose_probe.cpp ~808).
- **V66.89 "COHERENT-BODY-WEAPON"** = the kernel. `pose_math.h` + `coherent_pose.inl` own the production solve, `frik_pose_probe.cpp` owns hooks/identity/calibration/transaction. rules from then, still true: one snapshot per transaction; FIXED arm lengths + neutral body-space bend reference (no live attack elbow, no per-joint measured lengths); native pelvis/lower body is the base; shoulder compensation before the hard reach clamp; axial roll from the real forearm chain (never the 406 helper as a twist bone); controller and HMD converted with the same calibration basis; validate -> stage -> commit for skinning -> restore; a hand is published only after successful consumption/restoration. 74 unused functions (live native elbow solver, mixed-pose body writer, old finger interpolation, generic weapon writes) went to `research_archive/retired_pose_implementations/` - not in CMake, don't port them back.
- **V66.95-99** `continuous_elbow`; **V66.102/103** `upper_body.h` joint-limited shoulder girdle (elevation 20, protraction 16, retraction 6 deg), 155 deg flexion policy for too-close reaches, torso clearance elbow search (see §6).
- **V66.105** open hand: since V66.97 the OPEN endpoint was the game's relaxed-idle curve (half closed) and the V66.100 captured pair in `[VRIK.Fingers.<rigHash>]` stopped loading (101R2 changed the hash recipe). fix: blend OPEN toward the CDefaultSkeleton bind pose (fingers 80 %, thumb 40 %) when the bind pose is more open (`pose_fingers.inl:17-30`).
- **V66.107** standing eye height: a recenter used to take the CURRENT headset height as henry's standing head (recenter sitting = everything shifted). F8 stores `[VRIK.Head] StandingEyeHeightM` (OpenXR STAGE), recenter corrects to "standing straight"; head max 5 cm above native.
- **V66.109** camera: "one final orientation authority: HMD" (camera_hook.cpp:1351, native gives POSITION+HORIZONTAL_YAW, pitch/roll stripped). **V66.122** `PERF POSE_SOLVE` / `CAMERA_BODY targetErrorM` rows (normal 1.3-2.6 cm).
- **V66.123**: head target = CURRENT native head (same anchor as the camera, calibration head only if >75 cm off) - fixed the camera-inside-the-neck; arms shoulder-relative: F8 supplies only arm geometry, your shoulders are estimated from the headset yaw each frame, controller offset from your shoulder scaled by HenryArm/userArm and applied at henry's shoulder (your T-pose reach == henry's); soft reach: above 88 % of arm length the target approaches 98.5 % asymptotically (~20 deg minimum bend, a straight arm has no elbow direction).
- **V66.128** 6DOF body follow (F1 / `VrikBodyFollow6DOF`): head beyond a 15 cm lean radius moves the pelvis (cap 60 cm), real crouch lowers it (90 %, max 45 % of leg length), hips take 25 % of chest yaw, two-bone leg IK with fixed thigh/shin, foot steps (180 ms, 5 cm lift) when the planted offset is >16 cm. natural chest twist: chest yaw = 45*tanh(0.55*headYaw/45) spread 12/35/68/100 % over spine..spine3, neck takes 55 % of the rest; FRIK-style hand bias toward the hand midpoint (`VrikChestNaturalTwist`, `VrikChestHandBias`). tested, called working -> ON by default.
- **V66.137 VRIK cost**: `g_compose_defer137` ON - mid-solve only the "essential" ancestor-closed bone set is composed, passenger descendants (face, armour, cloth, helpers) deferred, one `ComposeFull137` before invariants/commit. 720-joint fixture ~383 us / 5904 compositions -> ~165 us / 1754. row `V66.89-POSE COMMIT ... essential=N/M defer137`. frame-time pressure removed, no latency added.
- **V66.161** hand aim pitch: in VRIK the native aim pitch follows the interaction hand ray (yaw never injected).

#### 3. who owns the body: native vs VRIK, passengers, transfers, latches

the decision is `NativeBodyRequired112()` -> `InspectNativeBody112()` in animation_filter.cpp, scanning henry's live animation FIFO every frame against `combat_visual_aliases112.h` / `vr_combat_ownership111.h`. order today (animation_filter.cpp ~700-735): `NATIVE_PASSENGER_A34` > `RIDING_NATIVE_TRANSFER_R95` (mount/dismount) > `NATIVE_GETUP_R96` > `PROTECTED_ACTION` (clinch, paired, death, finishers D83 keep VR head+torso) > `RIDING_NORMAL_VRIK_D81` > `QUALIFIED_LIVE_POSE`. it is decided per scan, no stored state - that is the point.

- **"passenger"** = the native animation owns the body and VRIK rides along or yields. **A34 (23 oct)**: for six retail activities (washing, soup eat/poison...) the observer rejoins the real action/scope/fragment/token after henry's Update and publishes a witness with 80 ms expiry; `coherent_pose.inl` rejects `NATIVE_BODY_NO_CURRENT_RELOADING_PASSENGER` unless a reload working hand (XR34) or instant pair hands (D72) are current. A34 also closed two holes: the F9/T4 held-object driver could outlive a new passenger until skinning (consumers now compare the witness with their original character/body), and the controller entity id did not prove the passenger's body (the FullBody scope context character must match).
- the alchemy "semi-passenger" (C45/C46R1 `VrMinigameHands`, `passengerAlchemy63`): torso/head authored, only free arms tracked.
- **contact handoff 59c3** (`contact_handoff59c3.h`): gesture identity comes from the completed pose producer; a release ticket is matched by gesture/key/generation/epoch/request/recovery, contact age max 160 ms, `Join::{Wait,Bound,Expired,Identity,Recovery,ReleaseTooLate}`; "preparing B must not erase the contact/release of A". the seam to combat. C57: one qualified equipment/profile snapshot per transaction.
- **latches - my rule (R96): no permanent blockers.** before R96 a native SEH fault switched a feature off until restart. `core/load_rearm_r96.h` `Seen::take()` fires once per COMPLETED observed load (T8 `inventory_lifecycle_t8` episode via `observed_load_recovery_r96`), `RearmAfterLoadR96()` in frik_pose_probe.cpp (~3967) clears DECAP_LOOSE_NECK, NPC_TOUCH, BLOOD_CONTACT_FX, NPC_FLINCH, FISTS... one log line each. real pin/vtable/retail mismatches are NOT reset (they stay off and say so).
- **pose restore debt R96 / R96R1**: when the restore after skinning fails for some joints, `pose_restore_debt_r96.h` remembers the failed joints + owner (character, skeleton, pose buffer, C37 player lifetime). recovery: exact readback on the same buffer, a new qualified player lifetime, or one retry per completed load (`ObservePoseRestoreRecoveryR96` at frik_pose_probe.cpp ~4835, read-only readback at callback end `ObservePoseRestoreAfterCallbackR96R1` ~5068). R97: my flag-only clear in `RearmAfterLoadR96` removed so there is ONE mechanism + a guard in `RecordPoseRestoreDebtR96` (coherent_pose.inl:229-233). OPEN RISK (never seen in a log): indebted buffer never returns AND player lifetime never changes -> VRIK off until restart = breaks my rule. fix if it shows (`POSE_RESTORE_FAULT_LATCH`): retire the debt on a completed load where the indebted buffer is not current.
- **uncapped hand stream latch (R96, mine)**: see tracking below. that one was a real session latch and it is gone.

#### 4. tracking samples and timing (59c6, D67, D69, D85R5R3, R94, R96)

- **R59.16C6 (13 oct)**: `tracking_sample59c6.h` - one immutable family HEAD/LEFT/RIGHT located at the SAME XrTime, published with the original tick/ns/epoch (not restamped after the GPU handoff). `VrSynchronousHands=1` default. family age max 80 ms, button/permission age 100 ms. sample-count gaps are no longer timeouts. this was the "coherent current-time hands" build, same day as "THE PERFORMANCE IS GREAT!!!! like butter smooth" (C7E-DECAP1, 13 oct 13:58).
- the jitter forensics (23-26 oct, F7 on 21,672 frame triples): arm IK, bone write, finger rig and controller input are CLEAN (jerk raw 0.31 -> finalGoal 0.62 mm/frame^2 median). noise enters at (1) the body anchor Z = animated native pelvis unfiltered (hands AND camera bob together, anchor-Z step p99 5.9 mm standing / 11.1 walking) = the `VrHmdBodyHeight` story in §5; (2) stale free-contact hold/glide with no live plane (L 31 % of free frames >1 cm off, up to 400 mm) -> D80 contact work; (3) anchor source switch frameValid 1->0->1 moving every goal by median 116 mm; (4) two yaw frames (hands via entity root yaw, camera via native camera basis).
- **C7E vs now (23 oct)**: C7E mapped the wrist from the SOLVED shoulder with the F8 arm ratio; C51R1/C54 went 1:1 from the head anchor so a user arm longer than henry's hit the reach cap every frame = "flailing"; H44 put the map back on the REST shoulder before the torso solve (proposal `VrArmAnchorSolved`).
- **D67 motion trials / D69 hysteresis / D85R5R3 fresh pose uncapped**: `VrMotionTrialD67` = 0 baseline, 2 FRESH POSE timing (default; waits for the next XR token, `WaitForNextRenderFrameToken(token,20,true,false)`, measured median 5.97 ms CPU wait, p95 7.4 ms = cadence coupling to 90 Hz), 3 ELBOW reach preference (F7 rung 3), **4 Fresh Pose Uncapped** (3 nov): no token wait, one extra pose-only locate of head+grips at the pre-update boundary so the native update, physics and VRIK consume it (`early_frame_pacing_d45.inl`, try-only locks, no second xrSyncActions). Home > VRIK EXPERIMENTS > Fresh Pose Uncapped. NOT proven to change FPS or smoothness - "there is no measured FPS gain or headset smoothness claim".
- **R94 `fresh_hand_target_r94.h`**: independent pre-update hand family (`handFramesR94`). R94R1 rejected the runtime's 50 ms prediction horizon (>4 periods at 90 Hz) and STARVED the hands (7,982 STALE_PREDICTION, 3 publications, `poseReads=88202` frozen, `POSE-REJECT HEAD_TRACKING_INVALID` every 2 s from 17:44:15, before any menu toggle). R95 removed the rejection. **R96 (mine)**: `ReadFamilyR94` (tracking_sample59c6.h:76-107) latched onto the uncapped stream for the whole session, never back to the worker family when it stopped. now the independent stream wins only while fresh (same epoch/generation, <80 ms `kIndependentFamilyFreshNsR96`), else a NEWER current-epoch worker frame, per read, nothing stored. that was "i switched body follows native and could not enable VRIK again" - the switch was innocent.
- **R88 #2 "hands not smooth / VRIK jittery"**: no VRIK regression, final/raw hand accel ratio 1.01; what changed was the game dropping to 74-83 fps so 9-18 % of XR frames reused the previous pose. dice game: exactly 50 % reuse because `dice_body_d81::read` (80 ms age + strict lease) is only current every second camera publish (owner alternates FirstPerson/Minigame).

#### 5. the body frame: tracked_frame51, HMD body height, crouch, R98 three modes

- **C51R1 (17 oct)** `tracked_frame51.h`: "one qualified completed pose supplies the frame for the camera, tracked hands and body". only a successful commit + restore may publish a shared frame; the anatomical native-eye offset is learned only from a neutral qualified frame. `pose_frame_foundation.inl:129-140` `pelvis_foundation54` with `isolateNativePelvisYawR25` removes native pelvis pitch/roll (and yaw where allowed) so the animated lean never reaches the hands. the 0.45 m residual check (`tracked_frame51.h` ~253) resets a bad frame, it is not a latch. `VrSmoothMovement` (default false) filters the shared frame temporally.
- **C54 (19-20 oct) HMD body height**: `VrHmdBodyHeight=true` = "HEIGHT: HMD+BODY": body/head/hands root-relative from the headset, physical crouch drives henry (`physicalCrouch54`, pose_follow_input.inl:26). false = "NATIVE CROUCH": native pelvis Z incl. the button crouch, with the walk bob (the anchor-Z jitter above). i asked for physical crouch on (H4). menu: Experiments > Body "Body uses tracked head height" (`SetHmdBodyHeight54`, body_settings.inl, bit 63 of the frame control token + generation bump so a pose never mixes two modes). this is the switch i call "body follows native".
- **R93 native crouch** (`native_crouch_r93.h`): exact retail `C_Player::IsCrouched` (`getterRva=0x72F628`, 27 byte prologue pinned), read on the game thread after Ready(), 80 ms receipt. `VrNativeCrouchR93` = crouch by BUTTON (native pelvis drop 0.5 m excused in the 45 cm residual check, `tracked_frame51.h:219-238`) vs PHYSICAL. Immersion > Crouching. OPEN RISK (no log yet): PHYSICAL + the crouch BUTTON drops the pelvis ~0.5 m, the frame fails the 0.45 m check, camera falls back to hold then native eye.
- **R98 (5 nov) three modes in one setting**: `VrBodyHeightModeR98` 0 HMD HEIGHT / 1 NATIVE HEIGHT / 2 HMD + NATIVE (RIDE/CROUCH) (default 2 when the key is missing; old `VrHmdBodyHeight` still written for old builds). COMBINED: z = HMD + (native - HMD) * weight, weight -> 1 while the crouch receipt or rider evidence (`RiderRecentR98`, 750 ms) is fresh, linear over 0.25 s, missing receipt holds max 0.5 s then HMD. first frame after a fresh start takes the wanted height directly. the crouch receipt is now also published in COMBINED+PHYSICAL (`native_crouch_r93.inl:8-11`). row: Experiments > Body "Body height" cycles the three; Immersion page 2 `Row::BodyHeight`. the Immersion crouch button no longer forces HMD height.
  why: in NATIVE height the hands bob 8 cm with the walk pelvis ~3x/s (F7 anchor z 1.534 -> 1.500 -> 1.486 -> 1.562) but the held weapon (a simulated body) only took a frame shift through `native_stance_delta_r93()` = zero unless BOTH frames were hmdHeight -> the weapon lagged, "came from behind". fix `shared_frame_delta_r98()` (tracked_frame51.h:368-385) at physical_weapon.h:1470/:370, intent_frame_rebase_h8.inl:24, coherent_pose.inl:1734. HMD mode bit-identical to R93. not headset tested.

#### 6. elbows and shoulders ("elbows have never been good")

the full table had 17 rows, short version with the reason each one died:

1. V66.69 fixed downward pole - stiff, "visibly worse". 2. V65.81/V66.88 pole from henry's ANIMATED elbow - follows the game animation not you; dropped in V66.89 (pose_math.h:57 "no live native elbow input exists"). 3. V66.95-99 `continuous_elbow` (body pole carried along the arm, tau 65-160 ms, 4 rad/s) - the FILTER is fine (bow arms 0 jumps), the target was wrong. 4. V66.103 torso clearance search (10 deg bins +-60) - picks opposite branches, 60 deg jumps (D66). 5. H14/H15 (20 oct): H14 moved the elbow 394 mm for a 0.6 mm wrist move; H15 used the previous bend as DESTINATION = "corkscrew elbows". 6. R5 (23 oct) anatomical destination side-down + .6 backward. 7. H18 `elbow_continuity60` never wired. 8. W1 (25 oct) pole .20 out/.45 back, hanging cap 30 deg - fixed the "chicken wing", "what stays wrong is the body-fixed pole". 9. H15/H17/169r2 bow policy - bow arms skip conditioning (0 jumps) but it is a second solver = context switch on every bow prepare. 10. D45 (28 oct) continuous local search + shoulder trial on F2 rung 3 - the F2 HOTKEY switched the solver while recording, each switch reset the history. 11. D48 FRIK lab (`frik_elbow_lab.h`): FRIK's min 10 / max 145 deg flexion, smooth windows, swivel retention .75/frame - NO rate cap, a target jump became a 20-45 deg/frame swing (112 jumps >15 deg in 45 s hands-up, 3 nov). 12. D64 wider cosine blend. 13. D67 ElbowContinuity (ON) - every policy change wiped the history -> 12-29 cm snaps. 14. D67 elbow trial (F7 rung 3). 15. D69 ElbowHysteresis (10 deg bins + 100 ms dwell + 180 deg/s) - a second filter in series, "can feel restrained". 16. D85R1..R5 "calibrated outward floor" for raised arms (`elbow_continuity.h:53,73-77`): `max(0,.2-dot)` kink * smoothstep over hand height -0.25..+0.05 m = exactly where the hand is pulling the bow to the face -> the elbow FLIPPED up/down with the arm axis unchanged. "you make every fix worse". **R88 reverted it** (ec339f7): replay 105 jumps -> 10. 17. D86 `elbow_destination_d86.h`: one anatomical destination for tracked AND bow arms on the fixed-length circle (relaxed solution, torso intrusion objective, forearm direction cue from the hand's local axis), the pole clock approaches it.

the common mistake of all of them: the elbow direction came ONLY from the body (fixed vector / torso search / game animation), never from your hand. a body pole is singular where the arm points along it (hand in front of the face, hand up and back over the shoulder) so mm of hand motion spin it; and the forearm ignored the displayed hand (wrist bend p95 135 deg with hands up = "elbows rotate strangely").

**R89 hand-guided elbow (3 nov, `elbow_r89.h`)**: target = unnormalised sum of parts perpendicular to the shoulder->wrist axis: body pole (W1 side-down + .6 backward; bow arms their archer preference) x .75, minus hand forward x 1 (elbow behind the wrist along the displayed hand), minus thumb direction x .5 (elbow on the little-finger side), + a push out of the chest capsule (torso radius + 5 cm, 8 cm band). weak sum = hold still. continuity in angle space tau 60 ms, max 5 rad/s (<=3.2 deg per 90 Hz frame), cold start at the current native elbow (no snap coming back from a native owner), policy changes no longer wipe the history (coherent_pose.inl ~1058). replay of the 3 nov log, hands above the shoulder: 112 jumps -> 0, wrist bend p95 135 -> 58.5 deg, elbow in chest 49 -> 0. replay harness: `tools/elbow_r89_export.py`, `tools/elbow_r89_replay_test.cpp`. NOT headset tested by me; the R90 review found no defect but says "not established as a headset cure".
**what runs today (R106)**: `pose_arm_solver.inl` ~17-70: D86 destination is the DEFAULT (`VrElbowSolverR89=false`, `VrFrikElbows=false`), Experiments > Elbow preference cycles D86 DEFAULT -> R89 HAND-GUIDED -> FRIK TRIAL (`live_settings_d39.h`). R89 and FRIK never run together (`frik_elbow_lab::Active() = enabled && !R89`). bow arms (`bowPoleH15`) skip conditioning. shoulders: `upper_body.h` `shoulder_goal103` / `shoulder_goal_reach104` (V66.103/104) + D45 shoulder fit on F2 rung 3 + `shoulder_reach_d38.h` wrist-authoritative fit. if the elbows still rotate with hands up: test R89 ON first, then FRIK OFF, before anyone touches the solver again.

#### 7. calibration (F8)

- F8 = guided palms-up T-pose, 2 s window, >=20 samples over >=1 s, HMD moved <8 cm, hands <12 cm (`calibration_profile.h` `Window`), gate `tpose()`: span .65-2.20 m, reach .30-1.30 m per side, |ly-ry|<.20. stores `Arms{span,leftReach,rightReach,headToHandPlane,leftOffset,rightOffset}` in the INI (sealed with a hash, `seal()`), plus `[VRIK.Head] StandingEyeHeightM` (V66.107) and the finger banks (V66.100 `[VRIK.Fingers.<rigHash>]`). yaw-only neutral: looking down during F8 must not define down as neutral (calibration_solver.inl:53, :148 "camera and every body-calibration path share horizontal HMD heading").
- `V65.49` first version, `V65.54/57a` averaged guided capture, `V66.100` persistent anthropometry, `V66.107` standing height, `V66.123` the profile supplies only arm geometry (see above), `C43` `VrCalibrationAutoRestore` / menu RESTORE: a recenter keeps the calibrated eye height (body base = headset corrected by (current eye - standing eye), camera origin gets the same shift) so recentering slouched or after a knockdown does not move the body for the session. `R59.16C10` longer receipt with the numbers. H47 (25 oct) "global calibration native camera" moved the recorder formatting off the game thread.
- `g_cal_state != 2` -> `CALIBRATION_NOT_READY` reject in BeginCoherent6689; with the CALIBRATED mode (`user175::Mode::Calibrated` = pose 4 / weapon 6) the normal VRIK runs. R96: the menu VRIK cycle could not reach OFF after R95 made Hands 4/6 (the cycle only knew 5/0) -> `user_modes175.h hands()` fixed.

#### 8. fingers

- input: trigger = index, grip = middle/ring/pinky, thumb open (V65.28 -> V66.78 -> today, `pose_fingers.inl`). the grip pose is OpenXR GRIP not AIM: -Y distal, -Z little-to-index side (comment pose_fingers.inl:3-7, "do not copy the aim-pose -Z").
- references: OPEN = V66.105 bind-pose blend (or a saved/hardcoded pair), CLOSED = the authored chain in hand space; H45 whole parent-local hands from the recorded native unarmed punch pose (capture SHA 5e2255..., `finger_rig_w1.h` matched by finger name crc + chain position so it works on other henry rigs). C59: middle PIP toward ring a few degrees CLOSED only (axis from the hand's own authored chain). D78 dialogue fingers, D81 collision fingers (fingertip plane curve search before the wrist moves, V66.103 §5), `finger_contact_h31` springs.
- DEAD END: a fabricated geometric fist fallback (V66.89: "native fingers remain until both endpoints are ready"). also don't trust the native relaxed-idle curve as OPEN (V66.97 regression).
- the mod must not write finger caches as files - everything is in KCD2VR.ini sections (my rule).

#### 9. hand bodies and the palm

- free-hand contact uses five palm samples (wrist/metacarpal rest positions, `pose_fingers.inl:223-256`) and 2 mm skin (D79 balanced mode `contact_balanced_d79.h`), station mode D75 (`station_hand_contact_d75.h`), precise toggle C44. the hand never gets collision authority over finger curl.
- `phys_bodies59c10r8.h` (R59.16C10R8): three PE_RIGID bodies (blade capsule, one sphere per palm) following the tracked poses by velocity, HIGGS style. `VrPhysBodies` 0 OFF / 1 FOLLOW (collide with nothing) / 2 COLLIDE (refused until a spawn-clearance boundary is verified). verified native layouts: `CreatePrimitive(+8102E0)`, `CRigidEntity::AddGeometry(+526194)`. default 0. this never got past FOLLOW in the headset as far as the docs say.
- `hand_present_d62.h`: optional late display shift max 5 cm / 15 deg, never owns a semantic hand. `hand_separation_ds1.h`: free-hand self separation.

#### 10. head / neck / NPC look-at

- V65.09 F5 rung 2: hook on the entity-target resolver `WHGame+0x59B0F0`; in dialogue, when the target is exactly the local player CEntity, the resolved head point is replaced by the final OpenXR camera world position (fresh <=120 ms, delta <=2 m, else pass-through). V65.28 added failure-reason diagnostics ("target wrapper payload is not the local player / too far..."). V65.61a "HMD-LOOKAT-REPAIR" is the repaired build of that. the NPC actually looking at you in dialogue still has issues (D79/D80 NPCLOOKAT runs).
- the player's own head: V63.3 mode 7 rotation-only overlay on SPINE2/3/NECK/HEAD -> V65.28 rung 5 -> V66.123 head anchor = current native head, 5 cm cap -> V66.128 chest twist/neck 55 %. the camera is NOT the VRIK head: it is the native camera + HMD (V66.109), joined to the committed pose through `tracked_camera_service.inl` (`PlainGameplayCameraE1()` needs `!nativeBodyOwned112`, eye reconstruction needs <80 ms source). when the pose is refused the camera falls back to the hold, then the native eye - that is where "black/jumping frames at pickup" complaints came from (D55 review: 18 final-owner publication races at 23:03, offsets 0.26-0.47 m between two camera bases).

#### 11. roomscale, body follow, crouch

- **body follow** (`VrikBodyFollow6DOF`, F1, V66.128; `body_follow_r38.h` R38 = visible body posture from the calibrated HMD yaw delta, foot stepping) - tested, ON.
- **R58 roomscale follow** (`roomscale58.h`, `VrRoomscaleFollow`): R = HMD offset + anchor; beyond a 12 cm dead zone the native movement stick is pushed toward you (gain 0.35 m for full follow stick, max stick .65, full beyond 0.9 m), as henry catches up the camera hook subtracts the walked distance. nothing teleported, walls stop him natively. **combat dodge** (`VrRoomscaleDodge`): >=10 cm sideways/backward HMD move within 250 ms while locked -> native dodge (jump + stick) once, 120 ms hold, 2 s cooldown; D70/D75 `leanHeightD70` .10 m: a lowered head is a lean, not a dodge, and never a forward step. the dodge was tested and works.
- **D76 native 1:1 physical walking** (`VrNativeRoomscale1to1D76`): finite displacement path - distinct tracked HMD samples -> horizontal delta `worldDelta = worldRight*dx - worldForward*dz` -> submitted ONCE to the native living-physics collision step, camera removes the consumed raw displacement only after native adoption. not a servo. D84 connected the controls (the old walking rows could switch the legacy follow on while D76 stayed off; F1 no longer writes follow=true). the live row labels an old selection `LEGACY FOLLOW`.
- honest status: in every log through D85R5 (R88 review) `VrRoomscaleFollow=false` and `VrNativeRoomscale1to1D76=false` - "walking did not stop working, it was never switched on". "6DOF not working at all" was the 6DOF crafting mode, not VRIK. physical walking is NOT headset-proven.
- crouch: see §5 (PHYSICAL vs NATIVE BUTTON, R98 COMBINED).

#### 12. horse riding

1. up to D55 (29 oct): every mounted clip (`relaxed_idle_rider_01_horse`, `2d_horse_walk/trot/canter`) was `PROTECTED_ACTION` native=1 - "the missing VR upper body on the horse is an explicit current native-owner policy". D46 had missed the four .bspace gait names and a neutral facial clip (`fa_idle_neutral_03` layer 1) intermittently claimed the whole body -> "slow reins vs fast tracked body" alternating; D50 fixed the classifier (`vr_combat_ownership111.h` four exact .bspace gaits, `body_context126.h` facial leaf exemption).
2. D76 (1 nov): `riding_vrik_d76.h` arms-only admission (`RIDING_GAIT_ARMS_D76`), full=false, no world contact, no physical mode, body follow reset. D79 run: the missing upper body is consistent with this implementation. also: the live model head is accepted only within 0.75 m of the calibration anchor (`LiveHeadUsableD80`) - on a horse it is further -> the metre-scale offsets.
3. D80 riding pose (mounted = always use the untouched native head of that gait), D81 "finish native mounting, wait 1000 ms, then admit the existing NORMAL VRIK path incl. unarmed riding" (`RIDING_AFTER_MOUNT_WAIT_D81` -> `RIDING_NORMAL_VRIK_D81`), R92 riding stationary tests.
4. **R94R1 run (4 nov, "2 m above the horse")**: thrown off 18:13:21 (no clip = ragdoll, VRIK on), getup 18:13:25 under VRIK, remount 18:13:52 with `horse_mount_RIGHT_02` which R94R1 did not know (only left clips in `vr_combat_ownership111.h:74-76`) -> VRIK + HMD-height frame ran through the mount; the `_align` clip lifts the root onto the saddle (1.36 m) while the animated head is still on the ground -> camera 1.55 m above henry for 3-4 s (`CAMERA_BODY cameraMinusSolvedZ=1.5552`). "henry leans, hands not on my body": seated SWORD gaits and `horse_combat_rg_idle_lngsw` were not in the seated list -> whole body native for a minute; the mounted poses that ran had `frameValid=0` = hands solved on the native leaning pelvis. horse VRIK was only ever good with no weapon drawn.
5. **R95**: `riding_aliases_r95.h` all 16 mount + 16 dismount clips -> `RIDING_NATIVE_TRANSFER_R95` wins before everything; seated list (272 names) incl. the sword/mace/shield gaits; `pose_frame_foundation.inl:64` uses the native seated pelvis height (no standing HMD height on the horse), :124 refuses a mounted pose without a frame (per pose, retried), seat reference has its own `lifetimeR95` retired on every mount/dismount.
6. **R96 (mine)**: `GetupR96()` = the 8 `getup_ground_{back,front,left,right}[_player]` clips native (`NATIVE_GETUP_R96`), per scan, no latch. **R97 merge**: the 1 s settle REMOVED (the seat is final within 2-3 cm at clip end; the wait restarted on every unqualified scan = 1 s native lean after every mount/load/toggle; want it back: `riding_vrik_d76.h:92` + a constant). seat foundation per visit kept (`tracked_frame51.h` `mountedR96`: first mounted frame captures the seat, no HMD standing height on the horse, residual >0.45 m re-captures). 56 missing rider gaits added (`2d_combat_lg_horse_*`, `*_walk_bwd*`, bow/crossbow/rifle; the list is `std::binary_search`ed, keep it sorted, 328 entries). `loadEpisodeR96` retires the seat on a save/load. **R98**: `riderSeenTickR98` (750 ms) feeds COMBINED height so seat-less gaps on the horse don't jump to standing HMD height.
7. OPEN: the ragdoll after being thrown (PLAYBACK `entries=0`, no clip at all) still runs under VRIK - no alias to key on, not guessed. nothing of R95-R98 riding is headset tested yet. 1-minute check: mount from the RIGHT side with F7 on: `BODY_GATE reason=RIDING_NATIVE_TRANSFER_R95` for the whole clip then `RIDING_NORMAL_VRIK_D81`, `cameraMinusSolvedZ` < 0.15; draw the sword on the horse: never `PROTECTED_ACTION alias=horse_combat_*`, `frameValid=1`.

#### 13. what the headset said (timeline)

12 sep V64.49 hand +25 cm visible. 13 sep V64.56 upper body WRITE-ELIGIBLE, "no useful visible result". 17 sep V65.27 arm good. 13 oct C7E "butter smooth" BUT hands disappear at shelves, head sinks in bow draw, no VRIK camera in dialogue. 13 oct C9A "sword position now ok", 6DOF body not following, dizzy bob. 14 oct C10R3 "nothing worked, same problems". 15 oct C19C1/C2 slideshow on every hit. 29 oct D55 elbow 74.6 mm for a 7 mm wrist move (shoulder moved 65 mm = torso input). 31 oct D66 "hands still not smooth; elbows better but too tucked; free hands dragged like weapons" (and i withdrew that the old build ever had perfectly smooth hands). 2-3 nov D85R5 elbow flicker at the bow ("you make every fix worse"). 4 nov R94R1 2 m above the horse, lean on the horse, VRIK lost after "body follows native".

#### 14. if you want to fix X, look here

- hands offset from the controllers: `pose_tracking_goals.inl` (requested = anchor + frame*(raw - trackedBase)), `pose_reach_input.inl` (shoulder-relative map, soft reach), `tracked_frame51.h`; F7 `FRAME/ARM` rows, `CAMERA_BODY targetErrorM`. check `VrBodyHeightModeR98` first.
- elbow flips / rotation hands up: `pose_arm_solver.inl` ~17-70 (solver selection), `elbow_r89.h`, `elbow_destination_d86.h`, `frik_elbow_lab.h`; F7 `ELBOW_INPUT` pole step >15 deg with axis <5 deg; `tools/elbow_r89_replay_test.cpp`. never add another filter on the body pole.
- shoulders / "too tucked": `upper_body.h` `shoulder_goal103` (20/16/6 deg), `elbow_shoulder_d45.h`, `shoulder_reach_d38.h`.
- hands jitter/step: F2 `FRAME reusePct` first (fps < 90 = pose reuse, not VRIK); then `VrMotionTrialD67` (2 vs 4), `ReadFamilyR94`, `hand_prediction_d69.h`. hand filters (`VrHandSmoothH50`, `smooth_lab_f2.h`) are visual only and never the answer.
- VRIK dead after a menu/toggle/load: look for `POSE-REJECT reason=` every 2 s and `XR_TIMING poseReads` not growing; `RejectCoherent6693` stages (`CALIBRATION_NOT_READY`, `NATIVE_BODY_NO_CURRENT_RELOADING_PASSENGER`, `DICE_WAIT_CURRENT_CAMERA_BODY_ANCHOR`, `T4_RETIRING_...`); `RearmAfterLoadR96`; pose restore debt (`POSE_RESTORE_FAULT_LATCH`).
- body height / crouch / bob: `tracked_frame51.h:199-225` (COMBINED weight), `body_settings.inl`, `native_crouch_r93.h/.inl`, `pose_follow_input.inl:26` (physicalCrouch54), `shared_frame_delta_r98()`.
- who owns the body right now: F7 `BODY_GATE native= reason=` rows; `animation_filter.cpp` ~700-745; `combat_visual_aliases112.h`, `riding_aliases_r95.h`, `native_passenger_a34.h`.
- horse: `riding_vrik_d76.h`, `riding_aliases_r95.h`, `tracked_frame51.h:113-177` (seat), `animation_filter.cpp:636-705`; tests `tools/check_riding_r96.py`.
- roomscale walking: `roomscale58.h`, `subsystems/.../pose_producer_d76.inl`, `VrNativeRoomscale1to1D76`; `ROOMSCALE follow= consumedFrames=` rows.
- fingers: `pose_fingers.inl`, `finger_reference.inl`, `calibration_runtime.inl` (INI banks, V66.105 `VRIK.Fingers.*` verdicts), `finger_rig_w1.h`.
- calibration: `calibration_profile.h` (gate numbers), `calibration_solver.inl`, `calibration_runtime.inl:246-300` (recenter/reanchor), `V66.107-HEAD REANCHOR` rows.
- NPC looks at you: `subsystems/dialogues/runtime/npc_lookat_hmd.cpp` (hook `+0x59B0F0`).
- VRIK cost: `V66.89-POSE COMMIT composedPerSolve essential=N/M defer137`, `V66.122 PERF POSE_SOLVE` (640-840 us is normal), `g_compose_defer137`.


---

## 4. PHYSICS, MOTION, TRACKING, DIAGNOSTICS AND TESTING

this section is everything "under" the gameplay features: the physics bodies and collisions, how the hands/head are sampled and paced, the perf story, the F-key recorders and the one log file, and how we test without a headset (which is most of the time). the gameplay features themselves (combat, crossbow, alchemy, doors...) have their own sections, here is only the stuff they all sit on. WHGame.dll is always the same retail image (timestamp 0x6A350E20, sha bdf8f9e4...79f7), every RVA below is for that one.

a short reading guide: physics = chapter 1, motion/tracking/jitter = chapter 2, performance = chapter 3, diagnostics/log = chapter 4, tests + build = chapter 5, "if you want to fix X" = chapter 6.

---

### 1. PHYSICS

#### 1.1 world collision for hands and weapons (ray sweeps, not bodies)

what i wanted: the hand stops at the table, the sword stops at the wall. simple.

1. **V66.92 (30 Sep 2026)** "VRIK-WORLD-COLLISION": first collision layer. F10 rung 6 = full upper body + static-world/terrain collision. it uses the game's own `IPhysicalWorld::RayWorldIntersection` helper at `WHGame+0x4846E4` (gEnv `+0x492B800`, `pPhysicalWorld = *(gEnv+0x30)`), query mask `ent_static|ent_terrain` (0x101), flags 0x8002040F. a 5-ray fan approximates a 5.5 cm hand volume, arm segments are checked at intervals and the endpoint is retracted toward the shoulder in 1.5 cm steps (max 12 cm). living actors, loose props, worn gear, doors deliberately NOT blocking. fingerprint mismatch = `V66.92-COLLISION BINDING BLOCKED`, falls open to plain VRIK. rung 5 kept as the A/B without collision.
2. **V66.113 (early Oct)** physics discovery: the engine HAS rigid/articulated bodies and constraints (Lua `SetPhysicParams` 0x712030, `AddImpulse` 0x37CFD7C, `AddConstraint` 0x37CF95C, Constraint.lua:47-85), but rays can never push anything. "the mechanisms exist; integration is not yet certified".
3. **R59.16C5/C6 (13 Oct)** physical_weapon.h sweeps: weapon = requested controller pose vs "allowed" pose, stop/slide/rebound against STATIC+TERRAIN, bounded motor (see 1.3). R58 added the "wall feel" counters (`MELEE_FLOW wall58{blocked slid backout stuck scrape}`) - before that "WALL" in the logs was just a timer label, there were NO world contact records at all.
4. **R11..R13 (25 Oct)** contact flicker: claude found the problem in the log: ALL 47 big right-hand jumps with a still controller were "plane frames" (fingertip ray hit = infinite plane, wrist projected out, clear pass = instant snap). the second one: native ray calls of 29-34 ms (2.6-3.1 display periods) and that the 18-query/1 ms budget cannot preempt a running call; R13 removed the diagnostic-only arm-segment queries (pose_arm_solver.inl:50-58, 4 native queries per frame nobody consumed).
5. **D79/D80 (1 Nov)** balanced mode (contact_balanced_d79.h `compose(): out[5]=thumb, out[6]=index`) solved every plane ANY feature hit for the WHOLE WRIST (`ProjectPalmPlanes6697`). D80 capture: 1288 blocked rows, 1029 of them ONLY a fingertip on the apple or on an npc/dog, wrist pushed median 28 mm (apple) / 61-76 mm (body, max 215) while the controller moved 0.2-2 mm; palm-on-table was fine (13 mm). that was the "VRIK going insane" flicker when taking an apple or petting the dog.
6. **D81 collision-only patch**: discovery rays carry provenance, they stop further entry but never spend an existing tip overlap as wrist displacement; a continuing same feature/part/normal keeps its measured surface push. 626 asan assertions, same path for Precise and Balanced, kept by D84.

WHAT WORKS NOW: `src/frik_pose_probe.cpp` `ContactHand6695` / `CompleteHandContact6695`, `src/vrik/contact_balanced_d79.h`, `src/subsystems/physics/pose_free_hand_contact.inl` (free hand admitted only when the hand is really free: coherent_pose.inl ~658), `src/vrik/physical_weapon.h` for the held weapon sweep (independent of the hand path, C55). `src/vrik/npc_contact172.inl` is the hand/weapon world query in "bodies" mode and since R103 the held prop is on its skip list. INI `VrPreciseSurfaceContacts` (default true; Precise ON = finger probes, OFF = simple; the atomic is `preciseContacts44`). known open: a fingertip already inside a surface clips instead of teleporting the hand out (intentional), 20 mm station display cap can't guarantee mesh clearance.

DEAD END: letting a fingertip plane move the wrist. DEAD END: filtering the hand to hide contact snaps (see 2.2) - the snap is 5-40 cm, no filter hides that without lag.

#### 1.2 physics BODIES for the hands and the blade (so the world knows we exist)

what i wanted: push a cup off the table with the hand, hit a corpse with the sword and it moves. HIGGS-style.

1. **KCD1 lesson (C10R7, 15 Oct)**: Diaz Dizazter's "Simple Ragdoll Physics" mod notes: melee never moved corpses, arrows did - because arrows are physical projectiles. weapons need bodies. his values/tables are NOT allowed (license), only the engine-generic CVar NAMES. KCD2 ships no `sys_spec_Physics.cfg`, so the mod writes CVars in memory.
2. **C10R8** `src/vrik/phys_bodies59c10r8.h/.inl`: three PE_RIGID bodies (blade capsule r 2.5 cm, palm spheres r 4.5 cm), masses `VrPhysHandMass` 3 / `VrPhysBladeMass` 4 kg, velocity drive closing the error in 35 ms (cap 14 m/s, 40 rad/s), teleport > 0.6 m, PARK when the target is stale > 80 ms. pinned slots: IPhysicalWorld vt 407FC80 Create +0x88 = 8463B8, Destroy +0x98 = 846E10, GetById +0xB0 = 667418; CRigidEntity vt 3A48DC0 SetParams +0x20 = 6F1BE4, GetStatus +0x30 = 8A6C90, Action +0x38 = AC0D18, AddGeometry +0x40 = 526194. menu Immersion > EXPERIMENTS > "PHYSICS HANDS + BLADE", INI `VrPhysBodies` 0 OFF / 1 FOLLOW / 2 COLLIDE. collision class `type=game ignore=living` so Henry is never touched, never `geom_colltype_ray` so our own sweeps don't see them.
   - mode 2 COLLIDE today: "new proxy creation REFUSED until a full-volume spawn-clearance boundary is verified" (phys_bodies59c10r8.h header). so the C10R8 bodies never collided in the headset. they follow, they get destroyed cleanly, thats it.
   - **STILL A LATCH**: `PHYS_BODIES LATCHED_OFF reason=... (feature off until restart)` (phys_bodies59c10r8.inl:90). R96 deliberately kept this one out of the load re-arm: "the physics-bodies lab (a retry could leave bodies)". if you touch it, make the re-arm destroy the old bodies first.
3. **H24 (20 Oct)** `VrNativeHandPhysics` + `native_hand_proxy_owner60.inl` (DS-F58): the hand grip proxy as a NATIVE owner driven through the T4 physics phase (8463B8 factory -> AdoptCreatedRigid -> ReadyRigid -> SetVelocity motor -> destroy). H24 also added the shared physics callback preparation at VRIK startup even with the switch off - that is the "127 guarded reads per idle callback" perf finding (3.3). default `VrNativeHandPhysics=false` (mod.cpp:4056).
4. **T4 (Oct)** `src/subsystems/physics/runtime/native_weapon_body_t4.cpp`: a real rigid sword proxy behind F9 rung 2, with State {Off,Waiting,Creating,Configuring,Active,Retiring,Quarantined,Unavailable} and Reason codes (ClearanceUnknown, BodyReplaced, NativeFault, WorldChanged...). `native_weapon_clearance_t4.cpp` has the exact-retail AD2450 capsule overlap (Clear/Occupied/Unknown only, no surface normal). H3 headset run: `F9=OBSERVE_ONLY bodyState=DISABLED bodyId=-1` and 8x `PREVIOUS_MUTATING_ATTEMPT_OR_FAULT` - the T4 installer never got past phase registration in the headset. H6 found the bug: a failed rung 2 kept rung 1 and F9 retried forever.
5. **R103 (5 Nov)** the collision CLASS bits: `self_collision_native_r103.inl` writes the same 28-byte type-26 SetParams packet the hand proxy (native_hand_proxy_owner60.inl:253) and weapon body (native_weapon_body_t4.cpp:227) already send: `type = (type & AND) | OR` at body +0xEC, `ignore` at +0xF0 (branch at 850AA2; rigid 6F1BE4 -> CPhysicalEntity::SetParams 84E3EC at 6F1CD4; articulated 6F2FE8 -> 6F1BE4 at 6F319B). Henry's living capsule has type bit 4 (living) - the mod reads it at native_physics_phase_t4.cpp:385. see 1.5/1.8 for what it is used for.

honest status: hands and blade as REAL colliding bodies has never been seen working in the headset. the hand sweeps (1.1) + the velocity-driven props/corpses (1.5, 1.8) are what you actually feel today.

#### 1.3 weapon feel: dynamics177, authority, RPG stats, SIMULATE HENRY

`src/vrik/weapon_dynamics177.h` (namespace `kcd2vr::dynamics177`) is the bounded "mod-owned presentation mechanics" of the held weapon - NOT engine rigid body params, NOT damage. history in the file comments:
- **F4 rung 2 (V66.177)** `Body body(cls, inventoryWeight, length)`: class masses sabre 1.2 / axe 1.8 / longsword 1.4 / mace 1.5 / halberd 3 kg, COM fraction, grip inertia. `Authority authority(str,agi,stamina,live,support)` = force 180 N / torque 35 Nm base, x(.6+.4 str) etc, support hand x1.7/1.8. `advance()` is a PD with velocity feed-forward, dt capped at 4.1 ms, gravity support reserved BEFORE steering (C10R14: before that a tired one-handed pole "sagged and hunted for seconds").
- **R59.16C10R2** `fatiguedAuthority59c10r2`, `compliance59c10r2` (the envelope: 5.5 cm / 8 deg one hand, 3 cm / 6 deg with support, widening when tired).
- **C10R14** `handling59c14`: one-handed polearm authority fraction `VrPoleOneHandFeel` (0.40 default, .15..1), axe/mace .85; `fatigueRamp59c14` (`VrWeaponFatigueRamp`, x4 at empty stamina).
- **C10R17** `stats59c17`: RPG stats RELATIVE to the weapon's own requirements (melee def +0x118 StrReq / +0x11C AgiReq, game window +-10 = rpg_param MaxStatToAttackStatDiff). `VrWeaponStatEffect` k (0 = R16 handling, 1 default, 1.5 max). my rule from 16 Oct (C26c): a weapon 10 str above Henry feels 3x heavier one-handed (S floor 1/3), two hands ~1.8x. plus `skillFactor26`: weapon skill x(1+.02(level-15)), .70..1.30.
- **C26c "SIMULATE HENRY"** `statSim26` atomic, INI `VrSimulateHenry`, WEAPON FEEL page button (vr_menu.cpp kWfSim26): 0 real stats, 1 LOW (str 6 agi 6 skill 2), 2 MEDIUM (14/14/12), 3 HIGH (25/25/25). feel only, `SetStatSim26` in `subsystems/physics/stat_sim_control.inl` logs `C26 SIMULATE_HENRY ... gameStats=UNCHANGED`. read by `pose_physical_executor.inl:30`.
- `scaledAuthority44c1` = the "weapon authority scale" slider (.2..2), `impact()` = bounded perceptual recoil (impulse 1.08 m v, capped 8 m/s, linear recoil max 1.2 m/s).
- **D69** `advanceAngularD69`: OFF-default "rigid primary grip" trial - only the angular motor, translation owned by the controller.

the trade-off documented on 23 Oct: everything added after C7E (handlingLoadsH2, handling59c14, fatigueRamp, stats59c17, skill26, impale sag C44) LOWERS authority = more lag. wanted design, but it IS the difference to the "butter smooth" C7E weapon. to feel C7E weight: `VrWeaponStatEffect=0`.

#### 1.4 ragdoll CVar lease (F9) and the lab rungs

- **C10R7** `ragdoll_lease59.h`: F9 lease writes `es_HitCharacters=1 es_HitDeadBodies=1 es_UpdateCollisionScript=1 e_DeferredPhysicsEvents=1 ca_UsePhysics=1` read->write->verify->restore, `PHYSICS_BASELINE` once per session, `Lease::Reassert` every 3 s because KCD2 resets CVars on load (`RAGDOLL_F9 REAPPLIED count=N`). nothing costly (p_max_MC_iters, e_PhysProxyTriLimit) and nothing world-changing (p_gravity_z). menu `[ ] RAGDOLL PHYSICS (F9)` saved as `VrRagdollF9Default`.
- **C58** `src/vrik/lab58.h`: rungs `kRungOff=0, kRungObserve=1, kRungCorpse=2, kRungLab=3`, subs LivingHitImpact / CorpseDrag / PropWake / LivingTouch. the living ones are "saved requests only; D51 living actuators are PAUSED" - knocking a living npc into ragdoll was not natural yielding and the dog flailing during petting was a regression, i am not bringing that back (DEVELOPMENT.md).
- **D84**: F9 is now an independent finite PHYSICS RECORDER (capture168 Domain::Physics), it does NOT select a gameplay physics mode anymore. corpse dragging runs from its own switch (see 1.5).

#### 1.5 corpses: drag, two hands, no latex, no pushing me

what i wanted: grab a dead body by the arm and drag it, lift it, with one or two hands. this went through more rounds than anything else in physics.

1. **console drag (C7-ish)**: `zzkcd2vr_phys.lua` could drive an entity's velocity toward a point in front of the view for a few seconds. it moved a corpse. "a working console command was not a working grab" - no tracked grip, no limb, no shoulder carry (DEVELOPMENT.md).
2. **C7G (13 Oct)**: entity root != articulated skeleton main body (`CPhysicalProxy +38` is canonical; logs had `CONTACT_BODY_REPLACED` rejections, dropping that check would push the wrong body). native carry is separate (`C_ActorCarryCorpse` vft 3A2F438, module +68 state 5 = carrying, 285E564 requests carry) - observed via `F5_CARRY`, never called.
3. **C19C4R3 (16 Oct)** lifetime facts: `GetPhysicalEntityById` 667418 returns the pointer with NO AddRef; queue bookkeeping bumps body+40 by 0x20000, the real refcount is +54 (AddRef 5311C4 / Release 531F84); status26 walks body+1D8 with NO lock. a constraint ID is not a hold and a pointer is not a lease.
4. **H6/H7 (19-20 Oct)** bounded corpse PUSH on F9 rung 1 restored (C69 corrected: the failed rung 2 kept rung 1, refusal must not retry every frame, `AddImpact` returns AL=1 regardless of the Action result so "true" only means submitted).
5. **H9 (20 Oct)** the plan that is still the plan: loose prop grip first (rigid, T4 leases), then corpse limb with a separate ARTICULATED lease, living touch needs a geometry/joint witness (raw part 379 is ambiguous, C62: primary parts and the joint+200 secondary branch coexist - never `rawpart-200`, never `rawpart==joint`).
6. **H21 (20 Oct)** `native_articulated_phase_h21.inl`: ArticulatedLease on the exact articulated vft 3A48B30 (GetStatus +30 = 8A9348, Action +38 = A99DF0, type 5 add -> AC0D18, type 26 status -> 8A6C90). "a positive returned id is Submitted, NOT Present".
7. **lab58 drag (C58..D40)** `src/vrik/lab58.inl` articulated route: either hand keeps a local point on a qualified corpse part, drives it with `pe_action_set_velocity` per part id (A99DF0 type 10, "+0x1F0 per part"), gain 6, maxSpeed 3, leash .5 m. `corpse_body_owner_d40.h`: a hand and an impale can never share a body. the circular bug: acquisition used the contact-corrected wrist, the body could stop that wrist against itself, moving the controller further did not move the servo target. fixed to use the completed tracked hand BEFORE contact correction (DEVELOPMENT.md).
8. **D72 saved route** `saved_corpse_drag_d72.inl` + `zzkcd2vr_corpse_d72.lua`: Lua provider (`F9_HAND_DRAG_D72 SEARCH/REFUSED/GRAB/DRIVE`), needs native `IsDead`, same thread, current VM, unchanged root, empty hand, consumed grip; whole-entity `SetVelocity` servo. D80 log: provider READY `gate=F9_LAB_AND_CORPSE_DRAG`, zero candidates. D84 removed the accidental gates (lab rung 3, F9 rung 3, physical combat ON) and the settings code that wrote the corpse preference back as false. "READY alone is insufficient".
9. **R95/R96**: Experiments > NPCS "Physical corpse dragging" (`VrLabCorpseDrag58`), R96 fixed TWO hands (the Lua search skipped the body the other hand held, the second hand always picked the same part -> claim refused; now nearest OTHER part). `VrCorpseSearchSqueezeR89` = search only on squeeze (if fps drops near bodies). the R97 run: only SEARCH/REFUSED rows (`NO_NEAR_DEAD_BONE`, `HAND_OCCUPIED`), no GRAB.
10. **R101 "latex"**: i saw limbs and the head stretch like latex when dragging or when a body hangs on the blade. cause: both routes overwrite the velocity of ONE part every frame = that part has infinite mass, the other ~40 kg hang through the soft joints and the skin stretches between the bones. fix `src/subsystems/physics/corpse_follow_r101.h`: `Shaper` caps the driven part (hand 2.5 m/s, 30 m/s^2 = 0.33 m/s per 11 ms frame, from 0 at the grab; hang 3 m/s, 40 m/s^2) and `Rest/plan()` stores the grab-time distance of every other part and only removes the velocity component that STRETCHES it away (slack 5 cm + 8 %, pull back 8/s max 1.5 m/s); guard: a part 20 cm over its limit (stuck) takes the pulling component off the gripped part, then the normal leash release - no latch, grab again at once. two hands: one writer per part, pair correction split in half. native side `gore32n::BodyFollowR101` in `native_gore.inl:79-103` (GetStatus 8A9348 by array index, writes with the existing PartVelocity). host test worst case: drag 2 m/s for 2 s stretch 3.597 m -> 0.010 m.
11. **R103 "the body pushes me"**: the ragdoll collides with Henry's capsule, when i pull it into my legs the parts are driven into me. fix: after each accepted part drive `self_collision_r103::Exclude(Kind::Corpse)` = ragdoll `ignore |= living` (type not changed), restore when the corpse origin is 1.0 m horizontal from Henry, at the latest 4 s after the last hand let go, two hands share one lease; validity check before restore = the ImpaleRoot47 proof (articulated vt 3A48B30, foreign +0x20 == entity, type +0x28). side effect while dragged: npcs also pass through that body. caveat in the code: the articulated SetParams skips everything while `[body+568h]` is set (6F317B) -> row `REFUSED`/`QUEUED_BY_PHYSICS_STEP`, retried every 250 ms.

WHAT WORKS NOW (code): articulated route lab58.inl:335-363 + saved route saved_corpse_drag_d72.inl:307-336, both through corpse_follow_r101 and the R103 lease; owner `corpse_body_owner_d40.h`; load/fault recovery `load_fault_recovery_r96.h` (gore32n::g_drive_fault33, lab58n::g_dragFault is an alias). F9 rows: `R101 F9_CORPSE_FOLLOW ok=1 parts= stretched= worstM= guarded=`, `R103 SELF_COLLISION phase=EXCLUDE kind=CORPSE`. NOT proven in the headset: R101 and R103 are untested as of R106 (no GRAB/DRIVE row exists in any log i sent). the saved route is a whole-entity servo, not a limb constraint - the H21 constraint only runs in the F9 grade-3 lab.

DEAD END: AddImpulse on the npc entity root (8A49FC picks the entity physics proxy, not the skeleton main) - a pcall true means no script error, not movement. DEAD END: the `Body::Living` enum filter in frik_pose_probe.cpp forwarding touch records only for Living while real limb contacts were ARTICULATED - removing the filter alone is wrong (AddImpact's int is a JOINT index, 37C6158 compares it against the joint count). DEAD END: bypassing `ROOT_STILL_LIVING` (serial 17 in the H3 log: collider==skeletonMain!=entityRoot, root stayed Living 24 polls / 1500 ms) - thats a real refusal, not a bug.

#### 1.6 part anchors and the part namespace

`src/vrik/part_anchor59c7g.h`: the exact retail `GetStatus(type=1)` buffer (0x80 bytes, +4 PART ID, +8 array index, position +0x18, rotation +0x3C, scale +0x4C, +0x78 timeBack=0 to avoid the historical-pose branch at 8A6E2D). `Identity {serial,epoch,request,target,entity,body,part}` must all match or the anchor is dropped. `part_namespace62.h`: the C62 finding that raw part ids are not per-instance witnesses. `impact_identity59c6.h` joins contacts by source sample + owner, not only the ms tick (C6).

#### 1.7 impale: velocity hang vs constraint, and the fast death

- **C32/C33 HANG** (`VrImpaleHang`): `native_impale_part.inl` drives one ragdoll part at the blade point with set_velocity (gore_policy.h follow=10, maxSpeed was 6 -> R101 3). waits up to 4.5 s for the ragdoll after the kill.
- **CL-IMPALE60 / H21 hold** (`src/vrik/impale_hold60.h`, `constraint_service60.h` DS-F18): type-5 ball joint between the ragdoll part and the weapon body, "every native death is a ragdoll (141/141 hasRagdoll)". packet layout proven, `[buddy+0x47]` type byte check NOT proven. if the add is refused the velocity hang stays. in the R97 log there were no H21/CL60 rows during the hang = the servo did it, not the constraint.
- **R96** the kill-impale gates: since D41 only a contact within 6 cm of the TIP counted, a real stab reports the contact on the BLADE (feature=2) -> now blade counts for sword classes; strength was stroke joules / full cut (75 J) so a stab scored 0.02 vs need 0.30 -> now fastest contact of the same stab / 6 m/s (0.30 = 1.8 m/s). damage unchanged.
- **R106** "impale fast death" `vrik/impale_death_speed_r106.h` (`VrImpaleFastDeathR106`, ON): while the servo waits for THIS victim's ragdoll his CActionController time scale (+0x9C) is set 3x, restored on pin / give-up / 5 s. never Henry, never two at once. untested.

#### 1.8 props: pickup servo -> static grab -> brush swap -> R103 limits

1. **staged_pickup596c2 / prop_drive5916c1 (C7-era)**: defers the native pickup, drives the exact CRigidEntity with a bounded velocity servo: force cap 40 N, mass 0.01..20 kg, sample lifetime 80 ms, max separation 35 cm, release keeps momentum. "a bounded servo, NOT a HIGGS constraint". contact classifier forgot CRigidEntity 3A48DC0 (C7G added RIGID=4).
2. **R89/R90/R94** `src/vrik/static_grab_r89.inl` "small props in the hand": Grip with an empty hand near a small rigid entity. R94R1 run at Kutna Hora: every tankard/plate said "grab: fixed level geometry has no loose entity" - all hits `bodyVftRva=3A578C0 owner=0 entityId=0` = static brush without a CEntity. also a Lua bug: `e.id` is a userdata handle so `e.id ~= id` was always true and `PrepareSceneryR94` refused everything (`sameIdR100` fixed it).
3. **R100 brush swap** (`brush_swap_r100.h/_native_r100.inl`, `VrSceneryPropSwapR100` ON): Grip on a level brush -> Hide (IRenderNode slot +0x220 = 5DF758, ERF_HIDDEN bit 8 of +0x28) -> suspend physics (CPhysicalWorld +0x98 = 846E10 mode 1, sim class byte +0x47 -> 0xFF) -> Lua `SwapSpawnR100` BasicEntity with the same .cgf, mass = volume x 350 kg/m3 clamped 0.2..1.5 kg, `ENTITY_FLAG_NO_SAVE` -> place (CEntity +0x168 = 509810 SetPosRotScale) -> normal small-prop acquire. brushes <= 65 cm, table of 256, everything restored on load (`SwapLoadR100`). pins `SwapPinsR100`, mismatch = one log line and swap off for that game build. 6 swaps worked in the R101 run (09:53-10:00).
4. **R103** the held prop pushed me away and spun: the prop is a normal rigid body, Henry's capsule is living, the servo added up to 40 N into me. fix: `Exclude(Kind::Prop)` = `ignore |= living` AND `type |= living` (so our own hand proxies/weapon bodies, which ignore living, don't hit it either), retried every 250 ms while held, restored when 0.6 m horizontal from Henry, min 0.4 s / max 4 s after release (a thrown prop does not hit me on the way out). `CarryLimitsR103`: force 40 -> 80 N, angular 80 -> 300 rad/s^2 per kg, max spin 12 -> 20 rad/s. plate misaligned 13 cm: the grab offset was taken from the raw controller (inside the table) instead of the visible, collision-solved hand -> `CarryHandR103` uses `PropWorldHand596`.
5. **R94R1 (KNOWN_ISSUES)**: "scenery teleport carry" replaced by a finite-mass velocity drive, RigidBodyLight classification corrected, limits 3 kg / 65 cm / 30 cm reach, release beyond 35 cm.

physics-less meshes and fixed world brushes without a usable .cgf path are not covered (cleanPath rule in brush_swap_r100.h:85). DEAD END: broadening inventory eligibility to every render mesh (D84).

#### 1.9 physics diagnostics (physics_d42, PHYS_DIAG, F9)

`src/diagnostics/physics_stage_d42.h`: read-only T4 stage receipts, QueueSize 64, DetailBudget 2048, lanes Bug/Vrik/Physics/Combat (D84 added F9 and F3 lanes to the original F2/F7). producers copy, only the worker formats. `physics_d42` is also used by lab58.inl and pose_held_solver_stage.inl. 931 `PHYS_DIAG` snapshots were in the D84 corpus. `f9_status_fd.h` = the Shift+F9 read-only detail ladder. `D84 PHYS_CORPSE_PROVIDER` reports transport/callback age/candidate flags from copied atomics. rule: F9 provider READY is installation evidence, not a grab.

---

### 2. MOTION / TRACKING

#### 2.1 the reference: C7E "butter smooth" (13 Oct) and what made it

the one build i called butter smooth is R59.16C7E-DECAP1. a static check in D79 showed the four things that made it and that they are all still in the tree:
- **pacing59c6** (`src/core/render_pacing59c6.h`, R59.16C6): the native L/R publisher builds a pair token `(pairFrame<<32)^executeSerial`, the desktop PRESENT counter is a different domain. C5 compared `latestDesktopPresent > pairToken` = a new certified pair was never "fresh" -> ~64 XR loops/s while native stereo did >100 pairs/s. C6: `Decide()` uses inequality for paired modes 2/6/7, `>` for desktop; grace measured with steady_clock not GetTickCount64 (mode 2 grace clamp(frame*.5, 3..7 ms), mode 3 .8/5..9, default .35/2..5); `WaitFuture` = never copy an unsubmitted desktop image.
- **tracking59c6** (`VrSynchronousHands=1`): head and both grip spaces located at the SAME XrTime, one immutable family, age bound 80 ms, button/permission age 100 ms (a new pose cannot renew an old button edge). "same coarse tick" samples are not zero-dt anymore (both interval endpoints in ns).
- **duplicate HUD publication reuse** (C7) and **`timeBeginPeriod(1)`** (C7A: every Sleep/cv timeout in the process rounded to 15.6 ms before). `xr_timing165.h` prints `headsetPeriodMs fresh reuse freshPct` every 5 s without F2.

C7E was not perfect either: disappearing arms and bad shelf contacts were in that period too. dont remember only the good part (DEVELOPMENT.md "Hands first").

#### 2.2 smoothing: what i tried and why "more filter = slow"

1. **smoothBody51 / C51 SMOOTH MOVEMENT (`VrSmoothMovement`, OFF)**: one anchor = native head low-passed in model space (0.30 s, 0.08 s for big changes), camera bob subtracted, hands = anchor + controller 1:1. the idea was fine, in the headset the filtered shared anchor that the HEAD also uses was the nauseating one. 24 Oct note: "that was the nauseating one, untouched here".
2. **D39 `VrSmoothHands`** (FRIK-style retention .7 per 90 Hz frame = `smooth_lab::CutoffHzForRetention`): "I kept trying smoothing because the head was stable and the hands were not. More filtering made them slow instead. That was the wrong thing to treat as progress." canonical OFF beats an old legacy ON (mod.cpp D39_SMOOTH_PREFERENCE_LOAD).
3. **H50 one-euro (`hand_smooth_h50`, `VrHandSmoothH50`)**: only the controller TERM of a FREE hand (requestedGoals - handAnchor51 and the DesiredHand wrist), not the head, not combat intent (ts stays raw). minCutoff 1.5 Hz, beta 20 Hz/(m/s), rot beta 5, bypass at 1 m/s / 4 rad/s raw, max lag 2 cm / 10 deg, restart on 100 ms gap or 25 cm jump. default today: `enabled{false}` in visual_selection59c7f.h, INI falls back to the legacy D39 value. tuned later to the tighter kBypassRawSpeed 1.0 and kMaxLagM .02.
4. **F2 rung 5 ANCHOR100MS** (anchor-Z-only filter) - i felt no change (26 Oct correction note). so finding #1 below is real but moves hands AND camera together.

DEAD END: any filter on the shared body/camera anchor. DEAD END: hand filtering to cure cm-scale contact/coordinate discontinuities ("a small adaptive filter could address residual input noise later; it cannot repair centimetre-scale coordinate/contact discontinuities", review 25 Oct).

#### 2.3 jitter forensics (what the F7 data actually said)

per-stage jerk on the R8d2 run, the bug hunt (21,672 frame triples, free hands): raw controller p99 13.2 mm/f^2, requested 14.1, finalGoal 16.0, solved wrist == finalGoal, bone 16.0, world 51.6, raw HMD 1.2. so arm IK, bone write, finger rig and controller input are CLEAN; noise enters at "requested" and at "finalGoal". ranked:
1. body anchor Z = animated native pelvis unfiltered (`anchor = calibratedHead + (nativePelvis - calibratedPelvis)`, tracked_frame51.h:120, XY pinned to rest at :182, Z follows unless hmdHeight) -> hands AND camera bob together vs the world. VrHmdBodyHeight=true kills it. (now `VrBodyHeightModeR98`, mode 2 HMD+NATIVE for ride/crouch.)
2. stale free-contact hold/glide with NO live plane: left hand 31 % of free frames >1 cm off (up to 40 cm), many steps exactly 16.0 = the quantized rate-limited release. -> contact_hold_r11, D81.
3. anchor source switch frameValid 1->0->1 moved every goal by median 116 mm (max 1.17 m) between frame51 and liveNativeHead123. D79 showed the same again: both hand goals jump 780 mm while the controllers move 0.16 mm when frameValid flips, a 0.45 m check compares HMD-height coordinates against the native anchor. R96 then fixed the "uncapped stream stays owner forever" variant in tracking_sample59c6.h.
4. two yaw frames: hands to world by the entity root rotation, camera_hook applies the HMD delta in the native camera basis (bow path uses the root) -> 1 deg = 9 mm at arm's length. TO VERIFY, never closed.
5. whole-image cadence: CADENCE reuse 4-11 %, peaks 25.7 % - a reused game image under a fresh HMD pose steps everything; the headset reprojects the head, never the hands.

also proven: the held-weapon recovery seed snap (R14: 437 mm for a 3 mm request; the log: "second clear step 434.63 mm, projection converted into -39.51 m/s"), fixed by retiring recovery inside the ordinary envelope (275/275 asan). and the arm map: C51R1/C54 removed C7E's "wrist = solved shoulder + (controller-HMD-userShoulder) * HenryArm/userArm", a user arm longer than Henry's got pulled in every frame = elbow flailing; `VrArmAnchorSolved` puts the C7E formula back after the torso solve.

#### 2.4 hands on the same frame as the head: HJ1, F2 rung 4, D67 trials, D69 lead, D61 lead, D85R5R3 uncapped

1. **HJ1 (R5, 25 Oct)** `src/vrik/hand_same_frame_hj1.h` (`VrHandSameFrameHJ1`): the XR thread publishes head+hands at 90 Hz, the game (75-85 fps) reads the hands early and the camera reads the head later in the same frame - when an XR frame ticks in between the picture has head N and hands N-1 = 11 ms (2 m/s -> 2.2 cm). HJ1 predicts which head the camera will use and locates both grips at that display time. defect: `Current->NoteCamera(frame42)->Current->NoteCamera` makes two locate targets with splitFrames 0, and tracking_reader.inl:27-33 swapped hands+poseTime but kept the family head = MIXED TIME family. today `hj1::enabled{false}`, only the HJ1 comparison rung turns it on.
2. **F2 rung 4 pacing (I10, 26 Oct)** `smooth_lab_f2.h SetMode(4)`: PublishViewCameraDetour waits at most once per verified gameplay frame for a NEW XR token (20 ms cap), on timeout or 8 slow intervals suspends 5 s (`Pacing`, kSlowRunLimit=8, kSuspendNs=5 s). i tested it: **visibly smoother, except fast motion**. 11,290 gated frames, zero timeouts, avg wait 3.97 ms max 15.24, reuse 1.47 %. the log showed: camera latch age rose from ~0 to 5-6 ms = the wait did not retime the hand sample upstream.
3. **D67 (31 Oct)** `src/vrik/motion_trials_d67.h`: F7 ladder 0 off / 1 baseline / 2 fresh-publication timing / 3 elbow+solve; **D70 made accepted fresh-publication timing (mode 2) the missing-key default** - `VrMotionTrialD67=2`. recording and timing are independent: F7 never turns the timing off. rejected in D67 review: a 90 ms prediction, new camera/stereo ownership, blanket collision pass-through. R92 split the elbow preference (`elbowPreferenceR92`, legacy mode 3 migrates once).
4. **D69** `claude_motion_d69.h`: four OFF experiments (Experiments > VRIK+ > PAGE2): extra hand prediction 0..33 ms (`handLeadMs` 11 default - "a chosen offset beyond OpenXR, not measured display delay"), same-cycle weapon placement, rigid primary grip (advanceAngularD69), elbow pole hysteresis.
5. **D61** `hand_present_d61.h` (`VrFreeHandPresentLeadD61`, OFF): presentation-depth lead isolated to FREE-HAND DISPLAY, raw gameplay tracking untouched.
6. **D85R5R3 Fresh Pose Uncapped (3 Nov)** mode 4: the concrete wait was `WaitForNextRenderFrameToken(token, 20, true, false)` in the fresh-pose path - 646 receipts, median token wait 5.97 ms, p95 7.44, max 10.48, and the fps cap off did not remove it. mode 4 = one pose refresh at the outer native-frame entry (before the update) with try-only locks, then return without any token wait (`early_frame_pacing_d45.inl`, xr_manager.cpp, frame_sync.cpp). camera source table: 0 cached worker keeps the cached head, 1 predicted uses the runtime target, 2 NOW converts QPC->XrTime and locates head+grips at one base target, 3 offset untouched. F7 row `FRESH_POSE_UNCAPPED ... waitNs=0 meshSubmission=UNOBSERVED`. "no measured FPS gain or smoothness claim". R94/R96: switching 2<->4 bumps `handRefreshGenerationR94`; a stale uncapped sample must not stay owner.

WHAT WORKS NOW (defaults): `VrPoseSourceMode` effective default 0 = cached XR worker (view_defaults_b2.h:44; my INI and the C7E history ran 2 = synchronous NOW - exactly this mixup showed up in D61), `VrSynchronousHands=true`, `VrMotionTrialD67=2`, H50/HJ1/D69/D61 all OFF, F2 rung ladder in `smooth_lab_f2.h` (rung 4 = pacing, D64 `timingLocationD64` 1 = the accepted late I10 boundary, 0 = early D45). open: the final palette/held-object draw-to-eye join is unobserved by every receipt; the 90/110 fps question (3.4).

#### 2.5 roomscale: stick servo -> finite native step

1. **R58 (20 Oct)** `roomscale58.h` `VrRoomscaleFollow`: beyond a 12 cm dead zone the native movement stick is pushed toward you, the camera hook subtracts what he walked. `VrRoomscaleDodge`: sideways/back HMD move >= 12 cm in 250 ms = B + stick direction (KCD2's locked dodge) held 120 ms, 700 ms cooldown, forward is never a dodge.
2. **D76 (1 Nov)** `src/subsystems/roomscale/` (`native_roomscale_d76.cpp`, `pose_producer_d76.inl`, `tracked_step_d76.h`): fresh raw HMD displacement joins the native living `Step` BEFORE capsule collision as a FINITE delta (`worldDelta = worldRight*rawXDelta - worldForward*rawZDelta`), consumed from the camera only after native adoption (receipt ledger). lowered head (10 cm with hysteresis) = lean, not a step. `VrNativeRoomscale1to1D76` OFF by default. "HMD tracking reports the headset, not a pelvis or a footstep".
3. **D84**: the visible walking controls could turn the OLD stick servo on while D76 stayed off (the behaviour i explicitly dislike). now the Mod getter reports walking as D76 OR a preserved legacy follow (labelled `LEGACY FOLLOW`), explicit walking selects D76, F1 only toggles the torso, off = no fallback to the servo. plus the pad race: axes/buttons were separate atomics, the dispatcher could send B with the previous frame's forward Y -> `pad_dispatch_d84.h` one coherent frame per dispatch (246 checks, the D83R1 preimage fails 116).

untested in the headset: D76 finite walking (i had it OFF in the reviewed INI), the pad coherence fix. 4 of the old `dir` dodge rows (18-19 Oct) had a forward component - the recognizer's no-forward clamp was already there.

DEAD END: offset-to-stick follow (R58) as the walking method.

---

### 3. PERFORMANCE

#### 3.1 VRIK cost (V66.137, Sep)
head motion "slightly jaggy" with VRIK on: 100-112 fps off -> ~92.6 on, `POSE_SOLVE ~800 us` per solve on the skinning path. reproduced offline with a 720-joint fixture: `BeginCoherent6689` does ~25 edits and `Compose6689` after each, every spine edit recomposes ALL descendants incl. ~210 face and ~300 armour helper bones = 5,900 compositions/solve, only ~80 bones are READ. fix: deferred passenger composition (`BuildEssential137`, `ComposeFull137` once before commit, kill switch `g_compose_defer137`), bit-identical output, 960 -> 390 us. the remaining 390 us was 43 % redundant `QNormalize` (never done - "fast FK path" item A).

#### 3.2 hook cost registry (V66.178R22, Oct)
"performance is bad after calibration" -> `src/diagnostics/hook_cost178r22.h`: every detour wraps its body in a Scope, 37 slots (INPUT_BEGIN ... POSE_TRANSACTION), counters calls/us/maxUs/origUs (time inside the GAME's original function)/ownUs. 5 s summary only under F2. R22C1 added resident-page checks before region queries for the hot reads, the target-bounds observer stops walking native objects when F3 is off, nested scopes are labelled inclusive ("do not add inclusive timings together or call them GPU").

#### 3.3 the decline, by the old logs (D61 hold, D79 history)
- comparable F2 baseline windows, median native update: D52 87, D55 79, D66 83, D69 83, D71 79, D78 82, D79 78 fps; XR delivery ~90 Hz throughout, D79 image reuse 12.2 %. R14 ~89 fresh/s vs R26+J27 ~60 (J27 also rendered 33 % more pixels - i raised the resolution AFTER noticing trouble, a confound not a cause). H50 and D52 were the SAME scene.
- two bogus comparisons corrected: old R14 counters had impossible values; old HOOK_COST totals omit POSE_TRANSACTION (added D48), so "D79 is slower than R14" by that total was counting a whole uncounted stage.
- idle work with everything OFF: (A) the shared T4 physics callback (H24) repeats `WorldQualified` 20x = 127 guarded reads per idle callback (same from R14 to D79, so not the D52->D55 cause alone); (B) companion timers: life_d73 every 250 ms, sheath every 1 s; (C) the native alpha/scale HUD interceptor outside diagnostics. ruled out: a 512-slot cache clear per hook, a multi-ms pose mutex wait, the 1 ms timer disappearing.
- the map was the strongest measured regression: D73 WORLD map ~87 fps open / MAP_TICK 6 ms, D79 ~34 fps / 15 ms, 54 stale hide/show cycles. MAP_TICK runs FIRST in the native frame callback. D84 removed automatic map profiling.
- the D61 INI loss (30 Oct): the 1349-line INI was gone, defaults selected Precise mode 1 / smoothing OFF / physics OFF = more ray work.

#### 3.4 the 90 / 110 question
the D84 rule: "Compare matched scene/configuration, refresh policy, CPU/GPU/frame submission and diagnostic state before attributing the remaining 90/110 FPS difference. H50/D52 were the same scene. No D84 FPS or vendor GPU result exists here." D85R5R3 removed the 6 ms token wait only in mode 4 and explicitly does not claim 110 back. nobody has measured a R9x build. the instrumented hooks' median own cost in D79 was 1.043 ms/frame, POSE_TRANSACTION ~0.74 ms, XR_PICKUP is a render-transfer WAIT (not pickup CPU) - "do not describe 1.043 ms as the entire cost of the mod", GPU and pre-hook native work are not in any counter.

#### 3.5 the recorder itself costs
H46: 1.25 M lines in 10 min (808 k `VRIK_F7 FINGER` = 321 MB) formatted ON THE GAME THREAD before native combat. H47 moved formatting/draining to the mod worker and batched the file writes (`FileBatchH47` 64 KB in diagnostic_trace.cpp). any "performance BAD" run with F3/F7 on is not comparable to C7E, which had no F7 recorder. `WritePerformanceD61` (Shift+F2, `performance_gate_d61.h`, 180 s, 2 s reports) is the whitelist for bounded aggregate timing; it never sets a capture168 bit.

---

### 4. DIAGNOSTICS

#### 4.1 history of the F keys
- **V65.x (Sep)** every F key was a LADDER of experiments: F8 stereo rungs (dual camera 16 mm / 40 mm / OpenXR IPD), F10 VRIK rungs 0..6 (see 1.1), F12 weapon authority (V65.73 "hand-root weapon authority", V66.84 "post-helper weapon authority"), F5 shop/LookAt probes. the static validation notes of that time are mostly "PASS: F10 rung X retained".
- **V66.168R1 (7 Oct)** "plain recording keys": F2 = HUD/bug recording, F3 = combat/contact, F5 = interaction, second press stops, 180 s lease expires on its own, no Shift. recording must NOT change the gameplay mode under test ("must not silently switch a smithing test from VRIK to native"). log fields still named F3/F5 are the OLD gameplay-mode values.
- **capture168** (`src/diagnostics/capture168.h`): `Domain {Combat, Interaction, Hud, Reload, Hands, Vrik, HandReference, Bug, Physics}`, `Duration=180000` ms, aliases Hud->Bug(F2), Hands->Interaction(F5), HandReference->Vrik(F7) (D48). `gate.select/active/receipt`: a producer must hold `active()`; `receipt(domain, session, now)` lets ALREADY captured rows and END receipts drain for 5 s after a stop under their original session - a replacement session rejects old rows. `vrikRung`/`cycleVrik` = the D67 F7 stages 1..3. **D84 added `Domain::Physics` = F9** as an independent recorder (it does not select a physics mode). Shift+F9 = read-only detail ladder, Shift+F2 = perf_d61 lease.
- today's table (DIAGNOSTICS.md): F2 bugs/timing/elbow comparison, F3 combat (D85 `ZONE` rows with origin/sent/requested +0xF0/committed +0x180/executed +0x150, `R3_NATIVE` with combatObj = actor+450), F5 interaction (3 rungs), F7 VRIK (3 rungs, `ANATOMY_D86`, `BASIS_D86`, `FRESH_POSE_UNCAPPED`), F9 physics, F11 reload (2 rungs, now also joined into F5/F7 via `reload_capture_d84.h` priority F5 > F7 > F11 with the full active mask so one copied record serves all keys).

#### 4.2 the log rules (the ones i keep repeating)
1. **ONE file: KCD2VR.log** (D81 "One mod log"): startup, companion loading, map perf, finger audits, every F-key capture. no KCD2VR-fingers BIN/JSON, no DetourModKit_Log, no map-performance or companion-startup sidecars, no .bak INI copies (R96 removed both pre-setup.bak writers; the last-good INI is in memory only). the logger is configured on the mod worker before hook startup; capture toggles never open/truncate/close another file. native callbacks enqueue within the logger's 512-byte inline limit and never wait for disk. "Forwarded means admitted to the logger, not durable on disk after a crash."
2. **`diagnostic_trace::Write` vs `WriteStatus`** (`src/diagnostic_trace.cpp`): `Write` is dropped when NO recorder mask is set and the message is not a session-bound END receipt ("Previously the essential() whitelist bypassed every F key" - that was the leak that made the log grow with recorders off). `WriteFor(domain)` needs that domain active, `WriteReceiptFor(domain, session)` uses the 5 s drain grace. `WriteStatus` always enqueues - "bounded lifecycle/fault receipts only, callers rate-limit, no screen or extra file" (R91R1: DLSS refusal, one-eye reset cut, unconfirmed resize). `CaptureVerbosityD81` raises the DMK level to Info for a capture batch and restores it.
3. **on-screen text (R90)**: the `KCD2VR ...` info line in the headset shows ONLY while F2 records. F3/F5/F7/F9/F11 write the same text to KCD2VR.log as `VR status: ...` (visible_failure logMailboxD80, coalesced on the worker, 30 s repeat) and never on screen. with F2 off nothing is shown.
4. **receipts are not results**: "F9 provider READY is installation evidence, not a successful grab", "an API return is not proof the body followed the hand", "a hide call is not the first visible pixel". every review since D80 has that sentence somewhere and it is right.
5. **no permanent latches**: my rule from R96: "don't add any more blockers which cancel functions forever, even after loading the save, until restart". `src/core/load_rearm_r96.h` `Seen::take()` = true once per COMPLETED observed load (T8 episode from `observed_load_recovery_r96.h`), each owner thread re-arms its own flags there (`RearmAfterLoadR96` in frik_pose_probe.cpp:3967: DECAP_LOOSE_NECK, NPC_TOUCH, BLOOD_CONTACT_FX, NPC_FLINCH, FISTS, NATIVE_COMBAT_REQUEST, FINISHER_UPPER, ATTACK_CLOCK_COMBO_HOLD, WEAPON_PREDICTION, TORCH_GESTURE, PHYSICAL_WEAPONS_HOLSTERS; log `R96 LOAD_REARM after save/level load: ...`). the R96R1 `load_fault_recovery_r96::Fault` clears itself when a load completes after the fault (gore32n::g_drive_fault33, g_corpse_contact_fault_h7, g_prop_fault5916c1). a latch is EITHER a Fault OR a re-armed flag, never both. kept as real session stops: game-code byte mismatch / hook install safety (they say so in one line) and the phys_bodies lab (1.2). R103/R104/R106 leases all end on their own (4 s / 10-60 s sit-out, never "off for the session").
6. **readback OFF decisions**: the exit-saved INI is a snapshot, "it must not be used to reconstruct the live options during an earlier action" (D85R5R2). same for "an off readback in the log does not supply the enable edge". and D84: the central settings used to overwrite the corpse preference with false on save - thats why an INI said OFF for a feature i had turned on.

#### 4.3 what a good recording looks like
keep the DLL hash + INI + companion identity with the log; the folder name of a review is not the build that made the log (D84 found a "D79" log with no in-file build marker). copy the diagnostics log BEFORE the next game start (the 708 MB H50 diag log was emptied by the next launch). run one key for one action (F3+F7 for a dodge, F5+F7 for a reload, F9 for a corpse), 180 s is plenty.

---

### 5. TESTING AND BUILD

#### 5.1 what "static validation" meant in V65 and what it means now
- **V65.03..V65.93 `*_STATIC_VALIDATION.md/.txt`**: Linux container, no MSVC, no DetourModKit package (the CMake configure stops at `find_package(DetourModKit)`), so "validation" = a python validator per build (`tools/validate_v65_08.py` 26/26, `audit_exact_whgame_v65_10.py` 11/11 byte pins against the real DLL) + "changed-file lexical/braces guard" + "PASS: F10 rung X retained". every one of them says "not a Windows/MSVC compile or headset result". they caught structure regressions, nothing more.
- **host syntax check** (V66.x onward): `g++ -fsyntax-only` of the big TUs against `tests/syntax_shims/` (fake windows.h, excpt.h, intrin.h, psapi.h, DetourModKit/hook.hpp + logger.hpp) and stub d3d12. the number of errors is the metric: "frik_pose_probe 38 errors (base 38, none in changed files), mod.cpp 1, xr_manager 825 = R105". a changed file that adds an error is the finding; the baseline errors are the shim gap (LARGE_INTEGER, SEH, D3D12). never a "successful whole-project build".
- **host fixtures** `tests/*.cpp` (1458 files): pure headers get direct tests (`corpse_follow_r101_test`, `self_collision_r103_test` 35, `lab58_test` 74, `pad_dispatch`, `xr_layer_guard_r104_test` 65...). production `.inl` code is EXTRACTED by anchor comments into a fixture with mocked native leaves (`tools/run_static_prop_r94.py`, `check_corpse_recovery_r96.py`, `check_impale_native_pending_r96r1.py` debug/optimized/asan_ubsan/retained_h18). `tools/host_compiler.py` picks `cl` (`/std:c++20 /O2 /EHsc /UNDEBUG`) or g++ (`-std=c++20 -O2 -UNDEBUG -pthread`). assertions stay on in Release. Windows-only: real SEH tests must use the Windows SDK, "a C++ exception substitute does not verify structured-exception cleanup".
- **preimage / mutant controls**: every D8x/R9x review compiles the OLD code into the same fixture and shows it FAILS (D84 pad race: preimage 116 failures; crossbow 606 checks with 5 historical mutants). a test that cannot fail the old code proves nothing.
- **claim boundary**: every round wrote down what was NOT run: `claim_boundary {windows_dll_built:false, game_run:false, headset_run:false, old_logs_are_r105_evidence:false ...}`, recordings with sha256 + in-file build markers, then per-check results. counts are assertions, not scenarios ("821 assertions across eight suites" is not 821 headset tests).
- **the full suite is broken in places and we say so**: `README_D87_RUNNER.md` (run_regressions_d87.ps1, `-Asan`): "historical fixture failures; a focused run is not a substitute". known stale: `protected_baseline171.json` (do not rewrite its hashes), `run_v66108_checks.sh` needs clang, `check_impale_native_pending_r96r1` fails on BodyFollowR101 since R101, `alchemy_input19c4_test` does not build with g++. "Record an unavailable compiler or fixture failure honestly."
- **headset test = the "1-minute check"**: exact key, exact action, exact row to grep. the old V65.56 / V66.137 / R59.16C6 test guides were the first form of it.
- **regression ledger** (D84): 14 "failed assumptions" with the current decision, e.g. "Provider READY or SetVelocity returning means a corpse moved", "BODY/F1 should automatically select walking", "A later filename/folder proves a later build ran", "Successful host tests certify Windows hooks or headset results". don't repeat one.

#### 5.2 build toolchain
- **Aug 30 2026**: the first build days were all DetourModKit link errors (`error C3646 'log_level'`, `DetourModKit/Logger.h` not found, `LNK2001 __std_find_last_of_trivial_pos_1` / `_Cnd_timedwait_for_unchecked` = a lib built with a newer MSVC runtime than mine).
- **vcpkg**: V63.0 made it optional (CMakeLists "V63.0: vcpkg is optional. Reuse an installed OpenXR package if present"), the R22C1 era still built with `-DCMAKE_TOOLCHAIN_FILE=E:/backup/vcpkg/...` and `DETOURMODKIT_ROOT=E:\backup\DetourModKit`. today vcpkg is retired: nothing in the tree needs it.
- **DetourModKit pinned** under `deps/DetourModKit`: the static lib is stored as `lib/libDetourModKit.lib.zip` (117 MB lib, sha c01dfd67..., `COMPRESSED_LIBRARY.json`), `DEPENDENCY_MANIFEST.json` hashes every header, the build restores the exact bytes and checks the hash. variant "reviewed adapter capacity128": `CAPACITY128.patch` = `MID_ADAPTER_CAPACITY 64 -> 128` in mid_hook_adapter.hpp (we ran out of mid-hook slots). override only via `DetourModKit_DIR`, a generic CMAKE_PREFIX_PATH does NOT override the pinned package. do not swap in a similarly named lib from an old build (the Aug 30 errors again).
- **OpenXR**: `FetchContent` of OpenXR-SDK-Source release-1.1.62, or `build_release.bat -OpenXrSdkSource E:\deps\OpenXR-SDK-Source`; `KCD2VR_SINGLE_BINARY` links the static loader. `tests/openxr_sdk/` carries the headers for host tests.
- **`build_release.bat`** -> `tools/build_release.ps1`, output `dist/prerelease`, never copies into the game, auto-shortens the build path, `-BuildDirectory`, `-OptionalGoreSource` (gore pak stays SEPARATE, never merged into the main pak). `VERIFY_BUILD.cmd` -> `tools/verify_release_payload.ps1`. `KCD2VR_PYTHON` env for the python.
- **proxy dinput8 (C29)**: `-DKCD2VR_PROXY_DINPUT8=ON` builds `src/proxy_dinput8_29.cpp/.def` as `dinput8.dll` (loads other *.asi itself, no asi loader). thats the file in Bin/Win64MasterMasterSteamPGO today. "a proxy filename alone does not remove the shared OpenXR loader dependency".
- **companion pak**: `tools/companion_sources_d39.py` `SOURCE_PAIRS` = the canonical member list (corpse_d72.lua, life_d73.lua, petting_d59.lua, alchemy_sound_d80.lua, combat tables, D68 glyph-only .gfx, kcd2vr_lab/zzkcd2vr_lab.lua ...) built from a verified C63 donor + overlays, so a new source edit cannot be silently left out of KCD2VR.pak; "299 source overlay members verified byte-for-byte". when a .lua changes, the pak must be reinstalled (R100/R101 readmes say so).
- **source archive**: `python tools/export_source.py --zip KCD2VR_SOURCE.zip` (no .git, no builds, no old PAK with the gore inside - that was why the archive had grown to 145 MB). `tools/check_subsystems.py` = 54 production TUs retained exactly once, disjoint frozen ownership (`subsystems.json`).
- **changes**: full replacement files in a folder, copied over my build folder, then build_release.bat. i cant apply .patch files.

---

### 6. IF YOU WANT TO FIX X, LOOK HERE

- hand jumps when touching an item/npc -> `frik_pose_probe.cpp ContactHand6695`, `contact_balanced_d79.h`, D81 provenance rule; measure with F7 "offset step > 1 cm on plane frames".
- weapon too heavy / laggy -> `weapon_dynamics177.h` (authority, stats59c17, skillFactor26), INI `VrWeaponStatEffect`, `VrPoleOneHandFeel`, `VrWeaponFatigueRamp`; SIMULATE HENRY on the WEAPON FEEL page to A/B a save.
- hands/blade should push things -> `phys_bodies59c10r8` (mode 2 refused until clearance), `native_weapon_body_t4.cpp` state machine + `native_weapon_clearance_t4.cpp`, H24 `native_hand_proxy_owner60.inl`. first thing: a load re-arm that destroys old bodies, then the clearance boundary.
- corpse drag stretches / pushes me / won't grab -> `corpse_follow_r101.h` (numbers), `self_collision_r103.h` (bits), `saved_corpse_drag_d72.inl` + `zzkcd2vr_corpse_d72.lua` (why REFUSED), `lab58.inl` articulated route, owner `corpse_body_owner_d40.h`. F9 rows `F9_HAND_DRAG_D72`, `R101 F9_CORPSE_FOLLOW`, `R103 SELF_COLLISION`.
- impale hang -> `native_impale_part.inl` (servo), `impale_hold60.h` + `constraint_service60.h` (constraint, F9 lab 3 only), `impale_death_speed_r106.h`.
- props -> `static_grab_r89.inl` (AcquireR100/Carry/Release/Tick), `brush_swap_r100.h` rules (`choose`, `brushRule`, `cleanPath`), `prop_drive5916c1.h` servo, `CarryLimitsR103`.
- collision class bits -> `self_collision_native_r103.inl Pins()/Exclude()/Restore()`, packet = type-26 SetParams, +0xEC type / +0xF0 ignore, living = bit 4.
- jitter -> read 2.3 first, then `tracked_frame51.h` (anchor), `tracking_sample59c6.h` / `tracking59c6` (families, epochs), `hand_same_frame_hj1.h`, `smooth_lab_f2.h` (rungs), `motion_trials_d67.h` (mode 2/4), `early_frame_pacing_d45.inl`. F7 + F2 rung 1 and compare raw -> requested -> finalGoal -> committed per sample.
- fps -> `hook_cost178r22.h` slots (F2), `performance_gate_d61.h` (Shift+F2), the idle T4 callback (`native_physics_phase_t4` WorldQualified x20), companion timers (life_d73 250 ms, sheath 1 s), map `MAP_TICK`. compare SAME scene, recorders OFF.
- roomscale -> `subsystems/roomscale/pose_producer_d76.inl` (delta), `native_roomscale_d76.cpp` (Step join), `pad_dispatch_d84.h` (coherent pad), `roomscale58.h` is the legacy servo.
- recorder/log -> `capture168.h` (domains, leases, receipt grace), `diagnostic_trace.cpp` (Write/WriteStatus/Drain, `VR status:`), `visible_failure.h` (R90 F2-only screen rule), `physics_stage_d42.h`, `reload_capture_d84.h`.
- a feature stays off until restart -> `load_rearm_r96.h` + `RearmAfterLoadR96` list, or make it a `load_fault_recovery_r96::Fault`. one log line naming what came back.
- build fails on DetourModKit -> `deps/DetourModKit/COMPRESSED_LIBRARY.json` hash, `DetourModKit_DIR`, CAPACITY128.patch; never an old lib from E:\backup.
- "is it tested?" -> this doc says HEADSET NOT RUN or i tested, then my log. host checks and headset checks are kept separate on purpose.


---

## 5. MELEE COMBAT

the biggest and most painful area of the mod. the goal never changed since september: a real VR weapon in the hand (weight, drag, collision), the blade decides WHEN and WHERE the hit lands, KCD2 decides everything else (block, perfect block, masterstrike, armour, damage, stamina, death, crime). blade & sorcery x PLANCK on a mannequin engine where NPCs are animation driven and ragdoll only on death. a lot of dead ends. read this before touching physical_weapon.h, geometry_hit178r27.inl, native_combat104.inl or the gore / masterstrike modules.

version tags in the comments: V66.1xx (sep/early oct), V66.178R1..R59 (8-11 oct), R59.16C.. (12-19 oct, C rounds), H.., D40..D87 (20 oct - 3 nov, D rounds), R88..R106 (3-5 nov). WHGame.dll = timestamp 0x6A350E20, every native address below is for that dll.

#### the short map of what runs today (R106)

- weapon = rigid body with mass/inertia driven to the controller by a force/torque limited PD drive (`kcd2vr::physical::step` in src/vrik/physical_weapon.h, dynamics in src/vrik/weapon_dynamics177.h). collision = sparse sweep against world + NPC bodies (npc_contact172 witness ray), contact record `ImpactRecord178r30`.
- the stroke recogniser (src/vrik/melee_stroke118.h: cut / thrust / pommel) turns blade motion into a NATIVE attack: prepare at wind-up, release at commit, zone from the stroke (stroke_zone117 / blade_zone_d84). the game runs a real attack action, so the NPC AI sees a telegraph and can block.
- the HIT is ours: when the blade touches the enemy the contact goes to the LIVE attack action's own collision callback `C5235C` (route `HYBRID_NATIVE_LIVE`, src/diagnostics/geometry_hit178r27.inl `LiveAttackAction178r35`) the moment the native strike phase 8 opens. if the window is not open 70 ms after the contact (`g_sync_grace_ms59c10r9`) the same contact goes ONCE through `C_CombatRPG::BuildAndSubmitMeleeHit` 0xD568B0 + the outer continuation (route `DIRECT_NATIVE_CONTINUATION`). one producer per stroke: henry's canned weapon sweep is refused in the ProcessCollisionHit detour (`NATIVE_SWEEP_REJECTED`). D83 audit of a D81R1 log: 64 native results, exactly 32/32 between the two routes.
- F4 is RETIRED as a cycle ("F4=RETIRED_HYBRID_IS_GAMEPLAY" in the mod.cpp hotkey line). calibrated gameplay = `Mode::CombatSynchronized` (`g_hybrid_default59`, C59). F3 = combat capture recorder. the old rung names still live in src/vrik/user_modes175.h because the log rows print them - don't renumber, the native tuples {3,2,4,7,..} are what the code reads.
- menu: EXPERIMENTS combat page (weight slider, skill feel, NPC COMBAT SETTINGS, COMBAT FLOW, MORE COMBAT TUNING, COMBAT TUNING 2 > WEAPON FEEL), Experiments tab > COMBAT page 2 (kCombatTrialsD70) and HANDS page 2 (weapon kicks, impale fast death). code: src/subsystems/hybrid/menu.inl, direct/menu.inl, masterstrike_combos/menu.inl, experimental/live_settings_d39.h.

#### 1. the physical weapon (weight, drag, two hands, flicker)

what i wanted: the weapon trails the hand a bit by its weight and henry's strength/agility, NOT the "20 kg log" of the early builds; two hands = stronger and more controlled; axes/maces head heavy; no flicker.

1. early V66 (sep): low-pass filtered weapon. DEAD END: it lagged EVERY motion, slow precise ones too = the log feel. rule since then: lag only when the requested acceleration exceeds what the arm can do.
2. V66.129 HIGGS style "constraint drive" (`constraintDrive129` branch): bounded authority, gains 42/s lin, 34/s ang, caps 5.5 cm / 5 deg. no mass, no stats. still the hybrid-mode drive today when `physical177` is off.
3. V66.177 `weapon_dynamics177.h`: a Body per class (sabre 1.2 kg, longsword 1.4, axe 1.8, mace 1.5, halberd 3.0, x sqrt(inventory weight / class reference)), com fraction .20 sword / .65 axe-mace, grip inertia, PD drive with feed forward, 180 N / 35 Nm times strength/agility/stamina (`authority()`), x1.7 force / x1.8 torque with the second hand, gravity. contact energy from the effective mass at the contact point. this is still the core.
4. R44: 180 N on 1.4 kg = 130 m/s^2 = "no weight". `VrWeaponWeightFeel` scales the caps (0.6 then, 0.85 now) = the "Weapon weight and drag (lower = heavier)" slider -> `in.weaponAuthorityScale178r44` -> `fatiguedAuthority59c10r2`.
5. C7B / C10R2: exhaustion divisor (x5 heavier at <= 15 % stamina) + compliance envelope `compliance59c10r2` (5.5 cm / 8 deg one hand, 3 cm / 6 deg two hands; without it the limit was invisible). C10R14 `handling59c14`: one-handed halberd = "10-pole stick" (.40 authority, envelope x2.5), axe/mace .85/1.25; `VrWeaponFatigueRamp` 0.6. C10R17 `statAuthority59c17`: strength/agility vs the WEAPON's own StrReq/AgiReq (+0x118/+0x11C) in the game's +-10 window (`VrWeaponStatEffect`); "Skill feel real/low/medium/high" fakes levels.
6. H21 cancelled the two-hand leverage AND widened lag x1.6 on top of real inertia = heavy polearm lag + springy rebound. D86 removed the double penalty: support = {1,1}, rebound critically damped (60 vs 27). DEAD END: counting two-hand heft twice.
7. R90 (3 nov): on that kernel a fast two-hand stop still overshot 1-2.5 deg (4-8 cm at the tip, halberd 12 rad/s) - the rotation servo kept its motor speed while the envelope dragged the blade. brake `brakeAngularR90` (wanted turn speed <= sqrt(2*authority*error)) -> 0 overshoot, tests/two_hand_brake_r90_test.cpp 18/18. option "Two-hand weight feel" (Experiments > GEAR, `VrTwoHandWeightFeelR90`, default OFF): ON = the weight slider also slows two-hand turns.
8. R98 flicker ("sword comes from behind" while walking): (a) in NATIVE body height mode the whole frame bobs ~8 cm 3x/s with the walk; the weapon took the shift only from `native_stance_delta_r93()` (zero unless both frames use HMD height) so the hands moved with the bob and the weapon chased them. fix `shared_frame_delta_r98()` (tracked_frame51.h) at physical_weapon.h:370/:1470. (b) when the next body pose was still being published the weapon stayed at its OLD world while the hands were at the new one -> one frame lag + snap. fix `rebaseR98` in src/vrik/physical_native.inl:807-847 (same hand-relative pose re-placed at the current entity world, counter `rebaseR98=` in the authority row). no host test covers it, not headset proven.
9. R103 `self_collision_r103.h`: held props / dragged corpse pushed henry away (prop servo into his living capsule, 40 N). fix = retail collision class bits (CPhysicalEntity +0xEC type / +0xF0 ignore, setter 850AA2): prop ignore|=living + type|=living, corpse ignore|=living, restored 0.6 m / 1.0 m away or 4 s after release. the weapon body already ignored living (native_weapon_body_t4.cpp).

what works now: `physical::step()` per side: body177 from `dynamics177::body()` (+ impale hang load), authority -> `fatiguedAuthority59c10r2` -> `classAuthority59c14`, `dynamics177::advance()` in <= 4 ms sub steps (physical_weapon.h ~2247-2290), recoil spring 900/s^2 on contact. hybrid weapons use the V129 branch with handBoost 1.75 + the R90 brake. INI: VrWeaponWeightFeel 0.85, VrWeaponFatigueRamp 0.6, VrWeaponStatEffect, VrTwoHandWeightFeelR90 false. open: black frames after a clinch (DLSS reset, not combat), hip sword drawn 0.4-0.67 m off in native crouch (R98 log).

#### 2. how a VR hit becomes a game hit (the route history - read this first)

1. V66.120..164R4: "F12=7 predictor redirect": the game's own weapon sweep (A7A124) was fed the tracked blade trajectory. 7 oct V66.164R4 i confirmed wall/cart sparks at the real strike point and an NPC kill. but the hit was animation bound: the NPC reacted when the ANIMATION would have hit, 0.4-1.0 s after the blade passed.
2. V66.174 body ray witness (pinned 0x50 ray_hit: collider +8, partid +0x14, point +0x24, normal +0x30, later surface_idx +0x18 / idmatOrg +0x1A). V66.176 `contact_authority176.h` ticket gate refused every sweep contact without a fresh physical release -> `nativeCalled=0` on 100 % of sweeps all week. DEAD END; R26 made `VrF4ContactGate=0` (pass-through) the default, still 0.
3. V66.177: physical weapon movement into the A7C624 geometry detour, one Process per release; found the native energy curve 2E-E^2 (`AttackEnergyModifier` 2.0) and that the ordinary energy provider reads native state, not our velocity.
4. R8 moved the attack transport to a virtual gamepad (XI) because the direct handler call "cost 69-140 ms"; R23 proved the cost was the F3 combo capture inside the handler; the XI route added 100-200 ms so prepares came after the swing. R26 `VrAttackTransport=0` (direct). DEAD END: XI for strikes (kept for the R3 hold).
5. R27 rung 6 "geometry hits": no native attack, the impact IS the hit - direct `BuildAndSubmitMeleeHit` in henry's `C_CombatActor::Update`, request shaped like the game's synthetic automation producer (0xC60F94). R29 log: 9/9 returned=1 with damage 8.9..23.6 but `henry{inCombat=0}` - no fight, no reaction. R29 auto R3 to engage -> DEAD END: without a fight R3 is toggle_crouch, henry crouched and dropped the sword ("after stabbing henry goes in the crouch and loses his weapon"). `VrGeometryAutoLock=false` since R31.
6. R31 first swing native to open the fight. R33 (01:14 log): 44 hits, enemy dies STANDING - the ordinary caller C5235C continues after D568B0 (`D499D8` victim reaction via target+0x3A8, `D542E0` listeners), none of it runs for a direct call -> outer continuation added. R34 telegraph fields mirrored so the AI could block. R35 log: reaction CREATED, still nothing visible. CONCLUSION: a direct call is NOT the native hit lifecycle; it exists only while henry's own attack action is alive. DEAD END as the main route (`VrGeometryDirectFallback=false`, `VrGeometryTelegraph=false`); the direct code lives on as the SYNC fallback.
7. R35-R37 THE LIVE ACTION ROUTE: an attack (D4CB30) registers {action, C5235C} at +0x28 of the sweep component actor+0x218 (count-prefixed DynArray, R37 fixed R36's wrong container read). call `C5235C(action, details)` with the blade contact; details layout from 40 HIT_TEMPLATE dumps (+0 point, +0x24 entity, +0x64/+0x6C surface, +0x70/+0x80 part, +0x7C coef, +0x88 = 0x00040012, +0xC8 target actor, +0xD0 attacker).
8. R39 log: handler REFUSED all 6 (`NOT_REACHED`): first check `72EFDC(actor,8)` = phase must be 8 (strike window); the blade arrives 100-250 ms after commit, mid wind-up; 120/128 contacts had no action yet. R39 HOLDS contacts until phase 8 (`VrGeometryHoldMs` 600). R40 one producer per stroke + contact bound to its action (`ACTION_REPLACED`). R41 log 13:01: THE FIRST VR CONTACT WITH A NATIVE HIT AND NATIVE COST (5/6 reached the builder, 53/36/39 damage, one parryInPlace, stamina 30-40). `VrContactStartsAttack` (R41) is OFF today.
9. R46 "contact-time admission" spoofed the phase payload to 8 - RETIRED in R46C1 (no setter contract). DEAD END: phase spoofing. R50C2 motion attack clock: the native action clock advances from the BLADE, so the window opens when the blade is there. R55 (02:28, first "working" log): matchedStarts 10/10, `GEOM_LIVE_HIT route=C5235C dmg 2.6..15.5`. R55 "thrust is a swing": the native stab (attack_light zone 0/5) never made a sound in any log; sent as attack_heavy from the origin zone it lands (damage type slash instead of stab = known loss).
10. C10R9 (13 oct, my call: "rung 3 = 1+2") SYNCHRONIZED: native admission/defense/combos/clock of rung 1, but the VR contact has authority: window not open 70 ms after the ORIGINAL contact, or no action although the release was admitted -> the same contact once through the direct hit + outer continuation, same serial. C10R16 VR INTENT: direct at once with energy scaled strength 30..120 %. C10R12 `g_sync_overdue_any59c12`. C19C2: the guard pump erased our own guard witness so the next attack was refused as foreign phase 128/256 - fixed. C59: synchronized hybrid = the baseline, later F4 retired.
11. 19 oct (70 gestures): 40 swings at an NPC, 26 landed. 46 contacts dropped `NO_PHYSICAL_GESTURE`: blade touched while the recogniser was not active (wrist flick tipSpeed 7.8 / handSpeed 1.3 -> REARM, or tail of a swing); the impact record carries a gesture id only while `s.stroke118` is active (physical_weapon.h ~1485), Submit refuses gesture 0. `VrHitLateGesture` (mint a gesture from the impact at >= 3 m/s) was NOT built; H8 `committed_stroke_h8.inl` keeps a bounded same-gesture carry-over through the recogniser ending instead.
12. D55 (29 oct) thrusts: COMMITTED is not a hit - 34 commits, 17 native results (10 health, 4 stamina, 2 perfect block), 3 retired by `NATIVE_BODY_REQUIRED_LOCKED_ACTION`, 6 WITHDRAWN (axis turned > cos 0.6 from the saved axis or 1 cm retreat). thresholds: axial handSpeed >= 1.0/stabSensitivity, blade cosine 0.80 in both frames, onset 1.25 m/s, travel 15 cm (heavy x1.3), prepare after 6 cm / 30 ms. D77 `StabFixD77` keeps a committed stab while both tips advance. D82 timing repair; D83: `VrIntentLiveWaitMs` 60->120 was refused on 15/79 unmatched gestures.

what works now: producer = the GEOM block in frik_pose_probe.cpp (~4132, `GEOM_IMPACT side= speed= energy= kind= entity= part=`), `impact_feature_d72` (tip = within 6 cm of the point, blade = 25..100 % along the axis, pommel = 6 cm of the butt), min body contact 1.5 m/s. queue/routes = geometry_hit178r27.inl (geom27) + src/subsystems/hybrid/*.inl (route_state, drain_live_delivery, sweep_owner) + src/subsystems/direct/*.inl (native_state_context = VR state driver). results = `H29 STRIKE_RESULT`, `C54R3 HIT_QUEUE`, `C7G CONTACT_RECIPE`, the `contact58c1::Receipt` (src/vrik/contact_receipt58c1.h: blocked/perfect/parry/master/death/combo/damage/stamina/point/feature/energy/thrustSpeed96/thrustTipDepth96). native input = native_input117.inl (`PhysCallAction6698`), native_combat104.inl (guard tick, native_admission147.h), native_bridges168.inl (R3). INI: VrGeometryLiveAction true, VrGeometryHoldMs 600, VrGeometryStrikePhase 8, VrGeometryRejectNativeSweep true, VrGeometryOuterContinuation true, VrGeometryDirectFallback false, VrAttackTransport 0, VrF4ContactGate 0, VrTargetRepeatMs 150, VrDirectOwnsHits true, VrNativeHitTime 0.3 (COMBAT FLOW slider), VrInstantFreeSwing. the repeat guard is the game's `wh_cs_RepeatHitTimeout` 1.5 s per target (ProcessCollisionHit 0xD53610 map at this+0x10, key details+0x24) - never lower it, it stops double damage from one swing.

#### 3. strike energy, damage from contact, min interval

- recogniser score `k.scale = clamp(sqrt(E/Eref)*weaponSensitivity, 0, 1.25)` (normal_intent_phase_h30.inl:633-668); `VrStrikeMinScale` 0.35, `VrStrikeMinScaleGate=false` = a TOO_WEAK stroke is logged, not vetoed. D66 review: every captured group had cuts above 1.00, so "you never swung hard enough" was never the gore problem.
- native contact strength (`power59c8::Compute`): joules / `VrStrikeReferenceJ` 75 ("energy of YOUR full stroke = 100 %"), then .30+.70*ratio, two-handed x1.25 inside `VrStrikeMaxPower` 1.1, min 0.30, `VrStrikeRepeatMs` 2000. this reaches native damage ONLY on the direct route (strength argument of D568B0). the LIVE route takes the native curve (state+0x420 sampled against the row timings scaled by the motion clock): R56 log `energy=0.27 -> dmg=15.45`, `energy=1.00 -> dmg=2.64`. so "damage scaled by energy" (C10R16) is only true for direct deliveries. OPEN - R56 §3e (write the sampled strength from energy at admission) was never done. fists: the game forces energy 1.0 (predicate 0xA7556C, class 12).
- `VrStrikeMinIntervalMs` 0 today. `VR_HIT_TOO_SOON` 150 ms (`VrTargetRepeatMs`) refused 2 hits in the 19 oct log; "let it through at 100 ms when the previous was blocked" not built.
- "contact damage effects" (the V66.177 name) = the material effect at the contact. R12 `contact_fx178r12.inl` replays the combat dispatcher +B24A0C (CMaterialEffects vtable+0x30 row x surface -> effect, +0x50 execute) for accepted strike contacts: 53 rows `hit_passed|stopped_<sword|axe|mace|halberd>_<slash|stab>_player` (stone sparks, wood chips, fabric/chainmail/plate incl. blood), one per strike >= 150 ms apart, `VrContactEffects=1`. with the live route the game plays its own effects (D55414) for admitted hits; R12 covers non-attack impacts + the C49 double blood.

#### 4. guard, block, perfect block, masterstrike, combos

guard (positional, no LT):
1. V66.176 `guard_parry176.h`: blade in a cone in front of the chest = native block pressed. R29: entry needs blade slower than `VrGuardEnterMaxSpeed` (1.2 then, 2.5 now) for `VrGuardEnterDwellMs` (80 -> 0) because every swing through the cone pressed block and a block decision cancelled a prepared attack.
2. R45 (R44C1 log): THE BLOCK THAT WAS NEVER HELD. native went 1 -> 256 one tick after each press, 256 was not in `native_admission147::guardWritable`, so the tick released and re-pressed at 50 Hz (327 PRESS / 318 RELEASE 10 ms apart) - no block ever stood > 10 ms, enemies ran combo after combo. R45 admits 256 as held block; permissive box (half width 42/50 cm, forward 0..1.0 m, height -42..+58, grip within 1.05 m, blade <= 28/22 deg off the opponent line); `VrGuardExitSpeed` 3.5 m/s (a cut starting in the box is an attack); `VrGuardAfterAttackMs` 400; one assisted block edge per incoming attack episode (`VrGuardIncomingRepress`, phase mask 12, cooldown 120 ms) - native timing decides normal/perfect.
3. C10R17 `VrGuardStrictEntry` (COMBAT FLOW): blade midpoint >= 12 cm in front of the chest, tip <= 10 cm behind (a sword cocked over the shoulder is not a guard; 53 % of guard holds were < 100 ms). `VrGuardRequireCounterMotion` (PARRY INPUT: COUNTER-MOTION vs WINDOW ASSIST). C19C2: the raised off-hand shield no longer cancels the primary strike. D55: guard box forward 0.10..0.60 m from the head (faceKnown), no forward-depth slider exists - width/height can't fix "block too far in front".
4. perfect block: native = block inside the slot AND weapon inside `wh_cs_PerfectBlock_SpatialVolumeRadius` 0.10 m of the canned block point. a tracked blade is never inside 10 cm = zero player PBs in every log until R56 leased the radius (0.35, now 0.6 in the shared cvar lease mod.cpp:1008 with `wh_cs_BlockCooldown 0.05`, shared rules opt-in). R41 rules pak MaxPerfectBlockSlotModifier -> R42 RPG registry hook (+0x6DD3D0) -> today `VrPlayerCombatWindowScaleH26` 1.5 (max 2, henry only). "PERFECT BLOCK: MINIMAL NATIVE COOLDOWN (1 ms)" = BlockCooldownLease59c17. 19 oct H3 log: 132 incoming / 36 blocks / 0 PB; a 17:09 run had 23 blocks, 12 perfect - so player PBs happen, not reliably. enemy side in the 19 oct log: 15/15 blocks were PERFECT parries - our rpg_param pak halved the guard-follow delay and cut `CombatAutoTrickReactionMaxDelay` 8->3; the T1 pak restored vanilla. NPC COMBAT SETTINGS page: difficulty native..C8, `VrNpcBlockSpeed`, unarmed spacing.

masterstrike:
1. retail (all from bytes, C47): `C_CombatTriggerMasterStrike` opens the window = count at state+0x7D8 (909FC4/909FF4), only with perk ability 73 (908DA0 kind 5 -> 909B4C sword family 1/2/4/16 -> HasSoulAbility). press = D4AD94 `cmp [rax+7D8],0 / jg 22B780B` -> 1483540 -> factory 1483A70 -> paired action (55/56, death 62/63) via 90C850. zone rule `master_strike_combat_zone_id` 0->5, 1->2, 2->1, 5->0 (opposite side).
2. R59 `VrMasterStrikeAllWeapons`: mid-hook +908E09 flips the family answer for henry with axe/mace/flail/halberd (3611's KCSE recipe), companion rows 26+63 + 80 fragments.
3. R14/C23 `VrMasterStrikeVeto=1` cleared AL at 1483A90 -> factory empty but D4AD94 still returned 1 = the press was EATEN (no master AND no ordinary attack). DEAD END, 0 now. C47 deny point at the cold block 22B780B (13 pinned bytes) -> RIP = D4ADCC = the retail closed-window path. `VrMasterStrikeStabOnly` 1 = allowed only for a THRUST with energy >= `VrMasterStrikeStabEnergy` 0.30 from the opposite zone of his executed zone (state+0x278); 2 = never the paired clip; 0 = no gate. COMBAT FLOW: KEEP THE STANCE (`VrMasterStrikeKeepStance`), hand-over speed `VrMasterHandoffForwardSpeed` 3.5.
4. 19 oct audit of 50 logs: henry native masterstrikes only 4 in the V66.135R1 observation-only run (VRIK off), then 1 per log at best. windows are mostly 10-16 ms (one guard-pump sample), our later commit press is almost never inside `+7D8 > 0`; DIRECT refuses by design (`NATIVE_MASTER_IS_PAIRED_SYNC_ACTION`). my "our outcome instead of the paired clip" (stab = shove/impale, cut on neck/limb = gore) exists only in the c58/c61a lineage (`VrMasterStrikeStabOnly=2` + DIRECT COUNTER STAB `counter_stab40.h`), not in the main line.
5. "masterstrike without delayed npc reaction" (D62C..D84): the hit of a paired clip is authored at 0.47-3.2 s (median 1.27), 42/90 master clips hit through the fragment procedure. D62C raised the pair's IAction speed bias +0x5C; D63 proved the pair join was absent; D72's 64x admitted-pair acceleration was removed in D84 ("a proved pair keeps native 1x while damage/contact ownership is unresolved"). `ms_reaction_owner_d62c.h/.inl`, `native_pair_d72.h`. STILL OPEN. D83R1 "NATIVE + HEAD TURNS" (`VrFinisherUpperBodyPlayerD83` / `NpcD83`, src/vrik/finisher_upper_d83.*): head/neck/spine2-3 follow the headset during the paired clip (45 deg yaw / 30 swing, shares 10/25/60/100 %), default ON - i had switched it OFF myself in the menu (R88).

combos:
1. why they never worked (C10R17): the combo manager (actor+0x3B0) advances on henry's hit flag state+0x480 (the direct hit never set it - fixed via 72FA34); phase observer 911250 RESETS the combo when phase returns to 1/2; trigger 91115C opens the slot (+0x860) at phase 16 for 0.25-0.5 s. a VR swing needs 1.2-1.3 s commit to commit; 19 oct: 0 of 33 landed hits had the next swing inside 0.5 s, 23 inside 3 s.
2. `native_combo_grace.inl` (combo59c17): defers the reset and re-arms the close timer (9D6A34) to `VrComboWindowMs` 1500 (COMBAT FLOW "EXTEND" + "Combo time"). `VrComboZoneAssist` announces the step-2 zone at PREPARE (`combo_zone_assist_cz.h`). D84 `VrBladeZoneD84` (side from where the blade swings, not where the hand was held; originMatch 2/4 in the D85 log, left OFF), `UpCutLowD84`, `StabStartD84`.
3. R88 (D85R5 logs): 0 of 56 results combo=1, 70 native STEP_FILTER all on step 1. the step-2 press is SWALLOWED: our CB3 keep leaves the slot payload while henry is already idle (phase 2), D4AD94 routes the press to C5AC2C which only chains from a RUNNING attack -> no attack starts, 3 s later RESET. same in D81R1 7/7, D66 3/3, D62C 7/7.
4. R14 "COMBO HELP: HOLD" (`combo_recovery_hold_r14.h`, `VrComboRecoveryHold`, 1000 ms): while the slot is open and no press yet, henry's attack clock stops in the last frames of phase 16 (dt nets 0) so the press lands on a live action. R94/R96: only a MANUAL guard ends the hold (our auto posture guard ended it), phase-32 tail holdable, auto guard stays down while the slot is open. MY INI HAS IT OFF (42x `R14 COMBO_HOLD_SUMMARY enabled=0`), the forced migration was refused - turn it ON in COMBAT FLOW. not headset proven with the R94 fixes.
5. kill combos / finishers: `finisher_cb1.h` `VrPhysicalComboFinisher` (the physical kind of the release Cut/Thrust/Pommel is published before the native release, read by the 9397CC gate), `final_combo_contact_d72.h`, `FinisherCandidateD41`, `InstantCombatD72`. exactly one recorded combo=1 result ever (D81R1 00:56:49, 36.9 dmg, comboEnd=0); no completed finisher in any log.

#### 5. kicks (R3)

WE NEED THE KICKS. a visible kick with every weapon, in and out of combat.

1. V66.162/168 press+release of `attack_special`: no kick clip in any log. V66.171R2 "R3 kick everywhere" listed as impossible that way (armed special = back-off / mercy).
2. R10: defaultProfile.xml says `attack_special` in `combat_locked` = LT HELD 0.25 s; every earlier route sent press+release 0 ms apart, never the hold. `VrKickTransport` 1 = XI LT hold 420 ms, 2 = input-mod LB, 3 = call the player input handler +0x8E5324 directly press(1)->hold(4)->release(2) (R17b, default today, ~69 ms per call). R22b log: R3 works unarmed.
3. data: native kick = attack_type 4 rows (8, `kick+attack_heavy`, fullbody, `r_noweapon`); armed special = 17 input-class-2 rows, all action 50 mercyAttack. R59 `Kick59` armed kick as a companion row at the unused zone 3 (replaced later).
4. D72 `dedicatedCombatKickD72`: companion input-class-2 COMBAT kick rows (zone 5) for ranged/shield/torch loadouts, R3 -> SPECIAL. D84: the free kick waited on the block modifier instead of the game's FreeGuard (guard type 23) -> `NativeFreeKickContinueD84` reads the real guard. D85 2 s cooldown + no-target route guessed from the guard. 3 nov: "kicks are now FAR WORSE". R88: the cooldown started after ANY press+release incl. a stab ("wait 2 s" x3) and the no-target route guessed; fixed (cooldown only after an observed native type 4, `kick_route_d85::onTerminal`; exact context getter 1315CF8 `NativeKickCombatContextR88`, 22 pinned bytes).
5. R96 weapon kicks vs enemies: with a melee weapon the combat route pressed HEAVY zone 5 = the retail low STAB (the "kick that was really a stab"). +12 rows / +12 fragments (`tools/generate_combat_kicks_r94.py`, companion combat_action_attack.xml:290-301 + kcd_male_combat_generated.adb) for sword/sabre/longsword/class16/axe/mace/class6 (+shield) and 2H longsword/polearm, `meleeCombatKickR94` -> SPECIAL. switch "Weapon kicks vs enemies" (`VrWeaponKickR96`, HANDS page 2, ON), OFF = heavy press. all 42 action-3 class-2 rows are kicks at zone 5, no retail attack uses class 2. risk: the game's own LB / LT-hold binding with a melee weapon at zone 5 could pick a kick. NEEDS THE REBUILT KCD2VR.pak. test 313/313.
6. R99 (R97 log): R3 without an enemy refused 8/8 `INPUT_NOT_WRITABLE`: our own modifier press put henry into phase 256 zone -1 and `guardWritable` (1/2/128) / `blockMetadataH14` (128/256 only with zone 7) refused it; the D84 mock left phase 1 so the test never caught it. fix `kickInputReadyR99` (native_admission147.h:40-53), FreeGuard wait 120 -> 300 ms. test_free_kick_ack_d84 69/69. NOT headset proven.
7. the visible leg: `kick26::Style` 0 GAME KICK (native clip, legs native), 1 VR LEG + GAME RULES (procedural `kick176` leg on top), 2 VR LEG ONLY. `kick52` native timing from the real row (1.1667 s, time_to_hit 0.7333, animevents whoosh 0.42-0.50). INI VrKickStyle 0, VrKickLeg 2, VrKickReachMs 730, VrKickFreesImpale true. COMBAT FLOW > KICKS. CL-E: a refused kick says why ("kick: refused - probably no stamina") only while F2 records. `kick_hit34.h` foot contact ledger = `AdmissionUnproven`, damage from the procedural leg was never built.

1-minute check (F3): no enemy, empty hands, R3 x3 -> `R3_NATIVE ... gate=NONE ... observedAttackType=4 ... freeGuard=23 route=FREE_GUARD`, `C26 KICK nativeClip=..._kick_nwpn`. bandit + longsword -> `route=COMBAT_DOWN5 hold=-3`. `U16 state=REFUSED` = the 600 ms ingress throttle, not a bug.

#### 6. clinch

native: guard type 4 `clinch` (sync_guard), resolved by a sync_attack (backoff/kick/punch/hook/throw) chosen by the winner; the player's window is `timeToClinchAction` with `attack_primary`, odds from `ClinchBackOffDmgRWeight` / `SkillWeight`. R55 log: my "weapon floating above me / lost tracking" was a 30 s clinch - VRIK handed to the native body, camera off that body's head, i never sent the input (DEEP_DIVE_HYBRID_COMBAT_V66.178R56.md §1). today: `clinch_intent_d40.h` Mode Distance / ContactGrip / Disabled (Experiments NPCS page "Player clinch initiation", `clinch_distance32.inl`), `UnarmedFixD77` 30 cm. the clinch body stays a native paired clip. the two-hand shove to WIN a clinch (R56 §4.2) was never built.

#### 7. free targeting, auto-turn veto, attack step

- the problem: henry snaps body/camera to the locked enemy (queued "turn self toward opponent" command, builder 0x14097F0, caller 0x14097E2) and steps forward in attacks (alignment query 9E37A8 / mover 14095E8).
- cvars: `VrCombatAutoLock` 1, `VrCombatLockDisabled` 0 (`wh_cs_PlayerLockDisabled` off: kicks need a lock), R57 "Responsive Targeting" lease (mod.cpp ~995: lock/unlock 8 m, tolerance 20, area 0.6, `VrTargetSwitchDwell` 0.4 s look-away time, Immersion slider).
- D40 `CombatNoStepD40` (alignment answers "no step" since R55, `noStepSkips55`) and `CombatNoTurnD40` (`NativeRotationDispatchD40` in src/subsystems/base/native_motion_owner.inl) only in attack phases 4/8/16 with a live ordinary slash/thrust/punch action - masterstrike/kick/clinch rows stay native. D42 `FreeTargetingD42` switch.
- R103 (R101 log): "free targeting is not working as free really, writing ON / PAUSED". (a) the label lied: `freeTargetingContextD42` is false whenever the menu is open -> always PAUSED; now OFF / ON (WORKS IN COMBAT) / ON ACTIVE / ON NOT READY. (b) `CONTACT_IDENTITY:102` = NPC managers turning toward henry (global builder), not hit protection, must stay. (c) `PHASE:15` was the bug: in guard 1/2, after-strike 32, block prepare 128, block 256 the native turn still ran, henry snapped whenever he was not swinging. `free_targeting_d42::guardPhase` + `guardType` (pending special keeps the turn), 0/unknown and 64 (his own stagger) native, paired actions excluded by the body census. tools/run_combat_no_turn_d40.py 213 checks on the real dll. NOT headset proven (R106: "tell me after this run"). row: `D42_F3_END FREE_TARGETING henryCalls= henryVetoed= keptPhases=0x..`.

#### 8. impale

what i wanted: a killing stab leaves him hanging on the blade, pull it out along the line, kick him off or push with the free hand; ~40 kg not 100; nearly instant.

1. R15 `VrImpaleLatch` (rung 5): an accepted thrust (scale >= 0.7) stopped by a living body pins the TIP; presentation only, release by 14 cm pull / 3 s.
2. C32 `VrImpaleHang` (gore_policy.h "i need only decapitations and impalements"): the dead body's ragdoll part follows the BLADE - one `pe_action_set_velocity` on the articulated entity per native frame (`gore32n`, src/subsystems/gore/native_gore.inl), only a native-dead victim (death=1 receipt), never a living actor. C40 `VrImpaleByArmour` (OFF): living STUCK display from stroke energy vs native armour (`impale40.h`). C44 `VrImpaleHangMassKg` 40: a quarter point mass on our weapon sim + target sag/droop (`gore32::hangSag`, physical_weapon.h ~2225); `VrImpaleKillEnergy` 0.3 filters pokes. C46 log "impalements were not working, nothing happened" -> C47 `impale47::pickStuckPoint`: the weapon side only latched when the CURRENT tip was within 12 cm of the native contact; the receipt comes 60-80 ms after the impact and the blade had moved 16-26 cm. now: within 12 cm = contact point, else the blade point nearest the contact 10..40 cm behind the tip, contact within 60 cm of the tip and 35 cm of the blade line. game thread waits 20 Hz up to `kCorpseWaitH18` 4500 ms for the death ragdoll (original collider must be the articulated root vft 3A48B30, foreign data type 2 = this entity, I10), weapon wait 4700, `impale_pending59.h` keeps the admitted contact through a short recovery. the D30 era (J27 00:22:10) is the one time it "worked partially": death starts, ragdoll at 1.5-3.0 s, blade out at 3.4 s = keep the blade in him and the grip held ~3 s.
3. CL-IMPALE60 `impale_hold60.h` + `impale_constraint_h21.cpp`: a real type-5 ball joint (ragdoll part <-> weapon body, 0x88 packet, AC0D18) instead of chasing; velocity hang is the fallback if the add is refused. R101 `corpse_follow_r101.h`: the body follows as one piece, no stretching.
4. R96/R97 KILLING BLOW REFUSED: the one killing stab in the R94R1 log (18:06:59 serial 11) said `NOT_A_CLEAN_BODY_THRUST` + `NO_PROVEN_PIERCING_TIP`. two gates refused EVERY real stab since D41 (28 oct): (a) `strike_feature_d41::canPierce` accepted only feature TIP (6 cm) but a real thrust reports the sample on the BLADE; (b) `VrImpaleKillEnergy` compared stroke joules / the full CUT reference (75 J) - a stab is 0.02-0.18 there, never 0.30, and it used the first touch (1.45 m/s) not the push (4.01 m/s 12 ms later, `EXACT_CONTACT_ALREADY_QUEUED`). fix: thrust power = fastest contact speed of the same gesture+target / 6 m/s (`noteThrust96`, same scale as the corpse stab), 0.30 = 1.8 m/s. R97 after the problem showed up (a thrust sliding past his flank would pin): BLADE pierces only on a sword-class thrust (0/1/2/4/16) whose TIP is >= `kBladeTipDepth96` 2 cm under the touched surface (`tipDepth96`, deepest sample, contact point + plane normal + stopped pose of the same physics step); no normal = tip only. the 18:06:59 kill: ~30 deg to the skin, tip 3-7 cm under -> impales. verdicts `C44 IMPALE_KILL ... feature= tipDepthCm= verdict=IMPALE_REQUESTED | BLADE_TOUCH_TIP_NOT_UNDER_SURFACE | TOO_WEAK_BELOW_VrImpaleKillEnergy`. tests impale_kill_r96 24/24. the thrust table only fills with VR hits on.
5. R106 "Impale: fast death" (`impale_death_speed_r106.h`, HANDS page 2, `VrImpaleFastDeathR106` ON): the game hands the body to ragdoll only after the death clip; while the servo waits for THIS victim his CActionController time scale (+0x9C) runs x3 (restored on pin / give-up / 5 s; only the admitted lethal-thrust victim, never henry). UNTESTED.
6. corpse stab `impale_corpse_r15.h` (`VrImpaleCorpse` true, `VrImpaleMinEnergy` 0.3 = speed/6) latches the same hang. an impale on a native masterstrike/finisher kill can't happen.

never fully proven in the headset since D30: a hang with the constraint hold. first thing with a new log: grep `C44 IMPALE_KILL`, `C47 IMPALE stage=`, `N2 IMPALE_CORPSE_STAB`, `R106 IMPALE_FAST_DEATH`.

#### 9. gore (decap, dismember, blood, the gore pak)

no runtime dismemberment exists in KCD2 (no mesh slicing; breakables only for rigid objects). everything is attachment hiding + prepared cut meshes in the separate adult pak `Mods\kcd2vr_gore` + Lua (`zzkcd2vr_gore.lua`) driven from the ASI through the deferred console Lua route. settings `VrDecapitation` true, `VrDismemberment`, `VrBloodyFights` true; Immersion page 2 "Gore" row cycles OFF -> BLOOD x2 -> BLOOD x2 + HEADS + LIMBS (R98).

1. 13 oct design: only on a native kill, cut not thrust, neck material from body_material2subpart.xml, energy gate, fail closed. C7E "DECAP1".
2. C43T: 10 "corpse" decaps on LIVING enemies (a living npc's limbs are articulated hit shapes too, he kept fighting headless). now only a GAME-killed npc (death=1 receipt, 16 slots, 1 h) counts as a corpse.
3. `VrDecapitationStyle` 0 HIDE HEAD, 1 LOOSE NECK (ragdoll head joint opened + kick, `decap_joint44.h`), 2 STUMP + FLYING HEAD (default): head parts hidden, meat stump + blood on the Neck bone, the victim's head skin on a rigid carrier pushed away (`VrDecapitationHeadImpulse` 20).
4. 19 oct root causes: THE PAK NEVER LOADED A MESH - Lua `CreateSkinAttachment` is (slot, name), no path; every v50..v59.1 call made an EMPTY attachment, the flying head moved but drew nothing. fix v60: 166 wrapper `kcd2vr_fly.cdf` files (male.chr/female.chr + one CA_SKIN head) via `LoadCharacter(1, wrapper)`. limbs never fired (living candidates only when the hit kills; dead path only head/neck; no visual route). decap too sensitive: INI MinPower/MinEnergy 0.6 vs code 0.9 + a wide "kill band" z 1.15..2.05 any material -> `VrDecapitationAllowBand` OFF (H18).
5. D56 eligibility `native_eligibility_d56.inl` asks the pak Lua `kcd2vr_gore_eligibility_d56` for an exact `MALE_HUMAN` token. D66 review: 440/440 replies `REFUSED_OR_UNKNOWN` in D62C/D63R1/D66 = the installed OLD pak had no D56 provider; energy was never the problem. D70G/D71G (31 oct): my stump cgf sat INSIDE the skull (z 1.532..1.620 vs neck opening 1.402) = the "strange rotated stump" photo; moved to the neck opening, scale 1.3, console tunables `KCD2VR_Gore.headStumpScale/Target/Pos`. in that log 4/4 flying heads spawned and moved 1.4-3.3 m; most neck cuts never START a decap because the hit part is not head/neck (`C60 DECAP_GATE verdict=MISS` hitModel z 1.27 = upper chest) and corpse cuts say `NO_EXACT_GESTURE`.
6. v62.38/39 -> v62.40 (2 nov): my repack with plain zip added 28-byte extra fields in the local headers, the game reads the offset from the central header 4 bytes early -> `ReadFile returned 15` in kcd.log, the gore script never loaded, every eligibility REFUSED. unzip -t said fine. v62.40 rewrote the headers raw. R94 pak: say() shows headset text only while F2 records. ALWAYS check kcd.log for "KCD2VR_GORE loaded version" and no "ReadFile returned 15" first.
7. limbs: `limb_cut_d43.inl`, DS-F23 material classifier (16/17 right forearm, 18 right hand, 23/24 left forearm, 25 left hand, 28/29 right calf, 30/31 right foot, 34/35 left calf, 36/37 left foot; horses reuse the ids = visual fallback for humans only), 2 s per-target dedupe, pak limbs = hand/foot cut variants only (491 skins), no arm/leg meshes. D56 gate `gore_cut_d56.h`: committed CUT, net >= .22 m, fast >= .30 m, total + transverse speed >= 2.5 m/s, transverse >= 65 %, power >= max(.90, VrDecapitationMinPower) - the .90 floor is hard, the ini 0.6 does nothing.
8. blood `blood_policy.h` C49 BLOODY FIGHTS: once per applied undefended hit by henry, the same MaterialEffects row again at the contact + the same native blood amount again on the struck zone (Lua `actor:AddBlood`, BloodMask zones), 4 s gush on kill moves only with a proven neck/wound anchor. the native hit blood is the game's (surface 116 -> blood row).

rows: `D56 GORE_ELIGIBILITY result=MALE_HUMAN`, `C60 DECAP_GATE`, `C43T DECAP_CORPSE`, `C50 GORE_EVENT`, kcd.log `KCD2VR_GORE FLY head spawned how=wrapper`. code: src/subsystems/gore/* (gore_policy.h rules, native_gore.inl calls, native_part_anchor.inl, native_impale_part.inl servo), src/vrik/gore32.h, gore_cut_d56.h, decap_joint44.h.

#### 10. NPC flinch, touch reactions, ragdoll

- R11 `npc_flinch178r11.inl` (`VrNpcFlinch=1`): on an accepted strike stopped by a living body, bend spine1/2/3 (30 % each) + neck (25 %) away from the blade for 350 ms through the same skinning detour the bow string uses, never henry, visual only. with the live route the game plays its own hit reaction (`CombatHitGen` 123 rows, median 1.67 s), so R11 only matters for contacts the game did not admit. the spring design for NPC hit flinch was never built.
- `NpcContactReactionD41` (reaction row from the accepted hit region), `VrNpcReactsStaminaHits` true, `touch_react43.inl` NPC TOUCH paused. ragdoll: R59 F9 cvar lease `ragdoll_lease59.h` (`VrRagdollImpactScale` 3, from the KCD1 "Semi Functional Ragdoll" mod). `wh_cs_HitReactions` / `wh_cs_PhysSim` never proven live. living NPCs are never forced to ragdoll.

#### 11. recorder and the "no permanent blockers" rule

- F3 = combat capture (3 min, `H29 STRIKE_RESULT`, `C23 OUTCOME_CHAIN`, `D42_F3`, `R3_NATIVE`). outside F3 `diagnostic_trace::Write` drops R3/kick rows (src/diagnostic_trace.cpp:126). F2 = the only rung that puts text in the headset (`visible_failure::screenAllowedR90`).
- R96/R97: 23 "off until restart" latches (drive/joint fault, contact fx, flinch, fists, native requests, finisher, motion clock, prediction, pose restore, physical weapons, VR hits, combo window/carrier, vrstate, swing stamina) re-arm once per COMPLETED save/level load (`core/load_rearm_r96.h` `Seen::take()` on the R96R1 `observed_load_recovery_r96` T8 episode; `RearmAfterLoadR96` in frik_pose_probe.cpp, `RearmCombatAfterLoadR96` in native_hit_probe134.inl). one line `R96 LOAD_REARM ...: <names>` only when something had latched.

#### 12. if you want to fix X, look here

- weapon weight/lag/overshoot: `dynamics177::body/authority/handling59c14`, the drive at physical_weapon.h ~2240-2320, VrWeaponWeightFeel + WEAPON FEEL page, VrTwoHandWeightFeelR90, two_hand_brake_r90_test.
- weapon flickers while walking: physical_native.inl `rebaseR98`, `shared_frame_delta_r98` (tracked_frame51.h), `VrBodyHeightModeR98`.
- a swing does not count: melee_stroke118.h thresholds (D55 list), `MELEE_INTENT reason=REARM/UNAVAILABLE/WITHDRAWN`, `GEOM_SKIP NO_PHYSICAL_GESTURE` (gesture 0 at impact), committed_stroke_h8.inl, VrStrikeMinScale.
- hit late / missing / double: geometry_hit178r27.inl + hybrid/route_state.inl (`g_hold_ms178r39`, `g_sync_grace_ms59c10r9`), `GEOM_LIVE_WAIT phase=`, `C54R3 HIT_QUEUE why=`, wh_cs_RepeatHitTimeout stays 1.5.
- damage does not follow the swing: only the DIRECT fallback carries our strength (power59c8); the LIVE route reads state+0x420 - R56 §3e not done.
- enemy blocks everything / no perfect blocks: companion rpg_param rows, NPC COMBAT SETTINGS, cvar lease mod.cpp ~995-1008, `VrPlayerCombatWindowScaleH26`, GUARD_SUMMARY / GUARD_INCOMING rows, native_admission147 (256 = held block).
- masterstrike: C47 deny point (22B780B -> D4ADCC), `VrMasterStrikeStabOnly`, `MS7D8_EDGE` rows (window length), perk 73; paired timing = ms_reaction_owner_d62c (native 1x, open).
- combos: native_combo_grace.inl (window), combo_recovery_hold_r14.h (HOLD - switch it ON), `C23 OUTCOME_CHAIN prepare=COMBO_SLOT release=UNSEEN` = the swallowed step 2, combo_zone_assist_cz.h, blade_zone_d84.h.
- kicks: native_bridges168.inl (`NativeFreeKickDispatchR24` / `NativeFreeKickContinueD84` / `kickInputReadyR99`), kick_route_d85.h, native_bridges168.h `dedicatedCombatKickD72` / `meleeCombatKickR94`, companion rows via tools/generate_combat_kicks_r94.py; `R3_NATIVE gate=` names the refusal.
- impale: strike_feature_d41.h (canPierce + tipDepth96), contact_receipt58c1.h (`strongEnough44`, thrust table), impale47.h, native_impale_part.inl (servo, hold60, R101 follow), impale_death_speed_r106.h.
- gore: check the pak in kcd.log first, then native_eligibility_d56.inl, gore_cut_d56.h floor .90, `decapZone47` band, limb_cut_d43.inl; pak Lua zzkcd2vr_gore.lua.
- henry turns / steps on his own: subsystems/base/native_motion_owner.inl, free_targeting_d42.h phases, `D42_F3_END FREE_TARGETING keptPhases=`.
- something stopped until restart: grep FAULT / LATCHED_OFF / "for this session", load_rearm_r96.h.


---

## 6. INTERACTIONS (pickups, props, doors, holsters, corpses, throwing, petting)

this is the area that ate the most rounds. the game has a canned animation for EVERYTHING (pick an apple = Henry bends,
reaches, 1 s later the apple is in the inventory) and we want the hand to do it. every sub-area went through the same
story: first we tried to drive the game's own action from the controller, then we found the native boundary where the
real work happens and called that directly, then i fought the latches/blockers that kept getting added. read the
"WE DON'T ADD ANY MORE BLOCKERS" part at the end before you touch anything here.

#### how the pieces fit today (R106)

- `src/action_time_owner59c11.cpp` = Queue/Update detours on CryMannequin `CActionController` (vtable WHGame+0x40519E8,
  Queue slot 19 = +0x684D8C, Update slot 21 = +0x4B3DF8, byte-pinned). pickup owner, pickup clip clock (H23/D44), native
  grasp, alchemy speed, mortar, crossbow XR34, throwing supply, doors (`door_use46::read` in UpdateDetour), impale fast
  death R106 all hang off it. `IsHenryController(ctrl)` = controller entity id (+0x30) == Henry's entity id.
- `src/native_grasp59c13.inl` = native pickup adapter (`C_PickUpItemLogic` event A7F894, transfer 8CF6FC, cleanup 40B628,
  attach-only 8D0404, hand detach 8D02AC, DropFromHand 1085770; pins in `core/native_grasp13_pins.h`).
- `src/vrik/instant_pickup_native_d72.inl` (D72 instant route), `src/vrik/staged_pickup596c2.inl` (the one held world prop,
  chest stow, `SmallPickupAdmissionR24`), `src/vrik/static_grab_r89.*` + `brush_swap_r100.h` + `brush_swap_native_r100.inl`
  + `self_collision_r103.*` (scenery props), `src/subsystems/doors/hand_only_h40*` + companion
  `zzkcd2vr_hand_only_doors_h40.lua` (doors), `src/vrik/physical_weapon.h` `holster_zone126/mount126` +
  `subsystems/throw_drop/holster_*.inl` + `subsystems/ranged_reload/stowed_bow_*.inl` (holsters), `src/vrik/lab58.inl` +
  `subsystems/physics/saved_corpse_drag_d72.inl` + `corpse_grip_cg1.h` + `corpse_follow_r101.h` + `zzkcd2vr_corpse_d72.lua`
  (corpses), `subsystems/throw_drop/*` (knives), `subsystems/interactions/pet_*_d59.h`, `washing_d59.h` (petting/wash),
  `core/load_rearm_r96.h` + `observed_load_recovery_r96.h` + `physics/load_fault_recovery_r96.h` (no permanent blockers).

### PICKUPS (apple, cheese, milk, anything PickableItem)

#### what we wanted
reach, squeeze Grip, the thing is in the hand NOW, no Henry bend, no 1 s delay, no flicker. keep holding it, look at it,
throw it, or bring it to the chest to put it in the inventory. the game's own rules (steal, owner, quest) stay native.

#### history, in order

1. **V66.89 / 29 Sep 2026 (design only)** - "native interactable identification, physical holding, chest stow and release"
. static search
   of PickableItem.lua / InteractorAction.lua / PickableArea / CarryableItem, the contract we still use came from it: Grip near an
   eligible item = hold it on the controller, chest region = ONE native pickup request, release away from chest = physics.
   the earlier "contact -> immediate A/Use" rule was dropped for loose items here.
2. **R59.6C2 (~12 Oct) "staged pickup"**. F5 rung 2 experiment:
   hold Grip with an empty hand within 25 cm of the object origin, native pickup request is DEFERRED, the existing world
   item follows the hand, release = native gravity, stow = hold it outside the chest then within 22 cm of the solved chest
   for 300 ms, only then the real native pickup. only for a `CRigidEntity` body (vtable 3A48DC0). that body gate is the
   thing that bit us for the next 3 weeks (see 8). never ran in the headset at that point.
3. **C7G (13 Oct)**: the staged prop driver numbers:
   force cap 40 N, mass 0.01..20 kg, sample lifetime 80 ms, max separation 35 cm. "a bounded servo, NOT a HIGGS
   constraint". the contact classifier had forgotten CRigidEntity (3A48DC0), added as RIGID. also the native corpse carry
   module (C_ActorCarryCorpse vft 3A2F438, states +68: 1..4 pickup, 5 carrying, 6..8 put-down) was mapped here and NOT
   called - "do not force carry through 285F388".
4. **C10R11..C10R15 (14-15 Oct) the action-time owner.** idea: don't fake anything, OWN the game's own `Picking`
   action: arm on the VR Use, let the clip play to its `Attach` event, then freeze it (speedBias 0) and hold the item on
   the controller for as long as Grip is closed, DROP = native DropFromHand 0x1085770, STOW = resume the clip so its
   Detach holsters natively. R11..R14 shipped with `VrActionTimeOwner=1` (OBSERVE) so the hold NEVER ran, and even OWN
   could not have armed: `GetFragmentName` (0x399874) is not a text name in this retail build so "Picking" never matched,
   and Henry's controller was only found through the skinning callback. C10R15 fixed both: Henry identity = controller
   entity id (+0x30) == Henry's entity via `g_henry_item_attach_manager -> player(+0x38) -> entity(+0x38) -> id(+0x0C)`,
   and ARM = the clip's own `Attach` event of a `3d_pick*` clip caught by the CSkeletonAnim event tap (dispatcher
   0x84351C slot 46). me after R14B: "no change in the interactions or in the weapons".
   DEAD END (C10R13): a mid-function hook AT 0x108583A inside the native hand drop. it sat on the first instruction of a
   3-pass loop whose back-edge jumps into the overwritten bytes -> second pass of EVERY native drop executed the middle of
   our jmp = crash. C10R18 moved it to the attachment world-pose getter (0x423974, vtable 0x3A657C0 slot 0x70) filtered
   by return address. the comment is still in native_grasp59c13.inl, don't put a hook there again.
5. **C55 (on C54, 18 Oct) "pickup ownership and native grip transfer"**. headset numbers from frozen C54: Use -> native Attach
   915..937 ms, Attach -> cleanup 122..751 ms, all four "ARMED_AT_NATIVE_ATTACH", none queue-armed (the Picking root was
   already installed before Attach). changes: adopt an already installed Picking FullBody root at the controller callback
   (`ReadCurrentRootIdentity57`, CAnimAction vtable WHGame+3A450B0, Picking CRC), 8x speed lease on the IAction
   pre-Attach, wait for ONE exact joint/socket -> wrist grip proof before FinishForHold, keep that captured grip while the
   item is held (`BuildToolPoses` stopped re-learning the pocket/exit animation every frame = the "item slides in the hand"
   bug), and if no grip correlates within 1 s request native cleanup/drop instead of a paused action forever.
   DEAD END: scaling the whole IAction (speedBias 8x on the action) - "caused outer End/cleanup to precede Attach"
   (comment in TickOwner). that is why H23 exists.
6. **H22/H23 (20 Oct)**. the clock lease moved
   from the IAction to the CLIP only: `CSkeletonAnim` layer play scale setter AA8448 (vt+B8), event scope 4B3A20 applies
   action+5C to the layer only if the scope +58 cache changed. `PickupClipClockH23` in `native_pickup_clock_h23.inl`:
   written=8.f, saved restored before every native ownership change (Queue/Update/Finish). "accelerate only the authored
   pickup CLIP, never IAction time. no normalized seek, no synthetic event". also the floating item fix: once the native
   canonical grip is captured the extra 0.02 m `VrHeldPropClearance` was separating the prop from the native fingers ->
   H23 uses the native grip with no shift. D44 added the migrating-worker protection (`PickupClipTransactionD44`,
   `g_pickup_clock_controller_d44`) because the controller callback can move between engine workers.
7. **D43..D55 headset (28-29 Oct)**: "slow/flickery apple acquisition", "pickup camera jumps", Use -> Attach ~0.67 s,
   adoption ~1.12 s (D43). D52: "Five explicit pickups reach HELD and INVENTORY_COMMITTED; native authored pickup interval
   remains".
8. **D72 (31 Oct) INSTANT pickup**. the real breakthrough: the native route has a
   `forceImmediate` flag. `normalOnUsed 2A8A838 -> C_PlayerStateHandler 33CA59C -> factory 33CA0E8 -> C_PickUpAction
   3B5BE20 -> 39C20C(forceImmediate=true) -> 39926C -> vft+138/8C8910 -> 8C9F08 -> 8CA6BC -> 8CD484(animated=false) ->
   transfer 8CF6FC(animated=false)`. no Picking child, no animation, the native parent does admission + completion. the
   item is then a physics body on the controller (staged prop path) until chest stow commits the inventory
   (`PropCommitInstantD72`, readback `nativeInventoryVerified=1`). Experiments > OBJECTS page 2: "INSTANT activities" master
   + "Instant interactions" (`VrInstantInteractionsD72`, default ON in source; my INI has it OFF - i said the native
   path "is the better one" after R94R1, see 12).
   me: "Apples became quite good with instant pickup. Preserve that, but do not call it universal: cheese/chicken/other
   shelf pickups still failed, delayed or flickered." (02_TESTED_HISTORY / 01_USER_BUGS)
9. **the gate parade (D73..D85R5)** - every build fixed one refusal and hit the next, and NO build ever logged a single
   `D72 INSTANT_PICKUP` success row (counted over the D73, D78, D79, D80, D81R1, D85R5 logs):
   - D73/D78: `WRIST_OWNER_NOT_ADMITTED` (instant was F5-only / workstation wrist) -> D79 removed.
   - D79: `FIST_OWNER` 11/11 - `FistInputOwned101()` asked "is the unarmed system SELECTED", never "is this hand a fist".
     the ordinary Use path already had the per-hand rule (Grip>0.60 AND Trigger>0.60). -> D80 per-hand.
   - D80, D85R5: `OBJECT_NOT_VERIFIED_RIGID_BODY bodyVftRva=3A578C0` = `CPhysicalEntity` = a STATIC body. things on tables
     and shelves are static. the D81 "small pickup" prep (`SmallPickupAdmissionR24` + Lua `KCD2VR_Lab.TryPreparePickupR24`
     inside `PickableItem:Use`, through the door GetCVar hook `kcd2vr_small_pickup_v1`) was supposed to `Physicalize(0,
     PE_RIGID)` them inside the same Use and never did: ~25 silent `return false` gates (`mass ~= 0` at
     zzkcd2vr_lab.lua:194, the reader registered only after the first instant Use...).
   - what i SAW while all that failed: the native fallback = `3d_pick_rhand.bspace` FullBody Picking with root
     motion = Henry bends for ~290 ms until the clip's Attach at t=0.452, then `JOB_ATTACH_ADOPTED` -> `SOCKET_PENDING`
     (item drawn on the animated hand socket `pck_ir_00`, we write nothing, native_grasp59c13.inl:362-367) -> `HELD` 5-21
     ms later = the 1-2 frame jump.
10. **R89 pickup (3 Nov)**: the prep moved IN FRONT of the Use, on the game thread, with a readback:
    `pickup_use_producer19c3.inl` posts {entity,hand} when the VR Use pulse goes out (~100 ms before the Use reaches the
    item); `static_grab_r89.inl Prep()` next frame: PickableItem + STATIC body + fits 65 cm -> Lua `Physicalize(0,PE_RIGID,
    {mass})` ONLY when `item:CanSteal(you)==false`, `BelongsToDeadBody()==false`, not npcOnly, one slot; then the body
    vft is read back: `R89 STATIC_GRAB phase=PREP_RIGID` / `PREP_STILL_STATIC`. when the instant path still refuses the
    reject literal is shown once: "pickup: game animation (<REASON>)".
11. **R94**: `F5_PICKUP_PREPARE_NATIVE ... grams=300` + `PREPARED_NATIVE_MASS_READBACK` - the prep now works, the
    item becomes PE_RIGID 0.3 kg, and `INSTANT_NATIVE_ACCEPTED_WORLD_HOLD` + `NATIVE_IMMEDIATE_RETURN result=1` appear
    for the first time ever (13/13 attempts in the R94R1 run).
12. **R94R1 headset (4 Nov) -> R96**: "i squeeze on a mug or apple, get the taken buzz, and nothing comes with
    the hand. the VR hand seems stuck on the object." all 46 `F5_PROP_BODY` rows: `deltaPosition=0.00000` while
    `observedV` went 0.19 -> 8.2 m/s. the velocity servo pushes a body that does not move (why the game ignores
    SetVelocity on these bodies is STILL unknown), and the displayed wrist follows the object pose (coherent_pose.inl
    ~848-864) so the hand looked glued. 8 holds ended by giving up, 5 by `BODY_STATUS_ENDED`. fix in
    staged_pickup596c2.inl: if the hand target is >= 5 mm from the object and the object moved < 0.5 mm for 100 ms, the
    SAME hold switches to entity-transform carry (`kinematicR96`, same `PropPlace596` / CEntity SetPosRotScale vft+0x168 =
    +509810 as the old kinematic mode), velocity zeroed, hand presentation cleared. row `F5_PROP stage=KINEMATIC_CARRY_R96
    reason=NATIVE_BODY_NOT_FOLLOWING_100MS`. from 17:53 that day i switched instant OFF and used the native path
    (NATIVE_GRASP, 488 rows, stows confirmed) - so the native/faster path with the H23 clip clock is what i actually
    plays with. R96 also: `PHYSICS_DRIVE_FAULT_LATCH` cleared by the next squeeze (was session-permanent), small-item
    reader retries every 5 s after 4 refusals (was forever).
13. **R97 merge**: R96R1 added the 80 ms prop hold gap (`sourceTickR96`, `observationGapR96`), load retires the
    old hold (`loadEpisodeR96`), `g_prop_fault5916c1` became a load-scoped `load_fault_recovery_r96::Fault`. R97 kept
    "next real squeeze clears it" on top, because the R96R1 way alone would block pickups until the next save load, which i called
    a blocker.
14. **D84 (2 Nov)**: chest storage haptic pulse on the hand that acquired, both paths (instant: after inventory readback;
    native: after the checked storage result + same hand release). chest gesture uses the completed REQUESTED wrist
    (before collision correction) so a hand stopped by the torso can still stow. "There is still one global staged prop
    and one native pickup owner" = sequential either-hand, never two unrelated props at once.

#### WHAT WORKS NOW
- native path (instant OFF, what i play): VR Use pulse -> `QueueDetour` arms the owner on Henry's Picking queue
  (`ARMED(queued after a VR hand Use)`, Use must be < 1.5 s old) or `AdoptPendingPickupAfterUpdateH5` adopts an
  already-installed root -> H23 clip clock 8x on the `3d_pick*` clip -> native Attach event -> `JOB_ATTACH_ADOPTED` ->
  canonical grip capture -> `HELD` on the controller -> Grip open = `DropFromHand` 1085770, chest = resume -> native
  Detach stows. menu: ACTIONS page `VrActionTimeOwner` 0/1/2 (default 2 OWN, a saved 1 is migrated to 2).
- instant path (`VrInstantInteractionsD72`): R89 prep -> D72 forceImmediate transfer -> staged prop on the controller
  (servo, 100 ms stuck -> kinematic) -> chest 300 ms -> `PropCommitInstantD72`. the lean is gone there, but the
  "body doesn't follow SetVelocity" thing means the kinematic fallback does the carrying.
- known open: the native fallback still has the bend + 1-2 frame jump for owned/large/special items (proposal:
  start the Picking fragment at the Attach time, write the controller placement at
  JOB_ATTACH_ADOPTED instead of waiting in SOCKET_PENDING - NOT done). `TryPrepareContactR24` in the lab Lua still
  compares `e.id == id` (userdata vs number, always false) - probably never worked in retail, left for a decision (R100).
  pickup camera jumps = camera, section 2/9.

### ITEM ATTACHMENT (what moves the thing in the hand / on the back)

#### the facts that were established (and the dead ends)
- **V65.07 (Sep)** static notes:
  `C_ItemAttachmentManager` has separate Attach / AttachLeft (side 1, hand_left) / AttachRight (side 0, hand_right) /
  draw / holster paths; `slt_0` is a real placement helper (28-byte QuatT, used by the arrow placement); `RgtArm01/LftArm01`
  are procedural IK handles only seen in door-style `JointId=slt_lock` entries; `C_AttachmentSlotPoseModifier` belongs to a
  clothing hierarchy, NOT the equipped weapon manager. decision then: zero item/IK/pose writes until the per-frame input
  transform owner was proven. V65.09 added the F10 attachment-order arm ladder, V66.79 the "attachment-root weapon".
- **C4R1 -> R14 -> D62C contract history (30 Oct)**: native pose restore after skinning (`Restore` + `PublishCommitted` after the original
  skinning call) was there since C4R1, not new. `PlaceBothConsumers6689` (entity PRS + character location) and
  `PhysAttachmentWorld164` identical in all three. the `CEntityAttachment::UpdateAttachment` hook WHGame+0x4238B8 is a HELD
  entity route, not a back-socket override. the one real upstream change: C4R1 `SuppressEnqueue114 = f3==2 ||
  hideNativeClipsActive178r28`, R14/current = `motionPolicy==2` only -> native attack clips stay alive under policy 3,
  so the stowed consumer gets live native animation. (and R14 already had that, so it does not explain a post-R14
  regression.)
- **the stowed-bow leak**: the BONE renderer does NOT read our skinning palette.
  character render 0x3D3384 -> attachment manager 0x3D45E0 -> BONE loop 0x20E7D20 -> BONE draw 0x20EB4B0 -> CCGF render
  0x27D22F8, computes `parent * cached native model QuatT(+0x30) * additional QuatT(+0x4C)`; cache refreshers 0x5D7BA0 /
  0x37599FC read the native joint pose. so a corrected VR torso does not move the bow drawn on it. the staged fix: resolve
  the 10 named equipment sockets once, publish their final model pose with the completed body publication, and at the
  BONE draw leaf pass a PRIVATE context with parent `P*D*inverse(N)` into the unchanged native renderer (D = VR socket bone
  * native attachment-relative, N = cached native model). 742 host checks, "no headset result for this correction".
  DEAD END (asset_pose): "the aim-pose BackSlot rotation controllers vary" - all 15 decoded keys are constant, no.
- **stowed hide** (`VrHideStowedBow` 0/1/2, default 2 = always; `VrHideStowedBowPitchDeg`, `VrHideStowedBowAttachments`
  default weapon_bow): C10R15 `StowedBowAttachmentTick59c15` - CAttachmentManager = char+8 (vt 0x4060548),
  GetInterfaceByName vt+0x40 = 0x971B2C, HideAttachment vt+0x88 = 0x5AF5E8, flags 0x70000. hides only what it hid. D33/D56
  made it one owner for every stowed bone attachment (`stowed_hide_hw.h`: station = all hidden, normal play = back ones
  hidden `VrHideBackWeapons`, D56 `station_weapon_entities_d56.inl` hides CA_BONE vft 0x3A657C0 + CEntityAttachment binding
  vft 0x3A68F68 entities at stations). hiding masks the leak, it does not fix ownership - say that in any readme.
- the hand side today: `physical_native.inl` ~846-879 requires attachment frame == completed visual-body publication,
  replay returns Busy while a newer body publication is incomplete (R14->D62 change, relevant to draw cuts). R98 added the
  weapon rebase (`physical_native.inl:807-847`): when Henry's next body pose is still being published the weapon is
  re-placed at the current world instead of trailing one frame.

### SMALL PROPS, LEVEL BRUSHES, HELD PROPS

#### what we wanted
tankards, plates, cups, candles, jugs on tables = take them, throw them. KCD2 has them as PE_STATIC `BasicEntity` (same
class as the loose ones, only `Physics.bRigidBody=false`, `EntityCommon.PhysicalizeRigid` picks PE_STATIC) or, much more
often, as plain level BRUSHES with NO entity at all (the game's IPL_GameData.pak: 923 Type="Brush" vs 47 GeomEntity in the
cooking_eating / illumination / baskets / food prefab folders).

#### history
1. **F9 "prop wake" lab (`VrLabPropWake58`, lab_prop58.h)** - refused `STATIC_BODY` by design ("CPhysicalEntity base = world
   geometry, never woken"), so it never touched a single table prop. DEAD END for props (keep it for the rigid-body lab).
2. **R89 static grab (3 Nov)** `src/vrik/static_grab_r89.h/.inl`, switch `VrStaticPropGrabR89` = Experiments >
   Objects page 2 "Small props in the hand". evidence: 734 `CONTACT_RAY source=HAND_WORLD` rows - 144 `bodyClass=BASE
   owner=0 entityId=0` (brush), ~190 BASE with an entity (apple/cheese/milk + ~50 dressing entities around the alchemy
   bench), 168 RIGID, ~250 ARTICULATED (NPCs). rules: touch memory 450 ms from the hand-contact mailbox, Grip close edge
   (<0.45 -> >=0.58, not a Grip+Trigger fist) with a FREE hand, class from `CEntityClass` name (BasicEntity, RigidBodyEx,
   RigidBody, RigidBodyLight, DestroyableObject, BreakableObject, Debris, GeomEntity = grab; Pickable/Door/Chest/Bed/.../Dog
   = somebody else owns it, no message), box diagonal <= 65 cm, mass = volume x 350 kg/m3 clamped 0.05..3 kg, reach 30 cm.
   grab = hand claim (mutual_admission pickup slot) + Lua `EnablePhysics(false)` + carry by SetPosRotScale; release (Grip
   <0.30) = `EnablePhysics(true); Physicalize(0,PE_RIGID,{mass}); AwakePhysics(1)` + hand velocity (max 8 m/s,
   `KinematicTrailD85`) with the pinned SetVelocity. one `R89 STATIC_GRAB phase=.. reason=..` row per attempt + one screen
   line ("grab: that is fixed level geometry, it can not move", "... too big (N cm)", "... is a <class>, not a loose prop").

3. **R91/R94**: moved the carry onto the verified native rigid-body servo (physics stays ENABLED during the hold,
   `PrepareSceneryR94` rigid mass readback, release keeps native momentum), lab default OFF.
4. **R94R1 headset -> R96**: 10 of 10 squeezes on mugs = "grab: fixed level geometry has no loose entity". the hand ray
   hits the TABLE under the mug (static, no entity) so the mug was never looked at. fix `NearbyEntityR96`: on a Grip close
   with no entity touch, retail `GetEntitiesInBox` (same contract as the anvil, `anvil_physics_a3.h`) in a 20 cm box around
   the palm, keep bodies whose foreign data is a real CEntity, palm inside box+4 cm, smallest box wins (mug, not table).
5. **R100 brush swap (5 Nov)** - because most of those mugs are BRUSHES and R96 could only say LEVEL_GEOMETRY.
   `VrSceneryPropSwapR100` = Experiments > Hands page 2 "Loose scenery props (swap)", default ON, works with "Small props"
   OFF. on Grip: hide the CBrush (IRenderNode::Hide slot +0x220 = +5DF758, ERF_HIDDEN bit 8 of flags +0x28, read back),
   suspend its physics (CPhysicalWorld vft +407FC80 slot +0x98 = DestroyPhysicalEntity +846E10 mode 1, restore = mode 2,
   sim class byte +0x47 -> 0xFF), spawn a NOT SAVED BasicEntity through Lua `SwapSpawnR100` (ScriptBind_System::SpawnEntity
   +39ACB00, object_Model = the brush .cgf from CStatObj +0xA0, bRigidBody, mass 0.2..1.5 kg, ENTITY_FLAG_NO_SAVE), id
   comes back through the private GetCVar note `kcd2vr_swap_note_r100:<serial>:<id>` (same retail getter hook +41C388 as
   the small pickup), place with SetPosRotScale at the brush pose (read back <= 1 cm / 0.02 rad), then the normal
   small-prop carry (`AcquireR100`). any failure after the hide rolls back at once (never a doubled mesh). brush proof:
   CBrush vtable +401FE70, statobj at +0x98, Matrix34 at +0x50, physics link brush+0x80 == body and body+0x20 == brush,
   body+0x28 == 1 (CBrush::Physicalize +530A3C). all pins checked before first use (`SwapPinsR100`), mismatch = one log line
   + swap off for that game build. table of 256, `SwapLoadR100` restores every brush and removes copies on a loading screen.
   deny list: no hlod/cestool/merged, not objects/manmade/structures, intermediates, natural. also found+fixed: KCD2 `e.id`
   is a userdata handle so `e.id ~= id` was ALWAYS true in the lab Lua (`sameIdR100`).
   headset (R101 build, 5 Nov): 6 swaps worked (tankard, plate, jug, scoop, demijohn) - BUT the held prop pushed Henry
   away and spun, the plate sat 13 cm off.
6. **R103 hold**: `self_collision_r103.h` + `self_collision_native_r103.inl`: the held prop gets `ignore |= living` and
   `type |= living` through the retail SetParams type-26 packet (rigid SetParams 6F1BE4 -> CPhysicalEntity::SetParams
   84E3EC, bits at +0xEC/+0xF0, same 28-byte packet the hand proxy and weapon body already send), read back, retried every
   250 ms; restored 0.4..4 s after release when the object is 0.6 m from Henry. servo: force 40 -> 80 N, angular 80 -> 300
   rad/s^2 per kg, max spin 12 -> 20 rad/s. grab offset now from the VISIBLE hand (`CarryHandR103` = the committed
   `allowed` wrist, not the raw controller that was inside the table top) = the plate offset fix. the hand/weapon world
   query skips the held prop (`npc_contact172.inl:79-85`). rows `R103 SELF_COLLISION phase=EXCLUDE|RESTORE ...`.
 not headset tested yet.

#### WHAT WORKS NOW / open
Tick = `static_grab_r89.inl` ~:482 from `NativeRhythmTick178` (frik_pose_probe.cpp ~3921), `TryGrab -> NearbyR100 ->
B::choose (brush_swap_r100.h:64) -> SwapGrabR100 / AcquireR100 -> Carry -> Release`. a quest BasicEntity could be moved by
you first (no readable bMissionCritical from C++) - not observed. a brush with a material override comes back with the
cgf's own material (`material=OVERRIDE` in the row). fast travel = loading screen = carried copy is dropped and brushes
return, by design.

### DOORS

#### what we wanted
palm on the door, Grip, move the hand, the door follows. release = the game finishes open/close. both hands.

#### history
1. **C37I1 (17 Oct)**: "the VR arm step inspected another hand's pickup candidate" - door target 952252 accepted for the
   right hand, the global last-hover was a left-hand PickableItem 263342 -> `CANDIDATE_TARGET_CLASS_OR_OWNER`. fix: freeze
   the door witness per target+hand at the native request.
2. **C46 door use token protocol** (`core/door_use46.h`, `door_signal19c4r4.cpp`, `KCD2VR_Doors.pak` with
   `zzkcd2vr_doors.lua`, protocol 1944): the mod drove the game's own door Use and then paused/scrubbed the door's Lua
   clock. in C55 all three accepted door actions were refused `NO_EXPLICIT_SEMI_INPUT`, projection `PALM_OUTSIDE_OR_STALE`.
3. **C64 (19 Oct)**: reproduced the real blocker: closing Grip captured a nearby generic Pickable as the entity
   lock BEFORE any Use was sent, so a Door offer appearing later was rejected; also the pickup qualification re-locked the
   wrong entity every frame. my held-Grip attempts at 01:52:33-01:53:17 were real (hand rays 5-9 cm from door 952252).

4. **C65 (19 Oct)**: i simplified the contract: palm within 20 cm of the SAME door, hold Grip = the native clip plays,
   release = PAUSE at the current progress (not finish), regrip = resume without a second Use. protocol 1965/rev 65, the
   250 ms unsent-squeeze expiry removed. **C66**: hand motion controls the native Door animation.

   DEAD END (the whole C46..C66 line): driving doors through Henry's native "use" action + the interactor + the action
   point "we spent two days on in assembly" - none of it is needed to move a door.
5. **DOOR01 (22 Oct)**: AnimDoor.lua IS the
   door. the leaf is a cga in slot 0 with two clips, the game opens it with `StopAnimation -> StartAnimation(0,name) ->
   SetAnimationSpeed(0,0,dir) -> SetAnimationTime(0,0,t)` and finishes in `AnimDoor.Server:OnUpdate` when GetAnimationTime
   reaches 1 (ReportOpened/Closed, portal, physics flags). `DoPlayAnimation(direction, forceTime, useSound, customAnim,
   usePlayerAnim=false, userId)` skips `player.actor:StartInteractiveActionByName` = no canned Henry animation, no
   interaction lock. 170 lines of Lua, `#KCD2VR_Door.Test(0.5)` from the console. i tested it in the dev build the same
   night: Test(0.5) = half open, WORKS. only the time mapping was inverted (t=1 was closed) - one table entry.

6. **H40 hand-only doors (23-24 Oct)**: productionised: ordinary Grip-only first press (Grip >= .55,
   Trigger < .10, strict fists stay > .60 both) -> original identity + completed world palm -> exact-retail
   `native_script_t2` packet -> callback `kcd2vr_door_hand_h40_t2` -> the selected door's own OnUpdate. native
   IsUsable/OnUsed keep lock/key/crime, only the scoped object-play argument becomes usePlayerAnim=false. 16-field per-side
   claim tokens, a refusal AFTER Lua reserved a pending door does not replay the canned Use. the 45 deg -> phase 0.5
   mapping was wrong (retail `door_l_b_o_player` has moved only 2.9 deg at phase 0.5, stealth clips overshoot to 96.8 deg)
   -> curve inversion per selected clip + hinge axis from `GetBonePos("door_left/right_pivot")` (callback 37D3EF8, slot
   zero world matrix B7258C).
7. **25 Oct, R11/R12 headset: "doors PERFECT (keep)".** from then on the door code was mostly touched by
   people fixing OTHER things, which is where the two R96 bugs come from.
8. **D46 (29 Oct)**: 68 `HOLSTER_CANCEL_D44 ... DOOR_OWNER` rows - an unconfirmed door reservation poisoned a held Grip
   into "release required" for the holster. the per-hand QuickTalk/door/F9 claim arbitration (H50) is the answer, not a
   door change.
9. **R94R1 (4 Nov)**: the donor door patch had forbidden key/manual-side unlocking; R94R1 restored it ("truly unusable
   locked doors cannot acquire the gesture; relocking revokes seeking"). `VrDoorGestures` no longer switches anything -
   "hand doors always on" (CL-D, `subsystems/doors/config.inl`).
10. **R96 the LEFT-HAND rule**: 10 of 30 H40 rows in KCD2VR2.log and 6 of 14 in KCD2VR.log were `pressL=
    OTHER_TARGET` (right: 4). in `hand_only_h40_input.inl` ANY fresh native non-door offer under that hand (`knownOther`)
    refused the squeeze before the nearby-door query - and the offer could be the sword in the OTHER hand (entity 262978
    ranked 0 in the door collector). now the other offer keeps the Grip only if (a) it is within the 25 cm hand-pickup
    reach AND (b) no door candidate for this same hand (`NativeDirectDoorCandidate54r1`, <= 0.5 m) is at least as close
    (+5 cm). no door found = the same Grip still falls back to native Use. ALSO: the H40 `vmQuarantine` (Lua VM changed
    during a held door -> hand doors off until restart) now lifts when the completed frame shows a different player or
    body (save load / actor change), `stage=CLEARED reason=VM_REPLACED`; same body = stays (R96R1: "a replacement VM
    cannot acknowledge the old native clip" - true for the same world, not for a gone one). R97 merged both with the R96R1
    time-of-read fixes (`ObserveClockR96`, probe tokens cleared on a VM change).

#### WHAT WORKS NOW
`subsystems/doors/hand_only_h40.h` (Frame, Identity, Valid, 80 ms freshness, Admission, quarantine), `hand_only_h40_input.inl`
(runs BEFORE the old native Use FSM), `_current.inl`, `_service.inl`, `_pose.inl`. KNOWN_ISSUES still says: "Retail Lua
control flow is exercised in both load orders under a host Lua VM; retail Lua 5.1/game integration and visible swinging are
not executed here" = the host tests; the headset said PERFECT on 25 Oct. the left-hand rule and the VM lift are NOT
headset-tested (5 Nov). the hand is not physically stopped by the door leaf (kinematic, like flat). double doors: each
leaf is its own AnimDoor entity, each hand can take one.

### HOLSTERS

#### what we wanted
Skyrim VR style: reach to the hip/shoulder, Grip, pull = draw. put it back in the zone, release = stow. the game decides
what you actually have equipped.

#### history
1. **V66.102 (Sep)**: arrows over the RIGHT shoulder (~22 cm zone), the bow drawn/holstered by the LEFT hand over the
   right shoulder, two-handed latch without the grip button.
2. **V66.131**: "releasing primary Grip anywhere no longer requests native Holster" - the item must come back to its own
   zone, dwell, then release. shoulder holsters = body-space vertical capsule. **V66.136**: zones ~24 cm hips / ~28 cm
   shoulders, `HolsterZoneRadius=0.30`, `HolsterOnGripRelease=0` default (physical stow), sticky latch, haptics 55%/70 ms
   zone cue + 85%/90 ms confirm. **V66.140**: one-handed NativeBelt items cache the native belt mount WHILE holstered so the
   stow point does not follow the drawn sword. **V66.152/153 "user layout"** (`UserShoulderHolsters=1`): swords/maces/axes/
   daggers (class 1-6, 16) = the game's OWN belt attachment, right hand; polearms, crossbows, firearms (7, 10, 13-15) =
   right shoulder; shields (8, 17) = left shoulder left hand; bow (9) = left shoulder left hand.
3. **C10R14B (14 Oct)**: "bow is RIGHT shoulder, if I take it with left hand (only left hand), left shoulder for the
   LEFT HAND with shield". `holster_zone126`: shields -> LeftShoulder in BOTH layouts, bow stays RightShoulder/left hand.
   draw selection matches zone AND hand (`PhysDrawCandidate126`) so the right hand at the right shoulder only reaches
   polearm/crossbow/firearm, never the bow.
4. **C45 (17 Oct) "fixed loadout holsters"**: axe/mace/flail -> right hip virtual zone (`RightHip45`), crossbows and
   rifle -> lower/middle left back (`LeftBack45`), dagger -> left chest reserved only (`LeftChest45`). "The game's crossbows
   are natively left-primary, unlike the rifle." release = one immediate native sheath request + max 250 ms visual bridge.

5. **H3 (19 Oct) = the accepted reference** ("the torch is perfect, nice, but still not real"; i later asked for H3 as
   the restoration reference for torch/holsters). **H42** moved the bow to the LEFT shoulder, **H43/H50** fed the holster
   from raw pre-reach `modelIntentGoalsH30` instead of the clipped visible hands, capsule [-.10,+.14] -> [0,+.14], fist
   exclusion from "BOTH hands Grip>.55" to "THIS hand Trigger+Grip>.60". found in the H3->H50 audit: bow right-shoulder gap
   45.65 cm in H3 geometry vs 24.17 cm left-shoulder in H50 for the same test palm = acquisition geography changed.

6. **R8d (25 Oct)**: `holster_diag_r8d.h` - `R8D HOLSTER_ATTEMPT firstMissing=` per attempt + one visible why
   line, `HOLSTER_SUMMARY` every 5 s. those are the "H3 holster lines" in the log. **R12**: `RightShoulderBowR12`,
   `RightBackR12` (the current enum values). **R15**: Grip can acquire inside the zone when already held, same-hand
   priority, wait for real native ack. **R26**: "unavailable idle candidate with closed Grip -> CancelAcquisitionR15 ->
   requireReleaseR15=true -> a later eligible zone cannot latch" -> idle refusal no longer consumes the Grip.

7. **headset D43..D55**: D43 "finicky holsters/bow acquisition", D46 "holsters still unreliable, bow zone high/unreachable,
   stowed bow comes forward with native torso movement" (the leak above), D52 "bow nearly perfect apart from oversized
   forward shoulder zone", **D55 "Holsters good"**. D85 `holster_shoulder_d85` = the PLAYER's shoulder plane, not Henry's,
   for the draw volume.
8. **R94R1 -> R95**: "holsters stopped working" = the VR menu "Hands" mode set pose 5 / weapon 0 (fists mode) and F12 went
   7 -> 0. R95: Hands = {3,2,4,6} keeps weapon/combat/holsters (`user_modes175.h`, frik_pose_probe.cpp ~5290). the same-day
   control run had F12=7 the whole time and the crossbow drew/stowed fine. the 6 `BOW_QUIVER_OWNS_SHOULDER` refusals = the
   polearm at the right shoulder while the bolt quiver owns that zone (design conflict, still open).


#### WHAT WORKS NOW
`physical_weapon.h`: `holster_zone126(p)`: bow -> `RightShoulderBowR12`, shield -> `LeftShoulder`, class 0 -> `LeftChest45`,
holsterSet 2 / class 7,10,13,14,15 -> `RightBackR12`, class 3,5,6 -> `RightHip45` (shoulder - 0.50 up + 0.03 right), class
1,2,4,16 -> `NativeBelt` (the game's scabbard, mount cached while holstered). `holster_hand126`: bow/shield = left,
crossbow 10/14/15 = left hand reaching behind the RIGHT back, rest right. capsule `shoulder_holster_closest131` (H3
-10..+14 cm for bow/shield, -14..+12 for the back). INI `HolsterZoneRadius` 0.28-0.30. the draw/stow state machine is in
`subsystems/throw_drop/holster_gestures.inl` + `holster_commit_d48.inl` + `holster_completion_h36.inl`. stowed hide =
`stowed_bow_*.inl`. open: the stowed bow/crossbow STILL moves with native upper-body clips (the render-leaf fix was staged,
never headset tested), quiver-vs-polearm shoulder conflict, and in native crouch the hip sword is drawn 0.4-0.67 m from
where the game puts it (R98 log, unconfirmed).

### CORPSE DRAGGING

#### what we wanted
grab a dead body by a limb with an empty hand and drag/lift it. two hands. no stretching. the native "carry corpse"
(shoulder hoist) stays the game's.

#### history
1. **C7G (13 Oct)**: part queries mapped (CArticulatedEntity vft 3A48B30, GetStatus +30 -> 8A9348, part status type 1 struct
   0x78: position +18, quat +3C), CPhysicalProxy +38 is the canonical body and a contacted skeleton body can differ
   (`CONTACT_BODY_REPLACED`). **C69/H6 (19 Oct)**: "restoring working corpse push", T4 phase hooks (6F036A, 38DC100) - the
   F9 physics lab. H9/H13: entity root vs skeleton main mismatch, `ImpaleRoot47`, type 10/11 add/remove, status 26 lifetime.

2. **lab58 (F9 rung 3, `VrLabCorpseDrag58`)**: the articulated route - per-part `pe_action_set_velocity` with partid
   (A99DF0 type 10, "+0x1F0 per part") on the gripped part, servo gain 6, maxSpeed 3, leash 0.5 (lab58.h:168-171).
   the early living-NPC version made the DOG ragdoll/flicker (D52: "F9 living path deliberately paused after dog ragdoll
   regression") - DEAD END, "Do not revive it".
3. **D72 saved route** (`saved_corpse_drag_d72.inl` + `zzkcd2vr_corpse_d72.lua`): the game-thread script client
   (`native_script_t2`) asks Lua for dead bodies near the hand (`System.GetEntitiesInSphere(h,1.6)` + `GetBonePos` per bone),
   drives with `PHYS02 Entity:SetVelocity`. D78: F9 corpse cursor. **D84**: corpse grabbing became "selected gameplay" with
   no F9/lab gate (it needed lab rung 3 AND F9 rung 3 AND physical combat on before; central settings also overwrote the
   preference with false on save/load). Experiments > NPCS "Physical corpse dragging". the honest line from that build:
   "Physics: NEVER reliably working; no general NPC/corpse grab/drag success. One-off console tests/partial reactions are
   not F9 acceptance."
4. **R89 corpse (3 Nov) "performance tanks as soon as corpse dragging is ON"**: `F2 FRAME frameMsP95` 20.0 ms with
   the search running vs 12.45 ms refused, before any body is touched - `SavedWantedD72` ran the Lua `Q` query every 100 ms
   per empty hand = ~20 full entity-system walks per second, plus `DragSearchW5` in lab58. fix: `corpse_grip_cg1.h`
   `kSearchGripR89=.15`, `kSearchWindowMsR89=700` - a search window opens when the grip reaches 0.15 and closes 700 ms
   later or when the hand opens; idle = zero VM calls. Experiments "Corpse search" ONLY ON SQUEEZE (`VrCorpseSearchSqueezeR89`).
   the drag itself was NOT proven: every search in those logs answered `NO_NEAR_DEAD_BONE`.
5. **R95/R96**: either hand; TWO hands was impossible (the Lua search skipped the body the other hand held, the second
   hand always picked the same part -> claim refused) -> second hand takes the nearest other part. left OFF by default (a
   squeeze near a body could steal a loot pickup); it is ON in my INI.
6. **R101 (5 Nov) "limbs and head stretch like latex"**: cause = both routes overwrite ONE part's velocity every frame =
   that part has infinite mass, the other ~40 kg hang from soft joints, the skin stretches between bones; the hang route
   (`gore_policy.h follow=10, maxSpeed=6`) same. `corpse_follow_r101.h`: `Shaper` (hand 2.5 m/s and 30 m/s^2, hang 3 m/s
   and 40 m/s^2, starts from 0 so no snap), `Rest + plan()` stores the grab-time distance of every other part and gives any
   part beyond distance + slack (5 cm + 8%) the gripped part's velocity + 8/s pull back (max 1.5 m/s), guard at 20 cm over
   limit (stuck on something) -> leash release, not a latch. two hands: one writer per part, pair correction. native side
   `gore32n::BodyFollowR101` (native_gore.inl:79-103) lists parts with GetStatus 8A9348 by index, writes with the existing
   PartVelocity. test numbers: drag 2 m/s for 2 s worst stretch 3.597 m -> 0.010 m. NOT headset tested - there is still
   no GRAB/DRIVE row in any of my logs.
7. **R103**: dragged corpse pushes Henry -> `Exclude(Kind::Corpse)` ignore |= living after each accepted part drive, restored
   when the corpse origin is 1.0 m from Henry or 4 s after the last release. note: articulated SetParams skips everything
   while `[body+568h]` is set -> row `REFUSED`/`QUEUED_BY_PHYSICS_STEP` if that happens.

#### WHAT WORKS NOW (in code, headset unproven)
`lab58n::Tick` from the outer frame -> `DragTickSide` (articulated) / `SavedServiceD72` (saved Lua route) -> CG1 squeeze
-> candidate -> `GRAB` -> `DRIVE` rows (`F9_HAND_DRAG_D72 event=`). the recording that decides: F9 capture, one squeeze
on a limb, must show `CANDIDATE -> GRAB -> DRIVE` with nonzero `observedDelta` and `R101 F9_CORPSE_FOLLOW ... ok=1`.

### THROW / DROP (throwing knives, dropping held things)

1. **T6 (19 Oct)** throwing inventory, **T15 (21 Oct)** "invisible inventory throwing holsters", **T17/T18 (23 Oct)** the
   native ammo breakthrough: CArrow vft 3ECE6F0, factory 289F48C, native launch 289F7A0, 2AAD50C particle/rigid velocity,
   fired transfer 8D534C(container, sourceAmmo, 1, false) moves the real unit into arrow+118 ("do not decrement again"),
   R8 to 289F7A0 is the targetEntityID not a hit ticket. 20 m effective flight then steep tail. "Do not cast existing melee
   WUID to Ammo".
2. **25 Oct user direction**: ordinary inventory weapons, default ON, no F-key rung, buy 5 at the weaponsmith, one
   adjustable pouch (chest point = shoulder midpoint - 18 cm up + 12 cm forward, radius 16 cm), Grip to draw, release to
   throw, "Holding a sword in the other hand must not globally disable knife draw". `VrLeftChestKnives` default true.

3. **K31..K40 (30 Oct - 1 Nov)**: D55 log: the pouch runs (K34 REPLENISH daggers 5, 67 `K35_POUCH_DRAW_READY`, 2 submitted)
   but ZERO `K35_KNIFE_MODEL_HELD`, both retired at `PRIVATE_LAUNCHER_ACQUIRE`; the buzz checked stock and hand, not a
   ready launcher, so "the buzz means I can pull a knife" was a lie. the host tests had stubbed `AmmoPrivateLauncherAcquireT21`
   to READY. K40 found the real one: `AmmoKindsT17` asked `IsKind(ammo,0x1B)`; the retail S_AmmoItemClass (vft 3BA6EE8+18 ->
   96F66C) accepts 3, never 0x1B (that is the weapon base) -> every genuine temporary Ammo was rejected BEFORE the factory.
   fix `kind(launcher,2) && kind(ammo,3)`.
4. **D80**: `PAID_NATIVE_PIN rva=289F7A0` fault 47x = still no throw. **D85R5**: moved the zone to the LEFT HIP
   (`ph::knife_left_hip_d85r5`, shoulder - 50 cm up + 8 cm forward, r 16 cm), killed the 5-knife starter grant
   (`StartupWanted()` false, `register()` removed from `inventory_delivery_t26.lua`), removed the REFILL button, stock = one
   retail GUID `3a640e5d...` (daggerCommon) only. result in my run: 26 `T27 LEFT_CHEST preflight` rows all `knives=0`,
   nothing happens. "no build in the logs ever threw a knife: D80 failed on the paid pin, D85R5 fails on stock."
5. **R88 (2 Nov)**: the D80 "REFILL KNIVES TO 5" button is back on the Immersion page (K34 path
   `inventory_lifecycle_t8::RequestReplenish -> kcd2vr_inventory_replenish_k34`), refusal text names the remedy. still open:
   the `VrChestDagger=true` visual dagger sits on the chest while the zone is at the left hip; "the draw, flight and hit have
   still never been seen in a headset".
6. drop of held items: native `DropFromHand` 0x1085770 (C10R15), `native_weapon_drop_t1.inl` (T1 release/throw seams on
   H12, T14), `throw_policy60.h`, `KinematicTrailD85` (max 8 m/s) for props/pickups, `allowed_release_history.inl`.

### DOG PETTING / FACE WASHING (D59)

- 17 Oct logs: `companion_bond` arrives together with toggle_crouch + opponent_lock on the same sample (R3 is multiplexed),
  then `player_dog_carees_v04` plays. C37I1 mapped the contract: hand near the dog's head, lean/pat, scoped bond request.
- **D59 (30 Oct)**: `pet_service_d59.h` / `pet_gesture_d59.h` / `dog_candidate_d59r1.*`: open hand (grip/trigger <
  0.35), current native Dog candidate <= 0.20 m, a caress of >= 8 cm travel with >= 6 cm net over >= 120 ms -> ONE
  `companion_bond` dispatch through `native_script_t2` to the retail `Dog.OnBonding` (Dog.lua:213 `self.dog:Bond(self.id)`,
  native 29DF430, concept HRAC_POCHVALA_PSA). both hands withdrawing > 0.32 m for 180 ms re-arms. the game owns
  eligibility, reward and the dog's own `Petting` fragment (kcd_dog_database.adb). `VrPetGestureD59`.
  **D62C headset: "dog petting works (human-qualified)"**; D77: "a real accepted partial feature; contact stability still
  poor" (fingers flutter on the dog). default was still OFF in mod.cpp until D79 ("Dog petting and riding hands default
  ON when the key is absent") - that mismatch is exactly the "proven = ON by default" rule. today
  `misc_gestures_d59::SetPet(ReadIniBoolExplicit(...,"VrPetGestureD59",true))` (mod.cpp ~4133).
- washing (`washing_d59.h`, `VrWashGestureD59`, default OFF): dip both open hands in the tub's upper trigger volume 100 ms,
  raise to the face 70 ms -> native wash. the native outcome is a 3300 ms behaviour-tree wait + two `washFace` events at
  1.1 s and 2.85 s - speeding the clip can not line that up. "Washing face has repeatedly failed." in every headset
  report; the D71 INI had it false while i thought it was on. open.


### HAND INTERACTION LASER, 6DOF INTERACTIONS

- **V66.69 F4 rung 2**: "When the right-hand item state
  is definitively EMPTY and Grip is held, the native KCD2 interaction ray is redirected to the controller/index-frame
  origin and AIM direction. UNKNOWN is not treated as empty. A thin red OpenXR ribbon visualizes the same ray and the
  central combat/interact cursor is hidden while active." the red texel is still in vr_menu.cpp ("V66.69 F4
  hand-interaction red laser texel"). the `interaction_alignment.cpp` candidate projection (`VrInteractionAimConeHalfAngle
  Degrees`, `VrInteractionAimRadiusMeters`) and the H40 door collector grew out of it. R104 moved all lasers/HUD/menu to
  checked SteamVR quad layers (section 2).
- "6DOF interactions" = the station hand mode `minigame_hands45::SixDofVrik` (`VrSmithingMode` / alchemy mode, FREE 6DOF
  default since D72, `VrStation6DofFallback`) - that is crafting (section 8), listed here only because i tested it and
  called it working, so it stays ON by default.

### "WE DON'T ADD ANY MORE BLOCKERS" + load re-arm

my rule (R96, 4 Nov): "don't add any more blockers which cancel functions forever, even after loading the save, until
restart". almost every round had added a fault latch somewhere ("a native SEH on a stale object -> feature off for the
session"), and i kept hitting features that silently died after one hiccup. R96 found 23
"off until restart" latches. what exists now:
- `core/observed_load_recovery_r96.h` = ONE positively observed loading episode for the whole mod (from
  `inventory_lifecycle_t8`). `core/load_rearm_r96.h` `Seen::take()` = true once per COMPLETED load for the owner
  that calls it (first call only takes the baseline). R97 merge: ONE load signal, no second counter from mod.cpp.
- `subsystems/physics/load_fault_recovery_r96.h` `Fault` = a latch that clears itself when a load completes after the
  fault: `gore32n::g_drive_fault33` (impale hang/hold, corpse drag; `lab58n::g_dragFault` is now an alias), `g_corpse_contact_
  fault_h7`, `g_prop_fault5916c1` (physical pickups).
- `RearmAfterLoadR96()` in frik_pose_probe.cpp (~3958-3982) re-arms the plain flags once per load and writes ONE line
  `R96 LOAD_REARM after save/level load: DECAP_LOOSE_NECK NPC_TOUCH BLOOD_CONTACT_FX NPC_FLINCH FISTS NATIVE_COMBAT_REQUEST
  FINISHER_UPPER ATTACK_CLOCK_COMBO_HOLD WEAPON_PREDICTION TORCH_GESTURE PHYSICAL_WEAPONS_HOLSTERS` (only the ones that were
  set). `g_pose_restore_fault6689` is owned by the R96R1 pose-restore debt, not touched here (clearing just the flag would
  skip the restore of the failed joints).
- a latch is EITHER a `Fault` OR a plain flag re-armed here, never both (no double reset).
- here specifically: pickup drive fault = next squeeze OR load; small-item reader = retry every 5 s; H40 VM
  quarantine = lifted on player/body change; static-grab swap table full = refuses until the next load only; corpse
  leash = normal release, regrab at once; R103 self-collision lease = ends at the latest 4 s after release.
- what stays OFF on purpose: real game-code byte mismatches (pins, retail build, hook prologues) - "they stay off and say
  so" with one log line - and `anvil_physics_a3::faulted` (a native SEH in GetEntitiesInBox), and the physics-bodies lab
  (a retry could leave bodies). `native_grasp59c13.inl GraspFault13` quarantine is lifted only when the native hand no longer
  holds the item - after a load with a different character it may not lift; never fired in a log (R96 §9), still a risk.
- the two things that are NOT headset proven and can still look like a blocker: pose-restore can keep VRIK off until
  restart if the exact failed buffer never comes back AND the player lifetime never changes.

### if you want to fix X, look here

- Henry bends / item jumps on a normal pickup: `native_grasp59c13.inl` ~362-367 (SOCKET_PENDING writes nothing), ~1069
  (`JOB_ATTACH_ADOPTED`), `action_time_owner59c11.cpp TickOwner`, the H23 clip clock in `native_pickup_clock_h23.inl`.
  proposal: start the fragment at Attach time.
- instant pickup does not move the item: `staged_pickup596c2.inl` ~373-430 (stuck check, `kinematicR96`), `prop_drive5916c1.h`
  (servo). why SetVelocity does nothing on a freshly physicalized PickableItem is the open question.
- item stays static / "pickup: <name> stays static": `static_grab_r89.inl Prep()`, `SmallPickupAdmissionR24` (staged_pickup
  596c2.inl ~416-508), `zzkcd2vr_lab.lua TryPreparePickupR24`.
- mug/plate can't be grabbed: `static_grab_r89.inl TryGrab/NearbyR100`, `brush_swap_r100.h B::choose/cleanPath/brushRule`,
  `brush_swap_native_r100.inl SwapPinsR100/SwapReadBrushR100`, Lua `SwapSpawnR100`. log `R100 BRUSH_SWAP result=`.
- held prop pushes you / spins / sits wrong: `self_collision_r103.h/.inl`, `CarryLimitsR103`, `CarryHandR103`.
- door won't take the left hand: `hand_only_h40_input.inl` (knownOther rule). doors dead after a script reload:
  `hand_only_h40.h` ~282-300 (VM_REPLACED lift). door angle wrong on a door type: the clip inversion in the H40 Lua.
- holster zone wrong / can't draw: `physical_weapon.h holster_zone126 / holster_mount126 / shoulder_holster_closest131 /
  draw_volume_ok_hs2`, `holster_gestures.inl` (`PhysDrawCandidate126`, `PhysDrawRefuseR8d`), log `R8D HOLSTER_ATTEMPT
  firstMissing=`. holsters dead after the menu: `user_modes175.h` (Hands mode must keep weapon owner).
- stowed bow moves with native swings: the staged BONE-draw-leaf bridge in ATTACHMENT_LEAK_TRACE_20261030/patch (never
  shipped); hide-only workaround `stowed_bow_visibility.inl`.
- corpse: search cost `corpse_grip_cg1.h SearchWindowR89`, grab `saved_corpse_drag_d72.inl` / `lab58.inl DragTickSide`,
  stretching `corpse_follow_r101.h`, pushing `self_collision_native_r103.inl Exclude(Kind::Corpse)`.
- knives: stock `native_supply_publication.inl:130` (needs daggers>0), zone `physical_weapon.h knife_left_hip_d85r5`,
  draw boundary `native_ammo_service_t22.inl` (`drawBoundaryD53`), kinds `AmmoKindsT17`, refill `menu.inl` K34.
- petting: `pet_gesture_d59.h` (8 cm / 120 ms / 0.20 m numbers), `pet_service_d59.h`, `dog_candidate_d59r1.inl`.
- a feature died until restart: grep the flag in `RearmAfterLoadR96`; if it is not there and not a `Fault`, that is the bug.


---

## 7. RANGED WEAPONS AND RELOADS (bow, crossbow)

the bow works (Oct 11 in the headset: "F11 rung2 bow PERFECT, make it default"; Nov 1: "bow is fixed"). the DAMN crossbow ate more rounds than anything else in the mod and is still only partly proven in the headset. read the dead ends before touching anything, each one cost a build. paths are `src/subsystems/ranged_reload/` in the R106 tree unless written otherwise; version tags in the code comments say who wrote what (V66.128, C63R2, D65C, R89, R96, R98, R103, R105 ...).

#### quick map - who owns what today

| thing | file / function | switch |
|---|---|---|
| bow draw (right Grip on the string, pull, release = shot) | `src/vrik/physical_weapon.h` bow kernel (bow_pull63 charge), `native_bow_commands.inl` BowBegin/BowFire -> `PhysBowCall6698(player,0/1)` | `BowGripDraw=1` (R59.6C1 default) |
| bow geometry + string visual | `bow_placement_lifetime.inl` (C10R2 receipts), `bow_geometry_state.inl`, `bow_string128.inl` | `BowStringFollowsHand=1`, `VrBowStringGlue=1` |
| arrow from the quiver | `staged_reload596c2.inl` ManualBowTick596 / ManualCreate596 / ManualNock596, policy `src/vrik/staged_hold596c2.h` stage596::Loader | `VrManualBowDefault59=true` (Immersion "BOW RELOAD") |
| arrow in the hand | `manual_arrow64.inl` ManualArrowAttachment64 (C63R2), `arrow_hold_d48.h` | `VrArrowHoldNativeTurn=true` (Experiments > VIEW) |
| bow shot direction | `bow_physical_aim_shoot_d65c.inl` Shoot detour WHGame+0x289F7A0 | `VrBowPhysicalAimD65C=true` |
| bow camera / zoom | `bow_camera_service.inl` (V66.127, C63R2, D37B), `bow_zoom121.inl` | - |
| crossbow HAND ONLY route (light string, bolt for all 3) | `crossbow_hand_d89.h/.inl` CrossbowHandTickD89, CrossbowHandDeferD89, XbowNativeGiveBackR96 | `VrCrossbowReloadRouteR96=true` (Experiments > INSTANT page, last row "Crossbow reload route") + `VrCrossbowReloadD65C=true` |
| crossbow NATIVE-DRIVEN route and the spanner cocking of class 14/15 | `crossbow_native_xr34.h` (pure Client: tick/restore/interval), `crossbow_native_xr34.inl` (native hooks), `crossbow_rifle_request_h21.inl` (request gate, tool/bolt/weapon writers, BuildCrossbowPosesXR34), `reload_passenger_xr34.inl`, `crossbow_mechanism_d78.inl`, `crossbow_constraint_d78.h` | `VrCrankTravelD66C=0.08` (class 15) |
| instant reloads (native tail speed only) | `kcd2vr::instant_d72::Reloads()` | `VrInstantReloadsD72=true` ("Fast reload transitions") |
| stowed bow hidden on the back | `stowed_bow_attachment.inl` + `stowed_hide_hw.h` (D33) | `VrHideStowedBow=2`, `VrHideStowedBowAttachments=weapon_bow` |
| crossbow zoom off | `options_xr32.h` / `menu_xr32.inl` | `VrCrossbowNoZoom=true` |

### BOW

#### what i wanted

Skyrim VR style. left hand holds the bow, right hand takes an arrow from behind the right shoulder, puts it on the string, pulls, lets go = shot. the arrow should sit in my hand, the string should come with my hand, the shot should go where the bow points, not where the game's animated camera sway wants. and no reload animation looping in my face.

#### history

1. **V63.8.1 research (Sep 2026)** - the plan. kcd_bow_database.adb: Charge (LayerManualUpdate, `bow_pull_back_over`, parameter `ChargeFraction`), Load, Fire, Uncharge; bows attach to `hand_left`, rig `bow_rig.chr`. decision already then: use the game's own bow state machine, left controller = bow, right = string, release = native Fire, never spawn our own projectile.

2. **V66.102 / V66.107 (early Oct)** - first quiver guess: right Grip over the right shoulder (22-28 cm) = arrow, nock at a guessed string point, release Grip = shot. bow drawn/holstered with the LEFT hand over the right shoulder. never proven.

3. **V66.121 - V66.126** - my run on V125R1: 20 bow starts, 18 geometric cancellations, ONE arrow flew. V66.126 went back to plain RIGHT TRIGGER = charge, release = shot, left Grip holds the bow. DEAD END: guessed nock/string axis with no measured bow geometry cancels almost every draw. V66.121 killed the bow zoom (`BOW_ZOOM ARMED`).

4. **V66.127 / V66.128** - bow camera stabilizer (the aim animation pushes the game camera forward on Henry's body; cancelled vs the idle anchor, 170 ms blend, 40 cm cap) and archer stance. V66.128 did the static work everything later stands on: `WHGame+0x2A8AD10` = C_MissileWeaponItem vtable slot 22, called once per nocked arrow, takes the attachment named **"arrow" on the BOW's own character**, reads the arrow model's **"slt_0"** helper (the nock) and sets the attachment offset to its inverse. the bow skeleton plays `bow_pull_back` via LayerManualUpdate driven by ChargeFraction = string and arrow are posed by native charge TIME, not by my hand. so 128 added `bow_string128.inl`: learn the bow's rest pose (idle >=300 ms) and full-draw pose (charge >=0.97), while drawing replay only the joints that differ at a fraction from the right hand along the learned draw axis. visual only, ChargeFraction/stamina/ammo not written. INI `BowStringFollowsHand`.

5. **V66.146R1 - V66.160** - "true bow reload" `BowManualReload=1` (right shoulder + Grip, `BowNativeGrabAtQuiver=1` starts the NATIVE load so the game's arrow appears in the hand, carry to the string, pull, release Grip) and V66.160 `BowStringTrigger=1` (TRIGGER takes the string). also: while the bow is out the right shoulder is the quiver, no weapon draw there (rule still alive). these INI keys still exist but are legacy, the staged loader replaced them.

6. **V66.172 / V66.175 (Oct 10-11)** - `select_bow_policy175` chooses the DRAW policy separately from the LOAD policy: physical string draw with native automatic loading by default, quiver loading optional. 175 also fixed `BowSkinning128` relearning the drawn pose as rest (`BowLearnNativeSample175`).

7. **V66.178R9 / R36C1 / R46C1** - string glue (`VrBowStringGlue=1`: string joint + arrow children at the string hand in bow-local space, 6 cm beside the authored path, never past full draw), Grip pose attachment (`Bow_CenterString` / `Bow_Arrow` pinned to the completed right wrist), "connected bow": rest/full pair frozen per rig lifetime, whole hierarchy at ONE draw fraction - that removed the sideways drift and the "trapezoid string".

8. **V66.178R59.6C1 (Oct 11)** - `BowGripDraw=1` default: right Grip latches the string, pull = draw, release Grip = shot, native auto loading. the draw i called PERFECT in F11 rung 2 and asked to make default; not retuned since. R59.6C2 added the F11 rung 2 staged loader: `staged_reload596c2.inl`, quiver box follows head yaw only (6-55 cm right, 6-55 cm behind, -35..+25 cm vertical), nock 14 cm / 80 ms, `ManualCreate596` = native hand-arrow create (manager 189F3C0, helper 28959D8), `ManualNock596` = remove 40B730, bind 289C988 (289C930 order), 393524(eq+28,1), tags C5F484.

9. **C10R2 (Oct 13)** - false cancels: `PhysApplyBowGeometry166` dropped the bow-local geometry after 150 ms and a missed refresh (skinning vs frame >3 cm / 0.08 rad) killed a live draw. fix: geometry lives while a fresh (<=80 ms) placement receipt with the same lifetime serial exists (`bow_placement_lifetime.inl`). still true today.

10. **C19C2 -> C19C4R4 -> C40 -> C56 -> C63R2 (Oct 15-19)** - the manual arrow. on R38 i said "the hand is holding the arrow, ok, nice, but the arrow stays not on the bow then". C19C4R4 (`manual_reload_attachment19c4r4.inl`): the native arrow must ACTUALLY attach to the hand before nocking (QUIVER_CREATE -> HAND_ATTACHMENT_ACK -> NOCK_REQUEST -> NOCK_ATTACHMENT_ACK, 500 ms `NOCK_ATTACHMENT_UNCONFIRMED` deadline = `Loader::NockAckBoundMs`). Oct 17 change: RELEASE near the nock leaves the arrow on the bow instead of holding Grip through the nock (`releaseToNock56`); C40 did it, C56 fixed a release + quick new Grip being lost (one-use `BowNockAck19c4r4`). C63R2: ONE placement owner for the hand arrow = `manual_arrow64.inl` ManualArrowAttachment64 (slt_0 helper + committed VR right hand, nock gap from the real arrow end, cue 14 cm / rearm 18 cm). C63 charge fix: charge = signed CHANGE of axial string travel vs the latch rest (the bug: rotating the left wrist alone gave charge .1247 and fired).

11. **C48 (Oct 18)** - first-draw rotation. geometry can now go structural rest -> corroborated partial axis (3 native ticks agree) -> full pair without a calibration shot; the first-draw writer interpolates the whole native draw chain toward rest by pull distance. bow torso share 45 %/35 deg -> 30 %/25 deg.

12. **R8D2 / H3 (Oct 25-26, from my captures)** - (a) the right-shoulder POLEARM draw stole the quiver reach (16:51:38 bow STOWED, polearm DRAWN) because `holster_gestures.inl` reserved the quiver only under the legacy `g_bow_manual_reload146` -> reserve the right shoulder while a bow is held and `StagedBowEnabled59()`. (b) bow flicker: 86 attachment restores "reason 8", the R20 repeat-profile validator treated a one-bit draw-length change as loss of the whole bow -> primary bow cache validates item/generation/entity/grip only, roundoff 1e-6.

13. **D37B (Oct 28)** - "view jumps when i draw": a busy try-lock in `TrackedFrameCameraCoreE1` dropped the stable camera base and the native bow camera came back (`TRACKED_CAMERA_CORE_BUSY`). camera, section 2.

14. **D47 / D48 / D56 (Oct 29-30)** - arrow wobble in the hand: the arrow sits on `entity_right` (RightHandSlot); in the load clips that bone turns 177-180 deg against RightHand and C63R2 copied that onto my wrist. D47: default-skeleton slot orientation, INI `VrArrowHoldNativeTurn`. i tested both and chose NATIVE TURN ("like i was holding the arrow wrong before"), D56 adopted it once (`arrow_hold_d48.h`, `VrArrowHoldNativeTurnD56Adopted=1`).

15. **D65C (Oct 31)** - shot direction. Fire (0x28999C8) calls Shoot at 0x289A102 (return 0x289A107) with dir/up/pos from the animated bow + sway (0x289C0D8). `bow_physical_aim_shoot_d65c.inl` hooks Shoot: for that caller it takes a one-shot snapshot published by BowFire (dir = grip->nock reversed, up = limb axis, pos = drawn nock, <=1500 ms old) and passes LOCAL copies; speed/payload untouched. refusal = native aim + "bow: physical aim unavailable (<reason>)". also every HAND_UNAVAILABLE cause named and a 250 ms draw grace (`kBowDrawGraceMsD65C`) so a one-frame geometry dropout (my log: bow entity 3.7 cm off, threshold 3 cm) does not cancel a held draw.

16. **D75/D76** - `draw_visibility_d45.inl` / `first_draw_binding_d75.inl`: the drawn bow stays hidden until the first VR placement readback (no native pose flash). D79 (Nov 1): "User confirms bow is fixed. Preserve D79 bow aim/orientation/draw." nothing on the bow changed after that on purpose (D83 review even rejected an overlay where a stray `//` swallowed `m.nockRelease56=...`).

#### what works now (bow)

- draw: left Grip holds the bow (`BowHandlingMode59c9`), right Grip near the measured string rest latches (`bow_pull63`), charge = signed axial travel from the latch baseline, release Grip = `PhysBowCall6698(player,1)` -> native Fire; needs a positive pull and the native charge ACK (`n.bowChargeObserved114`). stamina/ammo native. shot direction from the Shoot detour (D65C), row `D65C BOW_SHOOT aim=VR dir=.. deltaDeg=..` per shot.
- loading (`VrManualBowDefault59=true` -> `StagedBowEnabled59()`): after a shot, right Grip behind the right shoulder (yaw-relative box, `stage596::Loader`) -> `ManualCreate596` (28959D8, right hand) -> wait for the REAL hand attachment (19c4r4) -> `ManualArrowAttachment64` puts it on my wrist every frame, slt_0 nock at the wrist, native slot turn (D48) -> release within 14 cm of the string rest = `ManualNock596` once (40B730, 289C988, 393524, C5F484) -> `nockRelease56` ACK -> fresh Grip on the string draws. release away = hand arrow removed, no ammo spent. the game's auto-reload (18468BC from callers 2AAA012 / 1F24836 / 1F2469E / 224A38C / 222CB40 / 1F28766) is swallowed by `DeferredReload596` while the loader owns the bow, so the load animation never loops.
- string: `bow_string128.inl` + glue, whole chain at one fraction (R46C1), rest/full learned per rig lifetime (C10R2 serial).
- camera: `bow_camera_service.inl` keeps the model-local camera through quiver/nock/draw/release (C63R2, D37B); zoom killed in `bow_zoom121.inl` (caller 0x10610B9 only). first draw after equip: bow hidden until the first placement receipt (D45/D75); the first draw of a session has an arrow already nocked.

known open: the arrow mesh shows only once the native attachment is confirmed (a few frames after the quiver Grip); `NOCK_ATTACHMENT_UNCONFIRMED` = the 500 ms deadline hit, release/regrip. stamina drains by native hold time, not my pull. `BowManualReload`/`BowStringTrigger` keys are dead weight.

#### if you want to fix X (bow)

- arrow sits wrong in the fist -> `arrow_hold_d48.h`, `manual_arrow64.inl` BuildManualArrowPoses64, row `D53 F11_STAGE ARROW_BASIS localQ=`.
- draw cancels -> `physical_weapon.h` HandUnavailable branches, BOW_STATE `HAND_UNAVAILABLE_<CAUSE>`, `kBowDrawGraceMsD65C` 250 ms, receipts in `bow_placement_lifetime.inl`.
- nock never happens -> `staged_reload596c2.inl` ManualBowTick596, `stage596::Loader::update`, `manual_reload_attachment19c4r4.inl`, rows `C63R2 F11_STAGE HAND_ARROW_POSE`, `NOCK_*`.
- shot goes elsewhere -> `bow_physical_aim_shoot_d65c.inl`, row `D65C BOW_SHOOT`.
- string not following -> `bow_string128.inl` (`V66.128-BOW_STRING FOLLOW`), `bow_string_follow_control.inl`.
- reload animation plays by itself -> `reload_policy.inl` StagedBowEnabled59, autoload caller gate (`ManualAutomaticReload19c3`, pins in `reload_lifecycle178r36c2.h`).

### CROSSBOW

#### what i wanted

same as the bow. crossbow in the LEFT hand, right hand works. light: grab the string, pull back, cocked. goat's foot (medium) / cranequin (heavy): take the real spanner from my chest, hook it on the string, pull / crank. then a bolt from behind the right shoulder into the right hand, lay it on the rail. no Henry animation doing it for me. Nov 4: "can I finally pull the string or spanner myself and hold the bolt in my right hand to load ALL crossbows? or is it still giving control back to the native animation?" and about the bolt: "LOOK how we made it for the DAMN BOW ... why is the bolt not similar?"

#### facts about the game (proven static, WHGame SHA bdf8f9e4..., timestamp 0x6A350E20)

- weapon_class.xml: 10 light, 14 medium (loading_item_first = crossbow_goat_foot), 15 heavy (crossbow_lever, attach_lever_to_weapon=true), 13 rifle (`weapon_kind_xr32.h`; H21 first called 14/15 rifles, fixed in XR32).
- the reload is a NATIVE ACTION requested via 18468BC(equipment, DL, R8B) -> 2896174 -> allocator 2886B24; parent class 3E78CF8 (main+98), child 3A450B0 (+70). OnStart 2897A14 creates the TOOL item into main+80 (289F620) before anything is visible. event dispatcher 289CCD0 (12 effectful CRCs): SpawnLever AFE0B3FF / DespawnLever 3D6870EB -> 289F338(main, show) only un-hide/hide that item; LoadReachedCocking 8ABB129A -> 2895F30 -> 2895F74(player) [1741C80(comp,1) = comp+D8 cocked] + 2ACD5D4(eq,2); LoadReachedAmmo 34857AFA -> 289595C(action,1) = 28959D8(mgr, player, &action+88) creates the bolt in the hand (side 0 = right, all 15 recorded times); LoadReachedWeapon EF667DEA -> 289C930: remove 40B730, bind 289C988 (2A8A0C0 stores the ammo key at comp+E0, 2A8A030 weapon-side visual), 393524(eq+28,1), C5F484 tags. the heavy clip has a native typo event "DespawLever" (CRC 0x441B89A7), treated as inert.
- default skeleton joints (`RangedDefaultsXR34`): "Crossbow_arrow" (0, .138, .0596), "Crossbow_<light|medium|heavy>_string" y = .2317 / .2695 / .3123, z = .05975 -> string travel light 9.37 cm, medium 13.15 cm, heavy 17.43 cm, weapon-local.
- the game RE-REQUESTS the reload while the crossbow is not loaded (`equipment+50 == 0` -> 2ACE8A0; callers 224D631, 737A74, 222CB40, 2AAA012). every NATIVE_STOP in every log is 1-12 ms after such a request. the biggest reason "ride the native action" never finished (R89).
- bow and crossbow share the bolt factory 28959D8 and the remove/bind order; the bow's `ManualNock596` already uses it.

#### history

1. **H21 (Oct 20)** - `crossbow_rifle_request_h21.inl` + `crossbow_rifle_reload60.h`: "park and replay". `CrossbowRifleDeferH21` parks the game's request, `rrH21::Machine` kSpan/kPlace does the gesture (grip within 20 cm + pull >=10 cm; grip at the quiver, release at the rail), then `RangedReplayH21` replays the SAME request into 18468BC and the whole authored animation plays. F11=2 only. the H21 note itself: "do not call a replay a physical reload". the rifle (class 13) still sits here today, never tested.

2. **XR32 (Oct 22)** - class ids fixed (14/15 are crossbows, not rifles), `VrCrossbowNoZoom`, span measured weapon-local instead of world distance (walking earned a pull).

3. **XR34 (Oct 23)** - "ride the native action", still the NATIVE-DRIVEN route and the 14/15 cocking: let the game start its reload child, hook OnStart 0x2897A14 (`XR34PostStartNative`), write child+0x5C playback speed = 0, hook the clip metadata 0x289D788 (`Client::metadata`, CAF event list) and the anim scan, then every controller update `Client::tick` sets the speed so the native clock reaches exactly the earned target; `interval()` refuses any scan past the permitted event. phases AwaitClip -> Setup (clock at spawn-1e-4 until the tool sits on my hand) -> Span (neutral <.25, grasp >.65 within 12 cm, progress = axial pull / travel) -> Tail (native cock); loading child: Setup (clock to ammo+1e-5 -> native bolt in hand) -> TakeBolt -> PlaceBolt (release with the bolt slt_0 within 8 cm of "Crossbow_arrow") -> Tail (native load). anything wrong -> `restore()`, Henry finishes. working hand = VRIK passenger (`reload_passenger_xr34.inl`), tool/bolt on my wrist via `CrossbowAttachmentXR34` in the CEntityAttachment update hook, poses from `BuildCrossbowPosesXR34`.

4. **D46/D47 (Oct 29)** - first logs: XR34 never owned a reload (construct->LoadReachedCocking 4.18 s = authored rate), unowned unless F11=2. D47 named the refusals. four complete native cycles reconstructed.

5. **D65C (Oct 31)** - `VrCrossbowReloadD65C` default ON (replaced the F11=2 gates), heavy refused by name (no producer set crankQualified), visible lines "crossbow: native reload (<why>)" / "reload gesture lost, native finishes it (<reason>)", `reasonName()`.

6. **D66C (Oct 31)** - heavy crank: class 15 admitted, grip = heavy string joint, axis = string -> stirrup (FORWARD), travel `VrCrankTravelD66C` .04..0.20 default .08, lateral <=5 cm, vertical <=4 cm, chest gate (hand 10-40 cm below the head within 25 cm, then back within 30 cm of the string, `kChest*D66C`, reason WaitingChest). a push stroke, NOT a rotary crank. the headset never saw a class 15 reload, to this day.

7. **D72 (Oct 31)** - my "INSTANT activities" ask. the gesture itself (chest/string Grip, `CrossbowGripRequestD72`) submits the ONE native request, not "press LT first". `VrInstantReloadsD72` = speed of the native tail only. my recording: three START callbacks `CURRENT_NOT_OWNER_THREAD`, one TAKEN then `CLIP_UNSUPPORTED`.

8. **D62C -> D81 (Oct 30 - Nov 1)** - one gate per build (R89 counted it): D62C 6 requests/0 taken, D66 5/0 (START_REFUSED), D74 7/3 (CLIP_HEADER_SEGMENT_DURATION_OR_EVENT x5, PHYSICS_FRAME_STALE x2), D79 3/3 (CLIP_DUPLICATE_OR_AMBIGUOUS_SEMANTIC, NATIVE_FALLBACK), D80 5/5 (INPUT_LOST 80 ms first pose x3, CLIP_SCOPE_CHARACTER_ANIMATION_SET x2), D81R1 3/3 (INPUT_LOST 94 ms, NATIVE_FALLBACK x2). D78 added the live string/lever join (`crossbow_mechanism_d78.inl` reads the string joint from weapon skinning, `crossbow_constraint_d78.h` pins the wrist to it).

9. **D82 (Nov 2)** - the scope bug: `XR34NativeClock` and `ReloadPassengerCurrentXR34` required ONE layer slot in the native root scope; the real scopes are UpperBody (first layer 6, 2 slots) and FullBody (0, 3 slots). `selectedScopeClockEntryD82` fixed both gates; foreign CAF scans and native-inert foley events no longer cancel.

10. **D83 (Nov 2)** - an overlay rejected in review: `CrossbowSpanTickD82` used `abs(dot(delta, railAxis))` so an 8 cm PUSH cocked, and it was a second owner next to XR34. kept from it: right-shoulder quiver arbitration for a held crossbow (polearm "stow the weapon" refusals at 00:36:02 in the quiver zone).

11. **D85R2/R3/R6 (Nov 2-3)** - ownership reads the completed source packet, busy scans don't revoke, read-only scans don't bump the publication, duplicate CAF in the FIFO found (`NATIVE_CLOCK_NOT_QUALIFIED`). D85R5 was the first build where a reload reached Span with my hand on the string.

12. **R88 (Nov 3)** - D85R5 log: both cockings died 500 ms after the grab with `GEOMETRY_UNAVAILABLE: NO_STRING_OR_LEVER_POSE` - D78 made the live string join (about 40 exact-equality checks on the skinning thread) mandatory for stroke credit and it never published in the game (`SkinningTransformCoreDetour` returns before `CrossbowMechanismSkinningD78` unless a recorder runs). fix: for class 10/14 the string pose is optional, pull measured from the raw controller against the default string rest. and: bolt taken, then 7 s nothing - the placement receipt handshake (h21:529-583) never completed in any log, so the tracked hand carries the bolt and opening it within 12 cm of the rail places it. (R89 corrected one R88 claim: HAND_CENSUS handId=0 does not mean no bolt in hand; the game created it all 15 times.)

13. **R89 (Nov 3)** - the bow-pattern rewrite `crossbow_hand_d89.h/.inl`. the count: 39 reload starts over 7 builds, 22 taken by XR34, 0 cocked by hand, 0 bolts placed, 0 loads by hand. decision: never start the native reload while the route is on (`CrossbowHandDeferD89` swallows 18468BC with r8b==0, like `DeferredReload596`), own state machine `CrossbowHandTickD89` in the crossbow's frame: grip after an open hand within 12 cm of the string rest (`kStringReach`), pull >= 90 % travel (`kPullShare`), lateral >15 cm = slipped; cock = `2895F74(nullptr,player)` + `2ACD5D4(eq,2)`, readback comp+D8; bolt = `28959D8` (side 0 only) + `393524(eq+28,0)` + C5F484; open the hand within 12 cm of the rail (`kRailReach`) = 40B730 remove, 289C988 bind, readback loaded, notify, tags. leaves + handlers byte-pinned (`XbowPinsD89`). one line per refusal (`Note` texts).

14. **R91R2 (Nov 3)** - XR34 repairs: mechanism observer independent of the completed pose, reload source shares the body publication transaction, socket found before the body commit, rigid local grip per action/entity/socket, regrip uses the remaining travel. R91 added `crossbow_hand_owner_r91.h` (per-frame Admission which route owns the input, 80 ms).

15. **R93R4 (Nov 4)** - every give-back trigger (INPUT_LOST 80 ms / 500 ms first-pose join, GEOMETRY_UNAVAILABLE 500 ms string/lever join, PHYSICAL_BINDING_LOST with 80 ms same-binding recovery, unearned interval, context loss). Nov 3 capture: seven class 14 admissions lost ownership 30-63 ms in.

16. **R94R1 run (Nov 4, my logs 17:44/17:50/18:05)** - "crossbow / spanner / bolt flicker like crazy", "can't even reload natively". cause: the three attachment writers (`CrossbowWeaponAttachmentD72`, `CrossbowAttachmentXR34`, `CrossbowHeavyToolAttachmentD72`) returned false = native writes it whenever the reload payload was missing (80 ms retain then `withdraw()`), and the R94 publication gap made it missing every other frame -> VR hand, native hand, VR hand (D77 rows 319408-319446 toolKey flipping). XR34 took EVERY start incl. the game's own request, waited for hand input and stopped -> no LoadReachedAmmo in the whole log. and HAND ONLY was dead: `CrossbowHandOnlyR90()` returned false, R92-R95 wrote `VrCrossbowHandOnlyR89=false` to the INI on every exit.

17. **R95 / R95R1 (Nov 4)** - only a physical Grip admits a new VR reload (`acceptsNewActionR95`), LT stays native; event-free transition advances without the aux tool packet. R95R1 bolt: crossbow quiver reach = the bow's yaw-relative region (the old small sphere missed a hand 30 cm right / 40 cm behind the head), raw quiver intent survives the publication gap, a bolt whose attachment is still completing is "pending" not lost. the failed recording stopped at normalized 0.0099, just before LoadReachedAmmo at 0.0100: the bolt was never created (338 rows boltKey=0).

18. **R96 (Nov 4-5)** - (1) `VrCrossbowReloadRouteR96` (default true = HAND ONLY, Experiments INSTANT page last row, never refused) - a NEW key because the old one was poisoned; all three classes. (2) flicker: `hold()` in the three writers - while the same XR34 owner is current (<=80 ms) the item stays at the tracked wrist with the captured grip, no receipt, no credit; real loss still goes native. (3) parked requests are given back after 1.5 s of idle hands (`XbowNativeGiveBackR96`, `kNativeWindowMs=1500`, replays the same 18468BC, row `NATIVE_REQUEST_REPLAYED reason=HANDS_IDLE_NATIVE_RELOAD_R96`); a native action XR34 does not own passes whole (`NativePass`); HAND ONLY lets XR34 take only a NEW chest grip (`HAND_ONLY_ROUTE_LOAD_IS_NOT_XR34_R96`). (4) bolt on the bow's writer: `ManualArrowBeginCrossbowR96` / `XbowBoltLeaseR96`, own serial `g_manual_arrow_xbow_serial_r96` so the bow tick never withdraws it. (5) the spanner only exists inside the game's cocking action (289F338 un-hides it), so an uncocked 14/15 is `toolCocking` -> chest grip goes to XR34 hand-driven; R89's invisible "Tool" stage dropped. (6) latches `g_xbow_fault_d89` / `g_ranged_fault_h21` now clear per attempt (`XbowFaultRetireR96`).

19. **R96R1 + R97 merge (Nov 5)** - R96R1: one non-renewing 500 ms deadline for the first tool/bolt placement (`setupJoinCurrentR95R2`, reason `NO_FIRST_HELD_ITEM_PLACEMENT`), early release retires the grasp (`releasedAheadR95R2`), spanner grip times body scale, heavy tool 80 ms retention. merge problem: both sides together made the heavy writer "gap -> retain 80 ms -> withdraw" = flicker after 80 ms. fixed as ONE order `retainOrHold` (h21:747): retention first (no write), then hold (write, no receipt), hold as strict as the retention.

20. **R98 (Nov 5, my R97 run, MEDIUM crossbow only)** - (a) bolt in the hand 90 deg up like a dagger: `D53 ARROW_BASIS localQ=-0.70711,0,0,0.70711`, no crossbow clip turns the ITEM_R slot (joint 417) so the rig default put the bolt along the thumb side. fix `CrossbowBoltForwardR98()` = +90 deg about hand +Y, bolt serial only (expected localQ -0.5,0.5,0.5,0.5). (b) lever 180 deg: the grip was captured at clock 0.0000 before SpawnLever (0.2057); fix: capture only in Span/Tail. (c) string flicker while pushing the lever: the D86 string witness only existed on frames where the clock did NOT move, wrist pinned / free alternated; fix `laggingStringClockR98` (witness <=0.05 behind) for the pin only.

21. **R101 run -> R103 (Nov 5)** - lever at the chest, the game's hand did the cocking. log 1353 `hand reload stopped (GEOMETRY_UNAVAILABLE: NO_FIRST_HELD_ITEM_PLACEMENT)`, then `bolt in hand`, `loaded` - the BOLT PART WORKED in the headset (HAND ONLY, class 14). the lever: R98's "capture only in Span/Tail" was a DEADLOCK (Setup needs a receipt, the receipt needs a target, the target needed Span), so every class 14 cocking since R98 ran out the 500 ms join. R103 fix (h21:1045-1072): capture in Setup at the SpawnLever edge (`clock >= spawn - 2e-4`, 2nd body at the edge), the same moment the cranequin writer calibrates. row `R98 XBOW_TOOL_GRIP ... clock= spawn= handInToolQ=`.

22. **R105 / R106 (Nov 5)** - spanner grip = `inverse(S*A)*H` from the rig's reference hand H, socket S, attachment A (reference basis like the manual arrow) instead of the animated slot at the spawn edge; placement stays `W*inverse(grip)`. kept in R106, untested.

#### DEAD ENDS (do not repeat)

- DEAD END: world-distance pull (pre-XR32) - walking earned a pull. everything weapon-local (`RangedDefaultsXR34`).
- DEAD END: "ride the native reload action" as the ONLY route (D65C..D85). 7 builds, 7 first refusals, 0 cockings by hand, and the game re-requests the reload every few hundred ms while unloaded (NATIVE_STOP 1-12 ms after each request). kept only as NATIVE-DRIVEN fallback and for the 14/15 spanner (the spanner is born inside that action).
- DEAD END: mandatory live string join (D78) for stroke credit - never published in the game. optional since R88.
- DEAD END: `abs()` pull displacement (D83) - a push cocks. signed dot along rest->rail.
- DEAD END: a second manual owner next to XR34 without swallowing the native request (D83) - two writers = flicker + double cocks.
- DEAD END: withdraw-to-native on a missing packet (D72..R95 writers) - that IS the flicker. retain, then hold at the tracked wrist (`retainOrHold`).
- DEAD END: hand-in-tool grip from the idle slot at clock 0 (R97) = lever 180 deg; "only in Span" (R98) = deadlock. spawn edge (R103) or rig reference (R105).
- DEAD END: session latches (`g_xbow_fault_d89` forever, `g_ranged_fault_h21` forever, the mod writing `VrCrossbowHandOnlyR89=false` on exit). a fault belongs to the attempt, the mod never rewrites the route key.
- DEAD END: inventing a spanner outside the native action (no known factory; 289F338 only un-hides main+80).
- DEAD END: one-slot native scope (pre-D82) - the scopes are 6+2 and 0+3.
- DEAD END: HAND_CENSUS handId to see the bolt - inventory key, always 0 for a reload bolt.

#### what works now (crossbow, R106)

route HAND ONLY (`VrCrossbowReloadRouteR96=true` + `VrCrossbowReloadD65C=true`), crossbow in the LEFT hand (`Note::LeftHandOnly` otherwise), `CrossbowHandOwnsInputR91` decides per frame.

- game request: `CrossbowRifleDeferH21` -> `CrossbowHandDeferD89` swallows 18468BC (r8b==0, Henry's equipment, class 10/14/15) and parks it. hands idle 1.5 s -> `XbowNativeGiveBackR96` replays it, the game reloads natively, line "crossbow: hands idle - the game reloads it". a native action XR34 does not own passes whole.
- light (10): open hand, Grip within 12 cm of the light string rest, pull along rest->rail 90 % of 9.37 cm -> `XbowCallCockD89` (2895F74) + `XbowCallCockNotifyD89` (2ACD5D4), readback comp+D8, "crossbow: cocked". the string does NOT visibly follow the hand on HAND ONLY, it snaps at the end. >15 cm sideways = "string slipped". NOT yet seen in a headset log (all my runs were class 14).
- goat's foot (14) / cranequin (15): uncocked = `toolCocking`, the chest Grip (`CrossbowGripRequestD72`) submits the game's reload and XR34 takes it hand-driven: Setup holds the clock at spawn-1e-4, the real spanner (main+80) goes on my wrist (`CrossbowAttachmentXR34` / `BuildCrossbowPosesXR34`, grip `inverse(S*A)*H` since R105, captured at the spawn edge since R103), past SpawnLever -> Span: grasp within 12 cm, pull 13.15 cm (14) or push `VrCrankTravelD66C` after the chest gate (15), progress drives the native clock, string/lever from `crossbow_mechanism_d78.inl`, Tail -> native cock. back to native only if the first spanner placement misses 500 ms (`NO_FIRST_HELD_ITEM_PLACEMENT`) or the owner is really lost (tracking >80 ms, tool replaced, save load). mounted cranequin lever is moved by the game on purpose. class 14 reached the bolt stage in the headset (R101) but the lever was native then; R103/R105 untested. class 15 never seen.
- bolt (all three): cocked + Grip behind the right shoulder (bow quiver box) -> `takeBoltSequence` = 28959D8 (side must be 0), serial `ManualArrowBeginCrossbowR96`, placed every frame by `ManualArrowAttachment64` with `CrossbowBoltForwardR98`, "crossbow: bolt in hand - open the hand on the rail". open the hand within 12 cm of the rest..arrow groove -> 40B730 remove, 289C988 bind, readback loaded, "crossbow: loaded". elsewhere = "bolt dropped", take a new one. PROVEN in the headset on class 14 (R101 run, 09:47:09 bolt in hand, 09:47:12 loaded).
- NATIVE-DRIVEN (false): the whole XR34 machine for cock and load, with the R95/R96R1 timeouts.
- refusals: one line each, KCD2VR.log always, on screen only while F2/F11 records (`XR34VisibleD65C`, `XbowShowR90`). GAME_CODE_DIFFERS = pins failed, hand route off for this binary, game reloads.
- holster side: while a bow or a crossbow with physical reload is held, the right shoulder is the quiver, right-shoulder draw/switch candidates (polearm!) are refused (`BOW_QUIVER_OWNS_SHOULDER`). left-hand crossbow draw from the back works (R94R1 18:10:09 class 14).

#### open issues (crossbow)

- light string does not follow the hand on HAND ONLY (visual only).
- 14/15 spanner cocking never seen complete in a recording; R103 (spawn-edge capture) and R105 (reference grip) are source-only.
- class 15 never tested; the forward-push sign may be backwards (one-line flip in `RangedSpatialXR34` class 15 block).
- 1.5 s idle give-back is fixed, no key. the native re-request loop still bites NATIVE-DRIVEN.
- `tools/test_crossbow_mechanism_d78.py`: 5 pre-existing failures (lines 137-144 expect the old 6-frame fallback).

#### if you want to fix X (crossbow)

- nothing happens on string/chest/quiver -> `CrossbowHandTickD89` (`runnable`, `toolCocking`), `crossbow_hand_owner_r91.h`, rows `R89/R96 XBOW_HAND stage= command= ok=`, `F11 RANGED event=NATIVE_REQUEST_DEFERRED/REPLAYED`.
- the game reloads by itself -> `XbowNativeGiveBackR96`, `kNativeWindowMs`, `NativePass`; pins `XbowPinsD89` (GAME_CODE_DIFFERS).
- spanner not in my hand / turned -> h21 `BuildCrossbowPosesXR34` (R103 spawn-edge block, R105 reference grip), row `R98 XBOW_TOOL_GRIP`, join `setupJoinCurrentR95R2` (xr34.h, 500 ms).
- spanner / crossbow / bolt flicker -> h21 writers `CrossbowWeaponAttachmentD72` (:639), `CrossbowHeavyToolAttachmentD72` (:714, `retainOrHold` :747), `CrossbowAttachmentXR34` (:896); D77 `F11_BINDING` toolKey flipping.
- pull earns no progress -> xr34.h Span block (`kPausedJoinMsD85=500`, contact 12 cm, lateral 12 cm), rows `D48_F11 RELOAD phase= reason=` (5 GRIP_THE_STRING, 6 WORKING, 18 GEOMETRY_UNAVAILABLE, 19 INPUT_LOST, 20 NATIVE_STOP, 24 WAITING_FIRST_POSE, 27 TAKE_A_BOLT, 29 WAITING_FOR_NATIVE_LEVER_POSE).
- bolt wrong in the fist -> `CrossbowBoltForwardR98` (manual_arrow64.inl:38), row `D53 F11_STAGE ARROW_BASIS localQ=`.
- bolt not created -> `takeBoltSequence` side check, `NO_BOLTS_EQUIPPED` from the T22 stock reads (`throw_drop/runtime/native_ammo_service_t22.inl`, equipped stack >=2 counts).
- heavy crank sign/travel -> `RangedSpatialXR34` class 15 block, `VrCrankTravelD66C`, `kChest*D66C`.
- CLIP_UNSUPPORTED / CLOCK_UNSUPPORTED / NATIVE_CLOCK_NOT_QUALIFIED -> `XR34NativeClip`, `selectedScopeClockEntryD82`, FIFO duplicate (xr34.h:245-257), `crossbow_clock_entry_d87.h`.

### STOWED BOW, QUIVER ZONE, ZOOM, RIFLE

- stowed bow clipped through my view when i looked up. C10R14/R15 (Oct 14-15): the holstered bow is a bone attachment on Henry (`weapon_bow` -> BackSlot_bow), hidden via GetInterfaceByName 0x971B2C + IAttachment::HideAttachment 0x5AF5E8 while HMD pitch > `VrHideStowedBowPitchDeg` (mode 1) or always (`VrHideStowedBow=2`, mine). D33 `stowed_hide_hw.h` generalised it: back items hidden in normal play, everything at alchemy/smithing, restore exactly what we hid; quiver attachments on the list.
- holster zones: bow = RIGHT shoulder, LEFT hand only (C10R14B); right hand at the right shoulder reaches polearm/crossbow/firearm, and NOT while a bow / physical-reload crossbow is held (quiver reservation, R8D2 + D83).
- zoom: `bow_zoom121.inl` returns 0 to the exact FOV consumer caller 0x10610B9 while the physical bow is active; `EnableBowZoomGuard=false` is the old dead guard. crossbow `VrCrossbowNoZoom=true`, classes 10/14/15 (XR32).
- rifle (class 13): still the H21 park/replay under F11=2 (`F11_RUNG_NOT_2_RIFLE_STAGED`, `rifle_hint_d32.h`). nobody tested a rifle in VR. if you do it, copy the R89 bow pattern, not XR34.



---

## 8. CRAFTING, DICE AND MINIGAMES

this is the part of the mod that fights the game the most. KCD2 minigames are 100% native C++ with authored clips that
move henry's real hands, so every "VR hand" we add at the table has to steal ownership from an animation that wants
the same wrist. everything below is in the order it happened. where i say "i tested" it is a headset run by me,
where i say "static" it is a read of WHGame.dll (ts 0x6A350E20) and the source, no headset.

the four modes you can pick per station (menu ALCHEMY / SMITHING, INI `VrAlchemyMode` / `VrSmithingMode`, enum
`minigame_hands45::Mode` in src/vrik/minigame_hands45.h): 0 NATIVE (authored hands + A), 1 SEMI_PASSENGER (tracked
hands between native actions, native owns the clip), 2 GESTURE / FREE VRIK (tracked tool gestures), 3 6DOF_VRIK (head,
hands AND body follow, station root moves with the HMD). -1 inherits the old shared `VrMinigameHands`. smithing default
is 3, alchemy inherits (my INI has 3 for both). `VrWorkstationHands=false` kills all of it (`reason()` returns
WORKSTATION_INI_DISABLED). the witness that a minigame is live comes from the start/stop hooks 0x9F0970/0x9F1150/0x9F0E60
in src/activity_input163.cpp (`Kind175`: Alchemy, Smithing, Reading, Sharpening, Unknown, ForgeBuilder, Dice), 80 ms fresh.

---

### 1. ALCHEMY

#### 1.1 what i wanted
grab the herbs/bowl/mortar/jug with the hand (Grip), pour by tipping the jug over the kettle, grind by moving the
pestle, drop the plate/mortar contents into the kettle by turning it over, pump the bellows, no A button, no waiting
one second for henry's animation to catch up. recipes, quality, inventory stay the game's. and the book: walk to the
book at NORMAL speed, only the pickup/put-in gets faster.

#### 1.2 the native facts everything is built on (static, C35 notes 17 Oct)

- ONE verb for the whole table: `alch_use` (A, hold 0.25) -> AlchemyItem.OnUse -> Alchemy.OnUse(user). which of the 18
  hints it means (pour water/wine/oil/spiritus, clock, pot down/up, lay, crush, pour out, distill, take, insert, return..)
  the game decides from the interactor's selected entity (+0xF4) and the station state. extra binds: `alch_use_bag` LT
  (bellows), `alch_use_sandglass` RT, `alch_back` B, `alch_reset` X hold, move pot dpad-up.
- table props = AlchemyItem with alchemy_type = slot index: 0 dish/bowl, 1 hourglass, 2 mortar, 3 pestle, 4 distillator,
  5 empty glass/flask, 6 kettle/pot, 7..9 herbs, 10..12 special ingredients, 13 alcohol, 14 oil, 15 wine, 16 water,
  18 bellows. these slot numbers are all over the logs (`selected=952224/6` = the kettle).
- classes: C_Alchemy vft 0x3F60B08 (+0x2C running byte, +0x78 embedded C_AlchemyActionDirector), C_UseResourceAction
  vft 0x3A5C670 (the carafes), C_UseMortarAction (0x6C3C50), C_UsePotAction 0x3F605C0, C_UseRetortAction 0x3F60410,
  C_FinishRecipeFromMortarAction 0x3A5C900. the brew ends when the final action's anim-finished callback calls
  C_Alchemy::FinishStation(idx) = 0x2E16680 (exactly 3 callers), which runs BuildResult 0x2E2BD10 over the recipe DB.
  this is what `finish_observer_cb1` reads (recipe verdict only, never items).
- hand tools are `entity_right`/`entity_left` attachments on henry (attach 0x1085234, detach 0x10853CC), membership at
  owner+0x2A0/+0x2A4 -> type -> owner+0x150[type] entity (src/vrik/alchemy_tool18.h). carafe = LEFT hand type 13..16,
  mortar L type 2 + pestle R type 3. a HERB in the hand is only logical membership (0x8D42D8), not an attachment.
- the clips (male_alchemy.adb): pick_vial_1..4 is ONE clip take-pour-return (6.3 s), use_mortar loop, herb_to_pot,
  mortar_to_bowl (fires `WH_Particels.other.herbs_pouring_fast` at 0.323), bowl_to_pot, pot_move_up/down, use_bellow,
  distillation_pot. the herb->pot/mortar clips have NO particle row in male.animevents, only audio
  (f_he_herbs_put_water 0.4317, f_he_herbs_to_mortar 0.452).
- recipe steps are CONDITIONS (IngredientCount, IsMilled, WeakBoilingTime..), MortarUseCount 1, BellowsUseCountToDistill 2,
  HourGlassUnitTime 10 s. so we never have to "do the recipe", we only have to get the native action to run.
- the mid-clip hand positions per clip were decoded from the dba
  (e.g. alchemy_use_mortar 195 frames mid 97, alchemy_mortar_to_pot 177 frames) - used for the "blend native clip middle
  to our hand" idea, never shipped as such.

#### 1.3 history
1. **V65.19 (Sep)** - alchemy was just a button map: Back=B, Reset=X hold, Bellows=LT, Hourglass=RT, pot = dpad.
   nothing VR.
2. **V66.174R1/R2 (early Oct) - alchemy camera diagnostic.** the recipe BOOK zoomed/moved the camera when opened. V66.174R1
   built a read-only capture at PublishViewCamera (4 samples/s, F5 lease). R1 never ran (its flags came from a retired
   CActionMap observer that caused loading stalls - DEAD END, "must not be reinstalled"). R2 used the live F5 lease. it
   only observed, never suppressed. the book camera got handled later by the activity lens (`activity_lens59c10r1`,
   kindMask Alchemy=1 Reading=4 Sharpening=8 ForgeBuilder=16 Dice=32, writes the gameplay FOV instead of the minigame
   camera's 0.6 rad).
3. **V66.178R10..R13 (8-12 Oct) - "workstation hands" F5 rung 2.** first VR input at the table: Grip >= 0.58 holds A
   (`alch_use`), right-hand down-swing > 1.6 m/s then stop = RT 140 ms (`blacksmith_stroke`, 320 ms refractory,
   `HammerDetector`). the log search showed: "no alchemy/smithing VR-hand work ever existed (all six older logs)". hand visuals
   still native.
4. **V66.178R59.16C2 (14 Oct)** - the gestures moved out of F5 into normal play (`VrWorkstationGestures`, now legacy;
   `gesturesOn` in workstation178r20c1.h:136 ORs the real owners before it). plus the **pointing hand**: a hand >= 25 cm in
   front of the head aims KCD2's own interactor at the table tools (`VrAlchemyHandRay`, `pointingHand59c2`). i tested:
   the native prompt follows the hand, grip uses it. this is still the base of gaze-vs-hand selection.
5. **C10R19C4 (13-15 Oct) - alchemy_stage19c4.inl, the carafe pause.** the problem: the native carafe child (pick_vial_N) gets
   its speed bias -> 0 after the native Attach while Grip is held, restored on release so the native pour + return
   continue. i tested C31: "sometimes holds carafe, pour cannot finish, release floats". C32 scope fix not accepted.
   the file is still in the tree (src/alchemy_stage19c4.inl) but the owner today is C46/D85 (below).
6. **C35/C37I1 (17 Oct) - the selection bug.** claude found the Grip was reading the selected entity DURING the game's
   IsUsableByHold enumeration (hook 0x8D1ECC), which alternates 0 / entity inside one frame -> a Grip edge could grab an
   empty selection and keep it for the whole squeeze. fix: copy the FINAL selection after the completed gameplay frame
   (`interaction_native_selection68.inl`, `native_selection68.h`). second edge found in review: the palm consumer
   compared the fresh selection against the loop START time -> looked like a future sample -> Grip eaten. clock now
   sampled at the read. also killed the 83,979-row F5_CRAFT_ATTACH spam (holsters/bows through one cache slot).

7. **C46 (18 Oct) - semi-native alchemy (src/alchemy_semi_native46.inl, alchemy_resource46.inl, core/alchemy_semi_pins46.h).**
   the real owner design: hook the Henry CActionController, find the native working child (grinding, bellows, carafe,
   book) via `SemiAlchemyParent46`, and write the IAction +5C speed BEFORE native Update. speed 0 = paused (hand not
   moving), >0 = the hand "earns" authored seconds (`alchemy_work_clock_d47::Ledger`). the carafe path observes the
   exact native event scan (0x842248 -> 0x84351C -> 0x6C29D8) so Attach/Use/Detach fire once, never replayed. this is
   the pattern every later speed/pause trick reuses ("same pre-Update +5C lease").
8. **C49 (18 Oct)** - `fullAlchemy49`: F5=2 = FULL VRIK alchemy, F5 OFF = saved preference. semi selects with the HMD
   ray, full uses the palm ray. the semi input problem too: workstation frames copied allowed=false from
   the non-gameplay minigame tracking and starved the motion clock.
9. **G1/G2/G3 (19 Oct) - input67 / gesture_work67 / native_mortar69.** "crafting" was frozen as one block: pure adapter
   `src/subsystems/alchemy/input67.h` (selection + hold hints + attachments in, Use request out, "never a claim of
   native completion"), `gesture_work67.h` (Kind Bellows/Mortar/Resource/Book, Status TiltToPour/Pouring/Grinding/
   Pumping/ReleaseAwaitingNativeReturn..), G3 mortar: tilt over the pot with the real pair -> native PlaceMortar
   child+70 with originalUse/Detach 6C4A34, no duplicate Use. "PHYSICAL HAMMER MAPPING NOT IMPLEMENTED" in the same note.
10. **C55 (20 Oct) - guided crafting rung 3.** the "constrained"
    experiment: Touch+Grip admission needs native selected entity + hold hint + palm <= 8 cm from the use point; tool
    AND wrist stay joined (no dragging the carafe away from the native emitter); work only inside the 12 cm grip
    corridor; carafe = LEFT hand, mortar = RIGHT pestle. it also fixed 8 "connected gates" (SetInteractionExperiment36
    admitting only rung 2, forced alchemyPassenger42 killing alchemyPointer59c2, body bootstrap hard-requiring Semi..).
    13 host tests, zero headset proof. rung 3 still exists as a separate experiment, nobody uses it.
11. **A72 (23 Oct)** - idle hover of one hand no longer beats the other hand's deliberate Grip; a carried tool nominates
    with its carrying palm; a rejected mortar Stop can only be rearmed when the native owner proves it never entered the
    Stop call.
12. **H50/H51 (24-25 Oct) - "one grip away".** a log search showed: STEP15 build had valid hands, a 4 cm nomination,
    known native selection, fresh action, and `grip=(0.000,0.000)` in EVERY session - the Grip never reached the craft
    consumer. STEP30 regressed to SEMI and lost the hand publication. and it turned out my INI had been wiped that day
    (VrAlchemyMode 2 -> 1, so the run was SEMI not FREE). H51 = alchemy own-head trial (`VrAlchemyOwnHeadH51`, OFF, legacy).

13. **D43 (28 Oct) - FIRST SUCCESS.** i tested: mortar "turn over over the bowl" worked for the first time. water still
    unreachable, pestle below the hand. DS20 widened the mortar grind radius .12 -> .20 m (gesture_work67.h ~207) because
    "the gesture was not making it to grind".
14. **D52/D55 (29 Oct)** - "alchemy is better", release sound/particles ~1 s after the hand opens, mortar emptying NOT
    working, bellows not. D55: book better, bellows worked, grinding needed too much movement, retort should stay on its
    pivot. D53: mortar destination selection fixed (mortar_destination_d53_test). D55: `VrAlchemyToolContactD55` (world
    contact for tools in 6DOF).
15. **D59 (30 Oct)** - `VrAlchemyTransferD59` release transfer: keep a valid drop until the game can accept it.
16. **D63R1 (31 Oct) - the "nearly" build.** i tested: "alchemy: better", pickup and drop ~1 s late, distillation
    restores henry's long native arms. independent review measured it: A output 0-1 ms after the hand, native Use
    93-123 ms later, hand/tool joined 600-1299 ms later -> the delay is INSIDE the native attach lifecycle, not our
    wait.
17. **D65 (31 Oct) - transfer speed slider** (`VrAlchemyTransferSpeedD65`, src/alchemy_transfer_clip_d65.inl): speed up
    the CAF of the 8 transfer stages (take/return/place). DEAD END as written: it wrote the CAF speed AFTER native
    Update and restored before the next one, so the native action clock stayed 1x (found in D81). D66 review also
    found the stale-tool veto: `semi_workstations46.h:267` copied 91-second-old `joined=1` records into the input, so
    a never-acquired Grip could not be released, and the mortar pour request that timed
    out at 5032 ms because native_mortar69 waits for READY (#3).
18. **D72 (31 Oct) - INSTANT alchemy (src/subsystems/alchemy/instant_native_d72.*, `VrInstantCraftingD72`, menu
    "INSTANT crafting", ON).** the plan: let the native director admit the real parent action, DON'T build its authored
    child clip, call the class's typed semantic helper (0x6C5618 etc: empties the logical hand, moves contents
    0x6C4D5C, attaches resource geometry) once at the next Henry controller callback, then complete the parent.
    pins in instant_native_pins_d72.h. the review warned: completion inside
    Start hits an empty director slot (0x9CCD71 ordering) -> complete only after install. the pro import into D73
    dropped the D72 `pourRecipientD72` (5 cm lateral + vertical drop) and instantPour ack.
19. **D73 (1 Nov) - REGRESSION.** i tested: cannot take/release by hands, A needed. review: 32 F5 attempts, none
    completed; the palm nominated slot 7 with gap 0 while native selected slot 8; D70's unconditional palm-ray
    corridor filtered the destinations (a tilted vessel points away from the bowl!). DEAD END: direction corridor
    for destinations.
20. **D74/D75 (1 Nov)** - restored the recipient/completion paths, head-hover NAMES while the hand targets something
    else (`interaction_alchemy_names_d75.inl`), `PourMortarD75` (held mortar tipped over the pot = instant contents),
    D75 action matrix still excludes initial mortar pickup, liquids, retort, bellows, pot from instant.
21. **D79/D80 (1 Nov)** - six PLACE_INGREDIENT instant receipts completed natively (semanticEntered=1, parentRetired=1),
    but SILENT: no child = no authored sound. D80 `feedback_d80.h` + `sound_service_d80.inl`: play the exact animevent
    audio (f_he_herbs_to_mortar..) through the player audio proxy after a verified instant receipt. mortar emptying +
    particles WORKED in that run (native PlaceMortar path, left alone). D80 station prompts 45%.
22. **D81 (1 Nov) - presentation_speed_d81.*:** fixed D65 properly: write the pinned IAction +5C clock before Update
    (same lease as C46), 8x ceiling, 80 ms authored-time cap per update. but it classified Book and Resource too.
23. **D85 (2 Nov) - carafe_pour_d85.h.** evidence from D79: the UseResource child ran its full 6.3 s and no clock owner
    took it (Attach at 0.165, Use 0.4209, Detach 0.8664 normalized). now: reach fast until Attach, then 0 until the
    left hand holds + tilts >= .40 rad within 20 cm of the pot, 1x through Use/FX, 0 until the hand brings it home
    (10 cm, upright <= .35), 1x to Detach. events never sent/skipped. `resource_turn_d86.h` = the fast turn only with
    positive gesture credit and before the first post-Attach event. (`Bits` Valid/Grip/PotKnown/Pour/HomeKnown/Home)
24. **R88 (2 Nov, crafting)** - i tested D85R5: "changing stations (to the book and back) - the view snaps and
    turns". log: `use_alchemy_book` 0.36 s at 6.65-7.26x vs 2.5 s native in D66. fix: Book kind and MovePot
    (vft base+0x3A5C150) no longer sped; TakeIngredient/PlaceIngredient/Return*/TakeObject/PlaceBowl/PlaceMortar/carafe
    reach still are. and "plate turn without particles": instant PlaceBowl / PlaceIngredient->pot omitted the clip =
    no `herbs_throw`. fix: decline instant for dest 6 (pot) so the native clip plays, sped as a put-in.
    prompt icons 0.45 -> 0.9 (`VrMinigamePromptScale`, "CRAFT HINTS" 30/45/60/90/100).
25. **R94R1** - alchemy clock-order bug: validation sampled time BEFORE reading newer tool snapshots -> false
    rejection. snapshots first now.
26. **R96/R97 (4 Nov) - left hand pour.** i tested: mortar emptying works once, fails once. log: the failed one was
    taken with the RIGHT hand; the game still attaches it LEFT (toolL=952205/2), and tilt-pour was only evaluated with
    the LEFT Grip squeezed -> never. fix input67.h `carriedLeftR96`: the left hand counts as carrying, pot/dish nearest
    the LEFT palm is the destination, refused attempts rearm when upright < .35 rad. merged (R97) with the "unspent Stop
    request kept through a gap" (native_mortar69.inl).
27. **R103 (5 Nov) - mortar admission + "release as drop in".** i tested R101: mortar misbehaving. 4a: second mortar
    admitted as the BOWL (first nominated object kept after the game attached the mortar pair) -> input67.h:213-239
    re-admits the mortar on the left when the exact pair (L slot 2 + R slot 3, same table) is joined. 4b: pour armed on
    the bowl at pickup, then 9.6 s stuck in RELEASE_AWAITING_NATIVE_RETURN because only the SAME destination counted
    -> input67.h:826-839: once the grind is ready, whatever is under the left palm wins: tilt >= .45 pours there, opening
    the hand over kettle/bowl places there (`mortarReleaseIntent70`), then the native Use pulse (`mortarReturning69`).
    A stays the fallback.

#### 1.4 what works now (R106)
owner chain: `activity_input163.cpp` witness -> `workstation178r20c1.h` (`workstationOwnerH51`, gesturesOn) ->
`semi_workstations46::State` (src/vrik) copies the frame + native tools (`bridge67::tools` mailbox, `NativeTool`) ->
`subsystems/alchemy/input67.h` decides (pure) -> host publishes `Output` (A pulse / hold) -> native action runs ->
`action_time_owner59c11.cpp` includes the clock owners: alchemy_semi_native46.inl (C46 work clock),
alchemy_transfer_clip_d65.inl, presentation_speed_d81.inl (+carafe D85), instant_native_d72.inl, native_mortar69.inl,
native_return67.inl, sound_service_d80.inl.
- selection: hand within `near67::Enter=.10 / Leave=.14` m of the native AABB (`geometry68::Bounds`, retail local
  bounds x world Matrix34) nominates; `reach_policy_craft1.h` = CRAFT1 trial (OFF) alternative ranges; mode 3 owns an
  EMPTY selection with open hands (near_selection67.h:260) so the gaze cannot take the book remotely; head hover still
  shows names (D75).
- Grip on a nominated item = native contextual A with the native hold/release hint; herbs = logical hand (no attachment).
- instant (D72, ON): TakeObject/ReturnObject (bowl, hourglass, distillator, flask L), Take/ReturnIngredient (7..12),
  PlaceIngredient into bowl/mortar, PourMortarD75. NOT instant: anything into the pot (6), PlaceBowl (R88, keeps clip+
  particles), initial mortar pickup, liquids, retort, bellows, pot move.
- speed: `VrAlchemyTransferSpeedD65` (slider, Experiments ALCHEMY "Alchemy transfer speed", default 4, my INI 8) = pickup/
  return/put-in only via presentation_d81 `transferKind` (vfts 0x3A5C308 0x3A5C4C0 0x3B87A38 0x3A5CCD0 0x3C23010).
- carafe: D85 controller (above). mortar: grind = C46 work clock from pestle motion (`mortar_contact_repair.h` tests
  the pestle bounds against the mortar instead of 12 cm wrist distance), pour/place = input67 + mortar69/pour70.
- bellows: gesture Pumping in gesture_work67 -> native `alch_use_bag`; worked D55, "not universally".
- sound for instant drops: feedback_d80 cues. particles only where the native clip runs.
- recipe verdict: `alchemy_finish_cb1` hook on OnAlchemyRecipeFinish (`VrAlchemyFinishObserver`), log only.
- F5 receipts: `capture_d42.h` (D42_F5 rows: admitted=/selected=/mortarRequest=/status), `ALCHEMY_INSTANT_OUTCOME_D79`,
  `D84 F5_ALCHEMY_PRESENTATION appliedRate`, `R96`/`R103` rows.

#### 1.5 open
- pickup/release is still a native attach lifecycle away (600-1300 ms in D63R1 measurements) for everything not instant.
- pot move, retort, distillation = native (and the retort pulls henry's long arms back, reported D63R1, not fixed).
- bowl/pot continuous hand-clock ownership "remains unimplemented" (D85R5R1). PlaceBowl is one tilt intent + native clip.
- D85 carafe never headset-proven by me yet as of R106 docs; the plate (R92R3 "plate fails to empty", R103 plate offset
  fix) needs a retest.
- DEAD ENDS, do not repeat: writing CAF speed after Update (D65); speeding Book/MovePot (D81 -> R88); direction corridor
  on destinations (D70/D73); instant for into-pot drops (no particles); waiting for the SAME destination after the grind
  (D66/R103); requiring the left squeeze for a right-hand pickup (R96); keeping stale joined=1 tool records (D66).

#### 1.6 if you want to fix X
- hand does not grab: input67.h admission (~:200-260) + `near67` radii + the D42 rows `admitted=` vs `selected=`.
- something is too fast/slow: presentation_speed_d81.inl `ReadAlchemyPresentationD81` (kind table) and instant_native_d72.inl
  `AlchemyInstantPrepareD72` switch (decline = native clip).
- pour/mortar: input67.h stopping branch (~:800-840), native_mortar69.inl (`TickMortarStop69`, 5000 ms deadline),
  mortar_pour70.h (home capture), carafe_pour_d85.h `intent()`.
- no sound/particles: feedback_d80.h cue table; particles = let the clip run (decline instant).

---

### 2. SMITHING

#### 2.1 what i wanted
left hand holds the iron/tongs, right hand swings the hammer, the hit lands where the hammer is, moving the iron
in/out of the fire with the left hand, bellows with a hand pump, hammer and iron collide with the anvil, no stick aim.
my 27 Oct words: "only the anvil needs to be collided with - iron and hammer" and "the iron and the hammer have no
collisions"; "the chest is obstructing the view".

#### 2.2 native facts (C35)
maps: anvil `blacksmith_stroke` RT>0.4, `blacksmith_gamepad_x/y` left stick aim, to_forge A, swap_workpiece X; forge:
stick moves the piece, to_anvil A, inspection X, use_forgebag RT. phases 0..20 from the listener map refresh 0x858188
(ctx+0x88): anvil = 11 ready, 14 raised/charged, 16 stroke, 17; 15 = post-fall; forge 4/6/8. RT press from phase 11 =
raise/charge, RELEASE in 14 = the strike (so a synthetic trigger CLEAR in phase 14 IS a hit, there is no harmless
cancel - C37I1). hammer/tongs are entity_right/left attachments (same routine pair as alchemy, 0xB66624 object: +8 hand,
+0xC entity, +0x14 attached), workpiece in the tongs = "Assetslot". this is why the first smithing hold (CItem hand
slots) held NOTHING (C31/C32). animevents: AttachTongs/Workpiece/Hammer, FullStrokeCharge, StrokeHit (enum 10,
C51060 point), PerfectStroke, ToggleFlip, ForgeBagPull/Release (parser 0x857548). the anvil itself has NO pin in the
retail tables.

#### 2.3 history
1. **V66.178R10 (Oct)** - `HammerDetector`: down-swing > 1.6 m/s then stop -> RT 140 ms. found later: the pulse starts
   the native wind-up AFTER the physical swing = "raise+drop late and weak". DEAD END as a strike model.
2. **C37I1 (17 Oct)** - 1,068 auto-hold receipts, all workpiece; hammer on hand_right at 14:30:30 but reader NONE. live
   blocker = tool identity. C37I1 logs the exact refused check instead of NONE.
3. **G3 (19 Oct)** - static check: "PHYSICAL HAMMER MAPPING NOT IMPLEMENTED. quality cursor also controls authored blendspaces.
   no verified hammerface -> native quality UV conversion".
4. **C46 semi smith (`src/vrik/semi_smith46.h`, `semi_smith_clock46.inl`, core/semi_smith_pins46.h)** - the thing that
   works: the right hand supplies WORK to the exact native raise/finish child (builder 0x399AE0, child builder call
   0x399539): lift >= `kLiftStartM=.03` m requests native RT (phase 14), `raiseWork` = metres * `kMotionSecondsPerMetre=4.5`,
   down reversal >= `kDownReleaseM=.018` releases = strike, ack windows `kPressAckMs/kFinishAckMs=450`. left hand fore/aft
   = native axis `kAxisMetresPerSecond=.28` (found 22 Oct: "forward/backward moves the iron, not up/down", my
   correction). forge pulse 70 ms.
5. **24-26 Oct** - the "iron angle" hunt, static search: NO angle constant ever existed; the old fix was the canonical grip67
   carried across phases 11->14->16 (smith_tool36.h:92-96) + the H21 fixture gate (Semi/Gesture only). bad angle in
   6DOF = controller-placed by design.
6. **A2/A3 anvil (27 Oct, src/subsystems/smithing/anvil_a2.h, anvil_physics_a3.h)** - A2: LEARN the anvil from the
   first `kLockBlows` native StrokeHit points (top = lowest blow minus workpiece half thickness, footprint = centroid),
   locked per visit, replaces the rejected "J28-B cylinder". A3 ("try it, if you can find it"): the REAL anvil via
   CPhysicalWorld::GetEntitiesInBox (vtable +0x110 = +84C354, world slot +492D830) at the first blow point, read its
   AABB. menu `AnvilMode`: OFF / REAL BOUNDS / LEARNED (`VrSmithAnvilCollisionA2`, `VrSmithAnvilContact`).
7. **J27 (26 Oct, src/vrik/station_ws_j27.h)** - torso: the 6DOF root moved the pelvis by the FULL HMD delta so leaning
   over the table dragged the chest under the eyes. now hips stay, lean up to .45 m forward (.10 back, .15 side), chest
   >= `VrStationChestClearCm` (12) behind the eye (`VrStationTorsoFollowHead`). tools: hammer/tongs/workpiece hand gets a
   3-probe world sweep (`VrSmithToolCollision`). station root = `roomscale_station_ds.h` (+/-1 m square, deadZone .03).
8. **D51/D53 (29-30 Oct)** - bellows: the authored hand socket was 3.17 m from my palm; the real handle joint is 14 cm
   (`bellows_geometry_d51.h physicalHandleJointD53`). **D56** `native_floor_d56.h`: a presentation floor so the workpiece
   does not sink below the native idle placement. **D57** stroke receipts (diagnostic). **D60** smith deadline patch.
9. **D67** - `VrSmithRecoveryD67` (keep a deliberate lift through recovery), `VrSmithSemiAnchorsD67` = "Smith LT anvil
   assist": hold LT = native left workpiece placement at the anvil, release = tracked hand. my D81 INI had it OFF, D81
   repair turned it ON, D85 default ON (candidate_defaults_d72.h).
10. **D72 instant smith** (`subsystems/smithing/instant_native_d72.h`): only the ForgePump downstroke publishes to the
    native process callback. no instant hammer.
11. **D73 (1 Nov)** - i tested: anvil work "nearly perfect", body position good, axe ok. forge phase wrong: henry
    deposits the piece in the oven and leaves it; axe head not in the tongs at heating; tongs/left hand locked during
    heating; hammer sometimes glues down; learned anvil 3-5 cm too high.
12. **D80/D81** - 43 of 45 native StrokeHit callbacks had a VR receipt, 328-344 ms from VR release to native hit.
    `fall_timing_d81.h`: the acknowledged falling phase runs 4x (80 ms cap). smith contact at the station bounded
    continuously instead of a 20 mm hard threshold.
13. **D82 (2 Nov) - hammering after reheat.** i reported no strokes after reheating. old code needed two samples with
    <= 6 mm movement before a fresh lift could arm; moving hands never got there (6 mm/sample @ 90 Hz = .54 m/s). D82:
    quiet entry OR a fresh 18 mm lowering then a separate 30 mm lift, both in native anvil idle.
14. **D83R1** - phase 15 (observed post-fall, 3 x INACTIVE->REBASED in the D80 log) now continues an existing stroke;
    spatial tracking jump != lost history; held LT follows the native workpiece transform (the 20 cm visual slide
    DEAD END: it moved the iron away from where the native evaluator was).
15. **D84** - ack and release in the same packet lost the fall receipt (`fall_timing_d81.h` accepts phase 14 while
    releasing).
16. **D85R1** - "returnToStartD85R1_": after a VR-started stroke the hand must return to its launch height
    (1 cm) before a new lift can request another native stroke (59 native hits joined, duplicates ruled out).
    D85 craft: forge phase 8 RT exemption. i tested R92R1: "smithing quite OK".
17. **R88** - regression check only, nothing touched. **R96** - the anvil GetEntitiesInBox contract reused for the small
    prop palm box query (static_grab_r89).

#### 2.4 what works now
- `semi_smith46::Controller` (right hand lift/release -> native RT, mode 1/2/3), `inputContext` phases 11/14/15/16/17 + 8,
  `assist_d80::held` (LT, mode 3, phases 11/14/15/16/17, trigger > .55), fall_d81 receipt, anvil A2/A3, J27 torso + tool
  sweep, source_grip_h42 (Grip -> native contextual A before pickup, slots 0x2E0..0x2F8 on the model), smith_contact68
  (READ-ONLY StrokeHit frame), native_sources_h42.inl, roomscale station root.
- menu SMITH: Tracked smithing hands, Smithing work mode, Smith world tool collision, Smith tools follow hands
  (`VrSmithingAutoHold`), Smith stroke recovery, Smith LT anvil assist, anvil mode. logs: `SEMI_SMITH_INPUT`, `F5_HAMMER_STROKE`.
#### 2.5 open / dead ends
- no hammer-head -> quality UV mapping: the hit location is still the native 2D map (model+198), not the hammer. the
  anvil collision stops the hand, it does not create a hit.
- forge handover (deposit and return) and the heating tongs still wrong (D73 report), never closed.
- bellows right-hand downstroke requests exist (D73: 14 acquisitions / 9 requests) but "a request is not a confirmed
  native pump".
- DEAD ENDS: RT pulse after the swing (R10); CItem hand slot reader for hammer/tongs; J28-B cylinder anvil; 20 cm
  visual slide under LT (D83); 6 mm stillness arming (D82); clearing RT in phase 14 as a "cancel" (it strikes).
#### 2.6 look here
stroke not registering: semi_smith46.h ~:300-470 + fall_timing_d81.h + the `SEMI_SMITH_INPUT phase=` rows. iron
position: assist_d80.h + the LT placement in semi_workstations46. anvil height: anvil_a2.h kLockBlows / anvil_physics_a3.h.

---

### 3. WORKSTATION BODY, GUIDED RUNG, STATION REPOSITION
- `workstation178r20c1.h` (V66.178R20 era): rung bookkeeping (`experimentArmed36`, requested vs admitted), `fullVrAlchemy63`
  (gaze/head selection owner 63: active non-native mode + witness), `workstationOwnerH51`. F5 cycles OFF -> 1 record ->
  2 record+gestures+VRIK arms -> OFF; rung 3 = C55 guided (separate, unproven).
- `station_body_h41.h` (`VrStation6DofFallback`, default true): a refused 6DOF visit falls back to FREE body. moves only the
  qualified pelvis/legs, never CPlayer or its capsule.
- station reposition: `roomscale_station_ds.h` State applied by one root owner, J27 lean rules on top (6DOF only).
- crafting menu: `crafting_menu_h18` (G1 independent saved modes per station), `crafting/options_d67.h`.
- 6DOF design spec (25 Oct FULL_6DOF_CRAFT_DESIGN): Grip = nomination + contextual A, release = A release, root
  follows HMD in 2x2 m, native plays the clips, particles native. that IS what mode 3 became.

---

### 4. DICE

#### 4.1 what i wanted
sit at the table, see the dice at the right depth, pick the dice with the hand, no double image, no zoom left over
when i stand up. shaking/throwing the cup by hand would be nice - not done.

#### 4.2 native facts (DS_DICE_MINIGAME_20261101, hand_select_d85.h)
DiceInteractor.lua -> "dice:init" to the table AI (so_diceTable_new.xml) -> Minigame.StartDice native. C_Dice vft+178 =
2E8E45C `cmp [rcx+498h],5` = human input allowed; game model C_Dice+4A0, states PlayerTurn 3F711E0 / Holding 473F1C8
are the only ones that accept input; die list at C_Dice+(player+58h+4Bh)*16; native cursor index at +4F0, setter
2E951BC(C_Dice,index,false) (same call the stick handler 2E9007C makes); `dice_hold` toggles keeping a die via 2E8FA84,
it is NOT the cup. the throw is C_ThrowDiceAction, clips minigame_dice_shake 122 frames, dice_throw_success 217 - nobody
proved which clock fires the throw event, so we don't touch the cup. the minigame camera is the shared Minigame camera
(manager+0x338, vft 408EB60, 34.4 deg = 0.6 rad) vs FirstPerson manager+0x1E8 (95 deg).

#### 4.3 history
1. **D80** - dice lens: `activity_lens59c10r1` writes the gameplay FOV while the lens activity runs. D81: dice was in the
   lens policy but missing from the neutral-eye mask (bit 32) -> henry's native inspection lean moved the view forward.
   the D81 dice seated patch: model-local head anchor learned from the exact `minigame_dice_idle` clip, tracked
   spine/shoulders/arms/head, pelvis+legs native, "does NOT implement gesture throwing".
2. **D84** - F5 snapshot of the native action map + cursor (`subsystems/dice/diagnostic_d84.inl`).
3. **D85 - point to choose** (`hand_select_d85.h` + `hand_select_native_d85.inl`, `VrDiceHandsD85`, Experiments OBJECTS
   page 2 "Dice: point to choose (trial)", OFF): on my choosing turn the right hand ray picks the die, the game's own
   cursor setter jumps to it, A keeps/unkeeps, X/Y/B/stick native. row shows why it fell back (no hand ray, game refused).
   D85R1: DiceCursor removed from the HUD baseline caching/scaling - it has moving native coords from
   ShowDiceCursor in 1920x1080 space and treating it as a fixed centre element froze/moved the selector.
4. **D85R5 / R89 (3 Nov) - the frozen dice picture.** i tested: after the dice game i moved VR zoom to 0.95, the headset
   showed a STILL dice image, restart needed. the log showed it: DLSS stereo died at 23:50:36.8 (right-reset fault)
   mid-game, every menu edit did `SetStereoExperiment(2)` (host.inl:359, old since V66.24) with no producer, the last
   published pair stayed = the dice frame. fix: mode 2 only with a live run, Run resets a stale mode 2, published pair
   unpublished on stop (`R89 STEREO_IDLE_RESET`). AND "zoom stays after dice": the lens stopped writing at the witness
   edge but the game keeps the Minigame camera a while -> `ExitHoldR89`/`exitHoldStep`: keep writing the gameplay FOV
   while the view is still the Minigame camera, disarm on the first other camera.
5. **R90** - the exit hold was armed on ANY lens activity, not only dice -> R91 `DiceExitFrameR91` (dice
   instance/player/lifetime), R94R1/R95 (`policyKnown/policyFresh/protectedContext`): a missing policy sample suspends
   writes but keeps the visit identity; dialogue/other camera revokes.
6. **R92 (3 Nov) - dice marker** (`selection_marker_r92.h`, flashui_probe `ServiceDiceMarkerR92`): our own XR ring at the
   selected die, hid the WHOLE native DiceCursor while "prepared". i tested R92R1: "dice HUD at wrong depth". **R92R3**
   `hud_depth_r92r3.h computeSurface` - HUD plane 0.45-1.55 m at the selected die's XR pose instead of the fixed
   1.55 m ("depth-matched UI, not a cure for near world-dice double vision").
7. **R98 (4 Nov)** - i tested: "the round zoom-in circle is double". a static search of hud.gfx showed: `cccursor.dice` holds the
   ring (`cursor.gotoAndStop(2)`) AND the label (flag: icon+number+name). R92 hid both and the XR ring was a second
   circle. fix: companion HUD.xml:718 names the ring child `DiceCursorRing`; only that is set invisible while
   `witness175.dice != 0`, label stays native, R92 XR ring never submitted (`flatOwnerReady` false). log `R98 DICE_RING
   hidden=1` (hidden=0 = the companion pak with the new HUD.xml is not installed).
8. **R104** - dice marker moved to SteamVR overlays with HUD/lasers/cursor/menu. not headset tested.
#### 4.4 what works / open
works: seated VRIK upper body (D81), lens FOV + exit hold, hand pointing (trial OFF), native label at die depth, no
double ring (if the pak is current). open: physical cup (take/shake/place) = nothing, KNOWN_ISSUES says so; near-table
stereo double vision of the real dice is a render thing, not HUD. look here: activity_lens59c10r1.h/.cpp (exit),
hand_select_d85.h (`Turn`, `statusText`), flashui_probe.cpp ServiceDiceMarkerR92, hud_depth_r92r3.h.

---

### 5. LOCKPICKING, PICKPOCKETING, RESULT POPUPS (modal fit)
- lockpicking input is native (stick + the lock). the only mod work is the overlay FIT: `ui_modal_overlay43::Fit`
  (src/ui_modal_overlay43.h, C43) is shared by ApseModalDialog (the "you made X" popup after smithing/alchemy, opens
  exactly at the minigame STOP, UI-STATE Modal bit 6), LockPicking + Pickpocketing (S1, `activity_overlays_s1.inl`,
  parent `mc_LockPick` at 960.2,540) and the SkipTime wait clock (B2). their parents sit at the stage centre in square
  movie units.
- the static search LOCKPICK_DOOR_LAYERS (22 Oct) only mapped which door/lock layers exist - no gesture lockpicking was ever built.
- **R98 (4 Nov)** - i tested: the popup and the lock are wide ovals. old Fit: xs from the 16:9 safe box on the 2064x2208
  HUD texture, ys = sy*height/1080 -> 1.9x wider than tall for stage-unit movies. now ONE physical scale, ys = xs *
  (native.ys/native.xs), y from the safe centre; live-pixel movies get exactly the old numbers (recovery43_test), the
  stage-unit ones become round. no lockpicking session was in that log, so "round lock" is math-proven, not headset-proven.
- look here: ui_modal_overlay43.h:13-31, activity_overlays_s1.inl `ApplyActivityOverlayS1`, KNOWN_ISSUES "Stealth overlays".

---

### 6. READING / BOOKS
- two native books: ordinary Reading and the alchemy recipe book (OpenBook). native binds read_prev=LB, read_next=RB,
  read_back=B (never a page). `core/book_pages45.h` (C45) only produces 80 ms LB/RB pulses: `VrReadingPages` 0 thumbrest+
  flick, 1 thumbrest only, 2 native (C42 stick + grips). `reading_pages42.h` keeps a held Grip from masking the native
  page edge. `book_gesture46.h` (C46): the flick in a stable task frame from `book_surface46::Surface` (the copied native
  book plane, "a bounding box without a proven page face must leave valid=false").
- camera: V66.174 diagnosed the book zoom; today the activity lens (Reading=4) holds the gameplay FOV, and
  `BOOK_FOCUS_ZOOM` (focus_zoom58c2) exists but "never fired" in the R89 log. C32 (Oct) was "reading, native map and
  runtime recovery". presentation: native-bound tools and the alchemy book keep the VRIK wrist through native A/page
  transitions (B3/A72).
- R88: the walk to the book is back to native speed (see alchemy #24). my D55 report "book better".
- look here: book_pages45.h `Mode`, book_gesture46.h `Flick::local`, mod.cpp VrReadingPages (3691).

---

### 7. THE HELD MAP (M1 parchment / native paper M43)
- it is a folded PARCHMENT, not a book: retail Document Type 4, EntityScript="Book", model
  characters/assets/parchment_folded/parchment_folded.cdf. our item `kcd2vr_map` 5f3c2a71-..., two image pages
  (trosky / kuttenberg). chord: right thumbrest + left stick UP held 1 s (`VrHeldMap`, touch_bindings.cpp ~298). native
  Inventory FocusItem -> LeftDoubleClick(Read) once, up to 3 polls 250 ms inside the 3 s request; native Reading minigame
  owns the animation. "a request receipt is NOT proof the game accepted Read".
- the first helper was "metallic/glitchy" (my report) -> the native paper material route. DS7 (27 Oct) re-enabled the
  three retired sites in held_map_m1.h ("the original reason for the retirement was not documented").
- static art without minimap.dll: `static_paper_m41.h` objects/kcd2vr/staticmap40/ (26 sheets, `map_geometry_d64.h`
  width/height/ink per sheet), `paper_grip_d64.h` (right edge grip + optional left support, `VrMapTwoHandD64`,
  `VrMapPaperSizeD64` .40-1.5), `map_fold_m39.h` (grip .55 within 12 cm of the edge, hands 12-55 cm apart = fold/close),
  `map_hand_owner_m34.h` (who owns the hand, presentation only).
- D73 map backend request API (`VrMapNativePaperM43`, "Map performance trial", OFF): preference only until the CGF
  renderer (`native_paper_m43.h`: attachments kcd2vr_paper_m43 / kcd2vr_poi_m43 / kcd2vr_player_m43, retail
  CreateAttachment 20ED500 lowercases names) was connected. D75: quest markers from the native compass (C4D2E8 marker+68/
  +6C/+74, `quest_packet_d75.h` "Q75|..." max 16 + 1 waypoint, 2 Hz), the LoadObject-poison bug fixed (old icon shown).
  D80: stable slots, skip unchanged writes. D81: dark POI diamonds instead of the icon atlas on all 26 sheets, paper
  lookup once per activation (no per-field VirtualQuery), off-sheet player marker by slot visibility instead of moving
  it 1000 m away (expanded the helper bounds). D80 log: 90 FPS closed, 26-29 FPS with the map open; D81 unmeasured.
  D84: map profiling only with F2/Shift+F2.
- runtime: src/subsystems/vrmap/runtime/held_map_m1.cpp (`FindRawD72`, `KCD2VR_StaticPaper_M41` entities per region),
  native_paper_owner_m44.inl, quest_service_d75.inl, held_map_feed_t2.cpp. menu VIEW: Static paper map, Paper map size,
  Map performance trial, Map second-hand support.
- open: FPS with the map open, the native paper trial never headset-accepted, objective letters not drawn.

---

### 8. SHARPENING
no mod mechanics. it is a lens activity (Sharpening=8 in `activity_lens59c10r1`) so the view keeps the gameplay FOV, and
the HUD depth override of R92R3 explicitly leaves it alone. i tested R92R1: "sharpening OK". `r_Sharpening` in mod.cpp
is the image sharpening cvar, nothing to do with the grindstone.

---

### 9. the one rule
let the native director admit the REAL action, then own its clock (+5C before Update) or skip only its child. every
try that seeked, replayed events or faked a Use broke something. builds i called good: D43 (mortar tip), D63R1
(alchemy nearly, 1 s late), D73 (anvil nearly perfect), D80 (mortar + particles), R92R1 (smithing quite OK). everything
after R98 here is source-only and waits for my headset.


---

## 9. UI, HUD, MENUS, INPUT

this section is everything you look at that is not the world (the game HUD, the game menus, my own VR menu) and everything you press (touch bindings, openxr profiles, chords). it is the area with the most dead ends in the whole project, because the KCD2 UI is a Scaleform (Flash/GFx) movie drawn on the backbuffer AFTER the 3D picture, and for weeks we tried to bend that movie into a headset instead of just taking it off the backbuffer. read the DEAD END notes before you touch flashui_probe.cpp.

one thing first, the rules i set after too many rounds (they are enforced in code, dont break them):
- ONE log file: KCD2VR.log. no DetourModKit_Log, no -diagnostics.log, no perf sidecars, no finger cache files, no .bak next to the ini. (diagnostics/unified_log_d81.h, core/config_file_guard_d62.h comment "R96: the last-good checkpoint stays IN MEMORY only")
- NO on-screen text from the mod unless F2 is recording. a refusal goes to the log as one line ("VR status: ..."). (R90, DIAGNOSTICS.md)
- every INI key = a switch in the Experiments tab (or the release page), and a switch is never refused: ON means the thing runs.
- no permanent latch that kills a feature until restart. retry on the next attempt / next save load. (R98 dice ring "failure retries next frame (no latch)", R104/R106 layer quarantine "never off for the session")
- features i tested and called working are ON by default and stay on. no silent fallback that switches them off.

#### 1. the native HUD: from "patch hud.gfx" to "its own OpenXR layer"

what i wanted: the normal KCD2 HUD (compass, health/stamina, prompts, subtitles, the quick weapon/food selector, the fades) readable in the headset, not smeared over the whole FOV, and NOT cut in half by the square eye image.

HISTORY, in order:

1. V46.x-V58 (end Aug / early Sep 2026). the HUD was simply part of the mono backbuffer that i copied into both eyes. with a 16:9 source it was fine but useless (tiny), with the square/portrait resolutions we need for VR (1872x2016 = 0.75x Quest eye, 2496x2688 = 1.0x) the Flash stage got stretched and the prompts went off the edge.

2. V60.1-V61.12 "UI mod findings": i compared the letterbox remover pak and Sleek Modular HUD. a static search of the two showed the letterbox is done INSIDE Libs/UI/hud.gfx (fc_setRatioPreset, RATIO_PRESET_GAME/APSE/CUTSCENE/DIALOG, ratioStrips, SCREEN_WIDTH/HEIGHT, resolutionScale) - the no-letterbox variant of Sleek HUD differs in 12 bytes of one DoAction block.
   DEAD END A - "UI safe area pak": V61.10 generated a HUD.xml pak with fixed safe-area rectangles for 1248x1344 / 1872x2016 / 2496x2688 at 90% and 85% (the HUD Scaleform constraint rectangle). headset: it MOVED and CLIPPED the movie, it did not scale the content. retired in V61.12.
   DEAD END B - patching fc_setRatioPreset with an early return by byte offset: "too broad; start-menu text vanished and bars were not correctly removed". also offsets are per hud.gfx version. never patch gfx by offset again.
   what survived from that time: src/subsystems/hud_ui_controls/runtime/hud_xml_mod.cpp still has kCanvasW=1872/kCanvasH=2016 and writes a "KCD2VR_HUD_Runtime" HUD.xml pak from a button on the dev HUD page (write_hud_xml_mod / reset_hud_xml_mod in mod.cpp ~4930). legacy, dont use it for layout.

3. V64.x flashui_probe (Sep). instead of patching the movie, i hooked the game's own Flash bridge: flashui_probe.cpp resolves the HUD element and its MovieClips by name (FindMovieClip, ApplyMovieClipDisplay, CallFlashDisplayFnSEH) and writes _x/_y/_xscale/_yscale on named clips, exactly like the game does (SupportedBuild checks the WHGame timestamp 0x6A350E20 first). this is the mechanism everything later uses.
   V64.20 prompt device lock: HUD.xml exposes SetInputId (1 keyboard, 2 xbox, 4 ps). prompt_device_lock.cpp forces 2 (g_forced_device{2}) via kInputDeviceManagerSetRva 0xC3C034 so the glyphs are xbox even though we inject keyboard/mouse.
   V64.21 constraint findings: in the retail UIElements xmls the ItemSelection/ItemTransfer (loot, chest) panels are Align=fullscreen layer 20, while all the Apse* siblings (inventory, map, codex, books) are fixdyntexsize centered. that explained why loot panels were stretched when the inventory was fine. it was a hypothesis, confirmed later by the C43/R98 modal fit work (see 4).

4. V65.06b -> V65.28 the "AutoHUD / MasterSwitch" era (Sep). this is the worst dead end, it went back and forth 8 times:
   - V65.06b SAFE/CENTERED: per-widget sliders (hud/prompt/item panel/lower icons). worked, "runtime-proven", but every widget had its own slider = fiddly.
   - V65.14/V65.16 AutoHUD ON: one whole-HUD transform on the root clip "MasterSwitch" (alias of `all`) with size + vertical offset. "Auto HUD must project the COMPLETE HUD/root". V65.16 log showed the observer failing at startup and falling back.
   - V65.18/V65.20 multi-root ApplyCorrectedMonoGlobalUi (MasterSwitch + ItemSelection + ItemTransfer + AlchemyBook) with VrUiSafeWidth/VrUiSafeHeight, V65.22 one uniform tangent scale, V65.25 SAFE/CENTERED default again, V65.28 fallback back in...
   DEAD END C - scaling the whole root / MasterSwitch. the root also carries the full-field fader, vignette, ratioStrips, popUp dimmer. scale the parent = the black death fade / loading fade / dialogue dim becomes a SMALL BLACK SQUARE in the middle of the view. that is the "black box" i kept seeing. V66.89 hard-disabled whole-root AutoHUD, V66.90 wrote it down: "Scaling a parent that carries fullscreen fades/dimmers necessarily scales their coverage as well".. the functions ApplyAutoVrLayout / ApplyCorrectedMonoGlobalUi / ApplyVrAngularSafeLayout are still in flashui_probe.cpp but nothing production calls them except the legacy owner path. do not revive.
   DEAD END D - the old safe-HUD writer applied coreScale=0.72 on top of KCD2's live scale -> HUD "small/soft" and in bad wide positions.

5. V66.64 (27 Sep) HUD LAYER - the thing that actually fixed it. the "Frankenstein" build. KCD2 draws the HUD movie onto the main backbuffer after the left/right eye images are preserved. hud_layer_v6664.cpp REDIRECTS those late OMSetRenderTargets binds (RedirectMainBind) into a private transparent texture (cleared 0,0,0,0 once per frame), copies it to a 2-entry cache ring at the PRESENT barrier (OnPresentBarrier), and the XR thread shows it as one head-locked XrCompositionLayerQuad (XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT) covering exactly the eye FOV. eye images untouched. F4 = HUD layer on/off (default ON).
   it was removed once: V66.78 went back to the V66.63 "late HUD replay into both eyes" because V66.77 had a "colored peripheral band / fullscreen death-fade-dialogue artifacts". V66.80 put the quad layer back because the replay lost late Flash text and white HUD graphics ("narrow V66.63 draw-count replay"). since V66.80 the quad layer IS the production path.

6. V66.90 Native Canvas (end Sep): with the HUD on its own quad the question became only "where inside that quad are the readable things". i read the real hud.gfx hierarchy: a 1920x1080 logical movie, top-level anchor families tl tc tr cl cr cc bl bc br + popUpVisual + tutorialInDialog; fader/vignette/ratioStrips/popUp are SIBLINGS. HUD.xml child paths: subtitles = bc.subtitles, action hints = br.actionHints, stats/quick slots = bc.mc_StatsContainer, compass under tc, dialogue sides cl/cr. so: fit the authored 16:9 rectangle into the comfortable rectangle of the quad and translate+scale each readable anchor by the SAME affine map, never the root, never bbc (vignette+info) and never cccursor (world-projected cursor). one owner at a time (None/Legacy/NativeCanvas), readback after each setter, rollback on failure.
   30 Sep regression run: 93 HUD deferral rows "status=7 ForeignLayout" - the native-corner guard refused the live anchors because KCD2 had already resized them for the square resolution (they were not at the 1920x1080 corners). V66.96 fixed it: "normalize KCD2's live resized anchors rather than requiring 1920x1080 authoring coordinates".
   V66.96b headset: "the old 72%/60% comfort box made readable HUD and loot panels unnecessarily small" -> readable box 0.88/0.78 (comment in ui_surface_geometry.h ComputeHud).

7. V66.162R1 (Oct) draw mask instead of visibility rewrites: the periodic Buffs visibility rewrite was removed ("status icons pop"), a verified retail HUD draw hook scales only the selected clips to 0 during Render() and restores the exact live scale after (ui_contextual_hud162r1::DrawMask). native flash keeps owning _visible/_alpha/animation.

8. C rounds (mid Oct): C10R2 pause fit (see §4), C10R4 palm vitals (see §2), C22b HUD lag (see below), C26 F2 rungs, C32 "never crop the HUD texture to the safe rect" (damage/fader pixels live outside it), C34: F2 on rung 1 at 22:22:52 on 16 Oct dropped the HUD to the legacy owner = black square, no vitals, no prompts -> the corrected HUD became the baseline and F2 can no longer change presentation (ui_hud_experiment36c5::Ladder::Corrected() returns true). C43 modal fit, C44c2 whole readable HUD.

9. R104/R105/R106 (5 Nov, SteamVR): under SteamVR i had NO HUD, no laser, menu only during the slowdown. claude found the problem in the layer submission: SteamVR validates every layer in xrEndFrame (quaternion length, rect inside swapchain, size>0) and refuses the WHOLE frame if one quad is bad; VDXR just drew them. R104 added xr_layer_guard_r104.h (renormalize quat, clamp rect, min 0.5 mm size, drop only unrepairable) and the HUD swapchain rebuild after 2 s instead of a session-long x_failed latch. R105 removed the sit-out ("a generic error cannot identify a culprit") - half right, R106 keeps ONE xrEndFrame per frame and quarantines overlays only on layer codes (-1,-39,-42,-43,-44): 1 s out, back one by one 300 ms apart, the guilty one 10/20/40/60 s, never off for the session. always-on log rows "R104 XR_LAYERS ...". not headset tested yet when this doc was written.
   R105 "minimal HUD" package was REJECTED (i did not ask for it): one-time rewrite of 8 INI keys (hints OFF, subtitles OFF, quick slots OFF, crosshair OFF...), plane 1.55 m -> 3 m, readable box 0.80/0.70, palm dwell 150 ms, ui_hud_policy_r105.h. all reverted to R104 in R106. the only R105 HUD ideas kept: QAMWeapon/QAMFood never owned by the mod and CombatCursorCross native (§2).

WHAT WORKS NOW (R106):
- capture: hud_layer_v6664.cpp (namespace kcd2vr::hud_layer_v6664): RedirectMainBind / OnLateDraw / OnPresentBarrier on the render thread, XrPrepare on the XR thread (fresh = published within 250 ms, s_age_expired162r1 counts drops), 1 ms image wait (R104, was 0), Ensure() recreates on size/format change, g_create_failed cleared by the R104 retry. desktop mirror SetDesktopMirror36(true) so the monitor still shows the HUD.
- geometry: src/ui_surface_geometry.h. Surface{distance=1.55f, y=-0.02f}; ComputeHud() shoots the 4 FOV corner rays of BOTH eyes onto the plane at 1.55 m, takes the max half extents *1.035 = quad size (covers both eye frusta so faders stay full field), then Fit(eyes, 1920/1080, 1.55, 8 m, comfortX=.88f, comfortY=.78f) = the binocular readable box, expressed as Hud::safe in stage pixels. HudLatch caches it per session, only aspect or a real eye-FOV change (>0.002 rad) recomputes - "Physical head movement is not a HUD resize event".
- submission: xr_manager.cpp ~4349: hudDistance=1.55f, hud_quad.space=view_space_, position {0,-0.02,-1.55}, size = surface width/height (fallback 1.82 m wide if geometry invalid). C22b lag: VrHudFollowSlow (default true, UI page checkbox kHudLag22) + VrHudLag 0.4 (INI only): in gameplay the quad is in local_space_ with an orientation that nlerps toward the head with tau = 0.05+0.30*lag s and is clamped to 3..10 deg behind (maxLag=(3+7*lag) deg). orientation only, never the position, never the texture crop (C32). menus never lag. the dice game moves the plane to the selected die depth (R93 dice_hud_depth_r92r3, 0.45-1.55 m) with recomputed coverage.
- layout owner: mod.cpp apply_f4_vrhud_safe lambda (~5850). enum HudLayoutOwner {None,Legacy,NativeCanvas,UniformReadable,WholeReadable}; whole_readable_wanted44c2 is `... || true` so the owner is ALWAYS WholeReadable = flashui_probe::ApplyWholeReadableHud44c2(geometry, VrUiCompactness 1.0, VrUiSafeWidth 0.65, top/bottom boundary, VrUiSafeHeight, icon scale) -> ui_native_canvas::ComputeAdjustableReadable49 -> g_canvas.Run (the V66.90 transaction with readback). then ApplyUniformHudElements171 for the compass/stats/lower sliders (VrUiCompassScale/Y, VrUiStatsScale/X/Y, VrUiLowerScale/Y/Spread). a key hash (g_parent_key59c7) skips the native calls when nothing changed. notifications (6 message containers) are authored at 50% in the companion pak (D63), the old C++ factor is off (SetNotificationsRequestedD80(false)).
- VrAutoCorrectHud / VrHudNativeCanvasFit / VrHudNativeCanvasSize/SpreadX/SpreadY are still read and saved but with the `||true` they do not pick the owner any more. dead keys, kept so old INIs dont lose values.
- the companion pak Mods/KCD2VR/Data/KCD2VR.pak (tools/build_companion59.py from companion/source, member list in companion/WORKING_BASE_MEMBERS59C4.json) carries our HUD.xml (R98 added `<MovieClip name="DiceCursorRing" instancename="cccursor.dice.cursor"/>`), hud.gfx with the 50% notification containers + glyph/ingredient callbacks, Menu.gfx etc. the D74_NOLETTERBOX_COMPAT addon is only for people who use a replacement hud.gfx. optional, untested.

open: SteamVR overlay acceptance (R104-R106 not yet in the headset). the "peripheral double image" at 1.55 m that R105 tried to fix with 3 m - not measured, dont move the plane blindly, the readable box and the pointer inverse mapping use the same Surface.

if you want to fix: HUD missing/black -> KCD2VR.log "V66.64-HUD XR swapchain", "R104 XR_LAYER", "R104 XR_LAYERS refused". HUD too small/large -> ComputeHud comfort .88/.78 and ComputeAdjustableReadable49 in ui_native_canvas.h, VrUiCompactness. HUD cut at the edges -> HudLatch (is the aspect from xr_hud.width/height right?). a native clip not scaled -> the anchor list in ui_native_canvas.h, never the root.

#### 2. palm reveal / immersive contextual HUD

what i wanted: no permanent HUD in the world. look at my open hand -> i see the compass or the health bars for a moment, then they go away. the quick weapon/food selector must still work like in the flat game.

HISTORY:
1. V66.69 F4 "immersive UI" wrist HUD (ApplyWristHudLayout): Stats/Compass clips moved next to the hands at 0.72 scale, laser only with empty right hand. worked as a demo, but moving 2D clips on a head-locked quad is not a wrist panel. retired (f4WristApplied restore path still in mod.cpp).
2. V66.162R1 (10 Oct) contextual HUD, "Contextual HUD (VRIK only)" at the bottom of the HUD tab, default OFF. RIGHT palm = compass, LEFT palm = health/status/quick access. gaze test in ui_contextual_hud162r1::Looking(): controller GRIP pose, palm normal = +X left hand / -X right hand, palm 0.15-1.05 m from the head, head forward dot toPalm > 0.766 (40 deg) to enter / 0.669 (48 deg) to keep, palm normal dot toHead > 0.423 (65 deg) / 0.309 (72 deg). enter 100 ms, leave 160 ms (Gaze class). draw mask DrawMask scales hidden clips to 0 only in Render().
3. R59.16C10R16: the VRIK requirement dropped ("no VRIK requirement" in Gaze::Update). C37C3/C41: VrContextualHud default = true (= my ini).
4. C10R4 (14 Oct): headset log 00:32:39 showed Stats parent visible/alpha100, Stats itself INVISIBLE/alpha0 while HealthBar/StaminaBar children were visible -> writing alpha alone can never show the vitals through an invisible Stats. fix: a scoped _visible lease on bc.mc_StatsContainer and Stats during contextual gameplay, the exact Stats native writer hook (NativeHudVisible175) records native intent, palm reveal at alpha 100 even when native alpha was 0, restore before the ordinary owner writes on exit.
5. R59.16C10 HUD review 3c, the problem was the Stats gate: gate the Stats CHILD (bc.mc_StatsContainer.mc_StatsEle), never bc.mc_StatsContainer - that parent holds qamWeapon/qamFood, alpha 0 on it blanked an open quick selector. Buffs/Crime/Wanted/Trespassing moved into the right reveal (group 2); "they were group 4, which no branch ever showed". (comment in ui_palm_alpha44c2.h)
6. C22b (15 Oct): "a passing glance must not reveal" -> PalmReveal::dwellMs = 1000 continuous gaze on the OPEN palm, then shown max 3000 ms, then you have to look away and back. plus HideFight22 (128): the Hints clip (button hints, quick-talk prompt) is hidden while HenryInCombat178r31 or OpponentLocked58, held 3 s.
7. D84 (end Oct): sides SWAPPED to what i actually use: LEFT open palm = compass, RIGHT open palm = health/vitals/buffs. CompassGroupD84=Left, VitalsGroupD84=Right. "Semantic visibility and actual FlashUI alpha/visibility consumers agree." headset acceptance was still open in the D84 notes. D85R5R1: the palm normal is SIGNED now - an absolute dot accepted the back of the hand including a weapon grip. (ui_contextual_hud162r1.h)
8. R105 wanted 150 ms dwell, no 3 s expiry and the selectors hidden by "their own permission" - rejected in R106. my two asks went in instead: QAMWeapon/QAMFood (clips 3,4) are NEVER owned by the mod in any HUD mode - the game shows them on selection change and fades them itself; CombatCursorCross (13) is native (the game draws it only in a fight on the targeted enemy), the ordinary cursors CursorCross/CursorOutline/Cursor (14,15,19) still follow the "Center point" (VrHudCrosshairVisible) setting, alchemy gaze still pins 19.

WHAT WORKS NOW:
- switch: release page "Immersive HUD: ON/OFF" (release_menu_r92 Row::Hud -> s.contextual_hud) = VrContextualHud (default true, host.inl:540/591). also kExpContextualHud on the dev Experiments page.
- gaze+dwell: ui_hud_experiment36c5::PalmReveal (dwell 1000 ms, shown < 3000 ms, hides at once when the hand closes/turns/loses tracking) over ui_palm171::Gaze over ui_contextual_hud162r1::Looking. open hand = no trigger touch, no grip, no buttons, no held physical item (the "open" array comes from mod.cpp).
- clip table: ui_contextual_hud162r1::Clips (22 entries), groups Left=1 Right=2 Other=4 + preference bits HideCrosshair163=8 HideCompass163=16 HideStatus163=32 HideLower163=64 HideFight22=128. the preference bits are VrHudCrosshairVisible (default false), VrHudCompassVisible (true), VrHudStatusIconsVisible (false), VrHudLowerIconsVisible (true).
- three consumers must agree: (a) draw mask / semantic groups ui_hud_visibility175::Group() + ExperimentGroup36(), (b) palm alpha ui_palm_alpha44c2::Controller (11 Gates: Compass, bc.mc_StatsContainer.mc_StatsEle, Buffs, Crime, Wanted, Trespassing revealAtFullAlpha; bbc.vignetteHolder/GameLog/FancyEvent/CommonEvent/ShootingContest group 0 = never), (c) visibility transition ui_hud_transition170::Controller + the native writer hooks in flashui_probe.cpp (NativeHudVisible175, NativeHudDisplay54, NativeDestroyHudWrapper175) that keep a Registry keyed by {wrapper,handle,object,hud,generation} so a recreated HUD never gets a stale restore. mod.cpp drives them with flashui_probe::SetHudSemanticOwner54 / UpdatePalmAlpha44c2 / UpdateHudVisibility170.
- what is never touched: Hints/TutorialMessage/Subtitles (only size, via DrawMask Begin sizes), cccursor parent, InfoText (item name at the cursor), Bubbles, full-field faders.

if you want to fix: palm does not reveal -> KCD2VR.log "PALM_ALPHA" rows with F2, check Looking() angles and that the hand counts as open; vitals invisible but alpha ok -> the Stats _visible lease (C10R4) in flashui_probe NativeHudVisible175; selector blanked -> you gated bc.mc_StatsContainer again, dont; a clip you want hidden in a fight -> HideFight22 group in Clips.

#### 3. my own VR menu (vr_menu.cpp)

HISTORY:
1. V46.2: no ImGui (a second D3D12 device crashed the game before), a GDI-drawn 650x820 BGRA bitmap uploaded to one texture on the GAME's device, drawn with our own blend PSO through DeviceHook::ExecuteCommandListsBypass, input via SetWindowSubclass on the game HWND. V46.3.1 the parchment look (MIS_MENU_BOOK.bmp).
2. V58: two-page book, curved XR_KHR_composition_layer_cylinder when the runtime has it (VDXR does), quad fallback; dragging a slider > 0.5 s fades the book to 20%. V58.2: LEFT stick navigates the book, right ray points, RT clicks, A selects, B closes. V61.27: 20% smaller (1.64 m wide, texture 1100x680, at -1.45 m). V61.31: laser = the 3x3 gold texel of the menu texture stretched as a thin billboard quad from the grip to the hit (menu_laser_quad, height 0.0018 m). V62.4: the OpenXR AIM pose for the ray instead of grip -Z ("grip -Z was noticeably finicky on Touch/Quest and forced your hand close to the torso"). (xr_manager.cpp ~3700)
3. how to open it changed a lot: F7 / left X (V58), Left Grip + LS Down 0.5 s (V65.06), Right thumbrest + LS Down 0.5 s (V66.108, still current), F12 keyboard fallback (V65.14), Shift+F7 (developer_mode.h).
4. R92 (3 Nov) release/developer split: the startup launcher has a "developer mode" checkbox (D80), developer_mode::Enabled() is read once per process. without it the Home page shows VR CONFIG / GRAPHICS / UI SETTINGS / IMMERSION AND CONTROLS / UI / CONTROLLER CONTROLS and the VRIK buttons; "EXPERIMENTS / LIVE SETTINGS" and "COMBAT EXPERIMENTS" and all the ~30 dev pages (MenuPage enum in vr_menu.h:438, IsDeveloperPage()) are hidden and GoToPage() bounces to Home. (vr_menu.cpp 952-1001, 1685-1689)
5. R98: i found the gore switch missing from IMMERSION AND CONTROLS (it only lived on dev pages) -> Row::Gore cycles OFF / BLOOD x2 (VrBloodyFights) / + HEADS + LIMBS (VrDecapitation + VrDismemberment), log row "R98-IMMERSION ..." and the gore pak state only in the log.

WHAT WORKS NOW:
- release page: src/release_menu_r92.h, 2 pages x 8 rows: Comfort vignette, Joystick turning, Alchemy hands, Smithing hands, Crouching, Immersive HUD, Button icons (XBOX/PLAYSTATION = VrPadGlyphStyle 2/4), Dialogue view / Enemy combat, Activity gamepad controls, Physical walking and dodges, Fast block, Index-finger Quick Talk, Physical impalements, Gore, Body height. advance()/value() are pure functions, click goes through ApplyState + save_settings (vr_menu.cpp ~1057).
- Experiments (dev): src/subsystems/experimental/live_settings_d39.h = the reducer: struct Settings (every live bool/float), enum Group {Body,Hands,Alchemy,Smith,Gear,View,Objects,Npc,Combat,RepairsD65,MotionD67,CraftD67}, kRows[12][8] with title + one-line detail, Id enum per row. live_runtime_d40.inl applies them live and saves in the background ("automatic background save"). this is where "every INI key = a switch" is enforced: a new key gets a Settings member, a Row, a label in value(), an apply in d40, and the INI read/write in the owning host.inl. the rows are "larger, high-contrast bold explanations" since D85 because i could not read them.
- VR CONFIG > UI tab (MenuPage::VrHud/VrHudElementControls/VrHudElements): the HUD sliders (compactness, safe width, boundaries, compass/stats/lower, hints/control prompt/subtitle scale, kHudLag22 checkbox, hud_xml buttons).
- developer shortcuts: F2/F3/F4/F5/F9/F11/F12 and plain F7 need developer mode (developer_mode::IsDeveloperShortcut). F1 body follow, F6 scene escape, Shift+F8 calibration are always available.

#### 4. native menus: Theater, full menu fit, contextual fit, pause, modal

what i wanted: inventory/map/shop readable as a big flat screen (Theater) with a laser; loot/chest/cooking small panels in the world at the right size and round; pause menu as an overlay with the world still head-tracked; no tiny ovals.

HISTORY:
1. V58-V61: F6 manual theater (the whole mono frame as a flat quad). the automatic FOV-based menu detector was retired in V58 ("action cameras produced false positives during bow/jump/combat recordings").
2. V63.0 (UI88 in the code): the Touch AIM ray projected onto the real theater quad and normalized to the captured source -> Win32 mouse injection drives inventory/shop/cooking/map. the quad is flat_ui_distance_ (0.80-3.20 m) and width 1.82*(1.58/flat_ui_scale) clamped 1.05-2.65 m; full_menu_fit_ runs ui_surface_geometry::Fit on it; V66.97 passive_full_frame_scale_ 1.20 for loading/cutscene screens only. (xr_manager.cpp ~3745)
3. V65.72 Theater input = retail APSE keyboard/mouse transport, V65.24 game-state-only (Native Menus Creator element names + lifetime hooks, ui_native_state.cpp) decides Theater vs Immersive - NOT FOV, NOT cursor presence. V66.64 retired the "cursor gone + 800 ms -> immersive" auto exit (it closed the inventory under me); close needs an explicit request + 120 ms native absence, V66.65 logs "INVENTORY UNKNOWN" after 3 s.
4. 30 Sep regression: the catalogue gave ItemTransfer/ItemSelection Role::ContextMenu -> ContextualFit -> FullFrame() true -> loot panel in full theater. contract says standalone loot/transfer/selection = IMMERSIVE fitted, centered. fixed in V66.96/V66.97 (ApplyContextPanelLayout, the "immersive_laser_surface" hit test in xr_manager ~3814 against the HUD surface geometry so the laser hits the exact panel). (KCD2VR_2026-09-30_RUNTIME_REGRESSION_RECONCILIATION.md)
5. V66.109 pause: the Clean Pause mod proved Menu rendering and pause DoF can be separated. pause = PauseOverlay: KCD2 really paused, world stays head tracked, native blur/dim full field, only the readable menu roots fitted inward. C10R2 (13 Oct): the pause fit fitted 9 authored root children (cl, cr, titleBox, HelpContent, tooltip, actionHint, mod, support, version) but the actual buttons live in Container0/Container1 that Menu.gfx `setup` attachMovie's at runtime - siblings, not children; cl/cr are EMPTY sprites. "Successful readback from the original nine clips could truthfully report changed geometry while the actual menu stayed at native full-canvas scale." fix: both containers required, and Menu.gfx uses independent g_diffScaleW/H (1920x1800 source: X 100% / Y 60%) so the pause fit normalizes by the observed cl scales first. (ui_pause_overlay.h, ApplyPauseOverlayLayout)
6. C43 modal fit (ApseModalDialog = craft/alchemy result popup, LockPicking/Pickpocketing S1, SkipTime B2 wait clock) via ui_modal_overlay43::Fit. R98 (5 Nov): i saw the lockpick lock and the result popup as wide ovals. cause: Fit used xs from the 16:9 safe box on a 2064x2208 texture (=1.9) and ys from height/1080 - only round if the 1920x1080 stage is stretched over the whole texture; these movies anchor at the stage center (960,540) in square stage units. fix: one physical scale, ys = xs*(native.ys/native.xs), y from the quad metres-per-unit. live-pixel movies unchanged. tests/recovery43_test.cpp. not seen in the headset yet.
7. dice: R92 put an XR ring at the die depth and hid the whole native DiceCursor -> double circle (the native ring is in the flat HUD texture, our ring is a second quad). R98: only the ring child DiceCursorRing (cccursor.dice.cursor, new name in our HUD.xml) is set invisible while witness175.dice is live, the description flag stays native, the XR ring is never submitted, retry next frame, restore at game end. log "R98 DICE_RING hidden=1". (ServiceDiceMarkerR92 in flashui_probe.cpp ~2251)

WHAT WORKS NOW:
- router: src/ui_menu_router.h: Owner {None, InventoryMap, Shop, Frontend, Pause, ContextualMenu, CraftingMenu, NativeContext, LoadingScreen, GameOver...}, Presentation {Immersive, ApseTheater, FullMenuFit, ContextualFit, LoadingFit, PauseOverlay, DeathOverlay}, Input {Gameplay, MenuNavigation, PadNavigationOnly, PreserveNative, PassiveBack, Blocked}. the catalogue src/ui_native_catalogue.h lists every retail UIElement with Role and Alignment (from the V64.21 census). Inventory/Character/Map/Shop = ApseTheater + MenuNavigation; crafting menus and loot = Immersive; frontend = FullMenuFit; pause = PauseOverlay.
- pointer: either grip adds the laser (ui_menu_input.cpp, "M1 UI_LASER_INPUT ..." receipt rows), the pointer hand's trigger is withheld from XI so there is no double activation, a ray miss invalidates the click, drag release is always sent, UI grip ownership hard-releases on close and needs release+repress before a world grab. B held 2 s = Escape recovery (ui_back_escape_m1.h, "M1 INPUT B_HOLD_ESCAPE heldMs=2000") independent of what the menu classifier thinks.
- menu quad in xr_manager.cpp: menu_quad at -1.45 m / 1.64 m, flat (theater) quad from presentation59c2 anchor (QGazeNoRoll for native menus, QYawOnly otherwise), optional flat_game_cylinder when XR_KHR_composition_layer_cylinder exists.

#### 5. prompts, hints, subtitles, glyphs (D80 policy)

- glyphs: SetInputId forced to 2 (xbox) since V64.20; VrPadGlyphStyle 2 XBOX / 4 PLAYSTATION changes only the artwork ("Xbox and PlayStation glyph choices change the pictures, not the button layout. Empty symbols are a font/movie problem"). D68 glyph assets in the companion pak. the unsolved one: Quick Speak shows the shoulder-family icon, physical B should show B (V66.109 contract §4). the companion defaultProfile.xml has chat_init_with_focus xboxpad = xi_x first then xi_shoulderl (C22b).
- station prompts (crafting hints): D80 "Station prompts use 30% size and can hide after ten seconds, returning for a new work step or a look at an open palm" -> D81 migrated VrMinigamePromptScale=0.45 once (VrRepairDefaultsD81 marker, mod.cpp:3390) -> the current INI asks 0.90 ("twice the tested 45%"). prompt_policy_d80.h: minigameScale .45 default, timed (VrTimedInteractionPrompts, default true): visible 10000 ms at station entry / new phase, 3000 ms after a palm look; the MovieLease hands the station children from the C++ scaler to the movie-local owner so nothing scales twice; HintsLease writes the retail CVar wh_ui_ShowHints (VrShowInteractionHints, Immersion > SHOW HINTS) and restores our own write only. "Turning native hints off wins over a timed reveal." the old global hint sliders (VrUiHintsScale/Inset/Y) still control the global layout, they no longer force 60%.
- subtitles: bc.subtitles, only scaled (VrUiSubtitleScale/Y through the readable parent), never hidden by the mod. R105 "VrHudSubtitlesVisible=false" was reverted. the fc_setSubtitleSize percentage experiment was wrong (it takes a mode enum) and is retired (UI90_HISTORY §"Correction").
- notifications: 6 message containers authored at 50% in the companion (D63).

#### 6. touch bindings and chords

what i wanted: all gameplay on the Touch controllers, grips free for physical things (grab, holster, bow), one modifier for the rare stuff.

HISTORY:
1. V58.6 (early Sep) first gameplay profile, keyboard/mouse injection (SendInput, KeyTap/SetKeyHeld/MouseButton in touch_bindings.cpp, log prefix "V58.6-INJECT"): LS move, L3 sprint, RS X turn, RS up jump, R3 crouch, RT attack, LT block, A use, Left Y weapon swap, Left Menu short inventory / hold pause, X = MIS menu, Right Grip RESERVED. telemetry "V58.x-TOUCH-STATE" twice a second.
2. V64.56: the internal XI bridge (native_input_look_lab.cpp "universal Touch->KCD2 gamepad bridge, XI templates bootstrap from KCD2's own xi_* names"). V65.19: keyboard/mouse execution + Xbox prompt presentation as the stable pair. V66.96: preferred transport "OpenXR Touch -> KCD2VR VR premap -> KCD2 native XI" (native_vr_pad_premap.cpp, PublishGamepadState), scan codes as explicit degraded fallback, discovery bounded to two attempts. V66.109 contract still said "Native XI is still historically OFF on the tested V66.108 runtime"; the D-round logs (31 Oct) show nativeXI=1 on F5 pad rows, so it is live now. both paths coexist: the premap publishes a gamepad State {lx..dpad} to XI, specific things (map key, skip time, visor, torch, minigame superactions Q/F/R/X/I/E) are still KeyTap'ed as the retail keybindSuperactions.
3. modifier wars: V58 LT+RS = D-pad layer; V65.06 Left Grip + LS Up map / LS Down 0.5 s menu; V66.2 README: Left Grip utility, X tap menu, X hold skip time, B hold draw/sheath, Right Grip tap quick reply. V66.108 (3 Oct): "Left Grip is now entirely physical", the RIGHT THUMBREST capacitive touch (quick_access_touch = /user/hand/right/input/thumbrest/touch, a boolean) is THE modifier - the touch alone does nothing, a digital left-stick direction with it fires. V66.109 added thumbrest + X hold (Wait/Skip Time) and thumbrest + Left Menu = pause.
4. R58C2 pause chord: the pause chord is one owner BEFORE XI/fallback/UI routing (pause_chord58c2::State: quarantined until a neutral Menu, consumed once, consumes the Inventory release so Left Menu does not also open the inventory); it "deliberately survives Reset/UI transitions". (touch_bindings.h:98)
5. held map chord: thumbrest + LS Up held 500 ms (R92 "500ms map chord", was 1.0 s in V66.108; map_chord_since_ in touch_bindings.cpp ~305): if held_map_m1::Selected() the static paper map (M41) is requested, otherwise KeyTap(config_.map). "H25 MAP_GESTURE route=... accepted=..." row.
6. book pages: in the recipe book RS X = read_prev/read_next (VK_LEFT/RIGHT), Y = toggle (Q), A = select (E), B = back (R). in alchemy: LT bag/bellows (Q), RT sandglass (F held), X reset (R), thumbrest+LS Up = move pot (X held), Down = ingredients (I). B hold >= 250 ms = minigame exit (Escape held), B short = R. these are the retail keyboard superactions, "not invented aliases". (touch_bindings.cpp ~360, V66.98)

WHAT WORKS NOW (gameplay, from touch_bindings.cpp + the V66.109 contract):
LS move, L3 sprint, RS X snap/smooth turn (VrTurnMode), RS Up jump / RS Down crouch (unless Right Grip owns free look), RT attack/bow, LT block, A use, B tap Quick Speak (VrBQuickTalk) / B hold 1 s native Start, X torch tap / visor hold (x_utility_context), Left Menu inventory, Right thumbrest + Left Menu pause, + X hold 800 ms Wait, + LS Left/Right D-pad weapon/food quick access, + LS Up 500 ms map, + LS Down 500 ms VR menu, Right Grip + RS free look / bow aim, grips physical. in the VR menu everything is consumed (mis_menu_visible branch). in authored minigames the gameplay chords stand down (authored_minigame branch, activity_input163 witness175 with 31 action maps from V66.162R1).

#### 7. OpenXR input profiles (R102)

HISTORY: until R101 one xrSuggestInteractionProfileBindings call with the full Touch set including thumbrest/touch and trigger/touch. VDXR accepted it. SteamVR 2.18.4 rejected ONE path and with that the WHOLE profile -> "no laser, no moving, no controls" in my SteamVR test (log b2b136b1).
WHAT WORKS NOW: XrInput::SuggestBindings() in xr_input.cpp:129: Touch CORE set (grip+aim pose, trigger/value, squeeze/value, thumbstick, thumbstick/click, X Y Menu A B) first, if refused the MINIMAL set (poses, trigger, grip, stick), then each optional pair (thumbrest/touch, trigger/touch, haptic) is added only if the runtime accepts it, and the LAST call always re-suggests exactly the accepted set. then independent suggestions for valve/index_controller (left A/B as X/Y), htc/vive_controller (trackpad as stick, right menu = B), microsoft/motion_controller (trackpad click = A/X), khr/simple_controller (select = trigger). every result goes to KCD2VR.log once, "R102 XR_INPUT profile=... result=CORE|MINIMAL|FINAL_REFUSED bindings=N", and "R102 XR_INPUT bound profile left= right=" once per change. R105/R106: every xrSyncActions starts from neutral, focus loss clears everything, session STOPPING/READY restarts without re-creating actions. not headset tested as of R106.

#### 8. settings / INI rules

- SETTINGS.md is the player doc. keys keep their old numbered names (VrMotionTrialD67, VrCrossbowReloadRouteR96...) because renaming loses a saved choice. the INI is an exit snapshot: "It must not be used to reconstruct the live options during an earlier action" - that bit me more than once when a setting was "proved" from the saved file.
- defaults: a feature i called working is ON when the key is absent (dog petting, riding hands, 6DOF interactions, VrContextualHud, VrHudFollowSlow, VrImpaleFastDeathR106...). a saved OFF stays OFF. updating never replaces the INI.
- one-time migrations are marked: VrRepairDefaultsD81=1 (prompt scale .45, smith semi anchors, lively NPC, NPC listening), VrHeadSmoothPreferenceVersion 3 (C22b). R105's VrMinimalHudDefaultsR105 migration (8 HUD keys) was rejected and is not in the tree. the R1 marker now only controls the prompt migration.
- config file guard: core/config_file_guard_d62.h reads the whole file, checks it did not change during the read (Status::Changed), keeps ONE last-good checkpoint IN MEMORY (R96; the earlier .last-good.*.bak, stowed-bow and migration .bak files are gone - "no extra files in the game folder"). the launcher preserves INI + calibration.
- VrHudLag (0.4) is INI-only strength, the on/off is the menu checkbox. everything else HUD-related has a row.

#### 9. diagnostics that touch this area

capture168.h: enum Domain {Combat, Interaction, Hud, Reload, Hands, Vrik, HandReference, Bug, Physics}; Hud folds into Bug (F2), Hands into Interaction (F5), HandReference into Vrik (F7). finite 180 s leases, 5 s drain. F2 = bugs (+ HUD: READABLE_PARENTS, PALM_ALPHA, DRAW_SNAPSHOT, UI-STATE visible bits, XR_LAYERS detail, dice marker); Shift+F2 performance only; D84 map profiling needs F2. always-on without any key (few rows): R102 XR_INPUT, R102 XR_RUNTIME, V66.64-HUD XR swapchain ready/FAILED (max 40), R104 XR_LAYER repaired/dropped (first 2/3 per kind), R104 XR_LAYERS refused + 30 s summary, R105 XR_HEAD first pose, R91R1 lifecycle receipts. the F2 ladder has NO presentation rung any more (C34) - it cannot drop the HUD.

#### 10. quick "where to look"

- HUD gone / black / cut: hud_layer_v6664.cpp (capture), xr_manager.cpp ~4340 (quad), xr_layer_guard_r104.h (SteamVR), KCD2VR.log R104/V66.64-HUD rows.
- HUD size/position: ui_surface_geometry.h ComputeHud (.88/.78, 1.55 m), ui_native_canvas.h ComputeAdjustableReadable49, mod.cpp apply_f4_vrhud_safe, UI tab sliders in host.inl (VrUiCompactness, VrUiSafeWidth .65, boundaries).
- HUD lags/wobbles: C22b block in xr_manager.cpp (tau, 3..10 deg), VrHudFollowSlow / VrHudLag.
- palm reveal: ui_hud_experiment36c5.h (1000/3000 ms), ui_contextual_hud162r1.h (angles, Clips), ui_hud_visibility175.h Group(), ui_palm_alpha44c2.h Gates, flashui_probe.cpp NativeHudVisible175 / UpdatePalmAlpha44c2.
- selectors / crosshair / fight cursor: ui_hud_visibility175::Group (3,4,13 = native), VrHudCrosshairVisible.
- loot panel / lock / popup shape: ui_modal_overlay43.h Fit, ui_context_panel.h, activity_overlays_s1.inl, catalogue roles in ui_native_catalogue.h.
- pause menu size: ui_pause_overlay.h + ApplyPauseOverlayLayout (Container0/Container1 required), VrPauseOverlaySize.
- theater size/distance: flat_ui_scale_ / flat_ui_distance_ (xr_manager.cpp ~3745), passive_full_frame_scale_.
- laser/pointer: xr_manager.cpp menu ray (~3700, aim pose, 1100x680 uv), flat pointer (~3760), immersive panel hit (~3814), ui_menu_input.cpp M1 rows, ui_back_escape_m1.h.
- a binding: touch_bindings.cpp Update() (gameplay after the minigame/menu branches), native_vr_pad_premap.cpp for what reaches XI, pause_chord58c2.h, held_map_m1, xr_input.cpp SuggestBindings for the OpenXR side.
- a new setting: live_settings_d39.h (Settings + Row + Id + value label) -> live_runtime_d40.inl apply -> host.inl of the owning subsystem (bind + wb/wf) -> SETTINGS.md one paragraph. release-page rows in release_menu_r92.h.
- prompts/hints: prompt_policy_d80.h, prompt_device_lock.cpp (station dispatch, SetInputId), VrMinigamePromptScale, VrTimedInteractionPrompts, VrShowInteractionHints.
- dice ring/description: flashui_probe.cpp ServiceDiceMarkerR92, companion HUD.xml DiceCursorRing, hand_select_native_d85.inl for the hand pointing cursor.

honest status: the HUD on VDXR with the 1.55 m quad, the readable fit, palm reveal with 1 s dwell, Theater inventory/shop with the grip laser, the pause overlay, the thumbrest chords and the release menu are all headset-tested over many runs (Oct-Nov). NOT yet seen in the headset when this was written: R98 modal round fit and dice ring, R102 profiles, R104-R106 SteamVR layer guard/quarantine, R106 selector/fight-cursor change. the "double image at the periphery" is a real report with no measured fix.


---

## 10. WHERE THINGS STAND

the list below is the state at R106 (5 nov). R107..R149 came after it, one small change at a time (see 1.1 and the
KCD2VR.ini comments), so some of the "untested" points below may have moved since - check the INI comment for the R
number and my last log before you trust either.

what i still want (the big goals, unchanged since D84): clean separation of hooks and diagnostics, interactions and
corpses that just work, contact-first combat, crafting and reloads by hand, roomscale and dodges, a physical dice cup,
knives, DLSS/FSR/XeSS switching without restart.

works (i tested it, more than once):
 - native stereo, the body, hands, the sword with weight, the bow (arrow in the hand, release to nock), bolt on
   the medium crossbow, doors, pickups, small props (R100 brush swap - "THE SMALL OBJECTS ARE WORKING"), corpse
   dragging (R101), dog petting, alchemy ("nearly perfect"), smithing, dice (marker + ring), horse riding
   (seat foundation R97), the VR menu, Immersion and Controls page incl. Gore and Body height rows.

open / untested:
 - SteamVR as OpenXR runtime: R102 fixed the controllers (all-or-nothing binding), the overlays (HUD, lasers, VR
   menu) still missing in my R103 run; R104/R106 add the layer guard + sit-out and the R105 session lifecycle.
   NOT headset proven. the R106 XR_LAYERS rows in KCD2VR.log will say what steamvr refuses. VDXR works.
 - crossbow: goat's-foot lever / cranequin in the hand still "native moving, not in my hand" in R103 run; R103
   captured the lever grip at SpawnLever, R105 takes it from the rig reference. one of them should be
   right. the F11 row "XBOW_TOOL_GRIP" tells which. light crossbow string pull by hand never seen in a log.
 - free targeting R103 ("ON (WORKS IN COMBAT)", native auto-turn vetoed in the fight phases): untested.
 - held props / dragged corpse pushing henry away: R103 self-collision lease (+0xEC/+0xF0 class bits): untested.
 - impale: the hang works, the killing-blow depth rule (R97) works in logs; the body still dies animated first -
   R106 plays the death clip 3x so the ragdoll comes sooner. untested. "nearly instant ragdoll" is not possible
   without the game's handover, see 5.x.
 - kicks everywhere (R99 kickInputReadyR99, block press -> phase 256): untested since R99.
 - weapon flicker while walking: R98 rebase + shared frame delta. i said R97 was "quite happy", R98+ untested.
 - upside-down / backwards start sometimes (steamvr?): R105 logs the first head pose (R105 XR_HEAD row), R148 recenters a start/load that faces >90 deg away.
 - black frames: R153 (log 8 Nov 11:03-11:23, SteamVR): in the F2 windows every xrEndFrame had a projection
   (F2_CAUSAL stage 12 flags=layer count 2-4, never 0), refused=0, every failed eye copy was held (EYE_HOLD reshown
   17 at the first load, 6 at 11:21:48-53, noImage/tooOld/mismatch 0). gaps R141 did not cover: Theater frames
   (main menu / loading theater) with no copy went out with NO layer, and the menu vanished during a hold. R153
   holds the theater picture too (xr_eye_hold_r141 RecordTheater/PickTheater, same ticket, 2 s) and re-shows the
   last menu image during any hold. still open: one empty frame at a scene switch (ticket changed mid-copy, by
   design), and a long load where the game gives nothing for >2 s.
 - mortar / "release as drop in" over the kettle misbehaving in R101 run: R103 admission fix, untested.
 - the plate held in alchemy misaligned: R92R3/R103, untested.
 - pose-restore can in theory keep VRIK off until restart if the exact failed buffer never comes back (the R105
   notes said so too, never seen in a log).
