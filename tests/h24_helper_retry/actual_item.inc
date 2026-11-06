struct PhysEquipmentItem6698 {
    ph::Profile profile{};void* item=nullptr;void* entity=nullptr;float bounds[6]{};
    void* worldLink54=nullptr; // observation only; a stored native item need not have a world model
    int set=-1,slot126=-1;bool boundsValid=false,held=false,dropUnitT1=false;
    std::uint64_t guardFaceRetryRequestH24=0;unsigned guardFaceRetryCountH24=0;bool guardFaceWasHeldH24=false;
    bool slotValid34[2]{};std::array<ph::Pose,2> slot34{};std::uint64_t slotRetry34=0; // C34b: the weapon model's own hand helpers slt_0 / slt_1 (model-local metres)
};
