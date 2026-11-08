# Tests

use a fresh output folder. tests that extract production functions must read this source tree, not a fixture left
next to an older build.

what the tests are: host fixtures that run the real mod code (or production functions extracted by anchor comments)
with controlled fake engine calls. they catch bad state transitions and wiring. they do NOT prove the picture in the
headset, GPU contents, hooks in the real game or native combat results. the headset is the real test - one key, one
action, the row to grep (DIAGNOSTICS.md).

a test that can't fail on the old code proves nothing. most newer checks take `--source <older tree>` as a negative
control and must fail there.

## focused portable tests

a small current set without configuring every old fixture:

```bash
cmake -S tests -B ../d84-tests -DKCD2VR_D84_FOCUSED_ONLY=ON -DCMAKE_BUILD_TYPE=Release
cmake --build ../d84-tests --parallel 2
ctest --test-dir ../d84-tests --output-on-failure
```

assertions stay on in Release. native/GFx/OS leaves are fixtures, the targets are not the Windows mod. checks against
the real game bytes need your own retail WHGame.dll locally, no game bytes are in the tree.

combat zones: `zone_d85` (header rules against the recipe fixture and real log zone pairs) and `zone_d85_production`
(`tools/test_zone_d85.py`, the direct-context block and both strike-receipt zone expressions compiled against recorder
leaves). run the script with `--source <older tree>` as the negative control.

kicks: `python tools/test_kick_route_d85.py --out <dir>` runs the no-target route check and the R3 outcome funnel (2 s
cooldown, one reason line). with `--source <older tree>` it must fail.

```bat
python tools/run_native_kick_free_d85.py --out <dir> --dll <WHGame.dll> --tables <Data/Tables.pak> --pak <Mods/KCD2VR/Data/KCD2VR.pak>
```

runs the retail kick selector bytes on the built rows for every loadout without a target. needs companion/source in
the tree. it does not prove the animation or the hit.

## Windows checks

Python 3 and the Visual Studio x64 C++ tools. AddressSanitizer runs also need the MSVC sanitizer component. the real
native-attach cleanup test uses Windows structured exceptions and the Windows SDK - a portable try/catch replacement
would miss the bug it tests.

map / interaction suite:

```bat
python tools/run_m41_checks.py --out C:\temp\kcd2vr-checks --pak PATH_TO_CURRENT_MAIN_PAK
```

its Lua tests need `lupa.lua51`, `--lua-path` for an existing package folder. it also takes `--source-root` and
`--vcvars`, it never installs packages. use the main pak from the same build. without `--pak` it looks in the current
build/dist folders. `--control-no-model-gate` asks a deliberately broken generated fixture to fail, it does not change
the source.

most feature folders have their own `tools/check_*.py` / `tools/run_*.py`, read `--help` for what they need.

## old checks

some older runners keep exact snapshots of retired architectures. useful for comparison, but a hardcoded old hash or a
missing historical package is not a current test. `tests/protected_baseline171.json` points at an old tree that is not
in this source - don't rewrite its hashes to make it pass. `tools/run_v66108_checks.sh` needs Bash, Ninja and
Clang/GCC. the full suite has known historical fixture failures (tests/README_D87_RUNNER.md), a focused pass and an old
suite failure can both be true. say which checks you ran, a count of green lines is not gameplay validation.
