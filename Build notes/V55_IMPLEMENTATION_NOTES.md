# V55 implementation notes

- Added `CameraHook::SetRotationFilterMode`.
- Mode 1 holds the origin-relative HMD quaternion at 45 Hz.
- Mode 2 holds it at 60 Hz.
- Mode 3 applies shortest-path quaternion SLERP with alpha = `1-exp(-dt/0.008)`; alpha is time based rather than game-frame based.
- Filter state uses QPC time and resets on each F8 phase via a generation counter.
- Existing V54 worst-angular diagnostics remain active for every phase.
- Native stereo phase 4 from V54 (CVar-only framework activation) was retired from this ladder because it did not produce distinct eye output. Proven pipeline re-entry attempts remain as phases 5/6.
- Added independent F10 weapon/combat and F11 pickup/object timestamp recorders.
- F7 bridge toggle is ignored for safety while in the VR menu.
