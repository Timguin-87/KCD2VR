# V61.50 headset test guide

## 1. Use a clean UI state
Do not evaluate HUD/letterbox changes in this build. Remove the experimental KCD2VR HUD PAK variants from the prior test. This branch is for head sync + VRIK ownership only.

Dynamic HUD note: its own source says startup precedence is `LuaDB/MCM saved value > dynamic_hud.cfg > default`. Therefore a saved MCM `no_letterbox=0` can override a changed cfg. This is not part of the VRIK test.

## 2. Head sync
Open F7. On the top of the right page:
- **Best VR settings** = existing balanced graphics toggle.
- **Fix head sync** = new toggle.

`Fix head sync` checked means pose source mode 2 (`SYNC_NOW`). Unchecked means mode 0 (`CACHED_XR`).

Keep the FPS cap independent. Reproduce the known smooth combination first: set the game's FPS cap to 84 or 85 with the existing FPS slider + APPLY, then keep **Fix head sync** checked.

## 3. VRIK READ first
The supplied INI has:

```ini
[Experimental]
EnableVrikSemanticOwnershipTest = true
```

The hooks install at startup but mode begins OFF.

Press F9 once -> **READ**. Do not expect arm movement. In the F7 menu, the VRIK status should eventually show:
- rig `OK`;
- plausible joint count;
- `handR` >= 0;
- correlated counter increasing;
- zero final/prebuild writes.

If rig stays WAIT or handR is invalid, stop there and send the normal KCD2VR log. Do not continue to write modes.

## 4. FINAL HAND_R
Only after READ is valid, press F9 again. The mode becomes **FINAL HAND_R**. This applies only a conservative semantic right-hand final-output perturbation after `WHGame+0x5FE2F0`.

Record whether Henry's visible right hand/weapon moves. If another character/LOD moves or corruption appears, press F9 until OFF immediately.

## 5. PREBUILD HAND_R
If FINAL executes but no visible right-hand motion occurs, press F9 once more -> **PREBUILD HAND_R**. This modifies semantic right-hand buffer-1 `QuatT` immediately before the exact final transform builder consumes it and restores the original immediately after.

If this also executes with no visible motion, stop. The next reverse-engineering target is downstream of `desc+0x20`; do not increase amplitude and do not return to OperatorQueue.

## 6. Logging
While a VRIK mode is active, the normal log writes one status line per second:

`V61.50 VRIK status mode=... rig=... joints=... handR=... correlated=... finalWrites=... prebuildWrites=... rejects=...`

This is intentionally sparse so logging itself does not recreate head jitter.
