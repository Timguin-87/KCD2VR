# KCD2VR V21 — XR + game-side fallback matrix

V21 keeps V20's single XrManager ownership, persistent last-known-good stereo cache, and dual generated-state tracing.

Automatic phases start when the correlated main-4E6C mode-2 target first reaches the experiment:

- 0–5 s: exact selected pair builder (0x866F6C), left/right XR split.
- 5–10 s: broad camera-build boundary (0x865550), selector-aware left/right XR split.
- 10–15 s: broad selector-aware legacy V7 behavior, +30 mm/+30 mm.
- >=15 s: exact legacy V7 base-bank behavior, +30 mm/+30 mm.

The exact pair hook becomes observer-only after 5 s so phases never stack.

XR source ladder inside stereo phases:
1. fresh-xr <=80 ms
2. cached-xr last-known-good <=500 ms
3. relaxed-xr <=750 ms
4. fixed-64mm diagnostic fallback

Important markers:
- XR-VIEWS-V21
- NATIVE-PAIR-V21 apply / observer / post
- NATIVE-BROAD-V21 apply

The pair post trace includes q0..q11 at out+0x000 and s2q0..s2q5 at out+0x290.
