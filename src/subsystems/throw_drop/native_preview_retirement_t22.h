#pragma once
#include "native_transient_ammo_t20.h"

namespace kcd2vr::preview_retirement_t22 {
// Retained cleanup obligation only. It cannot authorize payment, creation,
// equipment selection or another launch. The original VM/load values stay
// unchanged even when a new current VM capability observes the same world.
struct ModelObligation {
    void* model=nullptr; // exact original native model captured BEFORE Attach
    bool required=false; // an entered/faulted Attach retains this obligation
};
struct Receipt {
    transient_ammo_t20::Receipt identity{};
    ModelObligation model{};
    bool pinned=false,entered=false,nativeReturned=false,acknowledged=false,scopeLost=false;
    bool entityAcknowledged=false,modelEntered=false,modelNativeReturned=false,modelAcknowledged=false;
    unsigned nativeId=0;
};
static_assert(std::is_trivially_copyable_v<Receipt>);
// Private existing script-owner scope, after T20 paid adapter:
// AmmoRetirePreviewT22(currentContext, originalPreview, optionalOriginalPaid,
//                      persistentRetirement, ModelObligation={}) -> ACK.
// Default-none callers retain an entity-only obligation. Required model cleanup
// additionally observes original count0/ID0/known binding0; entity absence alone
// cannot discharge it. entered/nativeReturned describe the single40B730 call;
// modelEntered is spent before that call OR the separate native model-only leaf.
// A fault consumes either budget. No decrement replay or direct field write.
// Unknown construction/source/record
// obligations remain separate and cannot be erased by this result.
}
