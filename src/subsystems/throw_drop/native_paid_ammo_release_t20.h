#pragma once
#include "native_transient_ammo_t20.h"
#include "runtime/ammo_eligibility_t20.h"
#include "vrik/weapon_release_t1.h"

namespace kcd2vr::paid_ammo_t20 {
// Disabled source candidate. One existing script/action owner includes the
// runtime after T17/T18, native eligibility and native_transient_ammo_t20.inl.
// No second executor. Payment requires BOTH category hooks and live mask lease;
// retained preview cleanup is independent of those recovery-protection gates.
enum class Status : unsigned char {
    Empty, Refused, PaymentUnknown, PaidNoLaunch, LaunchUnknown, Launched
};
enum class StockRole : unsigned char { LegacyMelee, RecoveredOwnAmmo };
enum class PresentationT26 : unsigned char { NativeAttachment, WorldEntity };
struct Prepared {
    // key is the ORIGINAL stock WUID, entity is the held preview ID,
    // generation is preview.serial. lastAllowed/motionTick are measured entity
    // geometry, not a controller target. The carrier spends its release first.
    weapon_release_t1::Request release{};
    transient_ammo_t20::Receipt* preview=nullptr;
    void* stockDefinition=nullptr; // exact current native definition retained at draw
    StockRole stockRole=StockRole::LegacyMelee;
    int family=-1; // 0 dagger, 1 small axe; exact authored GUID joins below
    PresentationT26 presentation=PresentationT26::NativeAttachment;
    // Exact native attachment/model/hand receipt, current release serial and
    // fresh current actor/input. Must remain true during Releasing, independently
    // of preview.status; this function is validation, never native mutation.
    bool (*currentHand)(const Prepared&) noexcept=nullptr;
    // Optional synchronous immutable-source validator. This supplements the
    // mandatory currentHand check; NULL keeps original source behavior. Never
    // store a stack closure or renew the release's original pose deadline.
    bool (*currentSource)(const native_script_t2::Context&,const Prepared&) noexcept=nullptr;
};
struct Receipt {
    std::uint64_t serial=0,legacyKey=0,launcherKey=0,payloadKey=0;
    supply_t2::Input owner{};int side=-1;
    transient_ammo_t20::Preview held{},flight{};
    void* legacy=nullptr; // identity only; never dereferenced after native cost
    void* stockDefinition=nullptr;
    void* inventory=nullptr;
    void* payload=nullptr;
    void* shootingManager=nullptr;
    void* itemManager=nullptr;
    void* world=nullptr;
    void* ammoDefinition=nullptr;
    std::uintptr_t scriptSystem=0,scriptState=0;
    std::uint64_t vmGeneration=0,load=0;
    int beforeCount=0,family=-1;
    StockRole stockRole=StockRole::LegacyMelee;
    Status status=Status::Empty;
    bool costEntered=false,costReturned=false,costAcknowledged=false;
    bool shootEntered=false,shootReturned=false,transferAcknowledged=false;
    bool factoryReturned=false,factoryTransferAcknowledged=false;
    bool originalScopeLost=false;
    unsigned factoryCalls=0;
    bool cleanupEntered=false,cleanupReturned=false,cleanupAcknowledged=false;
    std::uint32_t cleanupId=0;
};
static_assert(std::is_trivially_copyable_v<Receipt>);
// AmmoPaidReleaseT20(ctx, prepared, velocity, persistentReceipt) -> Status.
// The preview's Ready token is spent BEFORE native debit, even if that debit
// faults. N>=1 native legacy stacks are supported; quantity is exactly one.
// LegacyMelee and RecoveredOwnAmmo are distinct native classes/roles. Recovered
// own Ammo stays finite actor stock and is consumed normally; never converted
// into a melee item, replenished, or permanently escrowed in WorldInventory.
// Normal native debit + fresh exact WUID census authorizes payment; absent is
// only a successful native NULL, never a fault/default read. Payment and shoot
// are NOT atomic: paid-no-launch/unknown never refunds, replays or grants.
// An accepted Receipt retains its exact script/world/preview scope. Replacing
// caller storage during native callbacks cannot reattribute an earlier debit.
// AmmoPaidRetirePreviewT20(ctx, transientReceipt, paidReceipt) observes and
// retires ONLY that held preview. After transfer it requires the exact source
// empty plus occurrence-time full-factory transfer proof, captured BEFORE
// contact can replace/consume the flight payload; it cannot touch that item.
// Transfer proof is independent of Shoot return: a later native fault still
// leaves an uncertain flight obligation and never authorizes another shot.
// Immutable pre-Shoot identity copies prevent two coherently replaced caller
// receipts from reattributing an earlier debit/transfer. A lost original scope
// is sticky and refuses cleanup; the carrier must retain receipt storage until
// all obligations are discharged, rather than reset it during native callbacks.
// Before payment, stow remains the carrier's exact native payload cleanup.
// No save exclusion, durability, recovery/sticking or headset result is claimed.
}
