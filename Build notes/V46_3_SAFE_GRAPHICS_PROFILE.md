# V46.3 Safe Graphics Profile — rationale

The goal of this build is to bring useful renderer findings forward **without destabilizing the stereo investigation**.

### Why only view-distance floors?

Stereo-safe culling and longer world draw distance are separate problems. A small ordinary view-distance uplift reduces obvious flat-game distance pressure without pretending to solve left/right frustum visibility. The chosen floor (120) is inside Warhorse's shipped quality range and never reduces a user's higher setting.

### Why motion blur off?

Camera motion blur is a poor baseline for head-tracked VR and can contaminate temporal history. V46.3 disables `r_MotionBlur` only while the MIS Balanced profile is active.

### Why `r_Sharpening`, not CAS yet?

KCD2 already exposes a final image sharpening CVar. Using it gives us a low-risk clarity control while mono resolution/FOV/CustomRes behavior is still being measured. A mod-owned D3D12 CAS pass remains a later option once the authoritative eye texture is established.

### Why no culling/shadow/TAA changes?

Those are exactly the systems most likely to require **per-eye or shared-stereo semantics**, not static quality presets. Forcing them before native stereo can make a broken path appear fixed or create large performance regressions. They remain explicit future matrix tests.

### Restoration

MIS snapshots the five touched CVars before the first successful apply. Disabling the profile restores that snapshot immediately. Shutdown also restores the snapshot before renderer/OpenXR teardown. If any setter fails during profile application, V46.3 immediately rolls back instead of leaving a partial profile active.
