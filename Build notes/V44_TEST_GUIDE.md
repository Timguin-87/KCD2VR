# V44 test guide

Start with:

VrPresentationMode = 3
VrPresentationZoom = 1.0
VrSourceVerticalFovDegrees = 75.0
VrSwapEyes = false

Compare mode 3 directly against mode 1 (Fit). If mode 3 changes the zoom/edge geometry, try source FOV 60, 75, and 90. This is a compositor projection correction only; it does not claim true native stereo.

Expected log:
V44-PRESENT config ... mode=3 ... sourceVFov=75.00
V44-PRESENT fullscreen blit pipeline created with Fit/Crop/OpenXR-FOV controls
