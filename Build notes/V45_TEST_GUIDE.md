# V45 test

Use the packaged INI first. It intentionally starts with:

```
VrStereoExperiment = 0
VrPresentationMode = 1
VrAfrIpdScale = 0.0
VrSwapEyes = false
```

This keeps the working HMD bridge mono/Fit while the recursive-view path is observed.

For the strongest chance of naturally exercising CryEngine's recursive RTT path, load a normal outdoor gameplay scene containing visible water/reflections. You do not need to change IPD/FOV/presentation settings for this test.

Send back the log. The key markers are `V45-GET-RENDERVIEW`, `V45-RECURSIVE-PASS`, and `V45-PIPELINE ... viewType=1(Recursive)`.

Also include any `V45-RECURSIVE-RESOURCE` lines; these identify the GPU texture(s) used by the type-1 pass.
