#pragma once
// R107 (Miki): in cutscenes / native dialogues the game draws its cinematic bars for the 16:9 band, but our source
// frame is nearly square (2270x2429), so above and below the bars the world was still visible. When this is ON the
// Theater screen shows only the central 16:9 band of the frame in authored scenes (cutscene, native dialogue) -
// the rest is the black Theater surround, and the band is drawn at the full screen width = bigger too.
// Loading screens and menus are not cropped. Switch: Experiments page 1 "Cutscene 16:9 bars", INI VrCinematicBarsR107.
#include <atomic>
namespace kcd2vr::cinematic_bars_r107 {
inline std::atomic<bool> enabled{true};
inline constexpr float kAspect=16.0f/9.0f;
}
