# V25 — consumed parent causality test

V24 proved the selected mode-2 child never reaches WHGame+0x653940 (`tier=target` zero hits), while its `source38` mode-0 parent is consumed there hundreds of times with moving world matrices.

V25 moves the reversible camera perturbation to that exact consumed parent at the safe broad `WHGame+0x865550` build boundary. The child exact-pair hook is observer-only.

Automatic phases after parent target acquisition:

- 0–5 s: parent baseline, no write.
- 5–10 s: selected parent camera A +100 mm local-right.
- 10–15 s: selected parent camera A -100 mm local-right.
- 15–20 s: selected parent camera A = OpenXR left half-IPD.
- >=20 s: selected parent camera A = OpenXR right half-IPD.

All writes are restored immediately after the original parent camera-build call.

Markers:
- `PARENT-CAUSAL-V25 selected`
- `PARENT-CAUSAL-V25 baseline`
- `PARENT-CAUSAL-V25 apply`
- `RENDER-CONSUME-V25`

Interpretation:
- Main image shifts right then left around 5/10 s: consumed parent is causal for visible color view.
- Only shadows/culling change: this parent record still feeds auxiliary geometry/culling despite downstream consumption.
- No change: `0x653940` is not the final visible color-pass boundary; continue downstream from its expanded renderer-side output.
