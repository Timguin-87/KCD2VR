**Description**
**KCD2VR - Medieval Immersion Simulator ** adds OpenXR virtual reality support to Kingdom Come: Deliverance II. It features full 6DoF head tracking, full VRIK system, full combat system rework, separate options for minigames, comfort options, controller bindings for movement and combat, a dedicated in-headset VR menu. I wanted to make the best game I ever played fully playable in VR, that it feels like a native VR game as much as possible, but with working RPG systems and progression. 

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
Best practice: 
Visual Settings: Turn Motion Blur OFF. Depth of Field (DOF) OFF. Most of the settings are already in user.cfg and in Optimized VR Settings
choose "non developer" mode in .cmd (easier VR menu) or developer mode (lot of options)
you can configure most things in the VR menu (hold right thumb rest + left joystick down to open and close it), left and right grip for VR laser pointer
IMPORTANT WARNING: Do NOT change resolutions or apply changes inside the standard in-game Graphics menu—doing so forces a display mode reset that will crash the VR viewport.
Controls:  
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

****Requirements
VR Headset
optional: https://www.nexusmods.com/kingdomcomedeliverance2/mods/34?tab=files

**Future**
throwing daggers and weapons, rifle reload, fully immersive verbatim combos and masterstrikes (need animation artist or learn it myself - now there is still a disconnect feeling between your blade and "paired" animations), working live NPC grabbing and reactions and immersive clinches and a lot of bugfixing and optimization
KCD1VR mod
Known issues
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

## A note on where this project is headed

I've taken this about as far as I currently can on my own. What's here works and is
genuinely playable, but the two big open problems — real native stereo and VRIK — need
someone with real reverse-engineering chops and more time than I have. I'm not walking
away from this for now, and I'll keep trying to maintain what's shipped and keep
contributing where I can. But I'd rather be honest that this project would benefit
enormously from more capable modders picking up where the research notes leave off than
sit on it and let it stall. I ran a lot of tests and discarded a lot of old theories along
the way — maybe KCD1 has some dormant native stereo support in there, but in KCD2 I
couldn't find anything that actually works.

If that's you: this repo has the full history, including a lot of dead ends already ruled
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

## License

*(pick one before you publish — MIT is the simplest default if you don't have a strong preference)*
