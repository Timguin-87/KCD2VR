#pragma once
namespace kcd2vr::native_taa_probe_v6657 {
// V66.57: read-only TAA jitter probe. Hooks the TAA pass 0x180661DEC (PostAA temporal,
// 3 register args, pass-through) and, every 20 s, samples the CRenderView view info
// (crv+0x9B30, the struct 0x18057B050 returns to the TAA pass) for 8 consecutive stereo
// frames per eye. Logs the small, frame-to-frame changing values = the TAA jitter, so we
// can see whether each eye gets a moving jitter, the same one, or none.
bool Install() noexcept;
}
