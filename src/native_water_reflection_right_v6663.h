#pragma once
namespace kcd2vr::native_water_reflection_right_v6663 {
// V66.63: cheap stereo repair for CWaterStage temporal reflection history.
// Retail owns one A/B pair ($WaterVolumeRefl/$WaterVolumeReflPrev).  KCD2VR runs
// CWaterStage once per eye, so the second eye overwrites the pair and LEFT's next
// temporal read sees RIGHT history.  The repair provisions one private RIGHT A/B
// pair and temporarily installs it only around RIGHT CWaterStage::Execute.
bool Install() noexcept;
bool Installed() noexcept;
}
