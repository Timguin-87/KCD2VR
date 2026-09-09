# KCD2VR V62.2 — native FlashUI + absolute-pose VRIK correction

V62.2 is a focused correction to V62.1. It preserves V62.1 F6/head-sync/culling/dialogue behavior and fixes the two failed experimental paths reported from the headset test.

## UI correction

V62.1 incorrectly issued `#UIAction...` strings through the CryEngine console. In retail Pure-Game this is not a reliable Lua execution bridge. V62.2 instead mirrors retail WHGame's own FlashUI path:

- FlashUI global: `WHGame+0x492D938`
- GetElement: FlashUI vtable `+0x70`
- HUD GetMovieClip: HUD vtable `+0x238`
- MovieClip GetDisplayInfo: vtable `+0xD0`
- MovieClip SetDisplayInfo: vtable `+0xC8`
- recovered display fields: X `+0x00`, Y `+0x04`, XScale `+0x0C`, YScale `+0x10`, flags `+0x34`
- scale is written in Scaleform percent (`55` for 55%), not fraction (`0.55`).

No Lua mod, Dynamic HUD, PAK, HUD.xml or hud.gfx replacement is required.

## VRIK correction

The V62.1 solver treated relative/parent-space pose records as absolute model-space joint locations. This caused every solve to reject before a write.

V62.2 validates the CPoseData vtable on the supported retail build:

- absolute/model-space getter: `WHGame+0x53F550` at CPoseData vtable `+0x10`
- relative/parent-space getter: `WHGame+0x53D830` at vtable `+0x50`
- relative setter: `WHGame+0x53F4D0` at vtable `+0x70`

The solver now reads current absolute upperarm/forearm/hand positions, derives real current arm lengths in model space, solves the controller target there, converts desired absolute rotations back to parent-relative rotations, and writes only relative upperarm/forearm rotations immediately before `WHGame+0x5FE2F0`. The original relative records are restored immediately afterward.

Sparse status includes `failStage`:

- 0 = OK / solve reached
- 1 = pose API validation
- 2 = absolute/relative pose read
- 3 = hierarchy / parent chain
- 4 = implausible model-space bone lengths
- 5 = invalid controller target
- 6 = two-bone solve rejected
- 7 = relative setter failed

## Preserved

- V61.12 corrected culling
- V60.1 dialogue/inventory source-FOV guard
- Fix head sync mode 0/2 checkbox
- V62.1 F6 AUTO / FORCE SCREEN / FORCE IMMERSIVE behavior
- no high-frequency file logging
- no retired OperatorQueue/WeaponSolver-joint VRIK writes
