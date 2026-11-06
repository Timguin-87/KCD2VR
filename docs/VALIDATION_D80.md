# Validation

D80 is a successor to frozen D79 commit
`fa50fe526412516733f023cef5c6a07160052d4a`. The delivery manifest must identify
the final source commit and DLL/PAK/archive hashes. The full Windows x64
RelWithDebInfo DLL linked successfully through `build_release.bat`. The proxy,
imports, embedded menu art and build marker verification passed. No D80
injection or headset test is claimed. The final linked DLL SHA-256 is
`0ad65bd70fa12b243f5cd9b27d1be65451cf390535e5080cea160b0600d57889`.

The latest human headset report is D79: the bow was fixed and native mortar
emptying worked with particles. Horse riding remained bad, map performance had
regressed, and hand/item collisions remained inconsistent. These observations
are preservation targets and outstanding problems, not D80 test results.

## Completed checks

- Nineteen distinct prior repair/adapter fixtures passed under MSVC after final
  fixture corrections. Coverage includes requesting-hand pickup admission,
  instant pickup and flask receipt lifetime, held-tool contact, crossbow entry
  and transition timing, mounted ownership, dice activity classification,
  feature gates, and startup/graphics option handling.
- Eight selected fixtures passed AddressSanitizer, including the corrected final
  prompt policy. The gore menu additionally passed 47 checks under both MSVC
  and ASAN. It preserves saved head style and limb preferences through the
  OFF / HEADS ONLY / HEADS + LIMBS cycle; limbs default OFF.
- The regenerated mortar and flask adapters passed 78 and 292 checks under
  MSVC AddressSanitizer after adding the production namespace alias to the
  extracted test context. Runtime code was unchanged by that fixture correction.
- Both new herb-sound selector/queue and native-service fixtures passed ASAN.
  The actual Lua 5.1 callback, using the shipped `AudioUtils` with engine audio
  calls substituted, passed 39 checks. This proves bounded dispatch and
  deduplication, not audible playback. The accepted native mortar feedback path
  is unchanged.
- The focused D80 CMake/CTest configuration passed **7/7 tests with ASAN**:
  pickup input, prompt policy, riding pose, gore menu, herb sound selection,
  herb sound service, and the actual crossbow transition adapter. These overlap
  the individual fixtures above; they are not additional independent coverage.

The final repair checks also passed under MSVC AddressSanitizer:

- Left-trigger anvil assistance and retained smithing adapters: **364 checks**.
  Assistance is limited to the left hand in qualified anvil phases; heating and
  the right-hand tool retain their own ownership.
- Native hammer-grip capture: **46 checks**; retained tool/support lifecycle:
  **44 checks**. The latter verifies that heating preserves free tongs motion
  while the detached blank remains native, rather than restoring the retired
  whole-hand lock.
- Smithing/alchemy held-tool contact: **209 checks** across four adapters.
  Coverage includes current native front-face qualification, withdrawal,
  tangential motion, controller rotation, the 20 mm correction cap and owner
  changes during native queries.
- Free-hand contact and retained regression suites: **608 checks**, including
  **31** current unavailable-coverage checks. They cover changed source samples,
  root/feature geometry, unknown tracking and invalidated entity-backed contact.
  Missing coverage follows the current valid hand request; it does not create a
  clearance receipt or replay an old fixed wrist.

These totals describe overlapping focused fixtures, not independent gameplay
scenarios. Synthetic native leaves cannot prove every real mesh, moving door,
station tool or headset motion. The contact policy deliberately permits brief
clipping when coverage is unavailable rather than adding a frozen-hand timeout.

The current review receipts record source hashes, compiler commands and results.
Native memory, physics, registry, rendering and audio operations are explicit
substitutes in host fixtures where required. Passing them establishes source
behavior, not native animation completion, headset smoothness or gameplay success.
The legacy `tools/run_v66108_checks.sh` run could not configure because `clang++`
was unavailable. It is **not** counted as passing.

## Reproduce the focused checks

From an x64 MSVC developer command prompt, with Visual Studio 2022 C++ tools,
CMake and the MSVC AddressSanitizer component installed:

```bat
cmake -S tests -B build-d80-tests -G "Visual Studio 17 2022" -A x64 -DKCD2VR_D80_FOCUSED_ONLY=ON -DKCD2VR_HOST_ASAN=ON
cmake --build build-d80-tests --config RelWithDebInfo
ctest --test-dir build-d80-tests -C RelWithDebInfo --output-on-failure
```

For a non-ASAN run, use a separate build directory and
`-DKCD2VR_HOST_ASAN=OFF`. For the complete Windows DLL build, run
`build_release.bat`; see [BUILDING.md](BUILDING.md) for dependency and optional-gore
staging details. A successful fixture build is not a successful DLL link.

## Preserved boundaries and remaining limits

Thirty-one pinned bow/camera/stereo source files match D79 after normalizing
line endings; they are not claimed to be byte-identical.
This is a file-specific comparison: narrow dice activity classification and
mounted upper-body ownership changed in other files. No camera projection or
stereo scheduling rewrite was made. The controller-driven hand fixes and
accepted mortar/forge behavior remain preservation targets.

No game installation or user INI/calibration modification is part of this
validation. Optional adult gore assets remain a separate package. The new menu
exposes the existing gore controls; it does not establish successful limb cuts.

D80 is not yet headset-proven for complete hand smoothness, every pickup,
reliable crossbow reloads, riding, all crafting feedback, dialogue head following,
map performance/markers, corpse dragging, or inventory-backed knife throwing.
The held-tool contact repair addresses its observed freeze policy; it is not a
claim that all world/finger collisions are fixed.
