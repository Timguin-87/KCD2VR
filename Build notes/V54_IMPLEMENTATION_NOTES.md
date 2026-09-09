# V54 implementation notes

V54 intentionally separates tracking tests (1-3) from native-stereo attempts (4-6). No live resolution change. No AFR in the native-stereo phases.

Stereo attempt 4 enables existing renderer stereo CVars and supplies live OpenXR IPD. The game stereo framework itself was previously observed registered with default enable=1, so this phase focuses on the renderer parameters rather than blindly forcing the framework gate.

Attempts 5/6 intercept `CStandardGraphicsPipeline::Execute` for a Normal render view. They save the 3x4 camera at +0x8EE8, execute the scene at -IPD/2, then +IPD/2, then restore the original camera. Attempt 6 also temporarily writes selector +0x8EE0 as 0/1. All writes are guarded and reversible.

A successful pair of Execute calls alone does not prove stereo output: logs report `dualPassExecutions`; headset output must show distinct same-frame eye imagery. If both renders overwrite the same final target, the next step is locating/redirecting the per-eye render targets between passes.

VRIK phase zero adds an OpenXR-to-KCD2 hand target bridge but no unsafe native IK invocation.
