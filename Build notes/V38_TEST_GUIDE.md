# V38 test guide

Start with:

```ini
DiagnosticRendering=false
EnableVrOutputBridge=true
VrRenderScale=1.0
VrPresentationMode=2
VrPresentationZoom=1.0
VrLeftOffsetX=0.0
VrLeftOffsetY=0.0
VrRightOffsetX=0.0
VrRightOffsetY=0.0
```

Modes:
- 0 = Stretch (exact V37 behavior)
- 1 = Fit entire desktop frame, preserving aspect ratio with black bars
- 2 = Crop/fill the eye texture without aspect distortion (default)

Useful alignment experiment: change offsets by 0.01 at a time. Positive X samples farther right in the desktop source; positive Y samples farther down.

Expected markers:
- V38-PRESENT config ...
- V38-RESOLUTION eye=0/1 recommended=... swapchain=...
- V38-PRESENT fullscreen blit pipeline created with UV fit/crop controls
- V38-PRESENT-BLIT ... mode=2 zoom=1.000

V37 PRESENT capture remains the source and diagnostic clear remains the final fallback.
