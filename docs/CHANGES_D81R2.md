# D81R2 — prevent incompatible upscaler selection in Stereo

> Historical D81R2 record. D82 supersedes the Native-only selection policy with
> separate DLSS/FSR stereo candidates. See CHANGES_D82 and TEMPORAL_STEREO_D82.
> The tests and untouched-renderer statements below describe D81R2 only.

## Why this patch exists

The user reported a doubled image after the D81R1 reviewed merge, then confirmed
selecting DLSS in the startup window. The user relayed a local inspection and
repair of game-root user.cfg: r_SuperResolution_Mode was returned to 0.

The uploaded source proves the selection path: Start KCD2VR.cmd opens the setup
export; SaveSource writes the selected technology to user.cfg; DLSS is mode 2
and Native is mode 0. The old startup and VR-menu save paths accepted temporal
upscalers while production Stereo was selected. There was no compatibility
check or clear Stereo warning.

This is a strongly supported configuration explanation for the reported image,
not a claimed GPU trace of the exact bad frame. The uploaded log contains hook
setup but no active upscaler/eye-pair observation, and the screenshot filename
predates that log. A full restart with mode 0 is the remaining headset check.

The native stereo renderer and eye/projection code in the tested source match
the previously supplied D80/D81 foundation. The native TAA and PostAA history
repairs do not implement separate DLSS, FSR or XeSS instances/history. NVIDIA's
DLSS Programming Guide, section 3.17, requires separate instances for multiple
views, including the two eyes in VR:
https://raw.githubusercontent.com/NVIDIA/DLSS/main/doc/DLSS_Programming_Guide_Release.pdf

Sequential eye rendering is not inherently incompatible with DLSS. The missing
piece in this integration is the separate upscaler context and correctly matched
per-eye inputs/history. DLAA and other upscalers' native-AA quality modes still
use those contexts and are covered by the same current limitation.

## Immediate recovery

Exit KCD2 completely. In the game's root user.cfg, use:

    r_SuperResolution_Mode = 0

Then restart the game. Choose Native (no upscaling) in the old launcher. Keep
preserves the existing game setting; it only keeps Native if the file actually
contains Native. The user reported that this file correction was already made
locally. This source patch is not needed merely to try that correction.

No IPD, FOV, projection, calibration or native AA setting needs to be reset for
this recovery. The two r_DuplicateLastFrame... controls mentioned in the user's
handoff are separate from upscaler history ownership; this patch does not change
them or claim that they make a temporal upscaler Stereo-compatible.

## What changed

- A shared selection policy restricts production Stereo to Native. Mono retains
  its existing technologies and quality levels.
- The startup form offers Keep only after verifying Native in actual user.cfg.
  Opening the form does not edit the file. Explicit Save writes Native when
  selected; Keep refuses an unsupported or unverified value before changes.
- SaveSource enforces the same policy independently of the UI. Duplicate active
  mode assignments are replaced using the existing allowlisted config writer.
  Unrelated keys and native AA selection are preserved.
- Pending startup saves/merges check compatibility and retain unrelated INI
  settings and calibration.
- The VR Graphics selector and its backend reject incompatible saves in Stereo.
- A live Mono-to-Stereo request checks the actual current upscaler. If it is not
  verified Native, the menu directs the user to save Native and restart. The
  backend checks again so stale UI state cannot authorize the transition.
- Documentation now states this limitation rather than presenting every native
  game upscaler as a working Stereo option.

Manual game/config edits and the game's own graphics menu can bypass mod-menu
selection guards. This patch deliberately does not install an AA watchdog,
rewrite live rendering resources, or silently fall back to Mono.

## Scope preserved

The patch is based on KCD2VR-D81R1-REVIEWED-MERGE-20261102 from the newly supplied
KCD2VR_SOURCE(2).zip. It retains that merge's gameplay, collision, finger and map
changes. 362 protected renderer/camera/pose/companion/INI files compare byte for
byte with the supplied source. Native stereo scheduling, native per-eye TAA,
OpenXR submission, camera math, IPD and hand timing have no edits in this patch.
The original input archive is retained unchanged.

## Validation and limits

180 startup checks pass in debug, optimized, and address/undefined sanitizer
builds. They run the actual extracted production parser and SaveSource with
Windows I/O leaves substituted. The old SaveSource compiles but fails the
unsupported-Stereo-write test. Leak detection alone is disabled because the
sandbox cannot enumerate process tasks; address and UB checks pass.

262 menu/backend checks pass. The unchanged D81R1 control fails 88 of those
checks. They execute actual extracted callbacks, admission/commit blocks,
Home/View click routes, Graphics controls and relevant drawing calls. Native
CVar/config/GDI leaves are substituted. Covered cases include all offered
upscaler qualities, DLAA, Native, Mono, missing data, stale UI state, and a
changed live CVar between the UI check and backend call. Drawing bounds are
checked; Windows font rasterization is not performed.

The compatibility checks do not prove the final headset image. No D81R2 Windows
DLL was built here and no new headset run was performed. The uploaded
build_release.log records a successful D81R1 build, not D81R2. Build the patched
source on Windows before installing this prevention change.
