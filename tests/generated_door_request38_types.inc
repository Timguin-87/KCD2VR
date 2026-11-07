// Generated; production POD types only.
struct InteractionHandContext158 {int side=-1;bool passive=false,valid=false;float x=0,y=0,z=0,maxReach=0;std::uint64_t tick=0;};
struct DoorRequestWitness38 {
    interaction_alignment::HandCandidate candidate{};
    std::uint64_t epoch=0;
    std::uintptr_t player=0;
    kcd2vr::door_use46::Intent input46{};
    bool inputPresent46=false;
    kcd2vr::door_use46::AcceptedUse62 acceptedUse62{};
    kcd2vr::door_hold65::Hand hand65{};bool handSource65=false;
    // Diagnostic snapshots only, copied at the same native entry. No native
    // read/call and no permission/selection fallback is added by these fields.
    kcd2vr::door_use46::Input receiptInput63{};
    kcd2vr::door_use46::PhysicalUse62 receiptPhysical63{};
    interaction_alignment::HandCandidate receiptCandidate63{};
    bool receipt63=false,receiptHeld63=false,receiptPhysicalCurrent63=false;
    std::uint64_t diagnosticActionSeqH4=0; // copied call ID only; never grants ownership
};
