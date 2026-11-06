# R93R4: full source and separate optional gore

This is the complete cumulative source after R93R3. The separate build receipt
records the exact source commit, source closure, linked DLL, main companion and
optional gore hashes. Build binaries and raw historical logs are excluded from
the source ZIP. Extract into a new directory; no earlier copy-over is needed.

## Crossbow operation and remaining boundaries

The hand-driven path supports the native light, medium and heavy classes
(10, 14 and 15). It is independent of Instant Reloads. Hold the crossbow in the
left hand and use the right hand for the reload:

- Light: grip the string and pull it along the draw rail.
- Medium: obtain the native spanner with the chest Grip gesture, engage the
  string and perform the pull.
- Heavy: the actual native lever moves from the right hand to its weapon socket.
  The current gesture is a short rail/lever movement, not simulated rotary winding.
- After spanning, the quiver Grip requests native ammunition. The actual bolt
  belongs in the right hand until it is aligned with the crossbow rail and
  released there; the native nock event still owns the loaded state and cost.

The previous corrections make the body, string source and tool/bolt placement
agree on the completed pose. R93R4 also separates a missing placement receipt
from real item loss: for an unchanged item it retains an already-owned zero-speed
pause for at most 80 ms from the original successful placement. Fresh controller
samples cannot renew that deadline. No hand pose is cached, no gesture progress
or nock event is credited during the wait. This does not grant a first placement
which never succeeded; genuine identity changes still release immediately. Grip release, an incomplete pull or poor bolt
alignment normally pause/regrip rather than complete the reload automatically.
Real ownership changes, invalid native clips/clock, missing current geometry,
foreign native speed changes, and expired current input can still give native
control back. It is not safe to label an old pose as a new native clock.

The source path exists for every class. A successful new complete headset reload
for every class has not been established. Exact operation and remaining failure
boundaries are in `CROSSBOW_CURRENT_BEHAVIOR.txt` and the delivery review.

## Separate optional gore

The installed pack and the reviewed D77 donor are compared by member content,
not just their ZIP container hash. The optional overlay changes the main Lua
script only, retaining the reviewed assets. Head-only cap support and explicit
native limb cleanup acknowledgment are included. Neck-covering clothing is not
attached to detached heads. Native hit/death/cut eligibility remains unchanged.

To produce the optional package using the existing reviewed assets:

```bat
build_release.bat -OptionalGoreSource "E:\backup\KCD2VR\gore_D77_fix_v62_39"
```

Without that argument the build produces the main gameplay package only.
Optional gore assets are deliberately absent from this source ZIP. They are
provided separately in the gore delivery ZIP. A local build never installs them
automatically. The delivery receipt separately records any explicitly requested
installation into the game Mods directory and its retained backup.

## Validation

The new placement-receipt adapter passes 122 checks each in debug and optimized
MSVC builds, including all three bolt classes and both tool classes. Existing
regression fixtures cover the other modified ownership boundaries; the delivery records the actual Windows link.
Earlier source tests are detailed in R93R3_REVIEW.md. The legacy shell aggregate
still needs clang++ and could not configure on this host. The old D54 gore
presentation harness has identical failures on the unmodified D77 donor and
current script; these are not reported as newly passing tests.

No camera, stereo, upscaler or bow aiming retune belongs to this change. Existing
INI values and calibration are retained. Host tests and a successful link do
not establish headset acceptance.
