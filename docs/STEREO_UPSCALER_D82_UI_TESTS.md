# D82 startup and graphics regression coverage

D82 exposes Native, FSR and DLSS as Stereo choices. FSR and DLSS remain candidates pending a real Windows build and headset test. XeSS remains available in Mono and is refused in Stereo. Saving a technology in Graphics changes the next-start configuration; it does not replace resources in the running renderer.

A Stereo request is accepted only when the actual current native technology is known and supported. The existing settings worker saves that requested mode even if vendor instances are not ready yet. Actual stage4 activation has a separate requirement: FSR or DLSS must have a captured pair of vendor instances. Native needs no vendor pair. A missing pair opens Graphics with the restart explanation; the saved Stereo request enables early capture on the next launch. An explicit Mono selection cancels it through the existing mode setting. Unknown or unsupported native settings open Graphics with an explicit supported selection and do not save the request. A stale display state does not override the backend check.

The earlier D82 draft also discarded a Stereo request when twins were missing. That left a saved Mono profile in a restart loop: early capture stayed disabled after restart. Separating persisted intent from the existing worker activation gate resolves that loop without changing the live native technology.

The first Graphics draw originally erased the restart explanation when it seeded the current native settings. The D82 click-then-draw regression exposed this, and the production draw now retains that explanation while still reading the actual technology and quality.

## Tests that execute production code

The new runners extract literal functions or delimited source blocks. The fixture replaces operating-system, native configuration, drawing, and aggregate-readiness leaves. It does not reproduce the policy or rewrite production files to pass.

| Runner | Coverage | Result |
| --- | --- | --- |
| `tools/check_stereo_menu_d82.py` | Actual menu clicks/drawing, upscaler and mode-save callbacks, `ApplyMenuState` entry guard/commit, requested-mode INI write, and worker readiness operand | 635 checks passed |
| `tools/run_stereo_upscaler_d82.py` | Actual startup policy, parser, `LoadLaunchOptions`, `LaunchCompatible`, `CommitIni`, and `SaveSource` | 525 checks passed in optimized and ASAN/UBSAN builds |
| `tools/check_stereo_wiring_d82.py` | Production bootstrap, provider, source-ownership, epoch and queue wiring | Source contract checks; see the latest evidence report |

Menu coverage includes every native FSR/DLSS quality, all seven DLSS presets, exact values saved for restart, XeSS rejection in Stereo, unchanged Mono behavior, invalid or unreadable native settings, changed native readbacks between UI and backend, stale displayed mode, config-write failures, and deferred activation. A simulated restart exercises the retained mode3 request; the actual worker readiness operand remains false until the vendor creation leaf supplies a pair. Drawing checks the actual text and its existing rectangles. The explanatory text states that headset validation is still needed.

Startup coverage includes the three supported choices, the fourth Keep choice only when actual config is compatible, all native quality values, authoritative `user.cfg` over stale pending metadata, missing and unreadable config, duplicate assignments, preservation of unrelated graphics settings and calibration, copying the existing INI before editing, and failed atomic replacement. The startup graphics writer does not modify the INI. `CommitIni` edits its existing owned setup fields while retaining unrelated user preferences and calibration.

Seven causal negative controls are rejected. The menu controls discard the Stereo request until twins exist, omit the readiness explanation, trust stale UI mode for XeSS, and erase restart status on first draw. The startup controls permit unverified Keep, trust stale profile mode in the writer, and omit copying the existing INI. The mode-save fixture substitutes the existing asynchronous save request and filesystem leaves; it executes the exact production `VrStereoExperiment` writer expression when the simulated save worker flushes. These deliberately failing variants exist only in generated test directories. They are not production changes.

Run from the source root:

```sh
python tools/check_stereo_menu_d82.py --out /tmp/kcd2vr_d82_menu
python tools/run_stereo_upscaler_d82.py --out /tmp/kcd2vr_d82_startup
python tools/check_stereo_wiring_d82.py --out /tmp/kcd2vr_d82_wiring
```

Each output contains source hashes, the actual extracted blocks where applicable, compiler/run logs, and a JSON result. The D82 evidence set is under `evidence/upscaler_ui` in the repair handoff.

## Historical expectations

The D81R2 tests and runners remain unchanged as historical evidence. Their expectation that Stereo must reject every temporal upscaler is superseded by D82's captured-instance policy. Use `tests/stereo_menu_d82_test.cpp` and `tests/stereo_upscaler_d82_test.cpp` for the current boundary behavior. The D81R2-specific assertions embedded in `tests/startup_launch_options_d80_test.cpp` also describe the earlier Native-only policy; they are not a current D82 acceptance gate.

This does not supersede unrelated Windows startup-dialog coverage. The host fixtures do not execute Win32 font rasterization, real profile-API encoding and sharing behavior, the complete `ApplyMenuState` body, vendor GPU code, or a headset. They establish the source-level selection, persistence and refusal behavior. They do not establish stereo image quality or performance.
