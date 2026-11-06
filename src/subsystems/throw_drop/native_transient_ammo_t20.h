#pragma once
#include "native_script_t2.h"
#include "vrik/native_supply_t2.h"
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace kcd2vr::transient_ammo_t20 {
// Candidate only. No registration, hook, setting, caller or shipping enable is
// supplied here. Include the runtime contribution after the T17/T18 helpers in
// the existing action-owner TU; include THIS header at the TU's top level.
enum class Status : unsigned char {
    Empty, Refused, Unknown, NoSourceCreated, SourceRetired, PreviewNeedsCleanup, Ready
};
enum class Lookup : unsigned char { Unknown, Absent, Present };
struct Request {
    // Integration still owns positive finite-stock reservation and exact class
    // retrieval. A key is not a payment/launch grant; this API never spends it.
    supply_t2::Input owner{};
    std::uint64_t serial=0,legacyKey=0,launcherKey=0;
    int side=-1;
    // Actual, current S_AmmoItemClass from native metadata. Its GUID is copied
    // from native +8 into aligned storage, never parsed/reordered as UUID bytes.
    // Resolving the authored class belongs to the integrating native owner.
    void* ammoDefinition=nullptr;
    // Optional fixed owned-code validator for an immutable completed source
    // lease. Called synchronously only; no closure/userdata is retained in the
    // receipt. NULL preserves the original latest-source admission unchanged.
    // The caller must rejoin original request/claim and current native facts;
    // this cannot grant pose freshness, mutate inventory or launch anything.
    bool (*currentOwner)(const native_script_t2::Context&,const Request&) noexcept=nullptr;
};
struct Preview {
    void* arrow=nullptr;
    void* entity=nullptr;
    std::uint32_t entityId=0;
    std::uint64_t payloadKey=0;
};
// Historical read evidence only, never a flag write or current permission.
struct PreviewSaveRead {
    Preview preview{};
    void* payloadOwner=nullptr;
    void* secondaryOwner=nullptr;
    std::uint32_t entityFlags=0;
    bool flagsRead=false,privateWrapperOwned=false,nativeNoSave=false;
};
struct alignas(16) CreationRecord {
    unsigned char bytes[0xF8]{};
};
static_assert(offsetof(CreationRecord,bytes)==0);
static_assert(sizeof(CreationRecord)==0x100); // F8 native bytes + alignment tail
struct Receipt {
    supply_t2::Input owner{}; // retained original identity, never a fresh pose
    int side=-1;
    std::uint64_t serial=0,legacyKey=0,launcherKey=0,load=0;
    std::uint64_t vmGeneration=0;
    std::uintptr_t scriptSystem=0,scriptState=0;
    void* itemManager=nullptr;
    void* world=nullptr;
    void* definition=nullptr;
    void* source=nullptr; // identity only: re-resolve full WUID before use
    std::uint64_t sourceKey=0;
    alignas(16) unsigned char classGuid[16]{};
    CreationRecord record{}; // retained if native construction/cleanup faults
    Preview preview{};
    PreviewSaveRead previewSave{};
    Status status=Status::Empty;
    bool ctorEntered=false,ctorReturned=false,constructed=false;
    bool createEntered=false,createReturned=false,sourceOwned=false;
    bool stringsEntered=false,stringsReturned=false;
    bool treeEntered=false,treeReturned=false,freeEntered=false,freeReturned=false;
    bool recordClean=false;
    bool previewEntered=false,previewReturned=false,cloneAcknowledged=false;
    bool retireEntered=false,retireReturned=false,sourceAbsent=false;
    bool sourceObservationUnknown=false;
    // One source rejoin at the never-entered clone boundary, not a timeout renewal.
    bool previewGrantRejoinAttemptedD66=false,previewGrantRejoinedD66=false;
    const char* previewGrantReasonD66="NOT_ATTEMPTED";
    const char* boundaryD66="UNENTERED"; // retained scalar diagnostic; no permission

};
static_assert(std::is_trivially_copyable_v<Receipt>);
// D61: historical completion evidence only, NEVER current hand/pose authority.
// A completed factory preview can remain PreviewNeedsCleanup when its original
// <=80ms source expires during native work. Its owner may revalidate THIS receipt
// with a new current grant; it may never replay creation, payment or retirement.
inline bool CompletedPreviewD61(const Receipt& r) noexcept {
    const auto& p=r.preview;const auto& s=r.previewSave;
    return r.serial&&r.ctorEntered&&r.ctorReturned&&r.constructed&&r.recordClean&&
        r.createEntered&&r.createReturned&&r.sourceOwned&&r.sourceAbsent&&
        r.source&&r.sourceKey&&r.legacyKey&&r.launcherKey&&r.definition&&
        r.sourceKey!=r.legacyKey&&r.sourceKey!=r.launcherKey&&
        r.previewEntered&&r.previewReturned&&r.cloneAcknowledged&&
        p.arrow&&p.entity&&p.entityId&&p.payloadKey&&p.payloadKey!=r.sourceKey&&
        p.payloadKey!=r.legacyKey&&p.payloadKey!=r.launcherKey&&
        s.preview.arrow==p.arrow&&s.preview.entity==p.entity&&
        s.preview.entityId==p.entityId&&s.preview.payloadKey==p.payloadKey&&
        s.flagsRead&&s.privateWrapperOwned&&s.nativeNoSave&&!s.secondaryOwner;
}

// Runtime seam (private action-owner lexical scope):
//   AmmoTransientPreviewT20(ctx, request, persistentReceipt) -> Status
//   AmmoTransientReconcileT20(ctx, persistentReceipt) -> Status
// Reconcile may retire ONLY an exact owned temporary source if retirement was
// never entered. Once entered it only observes; it never retries a mutation.
// Never discard Unknown, reuse its record, or infer durability from this POD.
// A Ready preview still needs the existing native hand owner's attach/stow/
// cleanup lifecycle. Native entity NO_SAVE is positively READ before Ready and
// again before payment; losing that evidence retains the preview cleanup debt.
// Its actual payload must remain owned only by native Arrow+118/type5 wrapper,
// with no secondary/persistent owner. This is not a new item no-save flag, a
// pre-save lifetime hook, or actual save/reload acceptance.
// This primitive neither pays legacy stock nor launches.
// PreviewNeedsCleanup retains a real native preview after admission changed;
// SourceRetired alone is returned only when preview creation was never entered.
}
