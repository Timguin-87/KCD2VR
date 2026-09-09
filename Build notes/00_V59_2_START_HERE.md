# KCD2VR V59.2 — Bottom Edge Ladder

Use KCD2 game FOV **110**. This run is based on the headset finding that this gives nearly correct world scale, head motion and sharpness, with only the lower uncovered strip remaining.

Press **F8** once. Six phases, 10 seconds each:
1. A BLACK REFERENCE — exact no-clamp tangent mapping; black lower edge is expected.
2. B VERTICAL SHIFT — Y translation only; X untouched.
3. C Y COMPRESS 88% — mild Y-only compression; X untouched.
4. D TAIL FROM 80% — upper 80% exact; only lower 20% remapped to fill.
5. E TAIL FROM 90% — upper 90% exact; stronger local remap in bottom 10%.
6. F BOTTOM HOLD — exact geometry until source runs out, then hold last bottom row.

Report for each: world scale, head-motion stability, visible black, lower-edge distortion, UI/dialogue visibility.

No native render-twice, no UV pose reprojection, no horizontal scaling changes.
