# Tests and evidence

Use a fresh output directory. Tests that extract production functions should read
this source tree, not a generated fixture left beside an older delivery.

## Focused D84 portable tests

A small current target set is available without configuring every historical
fixture:

```bash
cmake -S tests -B ../d84-tests -DKCD2VR_D84_FOCUSED_ONLY=ON -DCMAKE_BUILD_TYPE=Release
cmake --build ../d84-tests --parallel 2
ctest --test-dir ../d84-tests --output-on-failure
```

These tests preserve assertions in Release. Native/GFx/OS leaves are controlled
fixtures; the targets are not the Windows mod. VALIDATION_D84.md lists the
additional literal production-consumer runners, sanitizer runs, exact-native
checks and expected-failing historical controls actually executed for D84.
Native binary checks require the user's supplied, pinned retail DLL locally;
no native byte slices or proprietary DLL are distributed in the patch.

D85 zone adds `zone_d85` (header rules against the recipe fixture and the 2 Nov log zone
pairs) and `zone_d85_production` (`tools/test_zone_d85.py`: the unchanged direct-context
block and both strike-receipt zone expressions compiled against recorder leaves). Run the
script with `--source <unpatched tree>` as the negative control; it fails on D84R1.

## Focused Windows checks

Python 3 and the Visual Studio x64 C++ tools are needed. AddressSanitizer runs
also require the MSVC sanitizer component. The real native-attach cleanup test
uses Windows structured exceptions and the Windows SDK; a portable `try/catch`
replacement would miss the bug it is testing.

The map/interaction suite is run with:

```bat
python tools/run_m41_checks.py --out C:\temp\kcd2vr-checks --pak PATH_TO_CURRENT_MAIN_PAK
```

Its Lua tests need `lupa.lua51`. Use `--lua-path` for an existing package directory.
The runner also accepts `--source-root` and `--vcvars`. It never installs packages.
Use the matching delivered main PAK, not a historical prepared companion. Without
`--pak`, it checks current build/dist locations. The optional
`--control-no-model-gate` asks for an intentionally broken generated fixture to
fail; it does not modify production source.

These fixtures exercise current presenters, identity checks, Lua and movie code,
with explicit substitutes for native engine calls. Passing them does not prove
native rendering, headset placement, loading latency or a game DLL link.

## Historical checks

Some older runners preserve exact snapshots from retired architectures. They are
useful for comparison, but a hardcoded old hash or absent historical package is
not a current acceptance test. In particular `protected_baseline171.json` refers
to an older runtime tree and ownership document kept in private history. That
entire old tree is deliberately not included in the clean export. Do not rewrite
its expected hashes to make it pass.

The legacy `tools/run_v66108_checks.sh` additionally needs Bash, Ninja and a
compatible Clang/GCC environment. Record an unavailable compiler or fixture
failure honestly. A focused MSVC pass and an old-suite failure can both be true.

A release report lists the exact checks completed for that source. Do not run
every historical script and treat a count of green lines as gameplay validation.

D85 kicks. `python tools/test_kick_route_d85.py --out <dir>` runs the real no-target
route check and the R3 outcome funnel (2 s cooldown, one reason line). With
`--source <older tree>` it must fail, thats the negative control.
`python tools/run_native_kick_free_d85.py --out <dir> --dll <WHGame.dll> --tables <Data/Tables.pak> --pak <Mods/KCD2VR/Data/KCD2VR.pak>`
runs the retail kick selector bytes on the built rows for every loadout without a
target. It needs companion/source in the tree. It does not prove the animation or the hit.
