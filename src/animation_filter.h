#pragma once
#include <cstdint>
#include <array>
#include <atomic>

namespace kcd2vr::animation_filter {

// Refreshed only after a calibrated Henry pose commits. Mode/input loss revokes it.
inline std::atomic<std::uint64_t> combatPoseTick112{0};
inline std::atomic<bool> nativeBodyOwned112{false};
// Set only by the same exact-Henry activated FIFO inspection as body ownership.
// No animation-start timer or diagnostic lease can keep a kick alive.
inline std::atomic<bool> nativeKickActive175{false};
inline bool NativeKickActive175() noexcept {return nativeKickActive175.load(std::memory_order_acquire);}
// Protected native FIFO entries (or unavailable evidence) retain the body in F3=2.
// Optional exact seated presentation witness; native mount/dismount stays native.
// Lifetime is captured with classification and revalidated before pose publication.
bool NativeBodyRequired112(void* skeleton_anim,bool* ridingGaitOnlyD76=nullptr,std::uint64_t* ridingLifetimeR95=nullptr) noexcept;
bool PlayerNativeBodyRequired112() noexcept;
// D83 finisher: 0 none, 1 Henry's own finisher (_master), 2 NPC finisher on Henry (_slave). Valid only right
// after NativeBodyRequired112 returned true on the same thread. alias = first finisher clip name.
unsigned FinisherUpperSideD83(const char** alias=nullptr) noexcept;
bool PlayerNativeBodyRequiredForMotion122() noexcept;
// Read-only exclusion query for the exact current Reloading child CAF. This
// never relaxes nativeBodyOwned112 or any general animation/motion owner.
bool ReloadPassengerOnlyXR34(void* skeleton_anim,std::uintptr_t entry,int animation,bool inertTransitionD87=false) noexcept;
// Exact admitted semantic pair presentation uses the same protected-FIFO exclusion.
bool AdmittedNativeClipOnlyD72(void* skeleton_anim,std::uintptr_t entry,int animation) noexcept;

enum class SuppressionMode : int {
    Off = 0,
    FistsIdle = 1,
    UnarmedCombat = 2,
    SwordCombat = 3,
    AllSelectedCombat = 4,
};

// Native curve access is read-only and does not start/seek an animation.
// 0 sampled, 1 no clip, 2 not CAF, 3 controller data unavailable, 4 joint not animated,
// -1 invalid binding/read/fault. A missing joint in an additive preserves the base.
struct CurveReferenceInfo {
    int id=-1; std::uint32_t flags=0; std::uint64_t alias=0;
};
bool ResolveCurveReference(void* skeleton_anim,const char* alias,CurveReferenceInfo& out) noexcept;
int SampleCurveReference(void* skeleton_anim,int id,std::uint32_t controller_crc,float phase,float* xyzw_xyz) noexcept;

struct PoseReferenceState {
    bool closed[2]{false,false}; // 0 right / 1 left, from ADB scope, NOT filename spelling
    bool relaxed[2]{false,false};
    std::uint64_t serial[2]{};
};
// Called only from the validated native Henry pose callback, before any VR writes.
// Reads the real ID FIFO including starts bypassing the name wrapper. Sole activated
// reference entries qualify; joint stability/completeness is checked by the pose core.
// Does not claim Mannequin scope weights or invoke animation playback.
bool ReadPoseReferenceState(void* skeleton_anim, PoseReferenceState& out) noexcept;
void SetInteractionAnimationTarget(void* target_anim, std::uint32_t target_id, std::uint64_t action_sequence) noexcept;

bool Install() noexcept;
void Uninstall() noexcept;
void SetMode(SuppressionMode mode) noexcept;
SuppressionMode Mode() noexcept;
const char* ModeName(SuppressionMode mode) noexcept;

// Called only from the already-correlated Henry skinning path. The descriptor's
// +0x10 member is CCharInstance+8 in the exact retail WHGame build.
void ObservePlayerSkinning(void* pose, void* skeleton_owner, void* char_instance_plus_8) noexcept;

bool Ready() noexcept;
bool PlayerAnimationObjectValidated() noexcept;
void* PlayerSkeletonAnim() noexcept; // R59.16C10R11: validated Henry CSkeletonAnim or nullptr
std::uint64_t SuppressedCount() noexcept;

// Compatibility API. A name-only animation observation never changes VRIK ownership.
// Native handoff requires an independently validated action lifecycle (not a timer).
bool NativeInteractionHandOffActive() noexcept;

// Retired compatibility calls return -1 / UINT64_MAX. Production reference capture
// uses ReadPoseReferenceState plus a complete, stable parent-local chain instead.

// V65.02 diagnostic-only Henry animation-start observer. Level 0 is silent;
// levels 1..5 are driven by the F9 ownership ladder. This never suppresses or
// starts animations and does not claim to be a weighted active-layer stack.
// Worker-only demand: F12=7 needs aliases even when F3/F5 diagnostics are off.
void ServiceAliasCatalog(bool motionRequired=false) noexcept;
void SetStartDiagnosticsLevel(int level) noexcept;

// V65.03 F5: runtime equivalent of the supplied FirstPersonDialogsAndMercy
// current-retail ADB blanking experiment. Fully gated and OFF by default.
void SetDialogueCameraSuppression(bool enabled) noexcept;

// V66.69 F4 physical-map proof. Queue a native standing-reading animation
// request, but consume it only inside Henry's already-running StartAnimation
// callback so the worker thread never calls gameplay animation systems directly.
// phase: 0=none, 1=ReadingBookIn, 2=ReadingBook loop, 3=ReadingBookOut.
void RequestPhysicalMapReadingAnimation(int phase) noexcept;
int PendingPhysicalMapReadingAnimation() noexcept;
std::uint64_t PhysicalMapReadingAnimationStarts() noexcept;

} // namespace kcd2vr::animation_filter

namespace kcd2vr::animation_filter {
struct PlaybackStamp {std::uint64_t serial=0,tick=0;};
// Only called on the matching, validated character's native skinning callback.
void ObserveDiagnosticSkinning(void* skeleton_anim,bool is_henry) noexcept;
void RegisterContactAnimation(void* skeleton_anim,std::uint32_t entity_id,std::uint64_t contact_sequence) noexcept;
void MarkContactForPlayback(std::uint64_t sequence) noexcept;
PlaybackStamp DiagnosticPlaybackStamp() noexcept;
// Copied existing Henry FIFO/name cache only: no new native read or weight claim.
// The cache belongs to the current finite F3 session; absence is UNKNOWN.
struct KickPlaybackOutput23 {
    std::uint64_t tick=0,serial=0; int id=-1,layer=-1,fifo=-1,segment=-1;
    float segmentTime=0; char name[160]{};
};
bool ReadKickPlaybackOutput23(void* skeleton_anim,KickPlaybackOutput23& out) noexcept;
struct HandPlaybackOutput28 {
    std::uint64_t tick=0,serial=0,signature=0;unsigned count=0;bool truncated=false;
    std::array<KickPlaybackOutput23,12> clips{};
};
// Existing sampled FIFO/name cache, independently stamped; not exact blend weights.
bool ReadHandPlaybackOutput28(void* skeleton_anim,HandPlaybackOutput28& out) noexcept;
// V66.136R3: read-only reload observation window driven by Henry's native playback FIFO.
bool ReloadDiagnosticWindow136R3(std::uint64_t now,int& weapon_code) noexcept;
const char* ReloadDiagnosticWeaponName136R3(int weapon_code) noexcept;
}
