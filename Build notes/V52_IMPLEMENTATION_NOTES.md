# V52 implementation notes

- Replaces the V51 legacy/QPC-late F8 phases with render-pose ownership experiments.
- Adds XrManager projection pose mode: dynamic or frozen-at-phase-start.
- Keeps default camera injection at reversible PublishViewCamera with immutable predicted OpenXR pose.
- Adds Oculus Touch A/B actions; left X/Y remain recenter bindings.
- Adds direct VR menu navigation and a software cursor in the menu texture.
- AFR phases now seed the camera IPD from the latest plausible OpenXR raw IPD instead of a fixed 64 mm constant.
- No live KCD2 resolution switching.
- No claim of native stereo or same-frame dual-eye rendering.
