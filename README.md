# KCD2VR - Medieval Immersion Simulator

OpenXR virtual reality mod for Kingdom Come: Deliverance II.
Nexus: https://www.nexusmods.com/kingdomcomedeliverance2/mods/3607

Full 6DoF head tracking, VRIK body, a reworked physical combat system, hand handled minigames, comfort options, controller bindings for movement and combat and a dedicated in-headset VR menu. I wanted to make the best game I ever played fully playable in VR, so it feels like a native VR game as much as possible, but with the RPG systems and progression still working. The game still decides combat results, inventory, recipes and what you are allowed to interact with - the VR side gives it your real movements.

Note: this is a beta, made by one passion-driven hobby developer in his free time over the last ~12 months, very slowly at first, the last 3 months a lot faster with the help of (expensive) AI. Don't expect perfection, I am shoehorning gestures and VRIK into an animation based game with a lot of buttons and a complex combat system. It's genuinely playable, but it's a very long game and i can't test every quest.

## Features

- full 6DoF and VRIK for every melee weapon, blocking with your weapon or hands, parrying with your swings, dodging with your head, combos and masterstrikes (optional gore mod with decapitations)
- two handed grip for any weapon, right trigger for power attacks / feints, kicks on R3 (with every melee weapon), fist fighting (grip + trigger)
- impaling enemies on the blade (only plate armour stops it)
- virtual holsters (see below), hand reloads for bows and crossbows, physical bow aiming
- weapon skill and RPG stats decide your weapon speed ("gravity drag"), enemy skill decides how fast they block, parry and dodge
- free targeting: the game's lock-on stays underneath, but the view doesn't get yanked around (3 modes)
- 6DoF crafting minigames: smithing with real hammer strokes (optional hold LT to "stabilize the iron on the anvil"), alchemy by hand
- small objects you can pick up, carry, throw and drop (with impact sounds), pull dead bodies with your grip, open doors with your hand
- in-headset VR menu, normal or developer mode (a LOT of options)
- true native stereo (DLSS / FSR / XeSS work in stereo), mono option for lower specs (the game is very demanding)
- dialogue camera tuned for VR (native or forced 1st person with VRIK)
- "Theater mode" (flat screen) for inventory, character sheet, cutscenes and cinematic dialogues - detected automatically
- immersive HUD: look at your open right palm for health, left palm for compass. Minimal HUD option (only prompts, dialogue, subtitles and the palm glance), tutorial messages on/off, crafting UI size slider
- held parchment map (right thumb rest + left stick up for 0.5 s)
- gestures: lower/raise the helmet visor with the left trigger near your left temple, point at your dog or an NPC with your index finger for quick talk (or taunt in combat), point at objects to select them
- pet your dog with your hand (if the game lets you)
- horse riding with VRIK, horse combat, X tap = Follow while riding
- controller layouts for OpenXR native (Quest via Virtual Desktop / Oculus), SteamVR and PS VR2
- fallbacks if something is not working: F6 switches theater / immersive, F7 opens the VR menu, hold B for 2 s to escape a menu, and you can switch from VRIK to native controls anytime (for tutorials or quick time events, if something goes wrong)

## Installation

1. download the latest version from the Files tab (Nexus) or the GitHub Releases page
2. copy `dinput8.dll` into the game's binary folder, next to `WHGame.dll` and the game's .exe (replace an existing dinput8.dll if you have one). you may need to "unblock" the dll (right click - properties - unblock)
   - Steam: `.../KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/`
   - GOG: `.../KingdomComeDeliverance2/Bin/Win64MasterMaster/`
3. copy the `KCD2VR` folder (with `Data/KCD2VR.pak`) into the game's `Mods` folder
4. optional: the Gore Mod (optional files) also goes into `Mods`
5. optional: no letterbox mod https://www.nexusmods.com/kingdomcomedeliverance2/mods/34?tab=files into `Mods`

That's it. no launch options are needed (no `-devmode`, no `+exec user.cfg`).

**first start:** start the game normally with your headset connected. the KCD2VR Setup window opens on the desktop (Alt+Tab or lift the headset if the game covers it): choose your headset, resolution, upscaler, graphics preset, controller layout and normal or developer mode, then Save. the settings are used from the next start. the mod writes `KCD2VR.ini` and the game's `user.cfg` for you. to see the setup again, set `first_start=1` in KCD2VR.ini (or run `Start KCD2VR.cmd` if you have it).

VRIK starts by itself with default arms. for exact arms do the T-pose calibration once (F8, or the VRIK button in the VR menu) - it's saved.

