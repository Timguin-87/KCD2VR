# V61.7 Culling timing probe

Observer-only addition to V61.6. No cull/FOV writes and no injection-mode change.

During normal gameplay, turn the head left/right across scenery that visibly pops.
Search KCD2VR-debug.log for `V61.7-CULL-TIMING`.

Key fields:
- `sameCamera=true`: frustum and publish refer to the same CCamera.
- `ageMs`: time between latest frustum sample and publish.
- `deltaDeg`: angular mismatch between the orientation used to build culling planes and the XR-posed camera being published.

If deltaDeg rises during head turns while ageMs is small, mode-1 late pose injection is a strong cause of edge culling.
If deltaDeg stays near zero while popping occurs, pursue native KCD2 LOD/object/light culling instead.
