# V58.2 build note — Touch menu navigation + telemetry

Baseline: V58.1 BUILD FIX — Curved UI / Dialogue / Primitive VRIK / Touch.

This candidate fixes one static binding mismatch found during handoff continuation:
`Mod::Run` passed the right thumbstick into `VrMenu::PumpVrInput`, but the corrected
user specification assigns MIS/menu D-pad navigation to the left stick. V58.2 now
passes the left thumbstick while retaining the right-hand ray/trigger/A/B controls.

A 500 ms rate-limited raw Touch state line was also added to make the first hardware
test diagnosable even when an emulated keyboard/mouse action itself does not fire.
Right grip is observed in telemetry only and remains unmapped.

No native joint/IK writes, renderer re-entry, stereo resource changes, game-resolution
changes, or new WHGame hooks are included.
