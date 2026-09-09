# V58.3 build note — FOV/menu/input regression repair

Baseline: V58.2 source candidate (itself based on the V58.1 build-fix package).

## Runtime evidence addressed

The V58.1 headset test showed three distinct issues:

1. The MIS fallback calibration had drifted from the V56.1 130-degree baseline to 115 degrees, and the active KCD2 ViewCamera started at 100 degrees even though the game UI showed the user's saved 130 FOV. Moving the KCD2 graphics FOV slider away and back caused the active camera to change. V58.3 restores the 130-degree MIS baseline and performs a one-time reversible `cl_fov` pulse only after the first real ViewCamera publication.
2. The F7 cylinder was active at the same time as the hybrid desktop mirror. V58.3 gates desktop mirroring off by default, leaving only the dedicated XR layer.
3. OpenXR Touch acquisition was alive (A and left-stick events appeared in the trace), but the Win32-emulated KCD2 actions were not useful in-game. V58.3 switches keyboard events to scan-code injection when available and adds success/failure diagnostics for keyboard and mouse injection.

## Deliberately NOT included
- No ViGEm/XInput implementation yet.
- No native skeleton writes / body IK writes.
- No true-stereo renderer changes.
- No new dialogue-camera change beyond the V58 baseline.
- No new WHGame RVA/hook experiments.

The purpose of this candidate is to get back to a known-good visual baseline and make the remaining controller failure measurable before changing input architecture.
