# Building on Windows

you need Visual Studio 2022 with the C++ desktop workload (x64 tools), CMake and Python 3. CMake from the Visual
Studio install is fine. the scripts don't install compilers or python packages for you.

from the source root:

```bat
build_release.bat
```

the output goes to `dist/prerelease`: the DLL (dinput8.dll), `Start KCD2VR.cmd`, `VERIFY_BUILD.cmd`,
`Mods/KCD2VR/Data/KCD2VR.pak` and BUILD_MANIFEST.json (hashes). nothing is copied into the game, you install it
yourself. build logs and intermediate files stay in the build folder, not in dist.
if the build fails it says so and the details are in `build_release.log`.

the GitHub "Download ZIP" of the repo works, extract it and run build_release.bat. no git needed.

## build folder

by default it builds in `build-prerelease` next to the source. if the source path is very deep the script picks a
short build path on the same drive by itself (the MSVC tools have path length limits). you can also give one:

```bat
build_release.bat -BuildDirectory E:\kcd2vr-build\local
```

an existing build cache is reused, delete the folder if you want a clean build.

## dependencies

everything needed is in the tree:
 - DetourModKit (hook library) is bundled under `deps/DetourModKit`. the static lib is stored as a zip
   (`lib/libDetourModKit.lib.zip`) so there is no single huge file in the repo. the build unpacks it into the build
   folder and checks the hash. don't replace it with a similarly named lib from an older build. a direct CMake override
   uses `DetourModKit_DIR`, a generic `CMAKE_PREFIX_PATH` does not override the pinned one.
 - the FidelityFX API headers are in `deps/FidelityFX_API_1_1_3`.
 - OpenXR: CMake downloads the pinned OpenXR-SDK-Source release on the first configure (needs internet once). if you
   already have a checkout:

```bat
build_release.bat -OpenXrSdkSource E:\deps\OpenXR-SDK-Source
```

python is found on PATH (not the WindowsApps stub). to pick one: set `KCD2VR_PYTHON` to a real python.exe.

vcpkg is not used anymore.

## companion and optional gore

the main companion (KCD2VR.pak) is built from the gameplay base pak in the tree plus the current source overlays
(`companion/source`). the base pak has older scripts inside, that is normal, the overlays replace them.

the gore pak is optional and separate, it is never merged into the main pak. if you have the gore zip:

```bat
build_release.bat -OptionalGoreSource E:\downloads\KCD2VR_OPTIONAL_GORE.zip
```

direct CMake uses `KCD2VR_OPTIONAL_GORE_SOURCE` for the same thing. use a clean output folder when you switch between
with and without gore, an unexpected old gore folder is reported and not silently included or deleted.

## install

copy dinput8.dll and `Start KCD2VR.cmd` to `Bin\Win64MasterMasterSteamPGO\` of the game and the `Mods\KCD2VR`
folder into the game's `Mods\`. always install the DLL and the pak from the same build. on a new install there is no
KCD2VR.ini yet, the mod writes the default one at the first start and the setup window opens (LAUNCHER_OPTIONS.md).
the KCD2VR.ini in the source root is my profile with every key commented, you can copy it next to the dll instead.
on an update keep your KCD2VR.ini, it has your settings and calibration.
the first lines of KCD2VR.log print the build marker, check it if you are not sure which build runs.

`VERIFY_BUILD.cmd` (in dist) checks the built payload against BUILD_MANIFEST.json.

## tests

see TESTING.md. host tests are not a DLL build, and a DLL build is not a headset test.

## source archive

to make a clean source zip (no .git, no build folders, no object files, no optional gore):

```bat
python tools/export_source.py --zip KCD2VR_SOURCE.zip
```
