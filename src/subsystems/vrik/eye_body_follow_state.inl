static std::atomic<float> g_eye_forward115{.09f},g_eye_up115{.08f};
// V66.128 (a) pelvis follow + leg IK (toggle: F1 / VR menu CONTROLS / INI VrikBodyFollow6DOF).
// OFF reproduces V66.127 exactly: pelvis and legs are never selected for writing.
std::atomic<bool> g_body_follow128{false};
