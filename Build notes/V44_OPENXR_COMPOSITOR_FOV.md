# V44 OpenXR compositor FOV remap

V43 did not modify the live projection: every V43-XR-FOV line reported applied=false because the mode-0 render record projection slot was zero.

V44 adds presentation mode 3 in the proven final D3D12 bridge shader. It maps each XR eye pixel through the runtime XrFovf tangent bounds into the captured desktop camera image. This is a compositor/projection correction only; it does not create geometry stereo or replace the native-stereo goal.

Settings:
- VrPresentationMode=3
- VrSourceVerticalFovDegrees=75.0
- VrPresentationZoom remains available as a post-remap diagnostic scale.

Expected marker: V44-PRESENT config ... mode=3 ... sourceVFov=75.00

Compare mode 1 (Fit) against mode 3. If mode 3 materially changes zoom/edge geometry, the visible mismatch is substantially projection-related. If it does not, the remaining double image is dominated by eye-image generation/routing rather than FOV.
