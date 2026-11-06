// Exact WHGame 6A350E20 / bdf8f9e4...4479f7 only (parent install fingerprint).
// C5235C calls 23CBF70 -> 9155C8, reads service+1A0, then vtable+528.
// RTTI: CGameRules primary vtable 3AF1678, +528 == 725698.
// 725698 appends a COPY of 0xA0 HitInfo to its +1D8 vector; 39DC05C
// copies 0xA0 bytes, C1EB94 handles growth. No retained stack pointer or SDK slot.
constexpr std::uintptr_t kGameGlobal5910c1=0x549D388,kGameRulesVft5910c1=0x3AF1678,kQueueHit5910c1=0x725698;
constexpr unsigned char kQueueHitEntry5910c1[]{0x48,0x83,0xEC,0x28,0x4C,0x8D,0x89,0xD8,0x01,0x00,0x00,0x49,0x8B,0x49,0x08,0x49,0x3B,0x49,0x10,0x74,0x12};
constexpr unsigned char kHitGlobalRead5910c1[]{0x48,0x8B,0x05,0x90,0x7D,0xB8,0x04,0x48,0x83,0xC4,0x20,0x5B,0xC3};
constexpr unsigned char kNativeHitEventCall5910c1[]{0xE8,0xAF,0x98,0x77,0x01,0x48,0x8D,0x55,0xA0,0x48,0x8B,0x88,0xA0,0x01,0x00,0x00,0x48,0x8B,0x01,0xFF,0x90,0x28,0x05,0x00,0x00};
static const void* HitEventReceiver5910c1() noexcept {
    if(!g_base||!retail_build112::Ready(g_base))return nullptr;
    static int verified=-1;
    if(verified<0)verified=(g_base&&EntryIs(kQueueHit5910c1,kQueueHitEntry5910c1,sizeof(kQueueHitEntry5910c1))&&
        EntryIs(0x9155F1,kHitGlobalRead5910c1,sizeof(kHitGlobalRead5910c1))&&
        EntryIs(0xC526BC,kNativeHitEventCall5910c1,sizeof(kNativeHitEventCall5910c1)))?1:0;
    if(verified!=1)return nullptr;
    // Read the initialized singleton; do not call its lazy constructor here.
    const auto* service=Rd<const void*>(reinterpret_cast<const void*>(g_base+kGameGlobal5910c1),0,nullptr);
    const auto* rules=Rd<const void*>(service,0x1A0,nullptr);
    if(!rules||Rd<std::uintptr_t>(rules,0,0)!=g_base+kGameRulesVft5910c1||
       Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+kGameRulesVft5910c1),0x528,0)!=g_base+kQueueHit5910c1)return nullptr;
    std::uintptr_t vector[3]{};
    if(!SehCopy146(static_cast<const unsigned char*>(rules)+0x1D8,vector,sizeof(vector)))return nullptr;
    const auto begin=vector[0],end=vector[1],cap=vector[2];
    if(end<begin||cap<end||(end-begin)%0xA0||(cap-begin)%0xA0||cap-begin>0xA00000)return nullptr;
    return rules;
}
static bool QueueHitEvent5910c1(const void* rules,const void* hit) noexcept {
    if(!rules||!hit||HitEventReceiver5910c1()!=rules)return false;
    using Fn=void(__fastcall*)(const void*,const void*);
    __try{reinterpret_cast<Fn>(g_base+kQueueHit5910c1)(rules,hit);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
