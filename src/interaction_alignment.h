#pragma once
#include <cstdint>
#include "vrik/palm_selection59c9.h"
#include "subsystems/interactions/dog_candidate_d59r1.h"
namespace kcd2vr {
class XrManager;
namespace interaction_alignment {
struct Status {
    bool installed{false}, enabled{false}, visual_ray_valid{false};
    std::uint64_t ray_builds{0}, interaction_calls{0}, redirects{0}, onscreen_checks{0}, onscreen_forced{0};
    float dir_x{0.0f}, dir_y{1.0f}, dir_z{0.0f};
    float native_dir_x{0.0f}, native_dir_y{1.0f}, native_dir_z{0.0f};
    bool native_dir_valid{false};
    float source_tan_x{0.0f}, source_tan_y{0.0f};
    float center_x{0.0f}, center_y{0.0f};
    float last_native_to_visual_deg{0.0f}, native_reach_m{0.0f};
    float base_radius_m{0.0f}, cone_half_angle_deg{0.0f};
};
bool Install();
void Uninstall();
// F5 capture only. Observer handles are retained until process exit.
void SetDoorDiagnosticsCaptureC54R3(bool active) noexcept;
void SetEnabled(bool enabled) noexcept;
// V66.156: backend lifetime is separate from ray-source ownership.
void SetVisualFallbackEnabled(bool enabled) noexcept;
// Native activity target selection resumes when its optional pointing hand is unavailable.
void SetNativeActivityFallback5916c3(bool enabled) noexcept;
// V66.69 F4 immersive interaction: replace eye origin+direction with the calibrated controller/index ray while preserving KCD2 reach/filter/action semantics.
void SetHandRayWorld(bool active, float ox,float oy,float oz, float dx,float dy,float dz, float max_reach_m=0.0f, float acquisition_radius_m=0.0f, int hand_owner=-1) noexcept;
// Inactive legacy HUD laser may release only its own default-owner ray.
// Ordinary Grip/context release continues through SetHandRayWorld(false,...).
void ClearLegacyHandRayWorldH5() noexcept;
// R137 (vrik/view_select_r137.h): the index-finger point ray (finger tip + hand direction, world metres) of the
// pointing hand, published every mod loop; active=false when no hand points. object selection only.
void SetPointRayR137(bool active,float ox,float oy,float oz,float dx,float dy,float dz,int side) noexcept;
bool HandRayActive() noexcept;
// D76R1 read-only name focus: current head-view native bounds, with the
// qualified hand nominee as fallback. Never grants Use or changes selection.
struct AlchemyHandNameFocusD72 {
    std::uintptr_t instance=0;
    std::uint64_t lifetime=0,sourceEpoch=0,modeEpoch=0;
    std::uint32_t player=0,entity=0;
    int type=-1,side=-1;
    bool operator==(const AlchemyHandNameFocusD72&) const = default;
};
bool AlchemyHandNameWantedD72() noexcept;
bool ReadAlchemyHandNameFocusD72(std::uint64_t now,AlchemyHandNameFocusD72& out) noexcept;
// Current physical Grip lock only; no candidate or engine lookup.
int LockedPalmOwner19c4(std::uint64_t now) noexcept;
void SetPalmRays59c9(const palm59c9::Pair& pair) noexcept;
// V66.178R14: the last interactive candidate the game tested that lies inside the hand ray's cone
// (the on-screen check we force). tick = GetTickCount64 of that test; distance along the ray.
struct HandCandidate {std::uint64_t tick{0};float x{0},y{0},z{0},distance{0};int owner{-1};bool object{false};std::uint32_t entity{0};char cls[32]{};std::uint64_t sourceGeneration=0,sourceEpoch=0,sourceTick=0;std::uintptr_t sourcePlayer=0;}; // R59.16C10R10: cls = exact script entity class of the candidate (AnimDoor, PickableItem, Dog, NPC...)
bool LastHandCandidate(HandCandidate& out) noexcept;
// Separate opt-in proximity observations of native Dog actor candidates. This
// leaves ordinary head/object selection unchanged and does not grant Bond.
using DogHandCandidateD59R1=dog_candidate_d59r1::Candidate;
void SetDogGestureObservationD59R1(bool enabled) noexcept;
bool ReadDogHandCandidateD59R1(int side,std::uint64_t now,DogHandCandidateD59R1& out) noexcept;
// Already selected native direct-ray door candidate, qualified to the current
// palm. It bypasses the projection-only hover list; Use acceptance is separate.
bool NativeDirectDoorCandidate54r1(int side,std::uint64_t now,HandCandidate& out) noexcept;
// Native door admission must match its accepted target and originating hand;
// an unrelated nearer hover candidate must not replace that copied witness.
bool ExactDoorCandidate38(std::uint32_t target,int side,std::uint64_t now,HandCandidate& out) noexcept;
std::uint64_t DoorSourceGeneration38() noexcept;
bool DoorSourceCurrent38(std::uint64_t generation) noexcept;
// Door-only raw acquisition identity, independent of the generic Trigger/ray
// heuristic. No target authority: current native eligibility AND a fresh same
// entity physical surface are still required before a Door point/Use/Arm.
std::uint64_t DoorAcquisitionGeneration65() noexcept;
bool DoorAcquisitionCurrent65(std::uint64_t generation) noexcept;
Status GetStatus() noexcept;
} // namespace interaction_alignment
} // namespace kcd2vr
