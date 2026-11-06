// DS-F46: native leaf implementation v2 (SEH-guarded, exact-pinned, inert
// until the type-6 remove path is proven and the executor registers it).
namespace kcd2vr::physics::constraint_leaf60 {
static bool pinAt(std::uintptr_t at,const unsigned char* bytes,std::size_t n) noexcept {
    __try{return std::memcmp(reinterpret_cast<const void*>(at),bytes,n)==0;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool GetterPinned(std::uintptr_t base) noexcept {
    return base&&retail_build112::Ready(base)&&
        pinAt(base+kGetEntityPhysics,kGetEntityPhysicsPrologue,sizeof(kGetEntityPhysicsPrologue));
}
bool ResolveBody(std::uintptr_t base,void* entity,void*& body) noexcept {
    body=nullptr;
    if(!base||!entity||!GetterPinned(base))return false;
    __try{
        using Fn=void*(__fastcall*)(void*);
        body=reinterpret_cast<Fn>(base+kGetEntityPhysics)(entity);
        if(!body)return false;
        const auto vft=*reinterpret_cast<const std::uintptr_t*>(body);
        // DS-F50: this leaf accepts the RIGID table ONLY - the articulated
        // table's Action slot is a different entry (A99DF0, corpse-push path).
        if(vft!=base+kRigidVft){body=nullptr;return false;}
        return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){body=nullptr;return false;}
}
Result ActionAddConstraint(std::uintptr_t base,void* body,const kcd2vr::constraint_packet60::Packet& packet) noexcept {
    Result out{};
    if(!base||!body||!retail_build112::Ready(base))return out;
    __try{
        const auto vft=*reinterpret_cast<const std::uintptr_t*>(body);
        if(vft!=base+kRigidVft)return out; // Refused
        const auto slot=*reinterpret_cast<const std::uintptr_t*>(vft+kActionSlot);
        // DS-F46: the slot pointer must BE the retail Action entry; the
        // prologue check is belt-and-braces (the same bytes occur elsewhere).
        if(slot!=base+kActionRva||!pinAt(slot,kActionPrologue,sizeof(kActionPrologue)))return out; // Refused
        using ActionFn=int(__fastcall*)(void*,const void*,int);
        const int native=reinterpret_cast<ActionFn>(slot)(body,&packet,0); // game thread, OUTSIDE the step
        out.nativeReturn=native;
        // DS-F50: retail tests the return with setg - positive = handle
        // (submitted/queued), <=0 = native refusal. No invented success.
        if(native>0){out.kind=Result::Kind::Submitted;out.returnedId=std::uint32_t(native);}
        return out;
    }__except(EXCEPTION_EXECUTE_HANDLER){out.kind=Result::Kind::Exception;out.returnedId=0;return out;}
}
Result ActionRemoveConstraint(std::uintptr_t base,void* body,const kcd2vr::constraint_packet60::RemovePacket& packet) noexcept {
    Result out{};
    if(!base||!body||!retail_build112::Ready(base))return out;
    __try{
        const auto vft=*reinterpret_cast<const std::uintptr_t*>(body);
        if(vft!=base+kRigidVft)return out;
        const auto slot=*reinterpret_cast<const std::uintptr_t*>(vft+kActionSlot);
        if(slot!=base+kActionRva||!pinAt(slot,kActionPrologue,sizeof(kActionPrologue)))return out;
        using ActionFn=int(__fastcall*)(void*,const void*,int);
        const int native=reinterpret_cast<ActionFn>(slot)(body,&packet,0);
        out.nativeReturn=native;
        // DS-F56 (Codex F48): the type-6 result is a STATUS, not an id, but a
        // native refusal must still be classified: positive = submitted (the
        // ACK stays the typed T4 Absent for the ORIGINAL id), <=0 = Refused.
        if(native>0)out.kind=Result::Kind::Submitted;
        out.returnedId=0; // never the requested id
        return out;
    }__except(EXCEPTION_EXECUTE_HANDLER){out.kind=Result::Kind::Exception;out.returnedId=0;return out;}
}
}
