# KCD2VR V61.20 — HUD XML tuner + lean stereo chase

## Build identity
Expected debug line:

`V61.20 BUILD ID HUD-XML-TUNER+LEAN-STEREO | FOV=95/95 | ROTORDER=RETIRED | HEADSCALE=RETIRED | HUD=XML-SCALE/X/Y | STEREO=STATIC-LEAN | CULL=GAMEPLAY-ONLY`

## What changed

- Game FOV default: 95.
- Source FOV default: 95.
- Rotation-order B*D / D*B experiment is retired from the menu and fixed to the stable default.
- Head/world translation-scale slider is removed from the menu.
- The old cull-scale slider is removed from the menu because the stable gameplay-only cull path no longer uses it as a tuning control.
- F6 theater scale is no longer occupying the main HUD tuning area.
- V61.19's hot stereo-gate and duplicate graphics-pipeline diagnostic detours are NOT installed. This should remove the diagnostic-induced head jitter.

## HUD XML tuning — priority test

The left page now contains:

- `HUD XML scale (restart)` — 0.50x to 1.10x.
- `HUD XML X` — -700 to +700 px.
- `HUD XML Y (- = up)` — -700 to +700 px.
- `WRITE HUD + RESTART`.
- `RESET HUD + RESTART`.

The coordinates are based on the same 1872x2016 HUD canvas that produced the previously tested 85% safe-area PAK.

At scale 0.85, X=0, Y=0, V61.20 writes exactly the old tested rectangle:

`top=151 left=140 width=1591 height=1714`

Therefore Y=-150 produces approximately:

`top=1 left=140 width=1591 height=1714`

which is the same 85% HUD size moved upward instead of downward.

### Suggested first HUD test

1. Remove/disable the old HUD-rescale mod so there is no conflicting `Libs/UI/UIElements/HUD.xml` override.
2. Start KCD2 with V61.20.
3. Open the KCD2VR menu.
4. Try `HUD XML scale = 0.85`, `X = 0`, `Y = -150`.
5. Click `WRITE HUD + RESTART`.
6. Exit KCD2 completely and relaunch it.
7. Check HUD position, interaction prompt, and archery cursor.
8. Adjust Y/X/scale, write again, restart, and repeat.

The button creates/updates:

`<KCD2>\Mods\KCD2VR_HUD_Runtime\mod.manifest`

and

`<KCD2>\Mods\KCD2VR_HUD_Runtime\Data\KCD2VR_HUD_Runtime.pak`

The PAK uses STORE/no compression, matching KCD2's documented retail mod requirements.

`RESET HUD + RESTART` removes only the generated `KCD2VR_HUD_Runtime` PAK/manifest. It never deletes third-party HUD mods.

Important: this XML method is startup-loaded. It does not pretend to be a live FlashUI transform. The sliders are live in the VR menu, but the game HUD changes after `WRITE` + full game restart.

## Stereo

V61.19 already captured the useful data, so V61.20 does not keep expensive hot-path observers running.

Retained static anchors:

- renderer global vtable RVA `0x3DFC008`
- `GetOrCreateRenderView` `WHGame+0x43EB60`
- `CStandardGraphicsPipeline` `WHGame+0x57278C`
- CSystem stereo candidate global `0x492D8C0`, vslot `0xF8`
- VolCloud `LeftEye` string RVA `0x3E01CE8`
- LeftEye registration/store sites `0x742413` / `0x742438`

V61.19 runtime capture showed the graphics pipeline alternating between two Normal `CRenderView` objects while the `LeftEye`/stereo parameter path was live. V61.20 preserves that evidence and continues from static reverse engineering instead of adding another render-thread detour.

Expected V61.20 log:

`V61.20-STEREO lean mode: no CSystem gate detour and no duplicate graphics-pipeline detour; static RE continues from V61.19 capture`

## What to send back

- Screenshot of the V61.20 menu so we can confirm the HUD controls are readable.
- HUD screenshot after the first 0.85 / X 0 / Y -150 restart test.
- Short debug log (30–60 seconds is enough).
- Whether the slight head jitter from V61.19 disappeared.
