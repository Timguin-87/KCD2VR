# Medieval Immersion Simulator — V46.4 Stereo Ladder

This is a source/project test package. No Windows `.asi` was built in the ChatGPT Linux environment.

Primary changes:
1. old F8 Phase 7 becomes the forced known-good mono geometry: cl_fov 110 + source VFOV 110 + OpenXR asymmetric-FOV remap, no CustomRes.
2. F8 becomes an 8x5-second stereo ladder while holding that geometry fixed.
3. menu hit testing and the software cursor are both scaled from Win32 client coordinates into current D3D12 backbuffer coordinates.
4. matrix-lock UX is explicit instead of silently ignoring clicks.
5. AFR blit count is exposed in stereo-phase logs.

Read `V46_4_TEST_GUIDE.md` before testing.
