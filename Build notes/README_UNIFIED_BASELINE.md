# KCD2VR unified research baseline

This is the consolidated development baseline built from the latest user
`KCD2VR-scaffold.zip`.

It contains:
- current V10 selected-camera-pair stereo experiment with the write safety gate
- native stereo research
- body/FRIK architecture and tested two-bone IK prototype
- motion-control and native melee architecture
- tested stamina/weight weapon handling prototype
- HIGGS/PLANCK-inspired grabbing and NPC physical interaction architecture
- physical bow architecture
- smithing/alchemy native-action research
- virtual holster/attachment architecture
- target-lock/cursor suppression and wrist/body UI plan

Only the already-existing tracking/stereo code is active. Future systems are
configuration placeholders and research/prototype code until their roadmap phase.
See `UNIFIED_TEST_ORDER.md` and `ROADMAP.md`.


## V32 current probe
See `STEREO_RENDERER_CVAR_PROBE_V32.md`. V31 proved the game stereo framework gate is already 1; V32 maps the renderer-side stereo CVar objects/backing state.
