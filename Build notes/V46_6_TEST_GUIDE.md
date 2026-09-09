# V46.6 test guide

1. Start in the KCD2 main menu and confirm the HMD is already in mono VR.
2. Check whether the bottom black bar is gone and whether the default 115° / 0.95 framing feels less nose-close.
3. Open F7 and verify mouse interaction still works.
4. Try `Mono framing` only a little around 0.95. It is now separate from capture FOV.
5. Press F8 once. Each phase lasts 5 seconds:
   - 1 = 112 / 0.97
   - 2 = 115 / 0.97
   - 3 = 118 / 0.97
   - 4 = 115 / 0.94
   - 5 = 115 / 1.00
6. Note which phase has the best world scale and least warping.
7. The log marker `V46.6-MONO-BLIT` includes `safeScaleLR`, which confirms whether the black-border safety clamp was active.

No CustomRes changes and no AFR changes are made by F8 in this build.
