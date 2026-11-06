#pragma once
#include <cstdint>

// D76: exact-retail additive living-physics displacement and origin receipt.
// No camera, controller, combat, animation or joystick writes occur here.
namespace kcd2vr::native_roomscale_d76 {
struct Vec3 { float x=0, y=0, z=0; };
struct Owner {
    std::uintptr_t player=0, entity=0, character=0;
    std::uint32_t entityId=0;
    std::uint64_t epoch=0; // Genuine tracking-origin generation.
    std::uint64_t lifetime=0; // Genuine player/world lifetime; survives pose-owner switches.
};
struct StepRequest {
    Owner owner{};
    std::uint64_t sample=0, tickMs=0;
    Vec3 worldDelta{};    // Horizontal game-world displacement, rotated once.
    Vec3 trackingDelta{}; // Corresponding raw XR horizontal delta, metres.
};
struct Origin {
    Owner owner{};
    std::uint64_t serial=0; // Even stable; read again after reading native root.
    std::uint64_t occurrence=0, lastSample=0;
    Vec3 consumedTracking{}, consumedWorld{};
    Vec3 adoptedPosition{}; // Evidence only. Never used as an equality join.
};
enum class Surface : unsigned char { Entity, Character };
struct Counters {
    std::uint64_t submitted=0, duplicate=0, stale=0, rejected=0;
    std::uint64_t applied=0, queued=0, entityAdopted=0, characterAdopted=0;
    std::uint64_t faults=0, lostReceipt=0;
};
// Install is inert on any retail fingerprint/pin mismatch. Called by host once.
bool Install() noexcept;
bool Ready() noexcept;
bool Enrolled() noexcept;
// Native view caches may enroll a zero origin only before any physical Step
// has been applied in this genuine origin epoch (including pending adoption).
bool OriginPristine() noexcept;
// Rebase is for the existing genuine recenter/world-origin lifecycle ONLY.
// Returns false while an already applied native occurrence still awaits adoption.
// A feature switch or native animation takeover must never call Rebase.
bool Rebase(const Owner&) noexcept;
// Existing explicit recenter consumer. Unenrolled succeeds; enrolled pending
// physics waits. Admission is paused until the producer observes the new epoch.
bool TryRecenterOrigin(std::uint64_t epoch) noexcept;
// Admission OFF retires only unsubmitted physical intent; adopted origin persists.
void SetAdmission(bool enabled) noexcept;
void CancelPending() noexcept;
// Distinct fresh samples coalesce until one actual native Step consumes them.
// The caller must retire a rejected sample; never replay offset debt later.
bool Submit(const StepRequest&) noexcept;
// Read origin BEFORE native root; accept only if SameSerial is true AFTER.
// False while a root write is in progress. No native calls or polling are made.
bool ReadEntityOrigin(std::uintptr_t entity, Origin&) noexcept;
bool ReadCharacterOrigin(std::uintptr_t character, Origin&) noexcept;
// Latest COMPLETED entity adoption, metadata only: valid while a later native
// root write is in progress. This must not be paired with a live native root
// read. Authored-camera fallback uses it only as the current XR origin.
bool ReadAdoptedEntityOrigin(const Owner&, Origin&) noexcept;
bool ReadAdoptedOrigin(Origin&) noexcept;
// Session tracking-coordinate debt, not a native pose/root receipt. Carries
// the last COMPLETED C through actor unavailability, writerodd, OFF and faults.
// A genuine coordinated origin epoch change is the only reset to zero.
bool ReadTrackingOriginD76(Origin&) noexcept;
bool SameSerial(Surface, std::uintptr_t identity, std::uint64_t serial) noexcept;
// Identity-only validity for an already captured native camera packet.
// A concurrent newer entity/character root write does not invalidate that packet.
bool CurrentOwner(const Owner&) noexcept;
Counters ReadCounters() noexcept;
}