want the setup window again any time without touching the ini? make a `config.bat` next to dinput8.dll with:

```bat
@echo off
"%SystemRoot%\System32\rundll32.exe" "%~dp0dinput8.dll",KCD2VRSetupW
```

**IMPORTANT:** do NOT change resolution or apply changes in the game's own graphics menu - the display mode reset crashes the VR view. use the setup window instead.

## Best practice

- motion blur and depth of field OFF (already in the user.cfg the setup writes)
- fullscreen mode, so the VR laser tracks properly
- if fights feel slow, use the Low graphics preset or DLSS Balanced - the game is the heavy part, the mod costs ~1 ms per frame
- most things can be changed in the VR menu: right thumb rest + left stick down (hold), or F7. left / right grip = laser pointer in menus
- the normal (non developer) VR menu is much easier, developer mode is for fine tuning

## Controls

- **grips**: hold weapons and items, pick up small things (or A), put things in your chest pouch, pull dead bodies, laser pointer in menus
- **grip + trigger**: fist (hand to hand combat)
- **triggers**: gestures, clinch / stealth minigames, crossbow (LT + RT), stealth takedowns (open left hand + LT)
- swings and stabs work as you do them. block by holding your weapon or hands in front of your face (block box size and speed are in the options), parry incoming blows with a swing, dodge with your head
- right trigger while swinging = power attack / feint
- left stick move, right stick turn (snap turn is an option), right stick up jump, down crouch, L3 sprint, R3 kick
- **A**: use / talk / pick up
- **B**: back in menus, hold 2 s to escape any menu
- **X**: quick talk, hold X = torch (SteamVR / PS VR2 layout: hold X = inventory). while riding: X tap = Follow
- **Y**: call horse, hold Y = dog (or command the dog with gestures)
- **right thumb rest** = modifier (on PS VR2: finger resting on O or triangle):
  - + left stick up (hold 0.5 s): parchment map (B closes)
  - + left stick left: weapon slots
  - + left stick right: quick consumable (A to drink)
  - + left stick down (hold, not click): VR menu
  - + hold X: wait menu
  - + left menu button: pause menu
- **left menu button**: inventory and the other menus (SteamVR layout: hold X, the left menu button opens the SteamVR overlay)
- **right menu / Oculus button**: recenter
- **keyboard**: F6 theater / immersive, F7 VR menu, F8 VRIK calibration

**native (non VRIK) controls** as an emergency option (or if a tutorial wants the native inputs): like a gamepad - right grip free look, left grip D-pad, right trigger attack, left trigger block, right stick attack directions.

## Virtual holsters

hold grip to hold the weapon, release grip to put it back.

- left hip: swords, right hip: axes and maces (right hand, grab the handle)
- right shoulder: bow / crossbow (left hand over the right shoulder)
- behind your back on the right: two handed weapons (right hand)
- left shoulder: shield (left hand over the left shoulder)
- behind left back: torch (left hand + left trigger, hold the trigger to put it back)
- bow: pull the arrow from the quiver, nock it, pull the string with right grip, release to shoot
- crossbow: pull the string with right grip (light, medium and heavy by hand), take a bolt, put it in the slot, LT + RT shoots

## Requirements

- a PC VR headset (tested: Quest 3 via Virtual Desktop and SteamVR, PS VR2 via SteamVR)
- a strong PC - the game is very demanding in stereo

## Compatibility

unknown... probably not the mods that change HUD positions or combat, otherwise other mods should be compatible - modded swords and weapons show fine (tried some). Extended NPC Action Menu (carry living NPCs) interferes.

## Known issues

- VRIK and combat are nearly there, but it's too much for one person to test everything. paired animations (combos, masterstrikes) can still feel disconnected from your blade
- hands can sometimes feel a bit choppy
- some flicker with DLSS / FSR when you are under the refresh rate of your headset
- the HUD sometimes refreshes and forgets to be immersive
- some doors don't open by hand
- dice minigame is not ideal
- switching between inventory / shop and immersive is not instant
- collisions are not great
- rifle reload is not finished (hold LT reloads, LT + RT shoots, you can aim with your hands)
- sometimes you need to recenter or redo the T-pose after some actions, or recenter so the interaction ray is central again (pointing your index finger at something moves the ray to that hand)
- no shield bash and no torch combat (the game has no animations for it)

the GitHub issues are more up to date than this list.

## Reporting bugs

