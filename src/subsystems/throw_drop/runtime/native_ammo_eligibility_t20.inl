// Include in action_time_owner59c11 before native_ammo_launch_t17.inl. Include
// ammo_eligibility_t20.h outside that namespace with the other public headers.
// SOURCE CANDIDATE: the host must call the qualified readiness observer before
// admitting any conversion. This file does not grant items or repair a nock.
namespace ae20=kcd2vr::ammo_eligibility_t20;
using AmmoCategoryFnT20=int(__fastcall*)(void*);
static AmmoCategoryFnT20 g_ammo_category_original_t20=nullptr,g_ammo_raw_original_t20=nullptr;
static std::atomic<bool> g_ammo_masks_ready_t20{false},g_ammo_eligibility_fault_t20{false};
static std::atomic<std::uint64_t> g_ammo_masks_episode_t20{0};
static ae20::MaskReceipt g_ammo_masks_t20{}; // qualified native owner only
// K39: identify the result already obtained by the bounded census. This never
// rescans the registry, follows retained row pointers or grants readiness.
static void AmmoMaskEvidenceK39(char* out,std::size_t size) noexcept {
    static constexpr const char* names[]{"UNKNOWN","READY","UNREADABLE","EMPTY","TOO_LARGE","CHANGED","UNKNOWN_MISSILE_CLASS","BIT_ZERO"};
    const auto status=unsigned(g_ammo_masks_t20.status);
    std::snprintf(out,size,"%s/count=%llu/missiles=%u/lease=%llu/current=%llu/ready=%d/fault=%d",
        status<std::size(names)?names[status]:"INVALID_STATUS",(unsigned long long)g_ammo_masks_t20.count,
        g_ammo_masks_t20.missileItems,(unsigned long long)g_ammo_masks_episode_t20.load(),
        (unsigned long long)inventory_lifecycle_t8::loadEpisode18.load(),int(g_ammo_masks_ready_t20.load()),int(g_ammo_eligibility_fault_t20.load()));
}

