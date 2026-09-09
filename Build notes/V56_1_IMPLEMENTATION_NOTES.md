# V56.1 implementation notes

- Fixed rotation-filter selector clamp: accepted range is now 0..7 instead of 0..3.
- Adaptive rotation smoothing is the startup/default restore mode.
- Capture FOV startup/reset changed from 115 to 130 degrees.
- Corrected slider direction: right now increases both capture FOV and gameplay framing/zoom-out.
- Removed the mostly-useless framing range below 0.95 from the UI; OpenXR-FOV coverage safety could clamp it and make it appear unresponsive.
- Added hard RESET VIEW and RESET F6 actions.
- Added persisted `VrFlatUiIpdMm`.
- F6 projection-flat mode now interpolates both projection eye positions and per-eye asymmetric FOV around a common centered eye geometry. 0 mm gives identical eye geometry; negative values reverse the binocular geometry for diagnosis.
- This remains a projection-flat diagnostic, not the final OpenXR quad implementation for native menus/dialogue.
- Action recorders and observer-only IK probes are retained unchanged.
- No blind skeleton/IK writes were added.
