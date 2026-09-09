# V58.4 build note

Reason for the branch: runtime feedback corrected the target. The previous 130-degree "baseline" was known-bad (binocular/too narrow), so V58.4 does not use it as the visual reference.

The current V58.1 log showed a fixed compositor source VFOV of 115 degrees while the actual KCD2 ViewCamera initially published 100 degrees and later changed after the in-game FOV slider was touched. V58.4 follows that live published camera FOV in its default angular-correct mode.

The former coverage safety clamp could reduce the entire tangent map whenever one asymmetric HMD edge exceeded the symmetric flat-source coverage. V58.4 default mode preserves requested central angular scale instead and edge-holds only out-of-source rays. F5 restores the old clamp/manual-source behavior for a one-launch A/B comparison.
