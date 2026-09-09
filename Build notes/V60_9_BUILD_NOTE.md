# V60.9 — HFOV + KCD2Tools Cull Sync + Normal View

Build ID: `HFOV-CULLSYNC-NORMALVIEW-20260906-M`

## New
- Added native `cl_hfov` slider/apply control (70–170°) on the right page.
- Applying `cl_hfov` logs `cl_hfov`, `cl_fov`, and custom-resolution values before/after so the reported resolution drop can be correlated.
- Replaced the old +0x30-only culling experiment with the coupled CCamera update documented by KCD2Tools TPVCamera: +0x30 plus +0x50/+0x58/+0x60/+0x68/+0x70, all restored immediately after the frustum builder.
- Existing Normal CRenderView type-0 logging remains enabled/read-only.

## Static findings
- Both WHGame.dll and WHGameArm.dll contain `cl_hfov` and `cl_fov`, confirming they are distinct native CVars.
- KCD2Tools states SetFrustum writes +0x30 and the five culling terms together; this explains why V60.4's +0x30-only widening could fail.
- KCD2Tools also exposes `System.GetViewCameraFov` / `System.SetViewCameraFov` in its script-bind docs.

No claim is made that `cl_hfov` is the culling control; V60.9 is designed to test that safely and log what actually changes.
