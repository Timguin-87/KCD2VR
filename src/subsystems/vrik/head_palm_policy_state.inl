static std::atomic<float> g_head_raise_max59c9{.18f}; // R59.16C9C: metres the VRIK head may rise above the calibrated anchor (INI VrHeadRaiseMaxCm)
// R59.16C8 (Miki: "can we automatically align the swords and two-handed grips to palm centre"): the OpenXR grip pose is the
// palm centre; the IK goal is the native wrist bone. With auto palm on, the wrist goal = grip - 0.5 * (wrist -> knuckle bar)
// from Henry's own finger skeleton in the desired hand orientation, so the palm (not the wrist) lands on the controller
// whatever the calibration. The manual wrist<->palm slider is added on top as a fine-tune. INI VrAutoPalmAlign (default 1).
static std::atomic<bool> g_auto_palm59c8{true};
static std::atomic<float> g_auto_palm_last_cm59c8[2]{0.f,0.f}; // diagnostics: last applied auto offset per side