static bool AmmoReadT20(std::uintptr_t address,void* out,std::size_t size) noexcept {
    if(!address||!out||!size)return false;
#ifdef KCD2VR_AMMO_ELIGIBILITY_T20_HOST_TEST
    return AmmoHostReadT20(address,out,size);
#else
    // No VirtualQuery, allocation, registry walk or native/VM call in a getter.
    __try {std::memcpy(out,reinterpret_cast<const void*>(address),size);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
#endif
}
static ae20::Family AmmoDefinitionT20(void* row) noexcept {
    const auto base=g_base.load(std::memory_order_acquire);
    const auto address=reinterpret_cast<std::uintptr_t>(row);
    std::uintptr_t vft=0,after=0;ae20::Guid guid{};int rawType=0;
    if(!base||!AmmoReadT20(address,&vft,sizeof(vft))||vft!=base+ae20::ammoVft||
       !AmmoReadT20(address+8,guid.data(),guid.size())||
       !AmmoReadT20(address+0xF0,&rawType,sizeof(rawType))||rawType!=1||
       !AmmoReadT20(address,&after,sizeof(after))||after!=vft)return ae20::Family::None;
    return ae20::family(guid);
}
// Existing factory admission can call this without a world/hand registry. That
// also catches an own payload restored into an ordinary bow before first draw.
// This observer neither creates nor changes an inventory item.
static ae20::Family AmmoPayloadDefinitionT20(void* item) noexcept {
    const auto base=g_base.load(std::memory_order_acquire);
    const auto address=reinterpret_cast<std::uintptr_t>(item);
    std::uintptr_t vft=0,definition=0,after=0,afterVft=0;int rawType=0;
    if(!base||!AmmoReadT20(address,&vft,sizeof(vft))||vft!=base+0x3A2DA88||
       !AmmoReadT20(address+0x48,&definition,sizeof(definition))||!definition||
       !AmmoReadT20(definition+0xF0,&rawType,sizeof(rawType))||rawType!=1)return ae20::Family::None;
    const auto family=AmmoDefinitionT20(reinterpret_cast<void*>(definition));
    if(!AmmoReadT20(address+0x48,&after,sizeof(after))||after!=definition||
       !AmmoReadT20(address,&afterVft,sizeof(afterVft))||afterVft!=vft)return ae20::Family::None;
    return family;
}
static bool AmmoOwnPayloadT20(void* item) noexcept {
    return AmmoPayloadDefinitionT20(item)!=ae20::Family::None;
}
static bool AmmoMaskLeaseT20() noexcept {
    return g_ammo_masks_ready_t20.load(std::memory_order_acquire)&&
        !g_ammo_eligibility_fault_t20.load(std::memory_order_acquire)&&
        g_ammo_masks_episode_t20.load(std::memory_order_acquire)==
            inventory_lifecycle_t8::loadEpisode18.load(std::memory_order_acquire);
}
static void InvalidateAmmoEligibilityT20() noexcept {
    g_ammo_masks_ready_t20.store(false,std::memory_order_release);
}
static int AmmoCategoryResultT20(void* row,int native,std::uintptr_t caller,ae20::Getter getter) noexcept {
    const auto base=g_base.load(std::memory_order_acquire);
    // Unknown, impact and firing consumers always retain the exact original
    // return. C5DD14 keeps its own displaced-item write/return semantics.
    if(native!=1||!base||caller<base||!AmmoMaskLeaseT20()||
       !ae20::selectionCaller(getter,caller-base)||AmmoDefinitionT20(row)==ae20::Family::None)return native;
    return AmmoMaskLeaseT20()?0:native;
}
static int __fastcall AmmoCategoryDetourT20(void* row) noexcept {
    const auto caller=KCD2VR_GRASP_CALLER();
    const int native=g_ammo_category_original_t20?g_ammo_category_original_t20(row):0;
    return AmmoCategoryResultT20(row,native,caller,ae20::Getter::Normalized);
}
static int __fastcall AmmoRawCategoryDetourT20(void* row) noexcept {
    const auto caller=KCD2VR_GRASP_CALLER();
    const int native=g_ammo_raw_original_t20?g_ammo_raw_original_t20(row):0;
    return AmmoCategoryResultT20(row,native,caller,ae20::Getter::Raw);
}

// A failed scan is terminal for this load episode until the host explicitly
// retries at a fresh readiness edge. Never repeat a full-table scan per frame.
// K32: *transient tells the caller whether a false was only 'not yet' (retry allowed, bounded) or final.
static bool ObserveAmmoMasksT20(const native_script_t2::Context& ctx,bool* transient=nullptr,bool* ownerFaultK39=nullptr) noexcept {
    if(transient)*transient=true;
    if(ownerFaultK39)*ownerFaultK39=false;
    // This read-only registry census is already serviced by the original-first
    // native script callback. Mannequin may have a different legitimate owner
    // thread; its identity is not a capability for this script-owned scan.
    if(!ctx.Current())return false;
    g_ammo_masks_ready_t20.store(false,std::memory_order_release);
    const auto load=inventory_lifecycle_t8::state.load(std::memory_order_acquire);
    const bool nativeFaultK39=g_native_fault13.load(std::memory_order_acquire);
    if(nativeFaultK39||g_ammo_eligibility_fault_t20.load(std::memory_order_acquire)||
       !retail_build112::Ready(WhBase())){
        if(ownerFaultK39)*ownerFaultK39=nativeFaultK39;
        if(transient)*transient=false;return false;
    }
    if(!g_native_ready13.load(std::memory_order_acquire)||(load&1))return false; // K32: not yet
    const auto episode=inventory_lifecycle_t8::loadEpisode18.load(std::memory_order_acquire);
    auto receipt=ae20::inspectMasks(WhBase(),&AmmoReadT20);
    if(!ctx.Current()||episode!=inventory_lifecycle_t8::loadEpisode18.load(std::memory_order_acquire)||
       load!=inventory_lifecycle_t8::state.load(std::memory_order_acquire))receipt.status=ae20::MaskStatus::Changed;
    g_ammo_masks_t20=receipt;
    if(receipt.status!=ae20::MaskStatus::Ready){
        // K32: changed / unreadable / empty registry while loading = not yet; schema shape failures are final.
        if(transient)*transient=receipt.status==ae20::MaskStatus::Changed||receipt.status==ae20::MaskStatus::Unreadable||
            receipt.status==ae20::MaskStatus::Empty||receipt.status==ae20::MaskStatus::Unknown;
        return false;
    }
    g_ammo_masks_episode_t20.store(episode,std::memory_order_release);
    g_ammo_masks_ready_t20.store(true,std::memory_order_release);
    return true;
}

#ifndef KCD2VR_AMMO_ELIGIBILITY_T20_HOST_TEST
static std::optional<DetourModKit::hook::Hook> g_ammo_category_hook_t20,g_ammo_raw_hook_t20;
static bool AmmoEligibilityPinsT20() noexcept {
    const auto b=WhBase();if(!retail_build112::Ready(b))return false;
    constexpr unsigned char normal[]{0x48,0x83,0xEC,0x28,0x48,0x8B,0x01,0xFF,0x50,0x50,0x33,0xC9,0x83,0xF8,0x05,0x0F,0x4D,0xC1,0x48,0x83,0xC4,0x28,0xC3};
    constexpr unsigned char raw[]{0x8B,0x81,0xF0,0,0,0,0xC3,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC};
    constexpr unsigned char cast[]{0x48,0x8B,0xC1,0xC3};
    constexpr unsigned char registry[]{0x48,0x8B,0x83,0x80,0,0,0,0x48,0x8B,0x53,0x78};
    constexpr unsigned char registryLiteral[]{0x48,0x8D,0x0D,0x85,0x95,0x86,0x02};
    constexpr unsigned char stride[]{0x48,0x8D,0x04,0x40,0x48,0x8D,0x0C,0xC2};
    constexpr unsigned char entry[]{0x48,0x8B,0x43,0x10};
    constexpr unsigned char metadata[]{0x48,0x83,0xEC,0x28,0x48,0x8B,0x81,0xE8,0,0,0};
    constexpr unsigned char namedGuid[]{0x48,0x8D,0x50,0x08,0xE8,0x56,0xC3,0xC4,0xFD};
    constexpr unsigned char itemDefinition[]{0x48,0x83,0x7F,0x48,0,0x48,0x0F,0x45,0x5F,0x48};
    constexpr unsigned char anyMask[]{0x4C,0x8B,0x41,0x08,0x49,0x8B,0,0x49,0x3B,0xC0,0x74,0x20,0x4C,0x8B,0x50,0x10,0x41,0xB9,1,0,0,0,0x8B,0x0A,0x41,0xD2,0xE1,0x45,0x84,0x8A,0x98,0,0,0,0x75,5,0x48,0x8B,0,0xEB,0xDE,0xB0,1,0xC3,0x32,0xC0,0xC3};
    if(!MatchBytes(b+0xC5DCFC,normal,sizeof(normal))||!MatchBytes(b+0x1A88CF0,raw,sizeof(raw))||
       !MatchBytes(b+0x5F5DA0,cast,sizeof(cast))||!MatchBytes(b+0x468373,registry,sizeof(registry))||
       !MatchBytes(b+0x2ABC294,registryLiteral,sizeof(registryLiteral))||!MatchBytes(b+0x468387,stride,sizeof(stride))||
       !MatchBytes(b+0x468409,entry,sizeof(entry))||!MatchBytes(b+0x476358,metadata,sizeof(metadata))||
       !MatchBytes(b+0x2ABC13D,namedGuid,sizeof(namedGuid))||!MatchBytes(b+0x4695EB,itemDefinition,sizeof(itemDefinition))||
       !MatchBytes(b+0x1EDC7B0,anyMask,sizeof(anyMask)))return false;
    for(const auto& pin:ae20::consumerPins)if(!MatchBytes(b+pin.rva,pin.bytes.data(),pin.size))return false;
    return Rd<std::uintptr_t>(reinterpret_cast<void*>(b+ae20::ammoVft),0x50,0)==b+0x1A88CF0&&
        Rd<std::uintptr_t>(reinterpret_cast<void*>(b+ae20::ammoVft),0xF0,0)==b+0x5F5DA0&&
        Rd<std::uintptr_t>(reinterpret_cast<void*>(b+ae20::missileItemVft),0x108,0)==b+0x5F5DA0;
}
static bool InstallAmmoEligibilityT20() noexcept {
    // Installation alone does NOT arm filtering or make a carrier ready.
    if(g_ammo_category_hook_t20||g_ammo_raw_hook_t20)return
        g_ammo_category_hook_t20&&g_ammo_raw_hook_t20&&g_ammo_category_original_t20&&g_ammo_raw_original_t20&&
        g_ammo_category_hook_t20->is_enabled()&&g_ammo_raw_hook_t20->is_enabled();
    if(!AmmoEligibilityPinsT20())return false;
    const auto b=WhBase();
    DetourModKit::hook::InlineRequest normal{"kcd2vr_t20_ammo_eligibility_normal",DetourModKit::hook::Target{DetourModKit::Address{b+0xC5DCFC}},DetourModKit::hook::Options{}};
    DetourModKit::hook::InlineRequest raw{"kcd2vr_t20_ammo_eligibility_raw",DetourModKit::hook::Target{DetourModKit::Address{b+0x1A88CF0}},DetourModKit::hook::Options{}};
    auto normalHook=DetourModKit::hook::inline_at(std::move(normal),&AmmoCategoryDetourT20);
    auto rawHook=DetourModKit::hook::inline_at(std::move(raw),&AmmoRawCategoryDetourT20);
    if(!normalHook||!rawHook)return false;
    g_ammo_category_hook_t20=std::move(*normalHook);g_ammo_raw_hook_t20=std::move(*rawHook);
    g_ammo_category_original_t20=g_ammo_category_hook_t20->original<AmmoCategoryFnT20>();
    g_ammo_raw_original_t20=g_ammo_raw_hook_t20->original<AmmoCategoryFnT20>();
    if(!g_ammo_category_original_t20||!g_ammo_raw_original_t20||
       !PinOwnCallbackT2(reinterpret_cast<const void*>(&AmmoCategoryDetourT20))||
       !PinOwnCallbackT2(reinterpret_cast<const void*>(&AmmoRawCategoryDetourT20)))return false;
    if(!g_ammo_category_hook_t20->enable()||!g_ammo_raw_hook_t20->enable()){
        g_ammo_eligibility_fault_t20.store(true,std::memory_order_release);return false;
    }
    return true;
}
#endif