please post issues on GitHub rather than in the Nexus comments, it's much easier to track. send `KCD2VR.log` from the game's binary folder (it's overwritten at every start, so copy it first). for a detailed log turn on developer mode and press F2 (rendering / bugs), F3 (combat) or F5 (interactions) while it happens.

give me 1-2 weeks after the release, i need a break from all this.

## Building from source

source only, no binaries in the repo.

you need:
- Visual Studio 2022 with "Desktop development with C++"
- CMake 3.20+
- Python 3
- Git (or "Download ZIP" on GitHub, that works too)

then in the source folder run:

```bat
build_release.bat
```

DetourModKit (prebuilt, capacity128) and the other dependencies are bundled in `deps/`, nothing to download. the build also builds the companion `KCD2VR.pak`. output goes to `dist/prerelease` - building does not install anything, copy `dinput8.dll` and the `KCD2VR` mods folder yourself. `build_release.log` tells you what failed if it fails.

## Patch history

- **R148** - SteamVR: intro / main menu screen comes in front of you, save load no longer faces you backwards
- **R147** - added: X tap = Follow while riding (the game wanted LB)
- **R146** - fixed: enemy weapon skill now really sets their block / parry / dodge speed. fixed: wrong "quiver" message with the bow. less log work when nothing records
- **R145** - added: crafting UI size slider. fixed: station prompts and icons growing back to full size, yellow messages too big. the setup writes the shadow cascades into user.cfg
- **R144** - fixed: stripe of missing shadows in the right eye (cascade layout kept after preset changes)
- **R143** - fixed: block cooldown, dodge window, focus zoom, no-fade and the VR dialogue camera did not work without `-devmode`
- **R142** - fixed: free targeting not free without `-devmode`, FREE PLUS view hold, Henry close-up at dialogue start
- **R141** - added: Minimal HUD, tutorial messages switch. fixed: bow / crossbow first draw orientation, see-through lockpicking lock, black frames with DLSS / FSR
- **R139** - fixed: Henry lunging forward with every attack
- **R138** - fixed: punches turning into kicks after closing the left fist
- **R134-R136** - fixed: quest / tutorial weapons (wooden practice sword) not in the hand
- **R137** - fixed: the view ray didn't select objects in VRIK. added: point with the index finger to select
- **R133** - added: free targeting 3 modes. fixed: building from a GitHub ZIP
- **R132** - fixed: VRIK lost on the ride to Trosky, F8 calibration failing there, prologue cutscenes not shown as cutscenes
- **R130-R131** - added: first start setup window without the .cmd
- **R128** - added: impale fully, only plate stops it, body stays on the blade
- **R129** - fixed: PlayStation button icons
- **R125-R127** - fixed: stowed weapons on the back showing in swings, HUD convergence slider, combat cross hidden (icons kept)
- **R124** - added: F6 / F7 back, prop impact sounds, BLOCK message switch, my INI as the default
- **R118-R123** - added: controller layouts (OpenXR / SteamVR / PS VR2), default INI built in, shield block box, authored weapon grips. fixed: stealth LT, alchemy selection

older history and all the dead ends are in `docs/KCD2VR_HISTORY_AND_RESEARCH.md`.

## License

KCD2VR source code is MIT (see `LICENSE`). DetourModKit is MIT, the AMD FidelityFX SDK and the OpenXR headers have their own licenses (see `deps/`). Kingdom Come: Deliverance II and its game files (the edited HUD / table / Lua files in `companion/`) belong to Warhorse Studios and are only included so the mod works.

## Support

if you want to support the development or just say thanks: [Buy Me a Coffee](https://buymeacoffee.com/nicolas125)

## Credits

a personal note: i'm only a hobby C++ programmer and English isn't my first language. this project only exists because of the inspiration from amazing flat2vr modders like praydog, PLANCK / HIGGS and FRIK, the mods on Nexus i learned from (central HUD for moving the HUD elements, the minimap lua mod for the map POIs) and the help of Claude and ChatGPT. for the haters: without the AI it would take me years, or a whole team. I never used any code or assets from other mods, only Warhorse Studios assets from the game.

thanks to:

- **Quang Trinh** and his KCD2Tools - the modding groundwork this builds on (the start was the camera). his DetourModKit is in `deps/`
- **libKCD2** on GitHub for a lot of static investigation
- **thazer** for his Dynamic HUD script, which made moving the HUD possible
- **Warhorse Studios** for Kingdom Come: Deliverance II. thank you for making the best game i ever played and something worth doing this for
- and my beautiful wife for her enormous patience with me sinking all my free time into this mod <3

if you want to help: the repo has the full history and the dead ends already ruled out with evidence, so you don't have to find them again. come find me, or just fork it - i need all the help i can get. i'm fairly sure porting this to KCD1 would be much easier than starting from scratch.
