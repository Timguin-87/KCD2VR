// 0 uncalibrated, 1 one hook thread is publishing baseline, 2 ready.
std::atomic<int> g_cal_state{0};
std::atomic<float> g_cal_rel_x{0},g_cal_rel_y{0},g_cal_rel_z{0};
std::atomic<float> g_cal_head_x{0},g_cal_head_y{0},g_cal_head_z{0};
std::atomic<float> g_cal_lrel_x{0},g_cal_lrel_y{0},g_cal_lrel_z{0};
std::atomic<float> g_cal_hq_x{0},g_cal_hq_y{0},g_cal_hq_z{0},g_cal_hq_w{1};
std::atomic<float> g_cal_lq_x{0},g_cal_lq_y{0},g_cal_lq_z{0},g_cal_lq_w{1};
std::atomic<float> g_cal_rq_x{0},g_cal_rq_y{0},g_cal_rq_z{0},g_cal_rq_w{1};
std::atomic<int> g_legacy_cal_state{0};
std::atomic<float> g_legacy_hq_x{0},g_legacy_hq_y{0},g_legacy_hq_z{0},g_legacy_hq_w{1};
std::atomic<float> g_legacy_rq_x{0},g_legacy_rq_y{0},g_legacy_rq_z{0},g_legacy_rq_w{1};
// V65.12 explicit T-pose calibration. Meta/Quest recenter arms capture; the
// validated Henry skinning boundary performs the actual avatar-space solve.
std::atomic<bool> g_vrik_calibration_requested{false};
std::atomic<std::uint64_t> g_vrik_calibration_requests{0};
// V65.43: require a short stable arms-out interval before committing calibration.
std::atomic_flag g_cal_stability_lock=ATOMIC_FLAG_INIT;
V3 g_cal_stable_head{},g_cal_stable_left{},g_cal_stable_right{};
std::uint64_t g_cal_stable_since_ms=0;
bool g_cal_stable_have_sample=false;
std::atomic<float> g_cal_pos_r_x{0},g_cal_pos_r_y{0},g_cal_pos_r_z{0};
std::atomic<float> g_cal_pos_l_x{0},g_cal_pos_l_y{0},g_cal_pos_l_z{0};
std::atomic<float> g_cal_rot_r_x{0},g_cal_rot_r_y{0},g_cal_rot_r_z{0},g_cal_rot_r_w{1};
std::atomic<float> g_cal_rot_l_x{0},g_cal_rot_l_y{0},g_cal_rot_l_z{0},g_cal_rot_l_w{1};
// V65.47 fixed calibrated avatar frame. Never rebuild tracking->model from the
// current animated native head; that was the cause of arms rotating behind Henry.
std::atomic<float> g_cal_frame_qx{0},g_cal_frame_qy{0},g_cal_frame_qz{0},g_cal_frame_qw{1};
std::atomic<float> g_cal_native_head_x{0},g_cal_native_head_y{0},g_cal_native_head_z{0};
std::atomic<bool> g_cal_native_anchor_pending{false}; // V65.66: XR arm calibration is immediately valid; first Henry pose only supplies model-space anchor
// V65.62: latest validated Henry head pose is copied while inside the proven
// character/pose callback. Guided F8 calibration can commit from this safe value
// without waiting for another lucky SetIK callback after the two-second capture.
std::atomic<bool> g_latest_native_head_valid{false};
std::atomic<float> g_latest_native_head_x{0},g_latest_native_head_y{0},g_latest_native_head_z{0};
std::atomic<float> g_ctrl_cry_x{0},g_ctrl_cry_y{0},g_ctrl_cry_z{0};

// Calibration consumes the view's room reference; it never owns/recenters it.
static std::atomic<kcd2vr::tracking125::OriginReaderH46> g_tracking_origin_probe_h46{nullptr};
static std::atomic<void*> g_cal_character_h46{nullptr}; // exact natural pose callback only
static std::atomic<std::uint64_t> g_cal_load_h46{0};

// D76: numeric calibration and consumed tracking-origin generation travel together.
std::atomic<std::uint64_t> g_cal_room_origin_d76{1};
// R132: why an F8 T-pose capture failed (one KCD2VR.log row). counted by the tracking producer while the capture runs,
// reset at each capture start. ok = admitted sample, blocked = tracking not allowed (game control/menu/dialogue),
// lost = headset or a controller not tracked.
std::atomic<std::uint32_t> g_cal_samples_ok_r132{0},g_cal_samples_blocked_r132{0},g_cal_samples_lost_r132{0};
