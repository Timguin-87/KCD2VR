// Generated verbatim from production; do not edit.
struct NativeHeldSourceT4 {
    heldT4::Input input{};
    std::uintptr_t item=0,link=0;
    std::uint64_t tick=0,leftNativeMembershipH36=0;
    unsigned requested=0,applied=0;
    float attachmentScale=1,authoredObjectScaleH5[3]{1,1,1};
    unsigned nativeSlot=6;
    bool valid=false;
};
struct NativeHeldProfile62 {unsigned requested=~0u,applied=~0u;unsigned char dirty=255,readMask=0;};
struct NativeHeldProfileReceipt62 {
    std::uint64_t key=0,next=0;NativeHeldProfile62 prior{};unsigned remaining=32;
    bool take(std::uint64_t k,const NativeHeldProfile62& p,std::uint64_t now) noexcept {
        if(!remaining||now<next||(key==k&&prior.requested==p.requested&&prior.applied==p.applied&&prior.dirty==p.dirty&&prior.readMask==p.readMask))return false;
        key=k;prior=p;next=now+1000;--remaining;return true;
    }
};
