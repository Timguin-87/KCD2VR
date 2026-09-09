# KCD2VR

VR support for Kingdom Come: Deliverance II via OpenXR — full 6DoF head tracking, native
Touch controller bindings for movement and combat, a proper in-headset menu, and a tuned
camera/FOV pipeline with either Mono or Pseudo-Depth-Stereo presentation.

This is still an early build, made by one person with a full-time job, in his free time —
"with a little help of my friends" (and AI).

## Features

- Full positional + rotational head tracking (6DoF)
- Native Touch controller bindings — movement, camera, combat, interaction (see [Controls](#controls) below). VRIK is in the works.
- Dedicated in-headset VR menu — no need to pull off the headset to change settings
- Tuned mono-world presentation: culling fix, FOV correlation, camera dolly (builds on KCD2Tools' shoulders — thank you)
- Dialogue camera handling tuned for VR (native or forced first-person)
- VR menu covering all the important settings, optimization, and tuning
- Pseudo-depth-stereo (sadly not true native stereo) using camera tricks — IPD adjustment and distance-based parallax correction, with extensive customization

## Installation (playing the released build)

1. Download the latest release from the [Releases](../../releases) page (or the Files tab on Nexus).
2. Extract `KCD2VR.asi`, `KCD2VR.ini`, `dinput8.dll`, `openxr.dll`, and `MIS_MENU_BOOK.bmp` so they sit next to `WHGame.dll` and the game's `.exe`:
   - **Steam:** `.../Kingdom Come Deliverance II/Bin/Win64MasterMasterSteamPGO/`
   - **GOG:** `.../Kingdom Come Deliverance II/Bin/Win64MasterMaster`
3. For a custom resolution, use the supplied `user.cfg` — it goes in the game's **root** folder (next to `system.cfg`), and you need to launch with `-devmode +exec user.cfg` added to your shortcut (or Steam → Properties → Launch Options).
4. Put your headset on, launch the game normally, and control everything with your Touch controllers.

### Best-practice settings

- **Resolution:** custom square or nearly-square (or portrait) resolutions look noticeably better in-headset than stretching a 16:9 image onto a near-square VR view — you lose a lot of pixels that way. Best results so far: **1920×1800** on the lower end; try higher if your system can take it. Tested at medium settings on an RTX 4080 with acceptable performance. Turning off motion blur helps; DOF is optional (it still looks fine with it on).
- **FPS cap:** the game doesn't lock to a true 90 FPS — it tends to hover around 94–96, which causes choppy head-turning. Capping around **84–85** looks great instead, though the right number varies by system. There's also an optional head-motion sync setting that can smooth this further, with some trade-offs — see the VR menu.

### ⚠️ Important: don't use the in-game graphics menu to change settings

Changing **resolution** in the in-game graphics menu will crash the VR view — clicking Apply on *any* setting in that menu forces a resolution reset, which is what actually breaks it. Advanced graphics settings and everything else in that menu are fine to change. Use `user.cfg` for resolution, and the in-headset **VR menu** for things like gamma and brightness instead.

## Controls

*(Oculus Touch controllers — not tested with any others)*

| Input | Action |
|---|---|
| Left Trigger | Block |
| Left Grip | Modifier (hold for combos below) |
| Left Grip + Left Stick Up | Torch |
| Left Grip + Left Stick Down | Helmet visor |
| Left Grip + Right Stick Left | Select left quick-consumable slot; keep holding → Henry drinks/uses it |
| Left Grip + Right Stick Right | Select right quick-consumable slot; keep holding → Henry drinks/uses it |
| X | VR MIS menu |
| Y (short / hold) | Call horse / Call dog |
| B (short / long) | Weapon swap / Sheath |
| A | Use / talk / pick up |
| Right Trigger | Attack |
| Right Grip + Right Stick | Theater-view zoom/framing, free look, and bow aiming |
| Right Menu/Oculus button | Recenter |
| Left Menu (short / long) | Inventory / Pause menu (or Back) |

**Combat:** holding Right Grip + Right Stick controls sword placement; the stick also controls view, and an extreme flick switches targets. Left Trigger blocks. Movement/strafing is the left stick.

## Requirements

- A VR headset
- Optional: [Letterbox remover](https://www.nexusmods.com/kingdomcomedeliverance/mods/1632)

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

## Future

- VRIK and native stereo — see [A note on where this project is headed](#a-note-on-where-this-project-is-headed) below
- KCD1VR

## Known issues

*(kept more up to date on this repo than on Nexus — check open issues too)*

- Snap-turn / slow-turn on the right stick takes some getting used to.
- VRIK is only rudimentary right now and doesn't cover all actions — but combined with the game's own animations, it mostly makes sense for the time being.
- VRIK combat and weapon handling need real work and a lot of debugging — more than one person can realistically get through alone.
- Herb picking, meal picking, and some other interactions aren't ideal yet.
- Changing graphics settings from the main menu/in-game will crash the VR view — use `user.cfg` for resolution, and the VR menu for things like brightness/gamma (see the warning under Installation above).

## Reporting bugs

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
