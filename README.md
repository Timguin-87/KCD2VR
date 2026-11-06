[README.md](https://github.com/user-attachments/files/34489817/README.md)
# KCD2VR

Tracked hands, weapons and room movement for Kingdom Come: Deliverance II.
The game owns combat results, inventory, recipes and interaction legality.
The VR side supplies tracked movement and the presentation of those actions.

****KCD2VR - Medieval Immersion Simulator****

adds OpenXR virtual reality support to Kingdom Come: Deliverance II. It features full 6DoF head tracking, full VRIK system, full combat system rework, separate options for minigames, comfort options, controller bindings for movement and combat, a dedicated in-headset VR menu. I wanted to make the best game I ever played fully playable in VR, that it feels like a native VR game as much as possible, but with working RPG systems and progression. 

Note: This is an beta version, made by passion-driven solo developer, which I build in my free time over the last... god... 12 painful months very slowly, then faster with the help of expensive AI and now ... I need a long vacation. 

**Features**

- Full 6DOF and full VRIK controls for every weapons, holster system, full bow handling, full weapon handling and blocking, hand reloads for crossbows and bows, working combos and masterstrikes (with optional gore mod with decapitations too) 
- native Touch controller bindings — movement, camera, combat, interactions, D-pad simulation (right thumb rest + left joystick on oculus) and VR laser (by holding left or right grip in the menus - working as a mouse pointer) or you can choose SteamVR bindings (Inventory is holding X) or ps2vr bindings (no thumb rest option using capacitive touch controls)
- working 6DOF crafting minigames, smithing with real hammer strokes (optional hold LT for "stabilize the iron on the anvil"), working alchemy
- Dedicated in-headset VR menu (no need to pull off the headset to change settings) with optional developer access
- True native stereo, optional mono options for lower specs (the game is very demanding)
- Dialogue camera handling tuned for VR (native or forced 1st person with VRIK)
- "Theater mode" for the inventory, character, cutscenes and cinematic dialogues
- immersive HUD option (look at your open right palm to show health, or left for compass) and in game VR map parchment (right thumb rest + left joystick up for 0.5s), gesture controls (lower helmet visor with left trigger move near your left temple up and down), point with your index finger at your dog or npc for quick talk, or taunt them during a combat
virtual holster system (see below)
- immersive bow and crossbow reloads and handling (pull the arrow from your quiver and put it in your bow and pull the string with your right hand holding grip, then release for shooting, or crossbows - pull the string with your right hand grip and then take out bolt and put it in the crossbow slot, holding LT + tap of RT shoots like in game) 
- progression system, where the weapon skill and RPG stats decides your weapon speed (more or less "gravity drag")
- you can pet your dog with your hand! (if the game decides you can)
- working horse combat and riding
- Fallbacks if something is not working (F6 for switching theater or native, F7 for VR menu), holding B for 2 seconds for escape (there are many menus, maybe I miss something it this very long game)

except bugs, but I will work on them slowly over the next months (the game is too long to test every possible quest etc), I can't verify, if it works with other vr systems (maybe you need your own controls mapping through steam vr), some things I missed... except some flicker sometimes, sometimes you need to recenter (T pose is best for it) after some actions or recalibrate the VRIK
If you want to support the development or just to say thanks, you may "Buy me a coffee" or something, much appreciated.
                                                         
                                                         
**Installation instructions**
Download the latest version from the Files tab.

Extract the files into kcd2 Master folder (dinput8.dll, Start KCD2VR.cmd and KCD2VR.ini should be next to WHGame.dll and game's .exe) - you may need to "unblock" the .dlls in your system (right click - properties, unblock .dll), replace the existing dinput8.dll, if you have one (other .asi mods should still work)

Steam:
.../KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/
GOG:
.../KingdomComeDeliverance2/Bin/Win64MasterMaster/


use my user.cfg for custom resolution (all you need is add -devmode +exec user.cfg in the shortcut, or in steam - properties launch options), this one goes to the root folder, where system.cfg is, or start the game with .cmd to choose your resolution according to your headset and it will "populate" the user.cfg for you
unpack the supplied folder with KCD2VR.pak and put it in the "Mods" folder

Download optional no letter box mod: https://www.nexusmods.com/kingdomcomedeliverance2/mods/34?tab=files and put it in the root kcd2/Mods/ directory.

Download optional "Gore Mod" from optional files with working decapitations (I am proud if it... but my blender skills are limited) and again, put it in the "Mods"

Put your headset on, for the first launch: launch the game through "Start KCD2VR.cmd" and choose your VR headset, resolution, upscaling, details, controller scheme, if you want the normal or "developer mode" with A LOT of options for finetuning the experience in game or in .ini
holding grip enables vr laser (left or right) for easier selection in menus, otherwise use joysticks and triggers

**Best practice:** 

**Visual Settings:** 
Turn Motion Blur OFF. Depth of Field (DOF) OFF. Most of the settings are already in user.cfg and in Optimized VR Settings
choose "non developer" mode in .cmd (easier VR menu) or developer mode (lot of options)
you can configure most things in the VR menu (hold right thumb rest + left joystick down to open and close it), left and right grip for VR laser pointer
IMPORTANT WARNING: Do NOT change resolutions or apply changes inside the standard in-game Graphics menu—doing so forces a display mode reset that will crash the VR viewport.

**Controls:  **
Triggers are used for gestures and in clinch/stealth minigames or crossbow (LT+RT)/rifle shooting or stealth actions
Grips are for holding weapons and interacting with the pickable items (you can still pick them with A (or X on ps2vr), or hold grab them yourself and put them in your "chest pouch", or you can pull dead npc with grip or interact with smaller static objects
you can open doors with the your hand too 
holding grip and trigger - fists for hand-to-hand combat, R3 for kicks
swings and stabs works for the weapons, blocks are holding your weapons or hands in front of your face roughly parallel to it (it is quite easy to figure out, but you can make extensive options for the speed and size of "block box"), you can also block by parrying the incoming enemy blows with your swings (very cool) or dodge with your HMD
you can pretty much hold any weapon with two hands for enhanced stability and power, holding right trigger activates "power attack" or feints
left joystick is for movement, right joystick for turning (snap turning is as option in immersive and controls menu)

**virtual holsters **

grab the weapon with the grip, holding grip is necessary, releasing grip returns the weapon to the holster)

visible holsters are working (left hip for swords, right hip for axes, maces) for right hand, just grab the handle and pull

right shoulder for bow/crossbow/rifle if you grab it with the left hand over the right shoulder

behind your back on the right side (behind your arm) for two handed weapons, grab with the right hand

left shoulder for shield, grab with the left hand over the left shoulder

behind left back for the torch (using left hand and left trigger, you can put it back with holding the trigger)

helmet visor up and down by holding left trigger and moving your left hand up and down near your left temple

Right thumb rest: modifier for D-pad actions 

Right thumb rest + Left Stick Up: hold it for 0.5s for immersive parchment map (B closes)

Right thumb rest + Left Stick Left: select weapons (but you have holsters too)

Right thumb rest + Left Stick Right: select quick-consumable slot and A to drink it

Right thumb rest + Left Stick Down: hold it longer for the VR Menu (open, close)

X: for quick talk (hold X for torch or in steamVR for vrmenu)
Y: Call horse; hold Y = Call dog (or dog interactions), otherwise you can command your dog with gestures
B: in menus for interactions or long press for "exit" menu
A: Use / talk / pick up (or use grip)

Right Menu/Oculus button: Recenter

Left Menu button: Inventory and other menus (character, ...) (in steamVR it is holding X)

R3 kick

L3 is sprint

right joystick up Jump, down crouch
right thumb rest + left menu button - pause menu (or holding B for 2s)
Emergency controls F6 for switch between theater and immersive mode and F7 (keyboard) for VR menu 
If you want to use the native non VRIK controls, (emergency option or if tutorials need native inputs for combos or.. whatever)
NATIVE INPUT CONTROLLS (NON VRIK): are similar to gamepad, right grip is free look, left grip is for D-pad simulation, right trigger for attacks, left trigger for the blocks, in combat right stick is for directions... like in native game.  

**Requirements**

VR Headset
optional: https://www.nexusmods.com/kingdomcomedeliverance2/mods/34?tab=files

**Future**
throwing daggers and weapons, rifle reload, fully immersive verbatim combos and masterstrikes (need animation artist or learn it myself - now there is still a disconnect feeling between your blade and "paired" animations), working live NPC grabbing and reactions and immersive clinches and a lot of bugfixing and optimization
KCD1VR mod

**Known issues**
VRIK is not ideal, VRIK combat and weapon handling are nearly there for me, but it is too much for one person to test it all. The plan is to integrate full working verbatim combos to the game (like pommel strike or knee strike as in game) - but you have extensive controls in developer mode - maybe come up with better ones, you can post them as you like - the main reason for the restrictions in actual combat is to not spawn the npc's with 1000 cuts, so to honor the kcd2 rpg systems
hands can feel sometimes choppy (openxr synchronization, didn't find the best working solution for it, yet), some functions can block the other, the doors are sometimes... not working..., sometimes you need to recalibrate the view to have interaction "ray" central
HUD is ... not ideal
dice minigame is also not ideal
switch between inventory/shop and immersive is not immediate
collisions are not great
rifles reload is not finished (but holding LT reloads them and LT+RT shoots... you can point them with your hands)
See the GitHub README's Known Issues section for the current list — it's kept more up to date there than here, there are extensive documentations and source builds, which will be regularly updated.

**Reporting bugs**
Please post issues on Github rather than the Nexus comments if you can, it's much easier to track. If something crashes or does not work persistently, look in the log file before you post (you need to enable diagnostics or developer mode in the .ini). 
It will be buggy now, probably, I only tried it on my PC and oculus via virtual desktop, steam and playstation VR... 
But give me like 1-2 weeks first, I need a break right now from this all. 
A note on where this project is headed
I've taken this about as far as I currently can on my own with the help of claude and chatgpt and some friends. What's here works and is genuinely playable. If you can help: the GitHub repo has the history and docs, Come find me, or just fork it, I need every help I can get. 
I am confident, that porting it to kcd1 would be relative easy, than starting from scratch, but didn't yet tested it. 
Credits
A personal note: I'm only a hobby C++ programmer and English isn't my first language — this project only exists because of inspiration from amazing flat2vr modders like praydog and others for initial building blocks, and this community where I took inspirations from some of the mods on this site (central HUD for how to move the HUD elements for initial work, minimap lua mod how to make POI's show for the vrmap), and from PLANCK/HIGGS and FRIK. For the haters, without the AI help, it would take me years to complete or I will need a whole team of people. 
Thanks to:
Quang Trinh and his KCD2Tools — modding groundwork this project builds on (the start was the camera)
libKCD2 on github for lot of static investigation
Warhorse Studios — for Kingdom Come: Deliverance II itself. Thank you for making the best game I ever played and something worth doing this for.
And my beautiful wife for her enormous patience with me sinking all of my free time to this mod <3


## Building from source

This repository is source-only. **vcpkg is no longer required** — CMake fetches the OpenXR SDK itself if it isn't already installed. The one external package you still need to provide is **DetourModKit** (prebuilt).

### Prerequisites

- **Visual Studio 2022** (Community edition is fine) with the **"Desktop development with C++"** workload installed. This gives you the **x64 Native Tools Command Prompt for VS 2022** — build from inside that prompt, not a plain `cmd`/PowerShell window, so the MSVC toolchain is on your `PATH` correctly.
- **CMake** 3.20 or newer (the Visual Studio installer can install this for you as an optional component, or grab it separately from [cmake.org](https://cmake.org/download/)).
- **Git**, for cloning this repo (and for CMake's own fetch step).
- **[DetourModKit](https://github.com/)** — grab the matching prebuilt package from its Releases page and unzip it somewhere on disk. You'll point `CMAKE_PREFIX_PATH` at that folder below.

### Build

Open the **x64 Native Tools Command Prompt for VS 2022** (Start Menu → "x64 Native Tools Command Prompt for VS 2022"), then:

```bat
cd path\to\KCD2VR

rmdir /s /q build

cmake -B build -S . -DCMAKE_PREFIX_PATH=C:\path\to\DetourModKit

cmake --build build --config RelWithDebInfo
```

- Replace `C:\path\to\DetourModKit` with wherever you extracted the DetourModKit package.
- The first `cmake -B build -S .` step is also where CMake fetches OpenXR automatically if it can't find an existing install — this needs an internet connection the first time.
- `rmdir /s /q build` at the start isn't required for a first build, but it's the reliable way to force a completely clean reconfigure if you're rebuilding after changing dependencies or CMake options — safe to skip if `build/` doesn't exist yet.
- Output lands under `build/` — copy `KCD2VR.asi` from there into your game folder alongside `KCD2VR.ini`, `dinput8.dll`, `openxr.dll`, and `MIS_MENU_BOOK.bmp` as described in [Installation](#installation-playing-the-released-build).

For a debug build with fuller diagnostic logging, use `--config Debug` instead of `--config RelWithDebInfo` in the last step — the release default keeps logging minimal for performance.


Please post issues on **GitHub** rather than the Nexus comments if you can — it's much easier to track. If something crashes, grab the log files first (see this repo's issue template / Known Issues for exactly which ones) before you post.

If that's you: this repo has the full history, including dead ends already ruled
out with real evidence, so you won't have to rediscover them the hard way. Come find me,
or just fork it — I need all the help I can get.

I'm fairly confident that porting this to KCD1 would be far easier than starting from
scratch, if anyone's up for it.

## Support

If you want to support development, or just say thanks: [Buy Me a Coffee](https://buymeacoffee.com/nicolas125)

## Credits

A personal note: I'm not a professional programmer, and English isn't my first language
— this project only exists because ChatGPT and Claude AI helped me build it and debug it.
I've poured about two weeks of work, testing, and ideas into it. It's genuinely amazing
what one person can achieve now with these tools, even with fairly limited coding
knowledge going in.

Thanks to:

- **Quang Trinh** and his [KCD2Tools](https://github.com/) — modding groundwork this project builds on.
- **thazer** — for his Dynamic HUD script, which made it possible to manipulate the HUD.
- **Warhorse Studios** — for Kingdom Come: Deliverance II itself. Thank you for making the best game I've ever played, and something worth doing all of this for.
- And my beautiful wife, for her patience.


The requested defaults reveal health/stamina/status on an open right-palm glance
and the compass on an open left-palm glance. Weapon/potion selectors, the
crosshair, ordinary interaction hints and spoken subtitle text start hidden.
Dialogue response choices, native menu controls and transient event notices stay
available. This is hand-triggered reveal of native HUD elements on the existing
reading plane; it is not a new wrist-mounted rendering system.

Extract this complete package into a new source directory and run
`build_release.bat` on Windows. No earlier patch overlay is required. This package
contains source, tests and build inputs; a Windows DLL has not been built here,
and the changes have not been tested in the game or a headset. Build the matching
main companion PAK with the binary. Keep your installed INI and calibration.

A one-time migration changes only the eight HUD preferences requested for this
repair. It records completion after all writes succeed, and later saved choices
remain yours. Physics, combat, body calibration, head-follow timing and reload
settings are preserved.

See [the R105 repair review](docs/R105_REPAIR_REVIEW.md) and
[validation evidence](docs/R105_VALIDATION.json) for exact scope, reproducible
checks, old-log provenance and the remaining visual/runtime checks. Historical
R96/R96R1 and earlier reviews describe their own revisions and are not new R105
headset evidence. The historical tests omitted from the supplied R104 archive
were restored without changing current production files; their provenance is in
[R105_RESTORED_TESTS.json](docs/R105_RESTORED_TESTS.json).

The default hand timing remains the accepted fresh-pose mode. Fresh Pose
Uncapped is available as a separate saved choice below. The supplied INI is used
only when no installed INI exists; the small R105 HUD migration also applies to
existing installations. Optional gore remains a separate package.

### The new timing choice

Open **Home > VRIK EXPERIMENTS > Fresh Pose Uncapped**. The same choice is on
**Home > EXPERIMENTS / LIVE SETTINGS > VRIK+ > Motion timing**. These retain the
existing developer-mode visibility. Selecting it applies it live and saves
`VrMotionTrialD67=4`; returning to **FRESH POSE timing** restores the existing
paced behavior. The saved/missing-key default remains 2.

Uncapped skips the game-thread XR-token waits. Once per native frame, before
the ordinary update, it attempts a fresh hand family using try-locks. Predicted
head modes use a bounded, coherent runtime prediction reference; current-time
mode uses the current runtime time. The head in this family is only the hands'
coordinate reference. It does not replace the camera head or its publication.
Failed attempts retain the completed pose and its actual age. This is an added
tracking sample at an existing supported boundary, not a new GPU palette writer
or a promise of a particular FPS. Manual caps, runtime cadence and GPU cost are
separate. F2/F7 can record the new route without re-enabling its token wait.

The `.cmd` opens the setup export in the installed mod binary. **Native (no
upscaling) + Save & start** explicitly writes `r_SuperResolution_Mode = 0`;
the save now verifies the staged and final file before reporting success.
**Keep settings & start** retains its existing discard-edits behavior.

The retained R1 changes are:

- One anatomical elbow destination for ordinary and bow arms, with fixed arm
  lengths and the admitted wrist preserved. Bow admission retains its shoulder
  basis and valid bend history. This addresses implementation defects instead
  of asking for another elbow-settings sweep.
- Separate dice/activity reference lifetimes, preventing an activity pose from
  becoming the ordinary body reference. Leaning during dice remains available.
- Less exaggerated lag and critically damped recoil for supported polearms.
- A completed-eye capture transaction for temporal upscalers, recoverable DLSS
  reset mismatches, and live-switch success based on a new stereo publication.
- Narrow alchemy acceleration: Henry's qualified pickup/return and pre-work
  turn can advance quickly; book/station changes and distillation keep native
  timing. The actual work/effect boundaries remain owned by real gestures.
- Pickup preparation installed before the first eligible Lua Use, with bounded
  F5 reasons for refused preparation. Existing rigid-body and inventory checks
  remain in place.
- A current-pose observer repair for crossbow string/tool geometry, an exact
  native no-opponent kick-query correction, and cooldown only for an observed
  kick request. Complete live reloads and all kick contexts remain unverified.
- Open, face-facing, looked-at palm HUD with a three-second maximum and twice
  the tested station prompt size.

The R2 corrections are:

- Corpse search receipts describe background work, not attempted grabs. Idle
  service work is throttled before native/script setup, distant current body
  bounds can avoid joint scans, and closely timed nearby hands can share a
  bounded discovery list. Remaining full entity traversals and unmeasured game
  frame cost are explicit; this is not a proven drag or FPS result.
- Ordinary inventory pickup respects a qualified requesting hand and prepares
  eligible static inventory items in both existing hold modes. Scenery props
  without native selection/ownership remain a separate unfinished feature.
- The post-dice report concerns a frozen old image after a 0.95 presentation
  zoom edit. That scalar is distinct from 95-degree native FOV. Gated receipts
  now join actual zoom application with publication/fault observations; the R1
  stereo-publication and activity-reference repairs address separate defects.
- Existing crossbow recordings are reviewed through native clock, completed
  pose and mechanism ownership. Finished inert duplicate clips, the UpperBody
  source route and same-clock newer-wrist admission receive bounded repairs.
  Unqualified native event intervals are not bypassed to manufacture success.
  Existing F5/F7/F11 captures now retain exact clock/interval refusal evidence
  with finite per-session budgets and no extra native traversal.
- F7 now records the elbow reference/conversion inputs and exact final-arm
  observation identity. No old elbow preset is treated as correct, and smoother
  numerical output is not called proven natural anatomy.
- The one-time movement-option override is removed. Exit-saved INI values do
  not establish earlier live state, and changing a flag is not a 6DOF repair.

The supplied starter INI preserves the user's uploaded exit profile. Only
station prompt scale (0.45 to 0.90) and the existing R1 migration marker differ
in setting values. Movement flags and calibration are preserved. The normal
release build preserves an existing game INI. The one-time migration applies
only the requested prompt size; later saved prompt changes remain effective.

## Build and integration

Use the complete `KCD2VR_SOURCE` directory from the R105 archive. Build in that
source directory, separate from the game installation. Historical patch and
revert instructions in earlier review notes do not apply to this full archive.

On Windows, install Visual Studio 2022's C++ desktop tools, CMake and Python 3,
then run:

```bat
build_release.bat
```

The output goes to `dist/prerelease`; building does not install it. Use the DLL
and its matching main companion together. The build regenerates that companion
from the verified gameplay donor plus current source overlays. The donor PAK's
older scripts are intentional; their age alone does not identify the installed
companion or prove a packaging failure. The optional gore package stays separate.

The source archive contains no newly compiled DLL/ASI, raw recordings or optional
gore payload. Its required gameplay donor PAK is a build input, and current
source overlays produce the main companion during the build. See [building](docs/BUILDING.md) and
[dependencies](docs/DEPENDENCIES.md) for the normal build requirements.

## Current source: OpenXR recovery and native spanner grip

R105 continues from the supplied R104 source. It repairs stale controller input,
OpenXR frame/session recovery, the weapon/potion selector visibility bypass and
the medium crossbow's animated spanner grip capture. Ordinary HUD content uses a
farther reading plane with a more central binocular safe area

## Settings and diagnostics

Read [settings](docs/SETTINGS.md), [diagnostics](docs/DIAGNOSTICS.md) and
[known issues](docs/KNOWN_ISSUES.md). Gameplay does not require a diagnostic key.
F2 records bugs/rendering, F3 combat, F5 interactions and minigames, Shift+F7 tracked or
native pose behavior, and F9 physics. F7 opens/closes the VR menu, F6 switches between
Theater (flat screen) and Immersive until you press it again or the next menu/scene change. Existing finite capture sessions can run
together. F11 remains available for the older dedicated reload capture.

Diagnostic receipts use the existing `KCD2VR.log`. Runtime identity, R105
frame/session recovery and the first head pose after startup or a reference
change are visible without an F2 capture; detailed feature captures keep their
existing gates. No fingers, map-performance, companion-startup or hook-library
sidecar is introduced. The existing last-good INI checkpoint is a settings
recovery file.

K
