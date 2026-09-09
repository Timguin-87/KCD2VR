# KCD2VR V62.3 — geometry diagnostics + live UI preview

V62.3 is a diagnostic correction to V62.2. It preserves the V62.2 camera, culling, head-sync and F6 behavior.

## Why VRIK changed back to diagnostic-only
The V62.2 headset log showed selector-0 values that had been treated as absolute/model-space positions collapsing onto almost one axis, with derived arm lengths 0.142 / 0.005 m. Direct retail disassembly confirms +0x53F550 and +0x53D830 are raw QuatT record accessors for selector 0 and selector 1 respectively; neither performs hierarchy accumulation.

Retail +0x5FE2F0 is the hierarchy builder. It reads selector-1 from the player pose, selector-1 from a companion object at jointOwner+0x20, reads signed parent ids from a 0x30-byte joint descriptor table, and composes the final 0x20-byte-per-joint output.

Therefore V62.3 performs no skeletal writes. F9 enables low-rate capture of:
- semantic UPPERARM_R / FOREARM_R / ELBOW_R / HAND_R ids;
- direct parent ids recovered from the same descriptor table +0x5FE2F0 uses;
- raw selector-0 and selector-1 QuatT translations;
- companion/reference selector-1 translations;
- final 0x20-byte upperarm/elbow/hand outputs;
- four common dual-quaternion translation-decoding length candidates.

The main structured log remains sparse. Full VRIK snapshots go to KCD2VR-diagnostics.log at about one line per second.

## UI
The V62.2 native FlashUI path is retained. V62.3 additionally previews the semantic UI sliders while dragging (throttled to ~80 ms) so values can be found visually instead of Apply-only. Logging is separately throttled to avoid recreating the old logging-induced judder.
