# V61.13.1 correction — ENGINE DOF is the startup pseudo mode

V61.13 accidentally made the FAKE DOF sweep the startup presentation. That was not the requested behavior.

V61.13.1 changes startup to the KCD2 ENGINE DOF focus-driven pseudo path:

- ENGINE DOF starts ON.
- FAKE DOF starts OFF.
- KCD2 focus distance, when exposed by the current game, drives pseudo scale:
  - near <= 0.60 m: 0.80
  - 0.60–1.50 m: interpolate 0.80 → 0.85
  - 1.50–4.00 m: interpolate 0.85 → 0.98
  - far >= 4.00 m: 0.98
- 250 ms smoothing remains.
- The mod only reads the game DOF focus value; it does not alter KCD2 DOF.
- If no valid engine focus source resolves, it holds pseudo 0.95. It does NOT fall back to the fake sweep.
- MONO / .80 / .85 / .95 remain manual override buttons and disable ENGINE DOF.
- FAKE DOF remains available as a manual diagnostic toggle only.

Important: this is real engine-focus-driven adaptive pseudo if KCD2 exposes the focus scalar we are probing. It is still global pseudo strength, not per-pixel depth reprojection.
