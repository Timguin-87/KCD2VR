# V61.35 depth-push merge / MSVC compile fix

Fixed the V61.35 final-output branch after merging the F4 depth-push behavior.

- Added live trusted-depth candidate access in `DeviceHook`.
- Added optional depth SRV + small pseudo-shift modulation in `XrManager`.
- F4 now arms/disarms both the generic depth observer and depth-push consumer.
- Preserved the new F9 final-output READ -> READ+WRITE -> OFF test; no CPoseData write path was re-enabled.
- Removed stale references to nonexistent `f8_restore_game_fov` and `f8_restore_source_fov`.
- F8 remains Center-Y correction only with game/source FOV user-owned.
- Corrected the associated `snprintf` argument list.

Source/static checked only; Windows/MSVC/headset runtime verification is still required.
