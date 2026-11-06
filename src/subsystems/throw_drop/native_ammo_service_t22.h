#pragma once
#include "native_private_launcher_t21.h"
#include "native_paid_ammo_release_t20.h"
#include "native_preview_retirement_t22.h"
#include "ammo_carry_t18.h"
#include "pending_reach_t23.h"
#include "ammo_readiness_k32.h"
#include "../../vrik/visible_failure.h"
namespace kcd2vr::ammo_service_t22 {
// H41 reviewed integration: LEFT world-held chest knives only. The service
// excludes legacy native-attachment draws; existing script/input owner retained.
inline constexpr bool integrated=true;
inline constexpr bool leftChestKnivesT26=true;
inline constexpr unsigned capacity=8;
enum class Stage:unsigned char {Empty,WaitingAdmission,Reserved,Preview,ModelPending,Held,Retiring,Unknown,Quarantined};
enum class Reason:unsigned char {None,Unavailable,Stock,Context,Model,Attachment,Motion,Stow,Release,Cleanup,WorldChanged,Capacity};
struct Status {std::uint64_t serial=0;int side=-1,family=-1;Stage stage=Stage::Empty;Reason reason=Reason::None;};
// Private lexical service after T18/T20/T21 contributions:
//   AmmoServiceInstallT22() -- once, before installing existing script client
//   AmmoServiceReadinessWantedT22()/ReadinessT22(ctx) -- existing callback only
//   AmmoServiceScriptT22(ctx,currentCompleteInput) -- original script callback
//   AmmoServiceOwnsT22() -- blocks old T18 escrow/carrier while enabled/retained
//   AmmoServiceStatusT22(side,out) -- POD only, no native reads
// Every native receipt has stable fixed storage throughout synchronous calls.
// Entered Attach pins its exact original model as a separate cleanup debt:
// entity retirement alone never releases that hand. Count1/ID/binding qualify
// held ACK; known pending binding0 may wait but cannot place or pay.
// Existing queued same-hand punch debt must admit the new gesture first.
}
