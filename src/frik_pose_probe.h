#pragma once
#include <cstdint>

namespace kcd2vr::frik_pose_probe {

struct TrackingPose {
    float px=0.0f, py=0.0f, pz=0.0f;
    float qx=0.0f, qy=0.0f, qz=0.0f, qw=1.0f;
};

enum class VrikTestMode : int {
    Off = 0,
    Read = 1,
    PrebuildHandDelta = 2,
    PrebuildTwoBonePositions = 3,
};

struct ProbeStatus {
    int mode=0;
    bool rig_valid=false;
    bool calibrated=false;
    bool tracking_valid=false;
    bool gameplay_allowed=false;
    std::uint32_t joint_count=0;
    int hand_r=-1;
    int solver_joint_c=-1;
    int solver_joint_e=-1;
    std::uint64_t correlated_calls=0;
    std::uint64_t setik_calls=0;
    std::uint64_t rig_maps=0;
    std::uint64_t final_output_writes=0;
    std::uint64_t prebuild_writes=0;
    std::uint64_t rejects=0;
    float upper_length=0.0f;     // direct selector-1 U->F length
    float fore_length=0.0f;      // direct selector-1 F->H length
    float ref_upper_length=0.0f;
    float ref_fore_length=0.0f;
    float controller_delta=0.0f;
    int fail_stage=0;
};

struct VrikDiagnostics {
    bool valid=false;
    std::uint64_t sample=0;
    int upper=-1, fore=-1, elbow=-1, hand=-1;
    int parent_upper=-2, parent_fore=-2, parent_elbow=-2, parent_hand=-2;
    int solver_joint_c=-1, solver_joint_e=-1;
    float b0_upper[3]{}, b0_fore[3]{}, b0_elbow[3]{}, b0_hand[3]{};
    float b1_upper[3]{}, b1_fore[3]{}, b1_elbow[3]{}, b1_hand[3]{};
    float ref_upper[3]{}, ref_fore[3]{}, ref_elbow[3]{}, ref_hand[3]{};
    float out_upper[8]{}, out_fore[8]{}, out_elbow[8]{}, out_hand[8]{};
    // Four common DQ translation interpretations. V62.9 uses the actual semantic
    // UPPERARM_R -> FOREARM_R -> HAND_R chain (403 -> 405 -> 408), not ELBOW_R helper 406.
    float cand_l1[4]{}, cand_l2[4]{};
    float b1_l1=0.0f, b1_l2=0.0f;
    float ref_l1=0.0f, ref_l2=0.0f;
    float controller_delta_cry[3]{};
};

bool Install() noexcept;
void Uninstall() noexcept;
void PublishTrackingSnapshot(const TrackingPose& head, bool head_valid,
                             const TrackingPose& right, bool right_valid,
                             bool gameplay_allowed) noexcept;
void SetLiveRightArmEnabled(bool enabled) noexcept; // compatibility: true -> READ, false -> OFF
bool LiveRightArmEnabled() noexcept;
void ToggleLiveRightArm() noexcept;
void ResetCalibration() noexcept;
void SetFinalSkinTestMode(int mode) noexcept;
int FinalSkinTestMode() noexcept;
void SetFinalSkinTestEnabled(bool enabled) noexcept;
bool FinalSkinTestEnabled() noexcept;
void CycleWriteMode() noexcept; // OFF -> READ -> HAND DELTA -> TWO-BONE POSITIONS -> OFF
const char* ModeName(int mode) noexcept;
ProbeStatus GetStatus() noexcept;
bool GetDiagnostics(VrikDiagnostics& out) noexcept;
}
