std::atomic<int> g_pose_baseline_state{0}; // 0=no baseline, 1=capturing, 2=ready; reset on every F9/F10 mode change
// V65.50 F8 guided pose sample accumulator (tracking only; actual hand-frame calibration remains T-pose owned).
std::atomic<int> g_guided_cal_pose{0};
std::atomic_flag g_guided_cal_lock=ATOMIC_FLAG_INIT;
struct GuidedCalAccum {
    std::uint64_t n=0; V3 h{},l{},r{};
    bool haveQ=false; Q4 hqRef{},lqRef{},rqRef{},hqSum{0,0,0,0},lqSum{0,0,0,0},rqSum{0,0,0,0};
};
GuidedCalAccum g_guided_cal_accum{};
GuidedCalAccum g_guided_pose_result[4]{};
// Only the current T-pose accumulator is used. No retired F8 routine is restored.
static kcd2vr::calibration_profile::Window g_arm_window66100{};
static kcd2vr::core::SnapshotMailbox<kcd2vr::calibration_profile::Arms> g_arm_profile66100;
static std::atomic<bool> g_saved_arm_ready66100{false},g_cal_reanchor66100{false},g_cal_center_reanchor125{false},g_arm_capture_active66100{false};
static std::atomic<std::uint64_t> g_arm_save_generation66100{0},g_capture_session66100{0};
static std::atomic<bool> g_finger_capture66100{false};

std::atomic<bool> g_body_cal_ready{false};
std::atomic<float> g_body_shoulder_rel_x{0},g_body_shoulder_rel_y{0},g_body_shoulder_rel_z{-0.20f};
std::atomic<float> g_body_shoulder_width{0.40f},g_body_arm_span{1.40f};
std::atomic<float> g_body_guard_forward_x{0},g_body_guard_forward_y{1};
std::atomic<std::uint64_t> g_hard_body_epoch{1}; // V65.77: invalidate fixed upper-body baseline whenever F10 mode changes

QuatT28 g_base_r[4]{}; // upper, fore, elbow-helper, hand
QuatT28 g_base_l[4]{};

// V64.0: lock-free snapshot of the previous correlated skinning frame's desired
// hand/helper transforms. SetIK normally runs before the final hierarchy builder,
// so the upstream experiment deliberately uses the latest complete target rather
// than calling OpenXR or rebuilding the rig in the SetIK hot hook.
std::atomic<std::uint64_t> g_grip_seq{0};
QuatT28 g_cached_goal0_hand_r{},g_cached_goal1_hand_r{},g_cached_goal0_hand_l{},g_cached_goal1_hand_l{};
QuatT28 g_cached_goal0_weapon_r{},g_cached_goal1_weapon_r{},g_cached_goal0_weapon_l{},g_cached_goal1_weapon_l{};
std::atomic<bool> g_cached_goal_hand_r_valid{false},g_cached_goal_hand_l_valid{false},g_cached_goal_weapon_r_valid{false},g_cached_goal_weapon_l_valid{false};
